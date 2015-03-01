#include "TString.h"
#include "TProfile.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include "TH1D.h"
#include "TH2D.h"
#include "TEventList.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TLeaf.h"
#include "TChain.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TDirectory.h"
#include "TPaveText.h"
#include "TPad.h"
#include "TFile.h"
#include "TTree.h"

int DrawFGvsRamp(int run){

  gStyle->SetTickLength(0.01,"X");
  gStyle->SetTickLength(0.01,"Y");
  TChain *ch = new TChain("mps_slug");
  TChain *ch1 = new TChain("mps_slug");
  ch1->Add(Form("%s/mps_only_friend_%i*.root",
		gSystem->Getenv("MPS_ONLY_ROOTFILES"),run));
  ch->Add(Form("%s/mps_only_%i_0*.root",
	       gSystem->Getenv("MPS_ONLY_ROOTFILES"),run));
  ch->Add(Form("%s/mps_only_%i_5*.root",
	       gSystem->Getenv("MPS_ONLY_ROOTFILES"),run));
  ch->Add(Form("%s/mps_only_%i_1*.root",
	       gSystem->Getenv("MPS_ONLY_ROOTFILES"),run));
  ch->Add(Form("%s/mps_only_%i_2*.root",
	       gSystem->Getenv("MPS_ONLY_ROOTFILES"),run));
  ch->Add(Form("%s/mps_only_%i_3*.root",
	       gSystem->Getenv("MPS_ONLY_ROOTFILES"),run));
  ch->Add(Form("%s/mps_only_%i_ful*.root",
	       gSystem->Getenv("MPS_ONLY_ROOTFILES"),run));
  
  ch->AddFriend(ch1);
  ch->SetMarkerColor(kRed);
  ch->SetLineColor(kRed);
  bool fge_exists  = (ch->GetLeaf("fge") ? 1 : 0);
  ch->Draw(">>list","ramp_good");
  TEventList *list = (TEventList*)gDirectory->Get("list");
  ch->SetEventList(list);
  //  ch->SetMarkerColor(kRed);
  ch->SetMarkerStyle(6);
  int offset = (ch->GetMaximum("bm_pattern_number")<10 ? 0 : 11 );
  TCanvas *c= new TCanvas("c","c",0,0,(fge_exists ? 1900 : 1600),700);
  TPaveText *pt = new TPaveText(0.8,0.85, 0.99,0.93,"ndc");
  pt->SetShadowColor(0);
  pt->SetFillColor(0);
  pt->SetTextColor(kRed);
  pt->AddText(Form("run %i",run));
  c->Divide((fge_exists? 5:4),2);
  c->cd(1);
  ch->Draw("fgx1:ramp_filled>>h1(300,-10,360)",
	   Form("bm_pattern_number==%i",offset));
  pt->Draw();
  c->cd(2);
  ch->Draw("fgx1:ramp_filled>>h2(300,-10,360)",
	   Form("bm_pattern_number==%i", 3+offset));
  pt->Draw();
  c->cd(3);
  ch->Draw("fgx2:ramp_filled>>h3(300,-10,360)",
	   Form("bm_pattern_number==%i", offset));
  pt->Draw();
  c->cd(4);
  ch->Draw("fgx2:ramp_filled>>h4(300,-10,360)",
	   Form("bm_pattern_number==%i", 3+offset));
  pt->Draw();
  c->cd(5+(int)fge_exists);
  ch->Draw("fgy1:ramp_filled>>h5(300,-10,360)",
	   Form("bm_pattern_number==%i", 1+offset));
  pt->Draw();
  c->cd(6+(int)fge_exists);
  ch->Draw("fgy1:ramp_filled>>h6(300,-10,360)",
	   Form("bm_pattern_number==%i", 4+offset));
  pt->Draw();
  c->cd(7+(int)fge_exists);
  ch->Draw("fgy2:ramp_filled>>h7(300,-10,360)",
	   Form("bm_pattern_number==%i", 1+offset));
  pt->Draw();
  c->cd(8+(int)fge_exists);
  ch->Draw("fgy2:ramp_filled>>h8(300,-10,360)",
	   Form("bm_pattern_number==%i", 4+offset));
  pt->Draw();
  if(fge_exists){
    c->cd(5);
    ch->Draw("fge:ramp_filled>>h9(300,-10,360)",
	     Form("bm_pattern_number==%i", 2+offset));
    pt->Draw();
  }

  c->SaveAs(Form("~/plots/ramp_plots/FG_vs_ramp_filled_run_%i.png", run));
  return 0;
}
