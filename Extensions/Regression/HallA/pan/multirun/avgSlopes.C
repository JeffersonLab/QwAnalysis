//////////////////////////////////////////////////////////////////////
//
// Create results file with slopes averaged over
//  1) a subset of runs, or
//  2) a slug, or
//  3) a specified cut.
//
//  Authors: Lisa Kaufman, Kent Paschke
//           Bryan Moffit (slight modifications)
//
//  Requires: root file created using obtainBMSlopes.C
//
//  Usage:
//   -  Providing a range of runs (e.g. 4006-4009),
//      Dithering slopes from helium run:
//
//         avgSlopes(4006,4009,"dither","helium")
//
//   -  Providing a slugnumber (e.g. slug 20),
//      Regression slopes from hydrogen run:
//
//         avgSlopes(20,"regress","hydrogen")
//
//   -  Providing a specified cut (e.g. slug!=3),
//      Dithering slopes from helium run, with a desired
//      string in the filename (noslug3)
//
//         avgSlopes("slug!=3","dither","helium","noslug3")
//
//   -  Providing a specified cut (e.g. slug!=3) as a cut
//      Dithering slopes from helium run, with a desired
//      string in the filename (noslug3)
//
//         TCut scut = "slug!=3";
//         avgSlopes(scut,"dither","helium","noslug3")
//

//
// creates average slopes results file for an inclusive range of runs
//
int avgSlopes(Int_t firstrun, 
	       Int_t lastrun, 
	       TString type="regress", 
	       TString exp ="helium")
{
  return avgSlopes(Form("run>=%d&&run<=%d",firstrun,lastrun),
		   type,
		   exp,
		   Form("runs%d_%d",firstrun,lastrun));
}


//
// creates results file for individual slug
//
int avgSlopes(Int_t slug,
	      TString type="regress", 
	      TString exp ="helium")
{
  return avgSlopes(Form("slug==%d",slug),
		   type,
		   exp,
		   Form("slug%d",slug));
}

//
// creates results file for a provided selection string
//
int avgSlopes(TString selection,
	      TString type, 
	      TString exp,
	      TString outname="specifiedcut")
{
  TCut selcut = selection;
  return avgSlopes(selcut,
		   type,
		   exp,
		   outname);
}

