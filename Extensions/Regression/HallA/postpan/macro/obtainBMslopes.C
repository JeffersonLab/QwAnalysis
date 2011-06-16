///////////////////////////////////////////////////////////////////
//
// obtainBMslopes.C
//   Bryan Moffit - Nov, 2004
//
//  Get the Beam Modulation slopes or Regression coefficients from the
//  postpan analysis ROOT files, and store them to a new ROOTfile
//  <type>_helium.root
//
//  Required file: HeSlugList.txt (can be changed)
//   - must be of this form...
//     <runnumber> <slug>
//  e.g.
//     2129   0
//     2226   1
//     2233   2
//
// Run this macro with the redana libraries loaded...
//  (using libRedAna.so or the redana executable)
//

int obtainBMslopes(TString type="dither") {

  // Change these parameters to suit your specific needs...
  //  TString exp = "helium";
  //  const UInt_t nDet = 2;
  //  TString sDet[nDet] = {"det1", "det3"};
  //  const UInt_t nMon = 5;
  //  TString sMon[nMon] = {"bpm4bx", "bpm4by", 
  //		     "bpm4ax", "bpm4ay",
  //		     "bpm12x"};
  //
  //  TString sluglist_filename = "HeSlugList.txt";

  // Change these parameters to suit your specific needs...
  TString exp = "Hydro_2004";
  const UInt_t nDet = 9;
  TString sDet[nDet] = {"det_all","det1","det2","det3","det4",
			 "det_l","det_r","det_lo","det_hi"};
  const UInt_t nMon = 5;
  TString sMon[nMon] = {"bpm4bx", "bpm4by", 
		     "bpm4ax", "bpm4ay",
		     "bpm12x"};

  TString sluglist_filename = "newHSlugList.txt";

  // Array containing run numbers and slug identifiers
  UInt_t runlist[500];
  UInt_t sluglist[500];
  UInt_t found=0;

  // List containing runs that had beam modulation
  ifstream myfile(sluglist_filename.Data());
  
  // Fill the runlist with the runs containing dithering
  while(1) {
    UInt_t run=0, slug=0;
    myfile >> run >> slug;
    if(run==0) break;
    runlist[found] = run;
    sluglist[found] = slug;
    found++;
    if(!myfile.good())
      break;
  }
  cout << found << " total runs in sluglist (" 
       << sluglist_filename << ")" << endl;
  cout << "largest run number = " << runlist[found-1] << endl;

  // Create the output rootfile and tree.
  TFile slope_summary_file(type+"_"+exp+".root","RECREATE");
  TTree slopes("slopes",type+" slopes for "+exp);
  slopes.SetMarkerStyle(22);


  // Create the tree Branches
  Int_t runnumber;
  Int_t supercycle,minirun;
  Int_t slugnum;
  Int_t firstev, lastev;
  Double_t runmod;  // runmod = run + supercycle/(max supercycles in run)
  Int_t ok_slopes=0;
  Double_t sc_slopes[nDet][nMon]; // Slopes from current supercycle

  cout << "Making branches" << endl;
  slopes.Branch("run",&runnumber,"run/I");
  if(type=="dither") {
    slopes.Branch("supercycle",&supercycle,"supercycle/I");
    slopes.Branch("firstev",&firstev,"firstev/I");
    slopes.Branch("lastev",&lastev,"lastev/I");
  } else if(type=="regress") {
    slopes.Branch("minirun",&minirun,"minirun/I");
  }
  slopes.Branch("runmod",&runmod,"runmod/D");
  slopes.Branch("slug",&slugnum,"slug/I");
  slopes.Branch("ok_slopes",&ok_slopes,"ok_slopes/I");
  for(UInt_t iDet=0; iDet<nDet; iDet++) {
    for(UInt_t iMon=0; iMon<nMon; iMon++) {
      TString bName = sDet[iDet] + "_" + sMon[iMon];
      TString bNameType = bName + "/D";
      slopes.Branch(bName,&sc_slopes[iDet][iMon],bNameType);
    }
  }

  UInt_t Badruns[200]; UInt_t nBadruns=0;
  // Loop over each run... obtain the slopes
  for(UInt_t irun=0; irun<found; irun++) {

    TaFileName::Setup(runlist[irun],type.Data());
    TString file = (TaFileName ("root")).Tstring();
    TFile anaFile(file,"READ");
    if(anaFile.IsZombie() || !anaFile.IsOpen()) {
      // Something wrong with this file... skip it.
      Badruns[nBadruns]=runlist[irun]; nBadruns++;
      anaFile.Close();
      cout << "   ... Skipping run " << runlist[irun] << endl;
    } else {

      Bool_t skip=kFALSE;
      // Update the Monitor and Detector indices
      TaRootDitDef *ditdef; TaRootRegDef *regdef;
      if(type=="dither") {
	ditdef = (TaRootDitDef*)anaFile.Get("TaRootDitDef;1");
	if(ditdef==0) {
	  cout << "run " << runlist[irun] << " has no TaRootDitDef.. "
	       << " SKIPPED." << endl;
	  Badruns[nBadruns]=runlist[irun]; nBadruns++;
	  skip=kTRUE;
	  anaFile.Close();
	}
      }
      if(type=="regress") {
	regdef = (TaRootRegDef*)anaFile.Get("TaRootRegDef;1");
	if(regdef==0) {
	  cout << "run " << runlist[irun] << " has no TaRootRegDef.. "
	       << " SKIPPED." << endl;
	  Badruns[nBadruns]=runlist[irun]; nBadruns++;
	  skip=kTRUE;
	  anaFile.Close();
	}
      }
      Int_t badIndex=-1, badType=0;
      Int_t index=-1;
      // Indices to be obtained from TaRootDitDef
      int monIndex[nMon];
      int detIndex[nDet];

      if(!skip) {
	for(UInt_t imon=0; imon<nMon; imon++) {
	  if(type=="dither") index = ditdef->GetMonitorIndex(sMon[imon]);
	  if(type=="regress") index = regdef->GetIVIndex("diff_"+sMon[imon]);
	  if(index==-1) {
	    badIndex=imon;
	    badType=1;
	  }
	  monIndex[imon] = index;
	}
	index=-1;
	for(UInt_t idet=0; idet<nDet; idet++) {
	  if(type=="dither") index = ditdef->GetDetectorIndex(sDet[idet]);
	  if(type=="regress") index = regdef->GetDVIndex("asym_n_"+sDet[idet]);
	  if(index==-1) {
	    badIndex=idet;
	    badType=2;
	  }
	  detIndex[idet] = index;
	}
      }

      // Skip this run if there's trouble with the indices
      if(badIndex!=-1 || skip) {
	if(!skip) {
	  cout << "run " << runlist[irun] 
	       << " Bad index for ";
	  if(badType==1) cout << sMon[badIndex];
	  if(badType==2) cout << sDet[badIndex];
	  cout << " - SKIPPED" << endl;
	  Badruns[nBadruns]=runlist[irun]; nBadruns++;
	}
	anaFile.Close();
      } else {
	
	// Get the slopes/coeffs tree..
	TTree *slpsTree;
	if(type=="dither") 
	  slpsTree = (TTree*)anaFile.Get("ditslps");
	else if (type=="regress")
	  slpsTree = (TTree*)anaFile.Get("regcoeffs");
	UInt_t nSC = slpsTree->GetEntries(); // # of supercycles in this run
	if(nSC==0) {
	  // No supercycles... head to the next run
	  cout << "run " << runlist[irun] 
	       << " No slopes - SKIPPED" << endl;
	  // Probably not a bad run, if looking for dithering
	  //  (maybe no dithering cycles were performed)
	  if(type=="regress") {Badruns[nBadruns]=runlist[irun]; nBadruns++;}
	  anaFile.Close();
	}else{	
	  
	  Double_t allslopes[nDet][nMon][100]; 
	  UInt_t slope_sc[100];
	  Double_t max_sc;
	  UInt_t sc_firstev[100], sc_lastev[100];
	  for(UInt_t idet=0; idet<nDet; idet++) {
	    for(UInt_t imon=0; imon<nMon; imon++) {
	      for(UInt_t isc=0;isc<100;isc++) {
		allslopes[idet][imon][isc] = -100000;
		slope_sc[isc] = 0;
		sc_firstev[isc] = 0; sc_lastev[isc]=0;
	      }
	    }
	  }
	  
	  // Obtain all of the slopes, for all of the supercycles..
	  for(UInt_t idet=0; idet<nDet; idet++) {
	    for(UInt_t imon=0; imon<nMon; imon++) {
	      TString drawcom;
	      if(type=="dither") drawcom = "slpDetVsMon[";
	      if(type=="regress") drawcom = "coeff[";
	      drawcom += detIndex[idet]; drawcom += "][";
	      drawcom += monIndex[imon]; 
	      if(type=="dither") {
		drawcom += "]:supercyc";
		drawcom += ":firstev:lastev";
	      }
	      if(type=="regress") drawcom += "]:minirun";
	      
	      slpsTree->Draw(drawcom,"","goff");
	      if(nSC!=slpsTree->GetSelectedRows()) {
		// I dont anticipate this as a problem... 
		// ... but just in case... here's a BIG warning.
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" 
		     << endl;
		cout << "!!! Number of Entries != Number of Supercycles !!!" 
		     << endl;
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" 
		     << endl;
	      }
	      for(UInt_t isc=0;isc<slpsTree->GetSelectedRows();isc++) {
		// isc doesn't necessarily equal the current supercycle #
		slope_sc[isc] = TMath::Nint(slpsTree->GetV2()[isc]);
		allslopes[idet][imon][isc] = slpsTree->GetV1()[isc];
		if(type=="dither") {
		  sc_firstev[isc] = TMath::Nint(slpsTree->GetV3()[isc]);
		  sc_lastev[isc] = TMath::Nint(slpsTree->GetV4()[isc]);
		}
	      }
	      // we can assume that the last isc contains the largest observed SC
	      Int_t mxrow = slpsTree->GetSelectedRows();
	      max_sc = slope_sc[mxrow-1] + 1;
	    }
	  }
	  
	  // Fill the branch variables with their values for this run.
	  runnumber = runlist[irun];
	  slugnum = sluglist[irun];
	  for(UInt_t isc=0; isc<nSC; isc++) {
	    for(UInt_t idet=0; idet<nDet; idet++) {
	      for(UInt_t imon=0; imon<nMon; imon++) {
		if(allslopes[idet][imon][isc]==-100000) {
		  ok_slopes=0;
		} else {
		  ok_slopes=1;
		}
		sc_slopes[idet][imon] = allslopes[idet][imon][isc];
	      }
	    }
#ifdef NOISY
	    cout << "run " << runnumber;
#endif
	    if(type=="dither") {
	      supercycle = slope_sc[isc];
	      firstev = sc_firstev[isc];
	      lastev = sc_lastev[isc];
// 	      runmod = (Double_t)runlist[irun] 
// 		+ Double_t(supercycle)/Double_t(nSC);
 	      runmod = (Double_t)runlist[irun] 
 		+ Double_t(supercycle)/Double_t(max_sc);
#ifdef NOISY
	      cout << " supercycle " << supercycle << endl;
#endif
	    }
	    if(type=="regress") {
	      minirun = slope_sc[isc];
	      runmod = (Double_t)runlist[irun] 
		+ Double_t(minirun)/Double_t(nSC);
#ifdef NOISY
	      cout << " minirun " << minirun << endl;
#endif
	    }
	    
	    // Make the summary file the current file.
	    slope_summary_file.cd();
	    // Fill the summary tree with the variables from this run.
	    slopes.Fill();
	  }
	}
	// Close the current run ROOTfile.
	anaFile.Close();
      }
    }
  }

  // make sure the summary file is the current file, and write to it.
  slope_summary_file.cd();
  // The line below is commented out because it makes a second copy
  // of the slopes tree.. and makes the rootfile larger.
  // I guess I dont need it.
  //  slopes.Write();
  slope_summary_file.Write();

  if(nBadruns>0) {
    cout << nBadruns << " Bad or suspect runs: " << endl;
    for(UInt_t irun=0; irun<nBadruns; irun++) {
      cout  << "\t" << Badruns[irun];
    }
    cout << endl;
  }
    
  return 1;


}

