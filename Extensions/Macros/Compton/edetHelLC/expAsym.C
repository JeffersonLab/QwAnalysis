///Author: Amrendra Narayan
///Courtesy: Don Jones seeded the code 
#include <rootClass.h>
#include "getEBeamLasCuts.C"
#include "comptonRunConstants.h"
#include "rhoToX.C"
#include "evaluateAsym.C"
#include "infoDAQ.C"
#include "weightedMean.C"
#include "writeToFile.C"
#include "qNormVariables.C"
#include "indepYield.C"//!temp
///////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton electron detector run laser wise and plots the ...
///////////////////////////////////////////////////////////////////////////

Int_t expAsym(Int_t runnum, TString dataType="Ac")
{
  cout<<"\nstarting into expAsym.C**************with dataType: "<<dataType<<"\n"<<endl;
  filePrefix= Form("run_%d/edetLasCyc_%d_",runnum,runnum);
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  const Bool_t debug = 1, debug1 = 0, debug2 = 0;
  const Bool_t lasCycPrint=1;//!if accum, scaler counts and asym are needed for every laser cycle
  Bool_t firstlinelasPrint[nPlanes][nStrips],firstLineLasCyc=kTRUE;

  Bool_t beamOn =kFALSE;//lasOn,
  Bool_t kRejectBMod = 1; //1: yes please reject; 0:Don't reject quartets during bMod ramp
  Int_t goodCycles=0,chainExists = 0, missedDueToBMod=0;
  Int_t l = 0;//lasOn tracking variables
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t nHelLCB1L1=0, nHelLCB1L0=0;
  Int_t totalMissedQuartets=0;
  Int_t entry=0; ///integer assitant in reading entry from a file
  Int_t missedLasEntries=0; ///number of missed entries due to unclear laser-state(neither fully-on,nor fully-off)
  Double_t yieldB1L1[nPlanes][nStrips], yieldB1L0[nPlanes][nStrips];
  Double_t diffB1L1[nPlanes][nStrips], diffB1L0[nPlanes][nStrips];
  Double_t bYield[nPlanes][nStrips],bDiff[nPlanes][nStrips],bAsym[nPlanes][nStrips];
  Double_t iLCH1L1=0.0,iLCH1L0=0.0,iLCH0L1=0.0,iLCH0L0=0.0;
  Double_t lasPow[3],y_bcm[3],d_bcm[3],bpm_3c20X[2],bModRamp[3];
  Double_t bpm_3p02aX[2],bpm_3p02aY[2],bpm_3p02bX[2],bpm_3p02bY[2],bpm_3p03aX[2],bpm_3p03aY[2];
  Double_t pattern_number, event_number;
  Double_t laserOnOffRatioH0;
  Double_t wmCountsNrAsym[nPlanes][nStrips],wmCountsDrAsym[nPlanes][nStrips];
  Double_t wmCountsNrBkgdAsym[nPlanes][nStrips],wmCountsDrBkgdAsym[nPlanes][nStrips];
  Double_t wmCountsNrBCqNormSum[nPlanes][nStrips],wmCountsDrBCqNormSum[nPlanes][nStrips];
  Double_t wmCountsNrBCqNormDiff[nPlanes][nStrips];
  Double_t wmCountsNrqNormB1L0[nPlanes][nStrips],wmCountsDrqNormB1L0[nPlanes][nStrips];
  
  Double_t LasCycAsymEr[nPlanes][nStrips],LasCycAsymErSqr[nPlanes][nStrips],qNormLasCycAsym[nPlanes][nStrips];
  Double_t lasPowLCB1L0=0.0,lasPowLCB1L1=0.0;

  TString readEntry;
  TChain *helChain = new TChain("Hel_Tree");//chain of run segments
  vector<Int_t>cutLas;//arrays of cuts for laser
  vector<Int_t>cutEB;//arrays of cuts for electron beam
  ofstream lasCycExpAsym,lasCycYield;
  ofstream lasCycCounts[nPlanes][nStrips],lasCycBCM,lasCycLasPow;//to write files every laserCycle
  ofstream outAsymLasCyc[nPlanes][nStrips];
  ofstream outfileExpAsymP,outfileBkgdAsymP,outfileYield,outfilelasOffBkgd,fortranCheck;
  ifstream infileLas, infileBeam;

  gSystem->mkdir(Form("%s/%s/run_%d",pPath,webDirectory,runnum));
  if (kRejectBMod) cout<<red<<"quartets during beam modulation ramp rejected"<<normal<<endl;
  else cout<<red<<"quartets during beam modulation ramp NOT rejected"<<normal<<endl;

  if(!maskSet) infoDAQ(runnum); //if the masks are not set yet, call the function to set it

  ///following variables are not to be reset every laser-cycle hence lets initialize with zero
  ///some of the variables declared in the compton header file
  for(Int_t p = startPlane; p <endPlane; p++) {      	
    for(Int_t s =startStrip; s <endStrip; s++) {
      //if (!mask[p][s]) continue;    
      wmCountsNrAsym[p][s]=0.0,wmCountsDrAsym[p][s]=0.0;
      wmCountsNrBkgdAsym[p][s]=0.0,wmCountsDrBkgdAsym[p][s]=0.0;
      wmCountsNrBCqNormSum[p][s]=0.0,wmCountsDrBCqNormSum[p][s]=0.0;
      wmCountsNrBCqNormDiff[p][s]=0.0;
      wmCountsNrqNormB1L0[p][s]=0.0,wmCountsDrqNormB1L0[p][s]=0.0;
 
      stripAsym[p][s]= 0.0,stripAsymEr[p][s]= 0.0;
      stripAsymDr[p][s]=0.0,stripAsymDrEr[p][s]=0.0;
      stripAsymNr[p][s]=0.0,stripAsymNrEr[p][s]=0.0;
      bkgdAsym[p][s]= 0.0,bkgdAsymEr[p][s]= 0.0;

      qNormB1L0[p][s]=0.0,qNormB1L0Er[p][s]=0.0;
      qNormCountsB1L1[p][s]=0.0,qNormCountsB1L0[p][s]=0.0;
      //qNormB1L1[p][s]=0.0,qNormB1L0[p][s]=0.0;
      qNormLasCycAsym[p][s]=0.0,LasCycAsymErSqr[p][s]=0.0;

      totyieldB1L1[p][s]=0,totyieldB1L0[p][s]=0;
    }
  }

  Int_t activeStrips[nPlanes][nStrips];//!not using as of now
  for(Int_t p = startPlane; p < endPlane; p++) { 
    Int_t skipMe = 0;
    for (Int_t s =startStrip; s <endStrip;s++) { 
      if (!mask[p][s]) continue;
      skipMe++;
      activeStrips[p][skipMe]=s+1;//this is required to keep it consistent with the case of the stripNumber being read from a file
    }
  }

  chainExists = helChain->Add(Form("$QW_ROOTFILES/Compton_Pass2b_%d.*.root",runnum));//for pass2b
  //chainExists = helChain->Add(Form("$QW_ROOTFILES/Compton_%d.*.root",runnum));//for myQwAnalyisis output
  cout<<"Attached "<<chainExists<<" files to chain for Run # "<<runnum<<endl;

  if(!chainExists){//delete chains and exit if files do not exist
    cout<<"\n\n***Error: The analyzed Root file for run "<<runnum<<" does not exist***\n\n"<<endl;
    delete helChain;
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
	if (debug) cout<<Form("cutLas[%d]=%d\n",(Int_t)cutLas.size(),entry);
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
	if (debug) cout<<Form("cutEB[%d]=%d\n",(Int_t)cutEB.size(),entry);
	cutEB.push_back(entry);
      }
    }
    infileBeam.close();
    nBeamTrips = (cutEB.size())/2;///note: even if the beam is completely off, this value would be 0;
  } else {
    cout << "****:Atleast one of the Cut files missing"<<endl;
    cout<<"       hence executing the cut function****"<<endl;
    Int_t nLasCycBeamTrips = getEBeamLasCuts(cutLas, cutEB, helChain,runnum);
    if (debug) cout<<Form("nLasCycBeamTrips: %d\n",nLasCycBeamTrips);
    //nLasCycles = (cutLas.size())/2;
    nLasCycles = nLasCycBeamTrips%1000 - 1;//!?why is this superior than above
    ////first two digits of return value of getEBeamLasCuts
    nBeamTrips = nLasCycBeamTrips / 1000;
    ////fourth digit of return value of getEBeamLasCuts
  }
  
  if (debug) cout<<Form("cutEB.size:%d,cutLas.size:%d\n",(Int_t)cutEB.size(),(Int_t)cutLas.size());

  Int_t nEntries = helChain->GetEntries();
  cout<<blue<<"This chain has "<<nEntries<<" entries"<<endl;
  cout<<"nbeamTrips="<<nBeamTrips<<endl;
  cout<<"nLasCycles="<<nLasCycles<<normal<<endl;
  
  helChain->ResetBranchAddresses();//!? should it be here?
  helChain->SetBranchStatus("*",0);  ////Turn off all unused branches, for efficient looping
  helChain->SetBranchStatus("CodaEventNumber",1);
  helChain->SetBranchStatus("pattern_number",1);
  helChain->SetBranchStatus("yield_sca_laser_PowT",1);
  helChain->SetBranchStatus("yield_sca_bcm6",1);
  helChain->SetBranchStatus("diff_sca_bcm6",1);
  helChain->SetBranchStatus("yield_sca_bpm_3p02aY",1); 
  helChain->SetBranchStatus("yield_sca_bpm_3p02aX",1); 
  helChain->SetBranchStatus("yield_sca_bpm_3p02bY",1); 
  helChain->SetBranchStatus("yield_sca_bpm_3p02bX",1); 
  helChain->SetBranchStatus("yield_sca_bpm_3p03aY",1); 
  helChain->SetBranchStatus("yield_sca_bpm_3p03aX",1);
  helChain->SetBranchStatus("yield_sca_bpm_3c20Y",1);
  helChain->SetBranchStatus("yield_sca_bpm_3c20X",1);
  helChain->SetBranchStatus("yield_sca_bmod_ramp",1);

  if (dataType == "Ev") {
    helChain->SetBranchStatus("yield_p*RawEv",1);
    helChain->SetBranchStatus("diff_p*RawEv",1);
    helChain->SetBranchStatus("asym_p*RawEv",1);
    for(Int_t p = 0; p <nPlanes; p++) {      
      helChain->SetBranchAddress(Form("yield_p%dRawEv",p+1),&bYield[p]);
      helChain->SetBranchAddress(Form("diff_p%dRawEv",p+1),&bDiff[p]);
      helChain->SetBranchAddress(Form("asym_p%dRawEv",p+1),&bAsym[p]);
    }
  } else if (dataType == "Sc") {
    helChain->SetBranchStatus("yield_p*RawV1495Scaler",1);
    helChain->SetBranchStatus("diff_p*RawV1495Scaler",1);
    helChain->SetBranchStatus("asym_p*RawV1495Scaler",1);
    for(Int_t p = 0; p <nPlanes; p++) {      
      helChain->SetBranchAddress(Form("yield_p%dRawV1495Scaler",p+1),&bYield[p]);
      helChain->SetBranchAddress(Form("diff_p%dRawV1495Scaler",p+1),&bDiff[p]);
      helChain->SetBranchAddress(Form("asym_p%dRawV1495Scaler",p+1),&bAsym[p]);
    }
  } else if(dataType == "AcV2") {
    helChain->SetBranchStatus("yield_p*RawAc_v2",1);//!there is ambiguity in this
    helChain->SetBranchStatus("diff_p*RawAc_v2",1);//!there is ambiguity in this
    helChain->SetBranchStatus("asym_p*RawAc_v2",1);//!there is ambiguity in this
    for(Int_t p = 0; p <nPlanes; p++) {      
      helChain->SetBranchAddress(Form("yield_p%dRawAc_v2",p+1),&bYield[p]);
      helChain->SetBranchAddress(Form("diff_p%dRawAc_v2",p+1),&bDiff[p]);
      helChain->SetBranchAddress(Form("asym_p%dRawAc_v2",p+1),&bAsym[p]);
    }
  } else if(dataType == "Ac") {
    helChain->SetBranchStatus("yield_p*RawAc",1);
    helChain->SetBranchStatus("diff_p*RawAc",1);
    helChain->SetBranchStatus("asym_p*RawAc",1);
    for(Int_t p = 0; p <nPlanes; p++) {      
      helChain->SetBranchAddress(Form("yield_p%dRawAc",p+1),&bYield[p]);
      helChain->SetBranchAddress(Form("diff_p%dRawAc",p+1),&bDiff[p]);
      helChain->SetBranchAddress(Form("asym_p%dRawAc",p+1),&bAsym[p]);
    }//the branch for each plane is named from 1 to 4
  } else cout<<red<<"dataType not defined clearly"<<normal<<endl;

  helChain->SetBranchAddress("mps_counter",&event_number);
  helChain->SetBranchAddress("pattern_number",&pattern_number);
  helChain->SetBranchAddress("yield_sca_laser_PowT",&lasPow);
  helChain->SetBranchAddress("yield_sca_bcm6",&y_bcm);
  helChain->SetBranchAddress("diff_sca_bcm6",&d_bcm);
  helChain->SetBranchAddress("yield_sca_bpm_3p02aX",&bpm_3p02aX);
  helChain->SetBranchAddress("yield_sca_bpm_3p02aY",&bpm_3p02aY);
  helChain->SetBranchAddress("yield_sca_bpm_3p02bX",&bpm_3p02bX);
  helChain->SetBranchAddress("yield_sca_bpm_3p02bY",&bpm_3p02bY);
  helChain->SetBranchAddress("yield_sca_bpm_3p03aX",&bpm_3p03aX);
  helChain->SetBranchAddress("yield_sca_bpm_3p03aY",&bpm_3p03aY);
  helChain->SetBranchAddress("yield_sca_bpm_3c20X",&bpm_3c20X);
  helChain->SetBranchAddress("yield_sca_bmod_ramp",&bModRamp);
  ///I need to open the lasCyc dependent files separately here since at every nCycle I update this file with a new entry
  ///..it should be opened before I enter the nCycle loop, and close them after coming out of the nCycle loop.
  if(lasCycPrint) {
    gSystem->mkdir(Form("%s/%s/run_%d/lasCyc",pPath,webDirectory,runnum));    
    for(Int_t p = startPlane; p <endPlane; p++) {
      for(Int_t s =startStrip; s <endStrip; s++) {
	if (!mask[p][s]) continue;    
	///lasCyc based files go into a special folder named lasCyc
	lasCycCounts[p][s].open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_"+dataType+"LasCycP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1));
	if(debug2) cout<<"opened "<<Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_"+dataType+"LasCycP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1)<<endl;
	///Lets open the files for writing asymmetries in the beamOn loop repeatedly
  	outAsymLasCyc[p][s].open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_"+dataType+"AsymPerLasCycP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1));
	if(debug2) cout<<"opened "<<Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_"+dataType+"AsymPerLasCycP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1)<<endl;
      }
    }
    lasCycBCM.open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycBcmAvg.txt",pPath,webDirectory,runnum,runnum));
    lasCycLasPow.open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_lasCycAvgLasPow.txt",pPath,webDirectory,runnum,runnum));  

    for(Int_t p = startPlane; p <endPlane; p++) {
      for(Int_t s =startStrip; s <endStrip; s++) {
	firstlinelasPrint[p][s] = kTRUE;///this needs to be '1' for every new file
      }
    }
  }

  for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
    if (debug) cout<<"\nStarting nCycle:"<<nCycle+1<<" and resetting all nCycle variables"<<endl;
    ///since this is the beginning of a new Laser cycle, and all Laser cycle based variables 
    ///..are already assigned to a permanent variable reset the LasCyc based variables
    nHelLCB1L1= 0, nHelLCB1L0= 0, missedLasEntries=0,missedDueToBMod=0; 
    iLCH1L1= 0.0, iLCH1L0= 0.0, iLCH0L1= 0.0, iLCH0L0= 0.0;
    lasPowLCB1L0=0.0,lasPowLCB1L1= 0.0;
    for(Int_t p = startPlane; p <nPlanes; p++) {      
      for(Int_t s = startStrip; s <endStrip; s++) {
	yieldB1L0[p][s] =0.0, yieldB1L1[p][s] =0.0;
	diffB1L0[p][s] =0.0, diffB1L1[p][s] =0.0;
 	LasCycAsymEr[p][s]= 0.0,LasCycAsymErSqr[p][s]= 0.0,qNormLasCycAsym[p][s]=0.0;
      }
    }
    // if(nCycle==17) {
    //   cout<<"\n"<<red<<"skipping laser cycle # "<<nCycle+1<<normal<<endl;
    //   continue;
    // }
    if(noiseRun) beamOn = kFALSE; //this Bool_t variable will be appropriately set by looking at the max current in this run in getEBeamLasCuts.C
    else if(nBeamTrips == 0) beamOn = kTRUE;         ///no beamtrip
    else if(nthBeamTrip < nBeamTrips) {  ///yes, we do have beamtrip(s)
      if(nthBeamTrip==0) { // haven't encountered a trip yet(special case of first trip)
	if(cutLas.at(2*nCycle+2)<cutEB.at(0)) beamOn = kTRUE; ///no beam trip till the end of THIS laser cycle
	else {                    ///there is a beam trip during this laser cycle
	  beamOn = kFALSE;
	  nthBeamTrip++;          ///encountered the first beam trip
	}
      } else if(cutLas.at(2*nCycle)<cutEB.at(2*nthBeamTrip-1)) {///the new lasCyc begins before the end of previous beamTrip
	beamOn=kFALSE;///continuation of the previous nthBeamTrip for current cycle
	if(debug1) cout<<"continuation of the nthBeamTrip:"<<nthBeamTrip<<" for lasCyc:"<<nCycle+1<<endl;
      } else if(cutLas.at(2*nCycle+2)<cutEB.at(2*nthBeamTrip)) {
	beamOn=kTRUE;///next beamTrip does not occur atleast till the end of THIS cycle
	if(debug1) cout<<"next beamTrip does not occur atleast till the end of THIS cycle"<<endl;
      } else { ///encountered "another" beam trip	
	beamOn = kFALSE;  
	nthBeamTrip++;
	cout<<"encountered a new beam trip in lasCyc: "<<nCycle+1<<endl;
      }
    } else if(nthBeamTrip == nBeamTrips) { ///encountered the last beamTrip     
      if (cutLas.at(2*nCycle) > cutEB.at(2*nthBeamTrip-1)) beamOn = kTRUE; ///current laser Cycle begins after the beamTrip recovered
      else beamOn = kFALSE;
    } else cout<<red<<"\n***Error ... Something drastically wrong in BeamTrip evaluation***\n"<<normal<<endl;
      
    if(debug) cout<<"Will analyze from entry # "<<cutLas.at(2*nCycle)<<" to entry # "<<cutLas.at(2*nCycle+2)<<endl;

    for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+2); i++) { 
      //loop over laser cycle periods taking one LasOff state and the following laserOn state
      if(debug1 && i%100000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;

      if((i < cutLas.at(2*nCycle+1)) && lasPow[0]<minLasPow) l= 0; ///laser off zone
      else if((i > cutLas.at(2*nCycle+1)) && (lasPow[0]>acceptLasPow)) l =1;///laser on zone
      ///the equal sign above is in laser-On zone because that's how getEBeamLasCuts currently assign it(may change!)
      else missedLasEntries++;

      helChain->GetEntry(i);
      //      if (kRejectBMod && (bModRamp[0]<100.0)) continue; 
      if (beamOn) { ////currently the counters are only populated for beamOn cycles
	if (kRejectBMod && (bModRamp[0]>100.0)) {
	  missedDueToBMod++; 
	  continue;
	}
	if (l==0) {//using the H=0 configuration for now
	  nHelLCB1L0++;
	  iLCH1L0 += (y_bcm[0]+d_bcm[0]);///@if the factor '2' is multiplied, that would count the bcm reading once for each of the two H1 measurements; without the factor, this is already an average of the two counts that were recorded during the two H1 events in a given quartet
	  iLCH0L0 += (y_bcm[0]-d_bcm[0]);
	  lasPowLCB1L0 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (!mask[p][s]) continue;
	      yieldB1L0[p][s] += bYield[p][s];
	      diffB1L0[p][s]  += bDiff[p][s];
	    }
	  }
	} else if (l==1) {////the elseif statement helps avoid overhead in each entry
	  nHelLCB1L1++;
	  iLCH1L1 += (y_bcm[0]+d_bcm[0]);
	  iLCH0L1 += (y_bcm[0]-d_bcm[0]);
	  lasPowLCB1L1 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (!mask[p][s]) continue;
	      yieldB1L1[p][s] += bYield[p][s];
	      diffB1L1[p][s]  += bDiff[p][s];
	    }
	  }
	}
      }///if (beamOn)
    }///for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+2); i++)
    if(debug) cout<<"had to ignore "<<((Double_t)missedLasEntries/(cutLas.at(2*nCycle+2) - cutLas.at(2*nCycle)))*100.0<<" % entries in this lasCycle"<<endl;

    totalMissedQuartets += missedDueToBMod++;
    //after having filled the above vectors based on laser and beam periods, find asymmetry for this laser cycle
    if (beamOn) {
      goodCycles++;
      if(kRejectBMod) cout<<blue<<"no.of quartets missed for BMod ramp this cycle: "<<missedDueToBMod<<normal<<endl;
      laserOnOffRatioH0 = (Double_t)nHelLCB1L1/nHelLCB1L0;
      totIAllL1 += iLCH1L1;//this tot* variable is not for anything serious, hence not bothering to tune it
      totIAllL0 += iLCH1L0;
      totHelB1L1 += nHelLCB1L1;
      totHelB1L0 += nHelLCB1L0;
      if (nHelLCB1L1<= 0||nHelLCB1L0<= 0) {
	printf("\n%s****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check nHelLCB1L1:%d, nHelLCB1L0:%d%s",red,nCycle,nHelLCB1L1,nHelLCB1L0,normal);
      } else if (iLCH1L1<= 0||iLCH1L0<= 0||iLCH0L1<= 0||iLCH0L0<= 0)
	printf("\n%s****  Warning: Something drastically wrong in nCycle:%d\n** check iLCH1L1:%f, iLCH1L0:%f, iLCH0L1:%f, iLCH0L0:%f**%s\n",red,nCycle,iLCH1L1,iLCH1L0,iLCH0L1,iLCH0L0,normal);
      else {
	for (Int_t p =startPlane; p <endPlane; p++) {
	  for (Int_t s = startStrip; s < endStrip; s++) {
	    if (!mask[p][s]) continue;
	    totyieldB1L1[p][s] += yieldB1L1[p][s];
	    totyieldB1L0[p][s] += yieldB1L0[p][s];
 	  }
 	}

	if(debug) cout<<red<<nCycle+1<<"\t"<<nHelLCB1L1<<"\t"<< nHelLCB1L0<<"\t"<<iLCH1L1<<"\t"<<iLCH1L0<<"\t"<<iLCH0L1<<"\t"<<iLCH0L0<<normal<<endl;
	//indepYield(yieldB1L1, yieldB1L0, nHelLCB1L1, nHelLCB1L0, iLCH1L1, iLCH1L0);//was invoked for checking
	///now lets do a yield weighted mean of the above asymmetry
	evaluateAsym(diffB1L1,diffB1L0,yieldB1L1,yieldB1L0,iLCH1L1,iLCH1L0,iLCH0L1,iLCH0L0,wmCountsNrAsym,wmCountsDrAsym,wmCountsNrBCqNormSum,wmCountsDrBCqNormSum,wmCountsNrBCqNormDiff,wmCountsNrqNormB1L0,wmCountsDrqNormB1L0,wmCountsNrBkgdAsym,wmCountsDrBkgdAsym,qNormLasCycAsym,LasCycAsymErSqr);
  
	if(lasCycPrint) {
	  //q=i*t;qH1L1=(iH1L1/nMpsH1L1)*(nMpsH1L1/MpsRate);//this really gives total-charge for this laser cycle
	  Double_t qLasCycL1 = iLCH1L1 /helRate;
	  Double_t qLasCycL0 = iLCH1L0 /helRate;
	  //!!check the above "average charge", it doesn't seem the appropriate way of averaging
	  if(debug1) {
	    cout<<"the Laser Cycle: "<<nCycle+1<<" has 'beamOn': "<<beamOn<<endl;
	    cout<<"printing variables on a per-laser-cycle basis"<<endl;
	    cout<<"laserOnOffRatioH0: "<<laserOnOffRatioH0<<endl;
	    cout<<Form("iLCH1L1: %f\t iLCH1L0: %f",iLCH1L1,iLCH1L0)<<endl;
	  }	   
	  if (!firstLineLasCyc) {
	    lasCycBCM<<"\n";
	    lasCycLasPow<<"\n";
	  }
	  firstLineLasCyc = kFALSE;
	  for(Int_t p = startPlane; p < endPlane; p++) {
	    for (Int_t s =startStrip; s <endStrip;s++) {
	      if (!mask[p][s]) continue;
	      if(!firstlinelasPrint[p][s]) {
		lasCycCounts[p][s]<<"\n";
		outAsymLasCyc[p][s]<<"\n";
	      }
	      firstlinelasPrint[p][s] =kFALSE;
	      if (lasCycCounts[p][s].is_open()) {
		lasCycCounts[p][s]<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,yieldB1L0[p][s]/qLasCycL0,yieldB1L1[p][s]/qLasCycL1);
	      } else cout<<"\n***Alert: Couldn't open file for writing laserCycle based values\n\n"<<endl;  
	      if (outAsymLasCyc[p][s].is_open()) {
		outAsymLasCyc[p][s]<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,qNormLasCycAsym[p][s],TMath::Sqrt(LasCycAsymErSqr[p][s]));
	      } else cout<<"\n***Alert: Couldn't open file for writing asymmetry per laser cycle per strip\n\n"<<endl;
	    }
	  }
	  lasCycBCM<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,(iLCH1L0+iLCH0L0)/nHelLCB1L0,(iLCH1L1+iLCH0L1)/nHelLCB1L1);
	  lasCycLasPow<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,lasPowLCB1L0/nHelLCB1L0,lasPowLCB1L1/nHelLCB1L1);
	}///if(lasCycPrint)

      }///sanity check of being non-zero for filled laser cycle variables
    }///if (beamOn)
    else cout<<"this LasCyc: "<<nCycle+1<<" had a beam trip(nthBeamTrip:"<<nthBeamTrip<<"), hence skipping"<<endl;
  }///for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
  
  for(Int_t p = 0; p <nPlanes; p++) {      
    for(Int_t s =startStrip; s <endStrip; s++) {
      if (!mask[p][s]) continue;    
      lasCycCounts[p][s].close();
      outAsymLasCyc[p][s].close();
    }
  }
  lasCycBCM.close();
  lasCycLasPow.close();

  cout<<red<<"the dataType is set to :"<<dataType<<normal<<endl;
  //notice that the variables to be written by the writeToFile command is updated after every call of weightedMean() function

  weightedMean(wmCountsNrAsym, wmCountsDrAsym, wmCountsNrBCqNormSum, wmCountsDrBCqNormSum, wmCountsNrBCqNormDiff, wmCountsNrqNormB1L0, wmCountsDrqNormB1L0, wmCountsNrBkgdAsym, wmCountsDrBkgdAsym);
  qNormVariables(totyieldB1L0,totyieldB1L1,totIAllL0,totIAllL1);


  writeToFile(runnum,dataType);

  //!!this currently would not work if the Cedge changes between planes
  if(debug1) {
    TCanvas *cStability = new TCanvas("cStability","stability parameters",0,0,1200,900);
    cStability->Divide(3,3);
    cStability->cd(1);
    helChain->Draw("sca_bcm6:event_number","sca_bcm6 < 200");
    cStability->cd(2);
    helChain->Draw("sca_bpm_3p02aY:event_number");
    cStability->cd(3);
    helChain->Draw("sca_bpm_3p02bY:event_number");
    cStability->cd(4);
    helChain->Draw("sca_bpm_3p03aY:event_number");
    cStability->cd(5);
    helChain->Draw("sca_bpm_3c20Y:event_number");
    //helChain->Draw("sca_bpm_3c20X:event_number");
    cStability->cd(6);
    helChain->Draw("sca_bpm_3p02aX:event_number");
    cStability->cd(7);
    helChain->Draw("sca_bpm_3p02bX:event_number");
    cStability->cd(8);
    helChain->Draw("sca_bpm_3p03aX:event_number");
    cStability->cd(9);
    helChain->Draw("sca_laser_PowT:event_number","sca_laser_PowT<180000");

    cStability->Update();
    cStability->SaveAs(Form("%s/%s/%sBeamStability.png",pPath,webDirectory,filePrefix.Data()));
  }
  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to evaluate expAsym.\n",div_output.quot,div_output.rem );  
  cout<<blue<<"total no.of quartets ignored due to Beam Mod : "<<totalMissedQuartets<<normal<<endl;
  cout<<"the no.of used laser cycles in this run was "<<goodCycles<<normal<<endl;
  return goodCycles;//the function returns the number of used Laser cycles
}
/******************************************************
!Further modifications:
* While rejecting an event due to Laser/Beam, reject the entire quartet
* If I figure out a way to normalize by laserPower then I can relax 
*..the condition of laserPower being 90% of maximum for the laser entries to be accepted.
* plot qNormAcB1H1L0LasCyc and qNormyieldB1L0LasCyc against
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
* wm: weightedMean, Ac: Accum, Sc: Scaler, Ev: Event
* each Laser cycle consitutes of one laser Off zone followed by one laser On period.
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
* Careful: there are some h=-9999 that appears in beginning of every runlet
* Laser power is uncorrelated with beam helicity hence not using its difference 
* ..to construct the true laser power but simply using the yield for laser power.
******************************************************/
