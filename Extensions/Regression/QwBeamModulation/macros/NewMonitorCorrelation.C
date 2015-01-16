#include "TCanvas.h"
#include "TChain.h"
#include "TSystem.h"
#include "TFile.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TTree.h"
#include "TEventList.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TF1.h"
#include <cstdio>
#include <iostream>
#include <fstream>
typedef struct leaf_t{
  int slug;
  int run;
  int runlet;
  double slope;
  double slope_err;
  double correl;
};
const int nMON = 5, nDET=32, nSLUG=322;
Int_t GetMonitorAndDetectorLists(TString *monitorList, TString *detectorList, Bool_t trunc){
  string line;
  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_13993*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES")));

  ifstream file(Form("%s/config/setup_mpsonly.config",
		     gSystem->Getenv("BMOD_SRC")));

  for(int i=0;i<5;i++){
    char id[4] = "   ", monitr[20] = "                   ";
    file>>id>>monitr;
    getline(file, line);
    monitorList[i] = TString(monitr);
    if(!ch->GetBranch(monitorList[i].Data())){
      cout<<monitorList[i].Data()<<" missing. Exiting.\n";
      return -1;
    }else{
      if( trunc && monitorList[i].Contains("qwk_"))
	monitorList[i].Replace(0,4,"");
      cout<<monitorList[i].Data()<<"\n";
    }
  }
  getline(file, line);
  Int_t nDet = 0;
  for(int i=0;i<nDET;i++){
    char id[4] = "   ", detectr[20] = "                   ";
    file>>id>>detectr;
    getline(file, line);
    detectorList[nDet] = TString(detectr);
    if(!ch->GetBranch(detectorList[nDet].Data())){
      cout<<detectorList[nDet].Data()<<" missing.\n";
    }else{
      if( trunc && detectorList[nDet].Contains("qwk_"))
	detectorList[nDet].Replace(0,4,"");
      cout<<detectorList[nDet].Data()<<endl;
      nDet++;
    }
    file.peek();
    if(file.eof())break;
  }
  delete ch;
  return nDet;
}

Int_t NewMonitorCorrelation(int run_period = 1)
{
  gStyle->SetOptFit(1111);
  Int_t run, minEntries = 10;
  TString MonitorList[nMON], MonitorListFull[nMON];
  TString DetectorList[nDET], DetectorListFull[nDET];
  int  start = (run_period == 1? 42 : 137);
  int  end = (run_period == 1? 137 : 322);
  GetMonitorAndDetectorLists(MonitorListFull, DetectorListFull, 0);
  Int_t nDet = GetMonitorAndDetectorLists(MonitorList, DetectorList,1);
  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return -1;
  }
  TChain *ch = new TChain("reduced_tree");
  ch->Add("/net/data2/paschkelab2/DB_rootfiles/run1/hydrogen_cell_reduced_tree.root");
  ch->Add("/net/data2/paschkelab2/DB_rootfiles/run2/hydrogen_cell_reduced_tree.root");
  ch->SetBranchAddress("run",&run);
