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


////////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton run laser wise and plots the results    //
//using the scalers. It uses the background scintillator rates times a    //
//scale factor to subtract background.                                    //
//The inputs are run number and either 0 or 1 for whether or not the file //
//is a first100K file.                                                    //
////////////////////////////////////////////////////////////////////////////

Int_t analyzeScaler(Int_t runnum, Bool_t isFirst100K=kFALSE)
{
  gROOT->Reset();
  Bool_t debug = kTRUE;

  //const Double_t MAXCUR = 140, MIN_SCA_CUR = 235, MAX_SCA_CUR = 468;//bcm2_3h05a  values
  const Double_t MAXCUR = 60, MIN_SCA_CUR = 235, MAX_SCA_CUR = 292;//bcm6_3c17 values
  const Int_t MIN_ENTRIES = 2000;//min # of entries to use a laserwise cycle
  const Int_t NCUTS = 500;//initialized size of TCut arrays.
  const Int_t PATHLENGTH = 255;
  const Int_t N_BT_SKIP = 5000;// # of entries to skip after beam trip to allow to stabilize
  const Double_t  MINC13 = 0;//minimum background rate on profile plot for lookup table 
  const Double_t  MAXC13 = 25;//maximum background rate on profile plot for lookup table 
  const Double_t  MINRoI = 10;//minimum background rate "region of interest"
  const Double_t  MAXRoI = 19;//maximum background rate "region of interest"
  const Double_t BCMGAIN = MAXCUR/(MAX_SCA_CUR-MIN_SCA_CUR);
  const Double_t SCALER_CUTOFF=1;//if scaler below this likely HV tripped
  const Double_t MINLASER = 2;//laser off if below this scaler value
  const Int_t NBINS = 200;
  const Double_t ARIGHT = 2;//right limit of asymmetry plot
  const Double_t ALEFT = (-1) * ARIGHT;//left limit of asymmetry plot

  char canvas1[PATHLENGTH], canvas2[PATHLENGTH];// canvas3[PATHLENGTH];
  char filename[PATHLENGTH];

  Bool_t local = kFALSE, chainExists = kFALSE, flipperIsUp = kFALSE;
  Bool_t laserOn, beamOn, laserOff, enoughData, inOnCycle, inRange;
  Bool_t beamOffLatch = kFALSE, doneSkipAfterBTrip;

  Int_t lasWiseCuts[NCUTS];//entry # of first and last of all laser cycles
  Int_t nOffEvtsInRow = 0, nCycles = -1, nEntries, cntr = 0;
  Int_t nGoodCycles = 0, entry=0, lBin, hBin, setPnt = 0;

  Double_t lasPow, bcm6, photon, scintC13, asymm, weight, bkgdLU, bkgdAvg; 
  Double_t lasHCutVal, lasLCutVal, currentCutVal, hBinX, lBinX, hBinY, lBinY, scale;
  Double_t lasWisePhoton[NCUTS], lasWisePhotonErr[NCUTS];
  Double_t lasWisePhotonAvg[NCUTS], lasWisePhotonAvgErr[NCUTS];//average of adjacent bkgd's
  Double_t lasWiseScint[NCUTS], lasWiseScintErr[NCUTS];
  Double_t lasWiseScintAvg[NCUTS], lasWiseScintAvgErr[NCUTS];//average of adjacent scints
  Double_t asymmPD[NCUTS];//laswise PD avg calculated bkgd subtraction
  Double_t asymmSc[NCUTS];//laswise scintillator calculated bkgd subtraction
  Double_t asymmLuT[NCUTS];//lookup table background subtraction using PD/C13 correlation
  Double_t errorXPD[NCUTS], errorXSc[NCUTS], errorXLuT[NCUTS];
  Double_t errorYPD[NCUTS], errorYSc[NCUTS], errorYLuT[NCUTS];
  Double_t cutNum[NCUTS], lookUp[4*(Int_t)MAXC13], count[4*(Int_t)MAXC13];

  FILE *polstats;//file for storing stats.

  TChain *helChain = new TChain("Hel_Tree");
  TChain *mpsChain = new TChain("Mps_Tree");

  //scaler asymmetry background subtracted by laserwise averaging PD rates 
  TH1D *hAsymmPDAvg = new TH1D("hAsymPDAvg","Scaler Asymmetry Using"
			      " PD Avg Laswise Rates", NBINS, ALEFT, ARIGHT);
  //scaler asymmetry corrected using scintillator for background subtraction
  //by forming lookup table for correlation between PD and scintC13 rates and
  //using scintC13 as monitor for PD background subtraction.
  TH1D *hAsymmScintLuT = new TH1D("hAsymmScintLuT","Scaler Asymmetry Using C13 with"
			 " Look Up Table", NBINS, ALEFT, ARIGHT);
  //scaler asymmetry corrected using scintillator for background subtraction
  //by finding laswise average scale factor PD/scintC13 and multiplying by 
  //scintC13 on an event by event basis.
  TH1D *hAsymmScintAvg = new TH1D("hAsymmScintAvg","Scaler Asymmetry Using C13 "
			 "Avg Laswise Rates ", NBINS, ALEFT, ARIGHT);//subtract avg laswise rates
  TH1D *hAsymmLuT = new TH1D("hAsymmLuT","hAsymmLuT", NBINS, ALEFT, ARIGHT);
  TH1D *hAsymmSc = new TH1D("hAsymmSC","hAsymmSc", NBINS, ALEFT, ARIGHT);
  TH1D *hAsymmPD = new TH1D("hAsymmPD","hAsymmPD", NBINS, ALEFT, ARIGHT);
  helChain->SetAlias("bcmYield","yield_bcm6_3c17"); 

  // Open either Pass1 or the First 100K
  if( isFirst100K ) 
  {
    mpsChain->Add(Form("%s/first100k_%d.root",
		       getenv("QW_ROOTFILES"),runnum));
    chainExists = helChain->Add(Form("%s/first100k_%d.root",
				     getenv("QW_ROOTFILES"),runnum));
  } 
  else 
  {
    mpsChain->Add(Form("%s/Compton_Pass1_%d.*.root",
		       getenv("QW_ROOTFILES"),runnum));
    chainExists = helChain->Add(Form("%s/Compton_Pass1_%d.*.root",
				     getenv("QW_ROOTFILES"),runnum));
  }
  if(chainExists == 0){//check local folder in case running locally
    mpsChain->Add(Form("Compton_Pass1_%d.*.root",runnum));
    chainExists = helChain->Add(Form("Compton_Pass1_%d.*.root",runnum));

    local = kTRUE;
  }
  if(!chainExists){//delete chains and exit if files do not exist
    cout<<"File does not exist."<<endl;
    helChain->TChain::~TChain();
    mpsChain->TChain::~TChain();
    return 0;
  }
  printf("%d files attached to chain.\n",chainExists);
  nEntries = helChain->GetEntries();
  printf("This chain has %i entries.\n", nEntries);
  if(local == kFALSE)
  {
    sprintf(canvas1,"www/run_%d/analyzeScaler_%d_plots.png",runnum,runnum);
    sprintf(canvas2,"www/run_%d/analyzeScaler_%d_LookupTable.png",runnum,runnum);
    sprintf(filename,"www/run_%d/analyzeScaler_%d_stats.txt",runnum,runnum);
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
  gStyle->SetStatH(0.05);
  TCanvas *cProf = new TCanvas("cProf", "cProf", 0, 0, 1200, 500);
  cProf->Divide(2,1);
  cProf->cd(1);
  //scintC13 is an # of counts per mps averaged over a quartet helicity
  //pattern so binning the histo with 4*scint rates gives the best possible binning
  helChain->Draw(Form("yield_sca_photon_rate_gen:yield_sca_laser_C13>>hProf(%i,0,%f)",
		      4*(Int_t)(MAXC13), MAXC13), 
		 Form("yield_sca_laser_PowT<%f",MINLASER), "prof goff");
  TProfile *hProf = (TProfile*)gDirectory->Get("hProf");
  hProf->GetXaxis()->SetTitle("ScintC13 Background Scaler (Counts)");
  hProf->GetYaxis()->SetTitle("PD Scaler (Counts)");
  hProf->SetTitle("Correlation Plot: PD Scaler vs Background Scintillator Scaler");
  hProf->SetMarkerColor(kBlue);
  hProf->SetLineColor(kBlue);
  hProf->Draw();  
  helChain->Draw(Form("yield_sca_photon_rate_gen:yield_sca_laser_C13>>hProf1(%i,%f,%f)",
		      4*(Int_t)(MAXC13), MINC13, MAXC13), 
		 Form("yield_sca_laser_PowT<%f && "
		      "yield_sca_laser_C13<%f && yield_sca_laser_C13>%f",
		      MINLASER, MAXRoI, MINRoI), "prof goff");

  TProfile *hProf1 = (TProfile*)gDirectory->Get("hProf1");
  hProf1->SetMarkerColor(kRed);
  hProf1->SetLineColor(kRed);
  hProf1->Draw("same"); 

  TLegend *legend1 = new TLegend(0.88,0.55,0.98,0.65);
  //legend1->SetHeader("Some histograms");
  legend1->AddEntry(hProf,"Cut","l");
  legend1->AddEntry(hProf1,"Used","l");
  legend1->SetFillColor(0);
  legend1->Draw();
  cProf->cd(2);
  gStyle->SetStatH(0.15);
  scale = MAXC13 * hProf->GetMean(2)/hProf->GetMean(1);
  helChain->Draw(Form("yield_sca_photon_rate_gen:yield_sca_laser_C13"
		      ">>hCor(200,0,%f,200)",MAXC13),
		 Form("yield_sca_laser_PowT<%f && "
		      "yield_sca_photon_rate_gen<%f &&"
		      "yield_sca_laser_C13<%f", MINLASER, scale, MAXC13), "goff");
  TH2D *hCor = (TH2D*)gDirectory->Get("hCor");
  // hCor->SetAxisRange(0,MAXC13, "X");
  // hCor->SetAxisRange(0,1.4*hProf->GetMaximum(), "Y");
  hCor->GetXaxis()->SetTitle("ScintC13 Background Scaler (Counts)");
  hCor->GetYaxis()->SetTitle("PD Scaler (Counts)");
  hCor->SetTitle("PD Scaler vs Background Scintillator Scaler");
  hCor->SetMarkerColor(kBlue);
  hCor->SetLineColor(kBlue);
  hCor->Draw("scat=1");  


  lBin = hProf->FindFirstBinAbove(MINC13);
  lBin = TMath::Max(lBin, 4*(Int_t)MINC13);
  hBin = hProf->FindLastBinAbove(MINC13);
  if(hBin!=-1)hBin = TMath::Min(hBin, 4*(Int_t)MAXC13);
  else hBin = (Int_t)MAXC13;                        

  //complete lookup table using values from hProf
  for(Int_t i=0; i<hBin;i++)
  {
    count[i] = (Double_t)i;
    lookUp[i] = hProf->GetBinContent(i+1);
  }

  cout<<"hBin = "<<hBin<<endl;

  //define temporary histograms  
  lBinX = helChain->GetMinimum("yield_sca_laser_C13");
  hBinX = helChain->GetMaximum("yield_sca_laser_C13");
  TH1D *hScint = new TH1D("hScint","Scint Background",NBINS, lBinX, hBinX);
  lBinY = helChain->GetMinimum("yield_sca_photon_rate_gen");
  hBinY = helChain->GetMaximum("yield_sca_photon_rate_gen");
  TH1D *hPhoton = new TH1D("hPhoton","PD Background",NBINS,lBinY, hBinY);
  printf("lBinX = %f, hBinX = %f\n",lBinX, hBinX);
  printf("lBinY = %f, hBinY = %f\n",lBinY, hBinY);

  for(Int_t i=0; i<hBin; i++)
    printf("lookUp[%i] = %f\n",(Int_t)count[i],lookUp[i]);

  //define cut values 
  currentCutVal = 0.8*(helChain->GetMaximum("yield_bcm6_3c17")-MIN_SCA_CUR)
                  * BCMGAIN + MIN_SCA_CUR;
  helChain->Draw("yield_sca_laser_PowT>>h(100,0)","yield_sca_laser_PowT>10", "goff");
  TH1F *h = (TH1F*)gDirectory->Get("h");
  lasHCutVal = 0.8*h->GetMean();
  lasLCutVal = MINLASER;
  printf("Laser power cut is %f.\n", lasHCutVal);
  printf("Current cut is %f.\n", currentCutVal);
  //turn off all branches then turn on the ones needed.
  helChain->SetBranchStatus("*",0);
  helChain->SetBranchStatus("yield_bcm6_3c17",1);
  helChain->SetBranchStatus("yield_sca_laser_PowT",1);
  helChain->SetBranchStatus("yield_sca_photon_rate_gen",1);
  helChain->SetBranchStatus("yield_sca_laser_C13",1);

  //set branch addresses
  helChain->SetBranchAddress("yield_bcm6_3c17", &bcm6);
  helChain->SetBranchAddress("yield_sca_laser_PowT", &lasPow);
  helChain->SetBranchAddress("yield_sca_photon_rate_gen", &photon);
  helChain->SetBranchAddress("yield_sca_laser_C13", &scintC13);



  ////////////////////////////////////////////////////////////
  //Loop over Hel_Tree and find laser cycles, backgrounds,  // 
  //and calibration factors for relating scintillator rates //
  //("yield_sca_laser_C13") to rates in the photon detector //
  //("yield_sca_photon_rate_gen").                          //
  ////////////////////////////////////////////////////////////
  // TCanvas *ctemp = new TCanvas("ctemp","ctemp",0,0,400,500);
  for(Int_t i=0;i<nEntries;i++)
  {
    helChain->GetEntry(i);
    laserOn = lasPow > lasHCutVal;
    laserOff = lasPow <= lasLCutVal;
    beamOn = bcm6 > currentCutVal && photon > SCALER_CUTOFF;//e-beam and HV on
    doneSkipAfterBTrip = setPnt == 0;

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
	hScint->Fill(scintC13);
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
	  //hScint->Draw();
	  //sprintf(canvas3,"~/users/jonesdc/compton_runs/misc/hist%i.png", nCycles);
	  //ctemp->Print(canvas3);
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
      i+=10;//go faster through on cycle;
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
    printf("lBin = %i,  hBin = %i.\n", lBin, hBin);
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
  helChain->SetBranchStatus("asym_sca_photon_rate_gen", 1);//turn on branch
  helChain->SetBranchAddress("yield_sca_laser_PowT", &lasPow);


  helChain->SetBranchAddress("yield_bcm2_3h05a", &bcm6);
  helChain->SetBranchAddress("asym_sca_photon_rate_gen", &asymm);

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
    weight = scintC13 - (Int_t)scintC13;

    while(inOnCycle)
    {
      inOnCycle = j<lasWiseCuts[(nCycles+1)*2];
      helChain->GetEntry(j);//increment tree loop
      laserOn = lasPow > lasHCutVal;
      laserOff = lasPow <= lasLCutVal;
      beamOn = bcm6 > currentCutVal && photon > SCALER_CUTOFF;//e-beam and HV on
      inRange =  ((Int_t)(scintC13*4) >= lBin)  && ((Int_t)(scintC13*4) < hBin);
      if(inRange) inRange = inRange && (lookUp[(Int_t)(scintC13 *4)] != 0);

      if(!beamOn)beamOffLatch = kTRUE;
      if(beamOn && beamOffLatch==kTRUE)
      {   
	j += N_BT_SKIP;//skip entries to allow beam to restabilize after trip
	beamOffLatch = kFALSE;
      }
      if(inRange && laserOn)//only fill if backgrounds below cutoff and laser on
	                    // and lookup table has a value
      {
	bkgdLU = lookUp[(Int_t)(scintC13 *4)];
	//printf("bkgdLU(scint=%2.2f) = lookUp[%i] = %f.\n", scintC13,
	//     (Int_t)(scintC13*4),lookUp[(Int_t)(scintC13*4)]);
	bkgdAvg = scintC13 * lasWisePhotonAvg[nCycles]/lasWiseScintAvg[nCycles];
	hAsymmLuT->Fill(asymm * photon / (photon - bkgdLU));
	hAsymmSc->Fill(asymm * photon / (photon - bkgdAvg));
	hAsymmPD->Fill(asymm * photon / (photon-lasWisePhotonAvg[nCycles]));    
      }
      j++;
    }

    enoughData = enoughData && hAsymmPD->GetEntries() > MIN_ENTRIES;

    if(enoughData)
    { 
      //printf("%i good cycles out of %i cycles\n", nGoodCycles, nCycles);
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
  for(Int_t i =0;i<nGoodCycles;i++)printf("cutNum[%i]=%f\n",i, cutNum[i]);

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
  mpsChain->SetBranchStatus("*", 1);
  helChain->SetBranchStatus("*", 1);

  return 1;
}
