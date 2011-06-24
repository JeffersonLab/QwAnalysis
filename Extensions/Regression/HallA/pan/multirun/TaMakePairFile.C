#define TaMakePairFile_cxx
//////////////////////////////////////////////////////////////////////
//
// TaMakePairFile.C
//
//  Class that compiles together many runs into a single rootfile
//
//    Usage: 
//       TaMakePairFile *mpf = new TaMakePairFile("summary.root",
//                                                "chooser.txt");
//       mpf->SetRunList("runlist.txt");
//       mpf->RunLoop();
//       mpf->Finish();
//
//////////////////////////////////////////////////////////////////////

#include <multirun/TaMakePairFile.h>
#include <fstream>

TaMakePairFile::TaMakePairFile(TString rootfilename, 
			       TString chooserfilename,
			       Bool_t ditherSwitch):
  fRootFile(0),
  fTree(0),
  pairSelect(0),
  regSelect(0),
  fChooser(0),
  fDitFilename(),
  fRunFilename(),
  fDBRunlist(),
  useDBRunlist(kFALSE),
  doubleVars(0),
  intVars(0),
  doubleRegVars(0),
  intRegVars(0),
  ditBPMindex(0),
  DETindex(0),
  fB12Kludge(kFALSE)
{
  // Constructor.  Create the output rootfile and tree.  
  //  Use the info in the TaVarChooser to make tree branches .

  printf("TaMakePairFile(%s,%s,%i)\n",rootfilename.Data(), chooserfilename.Data(), ditherSwitch);

  fRootFile = new TFile(rootfilename,"RECREATE");
  fRootFile->SetCompressionLevel(5);
  cout << "Output ROOTfile compression level "
       << fRootFile->GetCompressionLevel()
       << endl;
  fTree     = new TTree("S","Pair Summary File");
  fChooser  = new TaVarChooser(chooserfilename);
  doDit = ditherSwitch;

  MakeVarList();
  MakeBranches();

}

void TaMakePairFile::GetKludge() 
{ 
  if (fB12Kludge) {
    cout << "BPM12 Kludge is set" << endl;
    cout << "  any dither slope read in for bpm12x will be applied to bpm10x" 
	 << endl;
  } else {
    cout << "Kludge?  I don't see any Kludges around here" << endl;
  }
}
    

void TaMakePairFile::RunLoop() 
{
  // Loops through each run in the Runlist.

  if(!isConfigured()) return;

  pair_num=0;

  // Loop over each run
  for(UInt_t irun=0; irun<runlist.size(); irun++) {

    runnumber = runlist[irun].first;

    useDBRunlist=kFALSE;
    if(fDBRunlist) {
      if(fDBRunlist->GoodRunlist())
	useDBRunlist = kTRUE;
    }
    if(useDBRunlist) {
      if(!fDBRunlist->IsInRunlist(runnumber)) {
	cout << "Run " << runnumber << " is not in the runlist. "
	     << " Not including in PairFile." << endl;
	continue;
      }
      slug = fDBRunlist->GetSlug(runnumber);
      slowsign = fDBRunlist->GetSlowSign(runnumber);
    } else {
      slug      = runlist[irun].second;
      slowsign = 1; // default sign
    }

    // Need to write up this routine... uses TaRootRep
    //     slowsign  = GetRunSign();

    TaFileName panFN(runnumber,"standard","root");
    TFile panFile(panFN.Tstring());
    TTree *asy = (TTree*)panFile.Get("M");
    if(!panFile.IsOpen()) {
      delete asy;
      cerr << "*** Warning: unable to open PAN rootfile for run " 
	   << runnumber << endl;
      continue;
    }

    TaFileName regFN(runnumber,"regress","root","M");
    TFile regFile(regFN.Tstring());
    TTree *reg = (TTree*)regFile.Get("reg");
    if(!regFile.IsOpen()) {
      delete reg;
      cerr << "*** Warning: unable to open Regression rootfile for run " 
	   << runnumber << endl;
      continue;
    }

    // Should find a crafty way to check the trees too..
    //  ... like valid keys...

    Long64_t nevents = asy->GetEntries();
    
    pairSelect = new TaPairSelector(asy,doubleVars,intVars,*fChooser); 
    regSelect = new TaPairSelector(reg,doubleRegVars,intRegVars,*fChooser);

    // Configure dithering for each run, here. If fails, ditOK=0;
    ConfigureDithering();

    EventLoop(nevents);

    panFile.Close();
    regFile.Close();

    delete pairSelect;
    delete regSelect;

    cout << endl;

  }
}

