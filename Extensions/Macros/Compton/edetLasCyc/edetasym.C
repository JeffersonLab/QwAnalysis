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
  //   Int_t nMpsH0 = 0,nMpsH1 = 0, nMpsBkgd = 0;
  Int_t p1Ac[2][2][nSTRIPS], p2Ac[2][2][nSTRIPS],p3Ac[2][2][nSTRIPS],p4Ac[2][2][nSTRIPS];//index:h,l,strip
  Double_t lasPow[3], lasMax, helicity, comptQ[3], Qmax;//, beamMax, bcm
  Double_t pattern_number, event_number;
  //   Double_t stripAsymP1[nSTRIPS],stripAsymErP1[nSTRIPS],stripAsymRMSP1[nSTRIPS];
  Double_t stripAsymP2[nSTRIPS],stripAsymErP2[nSTRIPS],stripAsymRMSP2[nSTRIPS];//to store the mean distribution for all strips
  Double_t stripAsymP3[nSTRIPS],stripAsymErP3[nSTRIPS],stripAsymRMSP3[nSTRIPS];
  Double_t stripAsymP4[nSTRIPS],stripAsymErP4[nSTRIPS],stripAsymRMSP4[nSTRIPS];

  Double_t bP2[nSTRIPS], bP3[nSTRIPS], bP4[nSTRIPS];//bP1[nSTRIPS],
  //   Int_t lasOnH1[nSTRIPS], lasOnH0[nSTRIPS], lasOffH1[nSTRIPS], lasOffH0[nSTRIPS];
  Int_t  nMpsLasOnH1[nSTRIPS], nMpsLasOnH0[nSTRIPS], nMpsLasOffH1[nSTRIPS], nMpsLasOffH0[nSTRIPS];
  Int_t  unNormLasCycSum[nSTRIPS];
  Double_t  normLasOnHpLasCyc[nSTRIPS], normLasOnHmLasCyc[nSTRIPS];
  Double_t  normLasOffHmLasCyc[nSTRIPS], normLasOffHpLasCyc[nSTRIPS];
  Double_t BCnormLasOnHpLasCyc[nSTRIPS], BCnormLasOnHmLasCyc[nSTRIPS]; 
  Double_t  BCnormLasCycSum[nSTRIPS], BCnormLasCycDiff[nSTRIPS];
  Double_t tNormLasCycAsym[nSTRIPS], LasCycAsymEr[nSTRIPS];

  char hName[120],hNameEr[120];
  TH1D *hAsymP2S[nSTRIPS]; 
  TH1D *hAsymErP2S[nSTRIPS];

  for (Int_t s=0; s<nSTRIPS; s++) {
    sprintf(hName,"asymPlane2Str%d",s);
    hAsymP2S[s]= new TH1D(hName,"Plane 2 stripwise asymmetry",25,-0.001,0.001);
    sprintf(hNameEr,"asymErPlane2Str%d",s);
    hAsymErP2S[s]= new TH1D(hNameEr,"Single Strip asym stat.error",25,-0.001,0.001);
  }
  
  for (Int_t s=0; s<nSTRIPS; s++) {
    hAsymP2S[s]->SetBit(TH1::kCanRebin);
    hAsymP2S[s]->SetLineColor(kBlue);
    hAsymP2S[s]->SetTitle(Form("Plane 2 Strip %d Asymmetry",s));
    hAsymP2S[s]->GetXaxis()->SetTitle("Asymmetry");
    hAsymP2S[s]->GetYaxis()->SetTitle("Counts");

    hAsymErP2S[s]->SetBit(TH1::kCanRebin);
    hAsymErP2S[s]->SetLineColor(kBlue);
    hAsymErP2S[s]->SetTitle(Form("Plane 2 Strip %d Asymmetry Error",s));
    hAsymErP2S[s]->GetXaxis()->SetTitle("Asym Error");
    hAsymErP2S[s]->GetYaxis()->SetTitle("Counts");
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
    //   sprintf(textf,"%d_meanCtsPerStrip.txt",runnum);
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
      //       lasOnH1[s]= 0, lasOnH0[s]= 0, lasOffH1[s]= 0, lasOffH0[s]= 0;
      nMpsLasOnH1[s]= 0, nMpsLasOnH0[s]= 0, nMpsLasOffH1[s]= 0, nMpsLasOffH0[s] = 0;
      unNormLasCycSum[s]= 0;
      normLasOnHpLasCyc[s]= 0.0, normLasOnHmLasCyc[s]= 0.0;
      normLasOffHmLasCyc[s]= 0.0, normLasOffHpLasCyc[s]= 0.0;
      BCnormLasOnHpLasCyc[s]= 0.0, BCnormLasOnHmLasCyc[s]= 0.0; 
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
	  // 	  nMpsH0++;
	  nMpsLasOnH0[s]++;
// 	  p1Ac[h][1][s] += (Int_t)bP1[s];
	  p2Ac[h][1][s] += (Int_t)bP2[s];
	  p3Ac[h][1][s] += (Int_t)bP3[s];
	  p4Ac[h][1][s] += (Int_t)bP4[s];
	}
	else if(l==1 && h==1 && lasOn && beamOn) { //Laser stable at On & H+ & Beam On
	  // 	  nMpsH1++;      
	  nMpsLasOnH1[s]++;
// 	  p1Ac[h][1][s] += (Int_t)bP1[s];
	  p2Ac[h][1][s] += (Int_t)bP2[s];
	  p3Ac[h][1][s] += (Int_t)bP3[s];
	  p4Ac[h][1][s] += (Int_t)bP4[s];
	}
	else if(l==0 && h==0 && beamOn) { //Laser stable at Off & H- & Beam On
	  // 	  nMpsBkgd++;      
	  nMpsLasOffH0[s]++;
// 	  p1Ac[h][0][s] += (Int_t)bP1[s];
	  p2Ac[h][0][s] += (Int_t)bP2[s];
	  p3Ac[h][0][s] += (Int_t)bP3[s];
	  p4Ac[h][0][s] += (Int_t)bP4[s];
	}
	else if(l==0 && h==1 && beamOn) { //Laser stable at Off & H+ & Beam On
	  // 	  nMpsBkgd++;      
	  nMpsLasOffH1[s]++;
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
	if (nMpsLasOnH0[s]  <= 0 || nMpsLasOnH1[s]  <= 0) {
	  if(debug1) printf("**number of Laser ON events in strip:%d, laser nCycle:%d is ZERO**\n",s,nCycle);
	}
	else if (nMpsLasOffH0[s]  <= 0 || nMpsLasOffH1[s]  <= 0) {
	  if(debug1) printf("**number of Laser Off events in strip:%d, laser nCycle:%d is ZERO**\n",s,nCycle);
	}
	else {    
	  normLasOnHpLasCyc[s] = (Double_t)p2Ac[1][1][s]/nMpsLasOnH1[s];
	  if(debug2)printf(" normLasOnHpLasCyc:\t%f = lasOnH1:%d / nMpsLasOnH1:%d\n"
			   ,normLasOnHpLasCyc[s], p2Ac[1][1][s], (Int_t)nMpsLasOnH1[s] );
	  normLasOffHpLasCyc[s] = (Double_t)p2Ac[1][0][s]/nMpsLasOffH1[s];
	  if(debug2)printf(" normLasOffHpLasCyc:\t%f = lasOffH1:%d / secH1BkgdLasCyc:%d\n"
			   ,normLasOffHpLasCyc[s], p2Ac[1][0][s], (Int_t)nMpsLasOffH1[s] );
      
	  normLasOnHmLasCyc[s] = ((Double_t)p2Ac[0][1][s]/nMpsLasOnH0[s] );
	  if(debug2)printf(" normLasOnHmLasCyc:\t%f = lasOnH0:%d / secH0LasCyc:%d\n"
			   ,normLasOnHmLasCyc[s], p2Ac[0][1][s] ,(Int_t)nMpsLasOnH0[s] );
	  normLasOffHmLasCyc[s] = ((Double_t)p2Ac[0][0][s]/nMpsLasOffH0[s] );
	  if(debug2)printf(" normLasOffHmLasCyc:\t%f = lasOffH0:%d / secH0BkgdLasCyc:%d\n"
			   ,normLasOffHmLasCyc[s], p2Ac[0][0][s], (Int_t)nMpsLasOffH0[s] );
      
	  BCnormLasOnHpLasCyc[s] = (normLasOnHpLasCyc[s] - normLasOffHpLasCyc[s] );
	  BCnormLasOnHmLasCyc[s] = (normLasOnHmLasCyc[s] - normLasOffHmLasCyc[s] );

	  BCnormLasCycDiff[s] = (BCnormLasOnHpLasCyc[s] - BCnormLasOnHmLasCyc[s] );
	  if (debug2)printf(" BCnormLasCycDiff:\t%f\n", BCnormLasCycDiff[s] );
	  BCnormLasCycSum[s] = (BCnormLasOnHpLasCyc[s]  + BCnormLasOnHmLasCyc[s] );
	  if (debug2)printf(" BCnormLasCycSum:%f\n", BCnormLasCycSum[s] );
      
	  unNormLasCycSum[s] = p2Ac[1][1][s]  + p2Ac[0][1][s];
	  if (debug2) printf("Starting to evaluate asym\n");
	  if (debug2) printf(" unNormLasCycSum:%d\n",unNormLasCycSum[s] );
      
	  if (BCnormLasCycSum[s]  <= 0.0) {//this should never happen anyways
	    if (debug) printf("\n\n**Warning**:BCnormLasCycSum[nCycle=%d] is %f, strip=%d\n\n" ,nCycle,BCnormLasCycSum[s] ,s);
	  }
	  else {
	    tNormLasCycAsym[s] = (BCnormLasCycDiff[s] / BCnormLasCycSum[s] );
	    hAsymP2S[s]->Fill(tNormLasCycAsym[s]);
	    LasCycAsymEr[s]  = 1.0/sqrt(unNormLasCycSum[s]);
	    hAsymErP2S[s]->Fill(LasCycAsymEr[s]);
	    if (debug2) printf("sqrt(unNormLasCycSum ):%f\n",sqrt(unNormLasCycSum[s] ));
	    if (debug2) {
	      printf("for nCycle:%d, tNormLasCycAsym[strip:%d]= %f (stat.err:%f)\n",nCycle,s,tNormLasCycAsym[s] ,LasCycAsymEr[s] );
	      printf("formed by normalized BC (%f -/+ %f) \n",normLasOnHpLasCyc[s],normLasOnHmLasCyc[s]);
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
    hAsymP2S[s]->Draw("H");//"H","","goff");//WHY IS My goff not working!
    hAsymP2S[s]->Fit("gaus");
    stripAsymP2[s] = hAsymP2S[s]->GetMean();
    stripAsymRMSP2[s] = hAsymP2S[s]->GetRMS();

    c1->cd(2);
    hAsymErP2S[s]->Draw("H");//,"","goff");
    stripAsymErP2[s] = hAsymErP2S[s]->GetMean();
  }    
  for (Int_t s=startStrip;s<=endStrip;s++) {    
    //     printf("stripAsymP1[%d]:%f\t stripAsymErP1[%d]:%f\t stripAsymRMSP1[%d]:%f\n",s,stripAsymP1[s],s,stripAsymErP1[s],s,stripAsymRMSP1[s]);
    printf("stripAsymP2[%d]:%f\t stripAsymErP2[%d]:%f\t stripAsymRMSP2[%d]:%f\n",s,stripAsymP2[s],s,stripAsymErP2[s],s,stripAsymRMSP2[s]);
    //     printf("stripAsymP3[%d]:%f\t stripAsymErP3[%d]:%f\t stripAsymRMSP3[%d]:%f\n",s,stripAsymP3[s],s,stripAsymErP3[s],s,stripAsymRMSP3[s]);
    //     printf("stripAsymP4[%d]:%f\t stripAsymErP4[%d]:%f\t stripAsymRMSP4[%d]:%f\n",s,stripAsymP4[s],s,stripAsymErP4[s],s,stripAsymRMSP4[s]);
  }

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to complete.\n",div_output.quot,div_output.rem );
  return 1;
}
