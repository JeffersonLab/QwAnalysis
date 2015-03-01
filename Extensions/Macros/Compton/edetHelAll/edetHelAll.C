#include "rootClass.h"
#include "comptonRunConstants.h"
#include "stripMask.C"
#include "infoDAQ.C"
#include "evalBgdAsym.C"
#include "weightedMean.C"
#include "qNormVariables.C"
#include "bgdAsym.C"
#include "bgdCorrect.C"
#include "determineNoise.C"
#include "write3CfileArray.C"
#include "aggregateRate.C"
///////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton electron detector run laser wise for beam on, laser off cycles
////////////////////////////////////////////////////////////////////////////

Int_t edetHelAll(Int_t runnum =24503, TString dataType="Ac")
{
  cout<<"\nstarting into indepBgdAnal.C**************with dataType: "<<dataType<<"\n"<<endl;
  filePre = Form("run_%d/edetAll_%d_",runnum,runnum);///to to make it stand apart from the LC outputs
  time_t tStart = time(0), tEnd; 
  div_t div_output;
  const Bool_t debug = 1, debug1 = 0;
  Int_t chainExists = 0, missedDueToBMod=0;
  Int_t beamOn = 0;//beamOn,
  Int_t lasOn = 0;//lasOn tracking variables
  Int_t nHelLCB1L1=0, nHelLCB1L0=0;
  Int_t totalMissedQuartets=0;
  Int_t missedLasEntries=0; ///number of missed entries due to unclear laser-state(neither fully-on,nor fully-off)
  Double_t yieldB1L1[nStrips], yieldB1L0[nStrips];
  Double_t diffB1L1[nStrips], diffB1L0[nStrips];
  Double_t bYield[nStrips],bDiff[nStrips],bAsym[nStrips];
  Double_t qLCH1L1=0.0,qLCH1L0=0.0,qLCH0L1=0.0,qLCH0L0=0.0;
  Double_t lasPow[3],y_bcm[3],d_bcm[3],bModRamp[3];//?!the vector size should be 3 I suppose, not 4
  //Double_t lasPow[2],y_bcm[2],d_bcm[2],bpm_3c20X[1],bModRamp[2];
  Double_t pattern_number, event_number;
  Double_t countsLCB1H1L1[nStrips],countsLCB1H1L0[nStrips],countsLCB1H0L1[nStrips],countsLCB1H0L0[nStrips];
  TString file;
  Double_t lasPowLCB1L0=0.0,lasPowLCB1L1=0.0;

  TChain *helChain = new TChain("Hel_Tree");//chain of run segments

  chainExists = helChain->Add(Form("$QW_ROOTFILES/Compton_Pass2b_%d.*.root",runnum));//for pass2b
  cout<<"Attached "<<chainExists<<" files to chain for Run # "<<runnum<<endl;

  if(!chainExists){/// exit if rootfiles do not exist
    cout<<"\n\n***Error: The analyzed Root file for run "<<runnum<<" does not exist***\n\n"<<endl;
    return -1;
  }

  if(daqflag == 0) daqflag = stripMask(); //if the masks are not set yet, its needed in evaluateAsym.C
  //if(daqCheck ==0) daqCheck = infoDAQ(runnum, dataType); ///needed acTrig for applying deadtime correction
  cout<<magenta<<"not calling infoDAQ, for this version of analysis yet\n"<<normal<<endl;
  if(daqflag==-1) {
    cout<<red<<"\nreturned error from stripMask.C, hence exiting\n"<<normal<<endl;
    return -1;
  } else if(daqCheck<0) {
    cout<<red<<"\nreturned error from infoDAQ.C hence exiting\n"<<normal<<endl;
    return -2;
  }
  if (kRejectBMod) cout<<green<<"quartets during beam modulation ramp rejected"<<normal<<endl;
  else cout<<green<<"quartets during beam modulation ramp NOT rejected"<<normal<<endl;

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
    qNormCntsB1L1[s]=0.0,qNormCntsB1L1Er[s]=0.0;
    qNormCntsB1H1L0[s]=0.0,qNormCntsB1H1L0Er[s]=0.0;
    qNormAsymLC[s]=0.0,asymErSqrLC[s]=0.0;

    totyieldB1H1L1[s]=0,totyieldB1H1L0[s]=0,totyieldB1H0L1[s]=0,totyieldB1H0L0[s]=0;      
  }

  gSystem->mkdir(Form("%s/%s/run_%d",pPath,www,runnum));
  ///////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  TH1D *hBeam = new TH1D("hBeam","dummy",100,0,220);//typical value of maximum beam current
  //hBeam->SetBit(TH1::kCanRebin);
  helChain->Draw("yield_sca_bcm6.value>>hBeam","","goff");
  hBeam = (TH1D*)gDirectory->Get("hBeam");  
  //beamMax = hBeam->GetMaximum();
  beamMean = hBeam->GetMean();
  beamMeanEr = hBeam->GetMeanError();
  beamRMS = hBeam->GetRMS();
  beamMax = hBeam->GetBinLowEdge(hBeam->FindLastBinAbove(100));//to avoid extraneous values
  //cout<<"beamMax(bcm6): "<<beamMax<<",\t beamMean: "<<beamMean<<",\t beamRMS: "<<beamRMS<<endl;

  TH1D *hLaser = new TH1D("hLaser","dummy",1000,0,180000);//typical value of maximum laser power
  //hLaser->SetBit(TH1::kCanRebin);
  helChain->Draw("yield_sca_laser_PowT.value>>hLaser","","goff");
  hLaser = (TH1D*)gDirectory->Get("hLaser");  
  //laserMax = hLaser->GetMaximum();
  laserMax = hLaser->GetBinLowEdge(hLaser->FindLastBinAbove(100));//to avoid extraneous values
  cout<<"laserMax = "<<laserMax<<endl;
  if(beamMax<beamOnLimit) {
    cout<<red<<"beamMax is "<<beamMax<<" uA < beamOnLimit ("<<beamOnLimit<<" uA)"<<normal<<endl; 
    return -1;
  } else cout<<blue<<"beamMax is "<<beamMax<<" uA > beamOnLimit ("<<beamOnLimit<<" uA)"<<normal<<endl; 

  Int_t nEntries = helChain->GetEntries();
  cout<<blue<<"This chain has "<<nEntries<<" entries"<<endl;
  cout<<"beamMax="<<beamMax<<endl;
  cout<<"laserMax="<<laserMax<<endl;

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

  ///Not applying deadtime to this
  for (Int_t s =startStrip; s <endStrip; s++) {	///Needed in evalBgdAsym.C
    c2B1H1L1[s] = 1.0; 
    c2B1H1L0[s] = 1.0;
    c2B1H0L1[s] = 1.0;
    c2B1H0L0[s] = 1.0;
  }
  corrB1H1L1_0=1.0, corrB1H1L1_1=1.0; 
  corrB1H1L0_0=1.0, corrB1H1L0_1=1.0;
  corrB1H0L1_0=1.0, corrB1H0L1_1=1.0;
  corrB1H0L0_0=1.0, corrB1H0L0_1=1.0;
  cout<<blue<<"Not applying deadtime correction for run# "<<runnum<<normal<<endl;
  ///^^^^^^^^^ Not applying deadtime
  cout<<blue<<"\nThis analysis takes all entries in a run, without any consideration for laser cycles"<<normal<<endl;

  nHelLCB1L1= 0, nHelLCB1L0= 0, missedLasEntries=0,missedDueToBMod=0; 
  lasPowLCB1L0=0.0,lasPowLCB1L1= 0.0;
  for(Int_t s = startStrip; s <endStrip; s++) {
    yieldB1L0[s] =0.0, yieldB1L1[s] =0.0;
    diffB1L0[s] =0.0, diffB1L1[s] =0.0;
    asymErSqrLC[s]= 0.0,qNormAsymLC[s]=0.0;
  }

  acceptLasPow = laserFracHi * laserMax;
  cout<<"if laser-Off period has lasPower > "<<lasOffCut<<", or \n"
    <<"if laser-On period has lasPower < "<<laserFracHi * laserMax<<", it gets counted as 'missedLasEntries'"<<endl;

  for(Int_t i =0; i <nEntries; i++) { 
    helChain->GetEntry(i);

    if (kRejectBMod && (bModRamp[0]>100.0)) {
      missedDueToBMod++; 
      continue;
    }

    if(lasPow[0]<lasOffCut) lasOn =0; ///laser off zone
    else if(lasPow[0]>acceptLasPow) lasOn =1;///laser on zone
    else lasOn =-1;///just to set it to an absurd value, so that it doesn't retain previous

    if ((y_bcm[0]+d_bcm[0]) > beamOnLimit) beamOn =1;///beam was on
    else if(y_bcm[0] + d_bcm[0] < beamOnLimit) beamOn =0;
    else beamOn = -1;

    if (beamOn==1 && lasOn==0) {//using the H=0 configuration for now
      nHelLCB1L0++;
      qLCH1L0 += (y_bcm[0]+d_bcm[0])/(2*helRate);///@if the factor '2' is multiplied, that would count the bcm reading once for each of the two H1 measurements; without the factor, this is already an average of the two counts that were recorded during the two H1 events in a given quartet
      qLCH0L0 += (y_bcm[0]-d_bcm[0])/(2*helRate);
      lasPowLCB1L0 += lasPow[0];
      for(Int_t s =startStrip; s <endStrip; s++) {
        yieldB1L0[s] += bYield[s];
        diffB1L0[s]  += bDiff[s];
      }
    } else if (beamOn==1 && lasOn==1) {////the elseif statement helps avoid overhead in each entry
      nHelLCB1L1++;
      qLCH1L1 += (y_bcm[0]+d_bcm[0])/(2*helRate);
      qLCH0L1 += (y_bcm[0]-d_bcm[0])/(2*helRate);
      lasPowLCB1L1 += lasPow[0];
      for(Int_t s =startStrip; s <endStrip; s++) {
        yieldB1L1[s] += bYield[s];
        diffB1L1[s]  += bDiff[s];
      }
    } else missedLasEntries++; ///whenever the laser power was intermittent of the 'on' and 'off'
  }///for(Int_t i =0; i <nEntries; i++)

  ///Having populated all relevant variables, lets churn them to extract what we need
  if(debug) cout<<"had to ignore "<<((Double_t)missedLasEntries/nEntries)*100.0<<" % entries in this lasCycle"<<endl;
  totalMissedQuartets += missedDueToBMod++;
  if(kRejectBMod) cout<<blue<<"no.of quartets missed for BMod ramp this cycle: "<<missedDueToBMod<<normal<<endl;
  //after having filled the above vectors based on laser and beam periods, find asymmetry for this laser cycle
  if (nHelLCB1L1 == 0 && nHelLCB1L0 == 0) {
    cout<<red<<"this laser cycle has a problem "<<normal<<endl;
  } else if (nHelLCB1L1 < 0||nHelLCB1L0 < 0) {
    printf("\n%s****  Warning: Something drastically wrong in :\n\t\t** check nHelLCB1L1:%d, nHelLCB1L0:%d%s",red,nHelLCB1L1,nHelLCB1L0,normal);
  } else { ///if everything okay, start main buisness
    qAllH1L1 = qLCH1L1; ///in non LC analysis, qLCH1L1 would be the total current already
    qAllH1L0 = qLCH1L0;
    qAllH0L1 = qLCH0L1;
    qAllH0L0 = qLCH0L0;

    if(debug) cout<<"nHelLCB1L1:"<<nHelLCB1L1<<", nHelLCB1L0:"<< nHelLCB1L0<<blue<<",\nqLCH1L1:"<<qLCH1L1<<", qLCH1L0:"<<qLCH1L0<<", qLCH0L1:"<<qLCH0L1<<", qLCH0L0:"<<qLCH0L0<<normal<<endl;
    for (Int_t s =startStrip; s <endStrip; s++) {	  
      countsLCB1H1L1[s]=2.0*(yieldB1L1[s] + diffB1L1[s]);///the true total counts this laser cycle for H1L1
      countsLCB1H1L0[s]=2.0*(yieldB1L0[s] + diffB1L0[s]);
      countsLCB1H0L1[s]=2.0*(yieldB1L1[s] - diffB1L1[s]);
      countsLCB1H0L0[s]=2.0*(yieldB1L0[s] - diffB1L0[s]);
    }///for (Int_t s =startStrip; s <endStrip; s++)

    ///////////// Implementing noise subtraction //////
    ///Reading in the noise file
    if(kNoiseSub) {
      Int_t retNoise = determineNoise(runnum, stripArray);
      if (retNoise<0) cout<<red<<"\n Noise subtraction failed, check its reasons\n \n"<<normal<<endl;
    } 

    if(kNoiseSub) {
      for (Int_t s =startStrip; s <endStrip; s++) {	  
        ///the noiseSubtraction assumes that the +ve and -ve helicities are equal in number, hence dividing by 2
        countsLCB1H1L1[s] = countsLCB1H1L1[s] - rateB0[s]*(nHelLCB1L1/2.0)/helRate;
        countsLCB1H1L0[s] = countsLCB1H1L0[s] - rateB0[s]*(nHelLCB1L0/2.0)/helRate;
        countsLCB1H0L1[s] = countsLCB1H0L1[s] - rateB0[s]*(nHelLCB1L1/2.0)/helRate;
        countsLCB1H0L0[s] = countsLCB1H0L0[s] - rateB0[s]*(nHelLCB1L0/2.0)/helRate;
      }
    } else cout<<blue<<"\nNoise subtraction turned OFF for this analysis\n"<<normal<<endl; 
    ///////////////////////////////////////////////////

    qNormVariables(countsLCB1H1L0, qAllH1L0, qNormCntsB1H1L0, qNormCntsB1H1L0Er);//background yield
    qNormVariables(countsLCB1H0L0, qAllH0L0, qNormCntsB1H0L0, qNormCntsB1H0L0Er);//background uncorrected yield
    //qNormVariables(totyieldB1L1,totHelB1L1,tNormYieldB1L1,tNormYieldB1L1Er);
    //qNormVariables(totyieldB1L0,totHelB1L0,tNormYieldB1L0,tNormYieldB1L0Er);

    //if(kBgdCorrect && runnum!=24762) {
    //  bgdCorrect(qNormCntsB1H1L0, qNormCntsB1H0L0);///the call of this function in evalBgdAsym.C does not fix these rates written to files
    //} else cout<<magenta<<"\n NOT applying bgdCorrection for run "<<runnum<<normal<<endl;

    filePre = Form("run_%d/edetAll_%d_%s",runnum,runnum,dataType.Data());///to to make it stand apart from the LC outputs

    file = Form("%s/%s/%sqNormCntsB1H1L0P%d.txt",pPath,www,filePre.Data(),plane);
    write3CfileArray(file, stripArray, qNormCntsB1H1L0, qNormCntsB1H1L0Er);
    
    file = Form("%s/%s/%sqNormCntsB1H0L0P%d.txt",pPath,www,filePre.Data(),plane);
    write3CfileArray(file, stripArray, qNormCntsB1H0L0, qNormCntsB1H0L0Er);

    Double_t aggH1 = aggregateRate(runnum, "Ac", "qNormCntsB1H1L0P1.txt");///returns the aggregate rate
    Double_t aggH0 = aggregateRate(runnum, "Ac", "qNormCntsB1H0L0P1.txt");

    if (aggH1 > aggH0) {
      h1GTh0 = 1;
      cout<<magenta<<"rates in H1 are higher than H0"<<normal<<endl;
    } else if (aggH1 < aggH0) {
      h1GTh0 = 0;
      cout<<magenta<<"rates in H1 are higher than H0"<<normal<<endl;
    } else {
      h1GTh0 = -1;///undetermined 
      cout<<red<<" helicity state undertermined"<<normal<<endl;
    }

/////for the current version of bgd correction, I need to know CE hence reading in a file to find CE
    ifstream fIn;
    file = "comptEdgeRun2.txt";
    fIn.open(file);
    Int_t d1, d4, d5, d6; 
    Double_t d2, d3;
    TString s1, s2, s3, s4, s5, s6;
    if (fIn.is_open()) {
      fIn>>s1 >>s2 >>s3 >>s4 >>s5 >>s6;
      while (1) {
        fIn>>d1 >>d2 >>d3 >>d4 >>d5 >>d6;
        //cout<<d1<<"\t" <<d2<<"\t"  <<d3<<"\t"  <<d4<<"\t"  <<d5<<"\t"  <<d6<<endl;
        if(runnum == d1) {
          Cedge = d2; 
          cout<<blue<<"for run: "<<d1<<" the CE is: "<<Cedge<<normal<<endl;
          break;
        } else if(fIn.eof()) {
          cout<<red<<"reached end of file, without finding CE"<<normal<<endl;
          cout<<"hence forcing CE = 50"<<endl;
          Cedge = 50;
          break;
        }
      }
      fIn.close();
    } else {
      cout<<red<<"couldn't open "<<file<<endl;
      Cedge = 50;
    }

    cout<<blue<<"calling evalBgdAsym"<<normal<<endl;
    Int_t evalBgdRet = evalBgdAsym(countsLCB1H1L0, countsLCB1H0L0, qAllH1L0, qAllH0L0);
    if(evalBgdRet<0) {
      cout<<red<<"evalBgdAsym.C failed so exiting"<<normal<<endl;
      return -1;
    }
    ///explicitly doing the job of weightedMean.C 
    for (Int_t s =startStrip; s <endStrip; s++) {        
      if(std::find(skipStrip.begin(),skipStrip.end(),s+1)!=skipStrip.end()) continue;///to skip mask strips
      else if(wmDrBkgdAsym[s]<=0.0) cout<<red<<"weighted Mean of Asym Dr is non-positive for strip:"<<s+1<<endl;
      else {
        qNormB1L0[s] = wmNrqNormB1L0[s]/wmDrqNormB1L0[s];///pure Beam background
        qNormB1L0Er[s] = TMath::Sqrt(1.0/wmDrqNormB1L0[s]);
        if(debug1) printf("qNormB1L0[%d]:%f  qNormB1L0Er:%f\n",s,qNormB1L0[s],qNormB1L0Er[s]);

        bkgdAsym[s] = wmNrBkgdAsym[s]/wmDrBkgdAsym[s];
        bkgdAsymEr[s] = TMath::Sqrt(1.0/wmDrBkgdAsym[s]);
      }
    }///for (Int_t s =startStrip; s <endStrip; s++)

    ofstream outfileBkgdAsymP, outfilelasOffBkgd,outqNormH0L0, outqNormH1L0,outtNormYieldB1L0;
    //ofstream outfileYield, fortranCheck,outScaler;

    outfileBkgdAsymP.open(Form("%s/%s/%sBkgdAsymP%d.txt",pPath,www,filePre.Data(),plane));
    outfilelasOffBkgd.open(Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,www,filePre.Data(),plane));
    outtNormYieldB1L0.open(Form("%s/%s/%stNormYieldB1L0P%d.txt",pPath,www,filePre.Data(),plane));
    //outfileYield.open(Form("%s/%s/%sYieldP%d.txt",pPath,www,filePre.Data(),plane));
    //fortranCheck.open(Form("%s/%s/%sFortranCheckP%d.txt",pPath,www,filePre.Data(),plane));
    if (outfileBkgdAsymP.is_open()) {// && outfileYield.is_open() && outfilelasOffBkgd.is_open()) {
      //fortranCheck<<Form("%f\t%f\t%f\t%f\t%f",qAllH1L1,qAllH1L0,qAllH0L1,qAllH0L0,qAllH1L1+qAllH1L0+qAllH0L1+qAllH0L0)<<endl;;
      //fortranCheck<<Form("%f\t%f\t%f",totHelB1L1/helRate,totHelB1L0/helRate,(totHelB1L1+totHelB1L0)/helRate)<<endl;
      ////outfileYield<<";strip\texpAsymDr\texpAsymDrEr\texpAsymNr"<<endl;
      for (Int_t s =startStrip; s <endStrip;s++) { 
        outfileBkgdAsymP<<Form("%2.0f\t%f\t%f",(Double_t)s+1,bkgdAsym[s],bkgdAsymEr[s])<<endl;
        outfilelasOffBkgd<<Form("%2.0f\t%g\t%g",(Double_t)s+1,qNormB1L0[s],qNormB1L0Er[s])<<endl;
        outtNormYieldB1L0<<Form("%2.0f\t%g\t%g",(Double_t)s+1,tNormYieldB1L0[s],tNormYieldB1L0Er[s])<<endl;
        //outfileYield<<Form("%2.0f\t%g\t%g\t%g",(Double_t)s+1,stripAsymDr[s],stripAsymDrEr[s],stripAsymNr[s])<<endl;//has all expt asym components
        //fortranCheck<<Form("%d\t%d\t%d\t%d\t%d",s+1,totyieldB1H1L1[s],totyieldB1H1L0[s],totyieldB1H0L1[s],totyieldB1H0L0[s])<<endl;
      }///for (Int_t s =startStrip; s <endStrip;s++)
      outfileBkgdAsymP.close();
      outfilelasOffBkgd.close();
      outtNormYieldB1L0.close();
      //outfileYield.close();
      //fortranCheck.close();
    }///if (outfileBkgdAsymP.is_open())
    }
    tEnd = time(0);
    div_output = div((Int_t)difftime(tEnd, tStart),60);
    printf("\n it took %d minutes %d seconds to evaluate expAsym.\n",div_output.quot,div_output.rem );  
    if(kRejectBMod) cout<<blue<<"total no.of quartets ignored due to Beam Mod : "<<totalMissedQuartets<<normal<<endl;

    //TCanvas *cBgd = new TCanvas("cBgd",Form("bgd yield run %d",runnum),0,0,800,600);
    //cBgd->cd();
    //TGraphErrors *grBgd = new TGraphErrors(Form("%s/%s/%sLasOffBkgdP1.txt",pPath,www,filePre.Data()),"%lg %lg %lg");
    //TGraphErrors *grFlp = new TGraphErrors(Form("/w/hallc/compton/users/narayan/my_scratch/www/run_24762/edetAll_24762_AcLasOffBkgdP1.txt"),"%lg %lg %lg");
    ////TGraphErrors *grH0L0 = new TGraphErrors(Form("%s/%s/%sqNormCntsB1H1L0P%d.txt",pPath,www,filePre.Data(),plane),"%lg %lg %lg");
    ////TGraphErrors *grH1L0 = new TGraphErrors(Form("%s/%s/%sqNormCntsB1H0L0P%d.txt",pPath,www,filePre.Data(),plane),"%lg %lg %lg");
    ////grH0L0->SetLineColor(kBlue);
    ////grH0L0->Draw("AP");
    ////grH1L0->SetLineColor(kRed);
    ////grH1L0->Draw("P");
    //grBgd->Draw("AP");
    //grFlp->Draw("P");
    //cBgd->SaveAs(Form("yieldB1L0_%d_bgd_%d.png",runnum,(int)lasOffCut));

    bgdAsym(runnum,dataType);

    return nEntries;//the function returns the number of used Laser cycles
  }

