///Author: Amrendra Narayan
///Courtesy: Don Jones seeded the code 
#include "rootClass.h"
#include "getEBeamLasCuts.C"
#include "comptonRunConstants.h"
#include "rhoToX.C"
#include "evaluateAsym.C"
#include "infoDAQ.C"
#include "weightedMean.C"
#include "writeToFile.C"
#include "qNormVariables.C"
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
  const Bool_t lasCycPrint=0;//!if accum, scaler counts and asym are needed for every laser cycle
  const Bool_t kRejectBMod = 1; //1: yes please reject; 0:Don't reject quartets during bMod ramp
  const Bool_t kNoiseSub = 1;
  const Bool_t kDeadTime = 1;
  Bool_t firstlinelasPrint[nPlanes][nStrips],firstLineLasCyc=kTRUE;
  Bool_t beamOn =kFALSE;//lasOn,
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
  Double_t lasPow[3],y_bcm[3],d_bcm[3],bpm_3c20X[2],bModRamp[3];//?!the vector size should be 3 I suppose, not 4
  //Double_t lasPow[2],y_bcm[2],d_bcm[2],bpm_3c20X[1],bModRamp[2];
  Double_t bpm_3p02aX[2],bpm_3p02aY[2],bpm_3p02bX[2],bpm_3p02bY[2],bpm_3p03aX[2],bpm_3p03aY[2];
  //Double_t bpm_3p02aX[1],bpm_3p02aY[1],bpm_3p02bX[1],bpm_3p02bY[1],bpm_3p03aX[1],bpm_3p03aY[1];
  Double_t pattern_number, event_number;
  Double_t laserOnOffRatioH0;
  Double_t countsLCB1H1L1[nPlanes][nStrips],countsLCB1H1L0[nPlanes][nStrips],countsLCB1H0L1[nPlanes][nStrips],countsLCB1H0L0[nPlanes][nStrips];
  Double_t wmCountsNrAsym[nPlanes][nStrips],wmCountsDrAsym[nPlanes][nStrips];
  Double_t wmCountsNrBkgdAsym[nPlanes][nStrips],wmCountsDrBkgdAsym[nPlanes][nStrips];
  Double_t wmCountsNrBCqNormSum[nPlanes][nStrips],wmCountsDrBCqNormSum[nPlanes][nStrips];
  Double_t wmCountsNrBCqNormDiff[nPlanes][nStrips];
  Double_t wmCountsNrqNormB1L0[nPlanes][nStrips],wmCountsDrqNormB1L0[nPlanes][nStrips];

  Double_t asymErLC[nPlanes][nStrips],asymErSqrLC[nPlanes][nStrips],qNormAsymLC[nPlanes][nStrips];
  Double_t lasPowLCB1L0=0.0,lasPowLCB1L1=0.0;

  TString readEntry;
  TChain *helChain = new TChain("Hel_Tree");//chain of run segments
  vector<Int_t>cutLas;//arrays of cuts for laser
  vector<Int_t>cutEB;//arrays of cuts for electron beam
  ofstream lasCycExpAsym,lasCycYield;
  ofstream countsLC[nPlanes][nStrips],lasCycBCM,lasCycLasPow;//to write files every laserCycle
  ofstream outAsymLasCyc[nPlanes][nStrips];
  ofstream outfileExpAsymP,outfileBkgdAsymP,outfileYield,outfilelasOffBkgd,fortranCheck;
  ifstream infileLas, infileBeam, fileNoise;

  gSystem->mkdir(Form("%s/%s/run_%d",pPath,webDirectory,runnum));
  if (kRejectBMod) cout<<green<<"quartets during beam modulation ramp rejected"<<normal<<endl;
  else cout<<green<<"quartets during beam modulation ramp NOT rejected"<<normal<<endl;

  if(!maskSet) infoDAQ(runnum); //if the masks are not set yet, call the function to set it

  ///following variables are not to be reset every laser-cycle hence lets initialize with zero
  ///some of the variables declared in the compton header file
  for(Int_t p = startPlane; p <endPlane; p++) {      	
    for(Int_t s =startStrip; s <endStrip; s++) {
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
      qNormCountsB1L1[p][s]=0.0,qNormCountsB1L1Er[p][s]=0.0;
      qNormCountsB1L0[p][s]=0.0,qNormCountsB1L0Er[p][s]=0.0;
      qNormAsymLC[p][s]=0.0,asymErSqrLC[p][s]=0.0;

      totyieldB1L1[p][s]=0,totyieldB1L0[p][s]=0;//this can be removed after the variables below do the requisite
      totyieldB1H1L1[p][s]=0,totyieldB1H1L0[p][s]=0,totyieldB1H0L1[p][s]=0,totyieldB1H0L0[p][s]=0;      
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
  //////////////for a quick peep of the beam and laser stability of beam//////////
  if(debug1) {
    TCanvas *c1 = new TCanvas("c1"," ",0,0,900,900);
    c1->Divide(1,2);
    c1->cd(1);
    helChain->Draw("yield_sca_laser_PowT:pattern_number");    
    c1->cd(2);
    helChain->Draw("yield_sca_bcm6:pattern_number");
    c1->SaveAs(Form("%s/%s/%slasBeamStability.png",pPath,webDirectory,filePrefix.Data()));
  }
  //////////////////////////////////////////////////////////////////////////////

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

  ///////////// Implementing noise subtraction //////
  ///Reading in the noise file
  if(kNoiseSub) {
    cout<<blue<<"\nNoise subtraction turned ON for this analysis\n"<<normal<<endl;
    Double_t tB0H1L1,tB0H1L0,tB0H0L1,tB0H0L0,tBeamOff;
    Double_t lasPowB0H1L1, lasPowB0H1L0, lasPowB0H0L1, lasPowB0H0L0, tBeamOn;
    Double_t rateB0H1L1[nStrips],rateB0H1L0[nStrips],rateB0H0L1[nStrips],rateB0H0L0[nStrips],strip[nStrips];
    fileNoise.open(Form("%s/data/beamoff_23229.0.dat",pPath));
    if(fileNoise.is_open()) {
      cout<<blue<<"explicit noise subtraction"<<normal<<endl; 
      fileNoise>>tB0H1L1>>tB0H1L0>>tB0H0L1>>tB0H0L0>>tBeamOff;
      fileNoise>>lasPowB0H1L1>>lasPowB0H1L0>>lasPowB0H0L1>>lasPowB0H0L0>>tBeamOn;
      timeB0 = tB0H1L1+tB0H1L0+tB0H0L1+tB0H0L0;
      for(Int_t s=0;s<endStrip;s++) {
        fileNoise>>rateB0H1L1[s]>>rateB0H1L0[s]>>rateB0H0L1[s]>>rateB0H0L0[s]>>strip[s];
        rateB0[s]= (rateB0H1L1[s]+rateB0H1L0[s]+rateB0H0L1[s]+rateB0H0L0[s])*4.0/timeB0;
        //printf("%f\t%f\t%f\t%f\t%f\n",rateB0H1L1[s],rateB0H1L0[s],rateB0H0L1[s],rateB0H0L0[s],strip[s]);
        if(debug2) printf("%f\t%f\n",strip[s],rateB0[s]);
      }
    } else cout<<red<<"couldn't open the beam off file"<<normal<<endl;
  } else cout<<blue<<"\nNoise subtraction turned OFF for this analysis\n"<<normal<<endl; 
  ///////////////////////////////////////////////////

  //helChain->ResetBranchAddresses();//!? should it be here?
  helChain->SetBranchStatus("*",0);  ////Turn off all unused branches, for efficient looping
  //helChain->SetBranchStatus("CodaEventNumber",1);
  helChain->SetBranchStatus("pattern_number",1);
  helChain->SetBranchStatus("yield_sca_laser_PowT*",1);
  helChain->SetBranchStatus("yield_sca_bcm6*",1);
  helChain->SetBranchStatus("diff_sca_bcm6*",1);
  helChain->SetBranchStatus("yield_sca_bpm_3p02aY*",1); 
  helChain->SetBranchStatus("yield_sca_bpm_3p02aX*",1); 
  helChain->SetBranchStatus("yield_sca_bpm_3p02bY*",1); 
  helChain->SetBranchStatus("yield_sca_bpm_3p02bX*",1); 
  helChain->SetBranchStatus("yield_sca_bpm_3p03aY*",1); 
  helChain->SetBranchStatus("yield_sca_bpm_3p03aX*",1);
  helChain->SetBranchStatus("yield_sca_bpm_3c20Y*",1);
  helChain->SetBranchStatus("yield_sca_bpm_3c20X*",1);
  helChain->SetBranchStatus("yield_sca_bmod_ramp*",1);

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

  if (dataType == "Ev") {
    helChain->SetBranchStatus("yield_p*RawEv*",1);
    helChain->SetBranchStatus("diff_p*RawEv*",1);
    helChain->SetBranchStatus("asym_p*RawEv*",1);
    for(Int_t p = 0; p <nPlanes; p++) {      
      helChain->SetBranchAddress(Form("yield_p%dRawEv",p+1),&bYield[p]);
      helChain->SetBranchAddress(Form("diff_p%dRawEv",p+1),&bDiff[p]);
      helChain->SetBranchAddress(Form("asym_p%dRawEv",p+1),&bAsym[p]);
    }
  } else if (dataType == "Sc") {
    helChain->SetBranchStatus("yield_p*RawV1495Scaler*",1);
    helChain->SetBranchStatus("diff_p*RawV1495Scaler*",1);
    helChain->SetBranchStatus("asym_p*RawV1495Scaler*",1);
    for(Int_t p = 0; p <nPlanes; p++) {      
      helChain->SetBranchAddress(Form("yield_p%dRawV1495Scaler",p+1),&bYield[p]);
      helChain->SetBranchAddress(Form("diff_p%dRawV1495Scaler",p+1),&bDiff[p]);
      helChain->SetBranchAddress(Form("asym_p%dRawV1495Scaler",p+1),&bAsym[p]);
    }
  } else if(dataType == "AcV2") {
    helChain->SetBranchStatus("yield_p*RawAc_v2*",1);//!there is ambiguity in this
    helChain->SetBranchStatus("diff_p*RawAc_v2*",1);//!there is ambiguity in this
    helChain->SetBranchStatus("asym_p*RawAc_v2*",1);//!there is ambiguity in this
    for(Int_t p = 0; p <nPlanes; p++) {      
      helChain->SetBranchAddress(Form("yield_p%dRawAc_v2",p+1),&bYield[p]);
      helChain->SetBranchAddress(Form("diff_p%dRawAc_v2",p+1),&bDiff[p]);
      helChain->SetBranchAddress(Form("asym_p%dRawAc_v2",p+1),&bAsym[p]);
    }
  } else if(dataType == "Ac") {
    helChain->SetBranchStatus("yield_p*RawAc*",1);
    helChain->SetBranchStatus("diff_p*RawAc*",1);
    helChain->SetBranchStatus("asym_p*RawAc*",1);
    for(Int_t p = 0; p <nPlanes; p++) {      
      helChain->SetBranchAddress(Form("yield_p%dRawAc",p+1),&bYield[p]);
      helChain->SetBranchAddress(Form("diff_p%dRawAc",p+1),&bDiff[p]);
      helChain->SetBranchAddress(Form("asym_p%dRawAc",p+1),&bAsym[p]);
    }//the branch for each plane is named from 1 to 4
  } else cout<<red<<"dataType not defined clearly"<<normal<<endl;

  ///I need to open the lasCyc dependent files separately here since at every nCycle I update this file with a new entry
  ///..it should be opened before I enter the nCycle loop, and close them after coming out of the nCycle loop.
  if(lasCycPrint) {
    gSystem->mkdir(Form("%s/%s/run_%d/lasCyc",pPath,webDirectory,runnum));    
    for(Int_t p = startPlane; p <endPlane; p++) {
      for(Int_t s =startStrip; s <endStrip; s++) {
        //if (!mask[p][s]) continue;    
        ///lasCyc based files go into a special folder named lasCyc
        countsLC[p][s].open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_"+dataType+"LasCycP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1));
        if(debug1) cout<<"opened "<<Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_"+dataType+"LasCycP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1)<<endl;
        ///Lets open the files for writing asymmetries in the beamOn loop repeatedly
        outAsymLasCyc[p][s].open(Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_"+dataType+"AsymPerLasCycP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1));
        if(debug1) cout<<"opened "<<Form("%s/%s/run_%d/lasCyc/edetLasCyc_%d_"+dataType+"AsymPerLasCycP%dS%d.txt",pPath,webDirectory,runnum,runnum,p+1,s+1)<<endl;
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
  if(nLasCycles<=0) {
    cout<<red<<"no.of laser cycles found in this run is ZERO"<<endl;
    return -1;
  }
  cout<<"if laser-Off period has lasPower > "<<minLasPow<<", or \n"
    <<"if laser-On period has lasPower < "<<acceptLasPow<<", it gets counted as 'missedLasEntries'"<<endl;
  if(nthBeamTrip != nBeamTrips) cout<<"this run has beam trips"<<endl;  

  ///Eventually, I should read in the scalar rate from the root tree for this run and create the corrB1* variables based on the aggregate rate in this category; The fit formula that was found from simulation will anyways be used by all runs. For now, I will simply read in this file generated by G 300 times for this run range used for comparison. 
  if(kDeadTime) {
    cout<<blue<<"Deadtime correction is turned on\n Applying 2 factor correction files"<<normal<<endl;
    ifstream inRateCor;
    Float_t runDum;
    inRateCor.open(Form("%s/data/dtcorr_p0.dat",pPath));
    if(inRateCor.is_open()) {
      while(1) {
        inRateCor>>runDum>>corrB1H1L1_0>>corrB1H1L0_0>>corrB1H0L1_0>>corrB1H0L0_0;
        if(debug1) cout<<runDum<<"\t"<<corrB1H1L1_0<<"\t"<< corrB1H1L0_0<<"\t"<< corrB1H0L1_0<<"\t"<< corrB1H0L0_0<<endl;
        if(inRateCor.eof()) {
          cout<<red<<"exiting the rate file because encountered the end of file"<<normal<<endl;///shouldn't happen
          inRateCor.close();
          break;
        } else if(runDum == runnum) {
          cout<<blue<<"found the correction factors for run "<<runnum<<" hence exiting"<<normal<<endl; 
          cout<<"correction factors:"<<corrB1H1L1_0<<"\t"<<corrB1H1L0_0<<"\t"<<corrB1H0L1_0<<"\t"<<corrB1H0L0_0<<"\n"<<endl;
          inRateCor.close();
          break;
        }
      }
      inRateCor.open(Form("%s/data/dtcorr_p1.dat",pPath));
      if(inRateCor.is_open()) {
        while(1) {
          inRateCor>>runDum>>corrB1H1L1_1>>corrB1H1L0_1>>corrB1H0L1_1>>corrB1H0L0_1;
          if(debug1) cout<<runDum<<"\t"<<corrB1H1L1_1<<"\t"<< corrB1H1L0_1<<"\t"<< corrB1H0L1_1<<"\t"<< corrB1H0L0_1<<endl;
          if(inRateCor.eof()) {
            cout<<red<<"exiting the rate file because encountered the end of file"<<normal<<endl;///shouldn't happen
            inRateCor.close();
            break;
          } else if(runDum == runnum) {
            cout<<blue<<"found the correction factors for run "<<runnum<<" hence exiting"<<normal<<endl;
            cout<<"correction factors:"<<corrB1H1L1_1<<"\t"<<corrB1H1L0_1<<"\t"<<corrB1H0L1_1<<"\t"<<corrB1H0L0_1<<"\n"<<endl;
            inRateCor.close();
            break;
          }
        }
      } else cout<<red<<"\n***Alert: Could not open file for p1 DT correction factors\n"<<normal<<endl;
    } else cout<<red<<"\n***Alert: Could not open file for p0 DT correction factors\n"<<normal<<endl;
  } else {
    corrB1H1L1_0=1.0, corrB1H1L1_1=1.0; 
    corrB1H1L0_0=1.0, corrB1H1L0_1=1.0;
    corrB1H0L1_0=1.0, corrB1H0L1_1=1.0;
    corrB1H0L0_0=1.0, corrB1H0L0_1=1.0;
    cout<<blue<<"Not applying deadtime correction for run# "<<runnum<<normal<<endl;
  }

  for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
    //for(Int_t nCycle=0; nCycle<1; nCycle++) {//temp:to debug, run only 1 laserCycle
    //cout<<red<<"\n**Temporarily analyzing only one laser cycle for debug purpose**\n"<<normal<<endl;
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
        asymErLC[p][s]= 0.0,asymErSqrLC[p][s]= 0.0,qNormAsymLC[p][s]=0.0;
      }
    }

    if(debug) cout<<"Will analyze from entry # "<<cutLas.at(2*nCycle)<<" to entry # "<<cutLas.at(2*nCycle+2)<<endl;

    for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+2); i++) { 
      //loop over laser cycle periods taking one LasOff state and the following laserOn state
      //if(debug1 && i%100000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;//commented to speed up

      if((i < cutLas.at(2*nCycle+1)) && lasPow[0]<minLasPow) l= 0; ///laser off zone
      else if((i > cutLas.at(2*nCycle+1)) && (lasPow[0]>acceptLasPow)) l =1;///laser on zone
      ///the equal sign above is in laser-On zone because that's how getEBeamLasCuts currently assign it(may change!)
      else missedLasEntries++;

      helChain->GetEntry(i);
      //      if (kRejectBMod && (bModRamp[0]<100.0)) continue; 
      //      if (beamOn) { ////currently the counters are only populated for beamOn cycles
      if ((y_bcm[0]) > beamOnLimit) {///Is the beam>20uA for this quartet
        if (kRejectBMod && (bModRamp[0]>100.0)) {
          missedDueToBMod++; 
          continue;
        }
        //if(bYield[0][44]>0.0) cout<<blue<<i<<"\t"<<yieldB1L1[0][44]<<"\t"<<bYield[0][44]<<normal<<endl;    //temp
        if (l==0) {//using the H=0 configuration for now
          nHelLCB1L0++;
          iLCH1L0 += (y_bcm[0]+d_bcm[0]);///@if the factor '2' is multiplied, that would count the bcm reading once for each of the two H1 measurements; without the factor, this is already an average of the two counts that were recorded during the two H1 events in a given quartet
          iLCH0L0 += (y_bcm[0]-d_bcm[0]);
          lasPowLCB1L0 += lasPow[0];
          for(Int_t p = startPlane; p <endPlane; p++) {
            for(Int_t s =startStrip; s <endStrip; s++) {
              //if (!mask[p][s]) continue;
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
              //if (!mask[p][s]) continue;
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
    //    if (beamOn) {
    if(kRejectBMod) cout<<blue<<"no.of quartets missed for BMod ramp this cycle: "<<missedDueToBMod<<normal<<endl;
    laserOnOffRatioH0 = (Double_t)nHelLCB1L1/nHelLCB1L0;
    totIAllH1L1 += iLCH1L1;
    totIAllH1L0 += iLCH1L0;
    totIAllH0L1 += iLCH0L1;
    totIAllH0L0 += iLCH0L0;
    totHelB1L1 += nHelLCB1L1;
    totHelB1L0 += nHelLCB1L0;
    if (nHelLCB1L1 == 0||nHelLCB1L0 == 0) {
      cout<<blue<<"this laser cycle probably had laser off "<<nCycle+1<<normal<<endl;
    } else if (nHelLCB1L1 < 0||nHelLCB1L0 < 0) {
      printf("\n%s****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check nHelLCB1L1:%d, nHelLCB1L0:%d%s",red,nCycle+1,nHelLCB1L1,nHelLCB1L0,normal);
    } else if (iLCH1L1<= 0||iLCH1L0<= 0||iLCH0L1<= 0||iLCH0L0<= 0) {
      printf("\n%s****  Warning: Something drastically wrong in nCycle:%d\n** check iLCH1L1:%f, iLCH1L0:%f, iLCH0L1:%f, iLCH0L0:%f**%s\n",red,nCycle+1,iLCH1L1,iLCH1L0,iLCH0L1,iLCH0L0,normal);
      cout<<red<<"*** HENCE skipping this laser cycle ***"<<normal<<endl;
      continue;
    } else { ///if everything okay, start main buisness
      for (Int_t p =startPlane; p <endPlane; p++) {
        for (Int_t s = startStrip; s < endStrip; s++) {
          //if (!mask[p][s]) continue;
          totyieldB1L1[p][s] += 4*yieldB1L1[p][s];//yield in Hel_Tree is normalized for the quartet
          totyieldB1L0[p][s] += 4*yieldB1L0[p][s];
        }
      }

      if(debug) cout<<"In lasCyc# "<<nCycle+1<<", nHelLCB1L1:"<<nHelLCB1L1<<", nHelLCB1L0:"<< nHelLCB1L0<<blue<<",\niLCH1L1:"<<iLCH1L1<<", iLCH1L0:"<<iLCH1L0<<", iLCH0L1:"<<iLCH0L1<<", iLCH0L0:"<<iLCH0L0<<normal<<endl;
      //indepYield(yieldB1L1, yieldB1L0, nHelLCB1L1, nHelLCB1L0, iLCH1L1, iLCH1L0);//was invoked for checking
      ///now lets do a yield weighted mean of the above asymmetry
      Double_t qAvgLCH1L1 = iLCH1L1 /(helRate);
      Double_t qAvgLCH1L0 = iLCH1L0 /(helRate);
      Double_t qAvgLCH0L1 = iLCH0L1 /(helRate);
      Double_t qAvgLCH0L0 = iLCH0L0 /(helRate);
      ///a factor of 4 is needed to balance out the averaging in the yield and diff reported in the helicity tree
      for (Int_t p =startPlane; p <endPlane; p++) {	  	  
        for (Int_t s =startStrip; s <endStrip; s++) {	  
          //if (!mask[p][s]) continue;
          countsLCB1H1L1[p][s]=2.0*(yieldB1L1[p][s] + diffB1L1[p][s]);///the true total counts this laser cycle for H1L1
          countsLCB1H1L0[p][s]=2.0*(yieldB1L0[p][s] + diffB1L0[p][s]);
          countsLCB1H0L1[p][s]=2.0*(yieldB1L1[p][s] - diffB1L1[p][s]);
          countsLCB1H0L0[p][s]=2.0*(yieldB1L0[p][s] - diffB1L0[p][s]);
          totyieldB1H1L1[p][s] += countsLCB1H1L1[p][s];
          totyieldB1H1L0[p][s] += countsLCB1H1L0[p][s];
          totyieldB1H0L1[p][s] += countsLCB1H0L1[p][s];
          totyieldB1H0L0[p][s] += countsLCB1H0L0[p][s];
        }
      }

      ///// Modification due to explicit (electronic) noise subtraction /////      
      if(kNoiseSub) {
        for (Int_t p =startPlane; p <endPlane; p++) {	  	  
          for (Int_t s =startStrip; s <endStrip; s++) {	  
            //if (!mask[p][s]) continue;
            ///the noiseSubtraction assumes that the +ve and -ve helicities are equal in number, hence dividing by 2
            countsLCB1H1L1[p][s]=countsLCB1H1L1[p][s]-rateB0[s]*(nHelLCB1L1/2.0)/helRate;
            countsLCB1H1L0[p][s]=countsLCB1H1L0[p][s]-rateB0[s]*(nHelLCB1L0/2.0)/helRate;
            countsLCB1H0L1[p][s]=countsLCB1H0L1[p][s]-rateB0[s]*(nHelLCB1L1/2.0)/helRate;
            countsLCB1H0L0[p][s]=countsLCB1H0L0[p][s]-rateB0[s]*(nHelLCB1L0/2.0)/helRate;
          }
        }
      }
      //////////////

      /////Apply deadtime correction///////////////////
      if(kDeadTime) {
        if(corrB1H1L1_0==corrB1H1L0_0 || corrB1H0L1_0==corrB1H0L0_0) cout<<red<<"the correction factors for laser on/off is same for this run, hence something wrong"<<normal<<endl;
        else {
          for (Int_t p =startPlane; p <endPlane; p++) { 
            for (Int_t s =startStrip; s <endStrip; s++) {	
              c2B1H1L1[p][s] = (corrB1H1L1_0 + (s+1)*corrB1H1L1_1);
              c2B1H1L0[p][s] = (corrB1H1L0_0 + (s+1)*corrB1H1L0_1); 
              c2B1H0L1[p][s] = (corrB1H0L1_0 + (s+1)*corrB1H0L1_1);
              c2B1H0L0[p][s] = (corrB1H0L0_0 + (s+1)*corrB1H0L0_1);
            }
          }
        }
      } else {
         for (Int_t p =startPlane; p <endPlane; p++) { 
           for (Int_t s =startStrip; s <endStrip; s++) {	
             c2B1H1L1[p][s] = 1.0; 
             c2B1H1L0[p][s] = 1.0;
             c2B1H0L1[p][s] = 1.0;
             c2B1H0L0[p][s] = 1.0;
           }
         }
       }
      //the corr* variables depend on the aggregate rate at a time, unaffected by individual strip rate
      /////////////////////////////////////////////////

      Int_t evaluated = evaluateAsym(countsLCB1H1L1,countsLCB1H1L0,countsLCB1H0L1,countsLCB1H0L0,qAvgLCH1L1,qAvgLCH1L0,qAvgLCH0L1,qAvgLCH0L0,wmCountsNrAsym,wmCountsDrAsym,wmCountsNrBCqNormSum,wmCountsDrBCqNormSum,wmCountsNrBCqNormDiff,wmCountsNrqNormB1L0,wmCountsDrqNormB1L0,wmCountsNrBkgdAsym,wmCountsDrBkgdAsym,qNormAsymLC,asymErSqrLC);
      if(evaluated<0) {
        cout<<red<<"evaluateAsym reported background corrected yield to be negative in lasCycle "<<nCycle+1<<" hence skipping"<<normal<<endl;
        continue;///go to next nCycle
        //break;///exit the for loop of laser cycles
        //return -1;///exit the expAsym.C macro
      //} else if(skipCyc>0) {///if the skipCyc event happens more than twice(arbitrarily chosen number)
      //  cout<<red<<"skipping this laser cycle, nCycle "<<nCycle+1<<normal<<endl;
      //  continue;//break;
      } else goodCycles++;///
      if(lasCycPrint) {
        //q=i*t;qH1L1=(iH1L1/nMpsH1L1)*(nMpsH1L1/MpsRate);//this really gives total-charge for this laser cycle
        Double_t qLasCycL1 = iLCH0L1 /helRate;//(iLCH0L1 + iLCH1L1) /helRate;  
        Double_t qLasCycL0 = iLCH0L0 /helRate;
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
            //if (!mask[p][s]) continue;
            if(!firstlinelasPrint[p][s]) {
              countsLC[p][s]<<"\n";
              outAsymLasCyc[p][s]<<"\n";
            }
            firstlinelasPrint[p][s] =kFALSE;
            if (countsLC[p][s].is_open()) {
              countsLC[p][s]<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,yieldB1L0[p][s]/qLasCycL0,yieldB1L1[p][s]/qLasCycL1);
              countsLC[p][s]<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,(yieldB1L0[p][s]/((Double_t)nHelLCB1L0/helRate)),((TMath::Sqrt(yieldB1L0[p][s]))/(Double_t)(nHelLCB1L0/helRate)));
            } else cout<<"\n***Alert: Couldn't open file for writing laserCycle based values\n\n"<<endl;  
            if (outAsymLasCyc[p][s].is_open()) {
              outAsymLasCyc[p][s]<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,qNormAsymLC[p][s],TMath::Sqrt(asymErSqrLC[p][s]));
            } else cout<<"\n***Alert: Couldn't open file for writing asymmetry per laser cycle per strip\n\n"<<endl;
          }
        }
        lasCycBCM<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,(iLCH1L0+iLCH0L0)/nHelLCB1L0,(iLCH1L1+iLCH0L1)/nHelLCB1L1);
        lasCycLasPow<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,lasPowLCB1L0/nHelLCB1L0,lasPowLCB1L1/nHelLCB1L1);
      }///if(lasCycPrint)

    }///sanity check of being non-zero for filled laser cycle variables
    //    }///if (beamOn)
    //    else cout<<"this LasCyc: "<<nCycle+1<<" had a beam trip(nthBeamTrip:"<<nthBeamTrip<<"), hence skipping"<<endl;
  }///for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 

  for(Int_t p = startPlane; p <endPlane; p++) {
    for(Int_t s =startStrip; s <endStrip; s++) {
      //if (!mask[p][s]) continue;    
      countsLC[p][s].close();
      outAsymLasCyc[p][s].close();
    }
  }
  lasCycBCM.close();
  lasCycLasPow.close();

  cout<<blue<<"the dataType is set to :"<<dataType<<normal<<endl;
  //notice that the variables to be written by the writeToFile command is updated after every call of weightedMean() function

  if(goodCycles>0) {
    Double_t wm_out = weightedMean(wmCountsNrAsym, wmCountsDrAsym, wmCountsNrBCqNormSum, wmCountsDrBCqNormSum, wmCountsNrBCqNormDiff, wmCountsNrqNormB1L0, wmCountsDrqNormB1L0, wmCountsNrBkgdAsym, wmCountsDrBkgdAsym);
    if(wm_out<0) {
      cout<<red<<"the weightedMean macro returned negative,check what's wrong"<<normal<<endl;
      return -1;///exit the execution
    }
    qNormVariables(totyieldB1L1,totIAllH1L1,qNormCountsB1L1,qNormCountsB1L1Er);//background uncorrected yield
    qNormVariables(totyieldB1L0,totIAllH1L0,qNormCountsB1L0,qNormCountsB1L0Er);//background yield
    qNormVariables(totyieldB1L1,totHelB1L1,tNormYieldB1L1,tNormYieldB1L1Er);
    qNormVariables(totyieldB1L0,totHelB1L0,tNormYieldB1L0,tNormYieldB1L0Er);

    writeToFile(runnum,dataType);

    //!!this currently would not work if the Cedge changes between planes
    if(debug2) {
      TCanvas *cStability = new TCanvas("cStability","stability parameters",0,0,1200,900);
      cStability->Divide(3,3);
      cStability->cd(1);
      helChain->Draw("yield_sca_bcm6:pattern_number","yield_sca_bcm6.value < 200");
      cStability->cd(2);
      helChain->Draw("yield_sca_bpm_3p02aY:pattern_number");
      cStability->cd(3);
      helChain->Draw("yield_sca_bpm_3p02bY:pattern_number");
      cStability->cd(4);
      helChain->Draw("yield_sca_bpm_3p03aY:pattern_number");
      cStability->cd(5);
      helChain->Draw("yield_sca_bpm_3c20Y:pattern_number");
      //helChain->Draw("sca_bpm_3c20X:event_number");
      cStability->cd(6);
      helChain->Draw("yield_sca_bpm_3p02aX:pattern_number");
      cStability->cd(7);
      helChain->Draw("yield_sca_bpm_3p02bX:pattern_number");
      cStability->cd(8);
      helChain->Draw("yield_sca_bpm_3p03aX:pattern_number");
      cStability->cd(9);
      helChain->Draw("yield_sca_laser_PowT:pattern_number","yield_sca_laser_PowT<180000");

      cStability->Update();
      cStability->SaveAs(Form("%s/%s/%sBeamStability.png",pPath,webDirectory,filePrefix.Data()));
    }
  } else cout<<red<<"\nI didn't find even one laser cycler in this run hence NOT processing further"<<normal<<endl;
  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to evaluate expAsym.\n",div_output.quot,div_output.rem );  
  if(kRejectBMod) cout<<blue<<"total no.of quartets ignored due to Beam Mod : "<<totalMissedQuartets<<normal<<endl;
  cout<<blue<<"We used "<<goodCycles<<", out of "<<nLasCycles<<" laser cycles in run "<<runnum<<normal<<endl;
  return goodCycles;//the function returns the number of used Laser cycles
  }
  /******************************************************
    !Further modifications:
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
