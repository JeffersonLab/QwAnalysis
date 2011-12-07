#include "getRightmost.C"
#include "getLeftmost.C"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
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
#include <stdio.h>
#include <stdlib.h>
#include "/home/cdaq/compton/QwAnalysis/Analysis/include/QwSIS3320_Samples.h"

using namespace std;



////////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton run laser wise and plots the asymmetry. //
//It also stores the results of each laser wise asymmetry and differences //
//in a file called "Run_#_pedsubasymm.txt".                               //
//If "clear" is nonzero, the canvases will be deleted at the end.         //
//The inputs are run number and either 0 or 1 for whether or not the file //
//is a first100K file.                                                    //
////////////////////////////////////////////////////////////////////////////

Int_t getAsymmetryTree(Int_t runnum, Int_t segnum=0){
  gROOT->Reset();
  Bool_t debug = kTRUE;
  
//create useful variables
  const Double_t SCA_MIN_CUR = 235;//bcm scaler rate for 0 current
  const Double_t SCALER_CUTOFF = 20;//HV considered off if scaler rare less
  const Int_t MIN_ENTRIES = 2000;//min # of entries to use a laserwise cycle
  const Int_t PATHLENGTH =255, NCUTS = 100;
  Bool_t local ,flipperIsUp = kFALSE;
  Bool_t laserOn, laserOff, beamOn, noPrevEvt, pulseIsCentered, sampleExists;
  char canvas1[PATHLENGTH], filename[PATHLENGTH];
  Int_t nOffEvtsInRow = 0, nCycles = -1, nTree = 0, nEntries, nSkipped = 0;
  Int_t nGoodCycles = 0;
  Int_t mpsCounterCut[NCUTS];
  Double_t mpsCounter;
  Double_t sca_laser_PowT, bcm2, currentCutVal, Accum0_hw_sum, scaler;
  Double_t cutLower, cutUpper, hBinX, lBinX, Accum0_num_samples=0;
  Double_t pedestal = 0, minSample = 0, maxIndex = 0;
  Double_t lasWisePedOn[NCUTS], lasWisePedOnErr[NCUTS];
  Double_t lasWisePedOff[NCUTS], lasWisePedOffErr[NCUTS];
  Double_t lasWisePedShift[NCUTS], lasWisePedShiftErr[NCUTS];
  Double_t lasWiseYieldOn[NCUTS],lasWiseYieldOnErr[NCUTS];
  Double_t lasWiseBkgd[NCUTS],lasWiseBkgdErr[NCUTS];
  Double_t lasWiseBkgdAvg[NCUTS],lasWiseBkgdAvgErr[NCUTS];//avg two adjcnt Off states 
  Double_t pedAdjBkgd[NCUTS],pedAdjBkgdErr[NCUTS];
  std::vector<QwSIS3320_Samples>* sample = 0;

  TFile *file = TFile::Open(Form("$QW_ROOTFILES/Compton_Pass1_%i.00%i.root",runnum,segnum));
  TTree *mpsTree = (TTree*)file->Get("Mps_Tree");
  TTree *helTree = (TTree*)file->Get("Hel_Tree");

  local = kFALSE;
  if(!file){//check local folder in case running locally
    cout<<"File does not exist."<<endl;
    helTree->TTree::~TTree();
    mpsTree->TTree::~TTree();
    return 0;
  }

  sprintf(canvas1,"wwwdon/Run_%d/Run_%d_pedAdjAsymm.png",runnum,runnum);
  sprintf(filename,"wwwdon/Run_%d/Run_%d_pedAdjStats.txt",runnum,runnum);

  //Define current cut.
  currentCutVal = 0.8*(Double_t)mpsTree->GetMaximum("bcm2_3h05a");
  currentCutVal += 0.2*SCA_MIN_CUR;//take into account the offset at zero current.
  TCut currentCut = Form("bcm2_3h05a>%f",currentCutVal);

  //Define laser cut.
  mpsTree->Draw("sca_laser_PowT>>hTemp","sca_laser_PowT>10","goff");
  TH1F *hTemp = (TH1F*)gDirectory->Get("hTemp");
  cutLower = hTemp->GetMean()*0.9; 
  cutUpper = hTemp->GetMean()*1.3;
  printf("Laser: cutLower = %f    cutUpper = %f.\n", cutLower, cutUpper);
  hTemp->TH1F::~TH1F();//don't need this histo anymore.

  //find good values for limits of background histo
  mpsTree->Draw("fadc_compton_accum0.hw_sum>>hTemp3",
		 currentCut,"goff");
  TH1F *hTemp3 = (TH1F*)gDirectory->Get("hTemp3");
  lBinX = hTemp3->GetMean()*0.5;
  hBinX = hTemp3->GetMean()*1.5;
  printf("lBinx=%f    hBinX=%f\n", lBinX, hBinX);
  hTemp3->TH1F::~TH1F(); 

  //create useful histo's
  TH1F *hPedOn = new TH1F("hPedOn","hPedOn",1000,-50,50);
  TH1F *hPedOff = new TH1F("hPedOff","hPedOff",1000,-50,50);
  TH1F *hYieldOn = new TH1F("hYieldOn","hYieldOn", 1000,lBinX,hBinX); 
  TH1F *hBackground = new TH1F("hBackground","Background", 1000,lBinX,hBinX); 


  //turn off all branches that aren't needed before looping
  mpsTree->SetBranchStatus("*",0);//turn off all branches
  mpsTree->SetBranchStatus ("fadc_compton_samples*",1);//turn on desired branches
  mpsTree->SetBranchStatus("sca_laser_PowT",1);//turn on desired branches
  mpsTree->SetBranchStatus("bcm2_3h05a", 1);//turn on desired branches
  mpsTree->SetBranchStatus("fadc_compton_accum0*", 1);//turn on desired branches
  mpsTree->SetBranchStatus("sca_photon_rate_gen", 1);//turn on branches wanted
  mpsTree->SetBranchStatus("mps_counter", 1);//turn on branches wanted

  //define mps variables
  mpsTree->SetBranchAddress("fadc_compton_samples",&sample);//set addresses
  mpsTree->SetBranchAddress("sca_laser_PowT", &sca_laser_PowT);
  mpsTree->SetBranchAddress("bcm2_3h05a", &bcm2);
  mpsTree->SetBranchAddress("sca_photon_rate_gen",&scaler);
  mpsTree->SetBranchAddress("mps_counter",&mpsCounter);
  if(debug){
    printf("fadc_compton_samples status =%i.\n",
	   mpsTree->GetBranchStatus("fadc_compton_samples"));
    printf("sca_laser_PowT status =%i.\n",
	   mpsTree->GetBranchStatus("sca_laser_PowT"));
    printf("bcm2_3h05a status =%i.\n",
	   mpsTree->GetBranchStatus("bcm2_3h05a"));
    printf("fadc_compton_accum0 status =%i.\n",
	   mpsTree->GetBranchStatus("fadc_compton_accum0"));
    printf("sca_photon_rate_gen=%i.\n",
	   mpsTree->GetBranchStatus("sca_photon_rate_gen"));
    printf("mps_counter=%i.\n",
	   mpsTree->GetBranchStatus("mps_counter"));
  }

  //get access to leaves inside branch
  TBranch *bAccum0 = (TBranch*)mpsTree->GetBranch("fadc_compton_accum0");
  TLeaf *lAccum0_hw_sum = (TLeaf*)bAccum0->GetLeaf("hw_sum");
  TLeaf *lAccum0_num_samples = (TLeaf*)bAccum0->GetLeaf("num_samples");
  //  cout<<lAccum0_hw_sum<<" "<<lAccum0_num_samples<<endl;
  //output number of entries
  nEntries = mpsTree->GetEntries();
  printf("There are %i entries in mpsTree for run %i.\n",nEntries, runnum);



  //////////////////////////////////////////////////////////////////////
  //Loop through MPS tree getting the pedestal shifts and backgrounds.//
  //////////////////////////////////////////////////////////////////////

  nCycles=-1;
  for(Int_t i=0;i<nEntries;i+=2){
     mpsTree->GetEntry(i);
     /*
     if(debug && i%10000==0){
       printf("TLeaf hw_sum = %f\n",lAccum0_hw_sum->GetValue());
       printf("TLeaf num_samples = %f\n",lAccum0_num_samples->GetValue());
     }
     */

    Accum0_hw_sum = lAccum0_hw_sum->GetValue();
    Accum0_num_samples = lAccum0_num_samples->GetValue();
    laserOn = sca_laser_PowT > cutLower && sca_laser_PowT < cutUpper;
    laserOff = sca_laser_PowT <= 3.0;
    sampleExists = sample->size() > 0;

    //cause seg fault if try to Get*() and sample doesn't exist
    if(sampleExists){
      pedestal = sample->at(0).GetPedestal();
      minSample = sample->at(0).GetMinSample();
      maxIndex = sample->at(0).GetMaxIndex(); 
      noPrevEvt = pedestal - minSample <10;
      beamOn = bcm2 > currentCutVal && scaler > SCALER_CUTOFF;//e-beam and HV on
      pulseIsCentered = maxIndex > 20 && maxIndex < 100;
    

      //start counting consecutive laserOff events if laser turns off.
      if(laserOff){
	nOffEvtsInRow++;

	if(beamOn && pulseIsCentered && noPrevEvt){
	  if(Accum0_hw_sum>lBinX && Accum0_hw_sum<hBinX){//cut outliers???good idea???
	    hPedOff->Fill(pedestal);
	  }else printf("hw_sum = %f. Considered outlier and not included.\n",
		      Accum0_hw_sum);
	}

	if(beamOn)hBackground->Fill(Accum0_hw_sum);//fill histo with laser off yield

      }else if(laserOn){
  
	if(flipperIsUp){
	  if(hPedOff->GetEntries()>MIN_ENTRIES){//only store if sufficient data
	    lasWisePedOff[nCycles] = hPedOff->GetMean(); 
	    lasWisePedOffErr[nCycles] = hPedOff->GetRMS()
	                                /TMath::Sqrt((Double_t)hPedOff->GetEntries());
	    lasWiseBkgd[nCycles] = hBackground->GetMean();
	    lasWiseBkgdErr[nCycles] = hBackground->GetRMS()
	                              /TMath::Sqrt((Double_t)hBackground->GetEntries());
	    if(nCycles==0)
	    {
	      printf("First Laser Off entry = %i\n",i-nOffEvtsInRow);
	      mpsCounterCut[0] = (Int_t)mpsCounter - nOffEvtsInRow;//cycle change mps value
	    }else
	    {
	    printf("Laser OFF:  lasWisePedOff[%i]= %11.4e +/- %7.1e:  %d entries\n"
		   "              lasWiseBkgd[%i]= %11.4e +/- %7.1e:  %d entries\n"
		   "Begin Laser On: Entry = %i, mps_counter = %i\n",
		   nCycles, lasWisePedOff[nCycles],lasWisePedOffErr[nCycles],
		   (Int_t)hPedOff->GetEntries(),
		   nCycles, lasWiseBkgd[nCycles],lasWiseBkgdErr[nCycles],
		   (Int_t)hBackground->GetEntries(),
		   i,(Int_t)mpsCounter);
	    mpsCounterCut[nCycles*2] = (Int_t)mpsCounter;//cycle change mps value
	    }
	  }else
	  {
	    lasWisePedOff[nCycles] = -9999; 
	    lasWisePedOffErr[nCycles] = -9999;
	    lasWiseBkgd[nCycles] = -9999;
	    lasWiseBkgdErr[nCycles] = -9999;
	    mpsCounterCut[nCycles*2] = (Int_t)mpsCounter;//cycle change mps value
	    printf("Laser OFF:  lasWisePedOff[%i]= %1.0f. Insufficient OFF data:  %d entries\n"
		   "              lasWiseBkgd[%i]= %1.0f.                         %d entries\n"
		   "Begin Laser On: Entry = %i, mps_counter = %i\n",
		   nCycles, lasWisePedOff[nCycles],(Int_t)hPedOff->GetEntries(),
		   nCycles, lasWiseBkgd[nCycles],(Int_t)hBackground->GetEntries(),
		   i, (Int_t)mpsCounter);
	  }
	  flipperIsUp = kFALSE;
	  hPedOff->Reset();
	  hBackground->Reset();
	}
	if(beamOn && pulseIsCentered && noPrevEvt){
	  hPedOn->Fill(pedestal);
	}

	if(beamOn)hYieldOn->Fill(Accum0_hw_sum);//fill histo with laser on yield
	nOffEvtsInRow = 0;

      }else nOffEvtsInRow = 0;

      if(nOffEvtsInRow==MIN_ENTRIES*4 || nSkipped!=0){
	nCycles++;
	if(nCycles!=0 && hPedOn->GetEntries()>MIN_ENTRIES){
	  lasWisePedOn[nCycles-1] = hPedOn->GetMean(); 
	  lasWisePedOnErr[nCycles-1] = hPedOn->GetRMS()
	                        /TMath::Sqrt((Double_t)hPedOn->GetEntries());
	  lasWiseYieldOn[nCycles-1] = hYieldOn->GetMean();
	  lasWiseYieldOnErr[nCycles-1] = hYieldOn->GetRMS()
	                        /TMath::Sqrt((Double_t)hYieldOn->GetEntries());

	  printf("Laser ON:   lasWisePedOn[%i]= %11.4e +/- %7.1e:  %d entries\n"
		 "          lasWiseYieldOn[%i]= %11.4e +/- %7.1e:  %d entries\n"
		 "Begin Laser Off: Entry = %i\n",
		 nCycles-1, lasWisePedOn[nCycles-1], lasWisePedOnErr[nCycles-1],
		 (Int_t)hPedOn->GetEntries(),
		 nCycles-1, lasWiseYieldOn[nCycles-1],lasWiseYieldOnErr[nCycles-1],
		 (Int_t)hYieldOn->GetEntries(), i-MIN_ENTRIES*4); 

	  mpsCounterCut[nCycles*2-1] = (Int_t)mpsCounter;//cycle change mps value

	}else if(nCycles !=0){
	  lasWisePedOn[nCycles-1] = hPedOn->GetMean(); 
	  lasWisePedOnErr[nCycles-1] = hPedOn->GetRMS()
	                        /TMath::Sqrt((Double_t)hPedOn->GetEntries());

	  printf("Laser ON:   lasWisePedOn[%i] = %1.0f. Insufficient ON data:%d entries\n"
		 "          lasWiseYieldOn[%i] = %1.0f.                     :%d entries\n" 
		 "Begin Laser Off: Entry = %i\n",
		 nCycles-1, lasWisePedOn[nCycles-1], (Int_t)hPedOn->GetEntries(),
		 nCycles-1, lasWiseYieldOn[nCycles-1], (Int_t)hYieldOn->GetEntries(),i); 

	  mpsCounterCut[nCycles*2-1] =(Int_t) mpsCounter;//cycle change mps value
	}
	hPedOn->Reset();//reset On histogram
	hYieldOn->Reset();//reset On histogram
	flipperIsUp = kTRUE;
      }
      nSkipped = 0;
    }else if(nOffEvtsInRow==MIN_ENTRIES*4){
      nSkipped++;
      if(debug)printf("nSkipped= %i.\n",nSkipped);
    }

    //if last point store laser off data if more than MIN_ENTRIES samples available
    if(i==nEntries-1 && nOffEvtsInRow>=MIN_ENTRIES*4){
      if(hPedOff->GetEntries()>MIN_ENTRIES*4){//only store if sufficient data
	lasWisePedOff[nCycles] = hPedOff->GetMean(); 
	lasWisePedOffErr[nCycles] = hPedOff->GetRMS()
	  /TMath::Sqrt((Double_t)hPedOff->GetEntries());
	lasWiseBkgd[nCycles] = hBackground->GetMean();
	lasWiseBkgdErr[nCycles] = hBackground->GetRMS()
	  /TMath::Sqrt((Double_t)hBackground->GetEntries());
	printf("Laser OFF:  lasWisePedOff[%i]= %11.4e +/- %7.1e:  %d entries\n"
	       "              lasWiseBkgd[%i]= %11.4e +/- %7.1e:  %d entries.\n"
	       "Final Laser Off: Entry = %i, mps_counter = %i\n",
	       nCycles, lasWisePedOff[nCycles],lasWisePedOffErr[nCycles],
	       (Int_t)hPedOff->GetEntries(), nCycles, lasWiseBkgd[nCycles],
	       lasWiseBkgdErr[nCycles], (Int_t)hBackground->GetEntries(), 
	       i, (Int_t)mpsCounter);
      }else{
	lasWisePedOff[nCycles] = -9999; 
	lasWisePedOffErr[nCycles] = -9999;
	lasWiseBkgd[nCycles] = -9999;
	lasWiseBkgdErr[nCycles] = -9999;
	printf("Laser OFF:  lasWisePedOff[%i]= %1.0f. Insufficient OFF data.\n"
	       "           lasWiseBkgd[%i]= %1.0f. Insufficient OFF data.\n"
	       "Final Laser Off: Entry = %i, mps_counter = %i\n",
	       nCycles, lasWisePedOff[nCycles],nCycles, lasWiseBkgd[nCycles],
	       i, (Int_t)mpsCounter);
      }
    }
  }
  if(debug){
    printf("Check this list against the previous for agreement.\n");
    for(Int_t i=0;i<nCycles;i++){
      printf(" lasWisePedOff[%i]=%11.4e +/- %7.1e.\n",
	     i, lasWisePedOff[i],lasWisePedOffErr[i]);
      printf("  lasWisePedOn[%i]=%11.4e +/- %7.1e.\n",
	     i, lasWisePedOn[i],lasWisePedOnErr[i]);
      printf("lasWiseYieldOn[%i]= %11.4e +/- %7.1e.\n",
	     i, lasWiseYieldOn[i],lasWiseYieldOnErr[i]);
      printf("   lasWiseBkgd[%i]= %11.4e +/- %7.1e.\n",
	     i, lasWiseBkgd[i],lasWiseBkgdErr[i]);
    }
    printf("lasWisePedOff[%i]=%11.4e +/- %7.1e.\n",
	   nCycles, lasWisePedOff[nCycles],lasWisePedOffErr[nCycles]);
    printf("   lasWiseBkgd[%i]= %11.4e +/- %7.1e.\n",
	   nCycles, lasWiseBkgd[nCycles],lasWiseBkgdErr[nCycles]);
  }

  for(Int_t i=0;i<nCycles*2+1;i++)printf("mpsCounterCut[%i]=%i\n",i,mpsCounterCut[i]);

  for(Int_t i=0;i<nCycles;i++){
    Bool_t goodData = lasWisePedOff[i] > -9998 && lasWisePedOff[i+1] > -9998
                   && lasWisePedOn[i]  > -9998 && lasWiseBkgd[i]     > -9998 
                   && lasWiseBkgd[i+1] > -9998;
    if(goodData){
      nGoodCycles++;
      lasWisePedShift[i] = lasWisePedOn[i] - 0.5*(lasWisePedOff[i] + lasWisePedOff[i+1]); 
      lasWisePedShiftErr[i] = lasWisePedOnErr[i] +
	                      0.5*(lasWisePedOffErr[i] + lasWisePedOffErr[i+1]);
      lasWiseBkgdAvg[i] = 0.5 * (lasWiseBkgd[i] + lasWiseBkgd[i+1]);
      lasWiseBkgdAvgErr[i] = 0.5 * (lasWiseBkgdErr[i] + lasWiseBkgdErr[i+1]);
      pedAdjBkgd[i] = lasWiseBkgdAvg[i] + lasWisePedShift[i] * Accum0_num_samples;
      pedAdjBkgdErr[i] = lasWiseBkgdAvgErr[i] + lasWisePedShiftErr[i];

    }else{
      lasWisePedShift[i] = -9999;
      lasWisePedShiftErr[i] = -9999;
      lasWiseYieldOn[i] = -9999;
      lasWiseYieldOnErr[i] = -9999;
      lasWiseBkgdAvg[i] = -9999;
      pedAdjBkgd[i]= -9999;
      pedAdjBkgdErr[i] = -9999;
    }
    printf("lasWisePedShift[%i] = %11.4e +/- %7.1e.\n"
	   " lasWiseYieldOn[%i] = %11.4e +/- %7.1e.\n"
	   " lasWiseBkgdAvg[%i] = %11.4e +/- %7.1e.\n"
	   "     pedAdjBkgd[%i] = %11.4e +/- %7.1e.\n",
	   i, lasWisePedShift[i], lasWisePedShiftErr[i],
	   i, lasWiseYieldOn[i], lasWiseYieldOnErr[i],
	   i, lasWiseBkgdAvg[i], lasWiseBkgdAvgErr[i],
	   i, pedAdjBkgd[i], pedAdjBkgdErr[i]);

  }

  printf("There were %i good cycles out of %i cycles.\n", nGoodCycles, nCycles);
  helTree->ResetBranchAddresses();
  mpsTree->ResetBranchAddresses();
  //////////////////////////////////////////////
  //Loop through Hel_Tree to get the pedestal //
  //and  background subtracted asymmetries.   //
  //////////////////////////////////////////////


  //Redefine variables in terms of helTree.
  Bool_t enoughData = kFALSE;
  Int_t prevnCycles = 0, arrayLength = nCycles, numEnt = 0;
  Double_t yield_sca_laser_PowT, yield_bcm2, yieldAccum0, asymmAccum0, yieldScaler;
  Double_t difference;
  Double_t asymmetry, cutNum[NCUTS], errorX[NCUTS], errorY[NCUTS], asymm[NCUTS];

  //redefine current cut values in case different in helTree
  currentCutVal = 0.8*(Double_t)helTree->GetMaximum("yield_bcm2_3h05a")
                 +0.2*SCA_MIN_CUR;
  printf("Current cut is %f.\n",currentCutVal);
  printf("Using bcm2_3h05a.\n");

  //define laser cut
  helTree->Draw("yield_sca_laser_PowT>>hTemp2","yield_sca_laser_PowT>10","goff");
  TH1F *hTemp2 = (TH1F*)gDirectory->Get("hTemp2");
  cutLower = hTemp2->GetMean()*0.9;
  cutUpper = hTemp2->GetMean()*1.3;
  printf("Laser: cutLower = %f    CutUpper = %f.\n", cutLower, cutUpper);

  //output number of entries in helTree
  nEntries = helTree->GetEntries();
  printf("There are %i entries in helTree from Run %i.\n",nEntries, runnum);

  //create useful histo's
  TH1D *hAsymmetry = new TH1D("hAsymmetry","Asymmetry", 150,-4,4);
  TH1D *hAsymm = new TH1D("hAsymm","hAsymm", 150,-4,4);

  //create file for storing stats
  FILE *pedAdjStats = fopen(filename,"w");//file for storing stats.
  fprintf(pedAdjStats,"#CutNum\t Asymm\t\t AsymmRMS \tNumEnt\t"
	  " YieldOverBkgd \t Yield      \t YieldRMS \t"
	  " Background\t BkgdRMS \t PedestalShift\t PedShiftRMS\t "
	  "PedAdjBkgd\t PedAdjBkgdRMS\n");
 
  //create a canvas for plotting asymmetries
  TCanvas *c1 = new TCanvas("c1","c1",0,0,700,700);
  c1->Divide(1,2);
  TCanvas *c2 = new TCanvas("c2","c2",0,0,400,400);

  //turn off unwanted branches before looping through Hel_Tree
  helTree->SetBranchStatus("*",0);//turn off all branches
  helTree->SetBranchStatus("yield_sca_photon_rate_gen", 1);//turn on branch
  helTree->SetBranchStatus("yield_sca_laser_PowT", 1);//turn on branch
  helTree->SetBranchStatus("yield_bcm2_3h05a", 1);//turn on branch
  helTree->SetBranchStatus("yield_fadc_compton_accum0*", 1);//turn on branch
  helTree->SetBranchStatus("asym_fadc_compton_accum0*", 1);//turn on branch

  //define helicity variables 
  helTree->SetBranchAddress("yield_sca_laser_PowT", &yield_sca_laser_PowT);
  helTree->SetBranchAddress("yield_bcm2_3h05a", &yield_bcm2);
  helTree->SetBranchAddress("yield_sca_photon_rate_gen", &yieldScaler);

  //get access to leaves of branches
  TBranch * bYieldAccum0 = (TBranch*)helTree->
                           GetBranch("yield_fadc_compton_accum0");
  TLeaf* lYieldAccum0_hw_sum = (TLeaf*)bYieldAccum0->GetLeaf("hw_sum");
  TBranch* bAsymmAccum0 = (TBranch*)helTree->
                           GetBranch("asym_fadc_compton_accum0");
  TLeaf* lAsymm_hw_sum = (TLeaf*)bAsymmAccum0->GetLeaf("hw_sum");

  //delete previous histograms
  gSystem->Exec(Form("rm -f wwwdon/Run_%i/hist*",runnum));


  //Now loop over helTree and form asymmetries
  nTree = 0;
  nCycles = -1;//reset counter
  for(Int_t i=0;i<nEntries;i++)
  {

    helTree->GetEntry(i);//increment tree loop
    bYieldAccum0->GetEntry(i);
    bAsymmAccum0->GetEntry(i);

    //define conditions
    laserOn = yield_sca_laser_PowT>cutLower && yield_sca_laser_PowT<cutUpper;
    laserOff = yield_sca_laser_PowT <= 3.0;
    beamOn = yield_bcm2 > currentCutVal && yieldScaler > SCALER_CUTOFF;
    yieldAccum0 = lYieldAccum0_hw_sum->GetValue();
    asymmAccum0 = lAsymm_hw_sum->GetValue();
    enoughData = pedAdjBkgd[nCycles] > -9998;

    //start counting consecutive laserOff events if laser turns off.
    if(laserOff)
    {
      nOffEvtsInRow++;
    }
    else if(laserOn)
    { 
      nOffEvtsInRow = 0;
      if(flipperIsUp)
      {
	flipperIsUp = kFALSE;
      }

      //terminate loop if at end of pedAdjBkgd array.
      if(nCycles == arrayLength)
      {
	i = nEntries-1;
      }
      else if(beamOn && nCycles>=0)
      {
	if(prevnCycles==nCycles)
	{
	  printf("Now using pedAdjBkgd[%i]=%f\n",nCycles,pedAdjBkgd[nCycles]);
	  prevnCycles = nCycles + 1;
	}
	if(pedAdjBkgd[nCycles]> -9998)//do nothing if insufficient data
	{
	  difference = yieldAccum0*asymmAccum0;
	  asymmetry = difference/(yieldAccum0-pedAdjBkgd[nCycles]);
	  if(TMath::Abs(asymmetry)<4)//cut outliers
	  {
	    //if(i%1000==0)printf("%i\n",i);
	    hAsymmetry->Fill(asymmetry);
	    hAsymm->Fill(asymmetry);
	  }
	}
      }
    }
    if(nOffEvtsInRow==MIN_ENTRIES)//store ON data once sure ON state is passed 
    {
      printf("Entry = %i\n",i);
      nGoodCycles++;
      nCycles++;
      if(nCycles>0 && enoughData)
      {
	asymm[nCycles-1] = 100*hAsymm->GetMean();
	cutNum[nCycles-1] = nCycles;
	errorY[nCycles-1] = 100*hAsymm->GetRMS()
	                    /TMath::Sqrt(hAsymm->GetEntries());
	printf("asymm[%i]=%0.5f%%  (+/- %0.4f%%):  %1.0f entries\n",
	       nCycles-1,asymm[nCycles-1],errorY[nCycles-1],
	       hAsymm->GetEntries());
	printf("Entry = %i\n", i);
	fprintf(pedAdjStats,"%d\t%12.5e\t%9.2e\t%d\t%12.5e\t%12.5e\t%9.2e"
		"\t%12.5e\t%9.2e\t%12.5e\t%9.2e\t%12.5e\t%9.2e\n",
		nCycles-1,asymm[nCycles-1], errorY[nCycles-1],
		(Int_t)hAsymm->GetEntries(),
		lasWiseYieldOn[nCycles-1]-lasWiseBkgdAvg[nCycles-1],
		lasWiseYieldOn[nCycles-1],lasWiseYieldOnErr[nCycles-1],
		lasWiseBkgdAvg[nCycles-1], lasWiseBkgdAvgErr[nCycles-1],
		lasWisePedShift[nCycles-1], lasWisePedShiftErr[nCycles-1],
		pedAdjBkgd[nCycles-1], pedAdjBkgdErr[nCycles-1]);
	hAsymm->Draw();
	numEnt+=(Int_t)hAsymm->GetEntries();
	c2->Print(Form("wwwdon/Run_%i/hist%i.png",runnum,nCycles-1));
	hAsymm->Reset();
	errorX[nCycles-1] = 0;
      }
      flipperIsUp = kTRUE;
    }
    if(i==nEntries-1)printf("nOffEvtsInRow = %i.\n",nOffEvtsInRow);
  }
  
  fclose(pedAdjStats);
  c1->cd(1);
  hAsymmetry->Draw();
  c1->cd(2);

  //draw graph of asymmetries
  TGraphErrors *gr = new TGraphErrors(nCycles,cutNum,asymm,errorX,errorY);
  gr->GetXaxis()->SetTitle("Cut Number");
  gr->GetYaxis()->SetTitle("Accumulator_0 Asymmetry (%)");
  gr->GetYaxis()->SetTitleOffset(1.4);
  gr->SetTitle("Laserwise Accumulator_0 Asymmetry vs. Cut Number (%)");
  gr->SetMarkerColor(kBlue);
  gr->SetMarkerStyle(21);
  gStyle->SetOptFit(1111);
  gr->Fit("pol0");
  gr->Draw("AP");
  c1->Print(canvas1);

  //activate branches again
  mpsTree->SetBranchStatus("*", 1);
  helTree->SetBranchStatus("*", 1);

  return 1;
}


