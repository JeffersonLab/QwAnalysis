////Don Jones and Juan Carlos helped significantly in the development of this code
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
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0); //displays the fit parameters on the respective plots
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
  Bool_t lasOn, beamOn =kFALSE;
  Int_t chainExists = 0;
  Int_t h = 0, l = 0;//helicity, lasOn tracking variables
  Int_t nthBeamTrip = 0, nBeamTrips = 0;//beamTrip tracking variables
  Int_t nLasCycles=0;//total no.of LasCycles, index of the already declared cutLas vector
  Int_t AccumB1H0L0[nPlanes][nStrips];
  Int_t AccumB1H0L1[nPlanes][nStrips];
  Int_t AccumB1H1L0[nPlanes][nStrips];
  Int_t AccumB1H1L1[nPlanes][nStrips];

  Int_t nMpsB1H1L1, nMpsB1H0L1, nMpsB1H1L0, nMpsB1H0L0;
  Int_t unNormLasCycSum[nPlanes][nStrips];
  Double_t lasPow[3], lasMax, helicity, comptQ[3], Qmax;//, beamMax, bcm
  Double_t pattern_number, event_number;
  Double_t bRawAccum[nPlanes][nStrips];
  Double_t stripAsym[nPlanes][nStrips],stripAsymEr[nPlanes][nStrips],stripAsymRMS[nPlanes][nStrips] ;
  Double_t stripPlot[nStrips];//..!for plotting may not be required later
  Double_t normL1H1LasCyc[nPlanes][nStrips], normL1H0LasCyc[nPlanes][nStrips];
  Double_t normL0H0LasCyc[nPlanes][nStrips], normL0H1LasCyc[nPlanes][nStrips];
  Double_t BCnormL1H1LasCyc[nPlanes][nStrips], BCnormL1H0LasCyc[nPlanes][nStrips]; 
  Double_t BCnormLasCycSum[nPlanes][nStrips], BCnormLasCycDiff[nPlanes][nStrips];
  Double_t tNormLasCycAsym[nPlanes][nStrips], LasCycAsymEr[nPlanes][nStrips];
  Double_t zero[nStrips];//because TGraphErrors needs error on x-axis too

  char hName[nPlanes][120],hNameEr[nPlanes][120];

  std::vector<std::vector<TH1D> > hAsymPS, hAsymErPS;

  TGraphErrors *grAsymPlane[nPlanes];

  for (Int_t p =0; p <nPlanes; p++) {   
    if (p >= (Int_t) hAsymPS.size()) {
      hAsymPS.resize(p+1);
      hAsymErPS.resize(p+1);
      if(debug) cout<<"hAsymPS.size():"<<hAsymPS.size()<<endl;
    }
    for (Int_t s=0; s<nStrips; s++) { 
      sprintf(hName[p],"asymPlane%d_Str%d",p,s);
      hAsymPS[p].push_back(TH1D(hName[p],"Plane strip",25,-0.001,0.001));
      sprintf(hNameEr[p],"asymErPlane%dStr%d",p,s);
      hAsymErPS[p].push_back(TH1D(hNameEr[p],"Single Strip asym stat.error",25,-0.001,0.001));

      hAsymPS[p][s].SetBit(TH1::kCanRebin);
      hAsymPS[p][s].SetTitle(Form("Plane %d Strip %d Asymmetry",p,s));
      hAsymPS[p][s].GetXaxis()->SetTitle("Asymmetry");
      hAsymPS[p][s].GetYaxis()->SetTitle("Counts");

      hAsymErPS[p][s].SetBit(TH1::kCanRebin);
      hAsymErPS[p][s].SetTitle(Form("Plane %d Strip %d Asymmetry Error",p,s));
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
//       chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_Pass1_%d.*.root",runnum));//for Run2
      chainExists = mpsChain->Add(Form("$QW_ROOTFILES/Compton_%d.*.root",runnum));//for my Analyzer output files
      printf("Attached %d files to chain for Run # %d\n",chainExists,runnum);
    }
    if(!chainExists){//delete chains and exit if files do not exist
      cout<<"\n***Error: File for run "<<runnum<<" does not exist***\n"<<endl;
      delete mpsChain;
      return -1;
    }
  }

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
  
  for(Int_t p = 0; p <nPlanes; p++) {      
    mpsChain->SetBranchAddress(Form("p%dRawAc",p+1),&bRawAccum[p]);
  }//the branch for each plane is named from 1 to 4

  for(Int_t nCycle=0; nCycle<nLasCycles; nCycle++) { 
    if (debug) cout<<"\nStarting nCycle:"<<nCycle<<" and resetting all nCycle variables"<<endl;
    ////since this is the beginning of a new Laser cycle, and I have already assigned the 
    ////accumulated counts to a permanent variable reset the LasCyc counters
    nMpsB1H1L1= 0, nMpsB1H0L1= 0, nMpsB1H1L0= 0, nMpsB1H0L0= 0;

    for(Int_t p = 0; p <nPlanes; p++) {      
      for(Int_t s = 0; s <nStrips; s++) {
	AccumB1H0L0[p][s] = 0, AccumB1H0L1[p][s] = 0, AccumB1H1L0[p][s] = 0, AccumB1H1L1[p][s] = 0;
 	unNormLasCycSum[p][s]= 0;
	normL1H1LasCyc[p][s]= 0.0, normL1H0LasCyc[p][s]= 0.0;
	normL0H0LasCyc[p][s]= 0.0, normL0H1LasCyc[p][s]= 0.0;
	BCnormL1H1LasCyc[p][s]= 0.0, BCnormL1H0LasCyc[p][s]= 0.0; 
	BCnormLasCycSum[p][s]= 0.0, BCnormLasCycDiff[p][s]= 0.0;
	tNormLasCycAsym[p][s]= 0.0, LasCycAsymEr[p][s]= 0.0;
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
       * currently this method appears a little inefficient because since we not processing beamOff data
       * why do we even enter this laser-cycle which is not going to do anything;
       * but later on, we are going to use use the beam off part in someway and this would make sense then
       **********/
      if (beamOn) { ////currently the counters are only populated for beamOn cycles
	// 	if (h ==1 || h ==0) {  ////to avoid the h=-9999 that appears in beginning of every runlet
	if (h ==0 && l ==0) {
	  nMpsB1H0L0++;
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      AccumB1H0L0[p][s] += (Int_t)bRawAccum[p][s];
	    }
	  }
	}
	else if (h ==0 && l==1) {////the elseif statement helps avoid overhead in each entry
	  nMpsB1H0L1++;
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      AccumB1H0L1[p][s] += (Int_t)bRawAccum[p][s];
	    }	  
	  }
	}
	else if (h ==1 && l==0) {
	  nMpsB1H1L0++;
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      AccumB1H1L0[p][s] += (Int_t)bRawAccum[p][s];
	    }
	  }
	}
	else if (h ==1 && l==1) {
	  nMpsB1H1L1++;
	  for(Int_t p = startPlane; p <endPlane; p++) {      	
	    for(Int_t s =startStrip; s <endStrip; s++) {
	      AccumB1H1L1[p][s] += (Int_t)bRawAccum[p][s];
	    }
	  }
	}
      }
    }///for(Int_t i =cutLas.at(2*nCycle+1); i <cutLas.at(2*nCycle+3); i++)
    
    //after having filled the above vectors based on laser and beam periods, its time to calculate
    if (debug1) printf("\n  *Entering laser Cycle Analysis @ nCycle : %d* \n",nCycle);
    if (beamOn) {
      if (debug3) cout<<"the nCycle: "<<nCycle<<" has 'beamOn': "<<beamOn<<endl;
      if (nMpsB1H0L1<= 0 || nMpsB1H1L1<= 0 || nMpsB1H0L0<= 0 || nMpsB1H1L0<= 0)
	printf("\n****  Warning: Something drastically wrong in nCycle:%d\n\t\t** check nMpsB1H0L1:%d,nMpsB1H1L1:%d, nMpsB1H0L0:%d, nMpsB1H1L0:%d**\n",
	       nCycle,nMpsB1H0L1,nMpsB1H1L1,nMpsB1H0L0,nMpsB1H1L0);
      else {
	for (Int_t p =startPlane; p <endPlane; p++) {	  	  
	  for (Int_t s =startStrip; s <endStrip; s++) {	  
	    normL1H1LasCyc[p][s] = (Double_t)AccumB1H1L1[p][s]/nMpsB1H1L1;
	    if(debug2)printf(" normL1H1LasCyc:\t%f = lasOnH1:%d / nMpsB1H1L1:%d\n"
			     ,normL1H1LasCyc[p][s],AccumB1H1L1[p][s] , nMpsB1H1L1 );

	    normL0H1LasCyc[p][s] = (Double_t)AccumB1H1L0[p][s]/nMpsB1H1L0;
	    if(debug2)printf(" normL1H1LasCyc:\t%f = lasOnH1:%d / nMpsB1H1L1:%d\n"
			     ,normL0H1LasCyc[p][s],AccumB1H1L0[p][s] , nMpsB1H1L0 );

	    normL1H0LasCyc[p][s] = (Double_t)AccumB1H0L1[p][s]/nMpsB1H0L1;
	    if(debug2)printf(" normL1H1LasCyc:\t%f = lasOnH1:%d / nMpsB1H1L1:%d\n"
			     ,normL1H0LasCyc[p][s],AccumB1H0L1[p][s] , nMpsB1H0L1 );

	    normL0H0LasCyc[p][s] = (Double_t)AccumB1H0L0[p][s]/nMpsB1H0L0;
	    if(debug2)printf(" normL1H1LasCyc:\t%f = lasOnH1:%d / nMpsB1H1L1:%d\n"
			     ,normL0H0LasCyc[p][s],AccumB1H0L0[p][s] , nMpsB1H0L0 );
	    
	    unNormLasCycSum[p][s] = AccumB1H1L1[p][s] + AccumB1H0L1[p][s];

	    BCnormL1H1LasCyc[p][s] = (normL1H1LasCyc[p][s] - normL0H1LasCyc[p][s]);
	    BCnormL1H0LasCyc[p][s] = (normL1H0LasCyc[p][s] - normL0H0LasCyc[p][s]);

	    BCnormLasCycDiff[p][s] = (BCnormL1H1LasCyc[p][s] - BCnormL1H0LasCyc[p][s]);
	    BCnormLasCycSum[p][s] = (BCnormL1H1LasCyc[p][s]  + BCnormL1H0LasCyc[p][s]);
	    if (BCnormLasCycSum[p][s]  <= 0.0) {
	      if(debug) printf("\n**Warning**:BCnormLasCycSum[p%d][s%d] is %f in nCycle:%d\n",p,s,BCnormLasCycSum[p][s],nCycle);
	    }
	    else tNormLasCycAsym[p][s] = (BCnormLasCycDiff[p][s] / BCnormLasCycSum[p][s]);

	    if (unNormLasCycSum[p][s]<=0) {
	      if(debug) printf("***Warning: unNormLasCycSum[p%d][s%d]:%d\n",p,s,unNormLasCycSum[p][s]);
	      if(debug) printf("***Warning: tNormLasCycAsym[p%d][s%d]:%f\n",p,s,tNormLasCycAsym[p][s]);
	    }
	    else LasCycAsymEr[p][s]  = 1.0/sqrt(unNormLasCycSum[p][s]);

	    if (debug3) {
	      printf("for nCycle:%d, tNormLasCycAsym[p%d][s%d]= %f (stat.err:%f)\n",nCycle,p,s,tNormLasCycAsym[p][s] ,LasCycAsymEr[p][s]);
	      printf("formed by normalized BC (%f -/+ %f) \n",normL1H1LasCyc[p][s],normL1H0LasCyc[p][s]);
	    }
	    hAsymPS[p][s].Fill(tNormLasCycAsym[p][s]);
	    hAsymErPS[p][s].Fill(LasCycAsymEr[p][s]);	    
	  }
	}//for (Int_t p =startPlane; p <endPlane; p++) {
      }
    }///if (beamOn)
    else cout<<"this LasCyc(nCycle:"<<nCycle<<") had a beam trip, hence skipping"<<endl;
  }
  
  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip; s++) {        
      hAsymPS[p][s].Draw("H");//"H","","goff");//WHY IS My goff not working!
      hAsymPS[p][s].Fit("gaus");
      stripAsym[p][s] = hAsymPS[p][s].GetMean();
      stripAsymRMS[p][s] = hAsymPS[p][s].GetRMS();
      
      hAsymErPS[p][s].Draw("H");//,"","goff");
      stripAsymEr[p][s] = hAsymErPS[p][s].GetMean();
    }
  }
  if(debug2) {
  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    for (Int_t s =startStrip; s <endStrip;s++) {    
      printf("asym[%d][%d]:%f; asymEr[%d][%d]:%f; asymRMS[%d][%d]:%f\n",p,s,stripAsym[p][s],p,s,stripAsymEr[p][s],p,s,stripAsymRMS[p][s]);
    }
  }
  }
  for (Int_t s =0; s <nStrips;s++) {
    stripPlot[s]=s+1;
    zero[s] =0;
  }
  TCanvas *cAsym = new TCanvas("cAsym","Asymmetry Vs Strip number",10,10,800,800);
  cAsym->Divide(2,2);

  TLine *myline = new TLine(0,0,60,0);
  myline->SetLineStyle(1);

  for (Int_t p =startPlane; p <endPlane; p++) {	  	  
    cAsym->cd(p+1);
    grAsymPlane[p] = new TGraphErrors(nStrips,stripPlot,stripAsym[p],zero,stripAsymRMS[p]);
    grAsymPlane[p]->GetXaxis()->SetTitle("strip number");
    grAsymPlane[p]->GetYaxis()->SetTitle("asymmetry");
    grAsymPlane[p]->SetTitle(Form("Plane %d",p+1));
    grAsymPlane[p]->Draw("A*");
    myline->Draw();
    cAsym->Update();
  }

  tEnd = time(0);
  div_output = div((Int_t)difftime(tEnd, tStart),60);
  printf("\n it took %d minutes %d seconds to complete.\n",div_output.quot,div_output.rem );
  return 1;
}

/******************************************************
Querries:
******************************************************/

/******************************************************
Further modifications:
* the code should skip masked strips
* ensure efficient evaluation of beamtrips
* check consistency of cut on laserPow and beamtrip
******************************************************/
