#include "TString.h"
#include "TLine.h"
#include "TProfile.h"
#include "TSystem.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <cstdio>
#include "TH1D.h"
#include "TH2D.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TChain.h"
#include "TMath.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TDirectory.h"
#include "TMultiGraph.h"
#include "TGraphErrors.h"

Int_t plotPeriodOffsetFitResults(Int_t runStart = 13846, Int_t runEnd = 19000){
  gStyle->SetOptFit(1111);
  char *fgN[4] = {"fgx1","fgy1","fgx2","fgy2"};
  char fg[255], deg[255], err[255];
  Int_t nRuns = 0, n= 0;
  string str;
  Double_t period[4][5000], periodErr[4][5000], offset[4][5000], offsetErr[4][5000];
  Double_t runs[5000], runsErr[5000];
  for(int i = runStart;i<runEnd+1;i++){
    Bool_t endOfFile = 0;
    ifstream file(Form("%s/diagnostics/diagnostic_%i_ChiSqMin.set0.dat",
		       gSystem->Getenv("BMOD_OUT"), i));
    if(file.is_open()&&file.good()){
      //skip to end of file where fit results are stored
      nRuns ++;
      getline(file,str);
      while(str != "Ramp period fit results for function generator signals"){
	getline(file,str);
	if(file.eof()){
	  endOfFile = 1;
	  break;
	}
      }
      int good = 1;
      if(!endOfFile){
	for(int j=0;j<4;j++){
	  file>>fg>>deg>>err;
	  period[j][n] = atof(deg);
	  periodErr[j][n] = atof(err);
	  runs[n] = i;
	  runsErr[n] = 0;
	  getline(file,str);
	  //	  if(periodErr[j][n]>1000)
	  cout<<runs[n]<<": "<<period[j][n]<<"+/-"<<periodErr[j][n]<<endl;
	}
	getline(file,str);
	getline(file,str);
	for(int j=0;j<4;j++){
	  file>>fg>>deg>>err;
	  if(atof(deg)<-1000)good = 0;
	  offset[j][n] = atof(deg);
	  offsetErr[j][n] = atof(err);
	  getline(file,str);
	  //	  if(offsetErr[j][n]>1000)
	  cout<<runs[n]<<": "<<offset[j][n]<<"+/-"<<offsetErr[j][n]<<endl;
	}
	if(good)
	  n++;
      }
    }//else cout<<"File not found for run "<<i<<".\n";
    file.close();
  }
  cout<<nRuns<<" total runs found.\n";
  cout<<n<<" runs with fit results found.\n";
  TCanvas *cPeriod = new TCanvas("cPeriod","cPeriod",0,0,1400,1000);
  cPeriod->Divide(2,2);
  TGraphErrors *grPeriod[4];
  double fit = 0;
  for(int i=0;i<4;i++){
    cPeriod->cd(i+1);
    grPeriod[i] = new TGraphErrors(n-1,runs,period[i],runsErr,periodErr[i]);
    grPeriod[i]->Draw("ap");
    grPeriod[i]->SetMarkerColor(kBlue);
    grPeriod[i]->SetLineColor(kBlue);
    grPeriod[i]->SetMarkerStyle(7);
    grPeriod[i]->SetTitle(Form("%s Period Fit Result",fgN[i]));
    grPeriod[i]->GetXaxis()->SetTitle("Run");
    grPeriod[i]->GetYaxis()->SetTitle(Form("%s Period Fit Result",fgN[i]));
    grPeriod[i]->Draw("ap");
    TF1 *f = new TF1("f","pol0",0,360);
    grPeriod[i]->Fit(f);
    fit += f->GetParameter(0);
    gPad->Update();
  }
  cout<<"Average period: "<<fit/4.0<<endl;
  TCanvas *cOffset = new TCanvas("cOffset","cOffset",0,0,1400,1000);
  cOffset->Divide(2,2);
  TGraphErrors *grOffset[4];
  fit = 0;
  for(int i=0;i<4;i++){
    cOffset->cd(i+1);
    grOffset[i] = new TGraphErrors(n-1,runs,offset[i],runsErr,offsetErr[i]);
    grOffset[i]->Draw("ap");
    grOffset[i]->SetMarkerColor(kRed);
    grOffset[i]->SetLineColor(kRed);
    grOffset[i]->SetMarkerStyle(7);
    grOffset[i]->SetTitle(Form("%s Offset Fit Result",fgN[i]));
    grOffset[i]->GetXaxis()->SetTitle("Run");
    grOffset[i]->GetYaxis()->SetTitle(Form("%s Offset Fit Result",fgN[i]));
    grOffset[i]->Draw("ap");
    TF1 *f = new TF1("f","pol0",0,360);
    grOffset[i]->Fit(f);
    fit += f->GetParameter(0);
    gPad->Update();
  }
  cout<<"Average offset: "<<fit/4.0<<endl;
  return 0;
}
