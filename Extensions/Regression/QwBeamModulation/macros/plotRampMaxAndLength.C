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

Int_t plotRampMaxAndLength(Int_t runStart = 10000, Int_t runEnd = 19000){
  gStyle->SetOptFit(1111);
  char fg[255], deg[255];
  Int_t nRuns = 0, n= 0;
  string str;
  char *output = (char*)gSystem->Getenv("BMOD_OUT");
  //    output = "/net/data1/paschkedata1/bmod_out_new_monitors";
  Double_t rampmax[5000], ramplength[5000], degPerMPS[5000];
  Double_t runs[5000];
  for(int i = runStart;i<runEnd+1;i++){
    Bool_t endOfFile = 0;
    ifstream file(Form("%s/diagnostics/diagnostic_%i_ChiSqMin.set0.dat",
		       output, i));
    if(file.is_open()&&file.good()){
      //skip to end of file where fit results are stored
      nRuns ++;
      getline(file,str);
      while(str != "Ramp information"){
	getline(file,str);
	if(file.eof()){
	  endOfFile = 1;
	  break;
	}
      }
      getline(file,str);
      getline(file,str);
      file>>fg>>fg>>deg;
      rampmax[n] = atof(deg);
      int good = 1;
      if(atof(deg)<-1000)good = 0;

      file>>fg>>fg>>deg;
      ramplength[n] = atof(deg);
      runs[n] = i;
//       if(runs[n]>18895)
// 	printf("%i\n",int(runs[n]));
      file>>fg>>fg>>fg>>deg;
      degPerMPS[n] = atof(deg);
      runs[n] = i;
      if(good)
	n++;
    }
    file.close();
  }//else cout<<"File not found for run "<<i<<".\n";

  cout<<nRuns<<" total runs found.\n";
  cout<<n<<" runs with fit results found.\n";
  TCanvas *c = new TCanvas("c","c",0,0,1600,900);
  c->Divide(3,2);
  c->cd(1);
  TGraph *grRampmax = new TGraphErrors(n-1,runs,rampmax);
  double fit = 0;
  grRampmax->Draw("ap");
  grRampmax->SetMarkerColor(kBlue);
  grRampmax->SetLineColor(kBlue);
  grRampmax->SetMarkerStyle(7);
  grRampmax->SetTitle("Rampmax");
  grRampmax->GetXaxis()->SetTitle("Run");
  grRampmax->GetYaxis()->SetTitle("Rampmax");
  grRampmax->Draw("ap");
  TF1 *f = new TF1("f","pol0",0,360);
  grRampmax->Fit(f);
  fit = f->GetParameter(0);
  gPad->Update();
  
  cout<<"Rampmax: "<<fit<<endl;
  c->cd(2);
  TGraphErrors *grRamplength = new TGraphErrors(n-1,runs,ramplength);
  fit = 0;
  grRamplength->Draw("ap");
  grRamplength->SetMarkerColor(kRed);
  grRamplength->SetLineColor(kRed);
  grRamplength->SetMarkerStyle(7);
  grRamplength->SetTitle("Ramplength");
  grRamplength->GetXaxis()->SetTitle("Run");
  grRamplength->GetYaxis()->SetTitle("Ramplength");
  grRamplength->Draw("ap");
  grRamplength->Fit(f);
  fit += f->GetParameter(0);
  gPad->Update();
  c->cd(3);
  TGraphErrors *grDegPerMPS = new TGraphErrors(n-1,runs,degPerMPS);
  fit = 0;
  grDegPerMPS->Draw("ap");
  grDegPerMPS->SetMarkerColor(kRed);
  grDegPerMPS->SetLineColor(kRed);
  grDegPerMPS->SetMarkerStyle(7);
  grDegPerMPS->SetTitle("DegPerMPS");
  grDegPerMPS->GetXaxis()->SetTitle("Run");
  grDegPerMPS->GetYaxis()->SetTitle("Degrees Per MPS");
  grDegPerMPS->Draw("ap");
  grDegPerMPS->Fit(f);
  fit += f->GetParameter(0);
  gPad->Update();
  TH1F *hMax = new TH1F("hMax","Ramp Maximum",300,0,400);
  TH1F *hLen = new TH1F("hLen","Ramp Length",300,150,480);
  TH1F *hDeg = new TH1F("hDeg","Degrees Per MPS",300,20,80);
  for(int i=0;i<n;i++){
    hMax->Fill(rampmax[i]);
    hLen->Fill(ramplength[i]);
    hDeg->Fill(degPerMPS[i]);
  }
  c->cd(4);
  hMax->Draw();
  c->cd(5);
  hLen->Draw();
  c->cd(6);
  hDeg->Draw();
  cout<<"Ramplength: "<<fit<<endl;
  return 0;

}
