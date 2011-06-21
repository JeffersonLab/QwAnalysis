#include "cutOnLaser.C"
#include "getRightmost.C"
#include "getLeftmost.C"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TMath.h"
#include <vector>
#include "TROOT.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TCint.h"
#include "TSystem.h"
#include <iostream>
#include "TChain.h"
#include "TGaxis.h"
#include "TProfile.h"
#include <stdio.h>
#include <stdlib.h>

using namespace std;



/////////////////////////////////////////////////////////////////////////////
//This program performs a rudimentary Compton scaler analysis with summary //
//plots. It uses three different methods of background subtraction.        //
//using the scalers. It uses the background scintillator rates times a     //
//1. Average laserwise PD subtraction: uses average PD rates for two       //
//   surrounding laser off periods.                                        //
//2. Average laserwise scintillator scale factor subtraction: uses average //
//   ratio of PD to scintillator rate for two surrounding laser off periods//
//   to calculate a scale factor to subtract backgrounds using background  //
//   scintillator rates.                                                   //
//3. Runwise lookup table of laser off PD-scintillator correlation: uses a //
//TProfile of PD rates vs scintillator rates to form a lookup table to use //
//for determining PD backgrounds from scintillator rates.                  //
//The input is run number.                                                 //
/////////////////////////////////////////////////////////////////////////////

