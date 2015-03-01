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

  ch->Draw(">>list","ramp>10");
  TEventList *list = (TEvenList*)gDirectory->Get("list");
  ch->SetEventList(list);
  //  ch->SetMarkerColor(kRed);
  ch->SetMarkerStyle(6);
  TCanvas *c= new TCanvas("c","c",0,0,1000,1400);
  c->Divide(2,4);
  c->cd(1);
  ch->Draw("fgx1:ramp_filled","bm_pattern_number==11||bm_pattern_number==0");
  c->cd(2);
  ch->Draw("fgx1:ramp_filled","bm_pattern_number==14||bm_pattern_number==3");
  c->cd(3);
  ch->Draw("fgx2:ramp_filled","bm_pattern_number==11||bm_pattern_number==0");
  c->cd(4);
  ch->Draw("fgx2:ramp_filled","bm_pattern_number==14||bm_pattern_number==3");
  c->cd(5);
  ch->Draw("fgy1:ramp_filled","bm_pattern_number==12||bm_pattern_number==1");
  c->cd(6);
  ch->Draw("fgy1:ramp_filled","bm_pattern_number==15||bm_pattern_number==4");
  c->cd(7);
  ch->Draw("fgy2:ramp_filled","bm_pattern_number==12||bm_pattern_number==1");
  c->cd(8);
  ch->Draw("fgy2:ramp_filled","bm_pattern_number==15||bm_pattern_number==4");


}
