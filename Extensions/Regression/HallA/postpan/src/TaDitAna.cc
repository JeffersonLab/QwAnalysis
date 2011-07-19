//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer PostPan
//
//           TaDitAna.cc
//    - Dithering Class for ReD Analysis
//
// Author:  K.D. Paschke
//
////////////////////////////////////////////////////////////////////////
//
//    Central object in the correction of asymmetry using the
//    dithering technique.  
// 
//
// Current plan: One would run this analysis with the following major steps (not
// all of this relates only to TaDitAna):
//
// 1) Create new TaDitAna
//      Status: First pass largely done. Constructors have been built
// 2) TaDitAna->LoadModData()  - read file and load data for each supercycle
//      Status: First pass Largely done. Needs to have  diagnostic outputs.
// 3) TaDitAna->ComputeDitCoeffs()   - calculate dither slopes and correction
//                                     coeffs (and errors) for each supercycle
//      Status: Trivial first implementation calling TaDitCyc commands.  
//      Requirements: To support TaDitCyc on this, TaDitAna needs a way to
//                    combine subsequent supercycles if one is incomplete
//                    or uncertainty is otherwise too large. Also, TaDitAna
//                    should collect results and be ready to output these
//                    in some useful format(s), like text and root file?
// 4) TaDitAna->CorrectTree  -read pair tree and correct asymmetries for each
//                            detector using the differences from the selected
//                            monitors
//      Status: Not started.
//      Requirements: A plan on what corrections to apply to each event (how 
//              divided, how averaged?) and the machinery for applying these
//              corrections.  If we wish to average over more than one run, we may 
//              need to do this with a new class that will collect output from
//              the DitAna of more than one run.  Of course, diagnostic outputs
//              must also be designed.  Parsing of config file parameters
//              is also necessary
// 5) Results: devise output standards and histograms for both studying and 
//             archiving results of analysis.
//
//    Laundy List:
//        - need curmon added to config file, with normalization to be done in INIT
//        - averaging of dithering coeffs
//        - different "units" constants for energy bpms
//        - provide data structure and utilities for averaging slopes
//        - format text outputs for dit slopes and coeffs
//        - "dithering tree" output, containing dither slopes and coeffs for each
//          supercycle.  Corrected averaged asym's also?
//        - figure out how to open a damn output file
//        - track simulated constants into output
//        - figure how to merge cycles, and how to decide when to merge cycles
//        - figure out what kind of units I'm looking at...
//
////////////////////////////////////////////////////////////////////////

#define NOISY
#define DEBUG
#define USEPAIRTREE


#include "TaDitAna.hh"
#include "TaDitCyc.hh"
#include "TaConfFile.hh"
#include "TaInput.hh"
#include "TLeaf.h"
#include "TTree.h"
#include "TEventList.h"
#include "TaOResultsFile.hh"

#include <fstream>


ClassImp(TaDitAna);
ClassImp(TaRootDitDef);


TaDitAna::TaDitAna():
  fNumCyc(0),curCyc(0)
{
  // Default Constructor: puposefully kept as barren as possible
  //allocate no memory in the default constructor.
}

TaDitAna::~TaDitAna() {
}


Int_t TaDitAna::Init (TaInput& input)
{
  ///////////////////////////////////////////////////////////////
  // Initialization routine.
  //
#ifdef DEBUG
  cout << "TaDitAna::Init()" << endl;
#endif

  //  Call the Common init.
  if (VaAna::Init(input) != 0)
    return 1;

#ifdef USEPAIRTREE
  cout << "TaDitAna compiled to use the PAN PAIR Tree." << endl;
  Int_t nentries = (Int_t) fPairTree->GetEntries();
#else 
  cout << "TaDitAna compiled to use the PAN RAW Tree." << endl;
  Int_t nentries = (Int_t) fRawTree->GetEntries();
#endif
  cout << "Number of Entries = " << nentries << endl;
  // use number of entries to guess number of supercycles
  // supercycle ~2300 event (12x12 + 192)*7
  // ok, this number turns out to be bogus (how long is a DelayTask, anyway?)
  // but you get the idea.   I'll use 900 for now..
#ifdef USEPAIRTREE
  Int_t ncycles = nentries/450;
#else
  Int_t ncycles = nentries/900;
#endif
  if(ncycles>16)   fDitCyc.Expand(ncycles);
  fProtoCyc = new TaDitCyc(); // make prototype cycle 

  //  parse list of dithering coils
  DitList = fInput->GetConfig().GetDitList();

  for (Int_t id=0; id<fgNCoils; id++) DitMap[id]=-1;

  for (UInt_t id = 0; id<DitList.size(); id++) {

    // This is complicated by need to match coils in ditlist with
    // indexes from bmwobject, which requires DitMap
    TString ditname = DitList[id];
    Int_t ditindex = -1;
    if (ditname.Contains("8H",TString::kIgnoreCase)) ditindex = 0;
    else if (ditname.Contains("8V",TString::kIgnoreCase)) ditindex = 1;
    else if (ditname.Contains("10H",TString::kIgnoreCase)) ditindex = 2;
    else if (ditname.Contains("10V",TString::kIgnoreCase)) ditindex = 3;
    else if (ditname.Contains("1H",TString::kIgnoreCase)) ditindex = 4;
    else if (ditname.Contains("2H",TString::kIgnoreCase)) ditindex = 5;
    else if (ditname.Contains("3V",TString::kIgnoreCase)) ditindex = 6;
    else if (ditname.Contains("SL20",TString::kIgnoreCase)) ditindex = 7;
    if (ditindex>=0 && ditindex<fgNCoils) {
      if (DitMap[ditindex]==-1) {
	DitIndex[id]=ditindex;  // for extra safety, raw index for each coil in list
	DitMap[ditindex] = id;  // map from raw coil-index to list of coils
      } else {
	cout << "Coil name is not unique to ID string! " << ditname.Data() << endl;
	DitList.erase(DitList.begin() + id);
	cout << "Duplicate ID-tagged name removed from coil list " << endl;
        id -=1; // redo loop on next element of DitList array
      }
    }else{
      cout << "Coil name matches no ID string: " << ditname.Data() << endl;
      DitList.erase(DitList.begin() + id);
      cout << "Bogus coil name removed from coil list " << endl;
      id -= 1; // redo loop on next element of DitList array
    }
  }
  fNDit = DitList.size();
  fProtoCyc->SetDitNum(fNDit);

  // Parse the list of limits for the various dither objects
  for (Int_t id=0; id<fgNCoils; id++) DitLimit[id] = 1e6;
  vector <pair <TString, Double_t> > LL = fInput->GetConfig().GetDitLimList();
  for (UInt_t idl = 0; idl<LL.size(); idl++) {
    for (UInt_t id=0; id<DitList.size(); id++) {
      if (DitList[id].Contains(LL[idl].first,TString::kIgnoreCase)) {
	DitLimit[id]=LL[idl].second;
	cout << "Limiting input from coil " << DitList[id]
	     << Form(" to less than %.1f",DitLimit[id]) << endl;
      }
    }
  }

#ifdef NOISY

  for (Int_t id=0; id<fNDit; id++) {
    cout << "Dithering object " << DitList[id] << " used" << endl;
    cout << "       with matching index " << DitIndex[id] << endl;
    cout << "       check the map: " << DitMap[DitIndex[id]] << endl;
  }
#endif
  
  // need lists of detectors, monitors, and cuts
  DetList = fInput->GetConfig().GetDetList();
  MonList = fInput->GetConfig().GetMonList();
#ifdef USEPAIRTREE
  for(UInt_t i=0; i<DetList.size(); i++) {
    DetList[i].Prepend("avg_n_");
  }
  for(UInt_t i=0; i<MonList.size(); i++) {
    MonList[i].Prepend("avg_");
  }
#endif
  // If compiled with USEPAIRTREE, ok_cut is hard-coded.
#ifndef USEPAIRTREE
  RwPanCuts = fInput->GetConfig().GetRawCuts();
#endif

  if (fInput->GetConfig().UseCurMon()) {
#ifdef USEPAIRTREE
    fUseCurMon=kFALSE;
    cout << " Control file suggests use of Curmon  " << CurMon << endl;
    cout << "   HOWEVER.  Dithering compiled to use the pairtree." << endl
	 << "   So this request is ignored." << endl;
#else
    fUseCurMon=kTRUE;
    CurMon = fInput->GetConfig().GetCurMon();
    cout << " Control file suggests use of Curmon  " << CurMon << endl;
#endif
  }
  
  // finish prototyping the DitCyc in LoadModData, after
  // lists have been trimmed by availability in the tree  

  LoadModData(); // load data from raw tree


  return 0;
}



