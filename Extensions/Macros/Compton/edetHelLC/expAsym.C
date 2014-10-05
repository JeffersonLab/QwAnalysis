///Author: Amrendra Narayan
///Courtesy: Don Jones seeded the code 
#include "rootClass.h"
#include "comptonRunConstants.h"
#include "getEBeamLasCuts.C"
#include "stripMask.C"
#include "infoDAQ.C"
#include "evaluateAsym.C"
#include "evalBgdAsym.C"
#include "weightedMean.C"
#include "writeToFile.C"
#include "qNormVariables.C"
///////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton electron detector run laser wise and plots the ...
///////////////////////////////////////////////////////////////////////////

Int_t expAsym(Int_t runnum, TString dataType="Ac")
{
  cout<<"\nstarting into expAsym.C**************with dataType: "<<dataType<<"\n"<<endl;
  filePre = Form(filePrefix,runnum,runnum);
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  const Bool_t debug = 1, debug1 = 0, debug2 = 0;
  const Bool_t lasCycPrint=0;//!if accum, scaler counts and asym are needed for every laser cycle
  Bool_t beamOn =kFALSE;//lasOn,
  Int_t goodCycles=0,chainExists = 0, missedDueToBMod=0;
  Int_t lasOn = 0;//lasOn tracking variables
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t nHelLCB1L1=0, nHelLCB1L0=0;
  Int_t totalMissedQuartets=0;
  Int_t entry=0; ///integer assitant in reading entry from a file
  Int_t missedLasEntries=0; ///number of missed entries due to unclear laser-state(neither fully-on,nor fully-off)
  Double_t yieldB1L1[nStrips], yieldB1L0[nStrips];
  Double_t diffB1L1[nStrips], diffB1L0[nStrips];
  Double_t bYield[nStrips],bDiff[nStrips],bAsym[nStrips];
  Double_t iLCH1L1=0.0,iLCH1L0=0.0,iLCH0L1=0.0,iLCH0L0=0.0;
  Double_t lasPow[3],y_bcm[3],d_bcm[3],bModRamp[3];//?!the vector size should be 3 I suppose, not 4
  //Double_t lasPow[2],y_bcm[2],d_bcm[2],bpm_3c20X[1],bModRamp[2];
  Double_t pattern_number, event_number;
  Double_t laserOnOffRatioH0;
  Double_t countsLCB1H1L1[nStrips],countsLCB1H1L0[nStrips],countsLCB1H0L1[nStrips],countsLCB1H0L0[nStrips];

  Double_t lasPowLCB1L0=0.0,lasPowLCB1L1=0.0;

  TString readEntry;
  TChain *helChain = new TChain("Hel_Tree");//chain of run segments
  vector<Int_t>cutLas;//arrays of cuts for laser
  vector<Int_t>cutEB;//arrays of cuts for electron beam
  ofstream countsLC[nStrips],lasCycBCM,lasCycLasPow;//to write files every laserCycle
  ofstream outAsymLasCyc[nStrips];
  ifstream infileLas, infileBeam, fileNoise;

  gSystem->mkdir(Form("%s/%s/run_%d",pPath,www,runnum));
  if (kRejectBMod) cout<<green<<"quartets during beam modulation ramp rejected"<<normal<<endl;
  else cout<<green<<"quartets during beam modulation ramp NOT rejected"<<normal<<endl;

  if(daqflag == 0) daqflag = stripMask(); //if the masks are not set yet, its needed in evaluateAsym.C
  if(daqCheck ==0) daqCheck = infoDAQ(runnum, dataType); ///needed acTrig for applying deadtime correction
  if(daqflag==-1) {
    cout<<red<<"\nreturned error from stripMask.C, hence exiting\n"<<normal<<endl;
    return -1;
  } else if(daqCheck<0) {
    cout<<red<<"\nreturned error from infoDAQ.C hence exiting\n"<<normal<<endl;
    return -2;
  }

  //ifstream infileBeamProp; 
  /////Check if it is already known that this run has no beam
  //Double_t runN =0.0, maxBeam =0.0, totBeamTrips =0.0, lasMaxP =0.0, lasCycles =0.0, readEntries =0.0;
  //infileBeamProp.open(Form("%s/%s/%sinfoBeamLas.txt",pPath,www,filePre.Data()));
  //if(infileBeamProp.is_open()) {
  //  cout<<"reading file "<<Form("%s/%s/%sinfoBeamLas.txt",pPath,www,filePre.Data())<<endl;
  //  if(infileBeamProp.good()) {///its a single line read
  //    infileBeamProp>>runN>>maxBeam>>totBeamTrips>>lasMaxP>>lasCycles >>readEntries;
  //    if(maxBeam < beamOnLimit) {
  //      cout<<red<<"beamMax is "<<maxBeam<<" uA, < the beamOnLimit ("<<beamOnLimit<<" uA)"<<normal<<endl;
  //      return -1;
  //    } else cout<<blue<<"beamMax is "<<maxBeam<<" uA, hence continuing the analysis of this run"<<normal<<endl;
  //  }
  //}

  ///following variables are not to be reset every laser-cycle hence lets initialize with zero
  ///some of the variables declared in the compton header file
  for(Int_t s =startStrip; s <endStrip; s++) {
    wmNrAsym[s]=0.0,wmDrAsym[s]=0.0;
    wmNrBkgdAsym[s]=0.0,wmDrBkgdAsym[s]=0.0;
    wmNrBCqNormSum[s]=0.0,wmDrBCqNormSum[s]=0.0;
    wmNrBCqNormDiff[s]=0.0;
    wmNrqNormB1L0[s]=0.0,wmDrqNormB1L0[s]=0.0;

    stripAsym[s]= 0.0,stripAsymEr[s]= 0.0;
    stripAsymDr[s]=0.0,stripAsymDrEr[s]=0.0;
    stripAsymNr[s]=0.0,stripAsymNrEr[s]=0.0;
    bkgdAsym[s]= 0.0,bkgdAsymEr[s]= 0.0;

    qNormB1L0[s]=0.0,qNormB1L0Er[s]=0.0;
    qNormCountsB1L1[s]=0.0,qNormCountsB1L1Er[s]=0.0;
    qNormCountsB1L0[s]=0.0,qNormCountsB1L0Er[s]=0.0;
    qNormAsymLC[s]=0.0,asymErSqrLC[s]=0.0;

    totyieldB1L1[s]=0,totyieldB1L0[s]=0;//this can be removed after the variables below do the requisite
    totyieldB1H1L1[s]=0,totyieldB1H1L0[s]=0,totyieldB1H0L1[s]=0,totyieldB1H0L0[s]=0;      
  }

  chainExists = helChain->Add(Form("$QW_ROOTFILES/Compton_Pass2b_%d.*.root",runnum));//for pass2b
  //chainExists = helChain->Add(Form("$QW_ROOTFILES/Compton_%d.*.root",runnum));//for myQwAnalyisis output
  cout<<"Attached "<<chainExists<<" files to chain for Run # "<<runnum<<endl;

  if(!chainExists){/// exit if rootfiles do not exist
    cout<<"\n\n***Error: The analyzed Root file for run "<<runnum<<" does not exist***\n\n"<<endl;
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
    c1->SaveAs(Form("%s/%s/%slasBeamStability.png",pPath,www,filePre.Data()));
    //return 1;///
    //cout<<red<<"!!!temp exiting"<<endl;
  }
  //////////////////////////////////////////////////////////////////////////////
  infileLas.open(Form("%s/%s/%scutLas.txt",pPath,www,filePre.Data()));
  infileBeam.open(Form("%s/%s/%scutBeam.txt",pPath,www,filePre.Data()));

  if (infileLas.is_open() && infileBeam.is_open()) {
    cout<<"Found the cutLas and cutEB file in "<<Form("%s/%s/run_%d/",pPath,www,runnum)<<" directory."<<endl;
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
    ///If the beamMax is very low exit the execution here itself
    if(beamMax<beamOnLimit) {
      cout<<red<<"beamMax is "<<beamMax<<" uA < beamOnLimit ("<<beamOnLimit<<" uA)"<<normal<<endl; 
      return -1;
    } else cout<<blue<<"beamMax is "<<beamMax<<" uA > beamOnLimit ("<<beamOnLimit<<" uA)"<<normal<<endl; 
  }

  if (debug) cout<<Form("cutEB.size:%d,cutLas.size:%d\n",(Int_t)cutEB.size(),(Int_t)cutLas.size());

  Int_t nEntries = helChain->GetEntries();
  cout<<blue<<"This chain has "<<nEntries<<" entries"<<endl;
  cout<<"beamMax="<<beamMax<<endl;
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
  helChain->SetBranchStatus("pattern_number",1);
  helChain->SetBranchStatus("yield_sca_laser_PowT*",1);
  helChain->SetBranchStatus("yield_sca_bcm6*",1);
  helChain->SetBranchStatus("diff_sca_bcm6*",1);
  helChain->SetBranchStatus("yield_sca_bmod_ramp*",1);

  helChain->SetBranchAddress("mps_counter",&event_number);
  helChain->SetBranchAddress("pattern_number",&pattern_number);
  helChain->SetBranchAddress("yield_sca_laser_PowT",&lasPow);
  helChain->SetBranchAddress("yield_sca_bcm6",&y_bcm);
  helChain->SetBranchAddress("diff_sca_bcm6",&d_bcm);
  helChain->SetBranchAddress("yield_sca_bmod_ramp",&bModRamp);

  if (dataType == "Ev") {
    helChain->SetBranchStatus("yield_p*RawEv*",1);
    helChain->SetBranchStatus("diff_p*RawEv*",1);
    helChain->SetBranchStatus("asym_p*RawEv*",1);
    helChain->SetBranchAddress(Form("yield_p%dRawEv",plane),&bYield);
    helChain->SetBranchAddress(Form("diff_p%dRawEv",plane),&bDiff);
    helChain->SetBranchAddress(Form("asym_p%dRawEv",plane),&bAsym);
  } else if (dataType == "Sc") {
    helChain->SetBranchStatus("yield_p*RawV1495Scaler*",1);
    helChain->SetBranchStatus("diff_p*RawV1495Scaler*",1);
    helChain->SetBranchStatus("asym_p*RawV1495Scaler*",1);
    helChain->SetBranchAddress(Form("yield_p%dRawV1495Scaler",plane),&bYield);
    helChain->SetBranchAddress(Form("diff_p%dRawV1495Scaler",plane),&bDiff);
    helChain->SetBranchAddress(Form("asym_p%dRawV1495Scaler",plane),&bAsym);
  } else if(dataType == "AcV2") {
    helChain->SetBranchStatus("yield_p*RawAc_v2*",1);//!there is ambiguity in this
    helChain->SetBranchStatus("diff_p*RawAc_v2*",1);//!there is ambiguity in this
    helChain->SetBranchStatus("asym_p*RawAc_v2*",1);//!there is ambiguity in this
    helChain->SetBranchAddress(Form("yield_p%dRawAc_v2",plane),&bYield);
    helChain->SetBranchAddress(Form("diff_p%dRawAc_v2",plane),&bDiff);
    helChain->SetBranchAddress(Form("asym_p%dRawAc_v2",plane),&bAsym);
  } else if(dataType == "Ac") {
    helChain->SetBranchStatus("yield_p*RawAc*",1);
    helChain->SetBranchStatus("diff_p*RawAc*",1);
    helChain->SetBranchStatus("asym_p*RawAc*",1);
    helChain->SetBranchAddress(Form("yield_p%dRawAc",plane),&bYield);
    helChain->SetBranchAddress(Form("diff_p%dRawAc",plane),&bDiff);
    helChain->SetBranchAddress(Form("asym_p%dRawAc",plane),&bAsym);
    //the branch for each plane is named from 1 to 4
  } else cout<<red<<"dataType not defined clearly"<<normal<<endl;

  ///I need to open the lasCyc dependent files separately here since at every nCycle I update this file with a new entry
  ///..it should be opened before I enter the nCycle loop, and close them after coming out of the nCycle loop.
  if(lasCycPrint) {
    gSystem->mkdir(Form("%s/%s/run_%d/lasCyc",pPath,www,runnum));    
    for(Int_t s =startStrip; s <endStrip; s++) {
      ///lasCyc based files go into a special folder named lasCyc
      countsLC[s].open(Form("%s/%s/run_%d/lasCyc/edetLC_%d_"+dataType+"LasCycP%dS%d.txt",pPath,www,runnum,runnum,plane,s+1));
      if(debug1) cout<<"opened "<<Form("%s/%s/run_%d/lasCyc/edetLC_%d_"+dataType+"LasCycP%dS%d.txt",pPath,www,runnum,runnum,plane,s+1)<<endl;
      ///Lets open the files for writing asymmetries in the beamOn loop repeatedly
      outAsymLasCyc[s].open(Form("%s/%s/run_%d/lasCyc/edetLC_%d_"+dataType+"AsymPerLasCycP%dS%d.txt",pPath,www,runnum,runnum,plane,s+1));
      if(debug1) cout<<"opened "<<Form("%s/%s/run_%d/lasCyc/edetLC_%d_"+dataType+"AsymPerLasCycP%dS%d.txt",pPath,www,runnum,runnum,plane,s+1)<<endl;
    }
    lasCycBCM.open(Form("%s/%s/run_%d/lasCyc/edetLC_%d_lasCycBcmAvg.txt",pPath,www,runnum,runnum));
    lasCycLasPow.open(Form("%s/%s/run_%d/lasCyc/edetLC_%d_lasCycAvgLasPow.txt",pPath,www,runnum,runnum));  
  }
  if(nLasCycles<=0) {
    cout<<red<<"no.of laser cycles found in this run is ZERO"<<
    "\nHence this will useful entirely as a background run\n"<<normal<<endl;
    return -1;
  }
  cout<<"if laser-Off period has lasPower > "<<minLasPow<<", or \n"
    <<"if laser-On period has lasPower < "<<acceptLasPow<<", it gets counted as 'missedLasEntries'"<<endl;
  if(nthBeamTrip != nBeamTrips) cout<<"this run has beam trips"<<endl;  

  ///Eventually, I should read in the scalar rate from the root tree for this run and create the corrB1* variables based on the aggregate rate in this category; The fit formula that was found from simulation will anyways be used by all runs. For now, I will simply read in this file generated by G 300 times for this run range used for comparison. 
  if(kDeadTime) {
    ifstream inRateCor0, inRateCor1;
    Float_t runDum;
    if(acTrig==2) {
      if(k2parDT) {
        cout<<blue<<"Applying 2 parameter Deadtime correction for 2/3 trigger"<<normal<<endl;
        inRateCor0.open(Form("%s/data/dtcorr_2by3p0.dat",pPath));  //dtcorr1by3_p0.dat 
        inRateCor1.open(Form("%s/data/dtcorr_2by3p1.dat",pPath));///these files have all runs of run2
      } else {
        cout<<blue<<"Applying 1 parameter Deadtime correction for 2/3 trigger"<<normal<<endl;
        inRateCor0.open(Form("%s/data/dtcorr22.dat",pPath));
      }
    } else if(acTrig==3) {
      if(k2parDT) {
        cout<<blue<<"Applying 2 parameter Deadtime correction for 3/3 trigger"<<normal<<endl;
        inRateCor0.open(Form("%s/data/dtcorr_3by3p0.dat",pPath));
        inRateCor1.open(Form("%s/data/dtcorr_3by3p1.dat",pPath));
      } else {
        cout<<blue<<"Applying 1 parameter Deadtime correction for 3/3 trigger"<<normal<<endl;
        inRateCor0.open(Form("%s/data/dtcorr33.dat",pPath));
      }
    } else if(acTrig==1) {
      cout<<blue<<"NO correction file available for 1/3 trigger, hence no correction being applied"<<normal<<endl;
    } else cout<<red<<"\nTrigger undetermined at "<<acTrig<<", hence DT correction not applied due to ambiguity\n"<<normal<<endl;
    if(acTrig==2 || acTrig==3) {
      if(inRateCor0.is_open()) {
        while(1) {
          inRateCor0>>runDum>>corrB1H1L1_0>>corrB1H1L0_0>>corrB1H0L1_0>>corrB1H0L0_0;
          if(debug2) cout<<runDum<<"\t"<<corrB1H1L1_0<<"\t"<< corrB1H1L0_0<<"\t"<< corrB1H0L1_0<<"\t"<< corrB1H0L0_0<<endl;
          if(inRateCor0.eof()) {
            cout<<red<<"exiting the rate file because encountered the end of file"<<normal<<endl;///shouldn't happen
            inRateCor0.close();
            break;
          } else if(runDum == runnum) {
            cout<<blue<<"found the p0 correction factors for run "<<runnum<<" hence exiting"<<normal<<endl; 
            cout<<"correction factors:"<<corrB1H1L1_0<<"\t"<<corrB1H1L0_0<<"\t"<<corrB1H0L1_0<<"\t"<<corrB1H0L0_0<<"\n"<<endl;
            inRateCor0.close();
            break;
          }
        }
        if(k2parDT) {
          if(inRateCor1.is_open()) {
            while(1) {
              inRateCor1>>runDum>>corrB1H1L1_1>>corrB1H1L0_1>>corrB1H0L1_1>>corrB1H0L0_1;
              if(debug2) cout<<runDum<<"\t"<<corrB1H1L1_1<<"\t"<< corrB1H1L0_1<<"\t"<< corrB1H0L1_1<<"\t"<< corrB1H0L0_1<<endl;
              if(inRateCor1.eof()) {
                cout<<red<<"exiting the rate file because encountered the end of file"<<normal<<endl;///shouldn't happen
                inRateCor1.close();
                break;
              } else if(runDum == runnum) {
                cout<<blue<<"found the p1 correction factors for run "<<runnum<<" hence exiting"<<normal<<endl;
                cout<<"correction factors:"<<corrB1H1L1_1<<"\t"<<corrB1H1L0_1<<"\t"<<corrB1H0L1_1<<"\t"<<corrB1H0L0_1<<"\n"<<endl;
                inRateCor1.close();
                break;
              }
            }
          } else cout<<red<<"\n***Alert: Could not open file for p1 DT correction factors\n"<<normal<<endl;
        }
      } else cout<<red<<"\n***Alert: Could not open file for p0 DT correction factors\n"<<normal<<endl;
    }
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
    for(Int_t s = startStrip; s <endStrip; s++) {
      yieldB1L0[s] =0.0, yieldB1L1[s] =0.0;
      diffB1L0[s] =0.0, diffB1L1[s] =0.0;
      asymErSqrLC[s]= 0.0,qNormAsymLC[s]=0.0;
    }

    if(kOnlyGoodLasCyc) {
      ///Lets see if this laser cycle# corresponds to beamOn or not
      if(noiseRun) beamOn = kFALSE; //this Bool_t variable will be appropriately set by looking at the max current in this run in getEBeamLasCuts.C
      else if(nBeamTrips == 0) beamOn = kTRUE;         ///no beamtrip
      else if(nthBeamTrip < nBeamTrips) {  ///yes, we do have beamtrip(s)
        if(nthBeamTrip==0) { // haven't encountered a trip yet(special case of first trip)
          if(cutLas.at(2*nCycle+2)<cutEB.at(0)) beamOn = kTRUE; ///no beam trip till the end of THIS laser cycle
          else {                    ///there is a beam trip during this laser cycle
            beamOn = kFALSE;
            nthBeamTrip++;          
            cout<<"encountered the first beam trip"<<endl;
          }
        } else if(cutLas.at(2*nCycle)<cutEB.at(2*nthBeamTrip-1)) {///the new lasCyc begins before the end of previous beamTrip
          beamOn=kFALSE;///continuation of the previous nthBeamTrip for current cycle
          cout<<"continuation of the nthBeamTrip:"<<nthBeamTrip<<" for lasCyc:"<<nCycle+1<<endl; 
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
      ///^^identified whether the cycle was beamon or not
    } else beamOn = 1;///No laser cycle is tagged as beamOff,

    if (beamOn) {
      if(debug) cout<<"Will analyze from entry # "<<cutLas.at(2*nCycle)<<" to entry # "<<cutLas.at(2*nCycle+2)<<endl;

      for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+2); i++) { 
        //loop over laser cycle periods taking one LasOff state and the following laserOn state
        //if(debug1 && i%100000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;//commented to speed up
        helChain->GetEntry(i);

        if((i < cutLas.at(2*nCycle+1)) && lasPow[0]<minLasPow) lasOn= 0; ///laser off zone
        else if((i > cutLas.at(2*nCycle+1)) && (lasPow[0]>acceptLasPow)) lasOn =1;///laser on zone
        ///the equal sign above is in laser-On zone because that's how getEBeamLasCuts currently assign it(may change!)
        else lasOn = -1;///just to set it to an absurd value, so that it doesn't retain previous

        if ((y_bcm[0]+d_bcm[0]) > beamOnLimit) {///this check needed for kOnlyGoodCycles=0 case
          ///when only good cycles are taken, 'beamOn' will inhibit the whole laser cycle with a beam trip
          //..when all laserCycles are taken, 2nd condition will reject the individual beamOff quartets
          if (kRejectBMod && (bModRamp[0]>100.0)) {
            missedDueToBMod++; 
            continue;
          }
          //if(bYield[0][44]>0.0) cout<<blue<<i<<"\t"<<yieldB1L1[0][44]<<"\t"<<bYield[0][44]<<normal<<endl;    //temp
          if (lasOn==0) {//using the H=0 configuration for now
            nHelLCB1L0++;
            iLCH1L0 += (y_bcm[0]+d_bcm[0]);///@if the factor '2' is multiplied, that would count the bcm reading once for each of the two H1 measurements; without the factor, this is already an average of the two counts that were recorded during the two H1 events in a given quartet
            iLCH0L0 += (y_bcm[0]-d_bcm[0]);
            lasPowLCB1L0 += lasPow[0];
            for(Int_t s =startStrip; s <endStrip; s++) {
              yieldB1L0[s] += bYield[s];
              diffB1L0[s]  += bDiff[s];
            }
          } else if (lasOn==1) {////the elseif statement helps avoid overhead in each entry
            nHelLCB1L1++;
            iLCH1L1 += (y_bcm[0]+d_bcm[0]);
            iLCH0L1 += (y_bcm[0]-d_bcm[0]);
            lasPowLCB1L1 += lasPow[0];
            for(Int_t s =startStrip; s <endStrip; s++) {
              yieldB1L1[s] += bYield[s];
              diffB1L1[s]  += bDiff[s];
            }
          } else missedLasEntries++; ///whenever the laser power was intermittent of the 'on' and 'off'
        }///if ((y_bcm[0]+d_bcm[0]) > beamOnLimit)///!NOT populating beam off counts currently
      }///for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+2); i++)
      if(debug) cout<<"had to ignore "<<((Double_t)missedLasEntries/(cutLas.at(2*nCycle+2) - cutLas.at(2*nCycle)))*100.0<<" % entries in this lasCycle"<<endl;

      totalMissedQuartets += missedDueToBMod++;
      //after having filled the above vectors based on laser and beam periods, find asymmetry for this laser cycle
      if ((y_bcm[0]+d_bcm[0]) > beamOnLimit) {
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
          for (Int_t s = startStrip; s < endStrip; s++) {
            totyieldB1L1[s] += 4*yieldB1L1[s];//yield in Hel_Tree is normalized for the quartet
            totyieldB1L0[s] += 4*yieldB1L0[s];
          }

          if(debug) cout<<"In lasCyc# "<<nCycle+1<<", nHelLCB1L1:"<<nHelLCB1L1<<", nHelLCB1L0:"<< nHelLCB1L0<<blue<<",\niLCH1L1:"<<iLCH1L1<<", iLCH1L0:"<<iLCH1L0<<", iLCH0L1:"<<iLCH0L1<<", iLCH0L0:"<<iLCH0L0<<normal<<endl;
          //indepYield(yieldB1L1, yieldB1L0, nHelLCB1L1, nHelLCB1L0, iLCH1L1, iLCH1L0);//was invoked for checking
          ///now lets do a yield weighted mean of the above asymmetry
          Double_t qAvgLCH1L1 = iLCH1L1 /(helRate);
          Double_t qAvgLCH1L0 = iLCH1L0 /(helRate);
          Double_t qAvgLCH0L1 = iLCH0L1 /(helRate);
          Double_t qAvgLCH0L0 = iLCH0L0 /(helRate);
          ///a factor of 4 is needed to balance out the averaging in the yield and diff reported in the helicity tree
          for (Int_t s =startStrip; s <endStrip; s++) {	  
            countsLCB1H1L1[s]=2.0*(yieldB1L1[s] + diffB1L1[s]);///the true total counts this laser cycle for H1L1
            countsLCB1H1L0[s]=2.0*(yieldB1L0[s] + diffB1L0[s]);
            countsLCB1H0L1[s]=2.0*(yieldB1L1[s] - diffB1L1[s]);
            countsLCB1H0L0[s]=2.0*(yieldB1L0[s] - diffB1L0[s]);
            totyieldB1H1L1[s] += countsLCB1H1L1[s];
            totyieldB1H1L0[s] += countsLCB1H1L0[s];
            totyieldB1H0L1[s] += countsLCB1H0L1[s];
            totyieldB1H0L0[s] += countsLCB1H0L0[s];
          }

          ///// Modification due to explicit (electronic) noise subtraction /////      
          if(kNoiseSub) {
            for (Int_t s =startStrip; s <endStrip; s++) {	  
              ///the noiseSubtraction assumes that the +ve and -ve helicities are equal in number, hence dividing by 2
              countsLCB1H1L1[s]=countsLCB1H1L1[s]-rateB0[s]*(nHelLCB1L1/2.0)/helRate;
              countsLCB1H1L0[s]=countsLCB1H1L0[s]-rateB0[s]*(nHelLCB1L0/2.0)/helRate;
              countsLCB1H0L1[s]=countsLCB1H0L1[s]-rateB0[s]*(nHelLCB1L1/2.0)/helRate;
              countsLCB1H0L0[s]=countsLCB1H0L0[s]-rateB0[s]*(nHelLCB1L0/2.0)/helRate;
            }
          }
          //////////////

          /////Apply deadtime correction///////////////////
          if(kDeadTime) {
            if(corrB1H1L1_0==corrB1H1L0_0 || corrB1H0L1_0==corrB1H0L0_0) cout<<red<<"\nthe correction factors for laser on/off is same for this run, hence something wrong\n"<<normal<<endl;
            else if(k2parDT) {
              //cout<<"applying 2 parameter DT correction"<<endl;
              for (Int_t s =startStrip; s <endStrip; s++) {	
                c2B1H1L1[s] = (corrB1H1L1_0 + (s+1)*corrB1H1L1_1);
                c2B1H1L0[s] = (corrB1H1L0_0 + (s+1)*corrB1H1L0_1); 
                c2B1H0L1[s] = (corrB1H0L1_0 + (s+1)*corrB1H0L1_1);
                c2B1H0L0[s] = (corrB1H0L0_0 + (s+1)*corrB1H0L0_1);
              }
            } else {
              //cout<<"applying 1 parameter DT correction"<<endl;
              for (Int_t s =startStrip; s <endStrip; s++) {	
                c2B1H1L1[s] = corrB1H1L1_0 ;
                c2B1H1L0[s] = corrB1H1L0_0 ; 
                c2B1H0L1[s] = corrB1H0L1_0 ;
                c2B1H0L0[s] = corrB1H0L0_0 ;
              }
            }
          } else {
            for (Int_t s =startStrip; s <endStrip; s++) {	
              c2B1H1L1[s] = 1.0; 
              c2B1H1L0[s] = 1.0;
              c2B1H0L1[s] = 1.0;
              c2B1H0L0[s] = 1.0;
            }
          }
          //the corr* variables depend on the aggregate rate at a time, unaffected by individual strip rate
          /////////////////////////////////////////////////

          Int_t evaluated = evaluateAsym(countsLCB1H1L1, countsLCB1H1L0, countsLCB1H0L1, countsLCB1H0L0, qAvgLCH1L1, qAvgLCH1L0, qAvgLCH0L1, qAvgLCH0L0);
          if(evaluated<0) {
            cout<<red<<"evaluateAsym reported background corrected yield to be negative in lasCycle "<<nCycle+1<<" hence skipping"<<normal<<endl;
            continue;///go to next nCycle
            //break;///exit the for loop of laser cycles
            //return -1;///exit the expAsym.C macro
            //} else if(skipCyc>0) {///if the skipCyc event happens more than twice(arbitrarily chosen number)
            //  cout<<red<<"skipping this laser cycle, nCycle "<<nCycle+1<<normal<<endl;
            //  continue;//break;
        } else {
          evalBgdAsym(countsLCB1H1L0, countsLCB1H0L0, qAvgLCH1L0, qAvgLCH0L0);
          goodCycles++;///
        }
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
          for (Int_t s =startStrip; s <endStrip;s++) {
            if (countsLC[s].is_open()) {
              countsLC[s]<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,yieldB1L0[s]/qLasCycL0,yieldB1L1[s]/qLasCycL1)<<endl;
              countsLC[s]<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,(yieldB1L0[s]/((Double_t)nHelLCB1L0/helRate)),((TMath::Sqrt(yieldB1L0[s]))/(Double_t)(nHelLCB1L0/helRate)))<<endl;
            } else cout<<"\n***Alert: Couldn't open file for writing laserCycle based values\n\n"<<endl;  
            if (outAsymLasCyc[s].is_open()) {
              outAsymLasCyc[s]<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,qNormAsymLC[s],TMath::Sqrt(asymErSqrLC[s]))<<endl;
            } else cout<<"\n***Alert: Couldn't open file for writing asymmetry per laser cycle per strip\n\n"<<endl;
          }
          lasCycBCM<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,(iLCH1L0+iLCH0L0)/nHelLCB1L0,(iLCH1L1+iLCH0L1)/nHelLCB1L1)<<endl;
          lasCycLasPow<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,lasPowLCB1L0/nHelLCB1L0,lasPowLCB1L1/nHelLCB1L1)<<endl;
        }///if(lasCycPrint)

        }///sanity check of being non-zero for filled laser cycle variables
      }///if (y_bcm[0] > beamOnLimit)
      //    else cout<<"this LasCyc: "<<nCycle+1<<" had a beam trip(nthBeamTrip:"<<nthBeamTrip<<"), hence skipping"<<endl;
    } ///if (beamOn); for kOnlyGoodCycles=1, the whole loop is avoided if the lasCyc had a beamTrip
  }///for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 

  if(lasCycPrint) {///close them only if you open 
    for(Int_t s =startStrip; s <endStrip; s++) {
      countsLC[s].close();
      outAsymLasCyc[s].close();
    }
    lasCycBCM.close();
    lasCycLasPow.close();
  }

  cout<<blue<<"the dataType is set to :"<<dataType<<normal<<endl;
  //notice that the variables to be written by the writeToFile command is updated after every call of weightedMean() function

  if(goodCycles>0) {
    Double_t wm_out = weightedMean();//wmNrAsym, wmDrAsym, wmNrBCqNormSum, wmDrBCqNormSum, wmNrBCqNormDiff, wmNrqNormB1L0, wmDrqNormB1L0, wmNrBkgdAsym, wmDrBkgdAsym);
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
  } else cout<<red<<"\nI didn't find even one laser cycler in this run hence NOT processing further"<<normal<<endl;
  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to evaluate expAsym.\n",div_output.quot,div_output.rem );  
  if(kRejectBMod) cout<<blue<<"total no.of quartets ignored due to Beam Mod : "<<totalMissedQuartets<<normal<<endl;
  cout<<blue<<"We used "<<goodCycles<<", out of "<<nLasCycles<<" laser cycles in run "<<runnum<<normal<<endl;

  delete helChain;
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
   ******************************************************/
