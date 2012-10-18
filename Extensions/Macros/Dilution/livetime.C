//This script will produce some plots of computer livetime, to be used in the dilution analysis.
//Written by Josh Magee.
//magee@jlab.org
//July 6, 2012

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TStyle.h>
//#include <QwEvent.h>
#include <TPaveText.h>
#include <TPaveStats.h>

void livetime( Int_t runNum, Int_t lowcut=0, Int_t highcut=4e6, TString mycut = "") {
  gROOT->Reset();
  gROOT->SetStyle("Modern");

  //open the filename
  Char_t filename[100];
//  sprintf(filename, "QweakNew_%i.root", runNum);
//  sprintf(filename, "Qweak_%i.root", runNum);
//  sprintf(filename, "QweakMay11_%i.root", runNum);
  sprintf(filename, "QweakMarchGas11_%i.root", runNum);
  //  sprintf(filename, "Qweak_%i.000.trees.root", runNum);
  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<std::endl;
    return;
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<std::endl;

  //grab TTrees, set variables, and cuts
  TTree *mps_tree = (TTree*) file->Get("Mps_Tree");
  mps_tree->SetAlias("aRatio","sca_livetime.value/sca_totaltime.value");

  TTree *event_tree = (TTree*) file->Get("event_tree");
  Long_t num_entries = event_tree->GetEntries();

  highcut = num_entries<highcut ? num_entries : highcut;

  TString eventcut = Form("CodaEventType==1 && CodaEventNumber>%i && CodaEventNumber<%i",lowcut,highcut);
  TString cuts = mycut.IsNull() ? "&&" + eventcut :"&&"+ eventcut + "&&"+mycut;

  //create histograms 
//  TH1F *live = new TH1F("live","title",400,-200e3,200e3);
//  TH1F *total = new TH1F("total","title",400,-200e3,200e3);
  TH1F *live = new TH1F("live","title",400,-100e3,300e3);
  TH1F *total = new TH1F("total","title",400,-100e3,300e3);
  TH1F *ratio = new TH1F("ratio","title",300,0.75,0.95);
//  TH1F *ratio = new TH1F("ratio","title",300,0.3,0.85);
  TH2F *ratioTime = new TH2F("ratioTime","title",1000,lowcut,highcut,300,0.55,0.90);
  TH2F *compare = new TH2F("compare","title",140,-60e3,100e3,140,-60e3,100e3);


  TCanvas *c1 = new TCanvas("c1");
  c1->cd();
  gPad->SetLogy();
  mps_tree->Draw("sca_livetime.value>>live",Form("sca_livetime.value!=0 %s",cuts.Data()));
  live->SetTitle(Form("Run %i: Livetime scaler clock {events>%i && events<%i}",runNum,lowcut,highcut));
  live->GetXaxis()->SetTitle("sca_livetime (counts)");
  live->SetLineColor(kBlue+2);
  live->Draw();

  TCanvas *c2 = new TCanvas("c2");
  c2->cd();
  gPad->SetLogy();
  mps_tree->Draw("sca_totaltime.value>>total",Form("sca_totaltime.value!=0 %s",cuts.Data()));
  total->SetTitle(Form("Run %i: Totaltime scaler {events>%i && events<%i}",runNum,lowcut,highcut));
  total->GetXaxis()->SetTitle("sca_totaltime (counts)");
  total->SetLineColor(46);
  total->Draw();

  TCanvas *c3 = new TCanvas("c3");
  c3->cd();
//  mps_tree->Draw("aRatio>>ratio",Form("sca_totaltime.value!=0 && sca_livetime.value!=0 && aRatio<1.0 && aRatio>0.65 %s",cuts.Data()));
  mps_tree->Draw("aRatio>>ratio",Form("sca_totaltime.value!=0 && sca_livetime.value!=0 %s",cuts.Data()));
  ratio->SetTitle(Form("Run %i: Livetime/Total time {events>%i && events<%i}",runNum,lowcut,highcut));
  ratio->GetXaxis()->SetTitle("Calculated livetime(unitless)");
  ratio->SetLineColor(kBlue+2);
  ratio->Draw();
  c3->Print(Form("rootfile_plots/run%i/LTratio.png",runNum));

  TCanvas *c4 = new TCanvas("c4");
  c4->cd();
  mps_tree->Draw("aRatio:CodaEventNumber>>ratioTime",Form("sca_totaltime.value!=0 && sca_livetime.value!=0 && aRatio<1.0 && aRatio>0.65 %s",cuts.Data()));
  ratioTime->SetTitle(Form("Run %i: Livetime/Total time {events>%i && events<%i}",runNum,lowcut,highcut));
  ratioTime->GetXaxis()->SetTitle("CodaEventNumber");
  ratioTime->GetYaxis()->SetTitle("Calculated livetime (unitless)");
  ratioTime->SetMarkerColor(kBlue+2);
  ratioTime->Draw();
  c4->Print(Form("rootfile_plots/run%i/LTratiotime.png",runNum));

  //copy a few hists for fun
  TH1F *live2  = (TH1F*) live->Clone("live2");
  TH1F *total2 = (TH1F*) total->Clone("total2");

  TCanvas *c5 = new TCanvas("c5");
  c5->cd();
  gPad->SetLogy();
  live2->GetXaxis()->SetTitle("Livetime scaler (blue) and Total time scaler (red)");
  live2->SetLineColor(kBlue+2);
  live2->Draw();
  total2->SetLineColor(46);
  total2->Draw("sames");
  gPad->Update();
  c5->Print(Form("rootfile_plots/run%i/LTscalerclocks.png",runNum));
/*
  TCanvas *c6 = new TCanvas("c6");
  c6->cd();
  compare->SetStats(0);
  mps_tree->Draw("sca_livetime.value:sca_totaltime.value>>compare",
      Form("sca_livetime.value!=0 && sca_totaltime.value!=0 %s",cuts.Data()));
  compare->SetTitle(Form("Run %i: Livetime scalers vs. Totaltime scalers \
        {events>%i && events <%i}",runNum,lowcut,highcut));
  compare->GetXaxis()->SetTitle("sca_totaltime");
  compare->GetYaxis()->SetTitle("sca_livetime");
  compare->SetMarkerColor(kRed);
  compare->Draw("colz");
*/

} //end livetime function



