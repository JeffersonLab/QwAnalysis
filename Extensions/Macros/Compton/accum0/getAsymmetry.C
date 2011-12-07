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


/////////////////////////////////////////////////////////////////////////////
//Propagates error for background subtracted asymmetry                     //
//A=Yield*Ameas/(Yield-Background)                                         //
//variance = (eYield*dA/dYield)^2 + (eBkgd*dA/dBkgd)^2+ eAmeas*dA/dAmeas)^2//
/////////////////////////////////////////////////////////////////////////////
Double_t getError(Double_t Ameas, Double_t AmeasErr, 
		  Double_t yld, Double_t yldErr,
		  Double_t bkd, Double_t bkdErr, Double_t entries)
{
  Double_t error, variance, varAmeas, varYld, varBkd, delta;

  delta = yld - bkd; 
  varYld = TMath::Power(yldErr*bkd*Ameas/(delta*delta),2);
  varBkd = TMath::Power(bkdErr*Ameas*yld/(delta*delta),2);
  varAmeas = TMath::Power(AmeasErr*yld/delta,2);
  variance = varYld + varBkd + varAmeas;
  error = TMath::Sqrt(variance/entries);

  return error;
}



////////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton run laser wise and plots the asymmetry. //
//It also stores the results of each laser wise asymmetry and differences //
//in a file called "Run_#_pedsubasymm.txt".                               //
//If "clear" is nonzero, the canvases will be deleted at the end.         //
//The inputs are run number and either 0 or 1 for whether or not the file //
//is a first100K file.                                                    //
////////////////////////////////////////////////////////////////////////////

