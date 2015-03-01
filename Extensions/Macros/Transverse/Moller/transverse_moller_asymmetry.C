//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : February 27, 2012
//*****************************************************************************************************//
/*
This script uses the formulas given in 
Phys Rev D 69, 113001,(2004) for E158 moller transverse asymmetry predictions
to calculate the expected moller transverse asymmetry at QWeak kinematics

Inputs : x =
         par =
*/

#include <math.h>
#include <stdio.h>
#include <iostream>
#include <TMath.h>
#include <TCanvas.h>
#include <TText.h>
#include <TStyle.h>
#include <TF1.h>
#include <TPaveText.h>
#include <TAxis.h>
#include <TLegend.h>

// For Qweak beam energy
Double_t fit_function(Double_t *x, Double_t *par)
{

  // General
  Double_t m_e = 0.511; //MeV
  Double_t m_p = 938; //MeV
  Double_t alpha = 1.0/137; //fine structure constant
  
  // Qweak kinematics
  Double_t E = 877; //MeV beam energy after correcting for energy loss at the target
  
  Double_t s =  2*m_e*E; //MeV

  //std::cout<<"theta "<<x[0]<<std::endl;
  Double_t costhetacm = (((m_e/E)-1+ TMath::Cos((TMath::Pi()/180)*x[0]))/((m_e/E)+1-TMath::Cos((TMath::Pi()/180)*x[0])));
  //std::cout<<"cos "<<costhetacm<<std::endl;
  Double_t sinthetacm = sqrt(1 - costhetacm*costhetacm );
  //std::cout<<"sin "<<sinthetacm<<std::endl;

  Double_t t = -(s/2)*(1 - costhetacm);
  Double_t u = -(s/2)*(1 + costhetacm);

  //Exact Double_t dSigmaPhi_dOmega =  -(((alpha*alpha*alpha * m_e * sinthetacm))/((8*sqrt(s)*t*t*u*u)))*(3*s*(t*(u-s)*log(-t/s)-u*(t-s)*log(-u/s))-2*(t-u)*t*u);
  Double_t dSigmaPhi_dOmega =  -(((alpha*alpha*alpha * m_e * sinthetacm))/((8*sqrt(s)*t*t*u*u)))*(3*s*(t*(u-s)*log(-t/s)-u*(t-s)*log(-u/s))-2*(t-u)*t*u);
 
  
  // Born-level differential cross section
  
  Double_t dSigmaBorn_dOmega =  ((alpha*alpha)/(2*s))*((t*t+t*u+u*u)/(t*u))*((t*t+t*u+u*u)/(t*u));



  Double_t B_n  =-1*(dSigmaPhi_dOmega/dSigmaBorn_dOmega)*1e6*0.43; // 43% transverse during 877 MeV

  return B_n;
}


// For E158 beam energy (as a test)
Double_t function(Double_t *x, Double_t *par)
{

  // General
  Double_t m_e = 0.511; //GeV
  Double_t alpha = 1.0/137; //fine structure constant
  
  // Qweak kinematics
  Double_t E = 46000; //MeV beam energy
  
  Double_t s =  2*m_e*E+m_e;

  Double_t costhetacm = (((m_e/E)-1+ TMath::Cos(TMath::ACos(x[0])))/((m_e/E)+1-TMath::Cos(TMath::ACos(x[0]))));
  //std::cout<<"cos "<<costhetacm<<std::endl;
  Double_t sinthetacm = sqrt(1 - costhetacm*costhetacm );
  //std::cout<<"sin "<<sinthetacm<<std::endl;

  Double_t t = -(s/2)*(1 - costhetacm);
  Double_t u = -(s/2)*(1 + costhetacm);

  //Exact Double_t dSigmaPhi_dOmega =  -(((alpha*alpha*alpha * m_e * sinthetacm))/((8*sqrt(s)*t*t*u*u)))*(3*s*(t*(u-s)*log(-t/s)-u*(t-s)*log(-u/s))-2*(t-u)*t*u);
  Double_t dSigmaPhi_dOmega =  -(((alpha*alpha*alpha * m_e * sinthetacm))/((8*sqrt(s)*t*t*u*u)))*(3*s*(t*(u-s)*log(-t/s)-u*(t-s)*log(-u/s))-2*(t-u)*t*u);


  // Double_t dSigmaPhi_dOmega =  -(((alpha*alpha*alpha * m_e * TMath::Sin(TMath::ACos(x[0]))))/((8*sqrt(s)*t*t*u*u)))*(3*s*(t*(u-s)*log(-t/s)-u*(t-s)*log(-u/s))-2*(t-u)*t*u);
  
  
  // Born-level differential cross section
  
  Double_t dSigmaBorn_dOmega =  ((alpha*alpha)/(2*s))*((t*t+t*u+u*u)/(t*u))*((t*t+t*u+u*u)/(t*u));



  Double_t B_n  =(dSigmaPhi_dOmega/dSigmaBorn_dOmega)*1e6;

  return B_n;
}

