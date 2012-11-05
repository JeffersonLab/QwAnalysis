//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : October 8th 2012
//*****************************************************************************************************//
/*

Calculate the leakage of transverse asymmetry in to PV asymmetry, given residual transverse polarization and 
leakge at 100% transverse

Leakage at 100% transverse C_v = 0.008 +- 0.072 ppm
                           C_h = 0.004 +- 0.064 ppm


Leakage is defined as leakage_long = |C_v*P_v|+|C_h*P_h|
error on leakage = sqrt( (P_v*dC_v)^2+(C_v*dP_v)^2 + (P_h*dC_h)^2 + (C_h*dP_h)^2  )

*/

#include <iostream>
#include <fstream>
#include <vector>
#include <new>
#include <TF1.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TProfile.h>
#include <TString.h>
#include <stdexcept>
#include <TLine.h>
#include <TPaveStats.h>
#include <time.h>
#include <stdio.h>
#include <TBox.h>
#include <TPaveText.h>
#include <TObject.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSQLStatement.h>
#include <TText.h>
#include <vector>
#include <iomanip>
#include <TRandom.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TStyle.h>
#include <TApplication.h>
#include <math.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TString.h>
#include <TMath.h>
#include <string.h>

void trans_leakage(Double_t P_v, Double_t dP_v, Double_t P_h, Double_t dP_h){


  const Double_t C_v  = 0.008; //ppm
  const Double_t dC_v = 0.072; //ppm
  const Double_t C_h  = 0.004; //ppm
  const Double_t dC_h = 0.064; //ppm


  Double_t leakage  = 0.0;
  Double_t dleakage = 0.0;


  // Calculate leakage
  leakage = TMath::Abs(C_v*P_v)+TMath::Abs(C_h*P_h);


  // Calculate error on leakage
  dleakage = sqrt( pow((C_v*dP_v),2)+pow((dC_v*P_v),2)+pow((C_h*dP_h),2)+pow((dC_h*P_h),2) );

  std::cout<<" P_v = "<<P_v<<" +- "<<dP_v<<std::endl;
  std::cout<<" P_h = "<<P_h<<" +- "<<dP_h<<std::endl;
  std::cout<<" leakage = "<<leakage<<" ppm "<<std::endl;
  std::cout<<" dleakage = +-"<<dleakage<<" ppm "<<std::endl;

}


void draw_leakage(){

  gStyle->Reset();
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1);
  gStyle->SetPadTopMargin(0.15);
  gStyle->SetPadBottomMargin(0.15);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetStatColor(kWhite);
  gStyle->SetTitleFillColor(kWhite);
  gStyle->SetTitleFont(132,"xy");
  gStyle->SetTitleFont(132,"1");
  gStyle->SetLabelFont(132,"xy");
  gStyle->SetStatFont(132);
  gStyle->SetTitleSize(0.05,"xy");
  gStyle->SetLabelSize(0.05,"xy");



 //run1
  Double_t leakage1[5]={0.172,0.264,0.092,0.368,0.364}; //ppb
  Double_t dleakage1[5]={1.39,2.156,0.83,3.4,3.57}; //ppb
  Double_t wiens1[5]={1,2,3,4,5};
  Double_t we1[5]={0.0,0.0,0.0,0.0,0.0};

  //run2
  Double_t leakage2[5]={0.26,0.16,0.14,0.108,0.1};//ppb
  Double_t dleakage2[5]={3.4,2.2,1.17,0.85,0.79};//ppb
  Double_t wiens2[5]={6,7,8,9,10};
  Double_t we2[5]={0.0,0.0,0.0,0.0,0.0};


  TGraphErrors * gl1 = new TGraphErrors(5,wiens1,leakage1,we1,dleakage1);
  gl1->SetMarkerColor(kBlue-6);
  gl1->SetLineColor(kBlue-6);
  gl1->SetMarkerSize(1.2);
  gl1->SetMarkerStyle(21);
  gl1->Fit("pol0");
  TF1 * fl1 = (TF1*)(gl1->GetFunction("pol0"));
  fl1->SetLineColor(kBlue-6);
  fl1->SetLineStyle(3);

  TGraphErrors * gl2 = new TGraphErrors(5,wiens2,leakage2,we2,dleakage2);
  gl2->SetMarkerColor(kOrange+2);
  gl2->SetLineColor(kOrange+2);
  gl2->SetMarkerSize(1.2);
  gl2->SetMarkerStyle(21);
  gl2->Fit("pol0");
  TF1 * fl2 = (TF1*)(gl2->GetFunction("pol0"));
  fl2->SetLineColor(kOrange+2);
  fl2->SetLineStyle(3);

  TCanvas *c = new TCanvas("c","",1000,800);
  c->SetFillColor(kWhite);
  c->Divide(1,2);

  c->cd(1);
  gl1->Draw("AEP");
  gl1->SetTitle("Leakage in run 1");
  gl1->GetXaxis()->SetNdivisions(5);
  gl1->GetXaxis()->SetTitle("Run 1 Wien");
  gl1->GetYaxis()->SetTitle("Leakage (ppb)");
  gl1->GetYaxis()->SetRangeUser(-4.5,4.5);

  c->cd(2);
  gl2->Draw("AEP");
  gl2->SetTitle("Leakage in run 1");
  gl2->GetXaxis()->SetNdivisions(5);
  gl2->GetXaxis()->SetTitle("Run 2 Wien");
  gl2->GetYaxis()->SetTitle("Leakage (ppb)");
  gl2->GetYaxis()->SetRangeUser(-4.5,4.5);

}
