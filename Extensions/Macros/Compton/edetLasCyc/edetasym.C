////I acknowledge Don Jones and Juan Carlos for help in the code
#include <rootClass.h>
#include "getEBeamLasCuts.C"
//#include "comptonRunConstants.h" //we don't need to re-include this
div_t div_output;
//////////////////////////////////////////////////////////////////////////
/** This calculates the Compton differential cross section asymmetry.  **/
/** The asymmetry equation below is in dimensionless units x=k/kmax    **/
/** which puts the Compton edge at x = 1.Strip # has to be converted   **/
/** to this dimensionless scale.                                       **/
/** Arguments: scattering energy in strip # and e-beam polarization    **/
/** Beam and laser parameters are set as constants.                    **/ 
//////////////////////////////////////////////////////////////////////////

TChain *mpsChain = new TChain("Mps_Tree");//chain of run segments

vector<Int_t>cutLas;//arrays of cuts for laser
vector<Int_t>cutEB;//arrays of cuts for electron beam

///////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton electron detector run laser wise and plots the results
///////////////////////////////////////////////////////////////////////////
 
Int_t edetasym(Int_t runnum1, Int_t runnum2, Bool_t isFirst100k=kFALSE)
{
  gROOT->Reset();
  gStyle->SetFillColor(0);
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1); //displays the fit parameters on the respective plots
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleW(0.6);
  gStyle->SetTitleH(0.07);
  gStyle->SetLineWidth(2);

//   Char_t textf[255],textwrite[255];
  time_t tStart = time(0), tEnd; 
  Bool_t debug  = 0;
  Bool_t debug1 = 0;//print statements with line numb
  Bool_t debug2 = 0;
  Bool_t debug3 = 0;
  Bool_t  lasOn, beamOn=kFALSE, chainExists;
  Bool_t lastTrip=kFALSE;//asserted only when we encounter the last trip
  Bool_t goodCycle=kFALSE;//tracks if a given lasCyc had no beam trip
  Int_t h = 0, l = 0;//helicity, lasOn tracking variables
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t p1Ac[2][2][nSTRIPS], p2Ac[2][2][nSTRIPS], p3Ac[2][2][nSTRIPS], p4Ac[2][2][nSTRIPS];//index:h,l,strip
//   Int_t nMpsB0H0L0[nSTRIPS] = 0,nMpsBOH0L1[nSTRIPS] = 0, nMpsB0H1L0[nSTRIPS] = 0, nMpsB0H1L1[nSTRIPS] = 0;
  Int_t  nMpsB1H1L1[nSTRIPS], nMpsB1H0L1[nSTRIPS], nMpsB1H1L0[nSTRIPS], nMpsB1H0L0[nSTRIPS];
  Int_t  unNormLasCycSum[nSTRIPS];
  Double_t lasPow[3], lasMax, helicity, comptQ[3], Qmax;//, beamMax, bcm
  Double_t pattern_number, event_number;
  //   Double_t stripAsymP1[nSTRIPS],stripAsymErP1[nSTRIPS],stripAsymRMSP1[nSTRIPS];
  Double_t stripAsymP2[nSTRIPS],stripAsymErP2[nSTRIPS],stripAsymRMSP2[nSTRIPS];//to store mean distribution
  Double_t stripAsymP3[nSTRIPS],stripAsymErP3[nSTRIPS],stripAsymRMSP3[nSTRIPS];
  Double_t stripAsymP4[nSTRIPS],stripAsymErP4[nSTRIPS],stripAsymRMSP4[nSTRIPS];

  Double_t bP2[nSTRIPS], bP3[nSTRIPS], bP4[nSTRIPS];//bP1[nSTRIPS],
  Double_t  normL1H1LasCyc[nSTRIPS], normL1H0LasCyc[nSTRIPS];
  Double_t  normL0H0LasCyc[nSTRIPS], normL0H1LasCyc[nSTRIPS];
  Double_t BCnormL1H1LasCyc[nSTRIPS], BCnormL1H0LasCyc[nSTRIPS]; 
  Double_t  BCnormLasCycSum[nSTRIPS], BCnormLasCycDiff[nSTRIPS];
  Double_t tNormLasCycAsym[nSTRIPS], LasCycAsymEr[nSTRIPS];

  char hNameP1[120],hNameErP1[120];
  char hNameP2[120],hNameErP2[120];
  char hNameP3[120],hNameErP3[120];
  char hNameP4[120],hNameErP4[120];

