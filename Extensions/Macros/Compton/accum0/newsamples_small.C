#include "TStopwatch.h"
#include "iostream.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TString.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TFile.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TMath.h"
#include <vector>
#include "TPaveText.h"
#include "TLegend.h"
#include "TPaveStats.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TCint.h"
#include "TSystem.h"
#include <iostream>
#include <stdio.h>
#include "/home/cdaq/compton/QwAnalysis/Analysis/include/QwSIS3320_Samples.h"

Int_t newsamples_small(Int_t runnum, Int_t segnum = 0)
{
  const Int_t debug=0;

  TStopwatch timer;
  TString filename = Form("/home/cdaq/compton/QwScratch/rootfiles/"
                          "Compton_Pass1_%i.%03i.root",
			  runnum,segnum);

  printf("opening %s\n",filename.Data());
  TFile *file = TFile::Open(filename.Data());
  if(file == 0){
    printf("Searching local directory.\n");
    TString filename = Form("Compton_Pass1_%i.%03i.root",
			    runnum,segnum);
    file = TFile::Open(filename.Data());
  }
  if(file == 0){
    printf("File does not exist.\n");
    return 0;
  }
  TTree *mpsTree = (TTree*)file->Get("Mps_Tree");

  std::vector<QwSIS3320_Samples>* sampled_events = 0;
  mpsTree->SetBranchAddress("fadc_compton_samples",&sampled_events);

  Double_t sca_laser_PowT;
  mpsTree->SetBranchAddress("sca_laser_PowT",&sca_laser_PowT);

  Double_t sampxmax = 139;
  Double_t sampxmin = -11;
  Int_t sampxnbins = 150;
  Double_t maxmax = 1000;
  Double_t maxmin = -10;
  Int_t maxnbins = 101;
  Double_t minmax = 100;
  Double_t minmin = -100;
  Int_t minnbins = 100;

  TH1F *samp_laserON_pedestal = new TH1F("samp_laserON_pedestal","laser on sample pedestal",100,0,100);
  TH1F *samp_laserON_integral = new TH1F("samp_laserON_integral","laser on sample integral",100,0,10000);
  TH1F *samp_laserON_max = new TH1F("samp_laserON_max","laser on sample max",maxnbins,maxmin,maxmax);
  TH1F *samp_laserON_maxpos = new TH1F("samp_laserON_maxpos","laser on sample max position",sampxnbins,sampxmin,sampxmax);
  TH1F *samp_laserON_min = new TH1F("samp_laserON_min","laser on sample min",minnbins,minmin,minmax);
  TH1F *samp_laserON_minpos = new TH1F("samp_laserON_minpos","laser on sample min position",sampxnbins,sampxmin,sampxmax);

  TH1F *samp_laserOFF_pedestal = new TH1F("samp_laserOFF_pedestal","laser off sample pedestal",200,0,100);
  TH1F *samp_laserOFF_integral = new TH1F("samp_laserOFF_integral","laser off sample integral",200,0,10000);
  TH1F *samp_laserOFF_max = new TH1F("samp_laserOFF_max","laser off sample max",maxnbins,maxmin,maxmax);
  TH1F *samp_laserOFF_maxpos = new TH1F("samp_laserOFF_maxpos","laser off sample max position",sampxnbins,sampxmin,sampxmax);
  TH1F *samp_laserOFF_min = new TH1F("samp_laserOFF_min","laser off sample min",minnbins,minmin,minmax);
  TH1F *samp_laserOFF_minpos = new TH1F("samp_laserOFF_minpos","laser off sample min position",sampxnbins,sampxmin,sampxmax);

  TH2F *samp_laserON_maxVSmaxpos = new TH2F("samp_laserON_maxVSmaxpos","laser on sample max vs max position",
					    sampxnbins,sampxmin,sampxmax,maxnbins,maxmin,maxmax);
  TH2F *samp_laserOFF_maxVSmaxpos = new TH2F("samp_laserOFF_maxVSmaxpos","laser off sample max vs max position",
					     sampxnbins,sampxmin,sampxmax,maxnbins,maxmin,maxmax);
  TH2F *samp_laserON_minVSminpos = new TH2F("samp_laserON_minVSminpos","laser on sample min vs min position",
					    sampxnbins,sampxmin,sampxmax,minnbins,minmin,minmax);
  TH2F *samp_laserOFF_minVSminpos = new TH2F("samp_laserOFF_minVSminpos","laser off sample min vs min position",
					     sampxnbins,sampxmin,sampxmax,minnbins,minmin,minmax);

  Int_t nentries = mpsTree->GetEntries();
  printf("This run segment has %d  entries.\n", nentries);
  TCanvas *c_snap = new TCanvas("c_snap","A snapshot",0,0,640,480);

  //  Int_t warningnum_accum=0;
  Int_t warningnum_samp=0;
  //  Int_t numsamp=0;
  Bool_t laseron, laseroff, sampleinterest;
  printf("  %s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n","jentry",
	 "samp_max", "samp_maxpos", "samp_min", "samp_minpos", "samp_pedestal", 
	 "samp_integral", "sca_laser_PowT", "laseron", "laseroff");

  Double_t scale;
  for (Int_t jentry=0; jentry<nentries; jentry++) {
    // for (Int_t jentry=0; jentry<10000; jentry++) {


    mpsTree->GetEntry(jentry);
    laseron = sca_laser_PowT>250;
    laseroff = sca_laser_PowT<1;
    sampleinterest=0;
    if (sampled_events->size() <= 0) { // Did we get samples?
      warningnum_samp++;
      if (debug>0) printf("Warning:%4i: no samples for event %i\n",warningnum_samp,jentry);
    } else {
      Double_t samp_max = sampled_events->at(0).GetMaxSample();
      Double_t samp_maxpos = sampled_events->at(0).GetMaxIndex();
      Double_t samp_min = sampled_events->at(0).GetMinSample();
      Double_t samp_minpos = sampled_events->at(0).GetMinIndex();
      Double_t samp_pedestal = sampled_events->at(0).GetPedestal();
      Double_t samp_integral = sampled_events->at(0).GetSum();
      if (jentry%1000 == 0 || debug>0) {
	printf("%8i\t%10.2f\t%10.2f\t%10.2f\t%10.2f\t%10.2f\t%10.2f\t%10.2f\t%i\t%i\n",jentry,
	       samp_max, samp_maxpos, samp_min, samp_minpos, samp_pedestal, samp_integral, sca_laser_PowT, laseron, laseroff);
      }
      if (laseron) {
	samp_laserON_pedestal->Fill(samp_pedestal);
	samp_laserON_integral->Fill(samp_integral);
	samp_laserON_max->Fill(samp_max);
	samp_laserON_maxpos->Fill(samp_maxpos);
	samp_laserON_maxVSmaxpos->Fill(samp_maxpos,samp_max);
	samp_laserON_min->Fill(samp_min);
	samp_laserON_minpos->Fill(samp_minpos);
	samp_laserON_minVSminpos->Fill(samp_minpos,samp_min);
      } else {
	if (laseroff) {
	  samp_laserOFF_pedestal->Fill(samp_pedestal);
	  samp_laserOFF_integral->Fill(samp_integral);
	  samp_laserOFF_max->Fill(samp_max);
	  samp_laserOFF_maxpos->Fill(samp_maxpos);
	  samp_laserOFF_maxVSmaxpos->Fill(samp_maxpos,samp_max);
	  samp_laserOFF_min->Fill(samp_min);
	  samp_laserOFF_minpos->Fill(samp_minpos);
	  samp_laserOFF_minVSminpos->Fill(samp_minpos,samp_min);
	} else {
	  if (debug>0) printf("Event %6i laser indeterm %f.\n",jentry,sca_laser_PowT);
	}
      }
      if (samp_max>1500) {
	sampleinterest=1;
      }
      if (sampleinterest) {
	TGraph* graph = (TGraph*)sampled_events->at(0).GetGraph();
	graph->Draw("AL");
	c_snap->Print(Form("plots/samp_%i.%03i.%07i.png",runnum,segnum,jentry));
      }
    }
  }
  TCanvas *c_samples = new TCanvas("c_samples","c_samples",0,0,820,820);
  c_samples->Divide(2,3,0.001,0.001);

  samp_laserON_pedestal->SetLineColor(kRed);
  samp_laserON_max->SetLineColor(kRed);
  samp_laserON_integral->SetLineColor(kRed);
  samp_laserON_maxpos->SetLineColor(kRed);
  samp_laserON_minpos->SetLineColor(kRed);
  samp_laserON_min->SetLineColor(kRed);
  samp_laserOFF_pedestal->SetLineColor(kBlue);
  samp_laserOFF_max->SetLineColor(kBlue);
  samp_laserOFF_integral->SetLineColor(kBlue);
  samp_laserOFF_maxpos->SetLineColor(kBlue);
  samp_laserOFF_minpos->SetLineColor(kBlue);
  samp_laserOFF_min->SetLineColor(kBlue);
  c_samples->cd(1);
  TPaveStats *st1, *st2, *st3, *st4, *st5, *st6;
  scale = 0.9*samp_laserON_pedestal->GetEntries()/samp_laserOFF_pedestal->GetEntries();
  if(scale<1){
    samp_laserOFF_pedestal->SetTitle("Sample Pedestal: Laser Off/(On Scaled)");
    samp_laserOFF_pedestal->Draw();
    gPad->Update();
    st1 = (TPaveStats*)gPad->GetPrimitive("stats");
    st1->SetY1NDC(0.8); //new y start position
    st1->SetY2NDC(0.86); //new y end position
    samp_laserON_pedestal->Scale(1/scale);
    samp_laserON_pedestal->Draw("sames");
  }else{
    samp_laserON_pedestal->SetTitle("Sample Pedestal for Laser On/(Off Scaled)");
    samp_laserON_pedestal->Draw();
    gPad->Update();
    st1 = (TPaveStats*)gPad->GetPrimitive("stats");
    st1->SetY1NDC(0.663); //new y start position
    st1->SetY2NDC(0.833); //new y end position
    //    st1->SetX1NDC(0.58); //new x start position
    //    st1->SetX2NDC(0.78); //new x end position
    samp_laserOFF_pedestal->Scale(scale);
    samp_laserOFF_pedestal->Draw("sames");
  }
  c_samples->cd(2);
  if(scale<1){
    samp_laserOFF_integral->SetTitle("Sample Integral for Laser Off/(On Scaled)");
    samp_laserOFF_integral->Draw();
    gPad->Update();
    st2 = (TPaveStats*)gPad->GetPrimitive("stats");
    st2->SetX1NDC(0.58); //new x start position
    st2->SetX2NDC(0.78); //new x end position
    samp_laserON_integral->Scale(1/scale);
    samp_laserON_integral->Draw("sames");
  }else{
    samp_laserON_integral->SetTitle("Sample Integral for Laser On/(Off Scaled)");
    samp_laserON_integral->Draw();
    gPad->Update();
    st2 = (TPaveStats*)gPad->GetPrimitive("stats");
    st2->SetX1NDC(0.58); //new x start position
    st2->SetX2NDC(0.78); //new x end position
    samp_laserOFF_integral->Scale(scale);
    samp_laserOFF_integral->Draw("sames");
  }
  c_samples->cd(3);
  if(scale<1){
    samp_laserOFF_maxpos->SetTitle("Sample Max Position for Laser Off/(On Scaled)");
    st3 = (TPaveStats*)gPad->GetPrimitive("stats");
    samp_laserOFF_maxpos->Draw();
    gPad->Update();
    st3->SetX1NDC(0.58); //new x start position
    st3->SetX2NDC(0.78); //new x end position
    samp_laserON_maxpos->Scale(1/scale);
    samp_laserON_maxpos->Draw("sames");
  }else{
    samp_laserON_maxpos->SetTitle("Sample Max Position for Laser On/(Off Scaled)");
    samp_laserON_maxpos->Draw();
    gPad->Update();
    st3 = (TPaveStats*)gPad->GetPrimitive("stats");
    st3->SetX1NDC(0.58); //new x start position
    st3->SetX2NDC(0.78); //new x end position
    samp_laserOFF_maxpos->Scale(scale);
    samp_laserOFF_maxpos->Draw("sames");
  }
  c_samples->cd(4);
  if(scale<1){
    samp_laserOFF_max->SetTitle("Sample Max Spectra for Laser Off/(On Scaled)");
    samp_laserOFF_max->Draw();
    gPad->Update();
    st4 = (TPaveStats*)gPad->GetPrimitive("stats");
    st4->SetX1NDC(0.58); //new x start position
    st4->SetX2NDC(0.78); //new x end position
    samp_laserON_max->Scale(1/scale);
    samp_laserON_max->Draw("sames");
  }else{
    samp_laserON_max->SetTitle("Sample Max Spectra for Laser On/(Off Scaled)");
    samp_laserON_max->Draw();
    gPad->Update();
    st4 = (TPaveStats*)gPad->GetPrimitive("stats");
    st4->SetX1NDC(0.58); //new x start position
    st4->SetX2NDC(0.78); //new x end position
    samp_laserOFF_max->Scale(scale);
    samp_laserOFF_max->Draw("sames");
  }
  c_samples->cd(5);
  if(scale<1){
    samp_laserOFF_minpos->SetTitle("Sample Min Position for Laser Off/(On Scaled)");
    samp_laserOFF_minpos->Draw();
    gPad->Update();
    st5 = (TPaveStats*)gPad->GetPrimitive("stats");
    st5->SetX1NDC(0.58); //new x start position
    st5->SetX2NDC(0.78); //new x end position
    samp_laserON_minpos->Scale(1/scale);
    samp_laserON_minpos->Draw("sames");
  }else{
    samp_laserON_minpos->SetTitle("Sample Min Position for Laser On/(Off Scaled)");
    samp_laserON_minpos->Draw();
    gPad->Update();
    st5 = (TPaveStats*)gPad->GetPrimitive("stats");
    st5->SetX1NDC(0.58); //new x start position
    st5->SetX2NDC(0.78); //new x end position
    samp_laserOFF_minpos->Scale(scale);
    samp_laserOFF_minpos->Draw("sames");
  }
  c_samples->cd(6);
  if(scale<1){
    samp_laserOFF_min->SetTitle("Sample Min Spectra for Laser Off/(On Scaled)");
    samp_laserOFF_min->Draw();
    gPad->Update();
    st6 = (TPaveStats*)gPad->GetPrimitive("stats");
    st6->SetX1NDC(0.58); //new x start position
    st6->SetX2NDC(0.78); //new x end position
    samp_laserON_min->Scale(1/scale);
    samp_laserON_min->Draw("sames");
  }else{
    samp_laserON_min->SetTitle("Sample Min Spectra for Laser On/(Off Scaled)");
    samp_laserON_min->Draw();
    gPad->Update();
    st6 = (TPaveStats*)gPad->GetPrimitive("stats");
    st6->SetX1NDC(0.58); //new x start position
    st6->SetX2NDC(0.78); //new x end position
    samp_laserOFF_min->Scale(scale);
    samp_laserOFF_min->Draw("sames");
  }
  c_samples->Print(Form("plots/samplesummary_%i.png",runnum));
  c_samples->Print(Form("www/run_%i/samplesummary_%i.png",runnum,runnum));

  TCanvas *c_samples_corr = new TCanvas("c_samples_corr","c_samples_corr",0,0,820,820);
  c_samples_corr->Divide(2,2,0.001,0.001);
  c_samples_corr->cd(1);
  samp_laserOFF_maxVSmaxpos->Draw();
  c_samples_corr->cd(2);
  samp_laserON_maxVSmaxpos->Draw();
  c_samples_corr->cd(3);
  samp_laserOFF_minVSminpos->Draw();
  c_samples_corr->cd(4);
  samp_laserON_minVSminpos->Draw();
  c_samples_corr->Print(Form("plots/samplecorr_%i.png",runnum));


  Double_t fillTime = timer.CpuTime();
  printf("Time: %.1f s\tDone!\n",fillTime);
  return 1;
}
