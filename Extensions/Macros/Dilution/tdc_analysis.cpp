//Script to set up the f1 tdc analysis, define useful variables, etc
//Written by J. Magee, 2011-12-06

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TStyle.h>

//void tdc_analysis(Int_t, Int_t, Int_t, Int_t, Int_t, TString )
//void tdc_prelim (Int_t, TString )

void tdc_analysis( Int_t runNum, Int_t tsLow=-2000, Int_t tsHigh=2000, Int_t mdLow=-2000, Int_t mdHigh=2000, TString mycut="" ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");

  TString cuts = mycut.IsNull() ? mycut : "&&"+mycut;

  Char_t filename[100];

  //open the filename
  sprintf(filename, "Qweak_%i.root", runNum);
  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    return;
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *event_tree = (TTree*) file->Get("event_tree");

  //setting aliases because I'm lazy
  event_tree->SetAlias("md1mt","(md1p_f1+md1m_f1)/2");
  event_tree->SetAlias("md5mt","(md5p_f1+md5m_f1)/2");

  TH1F * ts1 = new TH1F("ts1",Form("Trigger Scintillator 1 \{%s\}", cuts.Data()),2300,-900,1400);
  TH1F * ts2 = new TH1F("ts2",Form("Trigger Scintillator 2 \{%s\}", cuts.Data()),2300,-900,1400);

  TCanvas *trigscint = new TCanvas("trigscint");
  trigscint->Divide(1,2);

  trigscint->cd(1);
  gPad->SetLogy();
  event_tree->Draw("ts2mt_f1>>ts2",Form("ts2m_f1!=0 && ts2p_f1!=0 && ts2mt_f1>%i && ts2mt_f1<%i %s", tsLow, tsHigh, cuts.Data()));

  trigscint->cd(2);
  gPad->SetLogy();
  event_tree->Draw("ts1mt_f1>>ts1",Form("ts1m_f1!=0 && ts1p_f1!=0 && ts1mt_f1>%i && ts1mt_f1<%i %s", tsLow,tsHigh, cuts.Data()));


  TH1F * md1 = new TH1F("md1",Form("MD 1 \{%s\}", cuts.Data()),2300,-900,1400);
  TH1F * md5 = new TH1F("md5",Form("MD 5 \{%s\}", cuts.Data()),2300,-900,1400);

  TCanvas *maindet = new TCanvas("maindet");
  maindet->Divide(1,2);

  maindet->cd(1);
  gPad->SetLogy();
  event_tree->Draw("md1mt>>md1",Form("ts2m_f1!=0 && ts2p_f1!=0 && ts2mt_f1>%i && ts2mt_f1<%i && md1m_f1!=0 && md5p_f1!=0 && md1mt>%i && md1mt<%i %s", tsLow, tsHigh, mdLow, mdHigh, cuts.Data()));

  maindet->cd(2);
  gPad->SetLogy();
  event_tree->Draw("md5mt>>md5",Form("ts1m_f1!=0 && ts1p_f1!=0 && ts1mt_f1>%i && ts1mt_f1<%i && md5m_f1!=0 && md5p_f1!=0 && md5mt>%i && md5mt<%i %s", tsLow, tsHigh, mdLow, mdHigh, cuts.Data()));

/*

  std::cout <<"Looking at ts1/md5...\n";
//  std::cout <<"Looking at ts1...\n";
  TCanvas *ts1 = new TCanvas("ts1");
  gPad->SetLogy();
  event_tree->Draw("ts1mt_f1>>h1(12000)","ts1m_f1!=0 && ts1p_f1!=0");
  std::cout <<"TS1:\n"
    <<"\tts1.cd()\n"
    <<"\tevent_tree->Draw(\"ts1mt_f1>>h1(12000)\",\"ts1m_f1!=0 && ts1p_f1!=0 && ts1mt_f1>### && ts1mt_f1<###\")\n";

//  std::cout <<"Looking at ts1/md5...\n";
  TCanvas *md5 = new TCanvas("md5");
  md5->Divide(1,2);
  md5->cd(1);
  gPad->SetLogy();
//  event_tree->Draw("md5mt>>h2(12000)","md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0");
//  event_tree->Draw("md5mt>>h2(300)","md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0 && ts1mt_f1>-1191 && ts1mt_f1<-1165 && md5mt<-1400 && md5mt>-1700");
//  event_tree->Draw("md5mt>>h2(300)","md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0 && ts1mt_f1>-1191 && ts1mt_f1<-1165 && md5mt<-1400 && md5mt>-1700 && Coda_ScanData1==8921 && Coda_CleanData==1 && CodaEventNumber>2000e3 && CodaEventNumber<2400e3");
//  event_tree->Draw("md5mt>>h2(300)","md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0 && ts1mt_f1>-1191 && ts1mt_f1<-1165 && md5mt<-1400 && md5mt>-1700 && Coda_ScanData1==8921 && Coda_CleanData==1 && CodaEventNumber>22e3 && CodaEventNumber<460e3");
  event_tree->Draw("md5mt>>h2(300)","md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0 && ts1mt_f1>-140 && ts1mt_f1<-135 && md5mt<-175 && md5mt>-200");
  std::cout <<"MD5:\n"
    <<"\tmd5.cd(1)\n"
    <<"\tevent_tree->Draw(\"md5mt>>h2(12000)\",\"md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0 && ts1mt_f1<### && ts1mt_f1>### && md5mt<### && md5mt>###\")\n";

//  std::cout <<"Looking at noise of md5...\n";
  md5->cd(2);
  gPad->SetLogy();
//  event_tree->Draw("md5mt>>h3(12000)","md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0");
//  event_tree->Draw("md5mt>>h3(300)","md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0 && ts1mt_f1>-1191 && ts1mt_f1<-1165 && md5mt<-1000 && md5mt>-1300");
//  event_tree->Draw("md5mt>>h3(300)","md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0 && ts1mt_f1>-1191 && ts1mt_f1<-1165 && md5mt<-1000 && md5mt>-1300 && Coda_ScanData1==8921 && Coda_CleanData==1 && CodaEventNumber>2000e3 && CodaEventNumber<2400e3");
//  event_tree->Draw("md5mt>>h3(300)","md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0 && ts1mt_f1>-1191 && ts1mt_f1<-1165 && md5mt<-1000 && md5mt>-1300 && Coda_ScanData1==8921 && Coda_CleanData==1 && CodaEventNumber>22e3 && CodaEventNumber<460e3");
  event_tree->Draw("md5mt>>h3(300)","md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0 && ts1mt_f1>-140 && ts1mt_f1<-135 && md5mt<-140 && md5mt>-165");
  std::cout <<"MD5 noise:\n"
    <<"\tmd5.cd(2)\n" 
    <<"\tevent_tree->Draw(\"md5mt>>h3(12000)\",\"md5m_f1!=0 && md5p_f1!=0 && ts1p_f1!=0 && ts1m_f1!=0 && ts1mt_f1<### && ts1mt_f1>### && md5mt<### && md5mt>###\")\n"; 
  
  std::cout <<"Looking at ts2/md1...\n";
//  std::cout <<"Looking at ts2...\n";
  TCanvas *ts2 = new TCanvas("ts2");
  gPad->SetLogy();
  event_tree->Draw("ts2mt_f1>>g1(12000)","ts2m_f1!=0 && ts2p_f1!=0");
  std::cout <<"TS2:\n"
    <<"\tts2.cd()\n"
    <<"\tevent_tree->Draw(\"ts2mt_f1>>h2(12000)\",\"ts2m_f1!=0 && ts2p_f1!=0 && ts2mt_f1>### && ts2mt_f1<###\")\n";

//  std::cout <<"Looking at ts2/md1...\n";
  TCanvas *md1 = new TCanvas("md1");
  md1->Divide(1,2);
  md1->cd(1);
  gPad->SetLogy();
//  event_tree->Draw("md1mt>>g2(12000)","md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0");
//  event_tree->Draw("md1mt>>g2(325)","md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0 && ts2mt_f1>-1190 && ts2mt_f1<-1170 && md1mt>-1725 && md1mt<-1400");
//  event_tree->Draw("md1mt>>g2(325)","md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0 && ts2mt_f1>-1190 && ts2mt_f1<-1170 && md1mt>-1725 && md1mt<-1400 && Coda_ScanData1==8921 && Coda_CleanData==1 && CodaEventNumber>2000e3 && CodaEventNumber<2400e3");
//  event_tree->Draw("md1mt>>g2(325)","md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0 && ts2mt_f1>-1190 && ts2mt_f1<-1170 && md1mt>-1725 && md1mt<-1400 && Coda_ScanData1==8921 && Coda_CleanData==1 && CodaEventNumber>22e3 && CodaEventNumber<460e3");
  event_tree->Draw("md1mt>>g2(325)","md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0 && ts2mt_f1>-140 && ts2mt_f1<-135 && md1mt>-205 && md1mt<-170");
  std::cout <<"MD1:\n"
    <<"\tmd1.cd(1)\n"
    <<"\tevent_tree->Draw(\"md1mt>>g2(12000)\",\"md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0 && ts2mt_f1<### && ts2mt_f1>### && md1mt<### && md1mt>###\")\n";

//  std::cout <<"Looking at noise of md1...\n";
  md1->cd(2);
  gPad->SetLogy();
//  event_tree->Draw("md1mt>>g3(12000)","md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0");
//  event_tree->Draw("md1mt>>g3(325)","md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0 && ts2mt_f1>-1190 && ts2mt_f1<-1170 && md1mt<-900 && md1mt>-1225");
//  event_tree->Draw("md1mt>>g3(325)","md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0 && ts2mt_f1>-1190 && ts2mt_f1<-1170 && md1mt<-900 && md1mt>-1225 && Coda_ScanData1==8921 && Coda_CleanData==1 && CodaEventNumber>2000e3 && CodaEventNumber<2400e3");
//  event_tree->Draw("md1mt>>g3(325)","md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0 && ts2mt_f1>-1190 && ts2mt_f1<-1170 && md1mt<-900 && md1mt>-1225 && Coda_ScanData1==8921 && Coda_CleanData==1 && CodaEventNumber>22e3 && CodaEventNumber<460e3");
  event_tree->Draw("md1mt>>g3(325)","md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0 && ts2mt_f1>-140 && ts2mt_f1<-135 && md1mt<-120 && md1mt>-155");
  std::cout <<"MD1 noise:"
    <<"\tmd5.cd(2)\n"
    <<"\tevent_tree->Draw(\"md1mt>>g3(12000)\",\"md1m_f1!=0 && md1p_f1!=0 && ts2p_f1!=0 && ts2m_f1!=0 && ts2mt_f1<### && ts2mt_f1>### && md1mt<### && md1mt>###\")\n";
*/  
} //end set-up