//
// creates results file for a provided selection
//
int avgSlopes(TCut selection,
	      TString type, 
	      TString exp,
	      TString outname="specifiedcut")
{
  cout << "Begin slope averaging." << endl;

  if( (!type.Contains("regress")) && (!type.Contains("dither")) ) {
    cout << "Bad analysis type: " << type << endl;
    return 0;
  }

  if( (!exp.Contains("helium")) && (!exp.Contains("hydrogen")) ) {
    cout << "Bad experiment name: " << exp << endl;
    return 0;
  }

  const UInt_t nDet = 17;
  TString sDet[nDet] = {"det1","det2","det3","det4",
			"det_lo","det_hi","det_l","det_r","det_all",
			"blumi1","blumi2","blumi3","blumi4",
			"blumi5","blumi6","blumi7","blumi8"};

  TCut sCut[nDet] = {"ok_slopesL","ok_slopesL","ok_slopesR","ok_slopesR",
			"ok_slopesL&&ok_slopesR","ok_slopesL&&ok_slopesR",
			"ok_slopesL","ok_slopesR",
			"ok_slopesL&&ok_slopesR",
			"ok_slopes","ok_slopes","ok_slopes","ok_slopes",
			"ok_slopes","ok_slopes","ok_slopes","ok_slopes"};

  Bool_t useDet[nDet] = {kTRUE,kTRUE,kTRUE,kTRUE,
 			 kTRUE,kTRUE,kTRUE,kTRUE,kTRUE,
 			 kTRUE,kTRUE,kTRUE,kTRUE,
 			 kTRUE,kTRUE,kTRUE,kTRUE};
  
  if (exp.Contains("helium",1)) {
    useDet[1] = kFALSE;
    useDet[3] = kFALSE;
    useDet[5] = kFALSE;
    useDet[6] = kFALSE;
    useDet[7] = kFALSE;
    useDet[8] = kFALSE;
  }

  const UInt_t nMon = 5;
  TString sMon[nMon] = {"bpm4bx", "bpm4by", 
			"bpm4ax", "bpm4ay",
			"bpm12x"};

  Double_t avgSlope[nDet][nMon];
  Double_t avgSlope_err[nDet][nMon];

  //needs to work for regression or dithering slopes
  cout << "Analysis type = " << type << endl;
  TString openfile = type +"_" + exp+".root";
  cout << "Slopes summary rootfile = " << openfile << endl;
  TFile file(openfile);
  
  TTree *slopes = (TTree*) file.Get("slopes");
  slopes->Draw("run",selection+"ok_slopes","goff");
  Int_t n = (Int_t)slopes->GetSelectedRows();
  if(n==0) {
    TString tstr = selection;
    cout << "No slopes passing selection: " << tstr.Data() << endl;
    return 0;
  }
  cout << " slopes passing selection = " << n << endl;
  const Int_t nruns = n;
  Double_t run[5000];
  for(UInt_t i=0;i<nruns;i++){
    run[i] = slopes->GetV1()[i];
  }
  
  // ASSUMES THE RUNS ARE LISTED IN ORDER!
  EventNumber_t firstRun = (EventNumber_t) run[0];
  EventNumber_t lastRun = (EventNumber_t) run[nruns-1];
  
  for(UInt_t idet=0; idet<nDet; idet++) {
    if (useDet[idet]) {
#ifdef DEBUG
      cout << sDet[idet] << endl;
#endif
      for(UInt_t imon=0; imon<nMon; imon++) {
	TString drawcmd;
	drawcmd = sDet[idet]+"_"+sMon[imon];
	slopes->Draw(drawcmd+">>h1",selection+sCut[idet],"goff");
	TH1F *h1 = (TH1F*)gROOT->FindObject("h1");
	if(h1->GetEntries()==0) {
	  avgSlope[idet][imon] = 0.0;
	  avgSlope_err[idet][imon] = 0.0;
	} else {
	  avgSlope[idet][imon] =  h1->GetMean();
	  avgSlope_err[idet][imon] = h1->GetRMS()/sqrt(h1->GetEntries());
	}
#ifdef DEBUG
	cout << "average slope for " << drawcmd
	     << " = " << avgSlope[idet][imon]<< endl
	     << "average slope error for " <<drawcmd
	     << " = " << avgSlope_err[idet][imon]<< endl;
#endif
	delete h1;
      }
    }
    
  }
  
  //Now I need to write these slopes out to a .res file
  TString ofilename = "slopes/avgSlopes_"+exp+"_"+type+"_"+outname+".res";
  cout << "Creating Results File: " << ofilename.Data() << endl;
  rfile = new TaOResultsFile("redana",ofilename,1000);
  
  for(UInt_t idet=0; idet<nDet; idet++) {
    if (useDet[idet]) {
      for(UInt_t imon=0; imon<nMon; imon++) {
	TString slopename = outname;
	if(type=="dither")
	  slopename += "_D";
	if(type=="regress")
	  slopename += "_asym_n_";
	slopename += sDet[idet];
	if(type=="dither")
	  slopename += "_vs_M";
	if(type=="regress")
	  slopename += "_vs_diff_";
	slopename += sMon[imon];
	if(type=="regress")
	  slopename += "_slope";
	rfile->WriteNextLine (slopename,
			      avgSlope[idet][imon],
			      avgSlope_err[idet][imon],
			      firstRun,
			      lastRun,
			      "","");
      }
    }
  }
  cout << "Slope averaging complete." << endl;

  return 1;
}