// Fit both Qweak and E158
void transverse_moller(){
  

  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.01);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadRightMargin(0.08);
  gStyle->SetPadLeftMargin(0.08);


  // histo parameters
  gStyle->SetTitleYOffset(0.8);
  gStyle->SetTitleXOffset(1.6);
  gStyle->SetLabelSize(0.05,"x");
  gStyle->SetLabelSize(0.05,"y");
  gStyle->SetTitleSize(0.05,"x");
  gStyle->SetTitleSize(0.05,"y");
 

  TCanvas * c = new TCanvas("c","Azimuthal coefficient #alpha_T at LO vs cosine of center of massangle in Qweak",0,0,1000,600);
  c->Draw();
  c->cd();
  TPad*pad1 = new TPad("pad1","pad1",0.005,0.800,0.990,0.990);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.990,0.800);
  pad1->SetFillColor(0);
  pad1->Draw();
  pad2->Draw();
  pad2->SetFillColor(0);
  pad1->cd();
  TPaveText *text = new TPaveText(.05,.1,.95,.8);
   
  text->AddText(" Leading Order Azimuthal asymmetry coefficient #alpha^{LO}_T vs #theta_{lab}");
  // text->AddText("  for E=1165MeV, sqrt(s)= 34 MeV (Qweak)");
  text->Draw();

  pad2->cd();
  TF1 *f1 = new TF1("func",fit_function,4,12,1);
  f1->SetParameters(1,2.0);
  f1->Draw();
  f1->SetLineColor(kRed);
  f1->GetXaxis()->SetTitle(" #theta_{lab}");
  f1->GetXaxis()->CenterTitle();
  f1->GetYaxis()->SetTitle(" #alpha_Tx10^{-6}");
  f1->GetYaxis()->CenterTitle();

  // Add the measurements
  Int_t n = 6;
  Double_t asym[8]={10.65,10.14,9.33,7.51,4.51,4.18}; //measured asymmetry (ppm)
  Double_t asyme[8]={0.84,0.62,0.54,0.74,0.67,1.35}; //measured asymmetry error(ppm)
  Double_t angle[8]={4.44,5.13,5.73,6.28,8.11,11.47};
  Double_t angle_e[8]={0.0,0.0,0.0,0.0,0.0,0.0};

  TGraphErrors* gr = new TGraphErrors(n,angle,asym,angle_e,asyme);
  gr->SetMarkerColor(4);
  gr->Draw("same");

//   TF1 *f2 = new TF1("myfunc",function, 0,10,1);
//   f2->SetParameters(1,-1.0);
//   f2->Draw("same");
  TLegend *legend = new TLegend(0.1,0.83,0.5,0.99,"","brNDC");
  legend->AddEntry(f1, "Qweak E=0.877GeV, P_T = 43% Theory", "l");
  legend->AddEntry(gr, "Qweak E=0.877GeV, P_T = 43% Measured", "l");
  //  legend->AddEntry(f2, "E158 (E=46GeV , #sqrt{s} #approx 217 MeV", "l");
  legend->Draw("");

  c->Print("transverse_moller.png");
  c->Print("transverse_moller.C");
  c->Print("transverse_moller.svg");

//   pad2->Clear();
//   f1 = new TF1("func",fit_function,-0.2,0.2,1);
//   f1->SetParameters(1,-0.2);
//   f1->Draw();
//   f1->SetLineColor(kRed);
//   f1->GetXaxis()->SetTitle(" cos#theta_{cm} (zoomed)");
//   f1->GetXaxis()->CenterTitle();
//   f1->GetYaxis()->SetTitle(" #alpha_Tx10^{-6}");
//   f1->GetYaxis()->CenterTitle();


//   f2 = new TF1("myfunc",function,-0.2,0.2,1);
//   f2->SetParameters(1,-0.2);
//   f2->Draw("same");


//   legend = new TLegend(0.1,0.83,0.5,0.99,"","brNDC");
//   legend->AddEntry(f1, "Qweak (E=1.165GeV , #sqrt{s} #approx 35 MeV", "l");
//   legend->AddEntry(f2, "E158 (E=46GeV , #sqrt{s} #approx 217 MeV", "l");
//   legend->Draw("");


  std::cout<<"Done!\n";


}

// For E158 : to check with results in the paper. 


void transverse_moller_E158(){
  
  TF1 *f1 = new TF1("myfunc",function,-1.0,1.0,1);
  f1->SetParameters(1,-1.0);
  f1->Draw();
  std::cout<<"Done!\n";
}


