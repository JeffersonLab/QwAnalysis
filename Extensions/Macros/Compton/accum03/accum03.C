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
#include <stdio.h>
#include <stdlib.h>

using namespace std;


////////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton run laser wise and plots the results.   //
//It also stores the results of each laser wise asymmetry and differences //
//in a file called "polstats.txt".                                        //
//If "clear" is nonzero, the canvases will be deleted at the end.         //
//The inputs are run number and either 0 or 1 for whether or not the file //
//is a first100K file. The last argument is also either 
//instructs the program whether or not to delete everything upon closing.//
////////////////////////////////////////////////////////////////////////////

Int_t accum03(Int_t runnum, Bool_t isFirst100K=kFALSE, Bool_t deleteOnExit=kFALSE){
  gROOT->Reset();
  //  gStyle->SetCanvasColor(0);
  // Create a chain
  const Double_t LOW_LSR_LMT = 20.0;//laser considered unlocked below this
  TChain *helChain = new TChain("Hel_Tree");
  TChain *mpsChain = new TChain("Mps_Tree");
  Int_t chainExists = 0;
  // const Double_t MAXCUR = 140, MINSCALER = 235, MAXSCALER = 468;//bcm2_3h05a  values
  const Double_t MAXCUR = 60, MINSCALER = 235, MAXSCALER = 292;//bcm6_3c17 values
  // Open either Pass1 or the First 100K
  if( isFirst100K ) {
    mpsChain->Add(Form("%s/first100k_%d.root",
		       getenv("QW_ROOTFILES"),runnum));
    chainExists = helChain->Add(Form("%s/first100k_%d.root",
				     getenv("QW_ROOTFILES"),runnum));
  } else {
    mpsChain->Add(Form("%s/Compton_Pass1_%d.*.root",
		       getenv("QW_ROOTFILES"),runnum));
    chainExists = helChain->Add(Form("%s/Compton_Pass1_%d.*.root",
				     getenv("QW_ROOTFILES"),runnum));
  }
  Bool_t local = kFALSE;
  if(chainExists == 0){//check local folder in case running locally
    mpsChain->Add(Form("Compton_Pass1_%d.*.root",runnum));
    chainExists = helChain->Add(Form("Compton_Pass1_%d.*.root",runnum));

    local = kTRUE;
  }
  if(chainExists == 0){//delete chains and exit if files do not exist
    cout<<"File does not exist."<<endl;
    helChain->TChain::~TChain();
    mpsChain->TChain::~TChain();
    return 0;
  }
  printf("%d files attached to chain.\n",chainExists);

  const Int_t MIN_ENTRIES = 2000;//min # of entries to use a laserwise cycle
  const Int_t NCUTS = 500;//initialized size of TCut arrays.
  const Double_t LSCALE = 0.7;//used to scale laser power on yield graph.
  const Int_t PATHLENGTH = 255;
  const Double_t BCMGAIN = MAXCUR/(MAXSCALER-MINSCALER);
  char canvas1[PATHLENGTH], canvas2[PATHLENGTH], canvas3[PATHLENGTH];
  char filename[PATHLENGTH];
  if(local == kFALSE){

    gSystem->mkdir(Form("www/run_%d",runnum),true);
    sprintf(canvas1,"www/run_%d/accum03_%d_plots.png",runnum,runnum);
    sprintf(canvas2,"www/run_%d/accum03_%d_asymm.png",runnum,runnum);
    sprintf(canvas3,"www/run_%d/accum03_%d_yieldvspatnum.png",runnum,runnum);
    sprintf(filename,"www/run_%d/accum03_%d_stats.txt",runnum,runnum);
  }else{
   gSystem->mkdir(Form("photonsummarydon/Run_%d",runnum),true);
   sprintf(canvas1,"photonsummary/Run_%d_plots.png",runnum);
   sprintf(canvas2,"photonsummary/Run_%d_asymm.png",runnum);
   sprintf(canvas3,"photonsummary/Run_%d_yieldvspatnum.png",runnum);
   sprintf(filename,"photonsummary/Run_%d_stats.txt",runnum);
  }
  FILE *polstats = fopen(filename,"w");//file for storing stats.
     
  //  TTree *myTree = (TTree*)file->Get("Hel_Tree");

  //////////////////////////////////
  //***Define the desired cuts.***//
  //////////////////////////////////
  std::vector<Int_t> cut;
  Int_t nCuts = cutOnLaser(cut, helChain);
  helChain->ResetBranchAddresses();//resets branch address
  printf("nCuts=%d\n",nCuts); 
  if(nCuts>2*NCUTS){//Stop execution if # of cuts exceeds initialized array.
    printf("Number of cuts exceeds program limits.");
    printf("Need to increase NCUTS value in accum03.C.\n");
    cut.vector::~vector();
    return 0;
  }
  Double_t hBinX = helChain->GetMaximum("yield_sca_laser_PowT");
  Double_t lBinX = helChain->GetMinimum("yield_sca_laser_PowT");
  Double_t diffX = hBinX - lBinX;
  //  TH1D *hTemp = new TH1D("hTemp","hTemp",1000,lBinX,hBinX);
  helChain->Draw(Form("yield_sca_laser_PowT>>hTemp(100,%f,%f)",lBinX,hBinX),
		 "yield_sca_laser_PowT>10","goff");
  TH1D *hTemp = (TH1D*)gDirectory->Get("hTemp"); 
  Double_t cutval1 = hTemp->GetMean()-10;
  Double_t cutval2 = hTemp->GetMean()*2.0;
  Double_t bkg[NCUTS], bkgWidth[NCUTS];
  cout<<cutval1<<"\t"<<cutval2<<"\t"<<hTemp->GetMean()<<endl;
  hTemp->TH1D::~TH1D();
  Double_t maxCur = helChain->GetMaximum("yield_bcm6_3c17");
  cout<<"maxCur="<<maxCur<<endl;
  Double_t cutCur = maxCur * 0.8 + 0.2 * MINSCALER;//subtract scaler pedestal of 235Hz
  TCut bcmCut = Form("yield_bcm6_3c17>%f",cutCur);
  TCut beamOffCut = Form("yield_bcm6_3c17<=%f", MINSCALER + 2);
  TCut scalerCut = "yield_sca_laser_photon> 2";//PMT On and beam hitting laser
  TCut powCut = bcmCut && scalerCut;//PMT On and hitting laser and beam On
  printf("BCM current cut is %f.\n",cutCur);
  TCut cut1, cut2, cut3, cut4;
  TCut lasOnCut = powCut && Form("yield_sca_laser_PowT>%f",cutval1)
                         && Form("yield_sca_laser_PowT<%f",cutval2);
  TCut lasOnCut1 = Form("yield_sca_laser_PowT>%f",cutval1);
  TCut lasOnCut2 = Form("yield_sca_laser_PowT<%f",cutval2);
  TCut unusedCut = powCut && Form("yield_sca_laser_PowT<=%f",cutval1)
                          && "yield_sca_laser_PowT>1.0" ||
		   powCut && Form("yield_sca_laser_PowT>=%f",cutval2); 
  TCut lasOffCut = powCut && Form("yield_sca_laser_PowT<=%f",LOW_LSR_LMT);//all unlocked times
  TCut lasOffCut2 = Form("yield_sca_laser_PowT<=%f",LOW_LSR_LMT);//all unlocked times
  TCut lasOFFCut = "";//only laser OFF periods i.e. when flipper is up
  TCut lasWiseCut[NCUTS];
  TCut bkgCut[NCUTS];
  TCut asymCut[NCUTS];

  for(Int_t i=0; i<nCuts;i++)//print out the cuts to make sure this program
    printf("cut.at[%d]=%d\n",i, cut.at(i));//has access to them.

  for(Int_t i=0; i*2<nCuts-2; i++){
    cut1 = (Form("Entry$>=%d",cut.at(2*i)));
    cut2 = (Form("Entry$<=%d",cut.at(2*i+1)));
    cut3 = (Form("Entry$>=%d",cut.at(2*i+2)));
    cut4 = (Form("Entry$<=%d",cut.at(2*i+3))); 
    lasWiseCut[i] = cut1 && cut4;
    bkgCut[i] = lasOffCut && lasWiseCut[i];
    asymCut[i] = lasOnCut && lasWiseCut[i];
    helChain->Draw("yield_fadc_compton_accum0.hw_sum-yield_fadc_compton_accum3.hw_sum >>h(100)",bkgCut[i],"goff");
    TH1F *h = (TH1F*)gDirectory->Get("h");
    bkg[i] = h->GetMean();
    bkgWidth[i] = h->GetRMS();
    cout<<"bkg["<<i<<"]="<<bkg[i]<<endl;
    if (i==0)
      lasOFFCut = (cut1 && cut2) || (cut3 && cut4);
    else lasOFFCut = lasOFFCut ||(cut3 && cut4);
    h->TH1F::~TH1F();
  }
  lasOFFCut = lasOFFCut && powCut;


  ////////////////////////////////////
  //***Create a canvas for plots.***//
  ////////////////////////////////////
  TCanvas *c1 = new TCanvas("c1","c1",0,0,900,820);
  c1->SetFillColor(0);
  c1->Divide(2,3);
  TCanvas *c3 = new TCanvas("c3","c3",0,0,900,820);
  c3->Divide(1,3);
  c1->cd(1);

  ////////////////////////////////////////
  //***Draw Current vs Pattern Number***//
  ////////////////////////////////////////
  hBinX = helChain->GetMaximum("pattern_number");
  lBinX = helChain->GetMinimum("pattern_number");
  diffX = hBinX - lBinX;
  hBinX += diffX*0.1;
  lBinX -= diffX*0.1;
  Double_t hBinY ,lBinY, diffY;
  helChain->Draw(Form("(yield_bcm6_3c17-%f)*%f:pattern_number>>hbcm(200,,,200,0.0,180.0)",
		      MINSCALER,BCMGAIN),"","goff");
  gPad->Update();
  TH2F *hbcm = (TH2F*)gDirectory->Get("hbcm");
  hbcm->GetXaxis()->SetTitle("Pattern Number");
  hbcm->GetYaxis()->SetTitle("BCM Yield");
  hbcm->SetMarkerColor(kRed);
  hbcm->SetTitle("BCM Yield vs. Pattern Number");
  cout<<"BCMGAIN="<<BCMGAIN<<endl;
  hbcm->Draw();
  if(hbcm->GetEntries()==0){
    printf("\nNo useful events in Run %d.\n\n",runnum); 
    hbcm->TH2F::~TH2F();
    c1->TCanvas::Destructor();
    cut.clear();
    return 0;
  }
 
  ////////////////////////////////////////////////////////
  //***Draw accumulator 0 yield with different colors***//
  //***for laser On and Laser Off states.            ***//
  ////////////////////////////////////////////////////////
  c1->cd(2);
  helChain->Draw("yield_fadc_compton_accum0.hw_sum-yield_fadc_compton_accum3.hw_sum >>h(100)","","goff");
  TH1F *h = (TH1F*)gDirectory->Get("h");
  if(h->GetEntries()==0){
    printf("\n\nNo useful events in Run %d.\n\n\n",runnum);
    hbcm->TH2F::~TH2F();
    h->TH1F::~TH1F();
    c1->TCanvas::Destructor();
    cut.clear();
    cut.clear();
    return 0;
  }

  hBinY = getRightmost(h);
  lBinY = getLeftmost(h);
  diffY = hBinY - lBinY;
  hBinY += diffY*0.1;
  lBinY -= diffY*0.1;
  Double_t hBinYtmp = h->GetMean()+3*h->GetRMS();
  if(hBinY>hBinYtmp){
    hBinY = hBinYtmp;
  }
  Double_t lBinYtmp = h->GetMean()-3*h->GetRMS();
  if(lBinY<lBinYtmp){
    lBinY = lBinYtmp;
  }
  h->TH1F::~TH1F();
  TH1F *hYieldOn = new TH1F("hYieldOn", "hYieldOn", 300, lBinY, hBinY);
  TH1F *hYieldOff = new TH1F("hYieldOff", "hYieldOff", 300, lBinY, hBinY);
  helChain->Draw("yield_fadc_compton_accum0.hw_sum-yield_fadc_compton_accum3.hw_sum >>hYieldOn",lasOnCut,"goff");
  hYieldOn->SetLineColor(kGreen);
  hYieldOn->GetXaxis()->SetTitle("Accumulator_0-3 Yields");
  hYieldOff->GetXaxis()->SetTitle("Accumulator_0-3 Yields");
  hYieldOn->SetTitle("Accumulator_0-3 Yields");
  hYieldOff->SetTitle("Accumulator_0-3 Yields");
  helChain->Draw("yield_fadc_compton_accum0.hw_sum-yield_fadc_compton_accum3.hw_sum >>hYieldOff",
		 lasOffCut,"goff");
  hYieldOff->SetLineColor(kBlue);
  if(hYieldOn->GetMaximum()>hYieldOff->GetMaximum()){
    gStyle->SetOptStat(1111);
    hYieldOn->Draw();
    gPad->Update();
    TPaveStats *st = (TPaveStats*)gPad->GetPrimitive("stats");
    st->SetY1NDC(0.67); //new y start position
    st->SetY2NDC(0.84); //new y end position
    hYieldOff->Draw("sames");
  }else{
    gStyle->SetOptStat(1111);
    hYieldOff->Draw();
    gPad->Update();
    TPaveStats *st = (TPaveStats*)gPad->GetPrimitive("stats");
    st->SetY1NDC(0.67); //new y start position
    st->SetY2NDC(0.84); //new y end position
    hYieldOn->Draw("sames");
  }
  TLegend *legend1 = new TLegend(0.86,0.55,0.98,0.67);
  //legend1->SetHeader("Some histograms");
  legend1->AddEntry(hYieldOn,"Laser On","l");
  legend1->AddEntry(hYieldOff,"Laser Off","l");
  legend1->SetFillColor(0);
  legend1->Draw();

  
  /////////////////////////////////////////////////////////
  //***Draw accumulator 0 yield vs pattern number with***//
  //***different colors for laser On and Off states.  ***//
  //***Superimpose scaled laser power on plot as well.***//
  ////////////////////////////////////////////////////////
  Float_t maxPowT = helChain->GetMaximum("yield_sca_laser_PowT");
  //cout<<lBinX<<"\t"<<hBinX<<"\t"<<lBinY<<"\t"<<hBinY<<endl;
  diffY = hBinY - lBinY;
  hBinY += diffY*0.1;
  lBinY -= diffY*0.1;
  //  cout<<lBinX<<"\t"<<hBinX<<"\t"<<lBinY<<"\t"<<hBinY<<endl;
  Float_t scale = LSCALE*(Float_t)(hBinY-lBinY)/(Float_t)maxPowT;
  // printf("maxPowT=%f\nscale=%f\n",maxPowT,scale);
  c1->cd(3);
  helChain->Draw(Form("yield_sca_laser_PowT*%f+%f:pattern_number >> hPow(%d,%f,%f,%d,%f,%f)",
		    scale,lBinY+diffY*0.05,2000,lBinX,hBinX,2000,lBinY,hBinY),
                    lasOnCut,"goff");
  helChain->Draw(Form("yield_sca_laser_PowT*%f+%f:pattern_number >> hPUnsd(%d,%f,%f,%d,%f,%f)",
		    scale,lBinY+diffY*0.05,2000,lBinX,hBinX,2000,lBinY,hBinY),
                    unusedCut,"goff");
  helChain->Draw(Form("yield_sca_laser_PowT*%f+%f:pattern_number >> hPLow(%d,%f,%f,%d,%f,%f)",
		    scale,lBinY+diffY*0.05,2000,lBinX,hBinX,2000,lBinY,hBinY),
                    lasOffCut,"goff");
  TH2F *hPow = (TH2F*) gDirectory->Get("hPow");
  TH2F *hPUnsd = (TH2F*) gDirectory->Get("hPUnsd");
  TH2F *hPLow = (TH2F*) gDirectory->Get("hPLow");
  hPow->SetMarkerColor(kRed);
  hPow->SetLineColor(kRed);
  hPUnsd->SetMarkerColor(kRed+3);
  hPUnsd->SetLineColor(kRed+3);
  hPLow->SetMarkerColor(kRed-8);
  hPLow->SetLineColor(kRed-8);
  helChain->Draw(Form("yield_fadc_compton_accum0.hw_sum-yield_fadc_compton_accum3.hw_sum:pattern_number >> hOn(%d,%f,%f,%d,%f,%f)"
               ,2000,lBinX,hBinX,2000,lBinY,hBinY)
	       ,lasOnCut,"goff");
  TH2F *hOn = (TH2F*) gDirectory->Get("hOn");
  hOn->SetMarkerColor(kGreen);
  hOn->SetLineColor(kGreen);
  hOn->GetXaxis()->SetTitle("Pattern Number");
  hOn->GetYaxis()->SetTitle("Accumulator_0-3 Values");
  hOn->SetTitle("Accumulator_0-3 vs. Pattern Number");
  helChain->Draw(Form("yield_fadc_compton_accum0.hw_sum-yield_fadc_compton_accum3.hw_sum:pattern_number >> hOff(%d,%f,%f,%d,%f,%f)"
               ,2000,lBinX,hBinX,2000,lBinY,hBinY)
	       ,lasOffCut,"goff");
  TH2F *hOff = (TH2F*) gDirectory->Get("hOff");
  hOff->SetMarkerColor(kBlue);
  hOff->SetLineColor(kBlue);
  gStyle->SetOptStat(111);
  hOn->Draw();
  gPad->Update();
  TPaveStats *st2 = (TPaveStats*)gPad->GetPrimitive("stats");
  st2->SetX1NDC(0.58); //new x start position
  st2->SetX2NDC(0.78); //new x end position
  gPad->Update();
  hOff->Draw("sames");
  hPow->Draw("same"); 
  hPLow->Draw("same");
  hPUnsd->Draw("same");
  helChain->Draw(Form("yield_fadc_compton_accum0.hw_sum-yield_fadc_compton_accum3.hw_sum:pattern_number >> hBeamOff(%d,%f,%f,%d,%f,%f)"
                ,2000,lBinX,hBinX,2000,lBinY,hBinY)
                ,beamOffCut,"goff");
  TH2F *hBeamOff = (TH2F*) gDirectory->Get("hBeamOff");
  hBeamOff->SetMarkerColor(kGray+1);
  hBeamOff->SetLineColor(kGray+1);
  hBeamOff->Draw("same");
  TLegend *legend2 = new TLegend(0.88,0.64,0.98,0.835);
  legend2->AddEntry(hPow,"Laser Power","l");
  legend2->AddEntry(hOff,"Laser Off","l");
  legend2->AddEntry(hOn,"Laser On","l");
  legend2->AddEntry(hBeamOff,"Beam Off", "l");
  legend2->SetFillColor(0);
  legend2->Draw();
  gPad->Update();

  //////////////////////////////////////////////////////////////////////
  //***Draw accum values for Laser On and Off using Accum 0 and 3. ***//
  //////////////////////////////////////////////////////////////////////
  c3->cd(1);
  hBinY = helChain->GetMaximum("yield_fadc_compton_accum3.hw_sum")*1.1;
  helChain->Draw("yield_fadc_compton_accum3.hw_sum >> hAccum3","","goff");
  TH1F *hAccum3 = (TH1F*)gDirectory->Get("hAccum3");
  hBinYtmp = hAccum3->GetMean()+ 5*hAccum3->GetRMS();
  if(hBinY>hBinYtmp){
    hBinY = hBinYtmp;
  }
  lBinY = getLeftmost(hAccum3);
  diffY = hBinY - lBinY;
  lBinY -= 0.1*diffY;
  lBinY = TMath::Max(lBinY,-10.0);
  helChain->Draw(Form("yield_fadc_compton_accum3.hw_sum:pattern_number >> hAccum3On(%d,%f,%f,%d,%f,%f)",2000,lBinX,hBinX,2000,lBinY,hBinY)
                 ,lasOnCut,"goff");
  helChain->Draw(Form("yield_fadc_compton_accum3.hw_sum:pattern_number >> hAccum3Off(%d,%f,%f,%d,%f,%f)",2000,lBinX,hBinX,2000,lBinY,hBinY)
                 ,lasOffCut,"goff");
  TH2F *hAccum3On = (TH2F*) gDirectory->Get("hAccum3On");
  TH2F *hAccum3Off = (TH2F*) gDirectory->Get("hAccum3Off");
  hAccum3Off->SetMarkerColor(kBlue);
  hAccum3Off->SetLineColor(kBlue);
  hAccum3On->SetMarkerColor(kGreen);
  hAccum3On->SetLineColor(kGreen);
  hAccum3On->SetTitle("Accumulator_3 vs. Pattern Number");
  hAccum3On->GetXaxis()->SetTitle("Pattern Number");
  hAccum3On->GetYaxis()->SetTitle("Accumulator_3 Values");
  hAccum3On->Draw();
  gPad->Update();
  TPaveStats *st5 = (TPaveStats*)gPad->GetPrimitive("stats");
  st5->SetX1NDC(0.58); //new x start position
  st5->SetX2NDC(0.78); //new x end position
  hAccum3Off->Draw("sames");
  TLegend *legend5 = new TLegend(0.88,0.68,0.98,0.835);
  legend5->AddEntry(hAccum3Off,"Laser Off","l");
  legend5->AddEntry(hAccum3On,"Laser On","l");
  legend5->SetFillColor(0);
  legend5->Draw();
  gPad->Update();

  //////////////////////////////////////////////////////////////////////
  //***Draw num_samples for Laser On and Off using Accum 0 and 3.  ***//
  //////////////////////////////////////////////////////////////////////
  c3->cd(2);
  hBinY = helChain->GetMaximum("yield_fadc_compton_accum3.num_samples")*1.1;
  helChain->Draw("yield_fadc_compton_accum3.num_samples>>hNumAccum3","","goff");
  TH1F *hNumAccum3 = (TH1F*)gDirectory->Get("hNumAccum3");
  hBinYtmp = hNumAccum3->GetMean()+ 5*hNumAccum3->GetRMS();
  if(hBinY>hBinYtmp){
    hBinY = hBinYtmp;
  }
  lBinY = getLeftmost(hNumAccum3);
  diffY = hBinY - lBinY;
  lBinY -= 0.1*diffY;
  lBinY = TMath::Max(lBinY,-10.0);
  helChain->Draw(Form("yield_fadc_compton_accum3.num_samples:pattern_number >> hNumAccum3On(%d,%f,%f,%d,%f,%f)",2000,lBinX,hBinX,2000,lBinY,hBinY)
                 ,lasOnCut,"goff");
  helChain->Draw(Form("yield_fadc_compton_accum3.num_samples:pattern_number >> hNumAccum3Off(%d,%f,%f,%d,%f,%f)",2000,lBinX,hBinX,2000,lBinY,hBinY)
                 ,lasOffCut,"goff");
  TH2F *hNumAccum3On = (TH2F*) gDirectory->Get("hNumAccum3On");
  TH2F *hNumAccum3Off = (TH2F*) gDirectory->Get("hNumAccum3Off");
  hNumAccum3Off->SetMarkerColor(kBlue);
  hNumAccum3Off->SetLineColor(kBlue);
  hNumAccum3On->SetMarkerColor(kGreen);
  hNumAccum3On->SetLineColor(kGreen);
  hNumAccum3On->SetTitle("Accumulator_3 Num Samples vs. Pattern Number");
  hNumAccum3On->GetXaxis()->SetTitle("Pattern Number");
  hNumAccum3On->GetYaxis()->SetTitle("Accumulator_3 Num Samples");
  hNumAccum3On->Draw();
  gPad->Update();
  TPaveStats *st3 = (TPaveStats*)gPad->GetPrimitive("stats");
  st3->SetX1NDC(0.58); //new x start position
  st3->SetX2NDC(0.78); //new x end position
  hNumAccum3Off->Draw("sames");
  TLegend *legend3 = new TLegend(0.88,0.68,0.98,0.835);
  legend3->AddEntry(hNumAccum3Off,"Laser Off","l");
  legend3->AddEntry(hNumAccum3On,"Laser On","l");
  legend3->SetFillColor(0);
  legend3->Draw();
  gPad->Update();

  c3->cd(3);
  hBinY = helChain->GetMaximum("yield_fadc_compton_accum0.num_samples-yield_fadc_compton_accum3.num_samples")*1.1;
  helChain->Draw("yield_fadc_compton_accum0.num_samples-yield_fadc_compton_accum3.num_samples>>hNumAccum03","","goff");
  TH1F *hNumAccum03 = (TH1F*)gDirectory->Get("hNumAccum03");
  hBinYtmp = hNumAccum03->GetMean()+ 5*hNumAccum03->GetRMS();
  if(hBinY>hBinYtmp){
    hBinY = hBinYtmp;
  }
  lBinY = getLeftmost(hNumAccum03);
  diffY = hBinY - lBinY;
  lBinY -= 0.1*diffY;
  lBinY = TMath::Max(lBinY,-10.0);
  helChain->Draw(Form("yield_fadc_compton_accum0.num_samples-yield_fadc_compton_accum3.num_samples.num_samples:pattern_number >> hNumAccum03On(%d,%f,%f,%d,%f,%f)"
                 ,2000,lBinX,hBinX,2000,lBinY,hBinY)
                 ,lasOnCut,"goff");
  helChain->Draw(Form("yield_fadc_compton_accum0.num_samples-yield_fadc_compton_accum3.num_samples.num_samples:pattern_number >> hNumAccum03Off(%d,%f,%f,%d,%f,%f)"
                 ,2000,lBinX,hBinX,2000,lBinY,hBinY)
                 ,lasOffCut,"goff");
  TH2F *hNumAccum03On = (TH2F*) gDirectory->Get("hNumAccum03On");
  TH2F *hNumAccum03Off = (TH2F*) gDirectory->Get("hNumAccum03Off");
  hNumAccum03Off->SetMarkerColor(kBlue);
  hNumAccum03Off->SetLineColor(kBlue);
  hNumAccum03On->SetMarkerColor(kGreen);
  hNumAccum03On->SetLineColor(kGreen);
  hNumAccum03On->SetTitle("Accumulator_0-3 Num Samples vs. Pattern Number");
  hNumAccum03On->GetXaxis()->SetTitle("Pattern Number");
  hNumAccum03On->GetYaxis()->SetTitle("Accumulator_0-3 Num Samples");
  hNumAccum03On->Draw();
  gPad->Update();
  TPaveStats *st4 = (TPaveStats*)gPad->GetPrimitive("stats");
  st4->SetX1NDC(0.58); //new x start position
  st4->SetX2NDC(0.78); //new x end position
  hNumAccum03Off->Draw("sames");
  TLegend *legend4 = new TLegend(0.88,0.68,0.98,0.835);
  legend4->AddEntry(hNumAccum03Off,"Laser Off","l");
  legend4->AddEntry(hNumAccum03On,"Laser On","l");
  legend4->SetFillColor(0);
  legend4->Draw();
  gPad->Update();


  //////////////////////////////////////////////////////////////////////
  //***Draw Background Subtracted Differences for Laser On and Off.***//
  //////////////////////////////////////////////////////////////////////
  helChain->Draw("asym_fadc_compton_accum0.hw_sum*yield_fadc_compton_accum0.hw_sum*2"
               "- asym_fadc_compton_accum3.hw_sum*yield_fadc_compton_accum3.hw_sum*2 >> h","","goff");
  h = (TH1F*)gDirectory->Get("h");
  hBinX =  h->GetRMS()*7;

  h->TH1F::~TH1F();
  lBinX = (-1)*hBinX;
  TH1F *hDiffOn = new TH1F("hDiffOn","hDiffOn",300, lBinX, hBinX);
  TH1F *hDiffOff = new TH1F("hDiffOff", "hDiffOff", 300, lBinX, hBinX); 
  TH1D *hTemp2 = new TH1D();
  Int_t m = 0, n = 0, p = 0, numEnt = 0;
  Int_t cutQual[NCUTS];//cut quality flag.0 if cut not usable.1 if good.
  for(Int_t i=0; i*2<nCuts-2; i++){
    hTemp = new TH1D("hTemp","hTemp",300,lBinX,hBinX);
    hTemp2 = new TH1D("hTemp2", "hTemp2",300, lBinX, hBinX);
    helChain->Draw("asym_fadc_compton_accum0.hw_sum*yield_fadc_compton_accum0.hw_sum*2"
                 "- asym_fadc_compton_accum3.hw_sum*yield_fadc_compton_accum3.hw_sum*2 >> hTemp",
		 asymCut[i],"goff");
    numEnt =(Int_t)hTemp->GetEntries();
    if(numEnt>MIN_ENTRIES){
      cutQual[i] = 1;
      m++;
      cout<<"Cut "<<i<<" for laser On is good."<<endl;
    }else{
      cout<<"Cut "<<i<<" for laser On is NOT good."<<endl;
      cutQual[i] = 0;
    }
    cut1 = Form("Entry$>=%d",cut.at(2*i)); 
    cut2 = Form("Entry$<=%d",cut.at(2*i+1));
    cut3 = Form("Entry$>=%d",cut.at(2*i+2));
    cut4 = Form("Entry$<=%d",cut.at(2*i+3));
    helChain->Draw("asym_fadc_compton_accum0.hw_sum*yield_fadc_compton_accum0.hw_sum*2"
                 "- asym_fadc_compton_accum3.hw_sum*yield_fadc_compton_accum3.hw_sum*2 >> hTemp2",
		 cut1 && cut2 && powCut,"goff");
    numEnt = (Int_t)hTemp2->GetEntries();
    helChain->Draw("asym_fadc_compton_accum0.hw_sum*yield_fadc_compton_accum0.hw_sum*2"
                 "- asym_fadc_compton_accum3.hw_sum*yield_fadc_compton_accum3.hw_sum*2 >> hTemp2",
	          cut3 && cut4 && powCut,"goff");
    numEnt = TMath::Min(numEnt,(Int_t)hTemp2->GetEntries());
    if(numEnt>MIN_ENTRIES){
      n++;
      cout<<"Cut "<<i<<" for laser Off is good."<<endl;
    }else{
      cout<<"Cut "<<i<<" for laser Off is NOT good."<<endl;
      cutQual[i] = 0;
    }
    if(cutQual[i] == 1){
      if(i==0){//add first off cycle if it is good.
	hDiffOn->Add(hTemp);
	hDiffOff->Add(hTemp2);
	helChain->Draw("asym_fadc_compton_accum0.hw_sum*yield_fadc_compton_accum0.hw_sum*2"
                     "- asym_fadc_compton_accum3.hw_sum*yield_fadc_compton_accum3.hw_sum*2 >> hTemp2",
		     cut1 && cut2 && powCut,"goff");
	hDiffOff->Add(hTemp2);
	p++;
      }else{//add all the rest of the on and off cycles.
	hDiffOn->Add(hTemp);
	hDiffOff->Add(hTemp2);
	p++;
      }
    }
    hTemp->TH1D::~TH1D();
    hTemp2->TH1D::~TH1D();
   }
  cout<<m<<" good cuts for laser On."<<endl;
  cout<<n<<" good cuts for laser Off."<<endl;
  cout<<"On && Off = "<<p<<" good cuts in total."<<endl;
  c1->cd(4);
  gStyle->SetOptStat(1111);
  hDiffOn->GetXaxis()->SetTitle("Accumulator_0-3 Differences");
  hDiffOn->SetTitle("Laser On Accumulator_0-3 Differences (2*Yield*Asymmetry)");
  hDiffOn->SetLineColor(kGreen);
  hDiffOn->Draw();
  c1->cd(5);
  hDiffOff->GetXaxis()->SetTitle("Accumulator_0-3 Differences");
  hDiffOff->SetTitle("Laser Off Accumulator_0-3 Differences"
		     " (2*Yield*Asymmetry)");
  hDiffOff->SetLineColor(kBlue);
  hDiffOff->Draw();

  ////////////////////////////////////
  //***Draw Accumulator Asymmetry***//
  ////////////////////////////////////
  TH1D *hAsymm = new TH1D("hAsymm","hAsymm",300,-2,2);
  Double_t sumMeanAsymm=0, sumReciprocalVar=0, sumWeighted=0;
  Double_t meanPosX;
  Double_t meanPosY;

  Double_t rms[NCUTS], mean[NCUTS];
  Double_t errorX[NCUTS], errorY[NCUTS], cutNum[NCUTS];
  c1->cd(6);
  hAsymm->GetXaxis()->SetTitle("Accumulator_0-3 Asymmetry");
  hAsymm->SetTitle("Accumulator_0-3 Asymmetry (Yield*Asymmetry)"
		   "/(Yield-Background)");
  n = 0;
  numEnt = 0;
  Double_t yieldOverBkgd;
  fprintf(polstats,"#CutNum\t Asymm\t\t AsymmRMS \tNumEnt\t YieldOverBkgd  "
	  " YOB_RMS  \t Background\t BkgdRMS \t AvgLasPow\t ScaLasOn"
	  " \t ScaLasOff\t MeanPosX\t MeanPosY\n"); 
  for(Int_t i=0; i*2<nCuts-2; i++){
    TH1D *hTempD = new TH1D("hTempD","hTempD",300,-2,2);
    helChain->Draw(Form("(asym_fadc_compton_accum0.hw_sum*yield_fadc_compton_accum0.hw_sum"
                        "-asym_fadc_compton_accum3.hw_sum*yield_fadc_compton_accum3.hw_sum)"
                       "/(yield_fadc_compton_accum0.hw_sum-yield_fadc_compton_accum3.hw_sum-%f)>>hTempD",
	              bkg[i]),asymCut[i],"goff");
    helChain->Draw(Form("(yield_fadc_compton_accum0.hw_sum-yield_fadc_compton_accum3.hw_sum-%f)>>hTempD2",
	              bkg[i]),asymCut[i],"goff");
    TH1D *hTempD2 = (TH1D*)gDirectory->Get("hTempD2");
    yieldOverBkgd = (Double_t)hTempD2->GetMean();
    numEnt = (Int_t)hTempD->GetEntries();
    if(numEnt>MIN_ENTRIES && cutQual[i]==1){
      mean[n] = hTempD->GetMean();
      rms[n] = hTempD->GetRMS();
      sumMeanAsymm += mean[n];
      sumReciprocalVar += numEnt/(rms[n]*rms[n]);
      sumWeighted += numEnt*mean[n]/(rms[n]*rms[n]);
      helChain->Draw("yield_sca_laser_photon>>hScaOn",
		     lasOnCut && lasWiseCut[i],"goff");
      gPad->Update();
      TH1F *hScaOn=(TH1F*)gDirectory->Get("hScaOn");
      Double_t scaLasOnRate = hScaOn->GetMean();
      hScaOn->TH1F::~TH1F();
      helChain->Draw("yield_sca_laser_photon>>hScaOff",lasOffCut && lasWiseCut[i]
		     ,"goff");
      gPad->Update();
      TH1F *hScaOff=(TH1F*)gDirectory->Get("hScaOff");
      Double_t scaLasOffRate = hScaOff->GetMean();
      hScaOff->TH1F::~TH1F();
      helChain->Draw("yield_sca_laser_PowT>>hLasOn",lasOnCut && lasWiseCut[i],
		     "goff");
      gPad->Update();
      TH1F *hLasOn=(TH1F*)gDirectory->Get("hLasOn");
      Double_t lasAvgPow = hLasOn->GetMean();
      hLasOn->TH1F::~TH1F();

      helChain->Draw("(yield_bpm_3p01_yp-yield_bpm_3p01_ym)/"
		 "(yield_bpm_3p01_yp+yield_bpm_3p01_ym)>>hPosYtmp",
		 powCut && lasWiseCut[i],"goff");
      gPad->Update();
      TH1F *hPosYtmp = (TH1F*)gDirectory->Get("hPosYtmp");
      helChain->Draw("(yield_bpm_3p01_xp-yield_bpm_3p01_xm)/"
		     "(yield_bpm_3p01_xp+yield_bpm_3p01_xm)>>hPosXtmp",
		     powCut && lasWiseCut[i],"goff");
      gPad->Update();
      meanPosY = hPosYtmp->GetMean();
      hPosYtmp->TH1F::~TH1F();
      TH1F *hPosXtmp = (TH1F*)gDirectory->Get("hPosXtmp");
      meanPosX = hPosXtmp->GetMean();
      hPosXtmp->TH1F::~TH1F();
      fprintf(polstats,"%d\t%12.5e\t%9.2e\t%d\t%12.5e\t%9.2e\t%12.5e\t%9.2e\t"
	               "%12.5e\t%12.5e\t%12.5e\t%12.5e\t%12.5e\n",
	      i, hTempD->GetMean(), hTempD->GetRMS(), numEnt,
	      yieldOverBkgd, hTempD2->GetRMS(),bkg[i], bkgWidth[i],lasAvgPow,
	      scaLasOnRate,scaLasOffRate, meanPosX, meanPosY);
      hAsymm->Add(hTempD);
      cutNum[n] = i+1;
      errorY[n] = rms[n]/TMath::Sqrt(numEnt);
      errorX[n] = 0;
      //      cout<<"errorY="<<errorY[n]<<endl;
      n++;
      cout<<"Cut "<<i<<" good. Added to hAsymm."<<endl;

    }else{
      cout<<"Cut "<<i<<" NOT good. NOT added to hAsymm."<<endl;
    }
    hTempD->TH1D::~TH1D();
    hTempD2->TH1D::~TH1D();
  }
  cout<<"Total of "<<n<<" good cuts added to hAsymm."<<endl;
  Double_t meanAsymm = sumWeighted/sumReciprocalVar;
  Double_t meanError = 1.0/TMath::Sqrt(sumReciprocalVar);
  printf("The weighted mean asymmetry is: %11.4e +/- %7.1e\n",
	 meanAsymm,meanError);
  // hAsymm->SetLineColor(kViolet+2);
  hAsymm->Draw();
  TPaveText *pt = new TPaveText(0.01,0.88,0.68,0.94, "NDC"); // NDC sets coords
                                              // relative to pad dimensions
  pt->SetFillColor(0); // text is black on white
  pt->SetTextSize(0.043); 
  pt->SetTextAlign(12);
  pt->AddText(Form("Weighted mean asymmetry: %0.2f\% \t (+/- %0.2f\%)\n",
                   meanAsymm*100,meanError*100," "));
  pt->Draw();
  fclose(polstats);
  TCanvas *c2 = new TCanvas("c2","c2",900,0,530,380);
  c2->SetFillColor(0);
  c2->SetLeftMargin(0.14);
  c2->SetRightMargin(0.06);
  TGraphErrors *gr = new TGraphErrors(n,cutNum,mean,errorX,errorY);
  gr->GetXaxis()->SetTitle("Cut Number");
  gr->GetYaxis()->SetTitle("Accumulator_0-3 Asymmetry");
  gr->GetYaxis()->SetTitleOffset(1.7);
  gr->SetTitle("Laserwise Accumulator_0-3 Asymmetry vs. Cut Number");
  gr->SetMarkerColor(kBlue);
  gr->SetMarkerStyle(21);
  gStyle->SetOptFit(1111);
  gr->Fit("pol0");
  gr->Draw("AP");
  //Save canvases to file.
  if(local == kFALSE){
    cout<<"Saving to web directory."<<endl;
  }else{
    cout<<"Saving to local directory."<<endl;
  }
  c1->Print(canvas1);
  c2->Print(canvas2);
  c3->Print(canvas3);
  cout<<canvas1<<endl;
  cout<<canvas2<<endl;
  printf("\n\n\nRun %d completed.\n\n\n",runnum);
  cut.clear();
  if (deleteOnExit==kTRUE){
    hBeamOff->TH2F::~TH2F();
    hbcm->TH2F::~TH2F();
    hYieldOn->TH1F::~TH1F();
    hYieldOff->TH1F::~TH1F();
    hPow->TH2F::~TH2F();
    hPLow->TH2F::~TH2F();
    hPUnsd->TH2F::~TH2F();
    hOn->TH2F::~TH2F();
    hOff->TH2F::~TH2F();
    hDiffOn->TH1F::~TH1F();
    hDiffOff->TH1F::~TH1F();
    hAsymm->TH1D::~TH1D();
    pt->TPaveText::~TPaveText();
    st2->TPaveStats::~TPaveStats();
    st3->TPaveStats::~TPaveStats();
    st4->TPaveStats::~TPaveStats();
    st5->TPaveStats::~TPaveStats();
    pt->TPaveText::~TPaveText();
    gr->TGraphErrors::~TGraphErrors();
    helChain->TChain::~TChain();
    mpsChain->TChain::~TChain();
    legend1->TLegend::~TLegend();
    legend2->TLegend::~TLegend();
    legend3->TLegend::~TLegend();
    legend4->TLegend::~TLegend();
    legend5->TLegend::~TLegend();
    c1->TCanvas::Destructor();
    c2->TCanvas::Destructor();
    c3->TCanvas::Destructor();
  }

  return 1;
}










///////////////////////////////////////////////////////
//This program loops over all runs in a directory.   //
//The program will loop over all Pass1 runs between  //
//the two argument numbers checking for all segments.//
///////////////////////////////////////////////////////

Int_t analyzeAll(Int_t runNumFirst, Int_t runNumLast){
  Int_t i = runNumFirst;
  while(i<= runNumLast){
    for(Int_t n=0;n<2;n++){
	accum03(i,kFALSE,0);
    }    
    i++;
  }
  return 1;
}
///////////////////////////////////////////////////////////
//To compile at the command line                         //
//g++ -Wall `root-config --ldflags --libs --cflags` -O0  //
//accum03.C -o accum03                             //
///////////////////////////////////////////////////////////

Int_t main(int argc, char *argv[]){
  if (argc < 3){
    cout<<"Usage:accum03(int runNumber,bool isFirst100K, bool deleteOnExit)"
	<<endl;
    return 0;
  }else if(argc==3){
  accum03(atoi(argv[1]),atoi(argv[2]));
  }else accum03(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));
  return 1;
}
