//Test file for slug calc class
//J. Magee
//February 14, 2013
//slugcalc.cc

#include <iostream>
#include <iomanip>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>

#include <TFile.h>
#include <TTree.h>
#include <TROOT.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TStyle.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <TLegend.h>
#include <TPaveStats.h>

#include "SlugCalc.h"
#include "SlugCalcVec.h"

using namespace std;

int main(void) {

  SlugCalc mySlugCalc(5);

  std::cout <<"Set mySlugCalc to " <<mySlugCalc.GetSlug() <<std::endl;

  mySlugCalc.Push(30,87.38,0.71);
  mySlugCalc.Push(38,88.03, 0.66);

  mySlugCalc.PrintVal();
  mySlugCalc.PrintError();

  std::cout <<"The weighted average is: \t" <<mySlugCalc.GetAvg() <<" +- " <<mySlugCalc.GetError() <<std::endl;

  return 0;
} //end test function

/*
void calculate( TString filename = "dummy2.dat" ) {
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  Bool_t debug  = 0;
  Int_t  runlet = 0;
  Int_t  count  = 0;

  Float_t run  = 0;
  Int_t   slug = 0;
  Int_t   ihwp = -1;
  Float_t data = 0;
  Float_t rms  = 0;

  SlugCalcVec timothy;

  TCanvas *canvas = new TCanvas("canvas");
  TMultiGraph *multi  = new TMultiGraph("multi","");

  std::cout <<filename  <<std::endl;
  ifstream file;
  file.open(filename,ios::in);

  if (!file.is_open()) {
    std::cout <<"Your file isn't open.\n Exiting progam.\n";
    return;
  }
  std::cout << "Confirmed file is open...\n";

  while( true ) {
    runlet++;

    file >> run; //read first to make sure we trigger eof when done reading

    if ( !file.eof() ) {
      file >> slug;
      file >> ihwp;
      file >> data;
      file >> rms;

      if (data!=0) timothy.push_back(slug,runlet,data,rms);

      if (debug==1) {
        std::cout <<runlet <<" \t";
        std::cout <<run  <<" \t";
        std::cout <<slug <<" \t";
        std::cout <<ihwp <<" \t";
        std::cout <<data <<" \t";
        std::cout <<rms  <<std::endl;
      } //end debug if
      continue;
    }
    break;
  } //end while loop

  file.close();
  std::cout << "File is now closed...\n";

  TGraphErrors *grIn = new TGraphErrors(timothy.size(),timothy.GetSlugsArray(),timothy.GetAvgArray(),0,timothy.GetErrorArray());

  grIn->SetMarkerColor(9);
  grIn->SetMarkerStyle(20);
  grIn->SetLineColor(9);
  grIn->SetMarkerSize(1.5);

  multi->Add(grIn);

  canvas->cd();
  multi->Draw("ap");

  multi->SetTitle("Asymmetry: Slugs 1021-1026");
  multi->GetYaxis()->SetTitle("Asymetry (ppm)");
  multi->GetXaxis()->SetTitle("Runlets (chronologically ordered)");
//  multi->SetMinimum(-250);
//  multi->SetMaximum(250);

//  multi->SetTitle("Charge Asymmetry in (good) Slugs 1021-1026");
//  multi->GetYaxis()->SetTitle("Charge asymmetry (ppm)");
//  multi->GetXaxis()->SetTitle("Runlet count");
  multi->GetYaxis()->SetTitleOffset(1.3);
  multi->GetXaxis()->SetTitleOffset(1.3);

  gPad->Modified();
  canvas->SetGrid();
  canvas->Update();
  canvas->Modified();

  timothy.PrintSlugVec();



} //end of function calculate

*/