Int_t qwikComptonScaler(Int_t runnum)
{
  gROOT->Reset();
  Bool_t debug = kTRUE;

  const Double_t MINSCINT = 0;//minimum background rate in Hz shown on profile plot
  const Double_t MAXSCINT = 25;//maximum background rate in Hz shown on profile plot
  const Double_t ARIGHT = 1;//left limit of asymmetry plot
  const Double_t ALEFT = (-1) * ARIGHT;//right limit of asymmetry plot

  const Int_t MIN_ENTRIES = 2000;//min # of "burst" entries required to use a laserwise cycle
  const Int_t NCUTS = 500;//initialized size of TCut arrays.
  const Int_t PATHLENGTH = 255;
  const Int_t NBINS = 4 * (Int_t)MAXSCINT;//number of bins in lookup table profile plot 
  const Int_t N_BT_SKIP = 5000;// # of entries to skip after beam trip to allow stabilization

  //Lookup table shows PD/scintillator correlation. Of this data it is useful to be able
  //to select a region of interest such as a range of linear behaviour.
  const Double_t MINRoI = 10;//minimum scintillator value in "Region of Interest" 
  const Double_t MAXRoI = 19;//maximum scintillator value in "Region of Interest" 

  const Double_t SCALER_CUTOFF = 3;//if scaler below this likely HV tripped
  const Double_t MINLASER = 4;//maximum laser power to be considered OFF (flipper up)
  const Double_t RANGE = MAXSCINT - MINSCINT;
  const Double_t MAXLuTERR = 1;//maximum lookup table error for a value to be used

  char canvas1[PATHLENGTH], canvas2[PATHLENGTH], filename[PATHLENGTH];

  Bool_t local = kFALSE, flipperIsUp = kFALSE;
  Bool_t laserOn, beamOn, laserOff, enoughData, inOnCycle, inRange;
  Bool_t beamOffLatch = kFALSE, doneSkipAfterBTrip;

  Int_t lasWiseCuts[NCUTS];//entry # of first and last of all laser cycles
  Int_t nOffEvtsInRow = 0, nCycles = -1, nEntries, cntr = 0;
  Int_t nGoodCycles = 0, entry=0, nBins, setPnt = 0, idx, chainExists=0;
 
  Double_t lasPow, bcm6, photon, dsScint, asymm, weight, bkgdLU, bkgdAvg, time; 
  Double_t lasHCutVal, lasLCutVal, currentCutVal, hBinX, lBinX, lastIdx = 0;
  Double_t lasWisePhoton[NCUTS], lasWisePhotonErr[NCUTS];
  Double_t lasWisePhotonAvg[NCUTS], lasWisePhotonAvgErr[NCUTS];//average of adjacent bkgd's
  Double_t lasWiseScint[NCUTS], lasWiseScintErr[NCUTS];
  Double_t lasWiseScintAvg[NCUTS], lasWiseScintAvgErr[NCUTS];//average of adjacent scints
  Double_t asymmPD[NCUTS];//laswise PD avg calculated bkgd subtraction
  Double_t asymmSc[NCUTS];//laswise scintillator calculated bkgd subtraction
  Double_t asymmLuT[NCUTS];//lookup table background subtraction using PD/C13 correlation
  Double_t errorXPD[NCUTS], errorXSc[NCUTS], errorXLuT[NCUTS];
  Double_t errorYPD[NCUTS], errorYSc[NCUTS], errorYLuT[NCUTS];
  Double_t cutNum[NCUTS], lookUp[NCUTS],lookUpErr[NCUTS], count[NCUTS];

  TChain *helChain = new TChain("h9001");

  FILE *polstats;//file for storing stats.

  nBins = 200;

  //scaler asymmetry background subtracted by laserwise averaging PD rates 
  TH1D *hAsymmPDAvg = new TH1D("hAsymPDAvg","Scaler Asymmetry Using"
			       " PD Avg Laswise Rates", nBins, ALEFT, ARIGHT);
  //scaler asymmetry corrected using scintillator for background subtraction
  //by forming lookup table for correlation between PD and dsScint rates and
  //using dsScint as monitor for PD background subtraction.
  TH1D *hAsymmScintLuT = new TH1D("hAsymmScintLuT","Scaler Asymmetry Using Scint with"
			 " Look Up Table", nBins, ALEFT, ARIGHT);
  //scaler asymmetry corrected using scintillator for background subtraction
  //by finding laswise average scale factor PD/dsScint and multiplying by 
  //dsScint on an event by event basis.
  TH1D *hAsymmScintAvg = new TH1D("hAsymmScintAvg","Scaler Asymmetry Using Scint "
			 "Avg Laswise Rates ", nBins, ALEFT, ARIGHT);//subtract avg laswise rates
  TH1D *hAsymmLuT = new TH1D("hAsymmLuT","hAsymmLuT", nBins, ALEFT, ARIGHT);
  TH1D *hAsymmSc = new TH1D("hAsymmSC","hAsymmSc", nBins, ALEFT, ARIGHT);
  TH1D *hAsymmPD = new TH1D("hAsymmPD","hAsymmPD", nBins, ALEFT, ARIGHT);

  helChain->SetAlias("bcmYield","Ibcm17");

  // Open either Pass1 or the First 100K
  

  chainExists = helChain->Add(Form("%s/qwick_analysis/compton_%d.*.root",
				   getenv("QW_ROOTFILES"),runnum),0);

  if(!chainExists)//check local folder in case running locally
  {
    chainExists = helChain->Add(Form("Compton_Pass1_%d.*.root",runnum));
    local = kTRUE;
  }
  if(!chainExists){//delete chains and exit if files do not exist
    cout<<"File does not exist."<<endl;
    helChain->TChain::~TChain();
    return 0;
  }
  printf("%d files attached to chain.\n",chainExists);
  nEntries = helChain->GetEntries();
  printf("This chain has %i entries.\n", nEntries);
  if(local == kFALSE)
  {/*
    sprintf(canvas1,"./qwikComptonScaler_%d_plots_burst100.png",runnum);
    sprintf(canvas2,"./qwikComptonScaler_%d_LookupTable_burst100.png",runnum);
    sprintf(filename,"./qwikComptonScaler_%d_stats_burst100.txt",runnum);
    */
    sprintf(canvas1,"www/run_%d/qwikComptonScaler_%d_plots.png",runnum,runnum);
    sprintf(canvas2,"www/run_%d/qwikComptonScaler_%d_LookupTable.png",runnum,runnum);
    sprintf(filename,"www/run_%d/qwikComptonScaler_%d_stats.txt",runnum,runnum);
    
  }
  else
  {
    gSystem->mkdir(Form("photonsummarydon/Run_%d",runnum),true);
    sprintf(canvas1,"photonsummary/Run_%d_plots.png",runnum);
    sprintf(canvas2,"photonsummary/Run_%d_LookupTable.png",runnum);
    sprintf(filename,"photonsummary/Run_%d_stats.txt",runnum);
  }
  polstats = fopen(filename,"w");
  cout<<polstats<<endl;
  if(!polstats)//if directory doesn't exist create it
  {
    printf("Creating directory www/run_%d/\n",runnum);
    gSystem->mkdir(Form("www/run_%d/",runnum),true);
    polstats = fopen(filename,"w");
  }

  //make a correlation profile plot that can be used as a lookup table
  gStyle->SetPadLeftMargin(0.12);
  //  gStyle->SetStatW(0.15);
  //  gStyle->SetStatH(0.2);
  //  gStyle->SetStatX(0.99);
  //  gStyle->SetStatY(0.99);
  TCanvas *cProf = new TCanvas("cProf", "cProf", 0, 0, 1200, 500);
  cProf->Divide(2,1);
  cProf->cd(1);
  cProf->SetRightMargin(0.1);
  helChain->Draw(Form("Rphotgate:Dsscin>>hProf(%i, %f, %f)", NBINS, MINSCINT, MAXSCINT),
		 Form("Cavpow <%f",MINLASER), "prof goff");
  TProfile *hProf = (TProfile*)gDirectory->Get("hProf");
  hProf->GetXaxis()->SetTitle("Scintillator Background Scaler (Hz)");
  hProf->GetYaxis()->SetTitle("PD Scaler (Hz)");
  hProf->SetTitle("Correlation Plot: PD Scaler vs Background Scintillator Scaler");
  hProf->SetMarkerColor(kBlue);
  hProf->SetLineColor(kBlue);
  hProf->GetYaxis()->SetTitleOffset(1.6);
  hProf->Draw();  
  cProf->Modified();
  helChain->Draw(Form("Rphotgate:Dsscin>>hProf1(%i, %f, %f)", NBINS , MINSCINT, MAXSCINT),
		 Form("Cavpow<%f && Dsscin>=%f && Dsscin<%f",MINLASER, MINRoI,MAXRoI),
		 "prof goff");
  TProfile *hProf1 = (TProfile*)gDirectory->Get("hProf1");
  hProf1->SetMarkerColor(kRed);
  hProf1->SetLineColor(kRed);
  hProf1->Draw("same"); 

  TLegend *legend1 = new TLegend(0.88,0.63,0.98,0.75);
  //legend1->SetHeader("Some histograms");
  legend1->AddEntry(hProf,"Cut","l");
  legend1->AddEntry(hProf1,"Used","l");
  legend1->SetFillColor(0);
  legend1->Draw();

  cProf->cd(2);
  cProf->SetRightMargin(0.1);
  helChain->Draw(Form("Rphotgate:Dsscin>>hCor(%i,%f,%f, %i)",NBINS, MINSCINT, MAXSCINT, NBINS),
		 Form("Cavpow<%f",MINLASER), "goff");
  gPad->Update();
  TH2D *hCor = (TH2D*)gDirectory->Get("hCor");
  hCor->SetAxisRange(0,MAXSCINT+100, "X");
  hCor->SetAxisRange(0,1.4*hProf->GetMaximum(), "Y");
  hCor->GetXaxis()->SetTitle("Scint Background Scaler (Hz)");
  hCor->GetYaxis()->SetTitle("PD Scaler (Hz)");
  hCor->SetTitle("PD Scaler vs Background Scintillator Scaler");
  hCor->SetMarkerColor(kBlue);
  hCor->SetLineColor(kBlue);
  hCor->GetYaxis()->SetTitleOffset(1.6);
  hCor->Draw();

  //complete lookup table using values from hProf
  for(Int_t i=0; i<NBINS;i++)
  {
    count[i] = (Double_t)i;
    lookUp[i] = hProf->GetBinContent(i+1);//chosen so that dsScint==i
    lookUpErr[i] = hProf->GetBinError(i+1);
    if(lookUp[i] != 0)lastIdx = i;//keep track of last filled index in lookup table
  }

  //define temporary histograms  
  lBinX = helChain->GetMinimum("Dsscint");
  hBinX = helChain->GetMaximum("Dsscint");
  TH1D *hScint = new TH1D("hScint","Scint Background",nBins, lBinX, hBinX);
  lBinX = helChain->GetMinimum("Rphotgate");
  hBinX = helChain->GetMaximum("Rphotgate");
  TH1D *hPhoton = new TH1D("hPhoton","PD Background",nBins,lBinX, hBinX);

  for(Int_t i=0; i<=lastIdx; i++)
    printf("lookUp[%i] = %12.4e +/- %9.2e\n", (Int_t)count[i], lookUp[i],lookUpErr[i]);

  //define cut values 
  currentCutVal = 0.8 * helChain->GetMaximum("Ibcm17");
  helChain->Draw("Cavpow>>h(100,0)","Cavpow>100", "goff");
  TH1F *h = (TH1F*)gDirectory->Get("h");
  lasHCutVal = h->GetMean()-10;
  lasLCutVal = MINLASER;
  printf("Laser power cut is %f.\n", lasHCutVal);
  printf("Current cut is %f.\n", currentCutVal);

  //turn off all branches then turn on the ones needed.
  //  helChain->SetBranchStatus("*",0);
  helChain->SetBranchStatus("Ibcm17",1);
  helChain->SetBranchStatus("Cavpow",1);
  helChain->SetBranchStatus("Rphotgate",1);
  helChain->SetBranchStatus("Dsscin",1);
  helChain->SetBranchStatus("Time",1);
  //set branch addresses
  helChain->SetBranchAddress("Ibcm17", &bcm6);
  helChain->SetBranchAddress("Cavpow", &lasPow);
  helChain->SetBranchAddress("Rphotgate", &photon);
  helChain->SetBranchAddress("Dsscin", &dsScint);
  helChain->SetBranchAddress("Time", &time);



  ////////////////////////////////////////////////////////////
  //Loop over Hel_Tree and find laser cycles, backgrounds,  // 
  //and calibration factors for relating scintillator rates //
  //("Dsscin") to rates in the photon detector ("Rphotgate")//
  ////////////////////////////////////////////////////////////
  for(Int_t i=0;i<nEntries;i++)
  {
    helChain->GetEntry(i,0);
    laserOn = lasPow > lasHCutVal;
    laserOff = lasPow < lasLCutVal;
    beamOn = bcm6 > currentCutVal && photon > SCALER_CUTOFF;//e-beam and HV on
    doneSkipAfterBTrip = setPnt == 0;
    //cout<<time<<endl;
    //if(time>1060) i = nEntries;//this line for run 20932
    //allow last point to be stored if file ends in a laser OFF state.
    if(laserOff && nOffEvtsInRow>MIN_ENTRIES && i == nEntries-1)
    {
      flipperIsUp = kTRUE;
      laserOff = kFALSE;
    }

    if(nOffEvtsInRow==1)entry = i;//keep track of where OFF starts

    //start counting consecutive laserOff events if laser turns off.
    if(laserOff)
    {
      nOffEvtsInRow++;
      if(!beamOn)beamOffLatch = kTRUE;
 
      if(beamOn && beamOffLatch == kFALSE && doneSkipAfterBTrip)
      {
	hScint->Fill(dsScint);
	hPhoton->Fill(photon);
      }
      if(beamOn && beamOffLatch==kTRUE)
      {   
	
	setPnt = N_BT_SKIP;//skip entries to allow beam to restabilize after trip
	beamOffLatch = kFALSE;
      }
      if(setPnt > 0)setPnt --;
    }
    else 
    {
      setPnt = 0;
      if(flipperIsUp)//store OFF data at first sign of laser ON
      {
	lasWiseCuts[nCycles*2+1] = i; 
	if(hScint->GetEntries()>MIN_ENTRIES)//only store if sufficient data
        {
	  lasWiseScint[nCycles] = hScint->GetMean();
	  lasWiseScintErr[nCycles] = hScint->GetRMS()
	    /TMath::Sqrt((Double_t)hScint->GetEntries());
	  lasWisePhoton[nCycles] = hPhoton->GetMean();
	  lasWisePhotonErr[nCycles] = hPhoton->GetRMS()
	                           /TMath::Sqrt((Double_t)hPhoton->GetEntries());
	}

	else//otherwise flag as insufficient data
	{
	  lasWiseScint[nCycles] = -9999; 
	  lasWiseScintErr[nCycles] = -9999;
	  lasWisePhoton[nCycles] = -9999;
	  lasWisePhotonErr[nCycles] = -9999;
	}
	flipperIsUp = kFALSE;
      }
      hScint->Reset();
      hPhoton->Reset();
      nOffEvtsInRow = 0;
    }
    
    if(nOffEvtsInRow==MIN_ENTRIES)
    {
      nCycles++;
      lasWiseCuts[nCycles*2] = entry;
      flipperIsUp = kTRUE;
    }

  }

  //print values to screen for debugging
  if(debug)
  {
    printf("nCycles = %i.\n", nCycles);
    for(Int_t i=0; i<=nCycles; i++)
    {
      printf("lasWisePhoton[%i]= %10.2e +/- %7.1e\n",
	     i, lasWisePhoton[i], lasWisePhotonErr[i]);
    }
    printf("\n");
    for(Int_t i=0; i<nCycles; i++)
    {
      if(lasWisePhoton[i]>-9998 && lasWisePhoton[i+1]>-9998)
      {
	lasWisePhotonAvg[i] = 0.5 * (lasWisePhoton[i] + lasWisePhoton[i+1]);
	lasWisePhotonAvgErr[i] = 0.5 * (lasWisePhotonErr[i] + lasWisePhotonErr[i+1]);
      }
      else
      {	
	lasWisePhotonAvg[i] = -9999;
	lasWisePhotonAvgErr[i] = -9999;
      }
      printf("lasWisePhotonAvg[%i]= %10.2e +/- %7.1e\n",
	     i, lasWisePhotonAvg[i], lasWisePhotonAvgErr[i]);
    }
    printf("\n");

    for(Int_t i=0; i<=nCycles; i++)
    {
      printf("lasWiseScint[%i]= %10.2e +/- %7.1e\n",
	     i, lasWiseScint[i], lasWiseScintErr[i]);
    } 
    printf("\n");

    for(Int_t i=0; i<nCycles; i++)
    {
      if(lasWiseScint[i]>-9998 && lasWiseScint[i+1]>-9998)
      {
	lasWiseScintAvg[i] = 0.5 * (lasWiseScint[i] + lasWiseScint[i+1]);
	lasWiseScintAvgErr[i] = 0.5 * (lasWiseScintErr[i] + lasWiseScintErr[i+1]);
      }
      else
      {	
	lasWiseScintAvg[i] = -9999;
	lasWiseScintAvgErr[i] = -9999;
      }
      printf("lasWiseScintAvg[%i]= %10.2e +/- %7.1e\n",

	     i, lasWiseScintAvg[i], lasWiseScintAvgErr[i]);
    }
    for(Int_t i=0; i<nCycles*2+2; i++)
      printf("lasWiseCuts[%i] = %i\n",i, lasWiseCuts[i]);
  }


  /////////////////////////////////////////////////////////////////
  //Plot and record laser wise asymmetries using scintillator as //
  //background monitor for background subtraction.               //
  /////////////////////////////////////////////////////////////////


  //turn on asymmetry branch
  helChain->SetBranchStatus("Aphotgate", 1);//turn on branch
  helChain->SetBranchAddress("Cavpow", &lasPow);


  helChain->SetBranchAddress("Ibcm17", &bcm6);
  helChain->SetBranchAddress("Aphotgate", &asymm);

  fprintf(polstats,"#CutNum\t AsymmPDLwAvg\t AsymmPDAvgErr \tNumEnt\t"
	  " AsymmScLwAvg\t AsymmScLwErr\tAsymmLuTable\t AsymmLuTabErr\t"
	  " PD_ScaBkgd\t PD_ScaBkgdErr\t ScintScaBkgd\t ScintScaBkgdErr\n");
 
  //Now loop over helChain and form asymmetries
  cntr = nCycles;
  nCycles = 0;//reset counter
  beamOffLatch = kFALSE;
  for(Int_t i=0;i<cntr;i++)
  {
    enoughData = lasWisePhotonAvg[nCycles] > -9998;
    Int_t j = lasWiseCuts[nCycles*2+1];
    inOnCycle = enoughData;
    weight = dsScint - (Int_t)dsScint;

    while(inOnCycle)
    {
      inOnCycle = j<lasWiseCuts[(nCycles+1)*2];
      helChain->GetEntry(j);//increment tree loop
      laserOn = lasPow > lasHCutVal;
      laserOff = lasPow < lasLCutVal;
      beamOn = bcm6 > currentCutVal && photon > SCALER_CUTOFF;//e-beam and HV on
      inRange =  (Int_t)dsScint >= MINRoI  && (Int_t)dsScint < MAXRoI;
      idx = (Int_t)((dsScint - MINSCINT)/RANGE*(Double_t)NBINS);
      //if(time>1060) inOnCycle = kFALSE;//this line for run 20932
      if(inRange) 
	inRange = inRange && lookUp[idx] != 0 && lookUpErr[idx] < MAXLuTERR;

      if(!beamOn)beamOffLatch = kTRUE;
      if(beamOn && beamOffLatch==kTRUE)
      {   
	j += N_BT_SKIP;//skip entries to allow beam to restabilize after trip
	beamOffLatch = kFALSE;
      }
      if(inRange && laserOn)//only fill if backgrounds below cutoff and laser on
	                    // and lookup table has a value
      {
	bkgdLU = lookUp[idx];
	//printf("bkgdLU(scint=%2.2f) = lookUp[%i] = %f.\n", dsScint,
	//       idx, lookUp[idx]);
	bkgdAvg = dsScint * lasWisePhotonAvg[nCycles]/lasWiseScintAvg[nCycles];
	hAsymmLuT->Fill(asymm * photon / (photon - bkgdLU));
	hAsymmSc->Fill(asymm * photon / (photon - bkgdAvg));
	hAsymmPD->Fill(asymm*photon / (photon-lasWisePhotonAvg[nCycles]));    

      }
      j++;
    }

    enoughData = enoughData && hAsymmPD->GetEntries() > MIN_ENTRIES;

    if(enoughData)
    { 
      //printf("Calculating cut %i\n", nGoodCycles);
      hAsymmPDAvg->Add(hAsymmPD);
      hAsymmScintAvg->Add(hAsymmSc);
      hAsymmScintLuT->Add(hAsymmLuT);

      asymmPD[nGoodCycles] = 100*hAsymmPD->GetMean();
      errorXPD[nGoodCycles] = 0;
      errorYPD[nGoodCycles] = 100 * hAsymmPD->GetRMS()
	                   /TMath::Sqrt(hAsymmPD->GetEntries());
      asymmSc[nGoodCycles] = 100*hAsymmSc->GetMean();
      errorXSc[nGoodCycles] = 0;
      errorYSc[nGoodCycles] = 100 * hAsymmSc->GetRMS()
	                   /TMath::Sqrt(hAsymmSc->GetEntries());
      asymmLuT[nGoodCycles] = 100*hAsymmLuT->GetMean();
      errorXLuT[nGoodCycles] = 0;
      errorYLuT[nGoodCycles] = 100 * hAsymmLuT->GetRMS()
	                   /TMath::Sqrt(hAsymmLuT->GetEntries());
      cutNum[nGoodCycles] = nCycles;

      fprintf(polstats,"%d\t%12.5e\t%9.2e\t%d\t%12.5e\t%9.2e"
	      "\t%12.5e\t%9.2e\t%12.5e\t%9.2e\t%12.5e\t%9.2e\n",
	      nCycles,asymmPD[nGoodCycles], errorYPD[nGoodCycles],

	      (Int_t)hAsymmSc->GetEntries(),asymmSc[nGoodCycles],
	      errorYSc[nGoodCycles],asymmLuT[nGoodCycles],
	      errorYLuT[nGoodCycles], lasWisePhotonAvg[nCycles],
	      lasWisePhotonAvgErr[nCycles],lasWiseScintAvg[nCycles],
	      lasWiseScintAvgErr[nCycles]);  
      nGoodCycles ++;

    }
    else printf("Not enough data for cut %i.\n", nCycles);
    hAsymmPD->Reset();
    hAsymmSc->Reset();
    hAsymmLuT->Reset();
    nCycles++;
  }//end of for loop
  fclose(polstats);
  //for(Int_t i =0;i<nGoodCycles;i++)printf("cutNum[%i]=%f\n",i, cutNum[i]);

  TCanvas *c1 = new TCanvas("c1","c1",0,0,1400,800);
  c1->Divide(3,2);
  c1->cd(1);
  hAsymmPDAvg->Draw();
  c1->cd(2);
  hAsymmScintAvg->Draw();
  c1->cd(3);
  hAsymmScintLuT->Draw();
  c1->cd(4);
  //draw graph of asymmetries
  //gStyle->SetTitleX(0.1); //title X location
  //gStyle->SetTitleY(0.9); //title Y location
  gStyle->SetTitleW(0.61); //title width

  gStyle->SetTitleH(0.08); //title height
  TGraphErrors *gr = new TGraphErrors(nGoodCycles,cutNum,asymmPD,errorXPD,errorYPD);
  gr->GetXaxis()->SetTitle("Cut Number");
  gr->GetYaxis()->SetTitle("Scaler Asymmetry (%)");
  gr->GetYaxis()->SetTitleOffset(1.2);
  gr->SetTitle("PD Average Bkgnd Subtracted Scaler Asymmetry(%)");
  gr->SetMarkerColor(kRed);
  gr->SetMarkerStyle(8);
  gStyle->SetOptFit(1111);
  gr->Fit("pol0");
  gr->Draw("AP");
  c1->cd(5);
  TGraphErrors *gr1 = new TGraphErrors(nGoodCycles,cutNum,asymmSc,errorXSc,errorYSc);
  gr1->GetXaxis()->SetTitle("Cut Number");
  gr1->GetYaxis()->SetTitle("Scaler Asymmetry (%)");
  gr1->GetYaxis()->SetTitleOffset(1.2);
  gr1->SetTitle("Scint Avg Bkgnd Subtracted Scaler Asymmetry (%)");
  gr1->SetMarkerColor(kRed);
  gr1->SetMarkerStyle(8);
  gStyle->SetOptFit(1111);
  gr1->Fit("pol0");
  gr1->Draw("AP");

  c1->cd(6);
  TGraphErrors *gr2 = new TGraphErrors(nGoodCycles,cutNum,asymmLuT,errorXLuT,errorYLuT);
  gr2->GetXaxis()->SetTitle("Cut Number");
  gr2->GetYaxis()->SetTitle("Scaler Asymmetry (%)");
  gr2->GetYaxis()->SetTitleOffset(1.2);
  gr2->SetTitle("Lookup Table Bkgnd Subtracted Scaler Asymmetry (%)");
  gr2->SetMarkerColor(kRed);
  gr2->SetMarkerStyle(8);
  gStyle->SetOptFit(1111);
  gr2->Fit("pol0");
  gr2->Draw("AP");
  c1->Print(canvas1);

  cProf->Print(canvas2);

  //activate branches again
  helChain->SetBranchStatus("*", 1);

  return 1;
}