void TaMakePairFile::EventLoop(Long64_t nevents)
{
  // For the current run... loop through each event (up to nevents).
  //  grabbing the selected data from the pan/regression rootfile.

  Double_t minL=0.,maxL=0.,minR=0.,maxR=0.; // These are boundaries to include! not cut!
  if(useDBRunlist) {
    minL = (Double_t)fDBRunlist->GetLeftDetLo(runnumber);
    maxL = (Double_t)fDBRunlist->GetLeftDetHi(runnumber);
    minR = (Double_t)fDBRunlist->GetRightDetLo(runnumber);
    maxR = (Double_t)fDBRunlist->GetRightDetHi(runnumber);
  }
  if (minL!=0 || maxL!=0 || minR!=0 || maxR!=0) {
    printf("Using events from %.0f to %.0f left arm and %.0f to %.0f right arm.",minL, maxL, minR, maxR);
  }

  for(UInt_t ient=0; ient<nevents; ient++){
    
    // Load up the events
    pairSelect->ProcessLoad(ient);
    // Load up the regression events.
    regSelect->ProcessLoad(ient);

    // Check for an ok_cut, or bpm saturation cut
    if(!pairSelect->ProcessCut()) continue; // Nope... move along..
    ok_Left = 1;
    ok_Right = 1;
    
    // Check for specific event cuts...
    Double_t m_ev_num = pairSelect->doubleData[0]; // always the first doubleVar.
    Int_t rfirst_ev = regSelect->intData[0];  // always the first two intRegVars
    Int_t rlast_ev = regSelect->intData[1];
    //	cout << "Mean event number = " << m_ev_num << endl;

    if((minL==0.)&&(maxL==0.)) ok_Left=1;
    else if (m_ev_num>minL && maxL==0) ok_Left=1;
    else if (m_ev_num<minL || m_ev_num>maxL) ok_Left = 0;

    if((minR==0.)&&(maxR==0.)) ok_Right=1;
    else if (m_ev_num>minR && maxR==0) ok_Right=1;
    else if (m_ev_num<minR || m_ev_num>maxR) ok_Right = 0;
    
    if (ok_Left==1 && ok_Right==1) ok_Both=1;
    else ok_Both=0;


    ok_regLeft = ok_Left; ok_regRight = ok_Right; ok_regBoth = ok_Both;
    if (ok_Left==0 && ok_Right==0) continue;

    // now check for ok_regLeft and ok_regRight
    //
    // first, a kludgey way to not be screwed if the new words don't exist.
    if ((rlast_ev - rfirst_ev)>1) {

      if((minL==0.)&&(maxL==0.)) ok_regLeft=ok_Left;
      else if (rfirst_ev>minL && maxL==0) ok_regLeft=ok_Left;
      else if (rfirst_ev<minL || rlast_ev>maxL) ok_regLeft = 0;

      if((minR==0.)&&(maxR==0.)) ok_regRight=ok_Right;
      else if (rfirst_ev>minR && maxR==0) ok_regRight=ok_Right;
      else if (rfirst_ev<minR || rlast_ev>maxR) ok_regRight = 0;
    
      if (ok_regLeft==1 && ok_regRight==1) ok_regBoth=ok_Both;
      else ok_regBoth=0;
    }

    pair_num++;

    // Copy data into the local leafs
    for(UInt_t i=0; i<intVars.size(); i++) {
      intData[i] = pairSelect->intData[i];
    }
    for(UInt_t i=0; i<doubleVars.size(); i++) {
      doubleData[i] = pairSelect->doubleData[i];
    }

    for(UInt_t i=0; i<intRegVars.size(); i++) {
      intRegData[i] = regSelect->intData[i];
    }
    for(UInt_t i=0; i<doubleRegVars.size(); i++) {
      doubleRegData[i] = regSelect->doubleData[i];
    }

    if (doDit) {
      // Calculate DitherCorected data
      for(UInt_t id=0; id<doubleDitVars.size(); id++) {
	if (ditOK[id]) {
	  Double_t cor=0;
	  for(UInt_t im=0; im<ditMonStr.size(); im++)
	    cor += fSlopes[id][im]*pairSelect->doubleData[ditMapMon[im]];
	  Double_t cordet = pairSelect->doubleData[ditMapUD[id]] - cor;
	  doubleDitData[id] = cordet;
	} else {
	  doubleDitData[id] = -1e6;
	}
      }
    } else { 
      for(UInt_t id=0; id<doubleDitVars.size(); id++)
	doubleDitData[id] = -1e6;
    }

    fTree->Fill();
  }
  printf("Total good pairs in slug: %i\n", pair_num);
  //  Int_t returnval = pair_num;
  //  return returnval;

}



