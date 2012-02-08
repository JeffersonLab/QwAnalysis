////Don Jones and Juan Carlos helped significantly in the development of this code
#include <rootClass.h>
#include "getEBeamLasCuts.C"
#include "maskedStrips.C"

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
//This program analyzes a Compton electron detector run laser wise and plots the ...
///////////////////////////////////////////////////////////////////////////
 
Int_t edetExpAsym(Int_t runnum1, Int_t runnum2, Float_t stripAsym[nPlanes][nStrips], Float_t stripAsymEr[nPlanes][nStrips], Float_t stripAsymRMS[nPlanes][nStrips], Bool_t isFirst100k=kFALSE)
{
  Char_t textf[255],textwrite[255];
  time_t tStart = time(0), tEnd; 
  Bool_t debug = 1, debug1 = 0, debug2 = 0;
  Bool_t lasOn, beamOn =kFALSE;
  Int_t chainExists = 0;
  Int_t nLasCycBeamTrips=0,goodCycle=0;
  Int_t h = 0, l = 0;//helicity, lasOn tracking variables
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t nMpsB1H1L1, nMpsB1H0L1, nMpsB1H1L0, nMpsB1H0L0;
  //Int_t entry;
  Float_t AccumB1H0L0[nPlanes][nStrips],AccumB1H0L1[nPlanes][nStrips],AccumB1H1L0[nPlanes][nStrips],AccumB1H1L1[nPlanes][nStrips];
  Double_t comptQH1L1, comptQH0L1, comptQH1L0, comptQH0L0;
  Double_t lasPowB1H1, lasPowB1H0; //, lasPowB0H1, lasPowB0H0;
  Double_t unNormLasCycSum[nPlanes][nStrips];
  Double_t lasPow[3], lasMax, helicity, bcm[3], Qmax;
  Double_t pattern_number, event_number;
  Double_t bRawAccum[nPlanes][nStrips];
  //  Double_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips],stripAsymRMS[nPlanes][nStrips] ;
  Float_t normAcB1H1L1LasCyc[nPlanes][nStrips], normAcB1H0L1LasCyc[nPlanes][nStrips];
  Float_t normAcB1H0L0LasCyc[nPlanes][nStrips], normAcB1H1L0LasCyc[nPlanes][nStrips];
  Float_t BCnormAcB1H1L1LasCyc[nPlanes][nStrips], BCnormAcB1H0L1LasCyc[nPlanes][nStrips]; //Background Corrected
  Float_t BCnormLasCycSum[nPlanes][nStrips], BCnormLasCycDiff[nPlanes][nStrips];
  Double_t qNormLasCycAsym[nPlanes][nStrips], LasCycAsymEr[nPlanes][nStrips];

  TH1D *h1[10], *h2[10]; //!temp test histograms
  char hName[nPlanes][120],hNameEr[nPlanes][120];

  std::vector<std::vector<TH1D> > hAsymPS;
  std::vector<std::vector<TH1D> > hAsymErPS;
  gStyle->SetOptFit(1);
  gStyle->SetOptStat(1);
  
  sprintf(textf,"r%d_expAsymP1.txt",runnum);
  ofstream outfileExpAsymP1(Form("%s",textf));
  printf("%s file created\n",textf);

//   sprintf(textf,"r%d_cutLas.txt",runnum);
//   ifstream infileLas(Form("%s",textf));
  
//   sprintf(textf,"r%d_cutBeam.txt",runnum);
//   ifstream infileBeam(Form("%s",textf));
  
  for (Int_t p =0; p <nPlanes; p++) {   
    if (p >= (Int_t) hAsymPS.size()) {
      hAsymPS.resize(p+1);
      hAsymErPS.resize(p+1);
    }
    for (Int_t s =0; s<nStrips; s++) { 
      sprintf(hName[p],"asymPlane%d_Str%d",p+1,s+1);
      hAsymPS[p].push_back(TH1D(hName[p],"Plane strip",25,-0.001,0.001));
      sprintf(hNameEr[p],"asymErPlane%dStr%d",p+1,s+1);
      hAsymErPS[p].push_back(TH1D(hNameEr[p],"Single Strip asym stat.error",25,-0.001,0.001));

      hAsymPS[p][s].SetBit(TH1::kCanRebin);
      hAsymPS[p][s].SetTitle(Form("Plane %d Strip %d Asymmetry",p+1,s+1));
      hAsymPS[p][s].GetXaxis()->SetTitle("Asymmetry");
      hAsymPS[p][s].GetYaxis()->SetTitle("Counts");

      hAsymErPS[p][s].SetBit(TH1::kCanRebin);
      hAsymErPS[p][s].SetTitle(Form("Plane %d Strip %d Asymmetry Error",p+1,s+1));
      hAsymErPS[p][s].GetXaxis()->SetTitle("Asym Error");
      hAsymErPS[p][s].GetYaxis()->SetTitle("Counts");
    }
  }

  TPaveText *pvTxt1 = new  TPaveText(0.75,0.84,0.98,1.0,"NDC");
  pvTxt1->SetBorderSize(1);
  
  /** Open either Pass1 or the First 100K **/
  for (Int_t runnum = runnum1; runnum <= runnum2; runnum++) {
    if( isFirst100k) {
      chainExists = mpsChain->Add(Form("$QW_ROOTFILES/first100k_%d.root",runnum));
    }
    else {
      //chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_Pass1_%d.*.root",runnum));//for Run2
      chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_%d.*.root",runnum));//for my Analyzer output files
      printf("Attached %d files to chain for Run # %d\n",chainExists,runnum);
    }
    if(!chainExists){//delete chains and exit if files do not exist
      cout<<"\n***Error: File for run "<<runnum<<" does not exist***\n"<<endl;
      delete mpsChain;
      return -1;
    }
    nLasCycBeamTrips = getEBeamLasCuts(cutLas, cutEB, mpsChain,runnum);
    if (debug) printf("nLasCycBeamTrips: %d\n",nLasCycBeamTrips);
///!trying to get the cut positions by reading a file rather than executing the function repeatedly
//..while re-running the macro again

//     if (infileLas.is_open()) {
//       while (infileLas.good() )	{
// 	infileLas >> entry;
// 	cutLas.push_back(entry);
//       }
//       infileLas.close();
//     } 
//     else cout << "\n*****Error: Unable to cutLas file*****\n"; 
//     nLasCycles = cutLas.size();

//     if (infileBeam.is_open()) {
//       while (infileBeam.good() )	{
// 	infileBeam>>entry;
// 	cutEB.push_back(entry);
//       }
//       infileBeam.close();
//     } 
//     else cout << "\n*****Error: Unable to cutEB file*****\n"; 
//     nBeamTrips = cutEB.size();

    if (debug) printf("cutEB.size:%d,cutLas.size:%d\n",cutEB.size(),cutLas.size());
  }
  Int_t nEntries = mpsChain->GetEntries();
  printf("This chain has %i entries.\n", nEntries);

  mpsChain->ResetBranchAddresses();//!? should it be here?
  nLasCycles = nLasCycBeamTrips%1000 - 1;
  ////first two digits of return value of getEBeamLasCuts
  nBeamTrips = nLasCycBeamTrips / 1000;
  ////fourth digit of return value of getEBeamLasCuts

  Qmax = mpsChain->GetMaximum("sca_bcm6/value");//!I should use sca_bcm6
  cout<<"Qmax = "<<Qmax <<endl;
  lasMax = mpsChain->GetMaximum("sca_laser_PowT/value");
  cout<<"lasMax="<<lasMax<<endl;
  cout<<"nbeamTrips="<<nBeamTrips<<endl;
  cout<<"nLasCycles="<<nLasCycles<<endl;

  mpsChain->SetBranchStatus("*",0);  ////Turn off all unused branches, for efficient looping
  mpsChain->SetBranchStatus("actual_helicity",1);
  mpsChain->SetBranchStatus("sca_laser_PowT",1);
  mpsChain->SetBranchStatus("sca_bcm6",1);
  mpsChain->SetBranchStatus("p*Ac",1);

  mpsChain->SetBranchAddress("event_number",&event_number);
  mpsChain->SetBranchAddress("pattern_number",&pattern_number);
  mpsChain->SetBranchAddress("actual_helicity",&helicity);
  mpsChain->SetBranchAddress("sca_laser_PowT",&lasPow);
  mpsChain->SetBranchAddress("sca_bcm6",&bcm);
  
  for(Int_t p = 0; p <nPlanes; p++) {      
    mpsChain->SetBranchAddress(Form("p%dRawAc",p+1),&bRawAccum[p]);
  }//the branch for each plane is named from 1 to 4

  for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
    if (debug) cout<<"\nStarting nCycle:"<<nCycle<<" and resetting all nCycle variables"<<endl;
    ////since this is the beginning of a new Laser cycle, and I have already assigned the 
    ////accumulated counts to a permanent variable reset the LasCyc counters
    nMpsB1H1L1= 0, nMpsB1H0L1= 0, nMpsB1H1L0= 0, nMpsB1H0L0= 0;
    comptQH1L1= 0.0, comptQH0L1= 0.0, comptQH1L0= 0.0, comptQH0L0= 0.0;
    lasPowB1H1= 0.0, lasPowB1H0= 0.0;
    for(Int_t p = 0; p <nPlanes; p++) {      
      for(Int_t s = 0; s <nStrips; s++) {
	AccumB1H0L0[p][s] =0.0, AccumB1H0L1[p][s] =0.0, AccumB1H1L0[p][s] =0.0, AccumB1H1L1[p][s] =0.0;
 	unNormLasCycSum[p][s]= 0.0;
	normAcB1H1L1LasCyc[p][s]= 0.0, normAcB1H0L1LasCyc[p][s]= 0.0;
	normAcB1H0L0LasCyc[p][s]= 0.0, normAcB1H1L0LasCyc[p][s]= 0.0;
	BCnormAcB1H1L1LasCyc[p][s]= 0.0, BCnormAcB1H0L1LasCyc[p][s]= 0.0; 
	BCnormLasCycSum[p][s]= 0.0, BCnormLasCycDiff[p][s]= 0.0;
	qNormLasCycAsym[p][s]= 0.0, LasCycAsymEr[p][s]= 0.0;
	stripAsym[p][s]= 0.0,stripAsymEr[p][s]= 0.0,stripAsymRMS[p][s]= 0.0;
      }
    }

    if(nthBeamTrip < nBeamTrips) {
      if(cutLas.at(2*nCycle+3)<cutEB.at(2*nthBeamTrip)) {
	//will work for the beginning when there has been no beamtrip yet
	//!?what if the run starts with a beamTrip
	//!?what if the run ends with a beamTrip
	//!?what if there is no beamTrip
	beamOn = kTRUE;
      }
      else {
	beamOn = kFALSE;
	nthBeamTrip++;
      }
    }
    else {//processing the last beamTrip
      if(nthBeamTrip != nBeamTrips) cout<<"\n***Error in BeamTrip evaluation***\n"<<endl;
      if (cutLas.at(2*nCycle+1)>cutEB.at(2*nthBeamTrip-1)) { //this happens when run ends with beamOn
 	beamOn = kTRUE;	
      }
      else {
 	beamOn = kFALSE;
      }
    }
      
    for(Int_t i =cutLas.at(2*nCycle+1); i <cutLas.at(2*nCycle+3); i++) { 
      //loop over laser cycle periods from one LasOn state upto just before beginning of next LasOn
      if(debug && i%100000==0) cout<<"Starting to analyze "<<i<<"th event"<<endl;
      if ((i >= cutLas.at(2*nCycle+1)) && (i < cutLas.at(2*nCycle+2))) l=1;
      if (i >= cutLas.at(2*nCycle+2)) l=0;

      mpsChain->GetEntry(i);
      lasOn = lasPow[0] > laserFrac *lasMax;//!this part should be directly done in getEBeamLasCuts.C
      h = (Int_t)helicity;
      if ((l==1) && lasOn) l=1; //making the laser On requirement more strict

      /**********
       * (a)currently this method appears a little inefficient because since we not processing beamOff data
       * why do we even enter this laser-cycle which is not going to do anything;
       * but later on, we are going to use use the beam off part in someway and this would make sense then
       **********/
      
      if (beamOn) { ////currently the counters are only populated for beamOn cycles
	// 	if (h ==1 || h ==0) {  ////to avoid the h=-9999 that appears in beginning of every runlet
	if (h ==0 && l ==0) {
	  nMpsB1H0L0++;
	  comptQH0L0 += bcm[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H0L0[p][s] += bRawAccum[p][s];// / bcm[0]; // /lasPow[0];
	    }
	  }
	}
	else if (h ==0 && l==1) {////the elseif statement helps avoid overhead in each entry
	  nMpsB1H0L1++;
	  comptQH0L1 += bcm[0];
	  lasPowB1H0 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H0L1[p][s] += bRawAccum[p][s];// / bcm[0]; // /lasPow[0];	    
	    }	  
	  }
	}
	else if (h ==1 && l==0) {
	  nMpsB1H1L0++;
	  comptQH1L0 += bcm[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H1L0[p][s] += bRawAccum[p][s];// / bcm[0];// /lasPow[0];	    
	    }
	  }
	}
	else if (h ==1 && l==1) {
	  nMpsB1H1L1++;
	  comptQH1L1 += bcm[0];
	  lasPowB1H1 += lasPow[0];
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      if (maskedStrips(p,s)) continue;
	      //if(bRawAccum[p][s]) 
	      AccumB1H1L1[p][s] += bRawAccum[p][s];// / bcm[0];// /lasPow[0];	    
	    }
	  }
	}
      }
    }///for(Int_t i =cutLas.at(2*nCycle+1); i <cutLas.at(2*nCycle+3); i++)
    
    //after having filled the above vectors based on laser and beam periods, its time to calculate
    if (beamOn) {
      goodCycle++;
      if (debug1) cout<<"the Laser Cycle: "<<nCycle<<" has 'beamOn': "<<beamOn<<endl;
      if (nMpsB1H0L1<= 0 || nMpsB1H1L1<= 0 || nMpsB1H0L0<= 0 || nMpsB1H1L0<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check nMpsB1H0L1:%d,nMpsB1H1L1:%d, nMpsB1H0L0:%d, nMpsB1H1L0:%d**\n",
	       nCycle,nMpsB1H0L1,nMpsB1H1L1,nMpsB1H0L0,nMpsB1H1L0);
      else if (comptQH0L1<= 0 || comptQH1L1<= 0 || comptQH0L0<= 0 || comptQH1L0<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check comptQH0L1:%f,comptQH1L1:%f, comptQH0L0:%f, comptQH1L0:%f**\n",
	       nCycle,comptQH0L1,comptQH1L1,comptQH0L0,comptQH1L0);
      else {
	for (Int_t p =startPlane; p <endPlane; p++) {	  	  
	  for (Int_t s =startStrip; s <endStrip; s++) {	  
	    if (maskedStrips(p,s)) continue;
	    normAcB1H1L1LasCyc[p][s] = AccumB1H1L1[p][s] /comptQH1L1;
	    normAcB1H1L0LasCyc[p][s] = AccumB1H1L0[p][s] /comptQH1L0;;// 
	    normAcB1H0L1LasCyc[p][s] = AccumB1H0L1[p][s] /comptQH0L1;
	    normAcB1H0L0LasCyc[p][s] = AccumB1H0L0[p][s] /comptQH0L0;

	    if(debug2) {
	      printf(" normAcB1H1L1LasCyc[%d][%d]:\t%f = AccumB1H1L1:%f / comptQH1L1:%f\n"
			     ,p,s,normAcB1H1L1LasCyc[p][s],AccumB1H1L1[p][s], comptQH1L1 );
	      printf(" normAcB1H1L0LasCyc[%d][%d]:\t%f = AccumB1H1L0:%f / comptQH1L0:%f\n"
			     ,p,s,normAcB1H1L0LasCyc[p][s],AccumB1H1L0[p][s], comptQH1L0 );
	      printf(" normAcB1H0L1LasCyc[%d][%d]:\t%f = AccumB1H0L1:%f / comptQH0L1:%f\n"
			     ,p,s,normAcB1H0L1LasCyc[p][s],AccumB1H0L1[p][s], comptQH0L1 );
	      printf(" normAcB1H0L0LasCyc[%d][%d]:\t%f = AccumB1H0L0:%f / comptQH0L0:%f\n"
			     ,p,s,normAcB1H0L0LasCyc[p][s],AccumB1H0L0[p][s], comptQH0L0 );
	    }
	    unNormLasCycSum[p][s] = AccumB1H1L1[p][s] + AccumB1H0L1[p][s];//total counts during Laser ON

	    BCnormAcB1H1L1LasCyc[p][s] = normAcB1H1L1LasCyc[p][s] - normAcB1H1L0LasCyc[p][s]; //* comptQH1L1/comptQH1L0;
	    BCnormAcB1H0L1LasCyc[p][s] = normAcB1H0L1LasCyc[p][s] - normAcB1H0L0LasCyc[p][s]; //* comptQH0L1/comptQH0L0;

	    BCnormLasCycDiff[p][s] = (BCnormAcB1H1L1LasCyc[p][s] - BCnormAcB1H0L1LasCyc[p][s]);
	    BCnormLasCycSum[p][s] = (BCnormAcB1H1L1LasCyc[p][s]  + BCnormAcB1H0L1LasCyc[p][s]);
	    if (BCnormLasCycSum[p][s]  <= 0.0) {
	      if(debug) printf("\n**Warning**:BCnormLasCycSum[p%d][s%d] is %f in nCycle:%d\n",p,s,BCnormLasCycSum[p][s],nCycle);
	    }
	    else qNormLasCycAsym[p][s] = (BCnormLasCycDiff[p][s] / BCnormLasCycSum[p][s]);

	    if (unNormLasCycSum[p][s]<=0) {
	      if(debug) printf("***Warning: unNormLasCycSum[p%d][s%d]:%f\n",p,s,unNormLasCycSum[p][s]);
	      if(debug) printf("***Warning: qNormLasCycAsym[p%d][s%d]:%f\n",p,s,qNormLasCycAsym[p][s]);
	    }
	    else LasCycAsymEr[p][s]  = 1.0/sqrt(unNormLasCycSum[p][s]);

	    if (debug1) {
	      printf("for nCycle:%d, qNormLasCycAsym[p%d][s%d]= %f (stat.err:%f)\n",nCycle,p,s,qNormLasCycAsym[p][s] ,LasCycAsymEr[p][s]);
	      printf("formed by normalized BC (%f -/+ %f) \n",normAcB1H1L1LasCyc[p][s],normAcB1H0L1LasCyc[p][s]);
	    }
	    hAsymPS[p][s].Fill(qNormLasCycAsym[p][s]);
	    hAsymErPS[p][s].Fill(LasCycAsymEr[p][s]);	    
	  }
	}//for (Int_t p =startPlane; p <endPlane; p++) {
      }
    }///if (beamOn)
    else cout<<"this LasCyc(nCycle:"<<nCycle<<") had a beam trip, hence skipping"<<endl;
  }//for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
  
  TCanvas *cmystrAsym = new TCanvas("cmystrAsym","Asymmetry in strip",10,10,1500,1100);
  cmystrAsym->Divide(2,4);
  Int_t n = 0;
  for(Int_t s =mystr; s >(mystr-4); s--) {
    cmystrAsym->cd(1+2*n);
    h1[s] = (TH1D*)hAsymPS[0][s].Clone();    
    h1[s]->Draw();
    //h1[s]->Fit("gaus");//the fit function probably automatically draws it too

    cmystrAsym->cd(2+2*n);
    h2[s] = (TH1D*)hAsymErPS[0][s].Clone(); //,"","goff");
    h2[s]->Draw();
    cmystrAsym->Update();
    n++;
  }
  cmystrAsym->SaveAs(Form("AsymStrip%d.png",mystr));
  
  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip; s++) {        
      if (maskedStrips(p,s)) continue;
      stripAsym[p][s] = hAsymPS[p][s].GetMean();
      stripAsymRMS[p][s] = hAsymPS[p][s].GetRMS();      
      stripAsymEr[p][s] = ( hAsymErPS[p][s].GetMean() ) / (TMath::Sqrt((Double_t)goodCycle));/// 1/sqrt(N)
    }
  }

  //sprintf(textwrite,"strip\texpAsym\tasymEr\tasymRMS");
  //outfileExpAsymP1 <<textwrite<<endl;
  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip;s++) {    
      if (maskedStrips(p,s)) continue;
      if (p==0) { //plane 1
	sprintf(textwrite,"%d\t%f\t%f\t%f",s+1,stripAsym[p][s],stripAsymEr[p][s],stripAsymRMS[p][s]);
	outfileExpAsymP1 <<textwrite<<endl;
      }
    }
  }
  
  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to evaluate edetExpAsym.\n",div_output.quot,div_output.rem );  
  return 1;
}

/******************************************************
!Querries:
* how to properly delete the new TGraphErrors and the new TCanvas and the new TLine 
* ..created in the code
* why is my 'goff' not working in the histogram Draw method?
******************************************************/

/******************************************************
!Further modifications:
* I'm not sure if the precursor file 'getEBeamLasCuts.C' can handle multile run numbers
*..though this file seems to be capable of handle multiple runnumbers, it is practically not
*.. because of the limitation posed by the getEBeamLasCuts.C file. For now, we are limited to 1 runnumber.
* due charge normalization per MPS window
* ensure efficient evaluation of beamtrips
* check consistency of cut on laserPow and beamtrip
******************************************************/
