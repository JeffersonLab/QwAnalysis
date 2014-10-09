#include "rootClass.h"
#include "comptonRunConstants.h"
#include "stripMask.C"
#include "infoDAQ.C"
#include "evalBgdAsym.C"
#include "weightedMean.C"
#include "qNormVariables.C"
#include "noiseCorrect.C"
#include "bgdAsym.C"
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
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nHelLCB1L1=0, nHelLCB1L0=0;
  Int_t totalMissedQuartets=0;
  Int_t missedLasEntries=0; ///number of missed entries due to unclear laser-state(neither fully-on,nor fully-off)
  Double_t yieldB1L1[nStrips], yieldB1L0[nStrips];
  Double_t diffB1L1[nStrips], diffB1L0[nStrips];
  Double_t bYield[nStrips],bDiff[nStrips],bAsym[nStrips];
  Double_t iLCH1L1=0.0,iLCH1L0=0.0,iLCH0L1=0.0,iLCH0L0=0.0;
  Double_t lasPow[3],y_bcm[3],d_bcm[3],bModRamp[3];//?!the vector size should be 3 I suppose, not 4
  //Double_t lasPow[2],y_bcm[2],d_bcm[2],bpm_3c20X[1],bModRamp[2];
  Double_t pattern_number, event_number;
  Double_t countsLCB1H1L1[nStrips],countsLCB1H1L0[nStrips],countsLCB1H0L1[nStrips],countsLCB1H0L0[nStrips];

  Double_t lasPowLCB1L0=0.0,lasPowLCB1L1=0.0;

  TString readEntry;
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
    qNormCountsB1L1[s]=0.0,qNormCountsB1L1Er[s]=0.0;
    qNormCntsB1H1L0[s]=0.0,qNormCntsB1H1L0Er[s]=0.0;
    qNormAsymLC[s]=0.0,asymErSqrLC[s]=0.0;

    totyieldB1L1[s]=0.0,totyieldB1L0[s]=0.0;//this can be removed after the variables below do the requisite
    totyieldB1H1L1[s]=0,totyieldB1H1L0[s]=0,totyieldB1H0L1[s]=0,totyieldB1H0L0[s]=0;      
  }

  //cout<<"beamMax="<<beamMax<<endl;
  //cout<<"nbeamTrips="<<nBeamTrips<<endl;
  //cout<<"nLasCycles="<<nLasCycles<<normal<<endl;

  gSystem->mkdir(Form("%s/%s/run_%d",pPath,www,runnum));
  Int_t nEntries = helChain->GetEntries();
  cout<<blue<<"This chain has "<<nEntries<<" entries"<<endl;
  ///////////////////////////////////////////////////
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

  cout<<"if laser-Off period has lasPower > "<<minLasPow<<", or \n"
    <<"if laser-On period has lasPower < "<<acceptLasPow<<", it gets counted as 'missedLasEntries'"<<endl;
  if(nthBeamTrip != nBeamTrips) cout<<"this run has beam trips"<<endl;  

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
  iLCH1L1= 0.0, iLCH1L0= 0.0, iLCH0L1= 0.0, iLCH0L0= 0.0;
  lasPowLCB1L0=0.0,lasPowLCB1L1= 0.0;
  for(Int_t s = startStrip; s <endStrip; s++) {
    yieldB1L0[s] =0.0, yieldB1L1[s] =0.0;
    diffB1L0[s] =0.0, diffB1L1[s] =0.0;
    asymErSqrLC[s]= 0.0,qNormAsymLC[s]=0.0;
  }

  Double_t tempBgdCut = 2000;///temporarily place background cut, to easily change without playing with comptonRunConstants.h
  for(Int_t i =0; i <nEntries; i++) { 
    helChain->GetEntry(i);

    if (kRejectBMod && (bModRamp[0]>100.0)) {
      missedDueToBMod++; 
      continue;
    }

    //if(lasPow[0]<minLasPow) lasOn =0; ///laser off zone
    if(lasPow[0]<tempBgdCut) lasOn =0; ///laser off zone
    else if(lasPow[0]>acceptLasPow) lasOn =1;///laser on zone
    else lasOn =-1;///just to set it to an absurd value, so that it doesn't retain previous

    if ((y_bcm[0]+d_bcm[0]) > beamOnLimit) beamOn =1;///beam was on
    else if(y_bcm[0] + d_bcm[0] < beamOnLimit) beamOn =0;
    else beamOn = -1;

    if (beamOn==1 && lasOn==0) {//using the H=0 configuration for now
      nHelLCB1L0++;
      iLCH1L0 += (y_bcm[0]+d_bcm[0]);///@if the factor '2' is multiplied, that would count the bcm reading once for each of the two H1 measurements; without the factor, this is already an average of the two counts that were recorded during the two H1 events in a given quartet
      iLCH0L0 += (y_bcm[0]-d_bcm[0]);
      lasPowLCB1L0 += lasPow[0];
      for(Int_t s =startStrip; s <endStrip; s++) {
        yieldB1L0[s] += bYield[s];
        diffB1L0[s]  += bDiff[s];
      }
    } else if (beamOn==1 && lasOn==1) {////the elseif statement helps avoid overhead in each entry
      nHelLCB1L1++;
      iLCH1L1 += (y_bcm[0]+d_bcm[0]);
      iLCH0L1 += (y_bcm[0]-d_bcm[0]);
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
    for (Int_t s = startStrip; s < endStrip; s++) {
      totyieldB1L1[s] = 4*yieldB1L1[s];//yield in Hel_Tree is normalized for the quartet
      totyieldB1L0[s] = 4*yieldB1L0[s];
    }///for (Int_t s = startStrip; s < endStrip; s++)

    totIAllH1L1 = iLCH1L1; ///in non LC analysis, iLCH1L1 would be the total current already
    totIAllH1L0 = iLCH1L0;
    totIAllH0L1 = iLCH0L1;
    totIAllH0L0 = iLCH0L0;

    if(debug) cout<<"nHelLCB1L1:"<<nHelLCB1L1<<", nHelLCB1L0:"<< nHelLCB1L0<<blue<<",\niLCH1L1:"<<iLCH1L1<<", iLCH1L0:"<<iLCH1L0<<", iLCH0L1:"<<iLCH0L1<<", iLCH0L0:"<<iLCH0L0<<normal<<endl;
    ///now lets do a yield weighted mean of the above asymmetry
    //Double_t qAvgLCH1L1 = iLCH1L1 /(helRate);
    //Double_t qAvgLCH0L1 = iLCH0L1 /(helRate);
    Double_t qAvgLCH1L0 = iLCH1L0 /(helRate);
    Double_t qAvgLCH0L0 = iLCH0L0 /(helRate);
    ///a factor of 4 is needed to balance out the averaging in the yield and diff reported in the helicity tree
    for (Int_t s =startStrip; s <endStrip; s++) {	  
      countsLCB1H1L1[s]=2.0*(yieldB1L1[s] + diffB1L1[s]);///the true total counts this laser cycle for H1L1
      countsLCB1H1L0[s]=2.0*(yieldB1L0[s] + diffB1L0[s]);
      countsLCB1H0L1[s]=2.0*(yieldB1L1[s] - diffB1L1[s]);
      countsLCB1H0L0[s]=2.0*(yieldB1L0[s] - diffB1L0[s]);
    }///for (Int_t s =startStrip; s <endStrip; s++)

    ///////////// Implementing noise subtraction //////
    if(kNoiseSub) {
      //if(0) {
      //  noiseCorrect(Double_t rate1[], Double_t rate2[], Double_t rate3[], Double_t rate4[], Int_t nHelLCB1L1, Int_t nHelLCB1L0) {
      noiseCorrect(countsLCB1H1L1, countsLCB1H1L0, countsLCB1H0L1, countsLCB1H0L0, nHelLCB1L1, nHelLCB1L0);
    } else cout<<blue<<"\nNoise subtraction turned OFF for this analysis\n"<<normal<<endl; 
    ///////////////////////////////////////////////////

    cout<<blue<<"calling evalBgdAsym"<<normal<<endl;
    Int_t evalBgdRet = evalBgdAsym(countsLCB1H1L0, countsLCB1H0L0, qAvgLCH1L0, qAvgLCH0L0);
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
    qNormVariables(countsLCB1H1L0, totIAllH1L0, qNormCntsB1H1L0, qNormCntsB1H1L0Er);//background yield
    qNormVariables(countsLCB1H0L0, totIAllH0L0, qNormCntsB1H0L0, qNormCntsB1H0L0Er);//background uncorrected yield
    qNormVariables(totyieldB1L1,totHelB1L1,tNormYieldB1L1,tNormYieldB1L1Er);
    qNormVariables(totyieldB1L0,totHelB1L0,tNormYieldB1L0,tNormYieldB1L0Er);

    if(runnum!=24762) bgdCorrect(qNormCntsB1H1L0, qNormCntsB1H0L0);///the call of this function in evalBgdAsym.C does not fix these rates written to files
    else cout<<magenta<<"won't apply bgdCorrection for run 24762"<<normal<<endl;

    filePre = Form("run_%d/edetAll_%d_%s",runnum,runnum,dataType.Data());///to to make it stand apart from the LC outputs
    ofstream outfileBkgdAsymP, outfilelasOffBkgd,outqNormH0L0, outqNormH1L0,outtNormYieldB1L0;
    //ofstream outfileYield, fortranCheck,outScaler;

    outfileBkgdAsymP.open(Form("%s/%s/%sBkgdAsymP%d.txt",pPath,www,filePre.Data(),plane));
    outfilelasOffBkgd.open(Form("%s/%s/%sLasOffBkgdP%d.txt",pPath,www,filePre.Data(),plane));
    outqNormH1L0.open(Form("%s/%s/%sqNormCntsB1H1L0P%d.txt",pPath,www,filePre.Data(),plane));
    outqNormH0L0.open(Form("%s/%s/%sqNormCntsB1H0L0P%d.txt",pPath,www,filePre.Data(),plane));
    outtNormYieldB1L0.open(Form("%s/%s/%stNormYieldB1L0P%d.txt",pPath,www,filePre.Data(),plane));
    //outfileYield.open(Form("%s/%s/%sYieldP%d.txt",pPath,www,filePre.Data(),plane));
    //fortranCheck.open(Form("%s/%s/%sFortranCheckP%d.txt",pPath,www,filePre.Data(),plane));
    if (outfileBkgdAsymP.is_open()) {// && outfileYield.is_open() && outfilelasOffBkgd.is_open()) {
      //fortranCheck<<Form("%f\t%f\t%f\t%f\t%f",totIAllH1L1,totIAllH1L0,totIAllH0L1,totIAllH0L0,totIAllH1L1+totIAllH1L0+totIAllH0L1+totIAllH0L0)<<endl;;
      //fortranCheck<<Form("%f\t%f\t%f",totHelB1L1/helRate,totHelB1L0/helRate,(totHelB1L1+totHelB1L0)/helRate)<<endl;
      ////outfileYield<<";strip\texpAsymDr\texpAsymDrEr\texpAsymNr"<<endl;
      for (Int_t s =startStrip; s <endStrip;s++) { 
        outfileBkgdAsymP<<Form("%2.0f\t%f\t%f",(Double_t)s+1,bkgdAsym[s],bkgdAsymEr[s])<<endl;
        outfilelasOffBkgd<<Form("%2.0f\t%g\t%g",(Double_t)s+1,qNormB1L0[s],qNormB1L0Er[s])<<endl;
        outqNormH1L0<<Form("%2.0f\t%g\t%g",(Double_t)s+1, qNormCntsB1H1L0[s], qNormCntsB1H1L0Er[s])<<endl;
        outqNormH0L0<<Form("%2.0f\t%g\t%g",(Double_t)s+1, qNormCntsB1H0L0[s], qNormCntsB1H0L0Er[s])<<endl;
        outtNormYieldB1L0<<Form("%2.0f\t%g\t%g",(Double_t)s+1,tNormYieldB1L0[s],tNormYieldB1L0Er[s])<<endl;
        //outfileYield<<Form("%2.0f\t%g\t%g\t%g",(Double_t)s+1,stripAsymDr[s],stripAsymDrEr[s],stripAsymNr[s])<<endl;//has all expt asym components
        //fortranCheck<<Form("%d\t%d\t%d\t%d\t%d",s+1,totyieldB1H1L1[s],totyieldB1H1L0[s],totyieldB1H0L1[s],totyieldB1H0L0[s])<<endl;
      }///for (Int_t s =startStrip; s <endStrip;s++)
      outfileBkgdAsymP.close();
      outfilelasOffBkgd.close();
      outqNormH1L0.close();
      outqNormH0L0.close();
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
    //TGraphErrors *grH0L0 = new TGraphErrors(Form("%s/%s/%sqNormCntsB1H1L0P%d.txt",pPath,www,filePre.Data(),plane),"%lg %lg %lg");
    //TGraphErrors *grH1L0 = new TGraphErrors(Form("%s/%s/%sqNormCntsB1H0L0P%d.txt",pPath,www,filePre.Data(),plane),"%lg %lg %lg");
    //grH0L0->SetLineColor(kBlue);
    //grH0L0->Draw("AP");
    //grH1L0->SetLineColor(kRed);
    //grH1L0->Draw("P");
    //cBgd->SaveAs(Form("r%d_bgd_%d.png",runnum,(int)tempBgdCut));

    bgdAsym(runnum,dataType);

    return nEntries;//the function returns the number of used Laser cycles
    }

