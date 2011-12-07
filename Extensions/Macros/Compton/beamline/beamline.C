// Standard Lib includes
#include <iostream>

// ROOT Includes
#include "TChain.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TGraph.h"
#include "TH2F.h"
#include "TGaxis.h"
#include "TRandom.h"

// Compton Includes
#include "getChain.C"

// Random number generator seed
ULong_t gRandomSeed = 55.;
// Prototype functions
void ExpandBPMRange(Double_t *min, Double_t *max);

Int_t beamline(Int_t runnum, Bool_t isFirst100k = kFALSE, Bool_t deleteOnExit = kFALSE)
{
  gROOT->Reset();

  // Get the chain
  TChain *mpsChain = getMpsChain(runnum,isFirst100k);
  if (!mpsChain) {
    std::cout << "What a scam!! We didn't even get data!" << std::endl;
    return -1;
  }

  // Prepare the canvas
  TCanvas *canvas = new TCanvas("BeamLineCanvas","Beam Line Canvas",800,1024);
  canvas->Divide(1,5);
  canvas->SetFillColor(0);
  TCanvas *tmpCanvas = new TCanvas("TmpBeamLineCanvas","Beam Line Canvas",1024,1600);
  tmpCanvas->Divide(1,4);

  // Prepare the web url
  TString www = TString(getenv("QWSCRATCH")) + Form("/www/run_%d/",runnum);
  gSystem->mkdir(www,true);
  TString canvas1 = www+"beamline_plots.png";

  // Record Extremeties
  Double_t maxMps, minMps;
  Double_t maxBPMX, minBPMX, minBPMY, maxBPMY;
  Double_t dx, dy;
  maxMps = mpsChain->GetMaximum("mps_counter");
  minMps = mpsChain->GetMinimum("mps_counter");

  // Plot BPM 3P02A
  TPad *p02AX = new TPad("p02AX","",0,0,1,1);
  TPad *p02AY = new TPad("p02AY","",0,0,1,1);
  p02AY->SetFillStyle(4000);
  TGaxis *a02A;

  minBPMX = mpsChain->GetMinimum("sca_bpm_3p02aX/value");
  maxBPMX = mpsChain->GetMaximum("sca_bpm_3p02aX/value");
  minBPMY = mpsChain->GetMinimum("sca_bpm_3p02aY/value");
  maxBPMY = mpsChain->GetMaximum("sca_bpm_3p02aY/value");

  ExpandBPMRange(&minBPMX,&maxBPMX);
  ExpandBPMRange(&minBPMY,&maxBPMY);

  TH2F *h02AX = new TH2F("h02AX",
      "BPM 3P02AX vs Mps Counter;Mps Counter;BPM 3P02AX (mm)",
      500,minMps,maxMps,500,minBPMX,maxBPMX);
  TH2F *h02AY = new TH2F("h02AY",
      "BPM 3P02AY vs Mps Counter;Mps Counter;BPM 3P02AY (mm)",
      500,minMps,maxMps,500,minBPMY,maxBPMY);

  // Now plot all the stuff out
  tmpCanvas->cd(1);
  p02AX->SetFillColor(0);
  mpsChain->Draw("sca_bpm_3p02aX:mps_counter>>h02AX","compton_charge>25");
  canvas->cd(1);
  h02AX = (TH2F*)gDirectory->Get("h02AX");
  p02AX->Draw();
  p02AX->cd();
  h02AX->SetMarkerColor(kBlue);
  h02AX->SetStats(kFALSE);
  h02AX->Draw();
  tmpCanvas->cd(1);
  mpsChain->Draw("sca_bpm_3p02aY:mps_counter>>h02AY","compton_charge>25","][sames");
  h02AY = (TH2F*)gDirectory->Get("h02AY");
  h02AY->SetMarkerColor(kRed);
  h02AY->SetStats(kFALSE);
  canvas->cd(1);
  dy = (maxBPMY-minBPMY)/.8;
  dx = (maxMps-minMps)/.8;
  p02AY->Range(minMps-.1*dx,minBPMY-.1*dy,maxMps+.1*dx,maxBPMY+.1*dy);
  p02AY->Draw();
  p02AY->cd();
  //h02AY->Draw();//"][sames");
  h02AY->Draw("][sames");
  p02AY->Update();
  a02A = new TGaxis(maxMps,minBPMY,maxMps,maxBPMY,minBPMY,maxBPMY,50510,"L+");
  a02A->SetLabelColor(kRed);
  a02A->SetTitleColor(kRed);
  a02A->SetTitle("BPM 3P02AY (mm)");
  a02A->Draw();

  // Plot BPM 3P02B
  TPad *p02BX = new TPad("p02AX","",0,0,1,1);
  TPad *p02BY = new TPad("p02AY","",0,0,1,1);
  p02BY->SetFillStyle(4000);
  TGaxis *a02b;

  minBPMX = mpsChain->GetMinimum("sca_bpm_3p02bX/value");
  maxBPMX = mpsChain->GetMaximum("sca_bpm_3p02bX/value");
  minBPMY = mpsChain->GetMinimum("sca_bpm_3p02bY/value");
  maxBPMY = mpsChain->GetMaximum("sca_bpm_3p02bY/value");

  ExpandBPMRange(&minBPMX,&maxBPMX);
  ExpandBPMRange(&minBPMY,&maxBPMY);

  TH2F *h02BX = new TH2F("h02BX",
      "BPM 3P02BX vs Mps Counter;Mps Counter;BPM 3P02BX (mm)",
      500,minMps,maxMps,500,minBPMX,maxBPMX);
  TH2F *h02BY = new TH2F("h02BY",
      "BPM 3P02BY vs Mps Counter;Mps Counter;BPM 3P02BY (mm)",
      500,minMps,maxMps,500,minBPMY,maxBPMY);

  // Now plot all the stuff out
  tmpCanvas->cd(2);
  p02BX->SetFillColor(0);
  mpsChain->Draw("sca_bpm_3p02bX:mps_counter>>h02BX","compton_charge>25");
  h02BX = (TH2F*)gDirectory->Get("h02BX");
  canvas->cd(2);
  p02BX->Draw();
  p02BX->cd();
  h02BX->SetMarkerColor(kBlue);
  h02BX->SetStats(kFALSE);
  h02BX->Draw();
  tmpCanvas->cd(2);
  mpsChain->Draw("sca_bpm_3p02bY:mps_counter>>h02BY","compton_charge>25","][sames");
  canvas->cd(2);
  h02BY = (TH2F*)gDirectory->Get("h02BY");
  h02BY->SetMarkerColor(kRed);
  h02BY->SetStats(kFALSE);
  dy = (maxBPMY-minBPMY)/.8;
  dx = (maxMps-minMps)/.8;
  p02BY->Range(minMps-.1*dx,minBPMY-.1*dy,maxMps+.1*dx,maxBPMY+.1*dy);
  p02BY->Draw();
  p02BY->cd();
  //h02BY->Draw();//"][sames");
  h02BY->Draw("][sames");
  p02BY->Update();
  a02b = new TGaxis(maxMps,minBPMY,maxMps,maxBPMY,minBPMY,maxBPMY,50510,"L+");
  a02b->SetLabelColor(kRed);
  a02b->SetTitleColor(kRed);
  a02b->SetTitle("BPM 3P02BY (mm)");
  a02b->Draw();



  // Plot BPM 3P03A
  TPad *p03AX = new TPad("p02AX","",0,0,1,1);
  TPad *p03AY = new TPad("p02AY","",0,0,1,1);
  p03AY->SetFillStyle(4000);
  TGaxis *a03a;

  minBPMX = mpsChain->GetMinimum("sca_bpm_3p03aX/value");
  maxBPMX = mpsChain->GetMaximum("sca_bpm_3p03aX/value");
  minBPMY = mpsChain->GetMinimum("sca_bpm_3p03aY/value");
  maxBPMY = mpsChain->GetMaximum("sca_bpm_3p03aY/value");

  ExpandBPMRange(&minBPMX,&maxBPMX);
  ExpandBPMRange(&minBPMY,&maxBPMY);

  TH2F *h03AX = new TH2F("h03AX",
      "BPM 3P03AX vs Mps Counter;Mps Counter;BPM 3P03AX (mm)",
      500,minMps,maxMps,500,minBPMX,maxBPMX);
  TH2F *h03AY = new TH2F("h03AY",
      "BPM 3P03AY vs Mps Counter;Mps Counter;BPM 3P03AY (mm)",
      500,minMps,maxMps,500,minBPMY,maxBPMY);

  // Now plot all the stuff out
  tmpCanvas->cd(3);
  p03AX->SetFillColor(0);
  mpsChain->Draw("sca_bpm_3p03aX:mps_counter>>h03AX","compton_charge>25");
  h03AX = (TH2F*)gDirectory->Get("h03AX");
  canvas->cd(3);
  p03AX->Draw();
  p03AX->cd();
  h03AX->SetMarkerColor(kBlue);
  h03AX->SetStats(kFALSE);
  h03AX->Draw();
  tmpCanvas->cd(3);
  mpsChain->Draw("sca_bpm_3p03aY:mps_counter>>h03AY","compton_charge>25","][sames");
  canvas->cd(3);
  h03AY = (TH2F*)gDirectory->Get("h03AY");
  h03AY->SetMarkerColor(kRed);
  h03AY->SetStats(kFALSE);
  dy = (maxBPMY-minBPMY)/.8;
  dx = (maxMps-minMps)/.8;
  p03AY->Range(minMps-.1*dx,minBPMY-.1*dy,maxMps+.1*dx,maxBPMY+.1*dy);
  p03AY->Draw();
  p03AY->cd();
  //h03AY->Draw();//"][sames");
  h03AY->Draw("][sames");
  p03AY->Update();
  a03a = new TGaxis(maxMps,minBPMY,maxMps,maxBPMY,minBPMY,maxBPMY,50510,"L+");
  a03a->SetLabelColor(kRed);
  a03a->SetTitleColor(kRed);
  a03a->SetTitle("BPM 3P03AY (mm)");
  a03a->Draw();



  // Plot BPM 3C020
  TPad *p020X = new TPad("p02AX","",0,0,1,1);
  TPad *p020Y = new TPad("p02AY","",0,0,1,1);
  p020Y->SetFillStyle(4000);
  TGaxis *a020;

  minBPMX = mpsChain->GetMinimum("sca_bpm_3c20X/value");
  maxBPMX = mpsChain->GetMaximum("sca_bpm_3c20X/value");
  minBPMY = mpsChain->GetMinimum("sca_bpm_3c20Y/value");
  maxBPMY = mpsChain->GetMaximum("sca_bpm_3c20Y/value");

  ExpandBPMRange(&minBPMX,&maxBPMX);
  ExpandBPMRange(&minBPMY,&maxBPMY);

  TH2F *h020X = new TH2F("h020X",
      "BPM 3C020X vs Mps Counter;Mps Counter;BPM 3C020X (mm)",
      500,minMps,maxMps,500,minBPMX,maxBPMX);
  TH2F *h020Y = new TH2F("h020Y",
      "BPM 3C020Y vs Mps Counter;Mps Counter;BPM 3C020Y (mm)",
      500,minMps,maxMps,500,minBPMY,maxBPMY);

  // Now plot all the stuff out
  tmpCanvas->cd(4);
  p020X->SetFillColor(0);
  mpsChain->Draw("sca_bpm_3c20X:mps_counter>>h020X","compton_charge>25");
  h020X = (TH2F*)gDirectory->Get("h020X");
  canvas->cd(4);
  p020X->Draw();
  p020X->cd();
  h020X->SetMarkerColor(kBlue);
  h020X->SetStats(kFALSE);
  h020X->Draw();
  tmpCanvas->cd(4);
  mpsChain->Draw("sca_bpm_3c20Y:mps_counter>>h020Y","compton_charge>25","][sames");
  canvas->cd(4);
  h020Y = (TH2F*)gDirectory->Get("h020Y");
  h020Y->SetMarkerColor(kRed);
  h020Y->SetStats(kFALSE);
  dy = (maxBPMY-minBPMY)/.8;
  dx = (maxMps-minMps)/.8;
  p020Y->Range(minMps-.1*dx,minBPMY-.1*dy,maxMps+.1*dx,maxBPMY+.1*dy);
  p020Y->Draw();
  p020Y->cd();
  //h020Y->Draw();//"][sames");
  h020Y->Draw("][sames");
  p020Y->Update();
  a020 = new TGaxis(maxMps,minBPMY,maxMps,maxBPMY,minBPMY,maxBPMY,50510,"L+");
  a020->SetLabelColor(kRed);
  a020->SetTitleColor(kRed);
  a020->SetTitle("BPM 3C020Y (mm)");
  a020->Draw();

  // Plot the BCM's
  TH2F *hBCM1 = new TH2F("hBCM1",
      "Current vs ps Counter;Mps Counter;Current ({\\mu}A)",
      500,minMps,maxMps,500,0.,200.);
  TH2F *hBCM2 = new TH2F("hBCM2",
      "Current vs ps Counter;Mps Counter;Current ({\\mu}A)",
      500,minMps,maxMps,500,0.,200.);
  TH2F *hBCM6 = new TH2F("hBCM6",
      "Current vs ps Counter;Mps Counter;Current ({\\mu}A)",
      500,minMps,maxMps,500,0.,200.);
  TH2F *hComptonCharge = new TH2F("hComptonCharge",
      "Current vs ps Counter;Mps Counter;Current ({\\mu}A)",
      500,minMps,maxMps,500,0.,200.);

  // Now plot all the stuff out
  canvas->cd(5);
  mpsChain->Draw("sca_bcm1:mps_counter>>hBCM1");
  hBCM1->SetMarkerColor(kBlue);
  hBCM1->SetStats(kFALSE);
  mpsChain->Draw("sca_bcm2:mps_counter>>hBCM2");
  hBCM2->SetMarkerColor(kRed);
  hBCM2->SetStats(kFALSE);
  mpsChain->Draw("sca_bcm6:mps_counter>>hBCM6");
  hBCM6->SetMarkerColor(kGreen);
  hBCM6->SetStats(kFALSE);
  mpsChain->Draw("compton_charge:mps_counter>>hComptonCharge");
  hComptonCharge->SetMarkerColor(kBlack);
  hComptonCharge->SetStats(kFALSE);


  //TPad *pBCM = new TPad("pBCM","",0,0,1,1);
  TPad *pBCM  = (TPad*)canvas->GetPad(5);
  pBCM->SetFillColor(0);
  pBCM->Draw();
  hBCM1->Draw();
  hBCM2->Draw("same");
  hBCM6->Draw("same");
  hComptonCharge->Draw("same");

  // Save it!
  canvas->SaveAs(canvas1);

  if ( deleteOnExit == kTRUE ) {
    delete mpsChain;
    delete canvas;
  }

  return 0;

}

void ExpandBPMRange( Double_t *min, Double_t *max )
{
  TRandom r;
  TTime t;
  Double_t rmin, rmax;
  r.SetSeed((ULong_t)t+gRandomSeed);
  rmax = r.Uniform(1.0,1.5);
  gRandomSeed += TMath::Abs(rmax);
  *max += rmax;
  r.SetSeed((ULong_t)t+gRandomSeed);
  rmin = -r.Uniform(.25,1.5);
  gRandomSeed += TMath::Abs(rmin);
  *min += rmin;
}
