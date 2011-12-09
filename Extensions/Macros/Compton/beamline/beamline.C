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
void ExpandBPMRange(Double_t *min, Double_t *max, Bool_t isX = kFALSE);
void PlotBPM(TString name,TString prefix,TChain *chain,Double_t minMps, Double_t maxMps,
    TCanvas *canvas, TCanvas *tmpCanvas,Int_t index);

Int_t beamline(Int_t runnum, Bool_t isFirst100k = kFALSE, Bool_t deleteOnExit = kFALSE)
{
  gROOT->Reset();

  // Get the chain
  TChain *helChain = getHelChain(runnum,isFirst100k);
  if (!helChain) {
    std::cout << "What a scam!! We didn't even get data!" << std::endl;
    return -1;
  }

  // Prepare the canvas
  TCanvas *canvas = new TCanvas("BeamLineCanvas","Beam Line Canvas",800,2048);
  canvas->Divide(1,9);
  canvas->SetFillColor(0);
  TCanvas *tmpCanvas = new TCanvas("TmpBeamLineCanvas","Beam Line Canvas",1024,1600);
  tmpCanvas->Divide(1,9);

  // Prepare the web url
  TString www = TString(getenv("QWSCRATCH")) + Form("/www/run_%d/",runnum);
  gSystem->mkdir(www,true);
  TString canvas1 = www+"beamline_plots.png";

  // Record Extremeties
  Double_t maxMps, minMps;
  maxMps = helChain->GetMaximum("mps_counter");
  minMps = helChain->GetMinimum("mps_counter");

  PlotBPM("3p02a","diff",helChain,minMps,maxMps,canvas,tmpCanvas,1);
  PlotBPM("3p02a","yield",helChain,minMps,maxMps,canvas,tmpCanvas,2);
  PlotBPM("3p02b","diff",helChain,minMps,maxMps,canvas,tmpCanvas,3);
  PlotBPM("3p02b","yield",helChain,minMps,maxMps,canvas,tmpCanvas,4);
  PlotBPM("3p03a","diff",helChain,minMps,maxMps,canvas,tmpCanvas,5);
  PlotBPM("3p03a","yield",helChain,minMps,maxMps,canvas,tmpCanvas,6);
  PlotBPM("3c20","diff",helChain,minMps,maxMps,canvas,tmpCanvas,7);
  PlotBPM("3c20","yield",helChain,minMps,maxMps,canvas,tmpCanvas,8);

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
  canvas->cd(9);
  helChain->Draw("yield_sca_bcm1:mps_counter>>hBCM1");
  hBCM1->SetMarkerColor(kBlue);
  hBCM1->SetStats(kFALSE);
  helChain->Draw("yield_sca_bcm2:mps_counter>>hBCM2");
  hBCM2->SetMarkerColor(kRed);
  hBCM2->SetStats(kFALSE);
  helChain->Draw("yield_sca_bcm6:mps_counter>>hBCM6");
  hBCM6->SetMarkerColor(kGreen);
  hBCM6->SetStats(kFALSE);
  helChain->Draw("yield_compton_charge:mps_counter>>hComptonCharge");
  hComptonCharge->SetMarkerColor(kBlack);
  hComptonCharge->SetStats(kFALSE);

  //TPad *pBCM = new TPad("pBCM","",0,0,1,1);
  TPad *pBCM  = (TPad*)canvas->GetPad(9);
  pBCM->SetFillColor(0);
  pBCM->Draw();
  hBCM1->Draw();
  hBCM2->Draw("same");
  hBCM6->Draw("same");
  hComptonCharge->Draw("same");

  // Save it!
  canvas->SaveAs(canvas1);

  if ( deleteOnExit == kTRUE ) {
    delete helChain;
    delete canvas;
  }

  return 0;

}

void ExpandBPMRange( Double_t *min, Double_t *max, Bool_t isX )
{
  Int_t exp;
  Double_t maxScale, minScale;
  frexp(*max,&exp);
  maxScale = pow(10.,exp<0?0:exp+1);
  maxScale = maxScale>1?1:maxScale;
  frexp(*min,&exp);
  minScale = pow(10.,exp<0?0:exp+1);
  minScale = minScale>1?1:minScale;
  Double_t maxRange[4] = {0.10,0.20,0.20,0.30};
  Double_t minRange[4] = {0.20,0.10,0.30,0.20};
  Int_t index = 0;
  if( !isX )
    index = 1;
  TRandom r;
  TTime t;
  Double_t rmin, rmax;
  r.SetSeed((ULong_t)t+gRandomSeed);
  rmax = r.Uniform(maxScale*maxRange[index],maxScale*maxRange[index+2]);
  gRandomSeed += TMath::Abs(rmax);
  *max += rmax;
  r.SetSeed((ULong_t)t+gRandomSeed);
  rmin = -r.Uniform(minScale*minRange[index],minScale*minRange[index+2]);
  gRandomSeed += TMath::Abs(rmin);
  *min += rmin;
}

