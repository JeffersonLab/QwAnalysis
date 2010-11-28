/* 
 * Here's a script to get pedestals from the "off" channels on my LED runs.
 */
{
  // gROOT->Reset();

  const char* runlist_fname = "LED_runlist.txt";
  const char* pedestal_fname = "LED_pedestals.txt"; 
  const char* signal_fname = "LED_signals.txt"; 
  int nread;
  bool compute_pedestals = 0;

  TCanvas *c = new TCanvas;

  // Read in the runlist
  TTree *t = new TTree; 
  nread = t->ReadFile(runlist_fname);
  if (!nread) {
    cerr << "Read zero entries from file '" << runlist_fname << "'\n";
    return;
  }

  // Declaration of leaf types
  Int_t           runnum;
  Char_t          channel[80];

  // List of branches
  TBranch        *b_runnum, *b_channel;  

  t->SetBranchAddress("runnum", &runnum, &b_runnum);
  t->SetBranchAddress("channel", channel, &b_channel);

  // If the pedestal file exists, then analyze the real data
  TTree *pedestal = new TTree;
  nread = pedestal->ReadFile(pedestal_fname,
			     "runnum/I:adc/C:isValid/I:chi2/D:ndf/I"
			     ":constant/D:mean/D:sigma/D");
  if (compute_pedestals = !nread) { // NB assignment in condition
    cerr << "Read zero entries from file '" << pedestal_fname << "'\n";
    cerr << "Performing pedestal computation\n";
  }

  ofstream pedestal_output, signal_output;

  if (compute_pedestals) {
    // This is where I'll save my pedestal fit parameters ...
    pedestal_output.open(pedestal_fname);
    if (!pedestal_output) {
      perror(Form("couldn't open output file '%s'", pedestal_fname));
      return;
    }
  } else {
    // This is where I'll save my signal fit parameters ...
    signal_output.open(signal_fname);
    if (!signal_output) {
      perror(Form("couldn't open output file '%s'", signal_fname));
      return;
    }
  }

  TFile *f = 0;
  TH1F *fitter = 0;
  TF1 *func =  0;

  int waste=0;
  while ( t->GetEntry(waste++) ) {

    // Since the fitter histogram etc. is created "in" the file that's
    // currently open, it has to be cleaned up before the file is
    // closed.
    if (fitter) fitter->Delete();
    if (func) func->Delete();
    if (f) f->Close(); 

    f = new TFile(Form("Qweak_%d.000.root",runnum));

    if (!f || !f->IsOpen()) {
      cerr << "couldn't open " << f->GetName() << "\n"; 
      continue;
    }
    TPRegexp signal = channel;  

    cout << "Run " << runnum << ": signal in channel " << channel << "\n"; 
    TTree *event_tree = (TTree*)f->Get("event_tree");
    if (!event_tree) {
      cerr << "can't get event_tree from " << f->GetName() << "\n"; 
      continue;
    }
    // There has to be a new fitter histogram etc. since we have changed to a new directory
    fitter = new TH1F("fitter","",4096,0-.5,4096-.5);

    for (int det = 1; det <= 8; ++det) {
      const char *sign[2] = { "m", "p" }; 
      // const char *sign[2] = { "neg", "pos" };
      for (int s = 0; s <= 1; ++s) {
	char *adc = StrDup(Form("md%d%s_adc", det, sign[s]));

	if (compute_pedestals) {
	  if (signal.Match(adc)) continue;

	  cerr << "fitting pedestal for " 
	       << adc << ", run " << runnum << "\n";

	  event_tree->Draw( Form(" %s >> fitter ",	adc),
			    Form(" %s != 0 " ,		adc),
			    "goff"); 
	  fitter->SetTitle(Form("%s, Run %d (pedestal)", adc, runnum));
	  TFitResult r = *fitter->Fit("gaus","S");

	  pedestal_output << runnum << '\t' << adc ;
	  pedestal_output << '\t' << r.IsValid() << '\t' << r.Chi2() << '\t' << r.Ndf() ;
	  for( int i = 0; i < r.NPar(); ++i ) {
	    pedestal_output << '\t' << r.Parameters()[i];
	  }
	  pedestal_output << '\n';
	  pedestal_output.flush();
	} 
	if (!compute_pedestals) {
	  if (!signal.Match(adc)) continue;

	  cerr << "fitting signals for " 
	       << adc << ", run " << runnum << "\n";

	  // Get the pedestal mean and width from the nearest preceding run
	  // based on http://root.cern.ch/root/roottalk/roottalk01/3646.html
	  // since "jfgi" is ROOT's main documentation ...
	  int npeds = 
	    pedestal->Draw( Form(" abs( runnum - %d ) : Entry$ ", runnum),
			    Form(" adc == \"%s\" && isValid ", adc),
			    "goff"); 
	  if (!npeds) {
	    cerr << "No pedestals recorded for " << adc << ", skipping\n";
	    continue;
	  }
	  int *index = new int[npeds];	//////////////// index is just temporary
	  //ascending, stable sort on run number difference
	  TMath::Sort( npeds, pedestal->GetV1(), index, false );  
	  // location in the tree of the winner
	  int nearest_entry = pedestal->GetV2()[ index[0] ];
	  delete[] index;		//////////////// index is just temporary

	  Int_t pedrun; TBranch *b_pedrun;
	  pedestal->SetBranchAddress("runnum", &pedrun, &b_pedrun);
	  Double_t constant,mean,sigma; 
	  TBranch *b_constant,*b_mean,*b_sigma;
	  pedestal->SetBranchAddress("constant", &constant, &b_constant);
	  pedestal->SetBranchAddress("mean", &mean, &b_mean);
	  pedestal->SetBranchAddress("sigma", &sigma, &b_sigma);
	  
	  pedestal->GetEntry(nearest_entry);

	  cout << adc << " pedestal run: " << pedrun << "\n"; 

	  // Fit the distribution to pedestal + poisson
	  event_tree->Draw( Form(" %s >> fitter ",	adc),
			    Form(" %s != 0 " ,		adc),
			    ""); 
	  fitter->SetTitle(Form("%s, Run %d (signal + pedestal)", adc, runnum));

	  func = new TF1("func", 
			 "[0]*TMath::Gaus(x,[1],[2]) + " 
			 "[3]*TMath::Poisson( (x-[1])/[4], [5] )", 0,4096);
          func->SetParameters( fitter->GetBinContent( fitter->FindBin( mean ) ) *sigma/0.4 ,
                               mean, sigma, 
                               fitter->GetMaximum() * 10 ,
                               5, 5 );
	  // we think we've measured the pedestal
	  func->FixParameter( 1, mean );
	  func->FixParameter( 2, sigma );

	  TFitResult r = *fitter->Fit("func","S");

	  // Write the fit results to the file
	  signal_output << runnum << '\t' << adc ;
	  signal_output << '\t' << r.IsValid() << '\t' << r.Chi2() << '\t' << r.Ndf() ;
	  for( int i = 0; i < r.NPar(); ++i ) {
	    signal_output << '\t' << r.Parameters()[i];
	  }
	  signal_output << '\n';
	  signal_output.flush();

	  fitter->SetAxisRange( mean - 5*sigma, 
				mean + 5*r.Parameters()[5]*r.Parameters()[4] );
	  c->Update();
	  c->SaveAs( Form( "%s.%d.png", adc, runnum ) );
	}

	// Clean up
	delete[] adc;
      }
    }
    cout << "\n";

    // break; // for now, process only one entry
  }
  pedestal_output.close();
  signal_output.close();
}
