#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <vector>

#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TROOT.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TCint.h"
#include "TSystem.h"
#include "TChain.h"
#include "TGaxis.h"
#include "TLeaf.h"

#include "getChain.C"
#include "cutOnLaser.C"
#include "getRightmost.C"
#include "getLeftmost.C"


////////////////////////////////////////////////////////////////////////////
//This program analyzes a Compton run laser wise and plots the results.   //
//It also stores the results of each laser wise asymmetry and differences //
//in a file called "polstats.txt".                                        //
//If "clear" is nonzero, the canvases will be deleted at the end.         //
//The inputs are run number and either 0 or 1 for whether or not the file //
//is a first100K file. The last argument is also either 
//instructs the program whether or not to delete everything upon closing.//
////////////////////////////////////////////////////////////////////////////

Int_t accum0(Int_t runnum, Bool_t isFirst100k = kFALSE, Bool_t deleteOnExit = kFALSE)
{
  gROOT->Reset();
  //  gStyle->SetCanvasColor(0);

  // Get the chain
  TChain *helChain = getHelChain(runnum,isFirst100k);
  TChain *mpsChain = getMpsChain(runnum,isFirst100k);

  const Double_t LOW_LSR_LMT = 20000.0; // laser unlocked below this
  const Double_t MAXCUR = 180, MINCUR = 2.0; // bcm6 values

  const Int_t MIN_ENTRIES = 2000;//min # of entries to use a laserwise cycle
  const Int_t NCUTS = 500;//initialized size of TCut arrays.
  const Double_t LSCALE = 0.7;//used to scale laser power on yield graph.

  TString www = TString(getenv("QWSCRATCH")) + Form("/www/run_%d/",runnum);
  gSystem->mkdir(www,true);
  TString canvas1 = www + "accum0_plots.png";
  TString canvas2 = www + "accum0_asymm.png";
  TString canvas3 = www + "accum0_yieldvspatnum.png";

  // File for storing stats.
  TString stats = www + "accum0_stats.txt";
  FILE *polstats = fopen(stats.Data(),"w");
     

  //////////////////////////////////////
  //***Turn off all unused branches***//
  //////////////////////////////////////
  /*
  helChain->SetBranchStatus("*", 0);
  helChain->SetBranchStatus("yield_sca_laser_PowT", 1);
  helChain->SetBranchStatus("yield_sca_bcm2_3h05a", 1);
  helChain->SetBranchStatus("yield_sca_bcm6", 1);
  helChain->SetBranchStatus("yield_fadc_compton_accum0*", 1);
  helChain->SetBranchStatus("asym_fadc_compton_accum0*", 1);
  helChain->SetBranchStatus("yield_sca_laser_photon", 1);
  helChain->SetBranchStatus("pattern_number", 1);
  */


  //////////////////////////////////
  //***Define the desired cuts.***//
  //////////////////////////////////

  // Get laser cycles
  std::vector<Int_t> cut = cutOnLaser(helChain);
  Int_t nCuts = cut.size();

  // Reset branch address
  helChain->ResetBranchAddresses();


  Double_t hBinX = helChain->GetMaximum("yield_sca_laser_PowT");
  Double_t lBinX = helChain->GetMinimum("yield_sca_laser_PowT");
  Double_t diffX = hBinX - lBinX;
  helChain->Draw(Form("yield_sca_laser_PowT>>hTemp(100,%f,%f)",lBinX,hBinX),
		 Form("yield_sca_laser_PowT>%f",LOW_LSR_LMT),"goff");
  TH1D *hTemp = (TH1D*)gDirectory->Get("hTemp"); 
  Double_t cutval1 = hTemp->GetMean()-LOW_LSR_LMT;
  Double_t cutval2 = hTemp->GetMean()*2.0;
  std::cout << "Laser power cuts: " << cutval1 << "\t" << cutval2
            << "\t" << hTemp->GetMean() << std::endl;
  delete hTemp;

  Double_t maxCur = helChain->GetMaximum("yield_sca_bcm6/value");
  Double_t cutCur = maxCur * 0.8;
  Bool_t useBCMCut;
  if ( cutCur > 0 ) {
    std::cout << "BCM current cut is " << cutCur << " uA." << std::endl;
    useBCMCut = kTRUE;
  } else {
    std::cout << "BCM current is too low at only " << cutCur << " uA. Disabling current cuts." << std::endl;
    useBCMCut = kFALSE;
  }
  useBCMCut = kTRUE;

  TCut bcmCut = Form("yield_sca_bcm6.value>%f",cutCur);
  TCut beamOffCut = Form("yield_sca_bcm6.value<=%f", MINCUR);

  TCut scalerCut = "yield_sca_laser_photon>2";//PMT On and beam hitting laser
  TCut powCut;
  if(useBCMCut) {
      powCut = bcmCut && scalerCut;//PMT On and hitting laser and beam On
  } else {
    powCut = scalerCut; // We can't rely on a BCM cut, just ask for PMT on
  }
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

  std::vector<TCut> lasWiseCut, bkgCut, asymCut;
  std::vector<Double_t> bkgMean, bkgWidth;
  for (Int_t i = 0; 2*i < nCuts-2; i++) {
    TCut cut1 = (Form("Entry$>=%d",cut.at(2*i)));
    TCut cut2 = (Form("Entry$<=%d",cut.at(2*i+1)));
    TCut cut3 = (Form("Entry$>=%d",cut.at(2*i+2)));
    TCut cut4 = (Form("Entry$<=%d",cut.at(2*i+3)));
    lasWiseCut.push_back(cut1 && cut4);
    bkgCut.push_back(lasOffCut && lasWiseCut[i]);
    asymCut.push_back(lasOnCut && lasWiseCut[i]);

    helChain->Draw("yield_fadc_compton_accum0.hw_sum>>h(100)",bkgCut[i],"goff");
    TH1F *h = (TH1F*) gDirectory->Get("h");
    bkgMean.push_back(h->GetMean());
    bkgWidth.push_back(h->GetRMS());
    std::cout << "bkgMean["  << i << "]=" << bkgMean[i]  << ", "
              << "bkgWidth[" << i << "]=" << bkgWidth[i] << std::endl;
    delete h;

    if (i == 0)
      lasOFFCut = (cut1 && cut2) || (cut3 && cut4);
    else
      lasOFFCut =      lasOFFCut || (cut3 && cut4);
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
  helChain->Draw("yield_sca_bcm6:pattern_number>>hbcm(200,,,200,0.0,180.0)","","goff");
  gPad->Update();
  TH2F *hbcm = (TH2F*)gDirectory->Get("hbcm");
  hbcm->GetXaxis()->SetTitle("Pattern Number");
  hbcm->GetYaxis()->SetTitle("BCM Yield");
  hbcm->SetMarkerColor(kRed);
  hbcm->SetTitle("BCM Yield vs. Pattern Number");
  hbcm->Draw();
  if(hbcm->GetEntries()==0){
    std::cout << "No useful events in run " << runnum << "." << std::endl;
    delete hbcm;
    delete c1;
    return 0;
  }
 
  ////////////////////////////////////////////////////////
  //***Draw accumulator 0 yield with different colors***//
  //***for laser On and Laser Off states.            ***//
  ////////////////////////////////////////////////////////
  c1->cd(2);
  helChain->Draw("yield_fadc_compton_accum0.hw_sum>>h(100)","","goff");
  TH1F *h = (TH1F*)gDirectory->Get("h");
  if(h->GetEntries()==0){
    std::cout << "No useful events in run " << runnum << "." << std::endl;
    delete hbcm;
    delete h;
    delete c1;
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
  delete h;
  TH1F *hYieldOn = new TH1F("hYieldOn", "hYieldOn", 300, lBinY, hBinY);
  TH1F *hYieldOff = new TH1F("hYieldOff", "hYieldOff", 300, lBinY, hBinY);
  helChain->Draw("yield_fadc_compton_accum0.hw_sum>>hYieldOn",lasOnCut,"goff");
  hYieldOn->SetLineColor(kGreen);
  hYieldOn->GetXaxis()->SetTitle("Accumulator_0 Yields");
  hYieldOff->GetXaxis()->SetTitle("Accumulator_0 Yields");
  hYieldOn->SetTitle("Accumulator_0 Yields");
  hYieldOff->SetTitle("Accumulator_0 Yields");
  helChain->Draw("yield_fadc_compton_accum0.hw_sum>>hYieldOff",
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
  Float_t maxPowT = helChain->GetMaximum("yield_sca_laser_PowT/value");
  //std::cout <<lBinX<<"\t"<<hBinX<<"\t"<<lBinY<<"\t"<<hBinY<<std::endl;
  diffY = hBinY - lBinY;
  hBinY += diffY*0.1;
  lBinY -= diffY*0.1;
  //  std::cout <<lBinX<<"\t"<<hBinX<<"\t"<<lBinY<<"\t"<<hBinY<<std::endl;
  Float_t scale;
  if (maxPowT > 0 ) {
    scale = LSCALE*(Float_t)(hBinY-lBinY)/(Float_t)maxPowT;
  } else {
    scale = LSCALE*(Float_t)(hBinY-lBinY)/1;
  }
  // printf("maxPowT=%f\nscale=%f\n",maxPowT,scale);
  TH2F *hPow = new TH2F("hPow","hPow", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *hPUnsd = new TH2F("hPUnsd","hPUnsd",2000,lBinX,hBinX,2000,lBinY,hBinY);
  TH2F *hPLow = new TH2F("hPLow","hPLow",2000,lBinX, hBinX,2000, lBinY, hBinY);
  TH2F *hOn = new TH2F("hOn","hOn", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *hOff = new TH2F("hOff","hOff", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *hBeamOff = new TH2F("hBeamOff","hOBeamOff", 2000, lBinX, hBinX, 
			                            2000, lBinY, hBinY);
  c1->cd(3);
  c3->cd(1);
  helChain->Draw(Form("yield_sca_laser_PowT*%f+%f:pattern_number>>hPow",
		    scale,lBinY+diffY*0.05),lasOnCut,"goff");
  helChain->Draw(Form("yield_sca_laser_PowT*%f+%f:pattern_number>>hPUnsd",
		    scale,lBinY+diffY*0.05),unusedCut,"goff");
  helChain->Draw(Form("yield_sca_laser_PowT*%f+%f:pattern_number>>hPLow",
		    scale,lBinY+diffY*0.05),lasOffCut,"goff");
  hPow->SetMarkerColor(kRed);
  hPow->SetLineColor(kRed);
  hPUnsd->SetMarkerColor(kRed+3);
  hPUnsd->SetLineColor(kRed+3);
  hPLow->SetMarkerColor(kRed-8);
  hPLow->SetLineColor(kRed-8);
  helChain->Draw("yield_fadc_compton_accum0.hw_sum:pattern_number>>hOn"
	       ,lasOnCut,"goff");
  hOn->SetMarkerColor(kGreen);
  hOn->SetLineColor(kGreen);
  hOn->GetXaxis()->SetTitle("Pattern Number");
  hOn->GetYaxis()->SetTitle("Accumulator_0 Values");
  hOn->SetTitle("Accumulator_0 vs. Pattern Number");
  helChain->Draw("yield_fadc_compton_accum0.hw_sum:pattern_number>>hOff"
	       ,lasOffCut,"goff");
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
  helChain->Draw("yield_fadc_compton_accum0.hw_sum:pattern_number>>hBeamOff"
		 ,useBCMCut?beamOffCut:"","goff");
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
  c1->cd(3);
  hOn->Draw();
  hOff->Draw("sames");
  hPow->Draw("same"); 
  hPLow->Draw("same");
  hPUnsd->Draw("same");
  hBeamOff->Draw("same");
  TLegend *legend2p = new TLegend(0.83,0.68,0.98,0.835);
  legend2p->AddEntry(hPow,"Laser Power","l");
  legend2p->AddEntry(hOff,"Laser Off","l");
  legend2p->AddEntry(hOn,"Laser On","l");
  legend2p->AddEntry(hBeamOff,"Beam Off", "l");
  legend2p->SetFillColor(0);
  legend2p->Draw();
  //Draw similar plot using scalers.
  c3->cd(2);
  hBinY = helChain->GetMaximum("yield_sca_laser_photon")*1.1;
  helChain->Draw("yield_sca_laser_photon>>h2","","goff");
  TH1F *h2 = (TH1F*)gDirectory->Get("h2"); 
  hBinYtmp = h2->GetMean()+ 5*h2->GetRMS();
  if(hBinY>hBinYtmp){
    hBinY = hBinYtmp;
  }
  lBinY = getLeftmost(h2);
  diffY = hBinY - lBinY;
  lBinY -= 0.1*diffY;
  lBinY = TMath::Max(lBinY,-10.0);
  TH2F *hOn2 = new TH2F("hOn2","hOn2", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  TH2F *hOff2 = new TH2F("hOff2","hOff2", 2000, lBinX, hBinX, 2000, lBinY, hBinY);
  helChain->Draw("yield_sca_laser_photon:pattern_number>>hOn2"
	         ,lasOnCut1 && lasOnCut2,"goff");
  helChain->Draw("yield_sca_laser_photon:pattern_number>>hOff2"
	         ,lasOffCut2,"goff");
  hOff2->SetMarkerColor(kBlue);
  hOff2->SetLineColor(kBlue);
  hOn2->SetMarkerColor(kGreen);
  hOn2->SetLineColor(kGreen);
  hOn2->SetTitle("Photon Detector Scaler vs. Pattern Number");
  hOn2->GetXaxis()->SetTitle("Pattern Number");
  hOn2->GetYaxis()->SetTitle("Photon Detector Scaler");
  hOn2->Draw();
  gPad->Update();
  TPaveStats *st3 = (TPaveStats*)gPad->GetPrimitive("stats");
  st3->SetX1NDC(0.58); //new x start position
  st3->SetX2NDC(0.78); //new x end position
  hOff2->Draw("sames");
  TLegend *legend3 = new TLegend(0.88,0.68,0.98,0.835);
  legend3->AddEntry(hOff,"Laser Off","l");
  legend3->AddEntry(hOn,"Laser On","l");
  legend3->SetFillColor(0);
  legend3->Draw();
  gPad->Update();


  //////////////////////////////////////////////////////////////////////
  //***Draw Beam Positions for Laser On and Off using Upstream BPM.***//
  //////////////////////////////////////////////////////////////////////
  c3->cd(3);
  Double_t lowY1, highY1, lowY2, highY2, diffY1, diffY2, meanY1, meanY2;
  Double_t rmsY1, rmsY2, offset;
   
  helChain->Draw("yield_sca_bpm_3p02aY>>hPosY1", useBCMCut?bcmCut:"","goff");
  gPad->Update();
  TH1F *hPosY1 = (TH1F*)gDirectory->Get("hPosY1"); 
  lowY1 = (Double_t)getLeftmost(hPosY1);
  highY1 = (Double_t)getRightmost(hPosY1);
  diffY1 = highY1 - lowY1;
  lowY1 -= 0.1*diffY1;
  highY1 += 0.1*diffY1;
  rmsY1 = hPosY1->GetRMS();
  meanY1 = (lowY1 + highY1)/2.0;
  delete hPosY1;
  helChain->Draw("yield_sca_bpm_3p02aX>>hPosX1", useBCMCut?bcmCut:"","goff");
  gPad->Update();
  //Scale second graph to first and put new axis on right side.
  TH1F *hPosX1 = (TH1F*)gDirectory->Get("hPosX1"); 
  lowY2 = (Double_t)getLeftmost(hPosX1);
  highY2 = (Double_t)getRightmost(hPosX1);
  lowY2 = TMath::Min(lowY2,lowY1);
  highY2 = TMath::Max(highY2,highY1);
  diffY2 = highY2 - lowY2;
  lowY2 -= 0.1*diffY2;
  highY2 += 0.1*diffY2;
  rmsY2 = hPosX1->GetRMS();
  meanY2 = (lowY2 + highY2)/2.0;
  delete hPosX1;
  TH2F *hPosY = new TH2F("hPosY","hPosY",1000,lBinX,hBinX,1000,lowY2,highY2);
  hPosY->GetXaxis()->SetTitle("Pattern Number");
  hPosY->GetYaxis()->SetTitle("Position");
  hPosY->SetTitle("Beam Position using Upstream BPM");
  hPosY->SetMarkerColor(kRed);
  hPosY->SetLineColor(kRed);
  hPosY->Draw();
  helChain->Draw("yield_sca_bpm_3p02aY:pattern_number>>hPosY", useBCMCut?bcmCut:"","");
  gPad->Update();
  TPaveStats *st4 = (TPaveStats*)gPad->GetPrimitive("stats");
  st4->SetX1NDC(0.58); //new x start position
  st4->SetX2NDC(0.78); //new x end position 
  Float_t scale1 = (diffY1+5*rmsY1)/(diffY2+5*rmsY2);
  offset = meanY1 - meanY2*scale1 - 0.4*diffY1;
  TH2F *hPosX = new TH2F("hPosX","hPosX",1000,lBinX,hBinX,1000,lowY2,highY2);
  helChain->Draw("yield_sca_bpm_3p02aX:pattern_number>>hPosX",useBCMCut?bcmCut:"","goff");
  printf("Scale hPosX by %f. Offset = %f.\n",scale1, offset);
  hPosX->SetMarkerColor(kBlue);
  hPosX->SetLineColor(kBlue);
  hPosX->Draw("sames");
  gPad->Update();

  TLegend *legend4 = new TLegend(0.88,0.68,0.98,0.835);
  legend4->AddEntry(hPosX,"X Pos","l");
  legend4->AddEntry(hPosY,"Y Pos","l");
  legend4->SetFillColor(0);
  legend4->Draw();
  gPad->Update();
 

  //////////////////////////////////////////////////////////////////////
  //***Draw Background Subtracted Differences for Laser On and Off.***//
  //////////////////////////////////////////////////////////////////////
  helChain->Draw("asym_fadc_compton_accum0.hw_sum"
                 "*yield_fadc_compton_accum0.hw_sum*2>>h","","goff");
  h = (TH1F*)gDirectory->Get("h");
  hBinX =  h->GetRMS()*7;

  delete h;
  lBinX = (-1)*hBinX;
  TH1F *hDiffOn = new TH1F("hDiffOn","hDiffOn",300, lBinX, hBinX);
  TH1F *hDiffOff = new TH1F("hDiffOff", "hDiffOff", 300, lBinX, hBinX); 
  TH1D *hTemp2 = new TH1D();
  Int_t m = 0, n = 0, p = 0, numEnt = 0;
  Int_t cutQual[NCUTS];//cut quality flag.0 if cut not usable.1 if good.
  for(Int_t i=0; i*2<nCuts-2; i++){
    hTemp = new TH1D("hTemp","hTemp",300,lBinX,hBinX);
    hTemp2 = new TH1D("hTemp2", "hTemp2",300, lBinX, hBinX);
    helChain->Draw("asym_fadc_compton_accum0.hw_sum"
                  "*yield_fadc_compton_accum0.hw_sum*2>>hTemp",
		 asymCut[i],"goff");
    numEnt =(Int_t)hTemp->GetEntries();
    std::cout << "Cut " <<i<<" has " << numEnt << " entries\n";
    if(numEnt>MIN_ENTRIES){
      cutQual[i] = 1;
      m++;
      std::cout <<"Cut "<<i<<" for laser On is good."<<std::endl;
    }else{
      std::cout <<"Cut "<<i<<" for laser On is NOT good."<<std::endl;
      cutQual[i] = 0;
    }
    TCut cut1 = Form("Entry$>=%d",cut.at(2*i)); 
    TCut cut2 = Form("Entry$<=%d",cut.at(2*i+1));
    TCut cut3 = Form("Entry$>=%d",cut.at(2*i+2));
    TCut cut4 = Form("Entry$<=%d",cut.at(2*i+3));
    helChain->Draw("asym_fadc_compton_accum0.hw_sum"
                 "*yield_fadc_compton_accum0.hw_sum*2>>hTemp2",
		 cut1 && cut2 && powCut,"goff");
    numEnt = (Int_t)hTemp2->GetEntries();
    helChain->Draw("asym_fadc_compton_accum0.hw_sum"
                 "*yield_fadc_compton_accum0.hw_sum*2>>hTemp2",
	          cut3 && cut4 && powCut,"goff");
    numEnt = TMath::Min(numEnt,(Int_t)hTemp2->GetEntries());
    if(numEnt>MIN_ENTRIES){
      n++;
      std::cout <<"Cut "<<i<<" for laser Off is good."<<std::endl;
    }else{
      std::cout <<"Cut "<<i<<" for laser Off is NOT good."<<std::endl;
      cutQual[i] = 0;
    }
    if(cutQual[i] == 1){
      if(i==0){//add first off cycle if it is good.
	hDiffOn->Add(hTemp);
	hDiffOff->Add(hTemp2);
	helChain->Draw("asym_fadc_compton_accum0.hw_sum"
                     "*yield_fadc_compton_accum0.hw_sum*2>>hTemp2",
		     cut1 && cut2 && powCut,"goff");
	hDiffOff->Add(hTemp2);
	p++;
      }else{//add all the rest of the on and off cycles.
	hDiffOn->Add(hTemp);
	hDiffOff->Add(hTemp2);
	p++;
      }
    }
    delete hTemp;
    delete hTemp2;
   }
  std::cout <<m<<" good cuts for laser On."<<std::endl;
  std::cout <<n<<" good cuts for laser Off."<<std::endl;
  std::cout <<"On && Off = "<<p<<" good cuts in total."<<std::endl;
  c1->cd(4);
  gStyle->SetOptStat(1111);
  hDiffOn->GetXaxis()->SetTitle("Accumulator_0 Differences");
  hDiffOn->SetTitle("Laser On Accumulator_0 Differences (2*Yield*Asymmetry)");
  hDiffOn->SetLineColor(kGreen);
  hDiffOn->Draw();
  c1->cd(5);
  hDiffOff->GetXaxis()->SetTitle("Accumulator_0 Differences");
  hDiffOff->SetTitle("Laser Off Accumulator_0 Differences"
		     " (2*Yield*Asymmetry)");
  hDiffOff->SetLineColor(kBlue);
  hDiffOff->Draw();

  /*
  c1->cd(5);
  hDiffOff->GetXaxis()->SetTitle("Accumulator_0 Differences");
  hDiffOff->SetTitle("Laser Off Accumulator_0 Differences"
                     " (2*Yield*Asymmetry)");
  helChain->Draw("asym_fadc_compton_accum0.hw_sum
                  *yield_fadc_compton_accum0.hw_sum*2>>hDiffOff",
	       lasOFFCut);
  */

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
  hAsymm->GetXaxis()->SetTitle("Accumulator_0 Asymmetry");
  hAsymm->SetTitle("Accumulator_0 Asymmetry (Yield*Asymmetry)"
		   "/(Yield-Background)");
  n = 0;
  numEnt = 0;
  Double_t yieldOverBkgd;
  fprintf(polstats,"#CutNum\t Asymm\t\t AsymmRMS \tNumEnt\t YieldOverBkgd  "
	  " YOB_RMS  \t Background\t BkgdRMS \t AvgLasPow\t ScaLasOn"
	  " \t ScaLasOff\t MeanPosX\t MeanPosY\n"); 
  for(Int_t i=0; i*2<nCuts-2; i++){
    TH1D *hTempD = new TH1D("hTempD","hTempD",300,-2,2);
    helChain->Draw(Form("asym_fadc_compton_accum0.hw_sum"
                      "*yield_fadc_compton_accum0.hw_sum"
                       "/(yield_fadc_compton_accum0.hw_sum-%f)>>hTempD",
	              bkgMean[i]),asymCut[i],"goff");
    helChain->Draw(Form("(yield_fadc_compton_accum0.hw_sum-%f)>>hTempD2",
	              bkgMean[i]),asymCut[i],"goff");
    TH1D *hTempD2 = (TH1D*)gDirectory->Get("hTempD2");
    yieldOverBkgd = (Double_t)hTempD2->GetMean();
    numEnt = (Int_t)hTempD->GetEntries();
    if(numEnt>MIN_ENTRIES && cutQual[i]==1){
      mean[n] = hTempD->GetMean();
      rms[n] = hTempD->GetRMS();
      sumMeanAsymm += mean[n];
      sumReciprocalVar += numEnt/(rms[n]*rms[n]);
      sumWeighted += numEnt*mean[n]/(rms[n]*rms[n]);
      helChain->Draw("yield_sca_laser_photon>>hScaOn",lasOnCut && lasWiseCut[i],"goff");
      gPad->Update();
      TH1F *hScaOn=(TH1F*)gDirectory->Get("hScaOn");
      Double_t scaLasOnRate = hScaOn->GetMean();
      delete hScaOn;
      helChain->Draw("yield_sca_laser_photon>>hScaOff",lasOffCut && lasWiseCut[i],"goff");
      gPad->Update();
      TH1F *hScaOff=(TH1F*)gDirectory->Get("hScaOff");
      Double_t scaLasOffRate = hScaOff->GetMean();
      delete hScaOff;
      helChain->Draw("yield_sca_laser_PowT>>hLasOn",lasOnCut && lasWiseCut[i],"goff");
      gPad->Update();
      TH1F *hLasOn=(TH1F*)gDirectory->Get("hLasOn");
      Double_t lasAvgPow = hLasOn->GetMean();
      delete hLasOn;

      helChain->Draw("yield_sca_bpm_3p02aY>>hPosYtmp", powCut && lasWiseCut[i],"goff");
      gPad->Update();
      TH1F *hPosYtmp = (TH1F*)gDirectory->Get("hPosYtmp");
      helChain->Draw("yield_sca_bpm_3p02aX>>hPosXtmp", powCut && lasWiseCut[i],"goff");
      gPad->Update();
      meanPosY = hPosYtmp->GetMean();
      delete hPosYtmp;
      TH1F *hPosXtmp = (TH1F*)gDirectory->Get("hPosXtmp");
      meanPosX = hPosXtmp->GetMean();
      delete hPosXtmp;
      fprintf(polstats,"%d\t%12.5e\t%9.2e\t%d\t%12.5e\t%9.2e\t%12.5e\t%9.2e\t"
	               "%12.5e\t%12.5e\t%12.5e\t%12.5e\t%12.5e\n",
	      i, hTempD->GetMean(), hTempD->GetRMS(), numEnt,
	      yieldOverBkgd, hTempD2->GetRMS(),bkgMean[i], bkgWidth[i],lasAvgPow,
	      scaLasOnRate,scaLasOffRate, meanPosX, meanPosY);
      hAsymm->Add(hTempD);
      cutNum[n] = i+1;
      errorY[n] = rms[n]/TMath::Sqrt(numEnt);
      errorX[n] = 0;
      //      std::cout <<"errorY="<<errorY[n]<<std::endl;
      n++;
      std::cout <<"Cut "<<i<<" good. Added to hAsymm."<<std::endl;

    }else{
      std::cout <<"Cut "<<i<<" NOT good. NOT added to hAsymm."<<std::endl;
    }
    delete hTempD;
    delete hTempD2;
  }
  std::cout << "Total of " << n << " good cuts added to hAsymm." << std::endl;
  Double_t meanAsymm = sumWeighted/sumReciprocalVar;
  Double_t meanError = 1.0/TMath::Sqrt(sumReciprocalVar);
  std::cout << "The weighted mean asymmetry is: "
            << std::setprecision(6) << meanAsymm << " +/- " 
            << std::setprecision(6) << meanError
            << std::endl;
  // hAsymm->SetLineColor(kViolet+2);
  hAsymm->Draw();
  TPaveText *pt = new TPaveText(0.01,0.88,0.68,0.94, "NDC"); // NDC sets coords
                                              // relative to pad dimensions
  pt->SetFillColor(0); // text is black on white
  pt->SetTextSize(0.043); 
  pt->SetTextAlign(12);
  pt->AddText(Form("Weighted mean asymmetry: %0.2f%% \t (+/- %0.2f%%)\n", 
                    meanAsymm*100,meanError*100));
  pt->Draw();
  fclose(polstats);
  TCanvas *c2 = new TCanvas("c2","c2",900,0,530,380);
  c2->SetFillColor(0);
  c2->SetLeftMargin(0.14);
  c2->SetRightMargin(0.06);
  TGraphErrors *gr = new TGraphErrors(n,cutNum,mean,errorX,errorY);
  gr->GetXaxis()->SetTitle("Cut Number");
  gr->GetYaxis()->SetTitle("Accumulator_0 Asymmetry");
  gr->GetYaxis()->SetTitleOffset(1.7);
  gr->SetTitle("Laserwise Accumulator_0 Asymmetry vs. Cut Number");
  gr->SetMarkerColor(kBlue);
  gr->SetMarkerStyle(21);
  gStyle->SetOptFit(1111);
  gr->Fit("pol0");
  gr->Draw("AP");

  // Save canvases to file.
  c1->Print(canvas1);
  c2->Print(canvas2);
  c3->Print(canvas3);

  cut.clear();
  if (deleteOnExit==kTRUE){
    delete hBeamOff;
    delete hbcm;
    delete hYieldOn;
    delete hYieldOff;
    delete hPow;
    delete hPosX;
    delete hPosX1;
    delete hPosY;
    delete hPLow;
    delete hPUnsd;
    delete hOn;
    delete hOff;
    delete hDiffOn;
    delete hDiffOff;
    delete hAsymm;
    delete pt;
    delete st2;
    delete st3;
    delete st4;
    delete pt;
    delete gr;
    delete helChain;
    delete mpsChain;
    delete legend1;
    delete legend2;
    delete legend2p;
    delete legend3;
    delete legend4;
    delete c1;
    delete c2;
    delete c3;
  }

  return 1;
}


////////////////////////////////////////////////////////
// This program loops over all runs in a directory.   //
// The program will loop over all Pass? runs between  //
// the two argument numbers checking for all segments.//
////////////////////////////////////////////////////////

Int_t analyzeAll(Int_t runNumFirst, Int_t runNumLast)
{
  Int_t i = runNumFirst;
  while (i <= runNumLast) {
    for (Int_t n = 0; n < 2; n++) {
      accum0(i,kFALSE,0);
    }
    i++;
  }
  return 1;
}


///////////////////////////////////////////////////////////
// To compile at the command line                        //
// g++ -Wall `root-config --ldflags --libs --cflags` -O0 //
// accum0.C -o accum0                                    //
///////////////////////////////////////////////////////////

Int_t main(int argc, char *argv[])
{
  if (argc < 3) {
    std::cout << "Usage: accum0(int runNumber, bool isFirst100k, bool deleteOnExit)"
	      << std::endl;
    return 0;
  } else if (argc == 3) {
    return accum0(atoi(argv[1]),atoi(argv[2]));
  } else {
    return accum0(atoi(argv[1]),atoi(argv[2]),atoi(argv[3]));
  }
}