Bool_t TaDitAna::LoadModData() 
{  
  // reads raw tree.  Creates and fills 
  // the containers for each modulation supercycle, in preparation
  // for the calculation of the dithering slopes
 
  // **
  // Set appropriate branches in the Tree... what's the difference
  // between a leaf and a branch?  errr... nothing, I think, if the
  // branch has only one leaf...  It's easier for me to think of this
  // approach for single words from the tree.  Anyhoo...
  // **
#ifdef USEPAIRTREE
  Double_t EvR_evNum;
#else
  Int_t EvR_evNum;
#endif
  Double_t EvR_bmwCycle,EvR_bmwObject;
  Double_t EvR_bmwFreq,EvR_bmwPhase,EvR_bmwPeriod;
  Double_t EvR_bmwCoil[]={1,2,3,4,5,6,7,8};

  // These words are unchecked: if they do not exist in the tree,
  // expect spectacular failure.
#ifdef USEPAIRTREE
  fPairTree->SetBranchAddress("m_ev_num",&(EvR_evNum));
  //  fPairTree->SetBranchAddress("avg_bmwcln",&(EvR_bmwClean));
  //  fPairTree->SetBranchAddress("avg_bmwval",&(EvR_bmwValue));
  fPairTree->SetBranchAddress("avg_bmwobj",&(EvR_bmwObject));
//   fPairTree->SetBranchAddress("avg_bmwfreq",&(EvR_bmwFreq));
//   fPairTree->SetBranchAddress("avg_bmqphase",&(EvR_bmwPhase));
  fPairTree->SetBranchAddress("avg_bmwperiod",&(EvR_bmwPeriod));
  fPairTree->SetBranchAddress("avg_bmwcyc",&(EvR_bmwCycle));
  fPairTree->SetBranchAddress("avg_bmwcoil1",&(EvR_bmwCoil[0]));
  fPairTree->SetBranchAddress("avg_bmwcoil2",&(EvR_bmwCoil[1]));
  fPairTree->SetBranchAddress("avg_bmwcoil3",&(EvR_bmwCoil[2]));
  fPairTree->SetBranchAddress("avg_bmwcoil4",&(EvR_bmwCoil[3]));
  fPairTree->SetBranchAddress("avg_bmwcoil5",&(EvR_bmwCoil[4]));
  fPairTree->SetBranchAddress("avg_bmwcoil6",&(EvR_bmwCoil[5]));
  fPairTree->SetBranchAddress("avg_bmwcoil7",&(EvR_bmwCoil[6]));
  fPairTree->SetBranchAddress("avg_bmwcoil8",&(EvR_bmwCoil[7]));
#else
  fRawTree->SetBranchAddress("ev_num",&(EvR_evNum));
  //  fRawTree->SetBranchAddress("bmwcln",&(EvR_bmwClean));
  //  fRawTree->SetBranchAddress("bmwval",&(EvR_bmwValue));
  fRawTree->SetBranchAddress("bmwobj",&(EvR_bmwObject));
//   fRawTree->SetBranchAddress("bmwfreq",&(EvR_bmwFreq));
//   fRawTree->SetBranchAddress("bmwphase",&(EvR_bmwPhase));
  fRawTree->SetBranchAddress("bmwperiod",&(EvR_bmwPeriod));
  fRawTree->SetBranchAddress("bmwcyc",&(EvR_bmwCycle));
  fRawTree->SetBranchAddress("bmwcoil1",&(EvR_bmwCoil[0]));
  fRawTree->SetBranchAddress("bmwcoil2",&(EvR_bmwCoil[1]));
  fRawTree->SetBranchAddress("bmwcoil3",&(EvR_bmwCoil[2]));
  fRawTree->SetBranchAddress("bmwcoil4",&(EvR_bmwCoil[3]));
  fRawTree->SetBranchAddress("bmwcoil5",&(EvR_bmwCoil[4]));
  fRawTree->SetBranchAddress("bmwcoil6",&(EvR_bmwCoil[5]));
  fRawTree->SetBranchAddress("bmwcoil7",&(EvR_bmwCoil[6]));
  fRawTree->SetBranchAddress("bmwcoil8",&(EvR_bmwCoil[7]));
#endif

  // **
  // Grab the Leafs from the Tree.
  // **
  vector <TLeaf*> leaf_mon;
  vector <TLeaf*> leaf_det;
  vector <TLeaf*> leaf_cut;
  TLeaf* leaf_curmon;
#ifdef USEPAIRTREE
  TLeaf* leaf_okcut = (TLeaf*)fPairTree->GetLeaf("ok_cut");
#endif

  // **
  // Get the Det leafs and check if they exist in the Tree
  // **
  for(UInt_t i=0; i<DetList.size(); i++) {
#ifdef USEPAIRTREE
    leaf_det.push_back((TLeaf*)fPairTree->GetLeaf(DetList[i]));
#else
    leaf_det.push_back((TLeaf*)fRawTree->GetLeaf(DetList[i]));
#endif
  }

  for(UInt_t i=0; i<DetList.size(); i++) {
    if(!leaf_det[i]) {
      cerr << DetList[i] << " does not exist in Raw Tree. " 
	   << "Will not include in analysis." << endl;
      DetList.erase(DetList.begin() + i);
      leaf_det.erase(leaf_det.begin() + i);
      i--;
    }
  }
  fNDet = DetList.size();
  fProtoCyc->SetDetNum(fNDet);
#ifdef NOISY
  for(Int_t i=0; i<fNDet; i++) {
    cout << "Detector " << i << ": " << DetList[i] << endl;
  }
#endif
  if (fNDet>fgMxNDet) {
    cout << "Too Many Detectors defined:" << 
      "Recompile TaDitAna with larger maximum!" <<endl;
    return kFALSE;
  }



  // **
  // Get the Mon leafs and check if they exist in the Tree
  // **
  for(UInt_t i=0; i<MonList.size(); i++) {
#ifdef USEPAIRTREE
    leaf_mon.push_back((TLeaf*)fPairTree->GetLeaf(MonList[i]));
#else
    leaf_mon.push_back((TLeaf*)fRawTree->GetLeaf(MonList[i]));
#endif
  }
  for(UInt_t i=0; i<MonList.size(); i++) {
    if(!leaf_mon[i]) {
      cerr << MonList[i] << " does not exist in Raw Tree. " 
	   << "Will not include in analysis." << endl;
      MonList.erase(MonList.begin() + i);
      leaf_mon.erase(leaf_mon.begin() + i);
      i--;
    }
  }
  fNMon = MonList.size();
  fProtoCyc->SetMonNum(fNMon);
#ifdef NOISY
  for(UInt_t i=0; i<MonList.size(); i++) {
    cout << "Monitor " << i << ": " << MonList[i] << endl;
  }
#endif
  if (fNMon>fgMxNMon) {
    cout << "Too Many Monitors defined:" << 
      "Recompile TaDitAna with larger maximum!" <<endl;
    return kFALSE;
  }


  // **
  // Get the CUT leafs and check if they exist in the Tree
  // **
  for(UInt_t i=0; i<RwPanCuts.size(); i++) {
    leaf_cut.push_back((TLeaf*)fRawTree->GetLeaf(RwPanCuts[i]));
  }
  for(UInt_t i=0; i<RwPanCuts.size(); i++) {
    if(!leaf_cut[i]) {
      cerr << RwPanCuts[i] << " does not exist in Raw Tree. " 
	   << "Will not include in analysis." << endl;
      RwPanCuts.erase(RwPanCuts.begin() + i);
      leaf_cut.erase(leaf_cut.begin() + i);
      i--;
    }
  }
#ifdef NOISY
  for(UInt_t i=0; i<RwPanCuts.size(); i++) {
    cout << "Raw PanCuts " << i << ": " << RwPanCuts[i] << endl;
  }
#endif
  fNRwCuts = RwPanCuts.size();
  
  // **
  // Get the CustomCuts if they are defined
  // **
  TCut customcut;
  TEventList *ditEList=0;
#ifdef USEPAIRTREE
  if(fInput->GetConfig().GetCustomCut().first=="P") {
    cout << "Getting PairTree Entries satisfying customcut.." << endl;
    customcut = fInput->GetConfig().GetCustomCut().second;
    fPairTree->Draw(">>ditEList",customcut);
#else
  if(fInput->GetConfig().GetCustomCut().first=="R") {
    cout << "Getting RawTree Entries satisfying customcut.." << endl;
    customcut = fInput->GetConfig().GetCustomCut().second;
    fRawTree->Draw(">>ditEList",customcut);
#endif
    ditEList = (TEventList*)gDirectory->Get("ditEList");
    if(ditEList) 
      if(ditEList->GetN()!=0){
	cout << "\tObtained " << ditEList->GetN()
	     << " entries satisfying customcut." << endl;
      } else {
	ditEList = 0;
	cout << "\tNo entries obtained for customcut." << endl;
      }
  }

  // **
  // Get the Current Monitor leaf
  // **

  if (fUseCurMon) {
    leaf_curmon = (TLeaf*) fRawTree->GetLeaf(CurMon);
    fUseCurMon=kTRUE;
    if (!leaf_curmon) {
      cerr << "Current Monitor " << CurMon << " does not exist in Raw Tree. " 
	   << "Will not include in analysis." << endl;
      fUseCurMon=kFALSE;
    }
  }


  // Start by making the average (or total) cycle

  avgCyc = fProtoCyc->Clone();


  // **
  // loop through tree to sort data in appropriate holders
  // **
#ifdef USEPAIRTREE
  Int_t nentries = (Int_t) fPairTree->GetEntries();
#else
  Int_t nentries = (Int_t) fRawTree->GetEntries();
#endif
  Bool_t inCycle = kFALSE;
  Double_t DetData[fgMxNDet];
  Double_t MonData[fgMxNMon];
  Int_t icurCyc = -1;
#ifdef USEPAIRTREE
  Double_t last_evNum =-1;
#else
  Int_t last_evNum =-1;
#endif
  Bool_t isClean;
  Int_t lastObject;
  Double_t lastValue;
  Int_t lastCycle=-1;

  for (Int_t iEv=0; iEv<nentries; iEv++ ) {
#ifdef USEPAIRTREE
    fPairTree->GetEntry(iEv);
#else
    fRawTree->GetEntry(iEv);
#endif

    // **
    // discontinuous event numbers invalidate the clean flag
    // **
// #ifdef USEPAIRTREE
//     //////////////////////////////////////////////////
//     // Need to check this bmwClean average...
//     //////////////////////////////////////////////////
//     isClean = (((int) EvR_bmwClean) == 1) && (last_evNum-EvR_evNum<=2.0);
// #else
//     isClean = (((int) EvR_bmwClean) == 1) && (EvR_evNum-1==last_evNum);
// #endif

#ifdef USEPAIRTREE
//     //////////////////////////////////////////////////
//     // check this bmwPeriod is between 2 and 30, 1 is usually bad
//     //////////////////////////////////////////////////
    isClean = (((int) EvR_bmwPeriod)>1) && (((int) EvR_bmwPeriod)<=50) && (last_evNum-EvR_evNum<=8.0); // 8.0 for multiplet
//    isClean = (last_evNum-EvR_evNum<=8.0); // 8.0 for multiplet
#else
    isClean = (((int) EvR_bmwPeriod)>1) && (((int) EvR_bmwPeriod)<=50) && (EvR_evNum-1==last_evNum);
    //    isClean = (EvR_evNum-1==last_evNum);
#endif

    last_evNum=EvR_evNum;
    Int_t evNum = (Int_t) EvR_evNum; // bad rounding, don't care

    // bmwobject: 1-8
    // make it 0-7 so that we dont have to change it everywhere
    lastObject = ((int) EvR_bmwObject) - 1;   

    //    lastValue = EvR_bmwValue * 1.E-5;
    // get the value from each coil, read by the scalers
    for(Int_t k=0;k<8;k++){
      if(lastObject==k){
	//	lastValue = EvR_bmwCoil[k] * 1.E-5; 
	lastValue = EvR_bmwCoil[k];  // need to make sure that the bmwCoil# is pedestal corrected.
	break;
      }
      lastValue = 1.E-5;
    }

    if (EvR_bmwCycle<=0 && lastObject<0) {  // ** not a step in a cycle
      if (inCycle) {
#ifdef NOISY
	cout << "TaDitAna:: ending supercycle " << lastCycle 
	     << " at event " << evNum << endl;
#endif
	curCyc->SetLastSCEvent(evNum);
	avgCyc->SetLastSCEvent(evNum);
// 	cout << "TaDitAna:: ending supercycle " << lastCycle 
// 	     << " at event " << iEv << endl;
// #endif
// 	curCyc->SetLastSCEvent(iEv);
// 	avgCyc->SetLastSCEvent(iEv);
	inCycle=kFALSE;
      }
    } else if (EvR_bmwCycle<=0 && lastObject>=0) {  // **  error condition
      cout << "*** cycle=0, but object != -1!! ***" << endl;
      inCycle=kFALSE;
      //    } else if (EvR_bmwCycle>0 && lastObject<0 && isClean) {
      // no longer an error condition
//       cout << "*** cycle>0, but object = -1!!   Supercycle: " << EvR_bmwCycle 
// 	   << " EvNum: " << iEv << endl;
    } else if (EvR_bmwCycle>0 && lastObject>=0) { // ** must be a step in a cycle
      if (inCycle && EvR_bmwCycle != lastCycle) { // check that cycle numbers agree
	cout << "*** error: no break between cycles ***" << endl;
	inCycle = kFALSE;
      }
      if (!inCycle) {// ** start a new cycle, if needed
#ifdef NOISY
	cout << "TaDitAna:: starting new supercycle " << EvR_bmwCycle
	     << " at event " << evNum << endl;
// 	cout << "TaDitAna:: starting new supercycle " << EvR_bmwCycle
// 	     << " at event " << iEv << endl;
#endif
// 	if ((avgCyc->GetFirstEvent())>iEv || (avgCyc->GetFirstEvent())<1) 
// 	  avgCyc->SetFirstEvent(iEv);
//
// 	if (curCyc!=0) curCyc->SetLastEvent(iEv);
	if ((avgCyc->GetFirstEvent())>evNum || (avgCyc->GetFirstEvent())<1) 
	  avgCyc->SetFirstEvent(evNum);

	if (curCyc!=0) curCyc->SetLastEvent(evNum);
	curCyc = fProtoCyc->Clone();
	fDitCyc.AddLast(curCyc);
	inCycle=kTRUE;
	icurCyc = fDitCyc.GetLast();
	//	curCyc->SetFirstEvent(iEv);
	curCyc->SetFirstEvent(evNum);
	cout << " Current DitCyc is number " << icurCyc << " in TObjArray" << endl;
	lastCycle = (Int_t) EvR_bmwCycle;
	curCyc->SetSuperCycleNumber(lastCycle);
	avgCyc->SetSuperCycleNumber(lastCycle);
      }
    }
    
    if (inCycle && isClean && lastObject>=0) {
      //      cout << "********INSIDE THE LOOP********" << endl;
      // **
      // check any cuts, as required  (raw tree cuts are all true if cut failed)
      // **
      Bool_t passRawCuts = kTRUE;
      for (Int_t icut = 0; icut<fNRwCuts; icut++) 
	if (leaf_cut[icut]->GetValue()) passRawCuts=kFALSE;  // cut failed 
      if(ditEList) {
	if(!ditEList->Contains(iEv)) passRawCuts=kFALSE; //customcut failed
      }
// *******unblank below for production data*********
#ifdef USEPAIRTREE
      if(!leaf_okcut->GetValue()) passRawCuts=kFALSE;
#endif
      // **
      // if all ok, get data for each detector and monitor into arrays
      // **
      if (passRawCuts) {
	//	cout << "Passed raw cut" << endl;
	// check that this object is used
	// first, is lastObject in range?
	if (lastObject>-1 && lastObject<fgNCoils) {
	  //	  cout << "lastobj: " << lastObject <<endl;
	  // then, is DitMap reasonably set?
	  Int_t icoil = DitMap[lastObject];
	  if (icoil>-1 && icoil<fNDit) {
	    for (Int_t idet=0; idet<fNDet; idet++) {
	      DetData[idet]= leaf_det[idet]->GetValue();
	      if (fUseCurMon) DetData[idet] = DetData[idet]/leaf_curmon->GetValue();
	    }
	    for (Int_t imon=0; imon<fNMon; imon++)
	      MonData[imon]= leaf_mon[imon]->GetValue();


	    // Finally, check the limit cut:
	    // Defalut DitLimit = 1e6, unless declared explicitly in the conf_dit
	    if ( abs(lastValue*1e5)<DitLimit[icoil] ) {
	      // send this data (detector, monitor, coil + value) to the DitCyc
	      ((TaDitCyc*) fDitCyc.UncheckedAt(icurCyc))
		->LoadData(icoil, lastValue,
			   &(DetData[0]),&(MonData[0]));
	      
	      // send this data to the average cycle, also
	      avgCyc->LoadData(icoil, lastValue,
			       &(DetData[0]),&(MonData[0]));
	    } else {
	      // debugging message
	      cout << "Discarded this data because of dit limit: "
		   << DitList[icoil] 
		   << Form(" at %.1f > limit of %.1f",
			   lastValue*1e5,DitLimit[lastObject])
		   << endl;
	    }
	  }
	}
      } else {
	// one could use this space to count the number of failures, for the ditcyc
	((TaDitCyc*) fDitCyc.UncheckedAt(icurCyc))->StepLostEventCounter();	
// 	cout << ((TaDitCyc*) fDitCyc.UncheckedAt(icurCyc))->GetLostEventCounter()
// 	     << "  for DitCyc " << icurCyc << endl;
      }
    }
  }  // end loop over all events in tree
  
  // bug, now that we use pair tree
  //    if (curCyc!=0) curCyc->SetLastEvent(nentries-1); 
  //    avgCyc->SetLastEvent(nentries-1);
  if (curCyc!=0) curCyc->SetLastEvent((Int_t) last_evNum);
  avgCyc->SetLastEvent((Int_t) last_evNum);
  if (inCycle) {
    curCyc->SetLastSCEvent((Int_t) last_evNum);
    avgCyc->SetLastSCEvent((Int_t) last_evNum);
  }
  
  fNumCyc = fDitCyc.GetLast() + 1;
#ifdef NOISY
  cout << "Finished loading data" << endl;
#endif
  // return success flag if task accomplished
  return kTRUE;
}


void TaDitAna::ComputeDitCoeffs() 
{  
  for (Int_t ic = 0; ic<fNumCyc; ic++) {
    ((TaDitCyc*) fDitCyc[ic])->CalcSlopes();
    // should check returned flag, and do something smart if failure!
  }
  for (Int_t ic = 0; ic<fNumCyc; ic++) {
    ((TaDitCyc*) fDitCyc[ic])->CalcCoeffs();
    // should check returned flag, and do something smart if failure!
  }

  avgCyc->CalcSlopes();
  avgCyc->CalcCoeffs();

}

void TaDitAna::Process() 
{  
//   // make one grandaverage ditcycle
//   if (fDitCyc[0]!=0) {
//     avgCyc = ( (TaDitCyc*) fDitCyc[0])->Copy();
//     for (Int_t ic = 1; ic<fNumCyc; ic++) 
//       avgCyc->Merge((TaDitCyc*) fDitCyc[ic]);
//   }

  ComputeDitCoeffs();


  OutputResults();
//   CorrectPairTree();

}

Bool_t TaDitAna::OutputSlopes() {
  Double_t dat, edat;
  Int_t ev_first, ev_last;
  char rtag[100];
  TString units;
  TString comment;

#ifdef USEPAIRTREE
  for(UInt_t i=0; i<DetList.size(); i++) {
    DetList[i].ReplaceAll("avg_n_","");
  }
  for(UInt_t i=0; i<MonList.size(); i++) {
    MonList[i].ReplaceAll("avg_","");
  }
#endif
  

  for (Int_t ic = 0; ic<fNumCyc; ic++) { // for each supercycle
    TaDitCyc *tcyc = (TaDitCyc*) fDitCyc[ic];
    if (!tcyc->CalcsAreDone()) continue;

    ev_first = tcyc->GetFirstEvent();
    ev_last = tcyc->GetLastEvent();
    // cout << "for supercycle " << ic << " ["<<ev_first<<","<<ev_last<<"]"<<endl;
    for (Int_t idit = 0; idit<fNDit; idit++) { //  for each coil
      // cout << "for dithering object " <<idit<<" "<<DitList[idit].Data()<< endl;
      for (Int_t idet =0; idet < fNDet; idet++) { //for each detector
	//cout << "   for detector " << idet << " " << DetList[idet].Data() << endl;
	//       get each slope of detector vs coil
	dat = tcyc->GetSlopeDetVDit(idet,idit);
	edat = tcyc->GetErrSlopeDetVDit(idet,idit);
	sprintf(rtag,"SC%d_D%s_vs_C%s",ic,
		DetList[idet].Data(),DitList[idit].Data());
	//cout << "rtag =" << rtag<< endl;
	units = "unit";
	units += "_per_";
	units += "mA";
	comment= "(these units are bullshit)";
	//       write each slope of detector vs coil
	fTxtOut->WriteNextLine(rtag,dat,edat,ev_first,ev_last,
			       units.Data(),comment.Data());
	//   void WriteNextLine (const char* rtag, 
	// 		      const Double_t res,
	// 		      const Double_t err,
	// 		      const EventNumber_t ev0,
	// 		      const EventNumber_t ev1,
	// 		      const char* units,
	// 		      const char* com);      // Write a line to the file
      }
      for (Int_t imon = 0; imon<fNMon; imon++) { //     for each monitor 
	// cout << "   for monitor " << imon << " " << MonList[imon].Data() << endl;
	//       get each slope of monitor vs coil
	dat = tcyc->GetSlopeMonVDit(imon,idit);
	edat = tcyc->GetErrSlopeMonVDit(imon,idit);
	sprintf(rtag,"SC%d_M%s_vs_C%s",ic,
		MonList[imon].Data(),DitList[idit].Data());
	//cout << "rtag =" << rtag<< endl;
	units = "unit";
	units += "_per_";
	units += "mA";
	comment= "(these units are bullshit)";
	//       write each slope of Monitor vs coil
	fTxtOut->WriteNextLine(rtag,dat,edat,ev_first,ev_last,
			       units.Data(),comment.Data());
      }
    }
    //   for each monitor
    for (Int_t imon = 0; imon<fNMon; imon++) {
      // cout << "   for monitor " << imon << " " << MonList[imon].Data() << endl;
      //for each detector
      for (Int_t idet =0; idet < fNDet; idet++) {
	//cout << "   for detector " << idet << " " << DetList[idet].Data() << endl;
	//       get each coeff of detector vs monitor
	dat = tcyc->GetCoeffDetVMon(idet,imon);
	edat = tcyc->GetErrCoeffDetVMon(idet,imon);
	//       write each coeff of detector vs monitor
	sprintf(rtag,"SC%d_D%s_vs_M%s",ic,
		DetList[idet].Data(),MonList[imon].Data());
	//cout << "rtag =" << rtag<< endl;
	units = "unit";
	units += "_per_";
	units += "unit";
	comment= "(these units are bullshit)";
	//       write each slope of Monitor vs coil
	fTxtOut->WriteNextLine(rtag,dat,edat,ev_first,ev_last,
				 units.Data(),comment.Data());
      }
    }
  }
  //   average slopes, coeffs for all supercycles
  //   write slopes, coeffs for all supercycles

  return kTRUE;
}



Bool_t TaDitAna::FillSlopesTree() {
// New version: uses vector branches
#ifdef DEBUG
  cout << "TaDitAna::FillSlopesTree()" << endl;
#endif

  char str[100];
  TString buff;
  TString buff2;
  Double_t MCout[fNMon][fNDit];
  Double_t DCout[fNDet][fNDit];
  Double_t DMout[fNDet][fNMon];
  Double_t eMCout[fNMon][fNDit];
  Double_t eDCout[fNDet][fNDit];
  Double_t eDMout[fNDet][fNMon];
  Int_t SC,firstev,lastev,lastSCev,lostcnt;
  TaDitCyc *tcyc;


  rootdef = new TaRootDitDef();
  for (Int_t i=0; i<fNDit; i++) {
    rootdef->PutCoil(DitList[i]);
  }
  for (Int_t i=0; i<fNMon; i++) {
    rootdef->PutMonitor(MonList[i]);
  }
  for (Int_t i=0; i<fNDet; i++) {
    rootdef->PutDetector(DetList[i]);
  }

  rootdef->Write();


  // create tree for dithering slopes for each supercycle
  TTree ditslps("ditslps","dithering slopes tree");
  // create supercycle label branch
  ditslps.Branch("supercyc",&SC,"supercyc/I");
  // create event label branchs
  ditslps.Branch("firstev",&firstev,"firstev/I");
  ditslps.Branch("lastev",&lastev,"lastev/I");
  ditslps.Branch("lastSCev",&lastSCev,"lastSCev/I");
  ditslps.Branch("lostCount",&lostcnt,"lostCount/I");

  // create branches for dB/dC [imon][icoil], dD/dC [idet][icoil], 
  // dD/dB [idet][imon]
  buff = "slpMonVsCoil";
  sprintf(str,"[%d][%d]/D",fNMon,fNDit);
  buff2 = buff + str;
  ditslps.Branch(buff,&(MCout[0]),buff2);

  buff = "slpDetVsCoil";
  sprintf(str,"[%d][%d]/D",fNDet,fNDit);
  buff2 = buff + str;
  ditslps.Branch(buff,&(DCout[0][0]),buff2);

  buff = "slpDetVsMon";
  sprintf(str,"[%d][%d]/D",fNDet,fNMon);
  buff2 = buff + str;
  ditslps.Branch(buff,&(DMout[0][0]),buff2);
  
  buff = "eslpMonVsCoil";
  sprintf(str,"[%d][%d]/D",fNMon,fNDit);
  buff2 = buff + str;
  ditslps.Branch(buff,&(eMCout[0][0]),buff2);

  buff = "eslpDetVsCoil";
  sprintf(str,"[%d][%d]/D",fNDet,fNDit);
  buff2 = buff + str;
  ditslps.Branch(buff,&(eDCout[0][0]),buff2);

//   sprintf(str,"EslpDetVsMon[%d][%d]",fNDet,fNMon);
//   buff = str;
//   buff2 = buff + "/D";
//   ditslps.Branch(buff,&(eDMout[0][0]),buff2);


  // loop over all supercycles
  for (Int_t ic = 0; ic<fNumCyc; ic++) { // for each supercycle
    tcyc = (TaDitCyc*) fDitCyc[ic];

    // first check that slopes were calc'd.  Any other quality checks
    // on each cycle should be verified here, as well.
    if (!tcyc->CalcsAreDone()) cout << "No tree fill for cyc " << ic << endl;
    if (!tcyc->CalcsAreDone()) continue;

    SC = ic;
    firstev = tcyc->GetFirstEvent();
    lastev = tcyc->GetLastEvent();
    lastSCev = tcyc->GetLastSCEvent(); 
   lostcnt = tcyc->GetLostEventCounter();

    //   fill transfer arrays with slopes
    for (Int_t idit = 0; idit<fNDit; idit++) { //  for each coil
      for (Int_t imon = 0; imon<fNMon; imon++) { //     for each monitor 
	MCout[imon][idit] = tcyc->GetSlopeMonVDit(imon,idit);
	eMCout[imon][idit] = tcyc->GetErrSlopeMonVDit(imon,idit);
      }
      for (Int_t idet =0; idet < fNDet; idet++) { //for each detector
	DCout[idet][idit] = tcyc->GetSlopeDetVDit(idet,idit);
	eDCout[idet][idit] = tcyc->GetErrSlopeDetVDit(idet,idit);
      }
    }
    for (Int_t imon = 0; imon<fNMon; imon++) { //     for each monitor 
      for (Int_t idet =0; idet < fNDet; idet++) { //for each detector
	DMout[idet][imon] = tcyc->GetCoeffDetVMon(idet,imon);
	eDMout[idet][imon] = tcyc->GetErrCoeffDetVMon(idet,imon);
      }
    }
    //   fill leaves
    ditslps.Fill();
  }
  // write ditslps
  ditslps.Write();

  // create tree for slopes averaged over all supercycles...
  
  // print out right now for debugging purposes
  PrintCycResults(-1);

  return kTRUE;
}


void TaDitAna::OutputResults() {
  OutputSlopes();
  FillSlopesTree();
}

void TaDitAna::PrintCycResults(Int_t icyc = -1) {

  TaDitCyc* pcyc;

  if (icyc==-1) {
    pcyc = avgCyc;
    cout << " Print Grand Average Cycle results: " << endl;
  } else if (icyc<fNumCyc) {
    pcyc = (TaDitCyc*) fDitCyc[icyc];
  } else {
    cout << "nonesuch cycle to print" << endl;
    return;
  }

  cout << endl << "  Monitors vs. Coils: " << endl;
  for (Int_t imon = 0; imon<fNMon; imon++) { //     for each monitor 
    for (Int_t idit = 0; idit<fNDit; idit++) { //  for each coil
      cout << MonList[imon] << " vs. " << DitList[idit] << " : ";
      cout << pcyc->GetSlopeMonVDit(imon,idit) << "+/-" 
	   << pcyc->GetErrSlopeMonVDit(imon,idit) << endl;
    }
  }

  cout << endl << "  Detectors vs. Coils: " << endl;
  for (Int_t idet = 0; idet<fNDet; idet++) { //     for each detitor 
    for (Int_t idit = 0; idit<fNDit; idit++) { //  for each coil
      cout << DetList[idet] << " vs. " << DitList[idit] << " : ";
      cout << pcyc->GetSlopeDetVDit(idet,idit) << "+/-" 
	   << pcyc->GetErrSlopeDetVDit(idet,idit) << endl;
    }
  }

  cout << endl << "  Detectors vs. Monitors: " << endl;
  for (Int_t idet = 0; idet<fNDet; idet++) { //     for each detitor 
    for (Int_t imon = 0; imon<fNMon; imon++) { //  for each coil
      cout << DetList[idet] << " vs. " << MonList[imon] << " : ";
      cout << pcyc->GetCoeffDetVMon(idet,imon) << "+/-" 
	   << pcyc->GetErrCoeffDetVMon(idet,imon) << endl;
    }
  }
  cout<< endl;

}



Bool_t TaDitAna::CorrectPairTree() {
#ifdef DEBUG
  cout << "TaDitAna::CorrectPairTree()" << endl;
#endif

  TTree dit("dit","Dithering corrected tree");

  //  Int_t nentries;
  Double_t mon[MonList.size()], det[DetList.size()], detOrig[DetList.size()],
    cuts;
  Double_t prcuts[PrPanCuts.size()];
  Int_t ev;
  //  Int_t supercycle;
  Double_t coeff[DetList.size()][MonList.size()];
  Double_t Err_coeff[DetList.size()][MonList.size()];
  TString buff, buff2;
  // additional map needed in case a requested value is missing
  // from the pair tree.
//   Int_t DetMap[DetList.size()]; // maps DetList to PairDetList
//   Int_t MonMap[MonList.size()]; // maps MonList to PairMonList


  cout << "Filling dit tree with dithering-corrected variables" << endl;

  // setup pointers to pairtree data for all detectors, monitors
  vector <TLeaf*> leaf_mon;
  vector <TLeaf*> leaf_det;
  vector <TLeaf*> leaf_cut;
  //  TLeaf* leaf_curmon;
  TLeaf* leaf_ev;
  TString varname;

  // **
  // Get the Det asym leafs and check if they exist in the Pair Tree
  // **
  for(UInt_t i=0; i<DetList.size(); i++) {
    varname = "asym_";
    if (fUseCurMon) varname += "n_";
    varname += DetList[i];
    
    leaf_det.push_back((TLeaf*)fPairTree->GetLeaf(varname));
    if(!leaf_det[i]) {
      cerr << varname << " does not exist in Pair Tree. " << endl;
      //      leaf_det.erase(leaf_det.begin() + i);
      cerr << " For safety and convenience, I choose to fail now! " << endl;
      return kFALSE;
    } else {
      DetVarList.push_back(varname);
    }
  }
#ifdef NOISY
  for(UInt_t i=0; i<DetVarList.size(); i++) {
    cout << "Correcting " << DetVarList[i] << endl;
  }
#endif


  // **
  // Get the Mon asym leafs and check if they exist in the Pair Tree
  // **
  for(UInt_t i=0; i<MonList.size(); i++) {
    varname = "diff_";
    varname += MonList[i];
    
    leaf_mon.push_back((TLeaf*)fPairTree->GetLeaf(varname));
    if(!leaf_det[i]) {
      cerr << varname << " does not exist in Pair Tree. " << endl;
      //      leaf_det.erase(leaf_det.begin() + i);
      cerr << " For safety and convenience, I choose to fail now! " << endl;
      return kFALSE;
    } else {
      MonVarList.push_back(varname);
    }
  }
#ifdef NOISY
  for(UInt_t i=0; i<MonVarList.size(); i++) {
    cout << "Using " << MonVarList[i] << endl;
  }
#endif


  // **
  // Get the CUT leafs and check if they exist in the Tree
  // **
  for(UInt_t i=0; i<PrPanCuts.size(); i++) {
    leaf_cut.push_back((TLeaf*)fPairTree->GetLeaf(PrPanCuts[i]));
  }
  for(UInt_t i=0; i<PrPanCuts.size(); i++) {
    if(!leaf_cut[i]) {
      cerr << PrPanCuts[i] << " does not exist in Pair Tree. " << endl;
      PrPanCuts.erase(PrPanCuts.begin() + i);
      leaf_cut.erase(leaf_cut.begin() + i);
    }
  }
#ifdef NOISY
  for(UInt_t i=0; i<PrPanCuts.size(); i++) {
    cout << "Pair PanCuts " << i << ": " << PrPanCuts[i] << endl;
  }
#endif
  fNPairCuts = PrPanCuts.size();

  leaf_ev = (TLeaf*) fPairTree->GetLeaf("m_ev_num");

  // get coeffs for each det/mon combo 
  // (strong assumption that DetList matches DetVarList!)
  for (UInt_t idet =0; idet<DetList.size(); idet++) {
    for (UInt_t imon =0; imon<MonList.size(); imon++) {
      coeff[idet][imon]=avgCyc->GetCoeffDetVMon(idet,imon);
      if (coeff[idet][imon]==1E-6) coeff[idet][imon]=0;
      Err_coeff[idet][imon]=avgCyc->GetErrCoeffDetVMon(idet,imon);
//       cout << "The coefficient for detector " << DetList[idet]
// 	   << " and monitor " << MonList[imon] << " is "
// 	   << coeff[idet][imon] << endl;
	
    }
  }

  //   create corrected pair data words in output file tree

  dit.Branch("ok_dit",&cuts,"ok_dit/D");

  for(UInt_t k = 0; k<DetVarList.size(); k++) {
     buff = DetVarList[k];
     buff2 = DetVarList[k] + "/D";
     dit.Branch(buff,&detOrig[k],buff2);
   }

  for(UInt_t k = 0; k<DetVarList.size(); k++) {
     buff = "dit_" + DetVarList[k];
     buff2 = DetVarList[k] + "/D";
     dit.Branch(buff,&det[k],buff2);
   }

  for(UInt_t k = 0; k<MonVarList.size(); k++) {
     buff = MonVarList[k];
     buff2 = MonVarList[k] + "/D";
     dit.Branch(buff,&mon[k],buff2);
   }

  dit.Branch("ev_dit",&ev,"ev_dit/I");


  // for each pair
  for (Int_t iEv=0; iEv<fPairTree->GetEntries(); iEv++ ) {
  //  for (Int_t iEv=1; iEv<10; iEv++ ) {
    fPairTree->GetEntry(iEv);
    ev = (Int_t) leaf_ev->GetValue();

    //    cout << "starting work on tree entry " << iEv << endl;
    // check that still in supercycle and update slopes if new supercycle
    // (not currently relevent because we're using run-averaged slopes)

    // get data from pair tree
    for (Int_t icut = 0; icut<fNPairCuts; icut++){
      prcuts[icut] = leaf_cut[icut]->GetValue();
      //      cout << "  Cut " << icut << " has value " << prcuts[icut] << endl;
    }

    for (UInt_t idet=0; idet<DetVarList.size(); idet++) {
      detOrig[idet]= leaf_det[idet]->GetValue();
      //      cout << "  Det " << idet << " has value " << detOrig[idet] << endl;
    }
    for (UInt_t imon=0; imon<MonVarList.size(); imon++) {
      mon[imon]= leaf_mon[imon]->GetValue();
      //      cout << "  Mon " << imon << " has value " << mon[imon] << endl;
    }
    //    cout << "  got data for event " << iEv << endl;

    // check cuts, etc
    Bool_t passPairCuts = kTRUE;
    for (Int_t icut = 0; icut<fNPairCuts; icut++) 
      if (!prcuts[icut]) passPairCuts=kFALSE;  // cut failed 

    //    cout << "  Checked cuts, and found " << passPairCuts << endl;
    if (passPairCuts) {
      //      cout << "passed cuts " << endl;
      // correct det asyms using dither slopes

      for (UInt_t idet=0; idet<DetVarList.size(); idet++) {
	//	cout << "filling tree for det = " << idet << endl;
 	Double_t corval = detOrig[idet];	  
	for (UInt_t imon=0; imon<MonVarList.size(); imon++) {
	  corval += -1*coeff[idet][imon]*mon[imon];
     }
 	det[idet]=corval;
      }
      cuts = 1;
    } else {
      for (UInt_t idet=0; idet<DetVarList.size(); idet++) {
	det[idet]=detOrig[idet];
      }
      cuts = 0;
    }

    // fill output tree
    dit.Fill();

    // fill averages for supercycle, and run-global, 
    //               for det asyms, mon values, 
    //               and slopes used.

  }

  // write output tree head
  dit.Write();

  return kTRUE;

}