void PlotBPM(TString name, TString prefix, TChain *chain,Double_t minMps, Double_t maxMps,
    TCanvas *canvas, TCanvas *tmpCanvas,Int_t index)
{
  // Prepare the strings
  TString desc;
  if( prefix == "yield" ) {
    desc = "Positions";
  } else {
    desc = "Differences";
  }
  TString padXName = Form("p%sX",name.Data());
  TString padYName = Form("p%sY",name.Data());
  TString histXName = "h"+prefix+name+"X";
  TString histYName = "h"+prefix+name+"Y";
  TString diffLeafXName = Form("%s_sca_bpm_%sX/value",prefix.Data(),name.Data());
  TString diffLeafYName = Form("%s_sca_bpm_%sY/value",prefix.Data(),name.Data());
  TString diffPlotX = Form("%s_sca_bpm_%sX.value:mps_counter>>%s",
      prefix.Data(),name.Data(),histXName.Data());
  TString diffPlotY = Form("%s_sca_bpm_%sY.value:mps_counter>>%s",
      prefix.Data(),name.Data(),histYName.Data());

  // Plot BPM
  TPad *padX = new TPad(padXName,"",0,0,1,1);
  TPad *padY = new TPad(padXName,"",0,0,1,1);
  padY->SetFillStyle(4000);
  TGaxis *yAxis;

  Double_t minBPMX,maxBPMX,minBPMY,maxBPMY;
  Double_t dx, dy;
  minBPMX = chain->GetMinimum(diffLeafXName);
  maxBPMX = chain->GetMaximum(diffLeafXName);
  minBPMY = chain->GetMinimum(diffLeafYName);
  maxBPMY = chain->GetMaximum(diffLeafYName);

  ExpandBPMRange(&minBPMX,&maxBPMX,kTRUE);
  ExpandBPMRange(&minBPMY,&maxBPMY,kFALSE);

  name.ToUpper();
  TH2F *hX = new TH2F(histXName,
      "BPM "+desc+" "+name+"X;Mps Counter;BPM "+desc+" "+name+"X (mm)",
      500,minMps,maxMps,500,minBPMX,maxBPMX);
  TH2F *hY = new TH2F(histYName,
      "BPM "+desc+" "+name+"Y;Mps Counter;BPM "+desc+" "+name+"Y (mm)",
      500,minMps,maxMps,500,minBPMY,maxBPMY);

  // Now plot all the stuff out
  tmpCanvas->cd(index);
  padX->SetFillColor(0);
  chain->Draw(diffPlotX,"yield_compton_charge>25");
  canvas->cd(index);
  hX = (TH2F*)gDirectory->Get(histXName);
  padX->Draw();
  padX->cd();
  hX->SetMarkerColor(kBlue);
  hX->SetStats(kFALSE);
  hX->GetYaxis()->SetNoExponent();
  hX->Draw();
  tmpCanvas->cd(index);
  chain->Draw(diffPlotY,"yield_compton_charge>25","][sames");
  hY = (TH2F*)gDirectory->Get(histYName);
  hY->SetMarkerColor(kRed);
  hY->SetStats(kFALSE);
  canvas->cd(index);
  dy = (maxBPMY-minBPMY)/.8;
  dx = (maxMps-minMps)/.8;
  padY->Range(minMps-.1*dx,minBPMY-.1*dy,maxMps+.1*dx,maxBPMY+.1*dy);
  padY->Draw();
  padY->cd();
  hY->Draw("][sames");
  padY->Update();
  yAxis = new TGaxis(maxMps,minBPMY,maxMps,maxBPMY,minBPMY,maxBPMY,50510,"L+");
  yAxis->SetLabelColor(kRed);
  yAxis->SetTitleColor(kRed);
  yAxis->SetTitle("BPM "+desc+" "+name+" (mm)");
  yAxis->SetNoExponent();
  yAxis->Draw();
}