//  std::vector<TH1D> hAsymP1S, hAsymErP1S;
  std::vector<TH1D> hAsymP2S, hAsymErP2S;
  std::vector<TH1D> hAsymP3S, hAsymErP3S; 
  std::vector<TH1D> hAsymP4S, hAsymErP4S;

  for (Int_t s=0; s<nSTRIPS; s++) { 
//     sprintf(hNameP1,"asymPlane1Str%d",s);
//     hAsymP1S.push_back(TH1D(hNameP1,"Plane 1 stripwise asymmetry",25,-0.001,0.001));
//     sprintf(hNameP1Er,"asymErPlane1Str%d",s);
//     hAsymErP1S.push_back(TH1D(hNameErP1,"Single Strip asym stat.error",25,-0.001,0.001));

    sprintf(hNameP2,"asymPlane2Str%d",s);
    hAsymP2S.push_back(TH1D(hNameP2,"Plane 2 stripwise asymmetry",25,-0.001,0.001));
    sprintf(hNameErP2,"asymErPlane2Str%d",s);
    hAsymErP2S.push_back(TH1D(hNameErP2,"Single Strip asym stat.error",25,-0.001,0.001));

    sprintf(hNameP3,"asymPlane3Str%d",s);
    hAsymP3S.push_back(TH1D(hNameP3,"Plane 3 stripwise asymmetry",25,-0.001,0.001));
    sprintf(hNameErP3,"asymErPlane3Str%d",s);
    hAsymErP3S.push_back(TH1D(hNameErP3,"Single Strip asym stat.error",25,-0.001,0.001));

    sprintf(hNameP4,"asymPlane4Str%d",s);
    hAsymP4S.push_back(TH1D(hNameP4,"Plane 4 stripwise asymmetry",25,-0.001,0.001));
    sprintf(hNameErP4,"asymErPlane4Str%d",s);
    hAsymErP4S.push_back(TH1D(hNameErP4,"Single Strip asym stat.error",25,-0.001,0.001));
  }

  for (Int_t s=0; s<nSTRIPS; s++) {
//     hAsymP1S[s].SetBit(TH1::kCanRebin);
//     hAsymP1S[s].SetTitle(Form("Plane 1 Strip %d Asymmetry",s));
//     hAsymP1S[s].GetXaxis()->SetTitle("Asymmetry");
//     hAsymP1S[s].GetYaxis()->SetTitle("Counts");
//     hAsymErP1S[s].SetBit(TH1::kCanRebin);
//     hAsymErP1S[s].SetTitle(Form("Plane 1 Strip %d Asymmetry Error",s));
//     hAsymErP1S[s].GetXaxis()->SetTitle("Asym Error");
//     hAsymErP1S[s].GetYaxis()->SetTitle("Counts");

    hAsymP2S[s].SetBit(TH1::kCanRebin);
    hAsymP2S[s].SetTitle(Form("Plane 2 Strip %d Asymmetry",s));
    hAsymP2S[s].GetXaxis()->SetTitle("Asymmetry");
    hAsymP2S[s].GetYaxis()->SetTitle("Counts");
    hAsymErP2S[s].SetBit(TH1::kCanRebin);
    hAsymErP2S[s].SetTitle(Form("Plane 2 Strip %d Asymmetry Error",s));
    hAsymErP2S[s].GetXaxis()->SetTitle("Asym Error");
    hAsymErP2S[s].GetYaxis()->SetTitle("Counts");

    hAsymP3S[s].SetBit(TH1::kCanRebin);
    hAsymP3S[s].SetTitle(Form("Plane 3 Strip %d Asymmetry",s));
    hAsymP3S[s].GetXaxis()->SetTitle("Asymmetry");
    hAsymP3S[s].GetYaxis()->SetTitle("Counts");
    hAsymErP3S[s].SetBit(TH1::kCanRebin);
    hAsymErP3S[s].SetTitle(Form("Plane 3 Strip %d Asymmetry Error",s));
    hAsymErP3S[s].GetXaxis()->SetTitle("Asym Error");
    hAsymErP3S[s].GetYaxis()->SetTitle("Counts");

    hAsymP4S[s].SetBit(TH1::kCanRebin);
    hAsymP4S[s].SetTitle(Form("Plane 4 Strip %d Asymmetry",s));
    hAsymP4S[s].GetXaxis()->SetTitle("Asymmetry");
    hAsymP4S[s].GetYaxis()->SetTitle("Counts");
    hAsymErP4S[s].SetBit(TH1::kCanRebin);
    hAsymErP4S[s].SetTitle(Form("Plane 4 Strip %d Asymmetry Error",s));
    hAsymErP4S[s].GetXaxis()->SetTitle("Asym Error");
    hAsymErP4S[s].GetYaxis()->SetTitle("Counts");
  }

  TPaveText *pvTxt1 = new  TPaveText(0.75,0.84,0.98,1.0,"NDC");
  pvTxt1->SetBorderSize(1);
 
  /** Open either Pass1 or the First 100K **/
  for (Int_t runnum = runnum1; runnum <= runnum2; runnum++) {
    if( isFirst100k) {
      mpsChain->Add(Form("$QW_ROOTFILES/first100k_%d.root",runnum));
      chainExists = mpsChain->Add(Form("$QW_ROOTFILES/pass2/first100k_%d.root",runnum));
    }
    else {
      chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_Pass1_%d.*.root",runnum));//for Qweak Run2
    }

    if(!chainExists){//delete chains and exit if files do not exist
      cout<<"File does not exist."<<endl;
      printf("file for run # %d does not exist\n",runnum);
      delete mpsChain;
      return 0;
    }
    //   sprintf(textf,"%d_asymPerStrip.txt",runnum);
    //   ofstream outfile(Form("%s",textf));
    //   printf("%s file created\n",textf);

  }
  //printf("%d files attached to chain.\n",chainExists);! shows always only 1 file attached

  Int_t nEntries = mpsChain->GetEntries();
  printf("This chain has %i entries.\n", nEntries);

  Int_t nLasCycBeamTrips = getEBeamLasCuts(cutLas, cutEB, mpsChain);
  if (debug3) printf("nLasCycBeamTrips: %d\n",nLasCycBeamTrips);
  if (debug3) printf("cutEB.size:%d,cutLas.size:%d\n",cutEB.size(),cutLas.size());
  mpsChain->ResetBranchAddresses();//!? should it be here?
  nLasCycles = nLasCycBeamTrips%1000 - 1;
  ////first two digits of return value of getEBeamLasCuts
  nBeamTrips = nLasCycBeamTrips / 1000;
  ////fourth digit of return value of getEBeamLasCuts

  Qmax = mpsChain->GetMaximum("compton_charge/value");//!I should use sca_bcm6
  cout<<"Qmax = "<<Qmax <<endl;
  lasMax = mpsChain->GetMaximum("sca_laser_PowT/value");
  cout<<"lasMax="<<lasMax<<endl;
  cout<<"nbeamTrips="<<nBeamTrips<<endl;
  cout<<"nLasCycles="<<nLasCycles<<endl;

  mpsChain->SetBranchStatus("*",0);  ////Turn off all unused branches, for efficient looping
  mpsChain->SetBranchStatus("actual_helicity",1);
  mpsChain->SetBranchStatus("sca_laser_PowT",1);
  mpsChain->SetBranchStatus("compton_charge",1);
  mpsChain->SetBranchStatus("p*Ac",1);

  mpsChain->SetBranchAddress("event_number",&event_number);
  mpsChain->SetBranchAddress("pattern_number",&pattern_number);
  mpsChain->SetBranchAddress("actual_helicity",&helicity);
  mpsChain->SetBranchAddress("sca_laser_PowT",&lasPow);
  mpsChain->SetBranchAddress("compton_charge",&comptQ);

  //   mpsChain->SetBranchAddress("p1RawAc",&bP1);//!the old analyzer doesn't have this branch
  mpsChain->SetBranchAddress("p2RawAc",&bP2);
  mpsChain->SetBranchAddress("p3RawAc",&bP3);
  mpsChain->SetBranchAddress("p4RawAc",&bP4);
  
  for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
    if (debug) cout<<"\nStarting nCycle:"<<nCycle<<" and resetting all nCycle variables"<<endl;
    ////since this is the beginning of a new Laser cycle, and I have already assigned the 
    ////accumulated counts to a permanent variable reset the LasCyc counters
    for(Int_t s=0; s<usedStrips; s++) {
      for(h=0; h<2; h++) {
	for(Int_t l=0; l<2; l++) {
	  p1Ac[h][l][s]=0, p2Ac[h][l][s]=0, p3Ac[h][l][s]=0, p4Ac[h][l][s]=0;
	}
      }
      nMpsB1H1L1[s]= 0, nMpsB1H0L1[s]= 0, nMpsB1H1L0[s]= 0, nMpsB1H0L0[s] = 0;
      unNormLasCycSum[s]= 0;
      normL1H1LasCyc[s]= 0.0, normL1H0LasCyc[s]= 0.0;
      normL0H0LasCyc[s]= 0.0, normL0H1LasCyc[s]= 0.0;
      BCnormL1H1LasCyc[s]= 0.0, BCnormL1H0LasCyc[s]= 0.0; 
      BCnormLasCycSum[s]= 0.0, BCnormLasCycDiff[s]= 0.0;
      tNormLasCycAsym[s]= 0.0, LasCycAsymEr[s]= 0.0;
//       stripAsymP1[s]= 0.0,stripAsymErP1[s]= 0.0,stripAsymRMSP1[s]= 0.0;
      stripAsymP2[s]= 0.0,stripAsymErP2[s]= 0.0,stripAsymRMSP2[s]= 0.0;
      stripAsymP3[s]= 0.0,stripAsymErP3[s]= 0.0,stripAsymRMSP3[s]= 0.0;
      stripAsymP4[s]= 0.0,stripAsymErP4[s]= 0.0,stripAsymRMSP4[s]= 0.0;
    }
    for(Int_t i=cutLas.at(2*nCycle+1); i<cutLas.at(2*nCycle+3); i++) { 
      //loop over laser cycle periods from one LasOn state upto just before beginning of next LasOn
      if(debug && i%25000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;
      if(i<124) i = 124; //the first 122 events with unknown helicity shouldn't creep in
      if ((i >= cutLas.at(2*nCycle+1)) && (i < cutLas.at(2*nCycle+2))) l=1;
      if (i >= cutLas.at(2*nCycle+2)) l=0;
      if(!lastTrip) {
	if(i>=cutEB.at(2*nthBeamTrip)) { // && (i<cutEB.at(2*nthBeamTrip+1))) { //identifying beam-off
	  beamOn = kFALSE;//declaring beam-off
	  nthBeamTrip ++;
	  cout<<"Beam OFF at entry "<<i <<endl;
	  if (nthBeamTrip == nBeamTrips) lastTrip=kTRUE;	
	}
	else beamOn = kTRUE;
      }
      else if (i >= (cutEB.at(2*nthBeamTrip-1))) beamOn = kTRUE;

      if ((!lastTrip) ? cutLas.at(2*nCycle+2)<cutEB.at(2*nthBeamTrip) : kTRUE) goodCycle=kTRUE;
      else goodCycle=kFALSE;

      mpsChain->GetEntry(i);

      lasOn = lasPow[0] > laserFrac *lasMax;
      h = (Int_t)helicity;

      for(Int_t s=0; s<usedStrips; s++) {
	//currently the counters are only populated for beamOn cycles
	if(l==1 && h==0 && lasOn && beamOn) { //Laser stable at On & H- & Beam On
	  nMpsB1H0L1[s]++;
// 	  p1Ac[h][1][s] += (Int_t)bP1[s];
	  p2Ac[h][1][s] += (Int_t)bP2[s];
	  p3Ac[h][1][s] += (Int_t)bP3[s];
	  p4Ac[h][1][s] += (Int_t)bP4[s];
	}
	else if(l==1 && h==1 && lasOn && beamOn) { //Laser stable at On & H+ & Beam On
	  nMpsB1H1L1[s]++;
// 	  p1Ac[h][1][s] += (Int_t)bP1[s];
	  p2Ac[h][1][s] += (Int_t)bP2[s];
	  p3Ac[h][1][s] += (Int_t)bP3[s];
	  p4Ac[h][1][s] += (Int_t)bP4[s];
	}
	else if(l==0 && h==0 && beamOn) { //Laser stable at Off & H- & Beam On
	  nMpsB1H0L0[s]++;
// 	  p1Ac[h][0][s] += (Int_t)bP1[s];
	  p2Ac[h][0][s] += (Int_t)bP2[s];
	  p3Ac[h][0][s] += (Int_t)bP3[s];
	  p4Ac[h][0][s] += (Int_t)bP4[s];
	}
	else if(l==0 && h==1 && beamOn) { //Laser stable at Off & H+ & Beam On
	  nMpsB1H1L0[s]++;
// 	  p1Ac[h][0][s] += (Int_t)bP1[s];
	  p2Ac[h][0][s] += (Int_t)bP2[s];
	  p3Ac[h][0][s] += (Int_t)bP3[s];
	  p4Ac[h][0][s] += (Int_t)bP4[s];
	}
      }
    }

    //after having filled the above vectors based on laser and beam periods, its time to calculate
    if (debug) printf("\n  ****Entering laser Cycle Analysis @ nCycle : %d**** \n",nCycle);
    if (goodCycle) {
      //       for (Int_t s=0;s<=usedStrips;s++) {
      for (Int_t s=startStrip;s<=endStrip;s++) {
	if (nMpsB1H0L1[s]  <= 0 || nMpsB1H1L1[s]  <= 0) {
	  if(debug1) printf("**number of Laser ON events in strip:%d, laser nCycle:%d is ZERO**\n",s,nCycle);
	}
	else if (nMpsB1H0L0[s]  <= 0 || nMpsB1H1L0[s]  <= 0) {
	  if(debug1) printf("**number of Laser Off events in strip:%d, laser nCycle:%d is ZERO**\n",s,nCycle);
	}
	else {    
	  normL1H1LasCyc[s] = (Double_t)p2Ac[1][1][s]/nMpsB1H1L1[s];
	  if(debug2)printf(" normL1H1LasCyc:\t%f = lasOnH1:%d / nMpsB1H1L1:%d\n"
			   ,normL1H1LasCyc[s], p2Ac[1][1][s], (Int_t)nMpsB1H1L1[s] );
	  normL0H1LasCyc[s] = (Double_t)p2Ac[1][0][s]/nMpsB1H1L0[s];
	  if(debug2)printf(" normL0H1LasCyc:\t%f = lasOffH1:%d / secH1BkgdLasCyc:%d\n"
			   ,normL0H1LasCyc[s], p2Ac[1][0][s], (Int_t)nMpsB1H1L0[s] );
      
	  normL1H0LasCyc[s] = ((Double_t)p2Ac[0][1][s]/nMpsB1H0L1[s] );
	  if(debug2)printf(" normL1H0LasCyc:\t%f = lasOnH0:%d / secH0LasCyc:%d\n"
			   ,normL1H0LasCyc[s], p2Ac[0][1][s] ,(Int_t)nMpsB1H0L1[s] );
	  normL0H0LasCyc[s] = ((Double_t)p2Ac[0][0][s]/nMpsB1H0L0[s] );
	  if(debug2)printf(" normL0H0LasCyc:\t%f = lasOffH0:%d / secH0BkgdLasCyc:%d\n"
			   ,normL0H0LasCyc[s], p2Ac[0][0][s], (Int_t)nMpsB1H0L0[s] );
      
	  BCnormL1H1LasCyc[s] = (normL1H1LasCyc[s] - normL0H1LasCyc[s] );
	  BCnormL1H0LasCyc[s] = (normL1H0LasCyc[s] - normL0H0LasCyc[s] );

	  BCnormLasCycDiff[s] = (BCnormL1H1LasCyc[s] - BCnormL1H0LasCyc[s] );
	  if (debug2)printf(" BCnormLasCycDiff:\t%f\n", BCnormLasCycDiff[s] );
	  BCnormLasCycSum[s] = (BCnormL1H1LasCyc[s]  + BCnormL1H0LasCyc[s] );
	  if (debug2)printf(" BCnormLasCycSum:%f\n", BCnormLasCycSum[s] );
      
	  unNormLasCycSum[s] = p2Ac[1][1][s]  + p2Ac[0][1][s];
	  if (debug2) printf("Starting to evaluate asym\n");
	  if (debug2) printf(" unNormLasCycSum:%d\n",unNormLasCycSum[s] );
      
	  if (BCnormLasCycSum[s]  <= 0.0) {//this should never happen anyways
	    if (debug) printf("\n\n**Warning**:BCnormLasCycSum[nCycle=%d] is %f, strip=%d\n\n" ,nCycle,BCnormLasCycSum[s] ,s);
	  }
	  else {
	    tNormLasCycAsym[s] = (BCnormLasCycDiff[s] / BCnormLasCycSum[s] );
	    hAsymP2S[s].Fill(tNormLasCycAsym[s]);
	    LasCycAsymEr[s]  = 1.0/sqrt(unNormLasCycSum[s]);
	    hAsymErP2S[s].Fill(LasCycAsymEr[s]);
	    if (debug2) printf("sqrt(unNormLasCycSum ):%f\n",sqrt(unNormLasCycSum[s] ));
	    if (debug2) {
	      printf("for nCycle:%d, tNormLasCycAsym[strip:%d]= %f (stat.err:%f)\n",nCycle,s,tNormLasCycAsym[s] ,LasCycAsymEr[s] );
	      printf("formed by normalized BC (%f -/+ %f) \n",normL1H1LasCyc[s],normL1H0LasCyc[s]);
	    }
	  }
	}
      }
    }
    else cout<<"this LasCyc had a beam trip, hence skipping"<<endl;
  }
  
  TCanvas *c1 = new TCanvas("c1","c1",0,0,800,400);
  c1->Divide(2,1);
  for (Int_t s=startStrip;s<=endStrip;s++) {    
    c1->cd(1);
//     hAsymP1S[s]->Draw("H");//"H","","goff");//WHY IS My goff not working!
//     hAsymP1S[s]->Fit("gaus");
//     stripAsymP1[s] = hAsymP1S[s]->GetMean();
//     stripAsymRMSP1[s] = hAsymP1S[s]->GetRMS();

    hAsymP2S[s].Draw("H");//"H","","goff");//WHY IS My goff not working!
    hAsymP2S[s].Fit("gaus");
    stripAsymP2[s] = hAsymP2S[s].GetMean();
    stripAsymRMSP2[s] = hAsymP2S[s].GetRMS();

//     hAsymP3S[s].Draw("H");//"H","","goff");//WHY IS My goff not working!
//     hAsymP3S[s].Fit("gaus");
//     stripAsymP3[s] = hAsymP3S[s].GetMean();
//     stripAsymRMSP3[s] = hAsymP3S[s].GetRMS();

//     hAsymP4S[s].Draw("H");//"H","","goff");//WHY IS My goff not working!
//     hAsymP4S[s].Fit("gaus");
//     stripAsymP4[s] = hAsymP4S[s].GetMean();
//     stripAsymRMSP4[s] = hAsymP4S[s].GetRMS();

    c1->cd(2);
    hAsymErP2S[s].Draw("H");//,"","goff");
    stripAsymErP2[s] = hAsymErP2S[s].GetMean();
  }    
  for (Int_t s=startStrip;s<=endStrip;s++) {    
    //     printf("stripAsymP1[%d]:%f\t stripAsymErP1[%d]:%f\t stripAsymRMSP1[%d]:%f\n",s,stripAsymP1[s],s,stripAsymErP1[s],s,stripAsymRMSP1[s]);
    printf("stripAsymP2[%d]:%f\t stripAsymErP2[%d]:%f\t stripAsymRMSP2[%d]:%f\n",s,stripAsymP2[s],s,stripAsymErP2[s],s,stripAsymRMSP2[s]);
//     printf("stripAsymP3[%d]:%f\t stripAsymErP3[%d]:%f\t stripAsymRMSP3[%d]:%f\n",s,stripAsymP3[s],s,stripAsymErP3[s],s,stripAsymRMSP3[s]);
//     printf("stripAsymP4[%d]:%f\t stripAsymErP4[%d]:%f\t stripAsymRMSP4[%d]:%f\n",s,stripAsymP4[s],s,stripAsymErP4[s],s,stripAsymRMSP4[s]);
  }

  //  delete hAsymP3S[0];

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to complete.\n",div_output.quot,div_output.rem );
  return 1;
}

/******************************************************
Querries:
1. how to fix the memory leak compalin ?
2. printing the data into a file rather than on the terminal
3. plotting the data
******************************************************/
