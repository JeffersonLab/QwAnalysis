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

void plotSlugAveragedCoilResiduals(int start = 139, int end = 321, int cos = 1){
  const int nDET = 3, nMOD = 5, nSLUG = 200;
  char* modType[nMOD] = {"X","Y","E","XP","YP"};
  int minRunsPerSlug = 4;
  TChain *slopes = new TChain("slopes");
  slopes->Add(Form("%s/../slopesTree_ChiSqMin.set0.root",gSystem->Getenv("BMOD_OUT")));
  TString MonitorList[nMOD];
  TString DetectorList[29];
  ifstream file(Form("%s/config/setup_mpsonly.config",gSystem->Getenv("BMOD_SRC")));
  double mean[nDET][nMOD][nSLUG], err[nDET][nMOD][nSLUG], slug[nSLUG], slugErr[nSLUG];
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
  TGraphErrors *gr[nDET*nMOD];
  c->Divide(5,3);
  int dtc[3] = {1,0,3};//{19,18,20};//{9,23,24}//{1,0,3};
    int col[3]={1,2,4},nSlug = 0;
  for(int idet=0;idet<nDET;idet++){
    for(int imod=0;imod<nMOD;imod++){
      c->cd(imod+1+nMOD*idet);
      nSlug = 0;
      int idx = imod+nMOD*idet;
      for(int slg = start;slg<=end;slg++){
	slopes->Draw(Form("%sRes_%s_Coil%i>>h%i",(cos ? "Cosine":"Sine"),
			  DetectorList[dtc[idet]].Data(),
			  imod, idx),
		     Form("slug==%f&&good",(double)slg),"goff");
	TH1D *h = (TH1D*)gDirectory->Get(Form("h%i",idx));
	if(h->GetEntries()>=minRunsPerSlug){
	  mean[idet][imod][nSlug] = h->GetMean();
	  err[idet][imod][nSlug] = h->GetMeanError();
	  slug[nSlug] = slg;
	  slugErr[nSlug] = 0;
	  nSlug++;
	}
	delete h;
      }
      gr[idx] = new TGraphErrors(nSlug,slug,mean[idet][imod],slugErr,err[idet][imod]);
      gr[idx]->Draw("ap");
      gr[idx]->SetTitle(Form("%s Residual %s / Coil%i (%smod)",
			     (cos ? "Cosine":"Sine"),
			     DetectorList[dtc[idet]].Data(),imod, modType[imod]));
      gr[idx]->GetYaxis()->SetTitle(Form("Residual %s/Coil%i (ppm)",
					 DetectorList[dtc[idet]].Data(),imod));
      gr[idx]->GetXaxis()->SetTitle("Slug");
      double size = 0.049;
      gr[idx]->GetXaxis()->SetTitleSize(size);
      gr[idx]->GetYaxis()->SetTitleSize(size);
//       gr[idx]->GetXaxis()->SetTitleColor(col[idet]);
//       gr[idx]->GetYaxis()->SetTitleColor(col[idet]);
      gr[idx]->GetXaxis()->SetTitleOffset(0.85);
      gr[idx]->GetYaxis()->SetTitleOffset(0.85);
      double mn = (imod==0 ? -120.:-120.);
      double mx = (imod==0 ? 60.:60.);
      gr[idx]->GetYaxis()->SetRangeUser(mn,mx);
      gr[idx]->SetMarkerStyle(7);
      gr[idx]->SetMarkerColor(col[idet]);
      gr[idx]->SetLineColor(col[idet]);
      gr[idx]->Draw("ap");
    }
  }

  gROOT->ForceStyle();
}
