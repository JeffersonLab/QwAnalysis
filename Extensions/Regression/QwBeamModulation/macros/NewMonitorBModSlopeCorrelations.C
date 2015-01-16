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

Int_t NewMonitorBModSlopeCorrelations(int run_period = 1)
{
  gStyle->SetOptFit(1111);
  gStyle->SetMarkerStyle(6);
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
  TChain *ch = new TChain("slopes");
  ch->Add("/net/data1/paschkedata1/MacrocycleSlopesTree_ChiSqMin.set0.root");
  ch->SetBranchAddress("run",&run); 
  double alpha = (run_period == 1 ? 35 : 37);
  double beta = (run_period == 1 ? 29 : 29);
  ch->SetAlias("MDall_X1",Form("mdallbars_targetX+%f*mdallbars_targetXSlope",1./alpha));
  ch->SetAlias("MDall_X2",Form("mdallbars_targetX-%f*mdallbars_targetXSlope",1./alpha));
  ch->SetAlias("MDall_Y1",Form("mdallbars_targetY+%f*mdallbars_targetYSlope", 1./beta));
  ch->SetAlias("MDall_Y2",Form("mdallbars_targetY-%f*mdallbars_targetYSlope", 1./beta));
  //  ch->SetBranchAddress("runlet",&runlet);
  vector<double>vSlug, vRun, vCorrelXXP, vCorrelXY, vCorrelXYP, vCorrelXE, vCorrelYXP, vCorrelYYP, vCorrelYE, vCorrelXPYP, vCorrelXPE, vCorrelYPE, vCorrelX1X2, vCorrelX1Y1, vCorrelX1Y2, vCorrelX1E, vCorrelY1X2, vCorrelY1Y2, vCorrelY1E, vCorrelX2Y2, vCorrelX2E, vCorrelY2E;
  TH2D *h[2000];
  TF1 *f = new TF1("f","pol1",-100000,100000);
  int n = 0;
  for(int islug=start;islug<end;islug++){
    if(islug%5==0)cout<<"Processing slug "<<islug<<endl;
    ch->Draw(Form("mdallbars_targetXSlope:mdallbars_targetX>>h%i",n),Form("good&&slug==%i",islug));
    gPad->Update();
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    if(h[n]->GetEntries()>minEntries){
      vSlug.push_back(islug);
      vCorrelXXP.push_back(h[n]->GetCorrelationFactor());
    }else continue;
    ++n;

    ch->Draw(Form("mdallbars_targetY:mdallbars_targetX>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelXY.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("mdallbars_targetYSlope:mdallbars_targetX>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelXYP.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("mdallbars_bpm3c12X:mdallbars_targetX>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelXE.push_back(h[n]->GetCorrelationFactor());
    ++n;


    ch->Draw(Form("mdallbars_targetXSlope:mdallbars_targetY>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelYXP.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("mdallbars_targetYSlope:mdallbars_targetY>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelYYP.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("mdallbars_bpm3c12X:mdallbars_targetY>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelYE.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("mdallbars_targetXSlope:mdallbars_targetYSlope>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelXPYP.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("mdallbars_bpm3c12X:mdallbars_targetXSlope>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelXPE.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("mdallbars_targetYSlope:mdallbars_bpm3c12X>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelYPE.push_back(h[n]->GetCorrelationFactor());
    ++n;


    ch->Draw(Form("MDall_X2:MDall_X1>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX1X2.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("MDall_Y1:MDall_X1>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX1Y1.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("MDall_Y2:MDall_X1>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX1Y2.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("mdallbars_bpm3c12X:MDall_X1>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX1E.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("MDall_X2:MDall_Y1>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelY1X2.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("MDall_Y2:MDall_Y1>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelY1Y2.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("mdallbars_bpm3c12X:MDall_Y1>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelY1E.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("MDall_X2:MDall_Y2>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX2Y2.push_back(h[n]->GetCorrelationFactor());
    ++n;
    ch->Draw(Form("MDall_X2:mdallbars_bpm3c12X>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelX2E.push_back(h[n]->GetCorrelationFactor());
    ++n;

    ch->Draw(Form("MDall_Y2:mdallbars_bpm3c12X>>h%i",n),Form("good&&slug==%i",islug),"goff");
    h[n] = (TH2D*)gDirectory->Get(Form("h%i",n));
    vCorrelY2E.push_back(h[n]->GetCorrelationFactor());
    ++n;


  }
//   TCanvas *c = new TCanvas("c","c",0,0,1900,900);
//   c->Divide(5,2);
//   c->cd(1);
//   TGraph *gSlopeXXP = new TGraph(vSlug.size(),&vSlug[0],&vSlopeXXP[0]);
//   gSlopeXXP->SetMarkerColor(kRed);
//   gSlopeXXP->SetMarkerStyle(7);
//   gSlopeXXP->Draw("ap");
//   gSlopeXXP->SetTitle("diff_targetXSlope/diff_targetX vs slug");
//   gSlopeXXP->GetYaxis()->SetTitle("diff_targetXSlope/diff_targetX(#murad/nm)");
//   gSlopeXXP->GetXaxis()->SetTitle("Slug");
//   gPad->Update();

//   c->cd(2);
//   TGraph *gSlopeYYP = new TGraph(vSlug.size(),&vSlug[0],&vSlopeYYP[0]);
//   gSlopeYYP->SetMarkerColor(kRed);
//   gSlopeYYP->SetMarkerStyle(7);
//   gSlopeYYP->Draw("ap");
//   gSlopeYYP->SetTitle("diff_targetYSlope/diff_targetY vs slug");
//   gSlopeYYP->GetYaxis()->SetTitle("diff_targetYSlope/diff_targetY(#murad/nm)");
//   gSlopeYYP->GetXaxis()->SetTitle("Slug");
//   gPad->Update();
//   c->cd(3);
//   TGraph *gSlopeXY = new TGraph(vSlug.size(),&vSlug[0],&vSlopeXY[0]);
//   gSlopeXY->SetMarkerColor(kRed);
//   gSlopeXY->SetMarkerStyle(7);
//   gSlopeXY->Draw("ap");
//   gSlopeXY->SetTitle("diff_targetY/diff_targetX vs slug");
//   gSlopeXY->GetYaxis()->SetTitle("diff_targetY/diff_targetX");
//   gSlopeXY->GetXaxis()->SetTitle("Slug");

//   c->cd(4);
//   TGraph *gSlopeXYP = new TGraph(vSlug.size(),&vSlug[0],&vSlopeXYP[0]);
//   gSlopeXYP->SetMarkerColor(kRed);
//   gSlopeXYP->SetMarkerStyle(7);
//   gSlopeXYP->Draw("ap");
//   gSlopeXYP->SetTitle("diff_targetYSlope/diff_targetX vs slug");
//   gSlopeXYP->GetYaxis()->SetTitle("diff_targetYSlope/diff_targetX(#murad/nm)");
//   gSlopeXYP->GetXaxis()->SetTitle("Slug");
//   gPad->Update();


//   c->cd(6);
//   TGraph*gDX = new TGraph(vSlug.size(),&vSlug[0],&vDX[0]);
//   gDX->SetMarkerColor(kRed);
//   gDX->SetMarkerStyle(7);
//   gDX->Draw("ap");
//   gDX->SetTitle("diff_targetX vs slug");
//   gDX->GetYaxis()->SetTitle("diff_targetX(nm)");
//   gDX->GetXaxis()->SetTitle("Slug");
//   gPad->Update();

//   c->cd(7);
//   TGraph*gDY = new TGraph(vSlug.size(),&vSlug[0],&vDY[0]);
//   gDY->SetMarkerColor(kRed);
//   gDY->SetMarkerStyle(7);
//   gDY->Draw("ap");
//   gDY->SetTitle("diff_targetY vs slug");
//   gDY->GetYaxis()->SetTitle("diff_targetY(nm)");
//   gDY->GetXaxis()->SetTitle("Slug");
//   gPad->Update();

//   c->cd(8);
//   TGraph*gDXP = new TGraph(vSlug.size(),&vSlug[0],&vDXP[0]);
//   gDXP->SetMarkerColor(kRed);
//   gDXP->SetMarkerStyle(7);
//   gDXP->Draw("ap");
//   gDXP->SetTitle("diff_targetX vs slug");
//   gDXP->GetYaxis()->SetTitle("diff_targetXSlope(#murad)");
//   gDXP->GetXaxis()->SetTitle("Slug");
//   gPad->Update();

//   c->cd(9);
//   TGraph*gDYP = new TGraph(vSlug.size(),&vSlug[0],&vDYP[0]);
//   gDYP->SetMarkerColor(kRed);
//   gDYP->SetMarkerStyle(7);
//   gDYP->Draw("ap");
//   gDYP->SetTitle("diff_targetYSlope vs slug");
//   gDYP->GetYaxis()->SetTitle("diff_targetYSlope(#murad)");
//   gDYP->GetXaxis()->SetTitle("Slug");
//   gPad->Update();

//   c->cd(10);
//   TGraph*gDE = new TGraph(vSlug.size(),&vSlug[0],&vDE[0]);
//   gDE->SetMarkerColor(kRed);
//   gDE->SetMarkerStyle(7);
//   gDE->Draw("ap");
//   gDE->SetTitle("diff_bpm3c12X vs slug");
//   gDE->GetYaxis()->SetTitle("diff_bpm3c12X(nm)");
//   gDE->GetXaxis()->SetTitle("Slug");
//   gPad->Update();


  TCanvas *cOld = new TCanvas("cOld","cOld",0,0,1900,900);
  cOld->Divide(5,2);
  cOld->cd(1);
  TGraph *gCorrelXXP = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXXP[0]);
  gCorrelXXP->SetMarkerColor(kBlue);
  gCorrelXXP->SetMarkerStyle(7);
  gCorrelXXP->Draw("ap");
  gCorrelXXP->SetTitle("Correlation mdallbars_targetX to mdallbars_targetX ");
  gCorrelXXP->GetYaxis()->SetTitle("Correlation MDall_XP to MDall_X");
  gCorrelXXP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(2);
  TGraph *gCorrelXY = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXY[0]);
  gCorrelXY->SetMarkerColor(kBlue);
  gCorrelXY->SetMarkerStyle(7);
  gCorrelXY->Draw("ap");
  gCorrelXY->SetTitle("Correlation mdallbars_targetY to mdallbars_targetX ");
  gCorrelXY->GetYaxis()->SetTitle("Correlation MDall_Y to MDall_X");
  gCorrelXY->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(3);
  TGraph *gCorrelXYP = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXYP[0]);
  gCorrelXYP->SetMarkerColor(kBlue);
  gCorrelXYP->SetMarkerStyle(7);
  gCorrelXYP->Draw("ap");
  gCorrelXYP->SetTitle("Correlation mdallbars_targetYSlope to mdallbars_targetX ");
  gCorrelXYP->GetYaxis()->SetTitle("Correlation MDall_YP to MDall_X");
  gCorrelXYP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(4);
  TGraph *gCorrelXE = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXE[0]);
  gCorrelXE->SetMarkerColor(kBlue);
  gCorrelXE->SetMarkerStyle(7);
  gCorrelXE->Draw("ap");
  gCorrelXE->SetTitle("Correlation mdallbars_bpm3c12X to mdallbars_targetX ");
  gCorrelXE->GetYaxis()->SetTitle("Correlation MDall_3c12X to MDall_X");
  gCorrelXE->GetXaxis()->SetTitle("Slug");
  gPad->Update();


  cOld->cd(5);
  TGraph *gCorrelYYP = new TGraph(vSlug.size(),&vSlug[0],&vCorrelYYP[0]);
  gCorrelYYP->SetMarkerColor(kBlue);
  gCorrelYYP->SetMarkerStyle(7);
  gCorrelYYP->Draw("ap");
  gCorrelYYP->SetTitle("Correlation mdallbars_targetYSlope to mdallbars_targetY ");
  gCorrelYYP->GetYaxis()->SetTitle("Correlation MDall_YP to MDall_Y");
  gCorrelYYP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(6);
  TGraph *gCorrelYXP = new TGraph(vSlug.size(),&vSlug[0],&vCorrelYXP[0]);
  gCorrelYXP->SetMarkerColor(kBlue);
  gCorrelYXP->SetMarkerStyle(7);
  gCorrelYXP->Draw("ap");
  gCorrelYXP->SetTitle("Correlation mdallbars_targetXSlope to mdallbars_targetY ");
  gCorrelYXP->GetYaxis()->SetTitle("Correlation MDall_XP to MDall_Y");
  gCorrelYXP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(7);
  TGraph *gCorrelYE = new TGraph(vSlug.size(),&vSlug[0],&vCorrelYE[0]);
  gCorrelYE->SetMarkerColor(kBlue);
  gCorrelYE->SetMarkerStyle(7);
  gCorrelYE->Draw("ap");
  gCorrelYE->SetTitle("Correlation mdallbars_bpm3c12X to mdallbars_targetY ");
  gCorrelYE->GetYaxis()->SetTitle("Correlation MDall_3c12X to MDall_Y");
  gCorrelYE->GetXaxis()->SetTitle("Slug");
  gPad->Update();


  cOld->cd(8);
  TGraph *gCorrelXPYP = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXPYP[0]);
  gCorrelXPYP->SetMarkerColor(kBlue);
  gCorrelXPYP->SetMarkerStyle(7);
  gCorrelXPYP->Draw("ap");
  gCorrelXPYP->SetTitle("Correlation mdallbars_targetXSlope to mdallbars_targetY ");
  gCorrelXPYP->GetYaxis()->SetTitle("Correlation MDall_XP to MDall_Y");
  gCorrelXPYP->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(9);
  TGraph *gCorrelXPE = new TGraph(vSlug.size(),&vSlug[0],&vCorrelXPE[0]);
  gCorrelXPE->SetMarkerColor(kBlue);
  gCorrelXPE->SetMarkerStyle(7);
  gCorrelXPE->Draw("ap");
  gCorrelXPE->SetTitle("Correlation mdallbars_bpm3c12X to mdallbars_targetXSlope ");
  gCorrelXPE->GetYaxis()->SetTitle("Correlation MDall_3c12X toMDall_ XP");
  gCorrelXPE->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cOld->cd(10);
  TGraph *gCorrelYPE = new TGraph(vSlug.size(),&vSlug[0],&vCorrelYPE[0]);
  gCorrelYPE->SetMarkerColor(kBlue);
  gCorrelYPE->SetMarkerStyle(7);
  gCorrelYPE->Draw("ap");
  gCorrelYPE->SetTitle("Correlation mdallbars_bpm3c12X to mdallbars_targetYSlope ");
  gCorrelYPE->GetYaxis()->SetTitle("Correlation MDall_3c12X to MDall_XP");
  gCorrelYPE->GetXaxis()->SetTitle("Slug");
  gPad->Update();


  TCanvas *cNew = new TCanvas("cNew","cNew",0,0,1900,900);
  cNew->Divide(5,2);
  cNew->cd(1);
  TGraph *gCorrelX1X2 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX1X2[0]);
  gCorrelX1X2->SetMarkerColor(kBlue);
  gCorrelX1X2->SetMarkerStyle(7);
  gCorrelX1X2->Draw("ap");
  gCorrelX1X2->SetTitle("Correlation MDall_X2 to MDall_X1 ");
  gCorrelX1X2->GetYaxis()->SetTitle("Correlation  MDall_X2 to MDall_X1");
  gCorrelX1X2->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(2);
  TGraph *gCorrelX1Y1 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX1Y1[0]);
  gCorrelX1Y1->SetMarkerColor(kBlue);
  gCorrelX1Y1->SetMarkerStyle(7);
  gCorrelX1Y1->Draw("ap");
  gCorrelX1Y1->SetTitle("Correlation MDall_Y1 to MDall_X1 ");
  gCorrelX1Y1->GetYaxis()->SetTitle("Correlation  MDall_Y1 to MDall_X1");
  gCorrelX1Y1->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(3);
  TGraph *gCorrelX1Y2 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX1Y2[0]);
  gCorrelX1Y2->SetMarkerColor(kBlue);
  gCorrelX1Y2->SetMarkerStyle(7);
  gCorrelX1Y2->Draw("ap");
  gCorrelX1Y2->SetTitle("Correlation MDall_Y2 to MDall_X1 ");
  gCorrelX1Y2->GetYaxis()->SetTitle("Correlation  MDall_Y2 to MDall_X1");
  gCorrelX1Y2->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(4);
  TGraph *gCorrelX1E = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX1E[0]);
  gCorrelX1E->SetMarkerColor(kBlue);
  gCorrelX1E->SetMarkerStyle(7);
  gCorrelX1E->Draw("ap");
  gCorrelX1E->SetTitle("Correlation MDall_bpm3c12X to MDall_X1 ");
  gCorrelX1E->GetYaxis()->SetTitle("Correlation  MDall_3c12X to MDall_X1");
  gCorrelX1E->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(5);
  TGraph *gCorrelY1Y2 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelY1Y2[0]);
  gCorrelY1Y2->SetMarkerColor(kBlue);
  gCorrelY1Y2->SetMarkerStyle(7);
  gCorrelY1Y2->Draw("ap");
  gCorrelY1Y2->SetTitle("Correlation MDall_Y2 to MDall_Y1 ");
  gCorrelY1Y2->GetYaxis()->SetTitle("Correlation  MDall_Y2 to MDall_Y1");
  gCorrelY1Y2->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(6);
  TGraph *gCorrelY1X2 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelY1X2[0]);
  gCorrelY1X2->SetMarkerColor(kBlue);
  gCorrelY1X2->SetMarkerStyle(7);
  gCorrelY1X2->Draw("ap");
  gCorrelY1X2->SetTitle("Correlation MDall_X2 to MDall_Y1 ");
  gCorrelY1X2->GetYaxis()->SetTitle("Correlation  MDall_X2 to MDall_Y1");
  gCorrelY1X2->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(7);
  TGraph *gCorrelY1E = new TGraph(vSlug.size(),&vSlug[0],&vCorrelY1E[0]);
  gCorrelY1E->SetMarkerColor(kBlue);
  gCorrelY1E->SetMarkerStyle(7);
  gCorrelY1E->Draw("ap");
  gCorrelY1E->SetTitle("Correlation MDall_bpm3c12X to MDall_Y1 ");
  gCorrelY1E->GetYaxis()->SetTitle("Correlation  MDall_3c12X to MDall_Y1");
  gCorrelY1E->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(8);
  TGraph *gCorrelX2Y2 = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX2Y2[0]);
  gCorrelX2Y2->SetMarkerColor(kBlue);
  gCorrelX2Y2->SetMarkerStyle(7);
  gCorrelX2Y2->Draw("ap");
  gCorrelX2Y2->SetTitle("Correlation MDall_Y2 to MDall_X2 ");
  gCorrelX2Y2->GetYaxis()->SetTitle("Correlation  MDall_Y2 to MDall_X2");
  gCorrelX2Y2->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(9);
  TGraph *gCorrelX2E = new TGraph(vSlug.size(),&vSlug[0],&vCorrelX2E[0]);
  gCorrelX2E->SetMarkerColor(kBlue);
  gCorrelX2E->SetMarkerStyle(7);
  gCorrelX2E->Draw("ap");
  gCorrelX2E->SetTitle("Correlation MDall_bpm3c12X to MDall_X2 ");
  gCorrelX2E->GetYaxis()->SetTitle("Correlation  MDall_3c12X to MDall_X2");
  gCorrelX2E->GetXaxis()->SetTitle("Slug");
  gPad->Update();

  cNew->cd(10);
  TGraph *gCorrelY2E = new TGraph(vSlug.size(),&vSlug[0],&vCorrelY2E[0]);
  gCorrelY2E->SetMarkerColor(kBlue);
  gCorrelY2E->SetMarkerStyle(7);
  gCorrelY2E->Draw("ap");
  gCorrelY2E->SetTitle("Correlation MDall_bpm3c12X to MDall_Y2 ");
  gCorrelY2E->GetYaxis()->SetTitle("Correlation  MDall_3c12X to MDall_Y2");
  gCorrelY2E->GetXaxis()->SetTitle("Slug");
  gPad->Update();





  return 0;
}