void TaMakePairFile::Finish() 
{
  // All done.  Write and close the output.

  cout << "Finished" << endl;

  if(!isConfigured()) {
    cerr << "Not configured so not writing tree.\n";
    return;
  }


  fRootFile->cd();
  fTree->Write();
  fRootFile->Close();

//   if(fChooser) delete fChooser;
//   if(fTree) delete fTree;
//   if(fRootFile) delete fRootFile;
}


void TaMakePairFile::MakeVarList() 
{
  // Using information from the TaVarChooser, create a list of variables
  //  to be read out from the rootfiles (as well as defining the branches
  //  of the output tree).

  doubleVars.push_back("m_ev_num"); // always first doubleVar... dont change it
  intVars.push_back("ok_cutC");
  intRegVars.push_back("reg_mini_first_ev");  // Always the first two intRegVars... 
  intRegVars.push_back("reg_mini_last_ev");
  intRegVars.push_back("minirun");  

  PushToDoubleList(fChooser->GetBatteries(),"diff_batt");

  PushToDoubleList(fChooser->GetDitBPMs(),"avg_bpm","dit");
  PushToDoubleList(fChooser->GetDitBPMs(),"diff_bpm","dit");

  PushToDoubleList(fChooser->GetBPMs(),"avg_bpm");
  PushToDoubleList(fChooser->GetBPMs(),"diff_bpm");

  PushToDoubleList(fChooser->GetBCMs(),"avg_bcm");
  PushToDoubleList(fChooser->GetBCMs(),"asym_bcm");
 
  vector <TString> flumi;
  if(fChooser->GetFLumi()) 
    for(UInt_t i=0; i<2; i++) 
      flumi.push_back(Form("%d",i+1));
  PushToDoubleList(flumi,"avg_n_flumi");
  PushToDoubleList(flumi,"asym_n_flumi");
  PushToDoubleList(flumi,"asym_n_flumi","reg");

  vector <TString> blumi(0);
  if(fChooser->GetBLumi()) {
    for(UInt_t i=0; i<8; i++) 
      blumi.push_back(Form("%d",i+1));
    PushToDoubleList(blumi,"avg_n_blumi");
    PushToDoubleList(blumi,"asym_n_blumi");
    PushToDoubleList(blumi,"asym_n_blumi","reg");
    vector <TString> temp;
    temp.push_back("_sum");
    temp.push_back("_ave");
    PushToDoubleList(temp,"asym_n_blumi");
    PushToDoubleList(temp,"asym_n_blumi","reg");
  }

  vector <TString> dets(0);
  if(fChooser->GetHe4Detectors()) {
    dets.push_back("1"); dets.push_back("3");
    dets.push_back("_lo");
    targetType=1; // helium
  } else if (fChooser->GetLH2Detectors()) {
    dets.push_back("1");   dets.push_back("2");
    dets.push_back("3");   dets.push_back("4");
    dets.push_back("_l");  dets.push_back("_r");
    dets.push_back("_lo"); dets.push_back("_hi");
    dets.push_back("_all");
    targetType=2; // hydrogen
  }
  PushToDoubleList(dets,"avg_n_det");
  if(fChooser->GetLH2Detectors()) 
    dets.push_back("_ave");
  PushToDoubleList(dets,"asym_n_det");
  PushToDoubleList(dets,"asym_n_det","reg");

  vector <TString> bmw(0);
  if(fChooser->GetBMW()) {
    bmw.push_back("bmwcln");
    bmw.push_back("bmwcyc");
    bmw.push_back("bmwobj");
    bmw.push_back("bmwval");
  }
  PushToDoubleList(bmw,"avg_");

  // Make list of dit corrected detectors. Assume that all
  // detectors will be used. If no dithering, these will be set 
  // with -1e6.
  if (doDit)
    PushToDitList(dets,"asym_n_det");

}