void tdc_prelim (Int_t runNum, TString mycut = "") {
  gROOT->Reset();
  gROOT->SetStyle("Modern");

  TString cuts = mycut.IsNull() ? mycut : "&&"+mycut;

  Char_t filename[100];

  //open the filename
  sprintf(filename, "Qweak_%i.root", runNum);
  TFile *file = new TFile(filename);
  if ( !file->IsOpen() ) {
    std::cerr <<"Error opening ROOTFILE " <<file <<".\n" <<endl;
    return;
  }
  std::cout <<"Successfully opened ROOTFILE " <<filename <<".\n" <<endl;

  TTree *event_tree = (TTree*) file->Get("event_tree");

 //setting aliases because I'm lazy
  event_tree->SetAlias("md1mt","(md1p_f1+md1m_f1)/2");
  event_tree->SetAlias("md5mt","(md5p_f1+md5m_f1)/2");


//  std::cout <<"Drawing diagnostic plots...\n";
  TCanvas *rate = new TCanvas("rate");
  gPad->SetLogy();
  event_tree->Draw("sca_4MHz/4e6",Form("%s",mycut.Data()));

  TH1F * ts1 = new TH1F("ts1",Form("Trigger Scintillator 1 \{%s\}", cuts.Data()),2300,-900,1400);
  TH1F * ts2 = new TH1F("ts2",Form("Trigger Scintillator 2 \{%s\}", cuts.Data()),2300,-900,1400);

  TCanvas *trigscint = new TCanvas("trigscint");
  trigscint->Divide(1,2);

  trigscint->cd(1);
  gPad->SetLogy();
  event_tree->Draw("ts2mt_f1>>ts2",Form("ts2m_f1!=0 && ts2p_f1!=0 %s", cuts.Data()));

  trigscint->cd(2);
  gPad->SetLogy();
  event_tree->Draw("ts1mt_f1>>ts1",Form("ts1m_f1!=0 && ts1p_f1!=0 %s", cuts.Data()));


  TH1F * md1 = new TH1F("md1",Form("MD 1 \{%s\}", cuts.Data()),2300,-900,1400);
  TH1F * md5 = new TH1F("md5",Form("MD 5 \{%s\}", cuts.Data()),2300,-900,1400);

  TCanvas *maindet = new TCanvas("maindet");
  maindet->Divide(1,2);

  maindet->cd(1);
  gPad->SetLogy();
  event_tree->Draw("md1mt>>md1",Form("ts2m_f1!=0 && ts2p_f1!=0 && md1m_f1!=0 && md5p_f1!=0 %s", cuts.Data()));

  maindet->cd(2);
  gPad->SetLogy();
  event_tree->Draw("md5mt>>md5",Form("ts1m_f1!=0 && ts1p_f1!=0 && md5m_f1!=0 && md5p_f1!=0 %s", cuts.Data()));


} // end prelim
