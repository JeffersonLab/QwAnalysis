#include <TChain.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TString.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH1D.h"

void plotSlugAveragedSlopes(int start = 139, int end = 321){
  const int nDET = 3, nMON = 5, nSLUG = 200;
  int minRunsPerSlug = 4;
  TChain *slopes = new TChain("slopes");
  slopes->Add(Form("%s/../slopesTree_ChiSqMin.set0.root",gSystem->Getenv("BMOD_OUT")));
  TString MonitorList[nMON];
  TString DetectorList[29];
  ifstream file(Form("%s/config/setup_mpsonly.config",gSystem->Getenv("BMOD_SRC")));
  double mean[nDET][nMON][nSLUG], err[nDET][nMON][nSLUG], slug[nSLUG], slugErr[nSLUG];
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
  gStyle->SetTitleSize(0.9);
  gStyle->SetTitleFontSize(0.1);
  gStyle->SetTitleTextColor(kBlack);
  TCanvas *c = new TCanvas("c","c", 0,0,1900,1000);
  TGraphErrors *gr[nDET*nMON];
  c->Divide(5,3);
  int dtc[3] = {9,23,24};//{19,18,20};//{1,0,3};
    int col[3]={1,2,4},nSlug = 0;
  for(int idet=0;idet<nDET;idet++){
    for(int imon=0;imon<nMON;imon++){
      c->cd(imon+1+nMON*idet);
      nSlug = 0;
      int idx = imon+nMON*idet;
      for(int slg = start;slg<=end;slg++){
	slopes->Draw(Form("%s_%s>>h%i",DetectorList[dtc[idet]].Data(),
			  MonitorList[imon].Data(), idx),
		     Form("slug==%f&&good&&%s",(double)slg,
			  (DetectorList[idet].Contains("ltg")
			   ?"pmtltg_targetX>-9998":"1")),"goff");
	TH1D *h = (TH1D*)gDirectory->Get(Form("h%i",idx));
	if(h->GetEntries()>=minRunsPerSlug){
	  mean[idet][imon][nSlug] = h->GetMean();
	  err[idet][imon][nSlug] = h->GetMeanError();
	  slug[nSlug] = slg;
	  slugErr[nSlug] = 0;
	  nSlug++;
	}
	delete h;
      }
      gr[idx] = new TGraphErrors(nSlug,slug,mean[idet][imon],slugErr,err[idet][imon]);
      gr[idx]->Draw("ap");
      gr[idx]->SetTitle(Form("Slope %s / %s",
			     DetectorList[dtc[idet]].Data(),
			     MonitorList[imon].Data()));
      gr[idx]->GetYaxis()->SetTitle(Form("%s/%s (%s)",
					 DetectorList[dtc[idet]].Data(),
					 MonitorList[imon].Data(),
					 (MonitorList[imon].Contains("Slope")
					  ? "ppm/#murad":"ppm/#mum")));
      gr[idx]->GetXaxis()->SetTitle("Slug");
      double size = 0.049;
      gr[idx]->GetXaxis()->SetTitleSize(size);
      gr[idx]->GetYaxis()->SetTitleSize(size);
//       gr[idx]->GetXaxis()->SetTitleColor(col[idet]);
//       gr[idx]->GetYaxis()->SetTitleColor(col[idet]);
      gr[idx]->GetXaxis()->SetTitleOffset(0.85);
      gr[idx]->GetYaxis()->SetTitleOffset(0.85);
      gr[idx]->SetMarkerStyle(7);
      gr[idx]->SetMarkerColor(col[idet]);
      gr[idx]->SetLineColor(col[idet]);
      gr[idx]->Draw("ap");
    }
  }

  gROOT->ForceStyle();
}