void TaMakePairFile::ConfigureDithering() {
  
  // Add dither corrected words here
  
  if (targetType==1) {
    fDitFilename = getDitSetFilenameHe(runnumber);    
  } else if (targetType==2) {
    fDitFilename = getDitSetFilenameH(runnumber);
  } else {
    doDit = kFALSE;
    fDitFilename="";
  }
    
  if (!fDitFilename.IsNull()) {

    if (doubleDitVars.size()) { 
      // only continue if some dither correction detectors are defined
      DitherAlias* dita = new DitherAlias(fDitFilename.Data());

      // Get ordered list of monitors
      ditMonStr.clear();
      vector<string> mstr = dita->GetMonNames();
      for (UInt_t im=0; im<mstr.size(); im++) {
	TString addmon = "diff_" + mstr[im];
	if (fB12Kludge && mstr[im]=="bpm12x") addmon = "diff_bpm10x";
	ditMonStr.push_back(addmon);
	//cout << "monitor list id: "<<im << "  monitor: " << ditMonStr[im] << endl;
	//cout << " Monitor list size = " << ditMonStr.size() << endl;
      }
      // Get vectors of coeffs for each cor. det.
      for (UInt_t i=0; i<doubleDitVars.size(); i++) {
	vector <Double_t> corrcoefs = dita->GetCorSlopes(ditDetSkel[i]);
	if (corrcoefs.size()) {
	  for (UInt_t im=0; im<ditMonStr.size(); im++) {
	    fSlopes[i][im] = corrcoefs[im];
	  }
	  ditOK[i]=kTRUE;
	} else {
	  cout << " *** Dithering problem : Failed to get slopes for det " 
	       << ditDetSkel[i] << endl;
	  cout << endl;
	  for (UInt_t im=0; im<ditMonStr.size(); im++) fSlopes[i][im] = 0.0;
	  ditOK[i]=kFALSE;
	}
      }

      // Search doubleVar to make sure that monitors are there and get index.
      for (UInt_t im=0; im<ditMonStr.size(); im++) {
	for (UInt_t i=0; i<doubleVars.size(); i++) {	
	    ditMapMon[im] = i;
	    if (doubleVars[i]==ditMonStr[im]) break;
	}
	if (ditMapMon[im]==(doubleVars.size()-1)) {
	  cout << "Dithering Monitor not found in list: " << ditMonStr[im] << endl;
	  doDit=kFALSE;
	}
      }

      // Search doubleVar to make sure that uncorrected asyms are there and get index
      for (UInt_t id=0; id<ditDetUD.size(); id++) {
	for (UInt_t i=0; i<doubleVars.size(); i++) {	
	    ditMapUD[id] = i;
	    if (doubleVars[i]==ditDetUD[id]) break;
	}
	if (ditMapUD[id]==(doubleVars.size()-1)) {
	  cout << "Dithering Detector not found in list: " << ditMapUD[id] << endl;
	  ditOK[id]=kFALSE;
	}
      }
    }
    cout << "CHECK if dithering configured correctly: " << doDit << endl;
  }  

}


void TaMakePairFile::PushToDitList(vector <TString> thisvar,
				      TString prefix) 
{
  // Given a vector of variables... push them onto the variable list,
  //  adding the prefix and a suffix where needed.

  // Regular Pan pushes...
  if(thisvar.size()) {
    for(UInt_t i=0; i<thisvar.size();i++) {
      doubleDitVars.push_back("dit_"+prefix+thisvar[i]);
      ditDetSkel.push_back("det"+thisvar[i]);
      ditDetUD.push_back(prefix+thisvar[i]);
      ditOK.push_back(kFALSE);
      cout << "adding " << "dit_"+prefix+thisvar[i] << " to doubleDitVars" << endl;
    }
  }
}



