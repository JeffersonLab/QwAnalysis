///Author: Amrendra Narayan
///Courtesy: Don Jones seeded the code 
#include <rootClass.h>
#include "getEBeamLasCuts.C"
#include "comptonRunConstants.h"
#include "maskedStrips.C"

///////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton electron detector run laser wise and plots the ...
///////////////////////////////////////////////////////////////////////////
 
Int_t expAsym(Int_t runnum, Float_t stripAsym[nPlanes][nStrips], Float_t stripAsymEr[nPlanes][nStrips], Float_t stripAsym_v2[nPlanes][nStrips], Float_t stripAsymEr_v2[nPlanes][nStrips], Bool_t isFirst100k=kFALSE) //, Float_t stripAsymRMS[nPlanes][nStrips]
{
  TString filePrefix= Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  Bool_t debug = 1, debug1 = 0, debug2 = 0;
  Bool_t beamOn =kFALSE;//lasOn,
  Int_t chainExists = 0,goodCycles=0;
  Int_t h = 0, l = 0;//helicity, lasOn tracking variables
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t nMpsB1H1L1=0, nMpsB1H0L1=0;
  Int_t nMpsB1H1L0L=0, nMpsB1H0L0L=0, nMpsB1H1L0R=0, nMpsB1H0L0R=0;
  Int_t nMpsB1H1L0=0,nMpsB1H0L0=0;
  Int_t entry=0; ///integer assitant in reading entry from a file
  Int_t missedLasEntries=0; ///number of missed entries due to unclear laser-state(neither fully-on,nor fully-off)
  Int_t AccumB1H0L1[nPlanes][nStrips],AccumB1H1L1[nPlanes][nStrips];
  Int_t AccumB1H0L0L[nPlanes][nStrips],AccumB1H1L0L[nPlanes][nStrips],AccumB1H0L0R[nPlanes][nStrips],AccumB1H1L0R[nPlanes][nStrips] ;
  Int_t AccumB1H0L1_v2[nPlanes][nStrips],AccumB1H1L1_v2[nPlanes][nStrips];
  Int_t AccumB1H0L0L_v2[nPlanes][nStrips],AccumB1H1L0L_v2[nPlanes][nStrips],AccumB1H0L0R_v2[nPlanes][nStrips],AccumB1H1L0R_v2[nPlanes][nStrips] ;
  Double_t comptQH1L1=0.0, comptQH0L1=0.0;
  Double_t comptQH1L0L=0.0, comptQH0L0R=0.0;
  Double_t comptQH1L0R=0.0, comptQH0L0L=0.0;
  Double_t lasPow[3], helicity, bcm[3], time_nano[3];
  Double_t pattern_number, event_number;
  Double_t bRawAccum[nPlanes][nStrips], bRawAccum_v2[nPlanes][nStrips];
  //  Double_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips]//,stripAsymRMS[nPlanes][nStrips] ;
  Float_t laserOnOffRatioH1=0.0, laserOnOffRatioH0=0.0;
  Float_t qNormAcB1H1L1LasCyc[nPlanes][nStrips], qNormAcB1H0L1LasCyc[nPlanes][nStrips];
  Float_t qNormAcB1H0L0RLasCyc[nPlanes][nStrips], qNormAcB1H1L0LLasCyc[nPlanes][nStrips];
  Float_t qNormAcB1H0L0LLasCyc[nPlanes][nStrips], qNormAcB1H1L0RLasCyc[nPlanes][nStrips];
  Float_t BCqNormAcB1H1L1LasCyc[nPlanes][nStrips], BCqNormAcB1H0L1LasCyc[nPlanes][nStrips];//Background Corrected
  Float_t BCqNormLasCycSum[nPlanes][nStrips], BCqNormLasCycDiff[nPlanes][nStrips];
  Float_t weightedMeanNrAsym[nPlanes][nStrips],weightedMeanDrAsym[nPlanes][nStrips];
  Float_t qNormLasCycAsym[nPlanes][nStrips], LasCycAsymEr[nPlanes][nStrips],LasCycAsymErSqr[nPlanes][nStrips];
  Float_t errB1H1L1[nPlanes][nStrips],errB1H0L1[nPlanes][nStrips];
  Float_t errB1H1L0L[nPlanes][nStrips],errB1H0L0L[nPlanes][nStrips],errB1H1L0R[nPlanes][nStrips],errB1H0L0R[nPlanes][nStrips];

  Float_t qNormAcB1H1L1LasCyc_v2[nPlanes][nStrips], qNormAcB1H0L1LasCyc_v2[nPlanes][nStrips];
  Float_t qNormAcB1H0L0RLasCyc_v2[nPlanes][nStrips], qNormAcB1H1L0LLasCyc_v2[nPlanes][nStrips];
  Float_t qNormAcB1H0L0LLasCyc_v2[nPlanes][nStrips], qNormAcB1H1L0RLasCyc_v2[nPlanes][nStrips];
  Float_t BCqNormAcB1H1L1LasCyc_v2[nPlanes][nStrips], BCqNormAcB1H0L1LasCyc_v2[nPlanes][nStrips];//Background Corrected
  Float_t BCqNormLasCycSum_v2[nPlanes][nStrips], BCqNormLasCycDiff_v2[nPlanes][nStrips];
  Float_t weightedMeanNrAsym_v2[nPlanes][nStrips],weightedMeanDrAsym_v2[nPlanes][nStrips];
  Float_t qNormLasCycAsym_v2[nPlanes][nStrips], LasCycAsymEr_v2[nPlanes][nStrips],LasCycAsymErSqr_v2[nPlanes][nStrips];
  Float_t errB1H1L1_v2[nPlanes][nStrips],errB1H0L1_v2[nPlanes][nStrips];
  Float_t errB1H1L0L_v2[nPlanes][nStrips],errB1H0L0L_v2[nPlanes][nStrips],errB1H1L0R_v2[nPlanes][nStrips],errB1H0L0R_v2[nPlanes][nStrips];

  Float_t term1[nPlanes][nStrips],term2[nPlanes][nStrips],NplusOn_SqQplusOn[nPlanes][nStrips],NminusOn_SqQminusOn[nPlanes][nStrips],NplusOffL_SqQplusOffL[nPlanes][nStrips];
  Float_t NminusOffL_SqQminusOffL[nPlanes][nStrips],NplusOffR_SqQplusOffR[nPlanes][nStrips],NminusOffR_SqQminusOffR[nPlanes][nStrips];
  Float_t erBCqNormLasCycSumSq[nPlanes][nStrips],weightedMeanNrBCqNormSum[nPlanes][nStrips],weightedMeanDrBCqNormSum[nPlanes][nStrips];
  Float_t stripAsymDr[nPlanes][nStrips],stripAsymDrEr[nPlanes][nStrips];

  Float_t weightedMeanNrBCqNormDiff[nPlanes][nStrips],weightedMeanDrBCqNormDiff[nPlanes][nStrips];//erBCqNormLasCycDiffSq[nPlanes][nStrips],
  Float_t stripAsymNr[nPlanes][nStrips],stripAsymNrEr[nPlanes][nStrips];

  Float_t qNormAcB1L0LasCyc[nPlanes][nStrips],erqNormB1L0LasCycSq[nPlanes][nStrips];
  Float_t weightedMeanNrqNormB1L0[nPlanes][nStrips],weightedMeanDrqNormB1L0[nPlanes][nStrips],qNormB1L0[nPlanes][nStrips],qNormB1L0Er[nPlanes][nStrips];
  Float_t tCounterH0L0R,tCounterH0L0L,tCounterH0L1,tCounterH1L0R,tCounterH1L0L,tCounterH1L1;

  TString readEntry;
  TChain *mpsChain = new TChain("Mps_Tree");//chain of run segments
  vector<Int_t>cutLas;//arrays of cuts for laser
  vector<Int_t>cutEB;//arrays of cuts for electron beam

  ofstream outfileExpAsymP, outfileExpAsymP_v2,outAsymComponents;
  ifstream infileLas, infileBeam;

  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1);
  
  ///following variables are not to be reset every laser-cycle hence lets initialize with zero
  for(Int_t p = startPlane; p <endPlane; p++) {      	
    for(Int_t s =startStrip; s <endStrip; s++) {
      //if (maskedStrips(p,s)) continue;    
      weightedMeanNrAsym[p][s]=0.0,weightedMeanDrAsym[p][s]=0.0;
      stripAsym[p][s]= 0.0,stripAsymEr[p][s]= 0.0;

      weightedMeanNrAsym_v2[p][s]=0.0,weightedMeanDrAsym_v2[p][s]=0.0;
      stripAsym_v2[p][s]= 0.0,stripAsymEr_v2[p][s]= 0.0;

      weightedMeanNrBCqNormSum[p][s]=0.0,weightedMeanDrBCqNormSum[p][s]=0.0;
      stripAsymDr[p][s]=0.0,stripAsymDrEr[p][s]=0.0;

      weightedMeanNrBCqNormDiff[p][s]=0.0,weightedMeanDrBCqNormDiff[p][s]=0.0;
      stripAsymNr[p][s]=0.0,stripAsymNrEr[p][s]=0.0;

      weightedMeanNrqNormB1L0[p][s]=0.0,weightedMeanDrqNormB1L0[p][s]=0.0;
      qNormB1L0[p][s]=0.0,qNormB1L0Er[p][s]=0.0;
    }
  }

  /// Open either Pass1 or the First 100K
  if( isFirst100k) {
    chainExists = mpsChain->Add(Form("$QW_ROOTFILES/first100k_%d.root",runnum));
  }
  else {
    chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_Pass1_%d.*.root",runnum));//for Run2
    //chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_%d.*.root",runnum));//for myQwAnalyisis output
    printf("Attached %d files to chain for Run # %d\n",chainExists,runnum);
  }
  if(!chainExists){//delete chains and exit if files do not exist
    cout<<"\n***Error: The analyzed Root file for run "<<runnum<<" does not exist***\n"<<endl;
    delete mpsChain;
    return -1;
  }
  
  infileLas.open(Form("%s/%s/%scutLas.txt",pPath,webDirectory,filePrefix.Data()));
  infileBeam.open(Form("%s/%s/%scutBeam.txt",pPath,webDirectory,filePrefix.Data()));
    
  if (infileLas.is_open() && infileBeam.is_open()) {
    cout<<"Found the cutLas and cutEB file in "<<Form("%s/%s",pPath,webDirectory)<<" directory."<<endl;
    while (infileLas.good()) {
      infileLas >> readEntry; //read the contents of the line in a string first
      if (readEntry.IsDigit()) { //check if the string is a combination of numbers of not
	entry = readEntry.Atoi(); //if string is a combination of numbers get the corresponding Integer of this string
	if (debug) printf("cutLas[%d]=%d\n",(Int_t)cutLas.size(),entry);
	cutLas.push_back(entry);
      }
      //else cout<<"check cutLas file for "<<runnum<<endl;///this always happens at the end of file read
    }
    infileLas.close();
    nLasCycles = (cutLas.size() - 2)/2;
    
    while (infileBeam.good()) {
      infileBeam >> readEntry;
      if (readEntry.IsDigit()) {
	entry = readEntry.Atoi();
	if (debug) printf("cutEB[%d]=%d\n",(Int_t)cutEB.size(),entry);
	cutEB.push_back(entry);
      }
      //else cout<<"check cutEB file for "<<runnum<<endl;
    }
    infileBeam.close();
    nBeamTrips = (cutEB.size())/2;
  }
  else {
    cout << "****:Atleast one of the Cut files missing"<<endl;
    cout<<"       hence executing the cut function****"<<endl;
    Int_t nLasCycBeamTrips = getEBeamLasCuts(cutLas, cutEB, mpsChain,runnum);
    if (debug) printf("nLasCycBeamTrips: %d\n",nLasCycBeamTrips);
    nLasCycles = nLasCycBeamTrips%1000 - 1;
    ////first two digits of return value of getEBeamLasCuts
    nBeamTrips = nLasCycBeamTrips / 1000;
    ////fourth digit of return value of getEBeamLasCuts
  }
  
  if (debug) printf("cutEB.size:%d,cutLas.size:%d\n",cutEB.size(),cutLas.size());

  Int_t nEntries = mpsChain->GetEntries();
  printf("This chain has %i entries.\n", nEntries);

  cout<<"nbeamTrips="<<nBeamTrips<<endl;
  cout<<"nLasCycles="<<nLasCycles<<endl;
  
  mpsChain->ResetBranchAddresses();//!? should it be here?
  mpsChain->SetBranchStatus("*",0);  ////Turn off all unused branches, for efficient looping
  mpsChain->SetBranchStatus("actual_helicity",1);
  mpsChain->SetBranchStatus("sca_laser_PowT",1);
  mpsChain->SetBranchStatus("sca_bcm6",1);
  mpsChain->SetBranchStatus("p*RawAc",1);
  mpsChain->SetBranchStatus("p*RawAc_v2",1);
  mpsChain->SetBranchStatus("time_nano",1);

  mpsChain->SetBranchAddress("event_number",&event_number);
  mpsChain->SetBranchAddress("pattern_number",&pattern_number);
  mpsChain->SetBranchAddress("actual_helicity",&helicity);
  mpsChain->SetBranchAddress("sca_laser_PowT",&lasPow);
  mpsChain->SetBranchAddress("sca_bcm6",&bcm);
  mpsChain->SetBranchAddress("time_nano",&time_nano);

  for(Int_t p = 0; p <nPlanes; p++) {      
    mpsChain->SetBranchAddress(Form("p%dRawAc",p+1),&bRawAccum[p]);
    mpsChain->SetBranchAddress(Form("p%dRawAc_v2",p+1),&bRawAccum_v2[p]);
  }//the branch for each plane is named from 1 to 4

  for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
    if (debug) cout<<"\nStarting nCycle:"<<nCycle<<" and resetting all nCycle variables"<<endl;
    ///since this is the beginning of a new Laser cycle, and all Laser cycle based variables 
    ///..are already assigned to a permanent variable reset the LasCyc based variables
    nMpsB1H1L1= 0, nMpsB1H0L1= 0, nMpsB1H1L0L= 0, nMpsB1H0L0L= 0, nMpsB1H1L0R= 0, nMpsB1H0L0R= 0;
    nMpsB1H1L0= 0, nMpsB1H0L0= 0, missedLasEntries=0; 
    comptQH1L1= 0.0, comptQH0L1= 0.0, comptQH1L0L= 0.0, comptQH0L0L= 0.0, nMpsB1H1L0R= 0, nMpsB1H0L0R= 0;
    laserOnOffRatioH1= 0.0, laserOnOffRatioH0= 0.0;
    tCounterH0L0R= 0.0,tCounterH0L0L= 0.0,tCounterH0L1= 0.0;
    tCounterH1L0R= 0.0,tCounterH1L0L= 0.0,tCounterH1L1= 0.0;

    //lasPowB1H1= 0.0, lasPowB1H0= 0.0;
    for(Int_t p = startPlane; p <nPlanes; p++) {      
      for(Int_t s = startStrip; s <endStrip; s++) {
	AccumB1H0L0L[p][s] =0,AccumB1H0L0R[p][s] =0, AccumB1H1L0L[p][s] =0, AccumB1H1L0R[p][s] =0;
	AccumB1H0L1[p][s] =0, AccumB1H1L1[p][s] =0;
	qNormAcB1H1L1LasCyc[p][s]= 0.0, qNormAcB1H0L1LasCyc[p][s]= 0.0; 
	qNormAcB1H0L0RLasCyc[p][s]= 0.0, qNormAcB1H1L0LLasCyc[p][s]= 0.0;
	qNormAcB1H0L0LLasCyc[p][s]= 0.0, qNormAcB1H1L0RLasCyc[p][s]= 0.0;
	BCqNormAcB1H1L1LasCyc[p][s]= 0.0, BCqNormAcB1H0L1LasCyc[p][s]= 0.0; 
	BCqNormLasCycSum[p][s]= 0.0, BCqNormLasCycDiff[p][s]= 0.0;
	qNormLasCycAsym[p][s]= 0.0, LasCycAsymEr[p][s]= 0.0,LasCycAsymErSqr[p][s]= 0.0;
	errB1H1L1[p][s]=0.0,errB1H0L1[p][s]=0.0;
	errB1H1L0L[p][s]=0.0,errB1H0L0L[p][s]=0.0,errB1H1L0R[p][s]=0.0,errB1H0L0R[p][s]=0.0;

	AccumB1H0L0L_v2[p][s] =0,AccumB1H0L0R_v2[p][s] =0, AccumB1H1L0L_v2[p][s] =0, AccumB1H1L0R_v2[p][s] =0;
	AccumB1H0L1_v2[p][s] =0, AccumB1H1L1_v2[p][s] =0;
	qNormAcB1H1L1LasCyc_v2[p][s]= 0.0, qNormAcB1H0L1LasCyc_v2[p][s]= 0.0; 
	qNormAcB1H0L0RLasCyc_v2[p][s]= 0.0, qNormAcB1H1L0LLasCyc_v2[p][s]= 0.0;
	qNormAcB1H0L0LLasCyc_v2[p][s]= 0.0, qNormAcB1H1L0RLasCyc_v2[p][s]= 0.0;
	BCqNormAcB1H1L1LasCyc_v2[p][s]= 0.0, BCqNormAcB1H0L1LasCyc_v2[p][s]= 0.0; 
	BCqNormLasCycSum_v2[p][s]= 0.0, BCqNormLasCycDiff_v2[p][s]= 0.0;
	qNormLasCycAsym_v2[p][s]= 0.0, LasCycAsymEr_v2[p][s]= 0.0,LasCycAsymErSqr_v2[p][s]= 0.0;
	errB1H1L1_v2[p][s]=0.0,errB1H0L1_v2[p][s]=0.0;
	errB1H1L0L_v2[p][s]=0.0,errB1H0L0L_v2[p][s]=0.0,errB1H1L0R_v2[p][s]=0.0,errB1H0L0R_v2[p][s]=0.0;

	term1[p][s]=0.0,term2[p][s]=0.0,NplusOn_SqQplusOn[p][s]=0.0,NminusOn_SqQminusOn[p][s]=0.0,NplusOffL_SqQplusOffL[p][s]=0.0;
	NminusOffL_SqQminusOffL[p][s]=0.0,NplusOffR_SqQplusOffR[p][s]=0.0,NminusOffR_SqQminusOffR[p][s]=0.0;
	erBCqNormLasCycSumSq[p][s]=0.0;//,erBCqNormLasCycDiffSq[p][s]=0.0;

	qNormAcB1L0LasCyc[p][s]=0.0,erqNormB1L0LasCycSq[p][s]=0.0;
      }
    }

    if(nBeamTrips == 0) beamOn = kTRUE;         ///no beamtrip
    else if(nthBeamTrip < nBeamTrips) {  ///yes, we do have beamtrip(s)
      if(nthBeamTrip==0) { // haven't encountered a trip yet(special case of first trip)
	if(cutLas.at(2*nCycle+3)<cutEB.at(0)) beamOn = kTRUE; ///no beam trip till the end of THIS laser cycle
	else {                    ///there is a beam trip during this laser cycle
	  beamOn = kFALSE;
	  nthBeamTrip++;          ///encountered a beam trip
	}
      }
      else if(cutLas.at(2*nCycle+1)<cutEB.at(2*nthBeamTrip-1)) beamOn=kFALSE;///continuation of the previous nthBeamTrip
      else if(cutLas.at(2*nCycle+3)<cutEB.at(2*nthBeamTrip)) beamOn=kTRUE;
      else { ///encountered "another" beam trip	
	beamOn = kFALSE;  
	nthBeamTrip++;          
      }
    }
    else if(nthBeamTrip == nBeamTrips) { ///encountered the last beamTrip     
      if (cutLas.at(2*nCycle+1) > cutEB.at(2*nthBeamTrip-1)) beamOn = kTRUE; ///current laser Cycle begins after the beamTrip recovered
      else beamOn = kFALSE;
    }   
    else cout<<"\n***Error ... Something drastically wrong in BeamTrip evaluation***\n"<<endl;
      
    if(debug) cout<<"Will analyze from entry # "<<cutLas.at(2*nCycle)<<" to entry # "<<cutLas.at(2*nCycle+3)<<endl;

    for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+3); i++) { 
      //loop over laser cycle periods taking one LasOn state and two adjacent laserOff states
      if(debug && i%100000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;

      if((i < cutLas.at(2*nCycle+1)) && lasPow[0]<minLasPow) l= -1; ///left laser off zone
      else if((i > cutLas.at(2*nCycle+2)) && lasPow[0]<minLasPow) l =0; ///right laser off zone
      else if((i >=cutLas.at(2*nCycle+1)) && (i <=cutLas.at(2*nCycle+2))) l =1;///laser on zone
      ///the equal sign above is in laser-On zone because that's how getEBeamLasCuts currently assign it(may change!)
      else missedLasEntries++;

      mpsChain->GetEntry(i);
      h = (Int_t)helicity;
      if (beamOn) { ////currently the counters are only populated for beamOn cycles
	// 	if (h ==1 || h ==0) {  ////to avoid the h=-9999 that appears in beginning of every runlet
	if (h ==0 && l ==0) {
	  nMpsB1H0L0R++;
	  comptQH0L0R += bcm[0];
	  tCounterH0L0R += time_nano[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      AccumB1H0L0R[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0]; // /lasPow[0];
	      AccumB1H0L0R_v2[p][s] += (Int_t)bRawAccum_v2[p][s];
	    }
	  }
	}
	else if (h ==0 && l ==-1) {
	  nMpsB1H0L0L++;
	  comptQH0L0L += bcm[0];
	  tCounterH0L0L += time_nano[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      AccumB1H0L0L[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0]; // /lasPow[0];
	      AccumB1H0L0L_v2[p][s] += (Int_t)bRawAccum_v2[p][s];// / bcm[0]; // /lasPow[0];
	    }
	  }
	}
	else if (h ==0 && l==1) {////the elseif statement helps avoid overhead in each entry
	  nMpsB1H0L1++;
	  comptQH0L1 += bcm[0];
	  tCounterH0L1 += time_nano[0];
	  //lasPowB1H0 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      AccumB1H0L1[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0]; // /lasPow[0];
	      AccumB1H0L1_v2[p][s] += (Int_t)bRawAccum_v2[p][s];// / bcm[0]; // /lasPow[0];
	    }	  
	  }
	}
	else if (h ==1 && l==0) {
	  nMpsB1H1L0R++;
	  comptQH1L0R += bcm[0];
	  tCounterH1L0R += time_nano[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      AccumB1H1L0R[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0];// /lasPow[0];
	      AccumB1H1L0R_v2[p][s] += (Int_t)bRawAccum_v2[p][s];// / bcm[0];// /lasPow[0];
	    }
	  }
	}
	else if (h ==1 && l==-1) {
	  nMpsB1H1L0L++;
	  comptQH1L0L += bcm[0];
	  tCounterH1L0L += time_nano[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      AccumB1H1L0L[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0];// /lasPow[0];
	      AccumB1H1L0L_v2[p][s] += (Int_t)bRawAccum_v2[p][s];// / bcm[0];// /lasPow[0];
	    }
	  }
	}
	else if (h ==1 && l==1) {
	  nMpsB1H1L1++;
	  comptQH1L1 += bcm[0];
	  tCounterH1L1 += time_nano[0];
	  //lasPowB1H1 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      AccumB1H1L1[p][s] += (Int_t)bRawAccum[p][s];// / bcm[0];// /lasPow[0];
	      AccumB1H1L1_v2[p][s] += (Int_t)bRawAccum_v2[p][s];// / bcm[0];// /lasPow[0];
	    }
	  }
	}
      }
    }///for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+3); i++)
    if(debug) cout<<"Had to skip "<<missedLasEntries<<" entries in this laser cycle"<<endl;

    //after having filled the above vectors based on laser and beam periods, find asymmetry
    if (beamOn) {
      goodCycles++;
      nMpsB1H1L0 = nMpsB1H1L0L + nMpsB1H1L0R; ///total laserOff time for this laserCycle
      laserOnOffRatioH1 = (Float_t)nMpsB1H1L1/nMpsB1H1L0;

      nMpsB1H0L0 = nMpsB1H0L0L + nMpsB1H0L0R; ///total laserOff time for this laserCycle
      laserOnOffRatioH0 = (Float_t)nMpsB1H0L1/nMpsB1H0L0;

      if(debug) cout<<"laserOnOffRatioH0: "<<laserOnOffRatioH0<<" laserOnOffRatioH1: "<<laserOnOffRatioH1<<endl;
      if (debug1) cout<<"the Laser Cycle: "<<nCycle<<" has 'beamOn': "<<beamOn<<endl;
      if (nMpsB1H0L1<= 0 || nMpsB1H1L1<= 0 || nMpsB1H0L0L<= 0 || nMpsB1H1L0L<= 0|| nMpsB1H0L0R<= 0 || nMpsB1H1L0R<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check nMpsB1H0L1:%d,nMpsB1H1L1:%d, nMpsB1H0L0L:%d, nMpsB1H1L0L:%d, nMpsB1H0L0R:%d, nMpsB1H1L0R:%d**\n",
	       nCycle,nMpsB1H0L1,nMpsB1H1L1,nMpsB1H0L0L,nMpsB1H1L0L,nMpsB1H0L0R,nMpsB1H1L0R);
      else if (comptQH0L1<= 0 || comptQH1L1<= 0 || comptQH0L0L<= 0 || comptQH1L0L<= 0 || comptQH0L0R<= 0 || comptQH1L0R<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check comptQH0L1:%f,comptQH1L1:%f, comptQH0L0R:%f, comptQH1L0R:%f**\n",
	       nCycle,comptQH0L1,comptQH1L1,comptQH0L0R,comptQH1L0R);
      else {
	if(debug2) {
	  printf("comptQB1H1L1:%f\tcomptQB1H0L1:%f\tcomptQB1H1L0R:%f\tcomptQB1H0L0R:%f\n",
		 comptQH1L1,comptQH0L1,comptQH1L0R,comptQH0L0R);
	  printf("nMpsB1H1L1:%d\tnMpsB1H0L1:%d\tnMpsB1H1L0R:%d\tnMpsB1H0L0R:%d\n",
		 nMpsB1H1L1,nMpsB1H0L1,nMpsB1H1L0R,nMpsB1H0L0R);
	}
	for (Int_t p =startPlane; p <endPlane; p++) {	  	  
	  for (Int_t s =startStrip; s <endStrip; s++) {	  
	    if (maskedStrips(p,s)) continue;
	    qNormAcB1H1L1LasCyc[p][s]  = AccumB1H1L1[p][s]  /comptQH1L1;
	    qNormAcB1H0L1LasCyc[p][s]  = AccumB1H0L1[p][s]  /comptQH0L1;
	    qNormAcB1H1L0LLasCyc[p][s] = AccumB1H1L0L[p][s] /comptQH1L0L;
	    qNormAcB1H0L0LLasCyc[p][s] = AccumB1H0L0L[p][s] /comptQH0L0L;
	    qNormAcB1H1L0RLasCyc[p][s] = AccumB1H1L0R[p][s] /comptQH1L0R;
	    qNormAcB1H0L0RLasCyc[p][s] = AccumB1H0L0R[p][s] /comptQH0L0R;

	    qNormAcB1H1L1LasCyc_v2[p][s]  = AccumB1H1L1_v2[p][s]  /comptQH1L1;
	    qNormAcB1H0L1LasCyc_v2[p][s]  = AccumB1H0L1_v2[p][s]  /comptQH0L1;
	    qNormAcB1H1L0LLasCyc_v2[p][s] = AccumB1H1L0L_v2[p][s] /comptQH1L0L;
	    qNormAcB1H0L0LLasCyc_v2[p][s] = AccumB1H0L0L_v2[p][s] /comptQH0L0L;
	    qNormAcB1H1L0RLasCyc_v2[p][s] = AccumB1H1L0R_v2[p][s] /comptQH1L0R;
	    qNormAcB1H0L0RLasCyc_v2[p][s] = AccumB1H0L0R_v2[p][s] /comptQH0L0R;
	    //adding new variable to track the laser-off, beam background
	    qNormAcB1L0LasCyc[p][s]= qNormAcB1H1L0LLasCyc[p][s]+qNormAcB1H0L0LLasCyc[p][s]+qNormAcB1H1L0RLasCyc[p][s]+qNormAcB1H0L0RLasCyc[p][s];
	    if (debug2) {
	      printf("qNormLasCycAsym[p%d][s%d]= %f (stat.err:%f)\n",p,s,qNormLasCycAsym[p][s],LasCycAsymErSqr[p][s]);
	      printf("formed by normalized BC (%f -/+ %f) \n",qNormAcB1H1L1LasCyc[p][s],qNormAcB1H0L1LasCyc[p][s]);
	      printf("AccumB1H1L1[%d][%d]: %d\tAccumB1H0L1: %d\tAccumB1H1L0R: %d\tAccumB1H0L0R: %d\n",
		     p,s,AccumB1H1L1[p][s],AccumB1H0L1[p][s],AccumB1H1L0R[p][s],AccumB1H0L0R[p][s]);
	    }
	    BCqNormAcB1H1L1LasCyc[p][s]=qNormAcB1H1L1LasCyc[p][s]-laserOnOffRatioH1*(qNormAcB1H1L0LLasCyc[p][s]+qNormAcB1H1L0RLasCyc[p][s]);
	    BCqNormAcB1H0L1LasCyc[p][s]=qNormAcB1H0L1LasCyc[p][s]-laserOnOffRatioH0*(qNormAcB1H0L0LLasCyc[p][s]+qNormAcB1H0L0RLasCyc[p][s]); 
	    BCqNormLasCycDiff[p][s] = (BCqNormAcB1H1L1LasCyc[p][s] - BCqNormAcB1H0L1LasCyc[p][s]);
	    BCqNormLasCycSum[p][s]  = (BCqNormAcB1H1L1LasCyc[p][s] + BCqNormAcB1H0L1LasCyc[p][s]);

	    BCqNormAcB1H1L1LasCyc_v2[p][s]=qNormAcB1H1L1LasCyc_v2[p][s]-laserOnOffRatioH1*(qNormAcB1H1L0LLasCyc_v2[p][s]+qNormAcB1H1L0RLasCyc_v2[p][s]);
	    BCqNormAcB1H0L1LasCyc_v2[p][s]=qNormAcB1H0L1LasCyc_v2[p][s]-laserOnOffRatioH0*(qNormAcB1H0L0LLasCyc_v2[p][s]+qNormAcB1H0L0RLasCyc_v2[p][s]); 
	    BCqNormLasCycDiff_v2[p][s] = (BCqNormAcB1H1L1LasCyc_v2[p][s] - BCqNormAcB1H0L1LasCyc_v2[p][s]);
	    BCqNormLasCycSum_v2[p][s]  = (BCqNormAcB1H1L1LasCyc_v2[p][s] + BCqNormAcB1H0L1LasCyc_v2[p][s]);

	    if (((BCqNormLasCycSum[p][s] <= 0.0)||(BCqNormLasCycSum_v2[p][s] <= 0.0)) && s<=Cedge) {
	      printf("\n**Warning**:BCqNormLasCycSum[p%d][s%d] is %f in nCycle:%d\n",p,s,BCqNormLasCycSum[p][s],nCycle);
	      printf("note: AccumB1H1L1:%d, AccumB1H1L0R:%d, AccumB1H0L1:%d, AccumB1H0L0R:%d\n"
		     ,AccumB1H1L1[p][s],AccumB1H1L0R[p][s],AccumB1H0L1[p][s],AccumB1H0L0R[p][s]);
	      printf("and comptQH1L1:%f, comptQH1L0R:%f, comptQH0L1:%f, comptQH0L0R:%f",
		     comptQH1L1,comptQH1L0R,comptQH0L1,comptQH0L0R);
	    }
	    else {
	      qNormLasCycAsym[p][s] = (BCqNormLasCycDiff[p][s] / BCqNormLasCycSum[p][s]);
	      qNormLasCycAsym_v2[p][s] = (BCqNormLasCycDiff_v2[p][s] / BCqNormLasCycSum_v2[p][s]);
	      
	      ///Evaluation of error on asymmetry; I've partitioned the evaluation in a way which avoids re-calculation
	      term1[p][s] = (1-qNormLasCycAsym[p][s])/BCqNormLasCycSum[p][s];
	      term2[p][s] = (1+qNormLasCycAsym[p][s])/BCqNormLasCycSum[p][s];
	      NplusOn_SqQplusOn[p][s]      = qNormAcB1H1L1LasCyc[p][s]/comptQH1L1;
	      NminusOn_SqQminusOn[p][s]    = qNormAcB1H0L1LasCyc[p][s]/comptQH0L1;
	      NplusOffL_SqQplusOffL[p][s]  = qNormAcB1H1L0LLasCyc[p][s]/comptQH1L0L;
	      NminusOffL_SqQminusOffL[p][s]= qNormAcB1H0L0LLasCyc[p][s]/comptQH0L0L;
	      NplusOffR_SqQplusOffR[p][s]  = qNormAcB1H1L0RLasCyc[p][s]/comptQH1L0R;
	      NminusOffR_SqQminusOffR[p][s]= qNormAcB1H0L0RLasCyc[p][s]/comptQH0L0R;

	      ///redefining these error variables 
	      errB1H1L1[p][s] = pow(term1[p][s],2) * NplusOn_SqQplusOn[p][s];
	      errB1H0L1[p][s] = pow(term2[p][s],2) * NminusOn_SqQminusOn[p][s];
	      errB1H1L0L[p][s]= pow(term1[p][s],2) * NplusOffL_SqQplusOffL[p][s];
	      errB1H0L0L[p][s]= pow(term2[p][s],2) * NminusOffL_SqQminusOffL[p][s]; 
	      errB1H1L0R[p][s]= pow(term1[p][s],2) * NplusOffR_SqQplusOffR[p][s];
	      errB1H0L0R[p][s]= pow(term2[p][s],2) * NminusOffR_SqQminusOffR[p][s];

 	      LasCycAsymErSqr[p][s] = (errB1H1L1[p][s] + errB1H0L1[p][s] + errB1H1L0R[p][s] + errB1H0L0R[p][s] + errB1H1L0L[p][s] + errB1H0L0L[p][s]);

	      if(LasCycAsymErSqr[p][s]!=(Float_t)0.0) {///eqn 4.17(Bevington)
		weightedMeanNrAsym[p][s] += qNormLasCycAsym[p][s]/LasCycAsymErSqr[p][s]; ///Numerator 
		weightedMeanDrAsym[p][s] += 1.0/LasCycAsymErSqr[p][s]; ///Denominator 
	      }
	      else cout<<"check plane "<<p<<" strip "<<s<<" . It gives zero Asym Er"<<endl;

	      errB1H1L1_v2[p][s] =(1-qNormLasCycAsym_v2[p][s])/(comptQH1L1 *BCqNormLasCycSum_v2[p][s]);
	      errB1H0L1_v2[p][s] =(1+qNormLasCycAsym_v2[p][s])/(comptQH0L1 *BCqNormLasCycSum_v2[p][s]);
	      errB1H1L0L_v2[p][s]=(1-qNormLasCycAsym_v2[p][s])/(comptQH1L0L*BCqNormLasCycSum_v2[p][s]);
	      errB1H0L0L_v2[p][s]=(1+qNormLasCycAsym_v2[p][s])/(comptQH0L0L*BCqNormLasCycSum_v2[p][s]);
	      errB1H1L0R_v2[p][s]=(1-qNormLasCycAsym_v2[p][s])/(comptQH1L0R*BCqNormLasCycSum_v2[p][s]);
	      errB1H0L0R_v2[p][s]=(1+qNormLasCycAsym_v2[p][s])/(comptQH0L0R*BCqNormLasCycSum_v2[p][s]);

	      LasCycAsymErSqr_v2[p][s] =(pow(errB1H1L1_v2[p][s],2))*(AccumB1H1L1_v2[p][s])+ (pow(errB1H0L1_v2[p][s],2))*(AccumB1H0L1_v2[p][s])
		+ (pow(errB1H1L0R_v2[p][s],2))*(AccumB1H1L0R_v2[p][s])+ (pow(errB1H0L0R_v2[p][s],2))*(AccumB1H0L0R_v2[p][s])
		+ (pow(errB1H1L0L_v2[p][s],2))*(AccumB1H1L0L_v2[p][s])+ (pow(errB1H0L0L_v2[p][s],2))*(AccumB1H0L0L_v2[p][s]);

	      if(LasCycAsymErSqr_v2[p][s]!=(Float_t)0.0)
		weightedMeanNrAsym_v2[p][s] += qNormLasCycAsym_v2[p][s]/LasCycAsymErSqr_v2[p][s]; ///Numerator eqn 4.17(Bevington)
	      else cout<<"check plane "<<p<<" strip "<<s<<" . It gives zero Asym Er"<<endl;
	      weightedMeanDrAsym_v2[p][s] += 1.0/LasCycAsymErSqr_v2[p][s]; ///Denominator eqn 4.17(Bevington)

	      ///Error evaluation for SUM (in asymmetry)
	      erBCqNormLasCycSumSq[p][s] = (NplusOn_SqQplusOn[p][s]+NminusOn_SqQminusOn[p][s]+NplusOffL_SqQplusOffL[p][s]+NminusOffL_SqQminusOffL[p][s]
					    +NplusOffR_SqQplusOffR[p][s]+NminusOffR_SqQminusOffR[p][s]);
	      ///Error of laser off background for every laser cycle
	      erqNormB1L0LasCycSq[p][s] = (NplusOffL_SqQplusOffL[p][s]+NminusOffL_SqQminusOffL[p][s]+NplusOffR_SqQplusOffR[p][s]+NminusOffR_SqQminusOffR[p][s]);

	      if(erBCqNormLasCycSumSq[p][s]!=0.0) {
		weightedMeanNrBCqNormSum[p][s] += BCqNormLasCycSum[p][s]/erBCqNormLasCycSumSq[p][s]; ///Numerator eqn 4.17(Bevington)
		weightedMeanDrBCqNormSum[p][s] += 1.0/erBCqNormLasCycSumSq[p][s]; ///Denominator eqn 4.17(Bevington)
		///The error for the difference and sum are same, hence reusing the variable
		weightedMeanNrBCqNormDiff[p][s] += BCqNormLasCycDiff[p][s]/erBCqNormLasCycSumSq[p][s]; ///Numerator eqn 4.17(Bevington)
		//weightedMeanDrBCqNormDiff[p][s] += 1.0/erBCqNormLasCycSumSq[p][s]; ///Denominator eqn 4.17(Bevington)
		weightedMeanNrqNormB1L0[p][s] += qNormAcB1L0LasCyc[p][s]/erqNormB1L0LasCycSq[p][s]; //refer bevington
		weightedMeanDrqNormB1L0[p][s] += 1.0/erqNormB1L0LasCycSq[p][s]; //refer bevington
	      }
	      else cout<<"**Alert: getting zero for erBCqNormLasCycSumSq"<<" in line"<<__LINE__<<endl;
	    }
	  }
	}///for (Int_t p =startPlane; p <endPlane; p++) {
      }///sanity check of being non-zero for filled laser cycle variables
    }///if (beamOn)
    else cout<<"this LasCyc(nCycle:"<<nCycle<<") had a beam trip(nthBeamTrip:"<<nthBeamTrip<<"), hence skipping"<<endl;
  }///for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 

  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip; s++) {        
      if (maskedStrips(p,s)) continue;
      if(weightedMeanDrAsym[p][s]==0.0) cout<<"stand. deviation in weighted Mean Asym is ZERO for p"<<p<<" s"<<s<<endl;
      else if(weightedMeanNrAsym[p][s]==0.0) cout<<"asym for strip "<<s<<" in plane "<<p<<" is zero"<<endl;
      else {
	stripAsym[p][s] = weightedMeanNrAsym[p][s]/weightedMeanDrAsym[p][s];
	stripAsymEr[p][s] = TMath::Sqrt(1/weightedMeanDrAsym[p][s]);

	stripAsym_v2[p][s] = weightedMeanNrAsym_v2[p][s]/weightedMeanDrAsym_v2[p][s];
	stripAsymEr_v2[p][s] = TMath::Sqrt(1/weightedMeanDrAsym_v2[p][s]);

	stripAsymNr[p][s] = weightedMeanNrBCqNormDiff[p][s]/weightedMeanDrBCqNormSum[p][s];
	//stripAsymNrEr[p][s] = TMath::Sqrt(1/weightedMeanDrBCqNormDiff[p][s]);

	stripAsymDr[p][s] = weightedMeanNrBCqNormSum[p][s]/weightedMeanDrBCqNormSum[p][s];
	stripAsymDrEr[p][s] = TMath::Sqrt(1/weightedMeanDrBCqNormSum[p][s]);
	///the error in numerator and denominator are same, hence reevaluation is avoided

	qNormB1L0[p][s] = weightedMeanNrqNormB1L0[p][s]/weightedMeanDrqNormB1L0[p][s];
	qNormB1L0Er[p][s] = TMath::Sqrt(1/weightedMeanDrqNormB1L0[p][s]);
      }
      if(debug2) printf("stripAsym[%d][%d]:%f  stripAsymEr:%f\n",p,s,stripAsym[p][s],stripAsymEr[p][s]);
    }
  }
  
  ///Writing to all useful files
  for(Int_t p = startPlane; p < endPlane; p++) { //currently this is set to work only for 1 plane
    //outfileExpAsymP<<"strip\texpAsym\tasymEr"<<endl; ///If I want a header for the following text
    outfileExpAsymP.open(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if (outfileExpAsymP.is_open()) {
      cout<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      for (Int_t s =startStrip; s <endStrip;s++) {    
	if (maskedStrips(p,s)) continue;
	outfileExpAsymP<<Form("%2.0f\t%f\t%f\n",(Float_t)s+1,stripAsym[p][s],stripAsymEr[p][s]);
      }
      outfileExpAsymP.close();
      cout<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
    }
    else cout<<"\n***Alert: Couldn't open file for writing experimental asymmetry values\n\n"<<endl;

    outfileExpAsymP_v2.open(Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if (outfileExpAsymP_v2.is_open()) {
      cout<<Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      for (Int_t s =startStrip; s <endStrip;s++) {    
	if (maskedStrips(p,s)) continue;
	outfileExpAsymP_v2<<Form("%2.0f\t%f\t%f\n",(Float_t)s+1,stripAsym_v2[p][s],stripAsymEr_v2[p][s]);
      }
      outfileExpAsymP_v2.close();
      cout<<Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
    }
    else cout<<"\n***Alert: Couldn't open file for writing experimental asymmetry values for version2(v2) data\n\n"<<endl;
    
    outAsymComponents.open(Form("%s/%s/%sexpAsymComponentsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    //outAsymComponents<<"strip\texpAsymNr\texpAsymDr\texpAsymDrEr"<<endl; ///If I want a header for the following text
    if(outAsymComponents.is_open()) {
      cout<<Form("%s/%s/%sexpAsymComponentsP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      for (Int_t s =startStrip; s <endStrip;s++) {    
	if (maskedStrips(p,s)) continue;
	outAsymComponents<<Form("%2.0f\t%g\t%g\t%g\t%g\t%g\n",(Float_t)s+1,stripAsymNr[p][s],stripAsymDr[p][s],stripAsymDrEr[p][s],qNormB1L0[p][s],qNormB1L0Er[p][s]);
      }
      outAsymComponents.close();
      cout<<Form("%s/%s/%sexpAsymComponentsP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
    }
    else cout<<"\n***Alert: Couldn't open file for writing asymmetry components\n\n"<<endl;      
  }

  //  delete mpsChain;
  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to evaluate expAsym.\n",div_output.quot,div_output.rem );  
  return goodCycles;//the function returns the number of used Laser cycles
}

/******************************************************
!Further modifications:
* Plot asymmetry numerators and denominators separately, because the numerator naturally 
*..has background subtraction, while the denominator is artificially subtracted.
* if I figure out a way to normalize by laserPower then I can relax 
*..the condition of laserPower being 90% of maximum for the laser entries to be accepted.
* plot qNormAcB1H1L0LasCyc and qNormAcB1H0L0LasCyc against
*..strip numbers to watch the helicity correlated beam-background
* evaluate asymmetry per pattern_number and see its behaviour
* ensure efficient evaluation of beamtrips
* check consistency of cut on laserPow and beamtrip
* add color code to the error and warning messages
******************************************************/

/******************************************************
Comments:
* B1: beam on; H1: helicity plus; L1: laser on; and vice versa
* qNormXXX: charge normalized quantity; tNormXXX: time normalized quantity
* BC: Backgraound Corrected.
* each Laser cycle consitutes of one laser on and one laser off period.
* ..it begins with a Laser On period and ends with the (just)next laser off period.
* While checking if we want to consider a laser cycle as bad, we check till 
* ..the beginning of the next laser cycle on purpose to make sure that the 
* ..beam was indeed ON during both laser On as well as Off periods
* nCycle refers to the present/current/ongoing laser-cycle
* nthBeamTrip (on the contrary) refers to the upcoming beamTrip
* the code counts from '0' but at all human interface, I add '+1' hence human counting
* Notice that my beamOff's are not true beam-off, they include ramping data as well
******************************************************/
