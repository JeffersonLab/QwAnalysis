#include <TChain.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TString.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TAxis.h"
#include "TFile.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH1D.h"

void plotAverageCoefficientDipoles(int start = 139, int end = 321){
  const int nDET = 9, nMOD = 5;
  char* modType[nMOD] = {"X","Y","E","XP","YP"};
  int minRunsPerSlug = 4;
  TChain *slopes = new TChain("slopes");
  slopes->Add(Form("%s/../MacrocycleSlopesTree_ChiSqMin.set0.root",gSystem->Getenv("BMOD_OUT")));
  TString MonitorList[nMOD];
  TString DetectorList[29];
  ifstream file(Form("%s/config/setup_mpsonly.config",gSystem->Getenv("BMOD_SRC")));
  double mean[nMOD][nDET], err[nMOD][nDET];
  double detector[nDET]={0,1,2,3,4,5,6,7,8}, detectorErr[nDET] ={0,0,0,0,0,0,0,0,0};
  char  mon[255], monitr[255], det[255], detectr[255];
  string line;
  for(int i=0;i<5;i++){
    file>>mon>>monitr;
    getline(file, line);
    MonitorList[i] = TString(monitr);
    MonitorList[i].Replace(0,4,"");
    cout<<MonitorList[i]<<endl;
  }
  getline(file, line);
  for(int i=0;i<29;i++){
    file>>det>>detectr;
    getline(file, line);
    DetectorList[i] = TString(detectr);
    if(!DetectorList[i].Contains("uslumi"))
      DetectorList[i].Replace(0,4,"");
    cout<<i<<" "<<DetectorList[i]<<endl;
  }
  gStyle->SetOptFit(1111);
  gStyle->SetTitleSize(0.9);
  gStyle->SetTitleFontSize(0.1);
  gStyle->SetTitleTextColor(kBlack);
  TCanvas *c[2];
  c[0] = new TCanvas("cSin","cSin", 0,0,1900,1000);
  c[1] = new TCanvas("cCos","cCos", 0,0,1900,1000);
  TGraphErrors *gr[2*nMOD];
  c[0]->Divide(3,2);
  c[1]->Divide(3,2);
  int id = 0;
  for(int imod=0;imod<nMOD*2;imod++){
    for(int idet=0;idet<nDET;idet++){
      c[(imod>4 ? 1 : 0)]->cd((imod>4 ? imod-4 : imod+1));
      slopes->Draw(Form("%s_Coil%i>>h%i",
			  DetectorList[idet].Data(),imod, id),
		   Form("slug>=%i&&slug<=%i&&good",start,end),"goff");
      TH1D *h = (TH1D*)gDirectory->Get(Form("h%i",id));
      if(h->GetEntries()>=minRunsPerSlug){
	mean[imod][idet] = h->GetMean();
	err[imod][idet] = h->GetMeanError();
      }
      id++;
      delete h;
    } 
    gr[imod] = new TGraphErrors(nDET,detector,mean[imod],detectorErr,err[imod]);
    gr[imod]->Draw("ap");
    gr[imod]->SetTitle(Form("%s Response to Coil %i (%smod)",
			    (imod>4 ? "Cosine":"Sine"),(imod%5), modType[(imod%5)]));
    gr[imod]->GetYaxis()->SetTitle(Form("Response to Coil%i (ppm)",imod));
    gr[imod]->GetXaxis()->SetTitle("Main Detector Bar");
    double size = 0.049;
    gr[imod]->GetXaxis()->SetTitleSize(size);
    gr[imod]->GetYaxis()->SetTitleSize(size);
    //       gr[imod]->GetXaxis()->SetTitleColor(col[idet]);
    //       gr[imod]->GetYaxis()->SetTitleColor(col[idet]);
    gr[imod]->GetXaxis()->SetTitleOffset(0.85);
    gr[imod]->GetYaxis()->SetTitleOffset(0.85);
    //      gr[imod]->GetYaxis()->SetRangeUser(-10,10);
    gr[imod]->SetMarkerStyle(20);
    gr[imod]->SetMarkerColor((imod>4 ? kBlue : kRed));
    gr[imod]->SetLineColor((imod>4 ? kBlue : kRed));
    gr[imod]->Draw("ap");
    gr[imod]->Fit("pol0");
  }
}


