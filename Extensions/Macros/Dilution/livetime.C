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

  TString eventcut = Form("CodaEventType==1 && CodaEventNumber>%i && CodaEventNumber<%i",lowcut,highcut);

  TString cuts = mycut.IsNull() ? "&&" + eventcut :"&&"+ eventcut + "&&"+mycut;

  //open the filename
  Char_t filename[100];
  sprintf(filename, "QweakNew_%i.root", runNum);
  //  sprintf(filename, "Qweak_%i.000.trees.root", runNum);
  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<std::endl;
    return;
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<std::endl;

  TTree *mps_tree = (TTree*) file->Get("Mps_Tree");
  mps_tree->SetAlias("aRatio","sca_livetime.value/sca_totaltime.value");

  //create histograms 
  TH1F *live = new TH1F("live","title",400,-200e3,200e3);
  TH1F *total = new TH1F("total","title",400,-200e3,200e3);
  TH1F *ratio = new TH1F("ratio","title",300,0.65,0.95);
  TH2F *ratioTime = new TH2F("ratioTime","title",1000,lowcut,highcut,300,0.70,1.00);

  TCanvas *c1 = new TCanvas("c1");
  c1->cd();
  gPad->SetLogy();
  mps_tree->Draw("sca_livetime.value>>live",Form("sca_livetime.value!=0 %s",cuts.Data()));
  live->SetTitle(Form("Run %i: Livetime {events>%i && events<%i}",runNum,lowcut,highcut));
  live->GetXaxis()->SetTitle("sca_livetime (counts)");
  live->SetMarkerColor(kBlue+2);
  live->Draw();

  TCanvas *c2 = new TCanvas("c2");
  c2->cd();
  gPad->SetLogy();
  mps_tree->Draw("sca_totaltime.value>>total",Form("sca_totaltime.value!=0 %s",cuts.Data()));
  total->SetTitle(Form("Run %i: Totaltime {events>%i && events<%i}",runNum,lowcut,highcut));
  total->GetXaxis()->SetTitle("sca_totaltime (counts)");
  total->SetMarkerColor(kBlue+2);
  total->Draw();

  TCanvas *c3 = new TCanvas("c3");
  c3->cd();
  mps_tree->Draw("aRatio>>ratio",Form("sca_totaltime.value!=0 && sca_livetime.value!=0 && aRatio<1.0 && aRatio>0.65 %s",cuts.Data()));
  ratio->SetTitle(Form("Run %i: Livetime/Total time {events>%i && events<%i}",runNum,lowcut,highcut));
  ratio->GetXaxis()->SetTitle("Calculated livetime(unitless)");
  ratio->SetMarkerColor(kBlue+2);
  ratio->Draw();

  TCanvas *c4 = new TCanvas("c4");
  c4->cd();
  mps_tree->Draw("aRatio:CodaEventNumber>>ratioTime",Form("sca_totaltime.value!=0 && sca_livetime.value!=0 && aRatio<1.0 && aRatio>0.65 %s",cuts.Data()));
  ratioTime->SetTitle(Form("Run %i: Livetime/Total time {events>%i && events<%i}",runNum,lowcut,highcut));
  ratioTime->GetXaxis()->SetTitle("CodaEventNumber");
  ratioTime->GetYaxis()->SetTitle("Calculated livetime (unitless)");
  ratioTime->SetMarkerColor(kBlue+2);
  ratioTime->Draw();


} //end livetime function



