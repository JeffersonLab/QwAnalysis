///Author: Amrendra Narayan
#include "rootClass.h"
#include "comptonRunConstants.h"
#include "getEBeamLasCuts.C"
#include "stripMask.C"
#include "evaluateAsym.C"
#include "evalBgdAsym.C"
#include "weightedMean.C"
#include "write3CfileArray.C"
#include "qNormVariables.C"
#include "determineNoise.C"
#include "corrDeadtime.C"
///////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton electron detector run laser wise and plots the ...
///////////////////////////////////////////////////////////////////////////

Int_t expAsym(Int_t runnum = 25419, TString dataType="Ac")
{
  cout<<"\nstarting into expAsym.C**************with dataType: "<<dataType<<"\n"<<endl;
  filePre = Form(filePrefix,runnum,runnum);
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  const Bool_t debug = 1, debug1 = 1, debug2 = 0;
  const Bool_t lasCycPrint=0;//!if accum, scaler counts and asym are needed for every laser cycle
  Bool_t beamOn =kFALSE;//lasOn,
  Int_t goodCycles=0,chainExists = 0, missedDueToBMod=0;
  Int_t lasOn = 0;//lasOn tracking variables
  Int_t beamStable = 0;
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t nHelLCB1L1=0, nHelLCB1L0=0;
  Int_t totalMissedQuartets=0;
  Int_t entry=0; ///integer assitant in reading entry from a file
  Int_t missedLasEntries=0, missedDueToStability=0; ///number of missed entries due to unclear laser-state(neither fully-on,nor fully-off)
  Double_t yieldB1L1[nStrips], yieldB1L0[nStrips];
  Double_t diffB1L1[nStrips], diffB1L0[nStrips];
  Double_t bYield[nStrips],bDiff[nStrips],bAsym[nStrips];
  Double_t qLCH1L1=-1.0,qLCH1L0=-1.0,qLCH0L1=-1.0,qLCH0L0=-1.0;
  Double_t lasPow[3],y_bcm[3],d_bcm[3],bModRamp[3];//declaring 3 element array to accomodate: value, deviceErrorCode, raw
  Double_t d_3p02aY[2], d_3p02bY[2], d_3p03aY[2];
  //Double_t lasPow[2],y_bcm[2],d_bcm[2],bpm_3c20X[1],bModRamp[2];
  Double_t pattern_number, event_number;
  Double_t laserOnOffRatioH0;
  Double_t countsLCB1H1L1[nStrips],countsLCB1H1L0[nStrips],countsLCB1H0L1[nStrips],countsLCB1H0L0[nStrips];
  Double_t stripArray[nStrips];
  Double_t lasPowLCB1L0=-1.0,lasPowLCB1L1=-1.0;

  TString readEntry;
  TChain *helChain = new TChain("Hel_Tree");//chain of run segments
  vector<Int_t>cutLas;//arrays of cuts for laser
  vector<Int_t>cutEB;//arrays of cuts for electron beam
  ofstream countsLC[nStrips],lasCycBCM,lasCycLasPow;//to write files every laserCycle
  ofstream outAsymLasCyc[nStrips];
  ifstream infileLas, infileBeam, fIn;
  TString file;

  gSystem->mkdir(Form("%s/%s/r%d",pPath,txt,runnum));///for text output files
  gSystem->mkdir(Form("%s/%s/r%d",pPath,www,runnum));///for image output files
  if (kRejectBMod) cout<<green<<"quartets during beam modulation ramp rejected"<<normal<<endl;
  else cout<<green<<"quartets during beam modulation ramp NOT rejected"<<normal<<endl;

  if(daqflag == 0) daqflag = stripMask(); //if the masks are not set yet, its needed in evaluateAsym.C
  if(daqflag==-1) {
    cout<<red<<"\nreturned error from stripMask.C, hence exiting\n"<<normal<<endl;
    return -1;
  }

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
    qNormCntsB1H1L1[s]=0.0,qNormCntsB1H1L1Er[s]=0.0;
    qNormCntsB1H1L0[s]=0.0,qNormCntsB1H1L0Er[s]=0.0;
    qNormAsymLC[s]=0.0,asymErSqrLC[s]=0.0;

    totyieldB1H1L1[s]=0,totyieldB1H1L0[s]=0,totyieldB1H0L1[s]=0,totyieldB1H0L0[s]=0;     
    stripArray[s] = s+1;
  }

  chainExists = helChain->Add(Form("$QW_ROOTFILES/Compton_Pass2b_%d.*.root",runnum));//for pass2b
  //chainExists = helChain->Add(Form("$QW_ROOTFILES/Compton_%d.*.root",runnum));//for myQwAnalyisis output
  cout<<"Attached "<<chainExists<<" files to chain for Run # "<<runnum<<endl;

  if(!chainExists){/// exit if rootfiles do not exist
    cout<<"\n\n***Error: The analyzed Root file for run "<<runnum<<" does not exist***\n\n"<<endl;
    return -1;
  }
  //////////////for a quick peep of the beam and laser stability of beam//////////
  if(debug2) {///there's a better version of this available in studyRun.C
    TCanvas *c1 = new TCanvas("c1"," ",0,0,900,900);
    c1->Divide(1,2);
    c1->cd(1);
    helChain->Draw("yield_sca_laser_PowT:pattern_number");    
    c1->cd(2);
    helChain->Draw("yield_sca_bcm6:pattern_number");
    c1->SaveAs(Form("%s/%s/%slasBeamStability.png",pPath,www,filePre.Data()));
  }
  //////////////////////////////////////////////////////////////////////////////
  infileLas.open(Form("%s/%s/%scutLas.txt",pPath,txt,filePre.Data()));
  infileBeam.open(Form("%s/%s/%scutBeam.txt",pPath,txt,filePre.Data()));

  if (infileLas.is_open() && infileBeam.is_open()) {
    cout<<"Found the cutLas and cutEB file around "<<filePre<<endl;
    while (infileLas.good()) {
      infileLas >> readEntry; //read the contents of the line in a string first
      if (readEntry.IsDigit()) { //check if the string is a combination of numbers of not
        entry = readEntry.Atoi(); //if string is a combination of numbers get the corresponding Integer of this string
        if (debug1) cout<<Form("cutLas[%d]=%d\n",(Int_t)cutLas.size(),entry);
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
        if (debug1) cout<<Form("cutEB[%d]=%d\n",(Int_t)cutEB.size(),entry);
        cutEB.push_back(entry);
      }
    }
    infileBeam.close();
    nBeamTrips = (cutEB.size())/2;///note: even if the beam is completely off, this value would be 0;

    file = Form("%s/%s/%sinfoBeamLas.txt",pPath,txt,filePre.Data());
    TString st1, st2, st3, st4, st5, st6, st7, st8, st9;
    Double_t d1, d2, d3, d4;
    fIn.open(file);
    if(fIn.is_open()) {
      fIn >>st1 >>st2 >>st3 >>st4 >>st5 >>st6 >>st7 >>st8 >>st9;
      fIn >>d1 >>beamMax >>beamRMS >>beamMean >>beamMeanEr >>d2 >>laserMax >>d3 >>d4;//nEntries;
      fIn.close();
    } else cout<<red<<"couldn't open "<<file<<normal<<endl;

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
  }
  if(beamMax<beamOnLimit) {
    cout<<red<<"beamMax is "<<beamMax<<" uA < beamOnLimit ("<<beamOnLimit<<" uA)"<<normal<<endl; 
    return -1;
  } else cout<<blue<<"beamMax is "<<beamMax<<" uA > beamOnLimit ("<<beamOnLimit<<" uA)"<<normal<<endl; 

  if (debug) cout<<Form("cutEB.size:%d,cutLas.size:%d\n",(Int_t)cutEB.size(),(Int_t)cutLas.size());

  Int_t nEntries = helChain->GetEntries();
  cout<<blue<<"This chain has "<<nEntries<<" entries"<<endl;
  cout<<"beamMax="<<beamMax<<endl;
  cout<<"laserMax="<<laserMax<<endl;
  cout<<"nbeamTrips="<<nBeamTrips<<endl;
  cout<<"nLasCycles="<<nLasCycles<<normal<<endl;
  acceptLasPow = laserFracHi * laserMax;
  cout<<"if laser-Off period has lasPower > "<<lasOffCut<<", or \n"
    <<"if laser-On period has lasPower < "<<acceptLasPow<<", it gets counted as 'missedLasEntries'"<<endl;

  ///////////// Implementing noise subtraction //////
  ///Reading in the noise file
  if(kNoiseSub) {
    Int_t retNoise = determineNoise(runnum, stripArray);
    if (retNoise<0) cout<<red<<"\n Noise subtraction failed, check its reasons\n \n"<<normal<<endl;
  } 
  ///////////////////////////////////////////////////

  //helChain->ResetBranchAddresses();//!? should it be here?
  helChain->SetBranchStatus("*",0);  ////Turn off all unused branches, for efficient looping
  helChain->SetBranchStatus("pattern_number",1);
  helChain->SetBranchStatus("yield_sca_laser_PowT*",1);
  helChain->SetBranchStatus("yield_sca_bcm6*",1);
  helChain->SetBranchStatus("diff_sca_bcm6*",1);
  helChain->SetBranchStatus("yield_sca_bmod_ramp*",1);
  helChain->SetBranchStatus("diff_sca_bpm_3p02aY*", 1);
  helChain->SetBranchStatus("diff_sca_bpm_3p02bY*", 1);
  helChain->SetBranchStatus("diff_sca_bpm_3p03aY*", 1);
  //helChain->SetBranchAddress("", 1);

  helChain->SetBranchAddress("mps_counter",&event_number);
  helChain->SetBranchAddress("pattern_number",&pattern_number);
  helChain->SetBranchAddress("yield_sca_laser_PowT",&lasPow);
  helChain->SetBranchAddress("yield_sca_bcm6",&y_bcm);
  helChain->SetBranchAddress("diff_sca_bcm6",&d_bcm);
  helChain->SetBranchAddress("yield_sca_bmod_ramp",&bModRamp);
  helChain->SetBranchAddress("diff_sca_bpm_3p02aY", &d_3p02aY);
  helChain->SetBranchAddress("diff_sca_bpm_3p02bY", &d_3p02bY);
  helChain->SetBranchAddress("diff_sca_bpm_3p03aY", &d_3p03aY);


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
    gSystem->mkdir(Form("%s/%s/r%d/lasCyc",pPath,txt,runnum));    
    for(Int_t s =startStrip; s <endStrip; s++) {
      ///lasCyc based files go into a special folder named lasCyc
      countsLC[s].open(Form("%s/%s/r%d/lasCyc/edetLC_%d_"+dataType+"LasCycP%dS%d.txt",pPath,txt,runnum,runnum,plane,s+1));
      if(debug1) cout<<"opened "<<Form("%s/%s/r%d/lasCyc/edetLC_%d_"+dataType+"LasCycP%dS%d.txt",pPath,txt,runnum,runnum,plane,s+1)<<endl;
      ///Lets open the files for writing asymmetries in the beamOn loop repeatedly
      outAsymLasCyc[s].open(Form("%s/%s/r%d/lasCyc/edetLC_%d_"+dataType+"AsymPerLasCycP%dS%d.txt",pPath,txt,runnum,runnum,plane,s+1));
      if(debug1) cout<<"opened "<<Form("%s/%s/r%d/lasCyc/edetLC_%d_"+dataType+"AsymPerLasCycP%dS%d.txt",pPath,txt,runnum,runnum,plane,s+1)<<endl;
    }
    lasCycBCM.open(Form("%s/%s/r%d/lasCyc/edetLC_%d_lasCycBcmAvg.txt",pPath,txt,runnum,runnum));
    lasCycLasPow.open(Form("%s/%s/r%d/lasCyc/edetLC_%d_lasCycAvgLasPow.txt",pPath,txt,runnum,runnum));  
  }
  if(nLasCycles<=0) {
    cout<<red<<"no.of laser cycles found in this run is ZERO"<<
      "\nHence this will useful entirely as a background run\n"<<normal<<endl;
    return -1;
  }

  if(dataType=="Ac" && kDeadTime) {///to ensure that it doesn't get applied for Scaler analysis
    corrDeadtime(runnum, dataType);
  } else {
    for (Int_t s =startStrip; s <endStrip; s++) {	
      c2B1H1L1[s] = 1.0; 
      c2B1H1L0[s] = 1.0;
      c2B1H0L1[s] = 1.0;
      c2B1H0L0[s] = 1.0;
    }
    cout<<blue<<"Not applying deadtime correction for run# "<<runnum<<normal<<endl;
  }

  for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
    //for(Int_t nCycle=0; nCycle<1; nCycle++) {//temp:to debug, run only 1 laserCycle
    //cout<<red<<"\n**Temporarily analyzing only one laser cycle for debug purpose**\n"<<normal<<endl;
    if(debug) cout<<"\nStarting nCycle:"<<nCycle+1<<" from entry # "<<cutLas.at(2*nCycle)<<" to # "<<cutLas.at(2*nCycle+2)<<endl;

    ///since this is the beginning of a new Laser cycle, and all Laser cycle based variables 
    ///..are already assigned to a permanent variable reset the LasCyc based variables
    nHelLCB1L1= 0, nHelLCB1L0= 0, missedLasEntries=0,missedDueToBMod=0, missedDueToStability=0; 
    qLCH1L1= 0.0, qLCH1L0= 0.0, qLCH0L1= 0.0, qLCH0L0= 0.0;
    lasPowLCB1L0=0.0,lasPowLCB1L1= 0.0;
    for(Int_t s = startStrip; s <endStrip; s++) {
      yieldB1L0[s] =0.0, yieldB1L1[s] =0.0;
      diffB1L0[s] =0.0, diffB1L1[s] =0.0;
      asymErSqrLC[s]= 0.0,qNormAsymLC[s]=0.0;
    }

    if(kOnlyGoodLasCyc) {
      ///Lets see if this laser cycle# corresponds to beamOn or not
      //if(noiseRun) beamOn = kFALSE; //this Bool_t variable will be appropriately set by looking at the max current in this run in getEBeamLasCuts.C
      if(nBeamTrips == 0) beamOn = kTRUE;         ///no beamtrip
      else if(nthBeamTrip < nBeamTrips) {  ///yes, we do have beamtrip(s)
        if(nthBeamTrip==0) { 
          cout<<"haven't encountered a trip yet(special case of first trip)"<<endl;
          if(cutLas.at(2*nCycle+2)<cutEB.at(0)) {
            beamOn = kTRUE; 
            if(debug) cout<<"no beam trip till the end of THIS laser cycle"<<endl;
          } else {                    ///there is a beam trip during this laser cycle
            beamOn = kFALSE;
            nthBeamTrip++;          
            cout<<"encountered the first beam trip in lasCycle: "<<nCycle+1<<endl;
          }
        } else if(cutLas.at(2*nCycle)<cutEB.at(2*nthBeamTrip-1)) {///the new lasCyc begins before the end of previous beamTrip
          beamOn=kFALSE;///continuation of the previous nthBeamTrip for current cycle
          cout<<"continuation of the nthBeamTrip:"<<nthBeamTrip<<" for lasCyc:"<<nCycle+1<<endl; 
        } else if(nthBeamTrip<nBeamTrips && cutLas.at(2*nCycle)>cutEB.at(2*nthBeamTrip+1)) {///the beginning of this lasCyc preceeds the end of next beamTrip also => there were more than one beamTrip in last lasCyc
          while(cutLas.at(2*nCycle) > cutEB.at(2*nthBeamTrip+1)) {
            if(debug1) printf("%snCycle: %d\tnthBeamTrip: %d\ncutLas[%d]: %d\tcutEB[%d]: %d%s\n", blue, nCycle, nthBeamTrip, 2*nCycle, cutLas[2*nCycle], 2*nthBeamTrip, cutEB[2*nthBeamTrip], normal);
            nthBeamTrip++;
            cout<<"increamenting the beam trip counter, nthBeamTrip : "<<nthBeamTrip<<endl;
            if (nthBeamTrip==nBeamTrips) break;//cutEB wont' exist for nthBeamTrip = nBeamTrips. 
          }
          cout<<"more than one beam trip must have happenned in the last lasCyc"<<endl;
          cout<<"beam trip counter incremented to : "<<nthBeamTrip<<endl;
          if (cutLas.at(2*nCycle) > cutEB.at(2*nthBeamTrip-1)) beamOn = kTRUE;
        } else if(cutLas.at(2*nCycle+2)<cutEB.at(2*nthBeamTrip)) {
          beamOn=kTRUE;///next beamTrip does not occur atleast till the end of THIS cycle
          if(debug) cout<<"next beamTrip does not occur atleast till the end of THIS cycle"<<endl; 
        } else { ///encountered "another" beam trip 
          beamOn = kFALSE;
          if(debug1) printf("%snCycle: %d\tnthBeamTrip: %d\ncutLas[%d]: %d\tcutEB[%d]: %d%s\n", magenta, nCycle, nthBeamTrip, 2*nCycle, cutLas[2*nCycle], 2*nthBeamTrip, cutEB[2*nthBeamTrip], normal);
          nthBeamTrip++;
          cout<<"encountered "<<nthBeamTrip<<" th beam trip in lasCyc: "<<nCycle+1<<endl;
        } 
      } else if(nthBeamTrip == nBeamTrips) { ///encountered the last beamTrip     
        if (cutLas.at(2*nCycle) > cutEB.at(2*nthBeamTrip-1)) beamOn = kTRUE; ///current laser Cycle begins after the beamTrip recovered
        else beamOn = kFALSE;
      } else cout<<red<<"\n***Error ... Something drastically wrong in BeamTrip evaluation***\n"<<normal<<endl;
      ///^^identified whether the cycle was beamon or not
    } else beamOn = 1;///No laser cycle is tagged as beamOff,

    if (beamOn) {
      for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+2); i++) { 
        //loop over laser cycle periods taking one LasOff state and the following laserOn state
        //if(debug1 && i%100000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;
        helChain->GetEntry(i);

        if((i < cutLas.at(2*nCycle+1)) && lasPow[0]<lasOffCut) lasOn= 0; ///laser off zone
        else if((i > cutLas.at(2*nCycle+1)) && (lasPow[0]>acceptLasPow)) lasOn =1;///laser on zone
        ///the equal sign above is in laser-On zone because that's how getEBeamLasCuts currently assign it(may change!)
        else {
          lasOn = -1;///just to set it to an absurd value, so that it doesn't retain previous
          missedLasEntries++; ///whenever the laser power was intermittent of the 'on' and 'off'
          continue;
        }

        if(d_3p02aY[0] < 0.05 || d_3p02aY[0] < 0.05 || d_3p02aY[0] < 0.05) beamStable = 1;
        else {
          beamStable = 0;
          missedDueToStability++;
          continue;
        }

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
            qLCH1L0 += (y_bcm[0]+d_bcm[0])/(2*helRate);//described on page 21 of logbook (12 Oct 14)
            qLCH0L0 += (y_bcm[0]-d_bcm[0])/(2*helRate);
            lasPowLCB1L0 += lasPow[0];
            for(Int_t s =startStrip; s <endStrip; s++) {
              yieldB1L0[s] += bYield[s];
              diffB1L0[s]  += bDiff[s];
            }
          } else if (lasOn==1) {////the elseif statement helps avoid overhead in each entry
            nHelLCB1L1++;
            qLCH1L1 += (y_bcm[0]+d_bcm[0])/(2*helRate);//described on page 21 of logbook (12 Oct 14)
            qLCH0L1 += (y_bcm[0]-d_bcm[0])/(2*helRate);
            lasPowLCB1L1 += lasPow[0];
            for(Int_t s =startStrip; s <endStrip; s++) {
              yieldB1L1[s] += bYield[s];
              diffB1L1[s]  += bDiff[s];
            }
          }
        }///if ((y_bcm[0]+d_bcm[0]) > beamOnLimit)///!NOT populating beam off counts currently
      }///for(Int_t i =cutLas.at(2*nCycle); i <cutLas.at(2*nCycle+2); i++)
      cout<<blue<<"\% of quartets ignored due to bad laser: "<<((Double_t)missedLasEntries/(cutLas.at(2*nCycle+2) - cutLas.at(2*nCycle)))*100.0<<" % entries in this lasCycle"<<endl;
      if(kRejectBMod) cout<<"# of quartets missed due to BMod ramp this cycle: "<<missedDueToBMod<<endl;
      cout<<"# of quartets missed due to beam fluctuations/stability : "<<missedDueToStability<<normal<<endl;

      totalMissedQuartets += missedDueToBMod++;
      //after having filled the above vectors based on laser and beam periods, find asymmetry for this laser cycle
      laserOnOffRatioH0 = (Double_t)nHelLCB1L1/nHelLCB1L0;
      qAllH1L1 += qLCH1L1;
      qAllH1L0 += qLCH1L0;
      qAllH0L1 += qLCH0L1;
      qAllH0L0 += qLCH0L0;
      totHelB1L1 += nHelLCB1L1;
      totHelB1L0 += nHelLCB1L0;
      if (nHelLCB1L1 == 0||nHelLCB1L0 == 0) {
        cout<<blue<<"this laser cycle probably had laser off "<<nCycle+1<<normal<<endl;
      } else if (nHelLCB1L1 < 0||nHelLCB1L0 < 0) {
        printf("\n%s****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check nHelLCB1L1:%d, nHelLCB1L0:%d%s",red,nCycle+1,nHelLCB1L1,nHelLCB1L0,normal);
      } else if (qLCH1L1<= 0||qLCH1L0<= 0||qLCH0L1<= 0||qLCH0L0<= 0) {
        printf("\n%s****  Warning: Something drastically wrong in nCycle:%d\n** check qLCH1L1:%f, qLCH1L0:%f, qLCH0L1:%f, qLCH0L0:%f**%s\n",red,nCycle+1,qLCH1L1,qLCH1L0,qLCH0L1,qLCH0L0,normal);
        cout<<red<<"*** HENCE skipping this laser cycle ***"<<normal<<endl;
        continue;
      } else { ///if everything okay, start main buisness

        if(debug) cout<<"In lasCyc# "<<nCycle+1<<", nHelLCB1L1:"<<nHelLCB1L1<<", nHelLCB1L0:"<< nHelLCB1L0<<blue<<",\nqLCH1L1:"<<qLCH1L1<<", qLCH1L0:"<<qLCH1L0<<", qLCH0L1:"<<qLCH0L1<<", qLCH0L0:"<<qLCH0L0<<normal<<endl;

        ///a factor of 4 is needed to balance out the averaging in the yield and diff reported in the helicity tree
        for (Int_t s =startStrip; s <endStrip; s++) {	  
          countsLCB1H1L1[s]=2.0*(yieldB1L1[s] + diffB1L1[s]);///the true total counts this laser cycle for H1L1
          countsLCB1H1L0[s]=2.0*(yieldB1L0[s] + diffB1L0[s]);
          countsLCB1H0L1[s]=2.0*(yieldB1L1[s] - diffB1L1[s]);
          countsLCB1H0L0[s]=2.0*(yieldB1L0[s] - diffB1L0[s]);
        }

        ///// Modification due to explicit (electronic) noise subtraction /////      
        if(kNoiseSub) {
          for (Int_t s =startStrip; s <endStrip; s++) {	  
            ///the noiseSubtraction assumes that the +ve and -ve helicities are equal in number, hence dividing by 2
            countsLCB1H1L1[s] = countsLCB1H1L1[s] - rateB0[s]*(nHelLCB1L1/2.0)/helRate;
            countsLCB1H1L0[s] = countsLCB1H1L0[s] - rateB0[s]*(nHelLCB1L0/2.0)/helRate;
            countsLCB1H0L1[s] = countsLCB1H0L1[s] - rateB0[s]*(nHelLCB1L1/2.0)/helRate;
            countsLCB1H0L0[s] = countsLCB1H0L0[s] - rateB0[s]*(nHelLCB1L0/2.0)/helRate;
          }
        }
        //////////////

        for (Int_t s =startStrip; s <endStrip; s++) {	  
          totyieldB1H1L1[s] += countsLCB1H1L1[s];///these still aren't corrected for trigger ineff/deadtime
          totyieldB1H1L0[s] += countsLCB1H1L0[s];
          totyieldB1H0L1[s] += countsLCB1H0L1[s];
          totyieldB1H0L0[s] += countsLCB1H0L0[s];
        }

        ///calling the function to evaluate asymmetry for counts in this laser cycle
        Int_t evaluated = evaluateAsym(countsLCB1H1L1, countsLCB1H1L0, countsLCB1H0L1, countsLCB1H0L0, qLCH1L1, qLCH1L0, qLCH0L1, qLCH0L0);
        if(evaluated<0) {
          cout<<red<<"evaluateAsym reported background corrected yield to be negative in lasCycle "<<nCycle+1<<" hence skipping"<<normal<<endl;
          continue;///go to next nCycle
          //break;///exit the for loop of laser cycles
          //return -1;///exit the expAsym.C macro
          //} else if(skipCyc>0) {///if the skipCyc event happens more than twice(arbitrarily chosen number)
          //  cout<<red<<"skipping this laser cycle, nCycle "<<nCycle+1<<normal<<endl;
          //  continue;//break;
      } else {
        evalBgdAsym(countsLCB1H1L0, countsLCB1H0L0, qLCH1L0, qLCH0L0);
        goodCycles++;///
      }
      if(lasCycPrint) {
        //q=i*t;qH1L1=(iH1L1/nMpsH1L1)*(nMpsH1L1/MpsRate);//this really gives total-charge for this laser cycle
        Double_t qLasCycL1 = qLCH0L1 + qLCH1L1;//(qLCH0L1 + qLCH1L1) /helRate;  
        Double_t qLasCycL0 = qLCH0L0 + qLCH1L0;
        //!!check the above "average charge", it doesn't seem the appropriate way of averaging
        if(debug1) {
          cout<<"the Laser Cycle: "<<nCycle+1<<" has 'beamOn': "<<beamOn<<endl;
          cout<<"printing variables on a per-laser-cycle basis"<<endl;
          cout<<"laserOnOffRatioH0: "<<laserOnOffRatioH0<<endl;
          cout<<Form("qLCH1L1: %f\t qLCH1L0: %f",qLCH1L1,qLCH1L0)<<endl;
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
        lasCycBCM<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,(qLCH1L0+qLCH0L0)/nHelLCB1L0,(qLCH1L1+qLCH0L1)/nHelLCB1L1)<<endl;
        lasCycLasPow<<Form("%2.0f\t%f\t%f",(Double_t)nCycle+1,lasPowLCB1L0/nHelLCB1L0,lasPowLCB1L1/nHelLCB1L1)<<endl;
      }///if(lasCycPrint)

      }///sanity check of being non-zero for filled laser cycle variables
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
    qNormVariables(totyieldB1H1L1, qAllH1L1, qNormCntsB1H1L1, qNormCntsB1H1L1Er);//background uncorrected yield
    qNormVariables(totyieldB1H1L0, qAllH1L0, qNormCntsB1H1L0, qNormCntsB1H1L0Er);//background yield
    //qNormVariables(totyieldB1H1L1, totHelB1L1, tNormYieldB1L1, tNormYieldB1L1Er);
    //qNormVariables(totyieldB1L0, totHelB1L0, tNormYieldB1L0, tNormYieldB1L0Er);

    filePre = Form(filePrefix+"%s",runnum,runnum,dataType.Data());
  
    file = Form("%s/%s/%sExpAsymP%d.txt",pPath, txt,filePre.Data(),plane);
    write3CfileArray(file, stripArray, stripAsym, stripAsymEr);
    
    file = Form("%s/%s/%sBkgdAsymP%d.txt",pPath, txt,filePre.Data(),plane);
    write3CfileArray(file, stripArray, bkgdAsym, bkgdAsymEr);

    file = Form("%s/%s/%sYieldP%d.txt",pPath, txt,filePre.Data(),plane);
    write3CfileArray(file, stripArray, stripAsymDr, stripAsymDrEr);

    file = Form("%s/%s/%sLasOffBkgdP%d.txt",pPath, txt,filePre.Data(),plane);
    write3CfileArray(file, stripArray, qNormB1L0, qNormB1L0Er);

    file = Form("%s/%s/%sqNormCntsB1H1L1P%d.txt",pPath, txt,filePre.Data(),plane);
    write3CfileArray(file, stripArray, qNormCntsB1H1L1, qNormCntsB1H1L1Er);

    file = Form("%s/%s/%sqNormCntsB1H0L0P%d.txt",pPath, txt,filePre.Data(),plane); 
    write3CfileArray(file, stripArray, qNormCntsB1H1L0, qNormCntsB1H1L0Er);

    file = Form("%s/%s/%stNormYieldB1L1P%d.txt",pPath, txt,filePre.Data(),plane);
    write3CfileArray(file, stripArray, tNormYieldB1L1, tNormYieldB1L1Er);

    file = Form("%s/%s/%stNormYieldB1L0P%d.txt",pPath, txt,filePre.Data(),plane);
    write3CfileArray(file, stripArray, tNormYieldB1L0, tNormYieldB1L1Er);

    //this currently would not work if the Cedge changes between planes
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
Comments:
   * B1: beam on; H1: helicity plus; L1: laser on; and vice versa
   * qNormXXX: charge normalized quantity; tNormXXX: time normalized quantity
   * BC: Backgraound Corrected.
   * wm: weightedMean, Ac: Accum, Sc: Scaler, Ev: Event
   * each Laser cycle consitutes of one laser Off zone followed by one laser On period.
   * ..it begins with a Laser Off period and ends with the (just)next laser On period.
   * If we get too many missedEntries,check 'acceptLasPow' and 'lasOffCut' variables
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