void TaMakePairFile::PushToDoubleList(vector <TString> thisvar,
				      TString prefix,
				      TString type) 
{
  // Given a vector of variables... push them onto the variable list,
  //  adding the prefix and a suffix where needed.

  // Regressing pushes...
  if(type=="reg") {
    if(thisvar.size()) {
      for(UInt_t i=0; i<thisvar.size();i++)
	doubleRegVars.push_back("reg_"+prefix+thisvar[i]);
    }
  } else {
    // Regular Pan pushes...
    if(thisvar.size()) {
      for(UInt_t i=0; i<thisvar.size();i++)
	if (thisvar[i].Contains("bpm") && type!="dit") {
	  // Push for BPMs... dithering BPMs already have x and y
	  doubleVars.push_back(prefix+thisvar[i]+"x");
	  doubleVars.push_back(prefix+thisvar[i]+"y");
	} else {
	  doubleVars.push_back(prefix+thisvar[i]);
	  // Keep track of dithering indices here..
	  if(type=="dit" && prefix.Contains("diff_bpm")) {
	    ditBPMindex.push_back(doubleVars.size());
	  }
	  if(prefix.Contains("asym_n_det") ||
	     prefix.Contains("asym_n_flumi") ||
	     prefix.Contains("asym_n_blumi")) {
	    DETindex.push_back(doubleVars.size());
	  }
	}
    }
  }
}

void TaMakePairFile::MakeBranches() 
{
  // Make the branches for the output tree, according to those variables
  //  choosen in the TaVarChooser.

  // Some default branches that are somewhat specific to each run.
  fTree->Branch("run",&runnumber,"run/I");
  fTree->Branch("sign",&slowsign,"sign/I");
  fTree->Branch("slug",&slug,"slug/I");
  fTree->Branch("ok_cutB",&ok_Both,"ok_cutB/I");
  fTree->Branch("ok_cutL",&ok_Left,"ok_cutL/I");
  fTree->Branch("ok_cutR",&ok_Right,"ok_cutR/I");  
  fTree->Branch("ok_regB",&ok_regBoth,"ok_regB/I");
  fTree->Branch("ok_regL",&ok_regLeft,"ok_regL/I");
  fTree->Branch("ok_regR",&ok_regRight,"ok_regR/I");  
  fTree->Branch("pair_num",&pair_num,"pair_num/I");

  for(UInt_t i=0; i<doubleVars.size(); i++) {
    fTree->Branch(doubleVars[i],&doubleData[i],doubleVars[i]+"/D");
  }
  for(UInt_t i=0; i<intVars.size(); i++) {
    fTree->Branch(intVars[i],&intData[i],intVars[i]+"/I");
  }
  for(UInt_t i=0; i<doubleRegVars.size(); i++) {
    fTree->Branch(doubleRegVars[i],&doubleRegData[i],doubleRegVars[i]+"/D");
  }
  for(UInt_t i=0; i<intRegVars.size(); i++) {
    fTree->Branch(intRegVars[i],&intRegData[i],intRegVars[i]+"/I");
  }
  for(UInt_t i=0; i<doubleDitVars.size(); i++) {
    fTree->Branch(doubleDitVars[i],&doubleDitData[i],doubleDitVars[i]+"/D");
  }

}

Bool_t TaMakePairFile::isConfigured() 
{
  // Check to make sure that we are all configured, before looping
  //  through the runs.

//   if(fDitFilename.IsNull() || runlist.empty()) {
  if(runlist.empty()) {
    cout << "Need to configure TaMakePairFile with:" << endl;
    if(runlist.empty())
      cout << "\t o Run/Slug list source.." << endl
	   << "\t\t TaMakePairFile::SetRunList(filename)" << endl;
    return kFALSE;
  }
  return kTRUE;
}

Bool_t TaMakePairFile::SetRunList(TString listfilename) 
{
  // Given a filename, create the runlist.
  //  File name is in this format
  //    run  slug
  // e.g.
  //    1028  0
  //    1029  1

  runlist.clear();
  
  ifstream slugfile(listfilename.Data());
  UInt_t found=0;
  while(1) {
    UInt_t run=0, slug=0;
    slugfile >> run >> slug;
    if(!slugfile.good()) break;
    runlist.push_back(make_pair(run,slug));
    found++;
  }
  if(found>0) {
    cout << "\n Found " << found << " runs!" << endl << endl;
    return kTRUE;
  } else {
    cout << "\nNo runs found... check filename: " << listfilename 
	 << endl << endl;
    return kFALSE;
  }

}

Bool_t TaMakePairFile::SetRunList(vector <pair <UInt_t,UInt_t> > list)
{
  // Set the runlist by providing the same structure that 
  //  TaMakePairFile uses.

  if(list.empty()) {
    cout << "WARNING: Input runlist is empty." << endl;
    return kFALSE;
  }

  runlist = list;
  return kTRUE;

}
