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
  Bool_t debug1 = 0;
  Bool_t debug2 = 0;
  Bool_t debug3 = 0;//print statements with line numb
  Bool_t  lasOn, beamOn=kFALSE, chainExists;
//   Bool_t lastTrip=kFALSE;//asserted only when we encounter the last trip
//   Bool_t goodCycle=kFALSE;//tracks if a given lasCyc had no beam trip
  Int_t h = 0, l = 0;//helicity, lasOn tracking variables
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t p1Ac[2][2][nStrips], p2Ac[2][2][nStrips], p3Ac[2][2][nStrips], p4Ac[2][2][nStrips];//index:h,l,strip
  //   Int_t nMpsB0H0L0[nStrips] = 0,nMpsBOH0L1[nStrips] = 0, nMpsB0H1L0[nStrips] = 0, nMpsB0H1L1[nStrips] = 0;
  Int_t  nMpsB1H1L1, nMpsB1H0L1, nMpsB1H1L0, nMpsB1H0L0;
  Int_t  unNormLasCycSum[nPlanes][nStrips];
  Double_t lasPow[3], lasMax, helicity, comptQ[3], Qmax;//, beamMax, bcm
  Double_t pattern_number, event_number;
  Double_t bP2[nStrips], bP3[nStrips], bP4[nStrips];//bP1[nStrips],
  //   Double_t stripAsymP1[nStrips],stripAsymErP1[nStrips],stripAsymRMSP1[nStrips];
  Double_t stripAsymP2[nStrips],stripAsymErP2[nStrips],stripAsymRMSP2[nStrips];//to store mean distribution
  Double_t stripAsymP3[nStrips],stripAsymErP3[nStrips],stripAsymRMSP3[nStrips];
  Double_t stripAsymP4[nStrips],stripAsymErP4[nStrips],stripAsymRMSP4[nStrips];
  Double_t stripPlot[nStrips];//..!for plotting may not be required later
  Double_t normL1H1LasCyc[nPlanes][nStrips], normL1H0LasCyc[nPlanes][nStrips];
  Double_t normL0H0LasCyc[nPlanes][nStrips], normL0H1LasCyc[nPlanes][nStrips];
  Double_t BCnormL1H1LasCyc[nPlanes][nStrips], BCnormL1H0LasCyc[nPlanes][nStrips]; 
  Double_t BCnormLasCycSum[nPlanes][nStrips], BCnormLasCycDiff[nPlanes][nStrips];
  Double_t tNormLasCycAsym[nPlanes][nStrips], LasCycAsymEr[nPlanes][nStrips];

  //   char hNameP1[120],hNameErP1[120];
  char hNameP2[120],hNameErP2[120];
  char hNameP3[120],hNameErP3[120];
  char hNameP4[120],hNameErP4[120];

  //  std::vector<TH1D> hAsymP1S, hAsymErP1S;
  std::vector<TH1D> hAsymP2S, hAsymErP2S;
  std::vector<TH1D> hAsymP3S, hAsymErP3S; 
  std::vector<TH1D> hAsymP4S, hAsymErP4S;

  for (Int_t s=0; s<nStrips; s++) { 
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

  for (Int_t s=0; s<nStrips; s++) {
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
      chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_Pass1_%d.*.root",runnum));//for Run2
//       cout<<"max compton_charge"<< mpsChain->GetMaximum("compton_chargee")<<endl;
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
  if (debug) printf("cutEB.size:%d,cutLas.size:%d\n",cutEB.size(),cutLas.size());
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
    nMpsB1H1L1= 0, nMpsB1H0L1= 0, nMpsB1H1L0= 0, nMpsB1H0L0= 0;
    for(Int_t s = startStrip; s <= endStrip; s++) {
      for(h=0; h<2; h++) {
	for(Int_t l=0; l<2; l++) {
	  p1Ac[h][l][s]=0, p2Ac[h][l][s]=0, p3Ac[h][l][s]=0, p4Ac[h][l][s]=0;
	}
      }
      //       stripAsymP1[s]= 0.0,stripAsymErP1[s]= 0.0,stripAsymRMSP1[s]= 0.0;
      stripAsymP2[s]= 0.0,stripAsymErP2[s]= 0.0,stripAsymRMSP2[s]= 0.0;
      stripAsymP3[s]= 0.0,stripAsymErP3[s]= 0.0,stripAsymRMSP3[s]= 0.0;
      stripAsymP4[s]= 0.0,stripAsymErP4[s]= 0.0,stripAsymRMSP4[s]= 0.0;
    }

    for(Int_t p = startPlane; p <=endPlane; p++) {
      for(Int_t s = startStrip; s <= endStrip; s++) {
	unNormLasCycSum[p][s]= 0;
	normL1H1LasCyc[p][s]= 0.0, normL1H0LasCyc[p][s]= 0.0;
	normL0H0LasCyc[p][s]= 0.0, normL0H1LasCyc[p][s]= 0.0;
	BCnormL1H1LasCyc[p][s]= 0.0, BCnormL1H0LasCyc[p][s]= 0.0; 
	BCnormLasCycSum[p][s]= 0.0, BCnormLasCycDiff[p][s]= 0.0;
	tNormLasCycAsym[p][s]= 0.0, LasCycAsymEr[p][s]= 0.0;
      }
    }

    if(nthBeamTrip < nBeamTrips) {
      if(cutLas.at(2*nCycle+3)<cutEB.at(2*nthBeamTrip)) {
	//will work for the beginning when there has been no beamtrip yet
	//!?what if the run starts with a beamTrip
	//!?what if the run ends with a beamTrip
	//!?what if there is no beamTrip
// 	goodCycle = kTRUE;
	beamOn = kTRUE;
      }
      else {
// 	goodCycle = kFALSE;
	beamOn = kFALSE;
	nthBeamTrip++;
      }
    }
    else {//processing the last beamTrip
      if(nthBeamTrip != nBeamTrips) cout<<"\n***Error in BeamTrip evaluation***\n"<<endl;
      if (cutLas.at(2*nCycle+1)>cutEB.at(2*nthBeamTrip-1)) { //this happens when run ends with beamOn
// 	goodCycle = kTRUE;
 	beamOn = kTRUE;	
      }
      else {
// 	goodCycle = kFALSE;
 	beamOn = kFALSE;
      }
    }
      
    for(Int_t i =cutLas.at(2*nCycle+1); i <cutLas.at(2*nCycle+3); i++) { 
      //loop over laser cycle periods from one LasOn state upto just before beginning of next LasOn
      if(debug && i%100000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;
      if(i<124) i = 124; //the first 122 events with unknown helicity shouldn't creep in
      if ((i >= cutLas.at(2*nCycle+1)) && (i < cutLas.at(2*nCycle+2))) l=1;
      if (i >= cutLas.at(2*nCycle+2)) l=0;

      mpsChain->GetEntry(i);
      lasOn = lasPow[0] > laserFrac *lasMax;//!this part should be directly done in getEBeamLasCuts.C
      h = (Int_t)helicity;

      if ((l==1) && lasOn) l=1; //making the laser On requirement more strict
      if (h ==0 && l==0) nMpsB1H0L0++;
      if (h ==0 && l==1) nMpsB1H0L1++;
      if (h ==1 && l==0) nMpsB1H1L0++;
      if (h ==1 && l==1) nMpsB1H1L1++;

      if ((h ==1 || h ==0) && beamOn) { //to avoid the h=-9999 that appears in beginning of every runlet
	////currently the counters are only populated for beamOn cycles
	for(Int_t s =startStrip; s <=endStrip; s++) {
	  // 	  p1Ac[h][l][s] += (Int_t)bP1[s];
	  p2Ac[h][l][s] += (Int_t)bP2[s];
	  p3Ac[h][l][s] += (Int_t)bP3[s];
	  p4Ac[h][l][s] += (Int_t)bP4[s];
	}
      }
    }///for(Int_t i =cutLas.at(2*nCycle+1); i <cutLas.at(2*nCycle+3); i++)

    //after having filled the above vectors based on laser and beam periods, its time to calculate
    if (debug1) printf("\n  ****Entering laser Cycle Analysis @ nCycle : %d**** \n",nCycle);
    if (beamOn) {
      cout<<"the nCycle: "<<nCycle<<" has 'beamOn': "<<beamOn<<endl;
      if (nMpsB1H0L1<= 0 || nMpsB1H1L1<= 0 || nMpsB1H0L0<= 0 || nMpsB1H1L0<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check nMpsB1H0L1:%d,nMpsB1H1L1:%d, nMpsB1H0L0:%d, nMpsB1H1L0:%d**\n",
	       nCycle,nMpsB1H0L1,nMpsB1H1L1,nMpsB1H0L0,nMpsB1H1L0);
      else {
	for (Int_t s =startStrip; s <=endStrip;s++) {	  
	  // 	  normL1H1LasCyc[1][s] = (Double_t) p1Ac[1][1][s]/nMpsB1H1L1;
	  // 	  normL0H1LasCyc[1][s] = (Double_t) p1Ac[1][0][s]/nMpsB1H1L0;
	  // 	  normL1H0LasCyc[1][s] = ((Double_t) p1Ac[0][1][s]/nMpsB1H0L1 );
	  // 	  normL0H0LasCyc[1][s] = ((Double_t) p1Ac[0][0][s]/nMpsB1H0L0 );
	  
	  normL1H1LasCyc[2][s] = (Double_t)p2Ac[1][1][s]/nMpsB1H1L1;
	  if(debug2)printf(" normL1H1LasCyc:\t%f = lasOnH1:%d / nMpsB1H1L1:%d\n"
			   ,normL1H1LasCyc[2][s], p2Ac[1][1][s], nMpsB1H1L1 );
	  normL0H1LasCyc[2][s] = (Double_t)p2Ac[1][0][s]/nMpsB1H1L0;
	  if(debug2)printf(" normL0H1LasCyc:\t%f = lasOffH1:%d / secH1BkgdLasCyc:%d\n"
			   ,normL0H1LasCyc[2][s], p2Ac[1][0][s], nMpsB1H1L0 );      
	  normL1H0LasCyc[2][s] = (Double_t)p2Ac[0][1][s]/nMpsB1H0L1;
	  if(debug2)printf(" normL1H0LasCyc:\t%f = lasOnH0:%d / secH0LasCyc:%d\n"
			   ,normL1H0LasCyc[2][s], p2Ac[0][1][s], nMpsB1H0L1 );
	  normL0H0LasCyc[2][s] = (Double_t)p2Ac[0][0][s]/nMpsB1H0L0;
 	  if(debug2)printf(" normL0H0LasCyc:\t%f = lasOffH0:%d / secH0BkgdLasCyc:%d\n"
			   ,normL0H0LasCyc[2][s], p2Ac[0][0][s], nMpsB1H0L0 );
      
	  normL1H1LasCyc[3][s] = (Double_t)p3Ac[1][1][s]/nMpsB1H1L1;
	  normL0H1LasCyc[3][s] = (Double_t)p3Ac[1][0][s]/nMpsB1H1L0;
	  normL1H0LasCyc[3][s] = (Double_t)p3Ac[0][1][s]/nMpsB1H0L1;
	  normL0H0LasCyc[3][s] = (Double_t)p3Ac[0][0][s]/nMpsB1H0L0;
     
	  normL1H1LasCyc[4][s] = (Double_t)p4Ac[1][1][s]/nMpsB1H1L1;
	  normL0H1LasCyc[4][s] = (Double_t)p4Ac[1][0][s]/nMpsB1H1L0;
	  normL1H0LasCyc[4][s] = (Double_t)p4Ac[0][1][s]/nMpsB1H0L1;
	  normL0H0LasCyc[4][s] = (Double_t)p4Ac[0][0][s]/nMpsB1H0L0;
      
	  unNormLasCycSum[1][s] = p1Ac[1][1][s] + p1Ac[0][1][s];
	  unNormLasCycSum[2][s] = p2Ac[1][1][s] + p2Ac[0][1][s];
	  unNormLasCycSum[3][s] = p3Ac[1][1][s] + p3Ac[0][1][s];
	  unNormLasCycSum[4][s] = p4Ac[1][1][s] + p4Ac[0][1][s];
	}
	for (Int_t p =startPlane; p <=endPlane; p++) {	  	  
	  for (Int_t s =startStrip; s <=endStrip;s++) {	  
	    BCnormL1H1LasCyc[p][s] = (normL1H1LasCyc[p][s] - normL0H1LasCyc[p][s]);
	    BCnormL1H0LasCyc[p][s] = (normL1H0LasCyc[p][s] - normL0H0LasCyc[p][s]);

	    BCnormLasCycDiff[p][s] = (BCnormL1H1LasCyc[p][s] - BCnormL1H0LasCyc[p][s]);
	    BCnormLasCycSum[p][s] = (BCnormL1H1LasCyc[p][s]  + BCnormL1H0LasCyc[p][s]);
	    if (BCnormLasCycSum[p][s]  <= 0.0) 
	      printf("\n**Warning**:BCnormLasCycSum[p%d][s%d] is %f in nCycle:%d\n",p,s,BCnormLasCycSum[p][s],nCycle);	
	    else tNormLasCycAsym[p][s] = (BCnormLasCycDiff[p][s] / BCnormLasCycSum[p][s]);

	    if (unNormLasCycSum[p][s]<=0) printf("***Warning: unNormLasCycSum[p%d][s%d]:%d\n",p,s,unNormLasCycSum[p][s]);
	    else LasCycAsymEr[p][s]  = 1.0/sqrt(unNormLasCycSum[p][s]);

	    if (debug2) {
	      printf("for nCycle:%d, tNormLasCycAsym[p%d][s%d]= %f (stat.err:%f)\n",nCycle,p,s,tNormLasCycAsym[p][s] ,LasCycAsymEr[p][s]);
	      printf("formed by normalized BC (%f -/+ %f) \n",normL1H1LasCyc[p][s],normL1H0LasCyc[p][s]);
	    }
	  }
	}
	for (Int_t s =startStrip; s <=endStrip;s++) {	  
	  // 	  hAsymP1S[s].Fill(tNormLasCycAsym[1][s]);
	  hAsymP2S[s].Fill(tNormLasCycAsym[2][s]);
	  hAsymP3S[s].Fill(tNormLasCycAsym[3][s]);
	  hAsymP4S[s].Fill(tNormLasCycAsym[4][s]);	  
	  // 	  hAsymErP1S[s].Fill(LasCycAsymEr[2][s]);
	  hAsymErP2S[s].Fill(LasCycAsymEr[2][s]);
	  hAsymErP3S[s].Fill(LasCycAsymEr[2][s]);
	  hAsymErP4S[s].Fill(LasCycAsymEr[2][s]);
	}
      }
    }///if (beamOn)
    else cout<<"this LasCyc(nCycle:"<<nCycle<<") had a beam trip, hence skipping"<<endl;
  }
  
//   TCanvas *c1 = new TCanvas("c1","edetAsymmetry Plane1",0,0,800,400);
  TCanvas *c2 = new TCanvas("c2","Plane2 Asymmetry Vs Strip number",200,10,600,400);
  TCanvas *c3 = new TCanvas("c3","Plane3 Asymmetry Vs Strip number",200,10,600,400);
  TCanvas *c4 = new TCanvas("c4","Plane4 Asymmetry Vs Strip number",200,10,600,400);

  for (Int_t s=startStrip; s<=endStrip;s++) {    
    //      c1->cd(1);
    //     hAsymP1S[s]->Draw("H");//"H","","goff");//WHY IS My goff not working!
    //     hAsymP1S[s]->Fit("gaus");
    //     stripAsymP1[s] = hAsymP1S[s]->GetMean();
    //     stripAsymRMSP1[s] = hAsymP1S[s]->GetRMS();
    
    hAsymP2S[s].Draw("");//"H","","goff");//WHY IS My goff not working!
    hAsymP2S[s].Fit("gaus");
    stripAsymP2[s] = hAsymP2S[s].GetMean();
    stripAsymRMSP2[s] = hAsymP2S[s].GetRMS();
    
    hAsymP3S[s].Draw("H");//,"","goff");//WHY IS My goff not working!
    hAsymP3S[s].Fit("gaus");
    stripAsymP3[s] = hAsymP3S[s].GetMean();
    stripAsymRMSP3[s] = hAsymP3S[s].GetRMS();
    
    hAsymP4S[s].Draw("H");//"H","","goff");//WHY IS My goff not working!
    hAsymP4S[s].Fit("gaus");
    stripAsymP4[s] = hAsymP4S[s].GetMean();
    stripAsymRMSP4[s] = hAsymP4S[s].GetRMS();
    
    //    hAsymErP1S[s].Draw("H");//,"","goff");
    //    stripAsymErP1[s] = hAsymErP1S[s].GetMean();
    
    hAsymErP2S[s].Draw("H");//,"","goff");
    stripAsymErP2[s] = hAsymErP2S[s].GetMean();
    
     hAsymErP3S[s].Draw("H");//,"","goff");
    stripAsymErP3[s] = hAsymErP3S[s].GetMean();
    
    hAsymErP4S[s].Draw("H");//,"","goff");
    stripAsymErP4[s] = hAsymErP4S[s].GetMean();
  }

//     for (Int_t s=startStrip;s<=endStrip;s++) {    
  //     printf("stripAsymP1[%d]:%f\t stripAsymErP1[%d]:%f\t stripAsymRMSP1[%d]:%f\n",s,stripAsymP1[s],s,stripAsymErP1[s],s,stripAsymRMSP1[s]);
  //     printf("asymP2S[%d]:%f; asymErP2S[%d]:%f; asymRMSP2S[%d]:%f\n",s,stripAsymP2[s],s,stripAsymErP2[s],s,stripAsymRMSP2[s]);
//        printf("asymP3S[%d]:%f; asymErP3S[%d]:%f; asymRMSP3S[%d]:%f\n",s,stripAsymP3[s],s,stripAsymErP3[s],s,stripAsymRMSP3[s]);
  //     printf("asymP4S[%d]:%f; asymErP4S[%d]:%f; asymRMSP4S[%d]:%f\n",s,stripAsymP4[s],s,stripAsymErP4[s],s,stripAsymRMSP4[s]);
//     }

  Double_t zero[nStrips];
  for (Int_t s =startStrip; s <=endStrip;s++) {
    stripPlot[s]=s+1;
    zero[s] =0;
  }
//   TGraphErrors *grP1 = new TGraphErrors(nStrips,stripPlot,stripAsymP1,zero,stripAsymRMSP1);
  TGraphErrors *grP2 = new TGraphErrors(nStrips,stripPlot,stripAsymP2,zero,stripAsymRMSP2);
  TGraphErrors *grP3 = new TGraphErrors(nStrips,stripPlot,stripAsymP3,zero,stripAsymRMSP3);
  TGraphErrors *grP4 = new TGraphErrors(nStrips,stripPlot,stripAsymP4,zero,stripAsymRMSP4);
  TLine *myline = new TLine(0,0,60,0);
  myline->SetLineStyle(1);

//   grP1->GetXaxis()->SetTitle("strip number");
//   grP1->GetYaxis()->SetTitle("asymmetry");
//   grP1->SetTitle("Asymmetry Vs Strip number");
//   grP1->Draw("A*");
//   c1->Update();

  c2->cd();
  grP2->GetXaxis()->SetTitle("strip number");
  grP2->GetYaxis()->SetTitle("asymmetry");
  grP2->SetTitle("Plane2 Asymmetry Vs Strip number");
  grP2->Draw("A*");
  myline->Draw();
  c2->Update();

  c3->cd();
  grP3->GetXaxis()->SetTitle("strip number");
  grP3->GetYaxis()->SetTitle("asymmetry");
  grP3->SetTitle("Plane3 Asymmetry Vs Strip number");
  grP3->Draw("A*");
  myline->Draw();
  c3->Update();

  c4->cd();
  grP4->GetXaxis()->SetTitle("strip number");
  grP4->GetYaxis()->SetTitle("asymmetry");
  grP4->SetTitle("Plane4 Asymmetry Vs Strip number");
  grP4->Draw("A*");
  myline->Draw();
  c4->Update();

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to complete.\n",div_output.quot,div_output.rem );
  return 1;
}

/******************************************************
Querries:
1. 
2. printing the data into a file rather than on the terminal
3. plotting the data
******************************************************/