Int_t getAsymmetry(Int_t runnum, Int_t segnum=0){
  gROOT->Reset();
  Bool_t debug = kTRUE;
  
//create useful variables
  const Double_t SCA_MIN_CUR = 235;//bcm scaler rate for 0 current
  const Double_t SCALER_CUTOFF = 10;//HV considered off if scaler rate less
  const Int_t MIN_ENTRIES = 1000;//min # helTree entries to use a laserwise cycle
  const Int_t PATHLENGTH =255, NCUTS = 100;
  Bool_t local ,flipperIsUp = kFALSE;
  Bool_t laserOn, laserOff, beamOn, noPrevEvt, pulseIsCentered, sampleExists;
  char canvas1[PATHLENGTH], filename[PATHLENGTH];
  Int_t nOffEvtsInRow = 0, nCycles = -1, nEntries, nSkipped = 0;
  Int_t nGoodCycles = 0, nBetweenOn = 0;
  Int_t patNumberCut[NCUTS];
  Double_t patNumber, patNum = 0;
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
  lBinX = hTemp3->GetMean()*0.3;
  hBinX = hTemp3->GetMean()*2.0;
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
  mpsTree->SetBranchStatus("pattern_number", 1);//turn on branches wanted

  //define mps variables
  mpsTree->SetBranchAddress("fadc_compton_samples",&sample);//set addresses
  mpsTree->SetBranchAddress("sca_laser_PowT", &sca_laser_PowT);
  mpsTree->SetBranchAddress("bcm2_3h05a", &bcm2);
  mpsTree->SetBranchAddress("sca_photon_rate_gen",&scaler);
  mpsTree->SetBranchAddress("pattern_number",&patNumber);
  if(debug){
    printf("fadc_compton_samples status =%i.\n",
	   mpsTree->GetBranchStatus("fadc_compton_samples"));
    printf("sca_laser_PowT status =%i.\n",
	   mpsTree->GetBranchStatus("sca_laser_PowT"));
    printf("bcm2_3h05a status =%i.\n",
	   mpsTree->GetBranchStatus("bcm2_3h05a"));
    printf("fadc_compton_accum0 status =%i.\n",
	   mpsTree->GetBranchStatus("fadc_compton_accum0"));
    printf("sca_photon_rate_gen status =%i.\n",
	   mpsTree->GetBranchStatus("sca_photon_rate_gen"));
    printf("pattern_number status =%i.\n",
	   mpsTree->GetBranchStatus("pattern_number"));
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

    if(laserOff) nOffEvtsInRow++;
    if(!laserOn) nBetweenOn++;

    if(nOffEvtsInRow==1)patNum = patNumber;//keep track of where OFF starts

    //causes seg fault if try to Get*() and sample doesn't exist
    if(sampleExists)
    {
      pedestal = sample->at(0).GetPedestal();
      minSample = sample->at(0).GetMinSample();
      maxIndex = sample->at(0).GetMaxIndex(); 
      noPrevEvt = pedestal - minSample <10;
      beamOn = bcm2 > currentCutVal && scaler > SCALER_CUTOFF;//e-beam and HV on
      pulseIsCentered = maxIndex > 20 && maxIndex < 100;
    

      //start counting consecutive laserOff events if laser turns off.
      if(laserOff)
      {

	if(beamOn && pulseIsCentered && noPrevEvt)
	{
	  if(Accum0_hw_sum>lBinX && Accum0_hw_sum<hBinX)//cut outliers???good idea???
	  {
	    hPedOff->Fill(pedestal);
	  }
	  else printf("hw_sum = %f. Considered outlier and not included.\n",
		      Accum0_hw_sum);
	}

	if(beamOn)hBackground->Fill(Accum0_hw_sum);//fill histo with laser off yield

      }
      else if(laserOn)
      {
	if(flipperIsUp)//store OFF data at first sign of laser ON
        {
	  if(hPedOff->GetEntries()>MIN_ENTRIES)//only store if sufficient data
	  {
	    lasWisePedOff[nCycles] = hPedOff->GetMean(); 
	    lasWisePedOffErr[nCycles] = hPedOff->GetRMS()
	                                /TMath::Sqrt((Double_t)hPedOff->GetEntries());
	    lasWiseBkgd[nCycles] = hBackground->GetMean();
	    lasWiseBkgdErr[nCycles] = hBackground->GetRMS()
	                              /TMath::Sqrt((Double_t)hBackground->GetEntries());
	    if(nCycles==0)
	    {
	      printf("First Laser Off entry = %i\n",i-nBetweenOn);
	    }
	    printf("Laser OFF:  lasWisePedOff[%i]= %11.4e +/- %7.1e:  %d entries\n"
		   "              lasWiseBkgd[%i]= %11.4e +/- %7.1e:  %d entries\n"
		   "Begin Laser On: Entry = %i\n",
		   nCycles, lasWisePedOff[nCycles],lasWisePedOffErr[nCycles],
		   (Int_t)hPedOff->GetEntries(),
		   nCycles, lasWiseBkgd[nCycles],lasWiseBkgdErr[nCycles],
		   (Int_t)hBackground->GetEntries(),i);

	    patNumberCut[nCycles*2] = (Int_t)patNumber;//begin laser ON pat num
	  }
	  else
	  {
	    lasWisePedOff[nCycles] = -9999; 
	    lasWisePedOffErr[nCycles] = -9999;
	    lasWiseBkgd[nCycles] = -9999;
	    lasWiseBkgdErr[nCycles] = -9999;
	    patNumberCut[nCycles*2] = (Int_t)patNumber;//begin laser ON pat num
	    printf("Laser OFF:  lasWisePedOff[%i]= %1.0f. Insufficient OFF data:  %d entries\n"
		   "              lasWiseBkgd[%i]= %1.0f.                         %d entries\n"
		   "Begin Laser On: Entry = %i\n",
		   nCycles, lasWisePedOff[nCycles],(Int_t)hPedOff->GetEntries(),
		   nCycles, lasWiseBkgd[nCycles],(Int_t)hBackground->GetEntries(),i);
	  }
	  flipperIsUp = kFALSE;
	  hPedOff->Reset();
	  hBackground->Reset();
	}
	if(beamOn && pulseIsCentered && noPrevEvt)hPedOn->Fill(pedestal);

	if(beamOn)hYieldOn->Fill(Accum0_hw_sum);//fill histo with laser on yield

	nOffEvtsInRow = 0;
	nBetweenOn = 0;
      }
      else nOffEvtsInRow = 0;

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

	  patNumberCut[nCycles*2-1] = (Int_t)patNum;//end laser ON pat num

	}else if(nCycles !=0){
	  lasWisePedOn[nCycles-1] = hPedOn->GetMean(); 
	  lasWisePedOnErr[nCycles-1] = hPedOn->GetRMS()
	                        /TMath::Sqrt((Double_t)hPedOn->GetEntries());

	  printf("Laser ON:   lasWisePedOn[%i] = %1.0f. Insufficient ON data:%d entries\n"
		 "          lasWiseYieldOn[%i] = %1.0f.                     :%d entries\n" 
		 "Begin Laser Off: Entry = %i\n",
		 nCycles-1, lasWisePedOn[nCycles-1], (Int_t)hPedOn->GetEntries(),
		 nCycles-1, lasWiseYieldOn[nCycles-1], (Int_t)hYieldOn->GetEntries(),i); 

	  patNumberCut[nCycles*2-1] =(Int_t)patNum;//end laser ON pat num
	}
	hPedOn->Reset();//reset On histogram
	hYieldOn->Reset();//reset On histogram
	flipperIsUp = kTRUE;
      }
      nSkipped = 0;
    }//end of if(sampleExists){}

    else if(nOffEvtsInRow==MIN_ENTRIES*4)
    {
      nSkipped++;
      if(debug)printf("nSkipped= %i.\n",nSkipped);
    }
    //if last point store laser off data if more than MIN_ENTRIES samples available
    //allow for remote possibility that exactly at the end of the run the laser
    //is in between lock and unlock i.e. trying to lock 
    if(i==nEntries-1 && nOffEvtsInRow>=MIN_ENTRIES*4
       || i==nEntries-1 && (!laserOn && !laserOff)){
      if(hPedOff->GetEntries()>MIN_ENTRIES) //only store if sufficient data
      {
	lasWisePedOff[nCycles] = hPedOff->GetMean(); 
	lasWisePedOffErr[nCycles] = hPedOff->GetRMS()
	  /TMath::Sqrt((Double_t)hPedOff->GetEntries());
	lasWiseBkgd[nCycles] = hBackground->GetMean();
	lasWiseBkgdErr[nCycles] = hBackground->GetRMS()
	  /TMath::Sqrt((Double_t)hBackground->GetEntries());
	printf("Laser OFF:  lasWisePedOff[%i]= %11.4e +/- %7.1e:  %d entries\n"
	       "              lasWiseBkgd[%i]= %11.4e +/- %7.1e:  %d entries.\n"
	       "Final Laser Off: Entry = %i\n",
	       nCycles, lasWisePedOff[nCycles],lasWisePedOffErr[nCycles],
	       (Int_t)hPedOff->GetEntries(), nCycles, lasWiseBkgd[nCycles],
	       lasWiseBkgdErr[nCycles], (Int_t)hBackground->GetEntries(),i);
      }
      else
      {
	lasWisePedOff[nCycles] = -9999; 
	lasWisePedOffErr[nCycles] = -9999;
	lasWiseBkgd[nCycles] = -9999;
	lasWiseBkgdErr[nCycles] = -9999;
	printf("Laser OFF:  lasWisePedOff[%i]= %1.0f. Insufficient OFF data.\n"
	       "              lasWiseBkgd[%i]= %1.0f. Insufficient OFF data.\n"
	       "Final Laser Off: Entry = %i\n",
	       nCycles, lasWisePedOff[nCycles],nCycles, lasWiseBkgd[nCycles],i);
      }
    }
  }

  if(debug)
  {
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
    printf(" lasWisePedOff[%i]=%11.4e +/- %7.1e.\n",
	   nCycles, lasWisePedOff[nCycles],lasWisePedOffErr[nCycles]);
    printf("   lasWiseBkgd[%i]= %11.4e +/- %7.1e.\n",
	   nCycles, lasWiseBkgd[nCycles],lasWiseBkgdErr[nCycles]);
  }

  for(Int_t i=0;i<nCycles*2;i++)printf("patNumberCut[%i]=%i\n",i,patNumberCut[i]);

  for(Int_t i=0;i<nCycles;i++){
    Bool_t goodData = lasWisePedOff[i] > -9998 && lasWisePedOff[i+1] > -9998
                   && lasWisePedOn[i]  > -9998 && lasWiseBkgd[i]     > -9998 
                   && lasWiseBkgd[i+1] > -9998;
    if(goodData)
    {
      nGoodCycles++;
      lasWisePedShift[i] = lasWisePedOn[i] - 0.5*(lasWisePedOff[i] + lasWisePedOff[i+1]); 
      lasWisePedShiftErr[i] = lasWisePedOnErr[i] +
	                      0.5*(lasWisePedOffErr[i] + lasWisePedOffErr[i+1]);
      lasWiseBkgdAvg[i] = 0.5 * (lasWiseBkgd[i] + lasWiseBkgd[i+1]);
      lasWiseBkgdAvgErr[i] = 0.5 * (lasWiseBkgdErr[i] + lasWiseBkgdErr[i+1]);
      pedAdjBkgd[i] = lasWiseBkgdAvg[i] + lasWisePedShift[i] * Accum0_num_samples;
      pedAdjBkgdErr[i] = lasWiseBkgdAvgErr[i] + lasWisePedShiftErr[i];
    }
    else
    {
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
  Bool_t enoughData = kFALSE, inOnCycle;
  Double_t yield_sca_laser_PowT, yield_bcm2, yieldAccum0, asymmAccum0, yieldScaler;
  Double_t difference, asymmetry;
  Double_t cutNum[NCUTS], errorX[NCUTS], errorY[NCUTS], asymm[NCUTS], errorY2[NCUTS];

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
  TH1D *hAsymRaw = new TH1D("hAsmRaw","hAsmRaw", 150,-4,4);
  TH1F *hYield = new TH1F("hYield","hYield", 1000,lBinX,hBinX); 

  //create file for storing stats
  FILE *pedAdjStats = fopen(filename,"w");//file for storing stats.
  fprintf(pedAdjStats,"#CutNum\t Asymm\t\t AsymmRMS \tNumEnt\t"
	  " YieldOverBkgd \t Yield      \t YieldRMS \t"
	  " Background\t BkgdRMS \t PedestalShift\t PedShiftRMS\t "
	  "PedAdjBkgd\t PedAdjBkgdRMS\n");
 
  //create a canvas for plotting asymmetries
  TCanvas *c1 = new TCanvas("c1","c1",0,0,700,900);
  c1->Divide(1,3);
  TCanvas *c2 = new TCanvas("c2","c2",0,0,400,400);

  //turn off unwanted branches before looping through Hel_Tree
  helTree->SetBranchStatus("*",0);//turn off all branches
  helTree->SetBranchStatus("yield_sca_photon_rate_gen", 1);//turn on branch
  helTree->SetBranchStatus("yield_sca_laser_PowT", 1);//turn on branch
  helTree->SetBranchStatus("yield_bcm2_3h05a", 1);//turn on branch
  helTree->SetBranchStatus("yield_fadc_compton_accum0*", 1);//turn on branch
  helTree->SetBranchStatus("asym_fadc_compton_accum0*", 1);//turn on branch
  helTree->SetBranchStatus("pattern_number", 1);//turn on branch
  printf("pattern_number status=%i\n",helTree->GetBranchStatus("pattern_number"));
  //define helicity variables 
  helTree->SetBranchAddress("yield_sca_laser_PowT", &yield_sca_laser_PowT);
  helTree->SetBranchAddress("yield_bcm2_3h05a", &yield_bcm2);
  helTree->SetBranchAddress("yield_sca_photon_rate_gen", &yieldScaler);
  helTree->SetBranchAddress("pattern_number", &patNumber);

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
  nCycles = 0;//reset counter
  for(Int_t i=0;i<nEntries;i++)
  {
    helTree->GetEntry(i);//increment tree loop
    bYieldAccum0->GetEntry(i);
    bAsymmAccum0->GetEntry(i);
      
    //skip first 500 in each ON cycle when lock unstable and
    //last 10 in case pattern_number off between trees
    inOnCycle = (patNumber > patNumberCut[nCycles*2]+ 500)
             && (patNumber < patNumberCut[nCycles*2+1] - 10);
    if(inOnCycle)
    {
      //define conditions
      laserOn = yield_sca_laser_PowT>cutLower && yield_sca_laser_PowT<cutUpper;
      laserOff = yield_sca_laser_PowT <= 3.0;
      beamOn = yield_bcm2 > currentCutVal && yieldScaler > SCALER_CUTOFF;
      yieldAccum0 = lYieldAccum0_hw_sum->GetValue();
      asymmAccum0 = lAsymm_hw_sum->GetValue();
      enoughData = pedAdjBkgd[nCycles] > -9998;
     
      //calculate asymmetry
      difference = yieldAccum0*asymmAccum0;
      asymmetry = difference/(yieldAccum0-pedAdjBkgd[nCycles]);
      if(laserOn && enoughData && beamOn && TMath::Abs(asymmetry)<4)
      {
	hAsymm->Fill(asymmetry);
	hAsymmetry->Fill(asymmetry);
	hAsymRaw->Fill(asymmAccum0);
	hYield->Fill(yieldAccum0);
      }	
    }
    else if(patNumber == patNumberCut[nCycles*2+1])
    {
      printf("patNumber=%i, enoughData=%i, nCycles=%i\n",
	     (Int_t)patNumber, enoughData, nCycles);
      if(enoughData)
      {
	Double_t a,b,c,d,e,f,g;
	asymm[nCycles] = 100 * hAsymm->GetMean();
	errorY[nCycles] = 100 * hAsymm->GetRMS()/TMath::Sqrt(hAsymm->GetEntries());
	a = hAsymRaw->GetMean();
	b = hAsymRaw->GetRMS();
	c = hYield->GetMean();
	d = hYield->GetRMS();;
	e = lasWiseBkgdAvg[nCycles];
	f = lasWiseBkgdAvgErr[nCycles];
	g = hAsymRaw->GetEntries();
	printf("Errors:hAsymRaw=%9.2e, hYield=%9.2e, lWbkgd=%9.2e\n",b, d, f);
	errorY2[nCycles] = 100 * getError(a, b, c, d, e, f, g);
	errorX[nCycles] = 0;
	cutNum[nCycles] = nCycles;
	fprintf(pedAdjStats,"%d\t%12.5e\t%9.2e\t%d\t%12.5e\t%12.5e\t%9.2e"
		"\t%12.5e\t%9.2e\t%12.5e\t%9.2e\t%12.5e\t%9.2e\n",
		nCycles,asymm[nCycles], errorY[nCycles],
		(Int_t)hAsymm->GetEntries(),
		lasWiseYieldOn[nCycles]-lasWiseBkgdAvg[nCycles],
		lasWiseYieldOn[nCycles],lasWiseYieldOnErr[nCycles],
		lasWiseBkgdAvg[nCycles], lasWiseBkgdAvgErr[nCycles],
		lasWisePedShift[nCycles], lasWisePedShiftErr[nCycles],
		pedAdjBkgd[nCycles], pedAdjBkgdErr[nCycles]);  
      }
      hAsymm->Draw();
      c2->Print(Form("wwwdon/Run_%i/hist%i.png",runnum,nCycles));
      hAsymRaw->Reset();
      hAsymm->Reset();
      hYield->Reset();
      nCycles++;
    }
  }//end of for loop
  fclose(pedAdjStats);
  c1->cd(1);
  hAsymmetry->Draw();
  c1->cd(2);

  //draw graph of asymmetries
  TGraphErrors *gr = new TGraphErrors(nGoodCycles,cutNum,asymm,errorX,errorY);
  gr->GetXaxis()->SetTitle("Cut Number");
  gr->GetYaxis()->SetTitle("Accumulator_0 Asymmetry (%)");
  gr->GetYaxis()->SetTitleOffset(1.4);
  gr->SetTitle("Laserwise Accumulator_0 Asymmetry vs. Cut Number (%)");
  gr->SetMarkerColor(kBlue);
  gr->SetMarkerStyle(21);
  gStyle->SetOptFit(1111);
  gr->Fit("pol0");
  gr->Draw("AP");
  c1->cd(3);
  TGraphErrors *gr1 = new TGraphErrors(nGoodCycles,cutNum,asymm,errorX,errorY2);
  gr1->GetXaxis()->SetTitle("Cut Number");
  gr1->GetYaxis()->SetTitle("Accumulator_0 Asymmetry (%)");
  gr1->GetYaxis()->SetTitleOffset(1.4);
  gr1->SetTitle("LasWise Accum0 Asymmetry  vs. Cut# (alternate error propagation)");
  gr1->SetMarkerColor(kBlue);
  gr1->SetMarkerStyle(21);
  gStyle->SetOptFit(1111);
  gr1->Fit("pol0");
  gr1->Draw("AP");
  c1->Print(canvas1);

  //activate branches again
  mpsTree->SetBranchStatus("*", 1);
  helTree->SetBranchStatus("*", 1);

  return 1;
}