//   double alpha = (run_period == 1 ? 30000 : 38000);
//   double beta = (run_period == 1 ? 22000 : 32000);
//   double alpha = (run_period == 1 ? 25920 : 38600);
//   double beta = (run_period == 1 ? 17860 : 33400);
  double alpha = (run_period == 1 ? 35000 : 37000);
  double beta = (run_period == 1 ? 29000 : 29000);
  ch->SetAlias("X1",Form("diff_qwk_targetX+%f*diff_qwk_targetXSlope", alpha));
  ch->SetAlias("X2",Form("diff_qwk_targetX-%f*diff_qwk_targetXSlope", alpha));
  ch->SetAlias("Y1",Form("diff_qwk_targetY+%f*diff_qwk_targetYSlope", beta));
  ch->SetAlias("Y2",Form("diff_qwk_targetY-%f*diff_qwk_targetYSlope", beta));
  //  ch->SetBranchAddress("runlet",&runlet);
  vector<double>vSlug, vRun, vDX, vDXP, vDY,vDYP, vDX1, vDX2, vDY1, vDY2, vDE,vSlopeXXP,  vSlopeYYP, vSlopeXY,vSlopeXYP, vCorrelXXP, vCorrelXY, vCorrelXYP, vCorrelXE, vCorrelYXP, vCorrelYYP, vCorrelYE, vCorrelXPYP, vCorrelXPE, vCorrelYPE, vCorrelX1X2, vCorrelX1Y1, vCorrelX1Y2, vCorrelX1E, vCorrelY1X2, vCorrelY1Y2, vCorrelY1E, vCorrelX2Y2, vCorrelX2E, vCorrelY2E;
  TH1D *h1d[2000];
  TH2D *h[2000];
  TF1 *f = new TF1("f","pol1",-100000,100000);
  int n = 0;
  for(int islug=start;islug<end;islug++){
    if(islug%5==0)cout<<"Processing slug "<<islug<<endl;
    ch->Draw(Form("diff_qwk_targetXSlope*1000000.0:diff_qwk_targetX>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    if(h[n]->GetEntries()>minEntries){
      h[n]->Fit(f);
      vSlug.push_back(islug);
      vSlopeXXP.push_back(f->GetParameter(1));
      vCorrelXXP.push_back(h[n]->GetCorrelationFactor());
    }else continue;
    ch->Draw(Form("diff_qwk_targetX>>h1d%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h1d[n] = (TH1D*)gDirectory->Get(Form("h1d%i",n));
    vDX.push_back(h1d[n]->GetMean());
    ++n;

    ch->Draw(Form("diff_qwk_targetY:diff_qwk_targetX>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    h[n]->Fit(f);
    vSlopeXY.push_back(f->GetParameter(1));
    vCorrelXY.push_back(h[n]->GetCorrelationFactor());
    ch->Draw(Form("diff_qwk_targetY>>h1d%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h1d[n] = (TH1D*)gDirectory->Get(Form("h1d%i",n));
    vDY.push_back(h1d[n]->GetMean());
    ++n;

    ch->Draw(Form("diff_qwk_targetYSlope*1000000.0:diff_qwk_targetX>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    h[n]->Fit(f);
    vSlopeXYP.push_back(f->GetParameter(1));
    vCorrelXYP.push_back(h[n]->GetCorrelationFactor());
    ch->Draw(Form("diff_qwk_targetYSlope*1000000.0>>h1d%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h1d[n] = (TH1D*)gDirectory->Get(Form("h1d%i",n));
    vDYP.push_back(h[n]->GetMean());
    ++n;

    ch->Draw(Form("diff_qwk_bpm3c12X:diff_qwk_targetX>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelXE.push_back(h[n]->GetCorrelationFactor());
    ch->Draw(Form("diff_qwk_targetXSlope*1000000.0>>h1d%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h1d[n] = (TH1D*)gDirectory->Get(Form("h1d%i",n));
    vDXP.push_back(h1d[n]->GetMean());
    ++n;

    ch->Draw(Form("diff_qwk_bpm3c12X>>h1d%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h1d[n] = (TH1D*)gDirectory->Get(Form("h1d%i",n));
    vDE.push_back(h1d[n]->GetMean());






    ch->Draw(Form("diff_qwk_targetXSlope*1000000.0:diff_qwk_targetY>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelYXP.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("diff_qwk_targetYSlope*1000000.0:diff_qwk_targetY>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    h[n]->Fit(f);
    vSlopeYYP.push_back(f->GetParameter(1));
    vCorrelYYP.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("diff_qwk_bpm3c12X:diff_qwk_targetY>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelYE.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("diff_qwk_targetXSlope:diff_qwk_targetYSlope>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelXPYP.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("diff_qwk_bpm3c12X:diff_qwk_targetXSlope>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelXPE.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("diff_qwk_targetYSlope:diff_qwk_bpm3c12X>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelYPE.push_back(h[n]->GetCorrelationFactor());
    ++n;


    ch->Draw(Form("X2:X1>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX1X2.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("Y1:X1>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX1Y1.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("Y2:X1>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX1Y2.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("diff_qwk_bpm3c12X:X1>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX1E.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("X2:Y1>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelY1X2.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("Y2:Y1>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelY1Y2.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("diff_qwk_bpm3c12X:Y1>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelY1E.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("X2:Y2>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX2Y2.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("X2:diff_qwk_bpm3c12X>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX2E.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("Y2:diff_qwk_bpm3c12X>>h%i",n),Form("asym_qwk_mdallbars.n>0&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelY2E.push_back(h[n]->GetCorrelationFactor());
    ++n;


  }
  TCanvas *c = new TCanvas("c","c",0,0,1900,900);
  c->Divide(5,2);
  c->cd(1);
  TGraph *gSlopeXXP = new TGraph(vSlug.size(),&vSlug[0],&vSlopeXXP[0]);
  gSlopeXXP->SetMarkerColor(kRed);
  gSlopeXXP->SetMarkerStyle(7);
  gSlopeXXP->Draw("ap");
  gSlopeXXP->SetTitle("diff_targetXSlope/diff_targetX vs slug");
  gSlopeXXP->GetYaxis()->SetTitle("diff_targetXSlope/diff_targetX(#murad/nm)");
  gSlopeXXP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  c->cd(2);
  TGraph *gSlopeYYP = new TGraph(vSlug.size(),&vSlug[0],&vSlopeYYP[0]);
  gSlopeYYP->SetMarkerColor(kRed);
  gSlopeYYP->SetMarkerStyle(7);
  gSlopeYYP->Draw("ap");
  gSlopeYYP->SetTitle("diff_targetYSlope/diff_targetY vs slug");
  gSlopeYYP->GetYaxis()->SetTitle("diff_targetYSlope/diff_targetY(#murad/nm)");
  gSlopeYYP->GetXaxis()->SetTitle("Slug");
  gPad->Update();
  c->cd(3);
  TGraph *gSlopeXY = new TGraph(vSlug.size(),&vSlug[0],&vSlopeXY[0]);
  gSlopeXY->SetMarkerColor(kRed);
  gSlopeXY->SetMarkerStyle(7);
  gSlopeXY->Draw("ap");
  gSlopeXY->SetTitle("diff_targetY/diff_targetX vs slug");
  gSlopeXY->GetYaxis()->SetTitle("diff_targetY/diff_targetX");
  gSlopeXY->GetXaxis()->SetTitle("Slug");

  c->cd(4);
  TGraph *gSlopeXYP = new TGraph(vSlug.size(),&vSlug[0],&vSlopeXYP[0]);
  gSlopeXYP->SetMarkerColor(kRed);
  gSlopeXYP->SetMarkerStyle(7);
  gSlopeXYP->Draw("ap");
  gSlopeXYP->SetTitle("diff_targetYSlope/diff_targetX vs slug");
  gSlopeXYP->GetYaxis()->SetTitle("diff_targetYSlope/diff_targetX(#murad/nm)");
  gSlopeXYP->GetXaxis()->SetTitle("Slug");
  gPad->Update();


  c->cd(6);
  TGraph*gDX = new TGraph(vSlug.size(),&vSlug[0],&vDX[0]);
  gDX->SetMarkerColor(kRed);
  gDX->SetMarkerStyle(7);
  gDX->Draw("ap");
  gDX->SetTitle("diff_targetX vs slug");
  gDX->GetYaxis()->SetTitle("diff_targetX(nm)");
  gDX->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  c->cd(7);
  TGraph*gDY = new TGraph(vSlug.size(),&vSlug[0],&vDY[0]);
  gDY->SetMarkerColor(kRed);
  gDY->SetMarkerStyle(7);
  gDY->Draw("ap");
  gDY->SetTitle("diff_targetY vs slug");
  gDY->GetYaxis()->SetTitle("diff_targetY(nm)");
  gDY->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  c->cd(8);
  TGraph*gDXP = new TGraph(vSlug.size(),&vSlug[0],&vDXP[0]);
  gDXP->SetMarkerColor(kRed);
  gDXP->SetMarkerStyle(7);
  gDXP->Draw("ap");
  gDXP->SetTitle("diff_targetX vs slug");
  gDXP->GetYaxis()->SetTitle("diff_targetXSlope(#murad)");
  gDXP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  c->cd(9);
  TGraph*gDYP = new TGraph(vSlug.size(),&vSlug[0],&vDYP[0]);
  gDYP->SetMarkerColor(kRed);
  gDYP->SetMarkerStyle(7);
  gDYP->Draw("ap");
  gDYP->SetTitle("diff_targetYSlope vs slug");
  gDYP->GetYaxis()->SetTitle("diff_targetYSlope(#murad)");
  gDYP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  c->cd(10);
  TGraph*gDE = new TGraph(vSlug.size(),&vSlug[0],&vDE[0]);
  gDE->SetMarkerColor(kRed);
  gDE->SetMarkerStyle(7);
  gDE->Draw("ap");
  gDE->SetTitle("diff_bpm3c12X vs slug");
  gDE->GetYaxis()->SetTitle("diff_bpm3c12X(nm)");
  gDE->GetXaxis()->SetTitle("Slug");
  gPad->Update();


  TCanvas *cOld = new TCanvas("cOld","cOld",0,0,1900,900);
  cOld->Divide(5,2);
  cOld->cd(1);
  TGraph *gCorrelXXP = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXXP[0]);
  gCorrelXXP->SetMarkerColor(kBlue);
  gCorrelXXP->SetMarkerStyle(7);
  gCorrelXXP->Draw("ap");
  gCorrelXXP->SetTitle("Correlation diff_targetXSlope to diff_targetX ");
  gCorrelXXP->GetYaxis()->SetTitle("Correlation XP to X");
  gCorrelXXP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(2);
  TGraph *gCorrelXY = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXY[0]);
  gCorrelXY->SetMarkerColor(kBlue);
  gCorrelXY->SetMarkerStyle(7);
  gCorrelXY->Draw("ap");
  gCorrelXY->SetTitle("Correlation diff_targetY to diff_targetX ");
  gCorrelXY->GetYaxis()->SetTitle("Correlation Y to X");
  gCorrelXY->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(3);
  TGraph *gCorrelXYP = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXYP[0]);
  gCorrelXYP->SetMarkerColor(kBlue);
  gCorrelXYP->SetMarkerStyle(7);
  gCorrelXYP->Draw("ap");
  gCorrelXYP->SetTitle("Correlation diff_targetYSlope to diff_targetX ");
  gCorrelXYP->GetYaxis()->SetTitle("Correlation YP to X");
  gCorrelXYP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(4);
  TGraph *gCorrelXE = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXE[0]);
  gCorrelXE->SetMarkerColor(kBlue);
  gCorrelXE->SetMarkerStyle(7);
  gCorrelXE->Draw("ap");
  gCorrelXE->SetTitle("Correlation diff_bpm3c12X to diff_targetX ");
  gCorrelXE->GetYaxis()->SetTitle("Correlation 3c12X to X");
  gCorrelXE->GetXaxis()->SetTitle("Slug");
  gPad->Update();


  cOld->cd(5);
  TGraph *gCorrelYYP = new TGraph(vSlug.size(),&vSlug[0],&vCorrelYYP[0]);
  gCorrelYYP->SetMarkerColor(kBlue);
  gCorrelYYP->SetMarkerStyle(7);
  gCorrelYYP->Draw("ap");
  gCorrelYYP->SetTitle("Correlation diff_targetYSlope to diff_targetY ");
  gCorrelYYP->GetYaxis()->SetTitle("Correlation YP to Y");
  gCorrelYYP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(6);
  TGraph *gCorrelYXP = new TGraph(vSlug.size(),&vSlug[0],&vCorrelYXP[0]);
  gCorrelYXP->SetMarkerColor(kBlue);
  gCorrelYXP->SetMarkerStyle(7);
  gCorrelYXP->Draw("ap");
  gCorrelYXP->SetTitle("Correlation diff_targetXSlope to diff_targetY ");
  gCorrelYXP->GetYaxis()->SetTitle("Correlation XP to Y");
  gCorrelYXP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(7);
  TGraph *gCorrelYE = new TGraph(vSlug.size(),&vSlug[0],&vCorrelYE[0]);
  gCorrelYE->SetMarkerColor(kBlue);
  gCorrelYE->SetMarkerStyle(7);
  gCorrelYE->Draw("ap");
  gCorrelYE->SetTitle("Correlation diff_bpm3c12X to diff_targetY ");
  gCorrelYE->GetYaxis()->SetTitle("Correlation 3c12X to Y");
  gCorrelYE->GetXaxis()->SetTitle("Slug");
  gPad->Update();


  cOld->cd(8);
  TGraph *gCorrelXPYP = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXPYP[0]);
  gCorrelXPYP->SetMarkerColor(kBlue);
  gCorrelXPYP->SetMarkerStyle(7);
  gCorrelXPYP->Draw("ap");
  gCorrelXPYP->SetTitle("Correlation diff_targetXSlope to diff_targetY ");
  gCorrelXPYP->GetYaxis()->SetTitle("Correlation XP to Y");
  gCorrelXPYP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(9);
  TGraph *gCorrelXPE = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXPE[0]);
  gCorrelXPE->SetMarkerColor(kBlue);
  gCorrelXPE->SetMarkerStyle(7);
  gCorrelXPE->Draw("ap");
  gCorrelXPE->SetTitle("Correlation diff_bpm3c12X to diff_targetXSlope ");
  gCorrelXPE->GetYaxis()->SetTitle("Correlation 3c12X to XP");
  gCorrelXPE->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(10);
  TGraph *gCorrelYPE = new TGraph(vSlug.size(),&vSlug[0],&vCorrelYPE[0]);
  gCorrelYPE->SetMarkerColor(kBlue);
  gCorrelYPE->SetMarkerStyle(7);
  gCorrelYPE->Draw("ap");
  gCorrelYPE->SetTitle("Correlation diff_bpm3c12X to diff_targetYSlope ");
  gCorrelYPE->GetYaxis()->SetTitle("Correlation 3c12X to XP");
  gCorrelYPE->GetXaxis()->SetTitle("Slug");
  gPad->Update();


  TCanvas *cNew = new TCanvas("cNew","cNew",0,0,1900,900);
  cNew->Divide(5,2);
  cNew->cd(1);
  TGraph *gCorrelX1X2 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX1X2[0]);
  gCorrelX1X2->SetMarkerColor(kBlue);
  gCorrelX1X2->SetMarkerStyle(7);
  gCorrelX1X2->Draw("ap");
  gCorrelX1X2->SetTitle("Correlation diff_X2 to diff_X1 ");
  gCorrelX1X2->GetYaxis()->SetTitle("Correlation  X2 to X1");
  gCorrelX1X2->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(2);
  TGraph *gCorrelX1Y1 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX1Y1[0]);
  gCorrelX1Y1->SetMarkerColor(kBlue);
  gCorrelX1Y1->SetMarkerStyle(7);
  gCorrelX1Y1->Draw("ap");
  gCorrelX1Y1->SetTitle("Correlation diff_Y1 to diff_X1 ");
  gCorrelX1Y1->GetYaxis()->SetTitle("Correlation  Y1 to X1");
  gCorrelX1Y1->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(3);
  TGraph *gCorrelX1Y2 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX1Y2[0]);
  gCorrelX1Y2->SetMarkerColor(kBlue);
  gCorrelX1Y2->SetMarkerStyle(7);
  gCorrelX1Y2->Draw("ap");
  gCorrelX1Y2->SetTitle("Correlation diff_Y2 to diff_X1 ");
  gCorrelX1Y2->GetYaxis()->SetTitle("Correlation  Y2 to X1");
  gCorrelX1Y2->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(4);
  TGraph *gCorrelX1E = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX1E[0]);
  gCorrelX1E->SetMarkerColor(kBlue);
  gCorrelX1E->SetMarkerStyle(7);
  gCorrelX1E->Draw("ap");
  gCorrelX1E->SetTitle("Correlation diff_bpm3c12X to diff_X1 ");
  gCorrelX1E->GetYaxis()->SetTitle("Correlation  3c12X to X1");
  gCorrelX1E->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(5);
  TGraph *gCorrelY1Y2 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelY1Y2[0]);
  gCorrelY1Y2->SetMarkerColor(kBlue);
  gCorrelY1Y2->SetMarkerStyle(7);
  gCorrelY1Y2->Draw("ap");
  gCorrelY1Y2->SetTitle("Correlation diff_Y2 to diff_Y1 ");
  gCorrelY1Y2->GetYaxis()->SetTitle("Correlation  Y2 to Y1");
  gCorrelY1Y2->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(6);
  TGraph *gCorrelY1X2 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelY1X2[0]);
  gCorrelY1X2->SetMarkerColor(kBlue);
  gCorrelY1X2->SetMarkerStyle(7);
  gCorrelY1X2->Draw("ap");
  gCorrelY1X2->SetTitle("Correlation diff_X2 to diff_Y1 ");
  gCorrelY1X2->GetYaxis()->SetTitle("Correlation  X2 to Y1");
  gCorrelY1X2->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(7);
  TGraph *gCorrelY1E = new TGraph(vSlug.size(),&vSlug[0],&vCorrelY1E[0]);
  gCorrelY1E->SetMarkerColor(kBlue);
  gCorrelY1E->SetMarkerStyle(7);
  gCorrelY1E->Draw("ap");
  gCorrelY1E->SetTitle("Correlation diff_bpm3c12X to diff_Y1 ");
  gCorrelY1E->GetYaxis()->SetTitle("Correlation  3c12X to Y1");
  gCorrelY1E->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(8);
  TGraph *gCorrelX2Y2 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX2Y2[0]);
  gCorrelX2Y2->SetMarkerColor(kBlue);
  gCorrelX2Y2->SetMarkerStyle(7);
  gCorrelX2Y2->Draw("ap");
  gCorrelX2Y2->SetTitle("Correlation diff_Y2 to diff_X2 ");
  gCorrelX2Y2->GetYaxis()->SetTitle("Correlation  Y2 to X2");
  gCorrelX2Y2->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(9);
  TGraph *gCorrelX2E = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX2E[0]);
  gCorrelX2E->SetMarkerColor(kBlue);
  gCorrelX2E->SetMarkerStyle(7);
  gCorrelX2E->Draw("ap");
  gCorrelX2E->SetTitle("Correlation diff_bpm3c12X to diff_X2 ");
  gCorrelX2E->GetYaxis()->SetTitle("Correlation  3c12X to X2");
  gCorrelX2E->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(10);
  TGraph *gCorrelY2E = new TGraph(vSlug.size(),&vSlug[0],&vCorrelY2E[0]);
  gCorrelY2E->SetMarkerColor(kBlue);
  gCorrelY2E->SetMarkerStyle(7);
  gCorrelY2E->Draw("ap");
  gCorrelY2E->SetTitle("Correlation diff_bpm3c12X to diff_Y2 ");
  gCorrelY2E->GetYaxis()->SetTitle("Correlation  3c12X to Y2");
  gCorrelY2E->GetXaxis()->SetTitle("Slug");
  gPad->Update();





  return 0;
}
