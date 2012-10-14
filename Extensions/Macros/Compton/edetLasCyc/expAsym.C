///Author: Amrendra Narayan
///Courtesy: Don Jones seeded the code 
#include <rootClass.h>
#include "getEBeamLasCuts.C"
#include "comptonRunConstants.h"
#include "maskedStrips.C"
#include "evaluateAsym.C"
///////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton electron detector run laser wise and plots the ...
///////////////////////////////////////////////////////////////////////////
 
Int_t expAsym(Int_t runnum)
{
  cout<<"\nstarting into expAsym.C**************\n"<<endl;
  TString filePrefix= Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  const Bool_t debug = 1, debug1 = 0, debug2 = 0;
  const Bool_t lasCycPrint=1;//!if accum & scaler counts are needed for every laser cycle
  Bool_t beamOn =kFALSE;//lasOn,
  Int_t goodCycles=0,chainExists = 0;
  Int_t h = 0, l = 0;//helicity, lasOn tracking variables
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t nMpsB1H1L1=0, nMpsB1H0L1=0, nMpsB1H1L0=0, nMpsB1H0L0=0;

  Int_t entry=0; ///integer assitant in reading entry from a file
  Int_t missedLasEntries=0; ///number of missed entries due to unclear laser-state(neither fully-on,nor fully-off)
  Int_t AccumB1H0L1[nPlanes][nStrips],AccumB1H1L1[nPlanes][nStrips];
  Int_t AccumB1H0L0[nPlanes][nStrips],AccumB1H1L0[nPlanes][nStrips];
//   Int_t EventB1H0L1[nPlanes][nStrips],EventB1H1L1[nPlanes][nStrips];
//   Int_t EventB1H0L0[nPlanes][nStrips],EventB1H1L0[nPlanes][nStrips];
  Int_t ScalerB1H0L1[nPlanes][nStrips],ScalerB1H1L1[nPlanes][nStrips];
  Int_t ScalerB1H0L0[nPlanes][nStrips],ScalerB1H1L0[nPlanes][nStrips];
  Int_t totScalerB1L0[nPlanes][nStrips],totScalerB1L1[nPlanes][nStrips]; 
  Double_t tNormScalerB1L1[nPlanes][nStrips],tNormScalerB1L0[nPlanes][nStrips];
  Double_t bRawScaler[nPlanes][nStrips];
  Double_t comptIH1L1=0.0, comptIH0L1=0.0,comptIH1L0=0.0,comptIH0L0=0.0;
  Double_t lasPow[3], helicity, bcm[3],bpm_3c20X[2];
  Double_t bpm_3p02aX[2],bpm_3p02aY[2],bpm_3p02bX[2],bpm_3p02bY[2],bpm_3p03aX[2],bpm_3p03aY[2];
  Double_t pattern_number, event_number;
  Double_t bRawAccum[nPlanes][nStrips], bRawAccum_v2[nPlanes][nStrips];
  Float_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips],stripAsym_v2[nPlanes][nStrips],stripAsymEr_v2[nPlanes][nStrips];
  Float_t weightedMeanNrAsym[nPlanes][nStrips],weightedMeanDrAsym[nPlanes][nStrips];
  Float_t LasCycAsymEr[nPlanes][nStrips],LasCycAsymErSqr[nPlanes][nStrips];
  Float_t lasPowB1H0L0,lasPowB1H1L0,lasPowB1H0L1,lasPowB1H1L1;//!as of now not tracking the las-off status

  Int_t AccumB1H0L1_v2[nPlanes][nStrips],AccumB1H1L1_v2[nPlanes][nStrips];
  Int_t AccumB1H0L0_v2[nPlanes][nStrips],AccumB1H1L0_v2[nPlanes][nStrips];
  Float_t weightedMeanNrAsym_v2[nPlanes][nStrips],weightedMeanDrAsym_v2[nPlanes][nStrips];
  Float_t LasCycAsymEr_v2[nPlanes][nStrips],LasCycAsymErSqr_v2[nPlanes][nStrips];
  Float_t stripAsymDr_v2[nPlanes][nStrips],stripAsymNr_v2[nPlanes][nStrips],qNormB1L0_v2[nPlanes][nStrips];
  Float_t stripAsymDrEr_v2[nPlanes][nStrips],stripAsymNrEr_v2[nPlanes][nStrips],qNormB1L0Er_v2[nPlanes][nStrips];
  Float_t weightedMeanNrBCqNormSum_v2[nPlanes][nStrips],weightedMeanDrBCqNormSum_v2[nPlanes][nStrips],weightedMeanNrBCqNormDiff_v2[nPlanes][nStrips];
  Float_t weightedMeanNrqNormB1L0_v2[nPlanes][nStrips],weightedMeanDrqNormB1L0_v2[nPlanes][nStrips],weightedMeanDrBCqNormDiff_v2[nPlanes][nStrips];
  Float_t weightedMeanNrBCqNormSum[nPlanes][nStrips],weightedMeanDrBCqNormSum[nPlanes][nStrips];
  Float_t stripAsymDr[nPlanes][nStrips],stripAsymDrEr[nPlanes][nStrips];
  Float_t weightedMeanNrBCqNormDiff[nPlanes][nStrips],weightedMeanDrBCqNormDiff[nPlanes][nStrips];
  Float_t stripAsymNr[nPlanes][nStrips],stripAsymNrEr[nPlanes][nStrips];
  Float_t weightedMeanNrqNormB1L0[nPlanes][nStrips],weightedMeanDrqNormB1L0[nPlanes][nStrips],qNormB1L0[nPlanes][nStrips],qNormB1L0Er[nPlanes][nStrips];
  Int_t totAccumB1H1L1[nPlanes][nStrips],totAccumB1H0L1[nPlanes][nStrips],totAccumB1H1L0[nPlanes][nStrips],totAccumB1H0L0[nPlanes][nStrips];//!for comparision for fortran tables
  Double_t totIH1L1=0.0,totIH1L0=0.0,totIH0L1=0.0,totIH0L0=0.0;
  Int_t totMpsB1H1L1=0,totMpsB1H1L0=0,totMpsB1H0L1=0,totMpsB1H0L0=0;

  TString readEntry;
  TChain *mpsChain = new TChain("Mps_Tree");//chain of run segments
  vector<Int_t>cutLas;//arrays of cuts for laser
  vector<Int_t>cutEB;//arrays of cuts for electron beam
  ofstream lasCycExpAsym,lasCycYield;
  ofstream lasCycScaler[nPlanes][nStrips],lasCycAccum[nPlanes][nStrips],lasCycBCM,lasCycLasPow;//to write files every laserCycle
  ofstream outfileExpAsymP,outfileYield,outfilelasOffBkgd,fortranCheck,outScaler;
  ifstream infileLas, infileBeam;

  gSystem->mkdir(Form("%s/%s/run_%d",pPath,webDirectory,runnum));

  ///following variables are not to be reset every laser-cycle hence lets initialize with zero
  for(Int_t p = startPlane; p <endPlane; p++) {      	
    for(Int_t s =startStrip; s <endStrip; s++) {
      //if (maskedStrips(p,s)) continue;    
      weightedMeanNrAsym[p][s]=0.0,weightedMeanDrAsym[p][s]=0.0;
      stripAsym[p][s]= 0.0,stripAsymEr[p][s]= 0.0;

      weightedMeanNrBCqNormSum[p][s]=0.0,weightedMeanDrBCqNormSum[p][s]=0.0;
      stripAsymDr[p][s]=0.0,stripAsymDrEr[p][s]=0.0;

      weightedMeanNrBCqNormDiff[p][s]=0.0,weightedMeanDrBCqNormDiff[p][s]=0.0;
      stripAsymNr[p][s]=0.0,stripAsymNrEr[p][s]=0.0;

      weightedMeanNrqNormB1L0[p][s]=0.0,weightedMeanDrqNormB1L0[p][s]=0.0;
      qNormB1L0[p][s]=0.0,qNormB1L0Er[p][s]=0.0;

      totAccumB1H1L1[p][s]=0,totAccumB1H0L1[p][s]=0,totAccumB1H1L0[p][s]=0,totAccumB1H0L0[p][s]=0;
      totScalerB1L1[p][s]=0,totScalerB1L0[p][s]=0,
      tNormScalerB1L1[p][s]=0.0,tNormScalerB1L0[p][s]=0.0;
    }
  }
  if(v2processed) {
    for(Int_t p = startPlane; p <endPlane; p++) {      	
      for(Int_t s =startStrip; s <endStrip; s++) {	
	weightedMeanNrAsym_v2[p][s]=0.0,weightedMeanDrAsym_v2[p][s]=0.0;
	stripAsym_v2[p][s]= 0.0,stripAsymEr_v2[p][s]= 0.0;
	
	weightedMeanNrBCqNormSum_v2[p][s]=0.0,weightedMeanDrBCqNormSum_v2[p][s]=0.0;
	stripAsymDr_v2[p][s]=0.0,stripAsymDrEr_v2[p][s]=0.0;
	
	weightedMeanNrBCqNormDiff_v2[p][s]=0.0,weightedMeanDrBCqNormDiff_v2[p][s]=0.0;
	stripAsymNr_v2[p][s]=0.0,stripAsymNrEr_v2[p][s]=0.0;
	
	weightedMeanNrqNormB1L0_v2[p][s]=0.0,weightedMeanDrqNormB1L0_v2[p][s]=0.0;
	qNormB1L0_v2[p][s]=0.0,qNormB1L0Er_v2[p][s]=0.0;
      }
    }
  }

  Int_t activeStrips[nPlanes][nStrips];//!not using as of now
  for(Int_t p = startPlane; p < endPlane; p++) { 
    Int_t skipMe = 0;
    for (Int_t s =startStrip; s <endStrip;s++) { 
      if (maskedStrips(p,s)) continue;
      skipMe++;
      activeStrips[p][skipMe]=s+1;//this is required to keep it consistent with the case of the stripNumber being read from a file
    }
  }

  //chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_Pass1_%d.*.root",runnum));//for Run2
  chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_%d.*.root",runnum));//for myQwAnalyisis output
  printf("Attached %d files to chain for Run # %d\n",chainExists,runnum);

  if(!chainExists){//delete chains and exit if files do not exist
    cout<<"\n\n***Error: The analyzed Root file for run "<<runnum<<" does not exist***\n\n"<<endl;
    delete mpsChain;
    return -1;
  }

  infileLas.open(Form("%s/%s/%scutLas.txt",pPath,webDirectory,filePrefix.Data()));
  infileBeam.open(Form("%s/%s/%scutBeam.txt",pPath,webDirectory,filePrefix.Data()));
    
  if (infileLas.is_open() && infileBeam.is_open()) {
    cout<<"Found the cutLas and cutEB file in "<<Form("%s/%s/run_%d/",pPath,webDirectory,runnum)<<" directory."<<endl;
    while (infileLas.good()) {
      infileLas >> readEntry; //read the contents of the line in a string first
      if (readEntry.IsDigit()) { //check if the string is a combination of numbers of not
	entry = readEntry.Atoi(); //if string is a combination of numbers get the corresponding Integer of this string
	if (debug) printf("cutLas[%d]=%d\n",(Int_t)cutLas.size(),entry);
	cutLas.push_back(entry);
      }
    }
    infileLas.close();
    nLasCycles = (cutLas.size() - 2)/2;
    //nLasCycles = (cutLas.size())/2;

    while (infileBeam.good()) {
      infileBeam >> readEntry;
      if (readEntry.IsDigit()) {
	entry = readEntry.Atoi();
	if (debug) printf("cutEB[%d]=%d\n",(Int_t)cutEB.size(),entry);
	cutEB.push_back(entry);
      }
    }
    infileBeam.close();
    nBeamTrips = (cutEB.size())/2;///note: even if the beam is completely off, this value would be 0;
  } else {
    cout << "****:Atleast one of the Cut files missing"<<endl;
    cout<<"       hence executing the cut function****"<<endl;
    Int_t nLasCycBeamTrips = getEBeamLasCuts(cutLas, cutEB, mpsChain,runnum);
    if (debug) printf("nLasCycBeamTrips: %d\n",nLasCycBeamTrips);
    //nLasCycles = (cutLas.size())/2;
    nLasCycles = nLasCycBeamTrips%1000 - 1;//!?why is this superior than above
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
  mpsChain->SetBranchStatus("sca_bpm_3p02aY",1); 
  mpsChain->SetBranchStatus("sca_bpm_3p02aX",1); 
  mpsChain->SetBranchStatus("sca_bpm_3p02bY",1); 
  mpsChain->SetBranchStatus("sca_bpm_3p02bX",1); 
  mpsChain->SetBranchStatus("sca_bpm_3p03aY",1); 
  mpsChain->SetBranchStatus("sca_bpm_3p03aX",1);
  mpsChain->SetBranchStatus("sca_bpm_3c20Y",1);
  mpsChain->SetBranchStatus("sca_bpm_3c20X",1);
  mpsChain->SetBranchStatus("p*RawAc",1);
  mpsChain->SetBranchStatus("p*RawEv",1);
  mpsChain->SetBranchStatus("p*RawV1495Scaler",1);

  mpsChain->SetBranchAddress("event_number",&event_number);
  mpsChain->SetBranchAddress("pattern_number",&pattern_number);
  mpsChain->SetBranchAddress("actual_helicity",&helicity);
  mpsChain->SetBranchAddress("sca_laser_PowT",&lasPow);
  mpsChain->SetBranchAddress("sca_bcm6",&bcm);
  mpsChain->SetBranchAddress("sca_bpm_3p02aX",&bpm_3p02aX);
  mpsChain->SetBranchAddress("sca_bpm_3p02aY",&bpm_3p02aY);
  mpsChain->SetBranchAddress("sca_bpm_3p02bX",&bpm_3p02bX);
  mpsChain->SetBranchAddress("sca_bpm_3p02bY",&bpm_3p02bY);
  mpsChain->SetBranchAddress("sca_bpm_3p03aX",&bpm_3p03aX);
  mpsChain->SetBranchAddress("sca_bpm_3p03aY",&bpm_3p03aY);
  mpsChain->SetBranchAddress("sca_bpm_3c20X",&bpm_3c20X);

  if(v2processed) mpsChain->SetBranchStatus("p*RawAc_v2",1);

  for(Int_t p = 0; p <nPlanes; p++) {      
    mpsChain->SetBranchAddress(Form("p%dRawAc",p+1),&bRawAccum[p]);
    mpsChain->SetBranchAddress(Form("p%dRawV1495Scaler",p+1),&bRawScaler[p]);
    if(v2processed) mpsChain->SetBranchAddress(Form("p%dRawAc_v2",p+1),&bRawAccum_v2[p]);
  }//the branch for each plane is named from 1 to 4
  
  ///I need to open the lasCyc dependent files separately here since at every nCycle I update this file with a new entry
  ///..it should be opened before I enter the nCycle loop, and close them after coming out of the nCycle loop.

  if(lasCycPrint) {
    gSystem->mkdir(Form("%s/%s/run_%d/lasCyc",pPath,webDirectory,runnum));    

    for(Int_t p = startPlane; p <endPlane; p++) {
      for(Int_t s =startStrip; s <endStrip; s++) {
	if (maskedStrips(p,s)) continue;    
	///lasCyc based files go into a special folder named lasCyc
	lasCycAccum[p][s].open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycAccumP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1));
	lasCycScaler[p][s].open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycScalerP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1));
	if(debug1) cout<<"opened "<<Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycAccumP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1)<<endl;
      }
    }
    lasCycBCM.open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycBcmAvg.txt",pPath,webDirectory,runnum,runnum));
    lasCycLasPow.open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycAvgLasPow.txt",pPath,webDirectory,runnum,runnum));
  }

  Bool_t firstlinelasPrint[nPlanes][nStrips],firstLineLasCyc=kTRUE;
  for(Int_t p = startPlane; p <endPlane; p++) {
    for(Int_t s =startStrip; s <endStrip; s++) {
      firstlinelasPrint[p][s] = kTRUE;///this needs to be '1' for every new file
    }
  }

  for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
    if (debug) cout<<"\nStarting nCycle:"<<nCycle<<" and resetting all nCycle variables"<<endl;
    ///since this is the beginning of a new Laser cycle, and all Laser cycle based variables 
    ///..are already assigned to a permanent variable reset the LasCyc based variables
    nMpsB1H1L1= 0, nMpsB1H0L1= 0, nMpsB1H1L0= 0, nMpsB1H0L0= 0, missedLasEntries=0; 
    comptIH1L1= 0.0, comptIH0L1= 0.0, comptIH1L0= 0.0, comptIH0L0= 0.0;
    lasPowB1H1L1= 0.0, lasPowB1H0L0=0.0,lasPowB1H0L1= 0.0, lasPowB1H1L0=0.0;
    for(Int_t p = startPlane; p <nPlanes; p++) {      
      for(Int_t s = startStrip; s <endStrip; s++) {
	AccumB1H0L0[p][s] =0, AccumB1H1L0[p][s] =0, AccumB1H0L1[p][s] =0, AccumB1H1L1[p][s] =0;
	ScalerB1H0L0[p][s] =0, ScalerB1H1L0[p][s] =0, ScalerB1H0L1[p][s] =0, ScalerB1H1L1[p][s] =0;
 	LasCycAsymEr[p][s]= 0.0,LasCycAsymErSqr[p][s]= 0.0;
	if(v2processed) {
	  AccumB1H0L0_v2[p][s] =0, AccumB1H1L0_v2[p][s] =0, AccumB1H0L1_v2[p][s] =0, AccumB1H1L1_v2[p][s] =0;
	  LasCycAsymEr_v2[p][s]= 0.0,LasCycAsymErSqr_v2[p][s]= 0.0;
	}
      }
    }

    if(noiseRun) beamOn = kFALSE; //this Bool_t variable will be appropriately set by looking at the max current in this run in getEBeamLasCuts.C
    else if(nBeamTrips == 0) beamOn = kTRUE;         ///no beamtrip
    else if(nthBeamTrip < nBeamTrips) {  ///yes, we do have beamtrip(s)
      if(nthBeamTrip==0) { // haven't encountered a trip yet(special case of first trip)
	if(cutLas.at(2*nCycle+2)<cutEB.at(0)) beamOn = kTRUE; ///no beam trip till the end of THIS laser cycle
	else {                    ///there is a beam trip during this laser cycle
	  beamOn = kFALSE;
	  nthBeamTrip++;          ///encountered the first beam trip
	}
      } else if(cutLas.at(2*nCycle+2)<cutEB.at(2*nthBeamTrip-1)) beamOn=kFALSE;///continuation of the previous nthBeamTrip for current cycle
      else if(cutLas.at(2*nCycle+2)<cutEB.at(2*nthBeamTrip)) beamOn=kTRUE;///next beamTrip does not occur atleast till the end of THIS cycle
      else { ///encountered "another" beam trip	
	beamOn = kFALSE;  
	nthBeamTrip++;
      }
    } else if(nthBeamTrip == nBeamTrips) { ///encountered the last beamTrip     
      if (cutLas.at(2*nCycle) > cutEB.at(2*nthBeamTrip-1)) beamOn = kTRUE; ///current laser Cycle begins after the beamTrip recovered
      else beamOn = kFALSE;
    } else cout<<"\n***Error ... Something drastically wrong in BeamTrip evaluation***\n"<<endl;
      
    if(debug) cout<<"Will analyze from entry # "<<cutLas.at(2*nCycle)<<" to entry # "<<cutLas.at(2*nCycle+2)<<endl;

    for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+2); i++) { 
      //loop over laser cycle periods taking one LasOff state and the following laserOn state
      if(debug && i%100000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;

      if((i < cutLas.at(2*nCycle+1)) && lasPow[0]<minLasPow) l= 0; ///laser off zone
      else if((i >=cutLas.at(2*nCycle+1)) && (i <=cutLas.at(2*nCycle+2)) && (lasPow[0] > acceptLasPow)) l =1;///laser on zone
      ///the equal sign above is in laser-On zone because that's how getEBeamLasCuts currently assign it(may change!)
      else missedLasEntries++;

      mpsChain->GetEntry(i);
      h = (Int_t)helicity;
      if (beamOn) { ////currently the counters are only populated for beamOn cycles
	if (h ==0 && l ==0) {
	  nMpsB1H0L0++;
	  comptIH0L0 += bcm[0];
	  lasPowB1H0L0 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      AccumB1H0L0[p][s] += (Int_t)bRawAccum[p][s];
	      ScalerB1H0L0[p][s] += (Int_t)bRawScaler[p][s];
	      if(v2processed) AccumB1H0L0_v2[p][s] += (Int_t)bRawAccum_v2[p][s];
	    }
	  }
	} else if (h ==0 && l==1) {////the elseif statement helps avoid overhead in each entry
	  nMpsB1H0L1++;
	  comptIH0L1 += bcm[0];
	  lasPowB1H0L1 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      AccumB1H0L1[p][s] += (Int_t)bRawAccum[p][s];
	      ScalerB1H0L1[p][s] += (Int_t)bRawScaler[p][s];
	      if(v2processed) AccumB1H0L1_v2[p][s] += (Int_t)bRawAccum_v2[p][s];
	    }	  
	  }
	} else if (h ==1 && l==0) {
	  nMpsB1H1L0++;
	  comptIH1L0 += bcm[0];
	  lasPowB1H1L0 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      AccumB1H1L0[p][s] += (Int_t)bRawAccum[p][s];
	      ScalerB1H1L0[p][s] += (Int_t)bRawScaler[p][s];
	      if(v2processed) AccumB1H1L0_v2[p][s] += (Int_t)bRawAccum_v2[p][s];
	    }
	  }
	} else if (h ==1 && l==1) {
	  nMpsB1H1L1++;
	  comptIH1L1 += bcm[0];
	  lasPowB1H1L1 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      AccumB1H1L1[p][s] += (Int_t)bRawAccum[p][s];
	      ScalerB1H1L1[p][s] += (Int_t)bRawScaler[p][s];
	      if(v2processed) AccumB1H1L1_v2[p][s] += (Int_t)bRawAccum_v2[p][s];
	    }
	  }
	}
      }///if (beamOn)
    }///for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+2); i++)
    if(debug) cout<<"ignored "<<missedLasEntries<<" entries in this lasCycle"<<endl;

    //after having filled the above vectors based on laser and beam periods, find asymmetry
    if (beamOn) {
      goodCycles++;
      Float_t laserOnOffRatioH1 = (Float_t)nMpsB1H1L1/nMpsB1H1L0;
      Float_t laserOnOffRatioH0 = (Float_t)nMpsB1H0L1/nMpsB1H0L0;
      totIH1L1 += comptIH1L1;
      totIH1L0 += comptIH1L0;
      totIH0L1 += comptIH0L1;
      totIH0L0 += comptIH0L0;
      totMpsB1H1L1 += nMpsB1H1L1;
      totMpsB1H1L0 += nMpsB1H1L0;
      totMpsB1H0L1 += nMpsB1H0L1;
      totMpsB1H0L0 += nMpsB1H0L0;
      Double_t qLasCycH1L1 = comptIH1L1 /MpsRate;//q=i*t;qH1L1=(iH1L1/nMpsH1L1)*(nMpsH1L1/MpsRate);//this really gives total-charge for this laser cycle
      Double_t qLasCycH0L1 = comptIH0L1 /MpsRate;
      Double_t qLasCycH1L0 = comptIH1L0 /MpsRate;
      Double_t qLasCycH0L0 = comptIH0L0 /MpsRate;

      if (nMpsB1H0L1<= 0||nMpsB1H1L1<= 0||nMpsB1H0L0<= 0||nMpsB1H1L0<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check nMpsB1H0L1:%d,nMpsB1H1L1:%d, nMpsB1H0L0:%d, nMpsB1H1L0:%d",nCycle,nMpsB1H0L1,nMpsB1H1L1,nMpsB1H0L0,nMpsB1H1L0);
      else if (comptIH0L1<= 0||comptIH1L1<= 0||comptIH0L0<= 0||comptIH1L0<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check comptIH0L1:%f,comptIH1L1:%f, comptIH0L0:%f, comptIH1L0:%f**\n",nCycle,comptIH0L1,comptIH1L1,comptIH0L0,comptIH1L0);
      else {
	for (Int_t p =startPlane; p <endPlane; p++) {	  	  
	  for (Int_t s = startStrip; s < endStrip; s++) {
	    if (maskedStrips(p,s)) continue;
	    totAccumB1H1L1[p][s] += AccumB1H1L1[p][s];
	    totAccumB1H1L0[p][s] += AccumB1H1L0[p][s];
	    totAccumB1H0L1[p][s] += AccumB1H0L1[p][s];
	    totAccumB1H0L0[p][s] += AccumB1H0L0[p][s];
	    totScalerB1L1[p][s] += (ScalerB1H1L1[p][s]+ScalerB1H0L1[p][s]);///(Double_t)(nMpsB1H1L1+nMpsB1H0L1);
	    totScalerB1L0[p][s] += (ScalerB1H1L0[p][s]+ScalerB1H0L0[p][s]);///(Double_t)(nMpsB1H1L0+nMpsB1H0L0);
	    //if(debug) printf("s:%d\t%d\t%d\t%d\n",s+1,totScalerB1L0[p][s],ScalerB1H1L0[p][s],ScalerB1H0L0[p][s]);
 	  }
 	}
	if(lasCycPrint) {
	  if(debug1) {
	    cout<<"the Laser Cycle: "<<nCycle<<" has 'beamOn': "<<beamOn<<endl;
	    cout<<"printing variables on a per-laser-cycle basis"<<endl;
	    cout<<"laserOnOffRatioH0: "<<laserOnOffRatioH0<<" laserOnOffRatioH1: "<<laserOnOffRatioH1<<endl;
	    printf("comptIH1L1: %f\t comptIH0L1: %f\t comptIH1L0: %f\t comptIH0L0: %f\n",comptIH1L1,comptIH0L1,comptIH1L0,comptIH0L0);
	    printf("nMpsB1H1L1: %d\tnMpsB1H0L1: %d\tnMpsB1H1L0: %d\tnMpsB1H0L0: %d\n",nMpsB1H1L1,nMpsB1H0L1,nMpsB1H1L0,nMpsB1H0L0);
	  }
	  if (!firstLineLasCyc) {
	    lasCycBCM<<"\n";
	    lasCycLasPow<<"\n";
	  }
	  firstLineLasCyc = kFALSE;
	  for(Int_t p = startPlane; p < endPlane; p++) {
	    for (Int_t s =startStrip; s <endStrip;s++) {
	      if (maskedStrips(p,s)) continue;
	      if(!firstlinelasPrint[p][s]) {
		lasCycAccum[p][s]<<"\n";
		lasCycScaler[p][s]<<"\n";
	      }
	      firstlinelasPrint[p][s] =kFALSE;
	      if (lasCycAccum[p][s].is_open() && lasCycScaler[p][s].is_open()) {
		lasCycAccum[p][s]<<Form("%2.0f\t%f\t%f\t%f\t%f",(Float_t)nCycle+1,
					AccumB1H0L0[p][s]/qLasCycH0L0,AccumB1H0L1[p][s]/qLasCycH0L1,
					AccumB1H1L0[p][s]/qLasCycH1L0,AccumB1H1L1[p][s]/qLasCycH1L1);
		lasCycScaler[p][s]<<Form("%2.0f\t%f\t%f\t%f\t%f",(Float_t)nCycle+1,
					 ScalerB1H0L0[p][s]/qLasCycH0L0,ScalerB1H0L1[p][s]/qLasCycH0L1,
					 ScalerB1H1L0[p][s]/qLasCycH1L0,ScalerB1H1L1[p][s]/qLasCycH1L1);		
	      } else cout<<"\n***Alert: Couldn't open file for writing laserCycle based values\n\n"<<endl;    
	    }
	  }
	  lasCycBCM<<Form("%2.0f\t%f\t%f\t%f\t%f",(Float_t)nCycle+1,comptIH0L0/nMpsB1H0L0
			  ,comptIH0L1/nMpsB1H0L1,comptIH1L0/nMpsB1H1L0,comptIH1L1/nMpsB1H1L1);
	  lasCycLasPow<<Form("%2.0f\t%f\t%f\t%f\t%f",(Float_t)nCycle+1,lasPowB1H0L0/nMpsB1H0L0,lasPowB1H0L1/nMpsB1H0L1,lasPowB1H1L0/nMpsB1H1L0,lasPowB1H1L1/nMpsB1H1L1);
	}

	evaluateAsym(AccumB1H1L1,AccumB1H1L0,AccumB1H0L1,AccumB1H0L0,comptIH1L1,comptIH1L0,comptIH0L1,comptIH0L0,weightedMeanNrAsym,weightedMeanDrAsym,weightedMeanNrBCqNormSum,weightedMeanDrBCqNormSum,weightedMeanNrBCqNormDiff,weightedMeanNrqNormB1L0,weightedMeanDrqNormB1L0);
	if(v2processed) evaluateAsym(AccumB1H1L1_v2,AccumB1H1L0_v2,AccumB1H0L1_v2,AccumB1H0L0_v2,comptIH1L1,comptIH1L0,comptIH0L1,comptIH0L0,weightedMeanNrAsym_v2,weightedMeanDrAsym_v2,weightedMeanNrBCqNormSum_v2,weightedMeanDrBCqNormSum_v2,weightedMeanNrBCqNormDiff_v2,weightedMeanNrqNormB1L0_v2,weightedMeanDrqNormB1L0_v2);
      }///sanity check of being non-zero for filled laser cycle variables
    }///if (beamOn)
    else cout<<"this LasCyc(nCycle:"<<nCycle<<") had a beam trip(nthBeamTrip:"<<nthBeamTrip<<"), hence skipping"<<endl;
  }///for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 

  for(Int_t p = 0; p <nPlanes; p++) {      
    for(Int_t s =startStrip; s <endStrip; s++) {
      if (maskedStrips(p,s)) continue;    
      lasCycAccum[p][s].close();
      lasCycScaler[p][s].close();
    }
  }
  lasCycBCM.close();
  lasCycLasPow.close();

  for (Int_t p =startPlane; p <endPlane; p++) { ///eqn 4.17 Bevington
    for (Int_t s =startStrip; s <endStrip; s++) {        
      if (maskedStrips(p,s)) continue;
      if(weightedMeanDrAsym[p][s]==0.0) cout<<"stand. deviation in weighted Mean Asym is ZERO for p"<<p<<" s"<<s<<endl;
      else {
	stripAsym[p][s] = weightedMeanNrAsym[p][s]/weightedMeanDrAsym[p][s];
	if (weightedMeanDrAsym[p][s]<0.0) stripAsymEr[p][s] = TMath::Sqrt(-(1.0/weightedMeanDrAsym[p][s]));//!!shouldn't need to do this
	else stripAsymEr[p][s] = TMath::Sqrt(1.0/weightedMeanDrAsym[p][s]);

	stripAsymNr[p][s] = weightedMeanNrBCqNormDiff[p][s]/weightedMeanDrBCqNormSum[p][s];
	//stripAsymNrEr[p][s] = TMath::Sqrt(1/weightedMeanDrBCqNormDiff[p][s]);

	stripAsymDr[p][s] = weightedMeanNrBCqNormSum[p][s]/weightedMeanDrBCqNormSum[p][s];
	if (weightedMeanDrBCqNormSum[p][s]<0.0) stripAsymDrEr[p][s] = TMath::Sqrt(-(1.0/weightedMeanDrBCqNormSum[p][s]));
	else stripAsymDrEr[p][s] = TMath::Sqrt(1.0/weightedMeanDrBCqNormSum[p][s]);
	///the error in numerator and denominator are same, hence reevaluation is avoided

	qNormB1L0[p][s] = weightedMeanNrqNormB1L0[p][s]/weightedMeanDrqNormB1L0[p][s];///pure Beam background
	if (weightedMeanDrqNormB1L0[p][s]<0.0) qNormB1L0Er[p][s] = TMath::Sqrt(-1.0/weightedMeanDrqNormB1L0[p][s]);
	else qNormB1L0Er[p][s] = TMath::Sqrt(1.0/weightedMeanDrqNormB1L0[p][s]);
      }
      if(debug2) printf("stripAsym[%d][%d]:%f  stripAsymEr:%f\n",p,s,stripAsym[p][s],stripAsymEr[p][s]);
    }
  }
  
  for(Int_t p = startPlane; p < endPlane; p++) { 
    for (Int_t s =startStrip; s <endStrip; s++) {        
      if (maskedStrips(p,s)) continue;
      tNormScalerB1L1[p][s] = totScalerB1L1[p][s]/((Double_t)(totMpsB1H1L1 + totMpsB1H0L1)/MpsRate);//time normalized
      tNormScalerB1L0[p][s] = totScalerB1L0[p][s]/((Double_t)(totMpsB1H1L0 + totMpsB1H0L0)/MpsRate);//time normalized
   }
  }
  
  for(Int_t p = startPlane; p < endPlane; p++) { 
    outfileExpAsymP.open(Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outfileYield.open(Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outfilelasOffBkgd.open(Form("%s/%s/%slasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    fortranCheck.open(Form("%s/%s/%sfortranCheckP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    outScaler.open(Form("%s/%s/%soutScalerP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
    if (outfileExpAsymP.is_open() && outfileYield.is_open() && outfilelasOffBkgd.is_open()) {
      cout<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      cout<<Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      cout<<Form("%s/%s/%slasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      cout<<Form("%s/%s/%sfortranCheckP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      cout<<Form("%s/%s/%soutScalerP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
      fortranCheck<<totMpsB1H1L1<<"\t"<<totMpsB1H1L0<<"\t"<<totMpsB1H0L1<<"\t"<<totMpsB1H0L0<<"\t"<<totMpsB1H1L1+totMpsB1H1L0+totMpsB1H0L1+totMpsB1H0L0<<endl;
      fortranCheck<<totIH1L1<<"\t"<<totIH1L0<<"\t"<<totIH0L1<<"\t"<<totIH0L0<<"\t"<<totIH1L1+totIH1L0+totIH0L1+totIH0L0<<endl;

      //outAsymComponents.open(Form("%s/%s/%sexpAsymComponentsP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      //outfileYield<<";strip\texpAsymDr\texpAsymDrEr\texpAsymNr"<<endl;
      //outfileExpAsymP<<";strip\texpAsym\tasymEr"<<endl; ///If I want a header for the following text
      Bool_t firstOne=kTRUE;
      for (Int_t s =startStrip; s <endStrip;s++) { 
	if (maskedStrips(p,s)) continue;
	if(!firstOne) {
	  outfileExpAsymP<<"\n";
	  outfileYield<<"\n";
	  outfilelasOffBkgd<<"\n";
	  fortranCheck<<"\n";
	  outScaler<<"\n";
	}
	firstOne =kFALSE;
	outfileExpAsymP<<Form("%2.0f\t%f\t%f",(Float_t)s+1,stripAsym[p][s],stripAsymEr[p][s]);
	outfileYield<<Form("%2.0f\t%g\t%g\t%g",(Float_t)s+1,stripAsymDr[p][s],stripAsymDrEr[p][s],stripAsymNr[p][s]);
	outfilelasOffBkgd<<Form("%2.0f\t%g\t%g",(Float_t)s+1,qNormB1L0[p][s],qNormB1L0Er[p][s]);
	fortranCheck<<Form("%d\t%d\t%d\t%d\t%d",s+1,totAccumB1H1L1[p][s],totAccumB1H1L0[p][s],totAccumB1H0L1[p][s],totAccumB1H0L0[p][s]);
	outScaler<<Form("%d\t%g\t%g",s+1,tNormScalerB1L1[p][s],tNormScalerB1L0[p][s]);
      }
      outfileExpAsymP.close();
      outfileYield.close();
      outfilelasOffBkgd.close();
      fortranCheck.close();
      outScaler.close();
      cout<<Form("%s/%s/%sexpAsymP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      cout<<Form("%s/%s/%sYieldP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      cout<<Form("%s/%s/%slasOffBkgdP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      cout<<Form("%s/%s/%sfortranCheckP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      cout<<Form("%s/%s/%soutScalerP%d.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
    } else cout<<"\n***Alert: Couldn't open file for writing experimental asymmetry values\n\n"<<endl;    
  }

  if (v2processed) {
    for (Int_t p =startPlane; p <endPlane; p++) {	  	  
      for (Int_t s =startStrip; s <endStrip; s++) {        
	if (maskedStrips(p,s)) continue;
	if(weightedMeanDrAsym_v2[p][s]<=0.0) cout<<"st.deviation in weighted Mean Asym is ZERO for p"<<p<<" s"<<s<<endl;
	else if(weightedMeanNrAsym[p][s]==0.0) cout<<"asym for strip "<<s<<" in plane "<<p<<" is zero"<<endl;
	else {
	  stripAsym_v2[p][s] = weightedMeanNrAsym_v2[p][s]/weightedMeanDrAsym_v2[p][s];
	  stripAsymEr_v2[p][s] = TMath::Sqrt(1/weightedMeanDrAsym_v2[p][s]);
	    
	  stripAsymNr_v2[p][s] = weightedMeanNrBCqNormDiff_v2[p][s]/weightedMeanDrBCqNormSum_v2[p][s];
	  //stripAsymNrEr_v2[p][s] = TMath::Sqrt(1/weightedMeanDrBCqNormDiff_v2[p][s]);
	    
	  stripAsymDr_v2[p][s] = weightedMeanNrBCqNormSum_v2[p][s]/weightedMeanDrBCqNormSum_v2[p][s];
	  stripAsymDrEr_v2[p][s] = TMath::Sqrt(1/weightedMeanDrBCqNormSum_v2[p][s]);
	  ///the error in numerator and denominator are same, hence reevaluation is avoided
	    
	  qNormB1L0_v2[p][s] = weightedMeanNrqNormB1L0_v2[p][s]/weightedMeanDrqNormB1L0_v2[p][s];
	  qNormB1L0Er_v2[p][s] = TMath::Sqrt(1/weightedMeanDrqNormB1L0_v2[p][s]);
	}
      }             
      
      outfileExpAsymP.open(Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      outfileYield.open(Form("%s/%s/%sYieldP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      outfilelasOffBkgd.open(Form("%s/%s/%slasOffBkgdP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1));
      if (outfileExpAsymP.is_open() && outfileYield.is_open() && outfilelasOffBkgd.is_open()) {
	cout<<Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
	cout<<Form("%s/%s/%sYieldP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
	cout<<Form("%s/%s/%slasOffBkgdP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" file created"<<endl;
	for (Int_t s =startStrip; s <endStrip;s++) {    
	  if (maskedStrips(p,s)) continue;
	  outfileExpAsymP<<Form("%2.0f\t%f\t%f\n",(Float_t)s+1,stripAsym_v2[p][s],stripAsymEr_v2[p][s]);
	  outfileYield<<Form("%2.0f\t%g\t%g\n",(Float_t)s+1,stripAsymDr_v2[p][s],stripAsymDrEr_v2[p][s]);
	  outfilelasOffBkgd<<Form("%2.0f\t%g\t%g\n",(Float_t)s+1,qNormB1L0_v2[p][s],qNormB1L0Er_v2[p][s]);
	}
	outfileExpAsymP.close();
	cout<<Form("%s/%s/%sexpAsymP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
	outfilelasOffBkgd.close();
	cout<<Form("%s/%s/%slasOffBkgdP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
	outfileYield.close();
	cout<<Form("%s/%s/%sYieldP%d_v2.txt",pPath,webDirectory,filePrefix.Data(),p+1)<<" filled and closed"<<endl;
      } else cout<<"\n***Alert: Couldn't open file for writing experimental asymmetry values for version2(v2) data\n\n"<<endl;
    }//for(Int_t p = startPlane; p < endPlane; p++)
  } //if (v2processed)  

  //!!this currently would not work if the Cedge changes between planes

  TCanvas *cStability = new TCanvas("cStability","stability parameters",0,0,1200,900);
  cStability->Divide(3,3);
  cStability->cd(1);
  mpsChain->Draw("sca_bcm6:event_number");
  cStability->cd(2);
  mpsChain->Draw("sca_bpm_3p02aY:event_number");
  cStability->cd(3);
  mpsChain->Draw("sca_bpm_3p02bY:event_number");
  cStability->cd(4);
  mpsChain->Draw("sca_bpm_3p03aY:event_number");
  cStability->cd(5);
  mpsChain->Draw("sca_bpm_3c20Y:event_number");
  //mpsChain->Draw("sca_bpm_3c20X:event_number");
  cStability->cd(6);
  mpsChain->Draw("sca_bpm_3p02aX:event_number");
  cStability->cd(7);
  mpsChain->Draw("sca_bpm_3p02bX:event_number");
  cStability->cd(8);
  mpsChain->Draw("sca_bpm_3p03aX:event_number");
  cStability->cd(9);
  mpsChain->Draw("sca_laser_PowT:event_number");

  cStability->Update();
  cStability->SaveAs(Form("%s/%s/%sBeamStability.png",pPath,webDirectory,filePrefix.Data()));
  
  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to evaluate expAsym.\n",div_output.quot,div_output.rem );  
  return goodCycles;//the function returns the number of used Laser cycles
}
/******************************************************
!Further modifications:
* While rejecting an event due to Laser/Beam, reject the entire quartet
* If I figure out a way to normalize by laserPower then I can relax 
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
* each Laser cycle consitutes of one laser Off and one laser On period.
* ..it begins with a Laser Off period and ends with the (just)next laser On period.
* If we get too many missedEntries,check 'acceptLasPow' and 'minLasPow' variables
* The first laser-on(if the run begins with that) is ignored.
* While checking if we want to consider a laser cycle as bad, we check till 
* ..the beginning of the next laser cycle on purpose to make sure that the 
* ..beam was indeed ON during both laser On as well as Off periods
* nCycle refers to the present/current/ongoing laser-cycle
* nthBeamTrip (on the contrary) refers to the upcoming beamTrip
* the code counts from '0' but at all human interface, I add '+1' hence human counting
* Notice that my beamOff's are not true beam-off, they include ramping data as well
* Note that the qwcompton labels the beam current as "compton_charge" hence the 
* comptI**** variables contain current while iCounter**** contain charge...sorry
* Careful: there are some h=-9999 that appears in beginning of every runlet
******************************************************/
