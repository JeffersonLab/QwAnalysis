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
  const Int_t MIN_ENTRIES = 3000;//min # of entries to use a laserwise cycle
  const Int_t PATHLENGTH =255, NCUTS = 100;
  Bool_t local ,flipperIsUp = kFALSE;
  Bool_t laserOn, laserOff, beamOn, noPrevEvt, pulseIsCentered, sampleExists;
  char canvas1[PATHLENGTH], filename[PATHLENGTH];
  Int_t nOffEvtsInRow = 0, nPts = -1, nTree = 0, nEntries, nSkipped = 0;
  Double_t sca_laser_PowT, bcm2, currentCutVal, Accum0_hw_sum, scaler;
  Double_t cutLower, cutUpper, hBinX, lBinX, Accum0_num_samples=0;
  Double_t pedestal = 0, minSample = 0, maxIndex = 0;
  Double_t lasWisePed[NCUTS], lasWisePedErr[NCUTS];
  Double_t lasWisePedShift[NCUTS], lasWisePedShiftErr[NCUTS];
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
  TH1F *hBackground = new TH1F("hBackground","Background", 1000,lBinX,hBinX); 


  //turn off all branches that aren't needed before looping
  mpsTree->SetBranchStatus("*",0);//turn off all branches
  mpsTree->SetBranchStatus ("fadc_compton_samples*",1);//turn on desired branches
  mpsTree->SetBranchStatus("sca_laser_PowT",1);//turn on desired branches
  mpsTree->SetBranchStatus("bcm2_3h05a", 1);//turn on desired branches
  mpsTree->SetBranchStatus("fadc_compton_accum0*", 1);//turn on desired branches
  mpsTree->SetBranchStatus("sca_photon_rate_gen", 1);//turn on branches wanted

  //define mps variables
  mpsTree->SetBranchAddress("fadc_compton_samples",&sample);//set addresses
  mpsTree->SetBranchAddress("sca_laser_PowT", &sca_laser_PowT);
  mpsTree->SetBranchAddress("bcm2_3h05a", &bcm2);
  mpsTree->SetBranchAddress("sca_photon_rate_gen",&scaler);
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

  nPts=-1;
  for(Int_t i=0;i<nEntries;i++){
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
    if(laserOff)nOffEvtsInRow++;
    //cause seg fault if try to Get*() and sample doesn't exist
    if(sampleExists){
      pedestal = sample->at(0).GetPedestal();
      minSample = sample->at(0).GetMinSample();
      maxIndex = sample->at(0).GetMaxIndex(); 
      noPrevEvt = pedestal - minSample <10;
      beamOn = bcm2 > currentCutVal && scaler > 3;//e-beam and HV on
      pulseIsCentered = maxIndex > 20 && maxIndex < 100;
    

      //start counting consecutive laserOff events if laser turns off.
      if(laserOff){
	//	nOffEvtsInRow++;
	if(beamOn && pulseIsCentered && noPrevEvt){
	  if(Accum0_hw_sum>lBinX && Accum0_hw_sum<hBinX){//cut outliers???good idea???
	    hPedOff->Fill(pedestal);
	    hBackground->Fill(Accum0_hw_sum);
	  }else printf("hw_sum = %f. Considered outlier and not included.\n",
		      Accum0_hw_sum);
	}
      }else if(laserOn){
	nOffEvtsInRow = 0;
	if(flipperIsUp){
	  if(hBackground->GetEntries()>MIN_ENTRIES){//only store if sufficient data
	    lasWisePed[nPts*2] = hPedOff->GetMean(); 
	    lasWisePedErr[nPts*2] = hPedOff->GetRMS()
	      /TMath::Sqrt((Double_t)hPedOff->GetEntries());
	    lasWiseBkgd[nPts] = hBackground->GetMean();
	    lasWiseBkgdErr[nPts] = hBackground->GetRMS()
	      /TMath::Sqrt((Double_t)hBackground->GetEntries());
	    printf("Laser OFF:  lasWisePed[%i]= %11.4e +/- %7.1e. %f entries\n"
		   "           lasWiseBkgd[%i]= %11.4e +/- %7.1e. %f entries\n"
		   "Begin Laser On: Entry = %i\n",
		   nPts*2, lasWisePed[nPts*2],lasWisePedErr[nPts*2],
		   hPedOff->GetEntries(),
		   nPts, lasWiseBkgd[nPts],lasWiseBkgdErr[nPts],
		   hBackground->GetEntries(),i);
	  }else{
	    lasWisePed[nPts*2] = -9999; 
	    lasWisePedErr[nPts*2] = -9999;
	    lasWiseBkgd[nPts] = -9999;
	    lasWiseBkgdErr[nPts] = -9999;
	    printf("Laser OFF:  lasWisePed[%i]= %1.0f. Insufficient data. %f entries\n"
		   "           lasWiseBkgd[%i]= %1.0f. Insufficient data. %f entries\n"
		   "Begin Laser On: Entry = %i\n",
		   nPts*2, lasWisePed[nPts*2],hPedOff->GetEntries(),
		   nPts, lasWiseBkgd[nPts],hBackground->GetEntries(),i);
	  }
	  flipperIsUp = kFALSE;
	  hPedOff->Reset();
	  hBackground->Reset();
	}
	if(beamOn && pulseIsCentered && noPrevEvt){
	  hPedOn->Fill(pedestal);
	}

      }else nOffEvtsInRow = 0;

      if(nOffEvtsInRow==MIN_ENTRIES*4 || nSkipped!=0){
	nPts++;
	if(nPts!=0 && hPedOn->GetEntries()>MIN_ENTRIES){
	  lasWisePed[nPts*2-1] = hPedOn->GetMean(); 
	  lasWisePedErr[nPts*2-1] = hPedOn->GetRMS()
	                        /TMath::Sqrt((Double_t)hPedOn->GetEntries());
	  printf("Laser ON:   lasWisePed[%i]= %11.4e +/- %7.1e.\n"
		 "Begin Laser Off: Entry = %i\n",
		 nPts*2-1, lasWisePed[nPts*2-1], lasWisePedErr[nPts*2-1],
		 i-MIN_ENTRIES*4); 
	}else if(nPts !=0){
	  lasWisePed[nPts*2-1] = hPedOn->GetMean(); 
	  lasWisePedErr[nPts*2-1] = hPedOn->GetRMS()
	                        /TMath::Sqrt((Double_t)hPedOn->GetEntries());
	  printf("Laser ON:   lasWisePed[%i]= %1.0f. Insufficient ON data.\n"
		 "Begin Laser Off: Entry = %i\n",
		 nPts*2-1, lasWisePed[nPts*2-1],i); 
	}
	hPedOn->Reset();//reset On histogram
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
	lasWisePed[nPts*2] = hPedOff->GetMean(); 
	lasWisePedErr[nPts*2] = hPedOff->GetRMS()
	  /TMath::Sqrt((Double_t)hPedOff->GetEntries());
	lasWiseBkgd[nPts] = hBackground->GetMean();
	lasWiseBkgdErr[nPts] = hBackground->GetRMS()
	  /TMath::Sqrt((Double_t)hBackground->GetEntries());
	printf("Laser OFF:  lasWisePed[%i]= %11.4e +/- %7.1e.\n"
	       "           lasWiseBkgd[%i]= %11.4e +/- %7.1e.\n"
	       "Final Laser Off: Entry = %i\n",
	       nPts*2, lasWisePed[nPts*2],lasWisePedErr[nPts*2],
	       nPts, lasWiseBkgd[nPts],lasWiseBkgdErr[nPts],i);
      }else{
	lasWisePed[nPts*2] = -9999; 
	lasWisePedErr[nPts*2] = -9999;
	lasWiseBkgd[nPts] = -9999;
	lasWiseBkgdErr[nPts] = -9999;
	printf("Laser OFF:  lasWisePed[%i]= %1.0f. Insufficient OFF data.\n"
	       "           lasWiseBkgd[%i]= %1.0f. Insufficient OFF data.\n"
	       "Final Laser Off: Entry = %i\n",
	       nPts*2, lasWisePed[nPts*2],nPts, lasWiseBkgd[nPts],i);
      }
    }
  }
  if(debug){
    printf("Check this list against the previous for agreement.\n");
    for(Int_t i=0;i<nPts*2+1;i++){
      printf("lasWisePed[%i]=%11.4e +/- %7.1e.      ",
	     i, lasWisePed[i],lasWisePedErr[i]);
      if(i<=nPts){
	printf("\tlasWiseBkgd[%i]= %11.4e +/- %7.1e.\n",
	       i, lasWiseBkgd[i],lasWiseBkgdErr[i]);
      }else printf("\n");
    }
  }

  for(Int_t i=0;i<nPts;i++){
    if(lasWisePed[i*2]!=-9999 && lasWisePed[i*2+1]!=-9999 && lasWisePed[i*2+2]!=-9999
       && lasWiseBkgd[i]!=-9999 && lasWiseBkgd[i+1]!=-9999){
      lasWisePedShift[i] = lasWisePed[i*2+1]
				      -(lasWisePed[i*2] + lasWisePed[i*2+2])/2.0; //no error weight
      lasWisePedShiftErr[i] = TMath::Abs(lasWisePedErr[i*2+1]
					 -(lasWisePedErr[i*2] + lasWisePedErr[i*2+2]))/2.0;
      lasWiseBkgdAvg[i] = (lasWiseBkgd[i] + lasWiseBkgd[i+1])/2.0;
      lasWiseBkgdAvgErr[i] = (lasWiseBkgdErr[i] + lasWiseBkgdErr[i+1])/2.0;
      pedAdjBkgd[i] = lasWiseBkgdAvg[i] + lasWisePedShift[i] * Accum0_num_samples;
      pedAdjBkgdErr[i] = lasWiseBkgdAvgErr[i] + lasWisePedShiftErr[i];
    }else{
      lasWisePedShift[i] = -9999;
      lasWisePedShiftErr[i] = -9999;
      lasWiseBkgdAvg[i] = -9999;
      pedAdjBkgd[i]= -9999;
      pedAdjBkgdErr[i] = -9999;
    }
    printf("lasWisePedShift[%i] = %11.4e +/- %7.1e.\n"
	   " lasWiseBkgdAvg[%i] = %11.4e +/- %7.1e.\n"
	   "     pedAdjBkgd[%i] = %11.4e +/- %7.1e.\n",
	   i, lasWisePedShift[i], lasWisePedShiftErr[i],
	   i, lasWiseBkgdAvg[i], lasWiseBkgdAvgErr[i],
	   i, pedAdjBkgd[i], pedAdjBkgdErr[i]);

  }



  ////////////////////////////////////////////////////////////////////////////////////
  //Loop through Hel_Tree to get the pedestal and background subtracted asymmetries.//
  ////////////////////////////////////////////////////////////////////////////////////


  //Redefine variables in terms of helTree.
  Bool_t enoughData = kFALSE;
  Int_t prevnPts = 0, arrayLength = nPts, numEnt = 0;
  Double_t yield_sca_laser_PowT, yield_bcm2, yieldAccum0, asymmAccum0, yieldScaler;
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
  TH1F *hAsymmetry = new TH1F("hAsymmetry","Asymmetry", 150,-3,3);
  TH1F *hAsymm = new TH1F("hAsymm","hAsymm", 150,-3,3);

  //create file for storing stats
  FILE *pedAdjStats = fopen(filename,"w");//file for storing stats.
  fprintf(pedAdjStats,"#CutNum\t Asymm(%%)\t AsymmRMS(%%) \n"); 

  //create a canvas for plotting asymmetries
  TCanvas *c1 = new TCanvas("c1","c1",0,0,700,700);
  c1->Divide(1,2);
  TCanvas *c2 = new TCanvas("c2","c2",0,0,400,400);

  //turn off unwanted branches before looping through Hel_Tree
  helTree->SetBranchStatus("*",0);//turn off all branches
  helTree->SetBranchStatus("yield_sca_photon_rate_gen", 1);//turn on branches wanted
  helTree->SetBranchStatus("yield_sca_laser_PowT", 1);//turn on branches wanted
  helTree->SetBranchStatus("yield_bcm2_3h05a", 1);//turn on branches wanted
  helTree->SetBranchStatus("yield_fadc_compton_accum0*", 1);//turn on branches wanted
  helTree->SetBranchStatus("asym_fadc_compton_accum0*", 1);//turn on branches wanted

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


  //Now loop over helTree and form asymmetries
  nTree = 0;
  nPts = -1;//reset counter
  for(Int_t i=0;i<nEntries;i++){

    helTree->GetEntry(i);

    laserOn = yield_sca_laser_PowT > cutLower && yield_sca_laser_PowT < cutUpper;
    laserOff = yield_sca_laser_PowT <= 2.0;
    beamOn = yield_bcm2 > currentCutVal && yieldScaler > 3.0;
    yieldAccum0 = lYieldAccum0_hw_sum->GetValue();
    asymmAccum0 = lAsymm_hw_sum->GetValue();
    enoughData = pedAdjBkgd[nPts] > -9998;
    //start counting consecutive laserOff events if laser turns off.
    if(laserOff){
      nOffEvtsInRow++;
    }else if(laserOn){
      if(flipperIsUp){
	flipperIsUp = kFALSE;
      }

      //terminate loop if at end of pedAdjBkgd array.
      if(nPts == arrayLength+1){
	i = nEntries;
      }else if(beamOn && nPts>=0){
	if(prevnPts==nPts){
	  printf("Now using pedAdjBkgd[%i]=%f\n",nPts,pedAdjBkgd[nPts]);
	  prevnPts = nPts + 1;
	}
	if(pedAdjBkgd[nPts]!=0){
	asymmetry = yieldAccum0*asymmAccum0/(yieldAccum0-pedAdjBkgd[nPts]);
	hAsymmetry->Fill(asymmetry);
	hAsymm->Fill(asymmetry);
	nOffEvtsInRow = 0;
	}
      }else nOffEvtsInRow = 0;
    }
    if(nOffEvtsInRow==MIN_ENTRIES ){
      nPts++;
      if(nPts>0 && enoughData){
	asymm[nPts-1] = 100*hAsymm->GetMean();
	cutNum[nPts-1] = nPts-1;
	errorY[nPts-1] = 100*hAsymm->GetRMS()/TMath::Sqrt(hAsymm->GetEntries());
	printf("asymm[%i]=%0.5f%%  (+/- %0.4f%%).\n",
	       nPts-1,asymm[nPts-1],errorY[nPts-1]);
	fprintf(pedAdjStats,"%d\t%12.5e\t%9.2e\n",
		nPts-1,asymm[nPts-1],errorY[nPts-1]);
	hAsymm->Draw();
	numEnt+=(Int_t)hAsymm->GetEntries();
	c2->Print(Form("wwwdon/Run_%i/hist%i.png",runnum,nPts-1));
	hAsymm->Reset();
	errorX[nPts-1] = 0;
      }
      flipperIsUp = kTRUE;
    }
  }

  fclose(pedAdjStats);
  c1->cd(1);
  hAsymmetry->Draw();
  c1->cd(2);

  //draw graph of asymmetries
  TGraphErrors *gr = new TGraphErrors(nPts,cutNum,asymm,errorX,errorY);
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

