//*****************************************************************************************************//
// Author : B. Waidyawansa
// Date   : Sepetember 27th 2012
//*****************************************************************************************************//
/*

A Macro to draw transverse correction histograms using inputs from a text file
To run.
.L DrawTCorrectionHistoFromFile.C+g
 DrawWtdHistograms(filename) {to draw histograms weighted by good number of patterns in mdallbars}
or
 DrawUWtdHistograms(filename) {to draw histograms unweighted}

 *filename* is the name of the text file containint the information.
The txt file should have the following format:
    in slug, out slug, entries,Pv, dPv, Ph,dPh, C, dC, reduced chi square

As a first attempt, only Pv, Ph, reduced chi-square and the correction histograms are drawn.

Correction is calculated based on the equations in elog https://qweak.jlab.org/elog/Analysis+%26+Simulation/758


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

//Draw histogram of corrections weighted by number of good entries in mdallbars
void DrawWtdHistograms(TString filename){

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
  gStyle->SetEndErrorSize(3);
  gStyle->SetErrorX(1.);

  gDirectory->Delete();

  // Constant values associated with the leakage
  const Double_t C_v  = 0.008; //ppm
  const Double_t dC_v = 0.072; //ppm
  const Double_t C_h  = 0.004; //ppm
  const Double_t dC_h = 0.064; //ppm
  Double_t leakage  = 0.0;
  Double_t dleakage = 0.0;


  string line;
  ifstream txtfile (filename+".txt");
  size_t found=0;

  Double_t P_v=0.0;
  Double_t P_h=0.0;
  Double_t dP_v=0.0;
  Double_t dP_h=0.0;
  Double_t C=0.0;
  Double_t dC=0.0;
  Double_t n=0.0;
  Double_t rchi=0.0;

  TH1F* h_lkg  =new TH1F("Leakage", "Transverse Leakage;Absolute leakage |C_{V}*P_{V}|+|C_{H}*P_{H}| (ppb);Patterns per slug pair", 50, -0.8, 1.2); 
  TH1F* h_ph   =new TH1F("P_{H}", "Residual Horizontal Transverse;Polarization;Patterns per slug pair", 40, -0.205, 0.195); // to get bin sizes of 0.01, set bin number = 20
  TH1F* h_pv   =new TH1F("P_{V}", "Residual Vertical Transverse;Polarization;Patterns per slug pair", 40, -0.205, 0.195);
  TH1F* h_rchi =new TH1F("Reduced #chi^{2}", "Reduced Chi-square of the fits;Reduced Chi-square;Patterns per slug pair", 25, 0, 5.0);
  TH1F* h_C    =new TH1F("C",  "Constant term;ppm;Patterns per slug pair", 25, 0, 5.0);
  TH1F* h_dlkg =new TH1F("dC", "Error on lekage", 50, -0.8, 1.2);
  
  // Calculate bin error as sqrt(sum(squares of weights))
  h_lkg ->Sumw2();
  h_ph  ->Sumw2();
  h_pv  ->Sumw2();
  h_rchi->Sumw2();
  h_C   ->Sumw2();
  h_dlkg ->Sumw2();

  // Read and load the values
  if (txtfile.is_open())
  {
    while ( txtfile.good() and !(txtfile.eof()))
    {
	getline (txtfile,line);
	//	cout << line << endl;
	while(!line.empty()){
	  //read from last "," 
	  found=line.find_last_of(",");
	  rchi = atof((line.substr(found+1,string::npos)).c_str()); // last column is the value of reduced chisquare
	  line.erase (found,string::npos); // erase from end to ",".

	  //read next ","
	  found=line.find_last_of(",");
	  dC   = atof((line.substr(found+1,string::npos)).c_str()); // next to last is dC
	  line.erase (found,string::npos); // erase	  
	  
	  //read next ","
	  found=line.find_last_of(",");
	  C   = atof((line.substr(found+1,string::npos)).c_str()); // next is C
	  line.erase (found,string::npos); // erase	  

	  //read next ","
	  found=line.find_last_of(",");
	  dP_h   = atof((line.substr(found+1,string::npos)).c_str()); // next is dP_h
	  line.erase (found,string::npos); // erase	  

	  //read next ","
	  found=line.find_last_of(",");
	  P_h   = atof((line.substr(found+1,string::npos)).c_str()); // next is P_h
	  line.erase (found,string::npos); // erase	  

	  //read next ","
	  found=line.find_last_of(",");
	  dP_v   = atof((line.substr(found+1,string::npos)).c_str()); // next is dP_v
	  line.erase (found,string::npos); // erase	  

	  //read next ","
	  found=line.find_last_of(",");
	  P_v   = atof((line.substr(found+1,string::npos)).c_str()); // next is P_v
	  line.erase (found,string::npos); // erase	  

	  //read next ","
	  found=line.find_last_of(",");
	  n   = atof((line.substr(found+1,string::npos)).c_str()); // next is number of patterns
	  line.erase (found,string::npos); // erase	  


	  // Calculate leakage and error
	  // Calculate leakage
	  leakage  =0;
	  leakage  = (TMath::Abs(C_v*P_v)+TMath::Abs(C_h*P_h))*1000; //convert to ppb
	  dleakage = sqrt( pow((C_v*dP_v),2)+pow((dC_v*P_v),2)+pow((C_h*dP_h),2)+pow((dC_h*P_h),2) );

	  // Fill histograms
	  h_ph  ->Fill(P_h,n);
	  std::cout<<"n = "<<n<<std::endl;
	  std::cout<<"sum of weights "<<h_ph->GetSumOfWeights()<<std::endl;
	  h_pv  ->Fill(P_v,n);
	  //  h_C   ->Fill(C,n);
	  h_rchi->Fill(rchi,n);
	  h_lkg ->Fill(leakage,n);
	  h_dlkg ->Fill(dleakage,n);

	  line.clear();
	}
    }
    txtfile.close();

  }
  else {
    std::cout << "Unable to open file"; 
    exit(1);
  }

  TCanvas * c = new TCanvas("c","",0,0,1200,1000);
  c->Draw();
  c->Divide(2,2);
  c->SetFillColor(kWhite);

  c->cd(1);
  h_ph->Draw();
  h_ph->Fit("gaus");
  h_ph->SetLineColor(kAzure-5);
  h_ph->SetMarkerColor(kAzure-5);
  h_ph->SetMarkerStyle(21);
  h_ph->SetMarkerSize(0.8);
  gPad->SetFillColor(kWhite);

  c->cd(2);
  h_pv->Draw();
  h_pv->Fit("gaus");
  h_pv->SetLineColor(kRed-3);
  h_pv->SetMarkerColor(kRed-3);
  h_pv->SetMarkerStyle(21);
  h_pv->SetMarkerSize(0.8);
  gPad->SetFillColor(kWhite);

  c->cd(4);
  h_lkg->Draw();
  h_lkg->Fit("gaus");
  h_lkg->SetLineColor(kViolet-6);
  h_lkg->SetMarkerColor(kViolet-6);
  h_lkg->SetMarkerStyle(21);
  h_lkg->SetMarkerSize(0.8);
  gPad->SetFillColor(kWhite);

  c->cd(3);
  h_rchi->Draw();
  h_rchi->Fit("gaus");
  h_rchi->SetLineColor(kGreen+2);
  h_rchi->SetMarkerColor(kGreen+2);
  h_rchi->SetMarkerStyle(21);
  h_rchi->SetMarkerSize(0.8);
  gPad->SetFillColor(kWhite);

  c->Modified();
  c->Update();

  c->Print(filename+".png");
  c->Print(filename+".C");


}


// Draw unweighted histograms
void DrawUWtdHisto(TString filename){

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


  string line;
  ifstream txtfile (filename);
  size_t found=0;

  Double_t val;
  Double_t w;

  std::vector<float> Pv;
  std::vector<float> Ph;

  TH1F* h_chim =new TH1F("Ph", "Reduced Chi-square of the fits;Reduced Chi-square;Patterns per slug pair", 25, 0, 5.0);

  // Read and load the values
  if (txtfile.is_open())
  {
    while ( txtfile.good() and !(txtfile.eof()))
    {
	getline (txtfile,line);
	cout << line << endl;
	while(!line.empty()){
	  found=line.find_last_of(",");
	  val = atof((line.substr(found+1,string::npos)).c_str()); //read from last "," to end:  this is the value 
	  line.erase (found,string::npos); // erase	  
	  w = atof(line.c_str()); //whats left is number of patterns
	  
	  h_chim->Fill(val,w);
	  line.clear();
	}
    }
    txtfile.close();

  }
  else {
    std::cout << "Unable to open file"; 
    exit(1);
  }

  TCanvas * c = new TCanvas("c","",0,0,800,500);
  c->Draw();
  c->SetFillColor(kWhite);
  h_chim->Draw();
  h_chim->Fit("gaus");
  h_chim->SetFillColor(kGreen+1);
  h_chim->SetFillStyle(3002);

  c->Modified();
  c->Update();

}

