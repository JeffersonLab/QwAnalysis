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
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//Don Jones 
//June 2013
//This macro plots the error weighted slopes vs slug. It reads the slopes for 
//each run from the output file in $BMOD_OUT/slopes/slopes_<run_number>.set4.dat
//The monitors and detectors are read in from the config file so that the ordering
//will always be right provided someone does not change the config file after
//the slopes files have been created.

void getErrorWeightedMean(Double_t *x, Double_t *xe, Double_t *mean, Int_t n){
  Double_t sum = 0, esqinv = 0; 
  for(int i=0;i<n;i++){
    sum += x[i] / (xe[i] * xe[i]);
    esqinv += 1 / (xe[i] * xe[i]);
  }
  mean[0] = sum / esqinv;
  mean[1] = sqrt(1 / esqinv);
}

Int_t plotInvertedSlopesBySlug_bkgd(Int_t slug_start = 139, Int_t slug_end = 225){
  //  gStyle->SetOptFit(1111);
  char  x[255], xe[255], mon[255], monitr[255], det[255], detectr[255];
  string line;

  Bool_t ppm_per_micro = 1;
  gStyle->SetOptFit(1111);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.25);
  gStyle->SetOptStat(0);

  const Int_t nDET = 4, nMOD = 5, nSLUGS = 1000;
  Double_t slugArray[nSLUGS], slugArrayErr[nSLUGS];
  Int_t order[4] = {1,2,3,4};
  Int_t slug[nSLUGS][100];
  TString MonitorList[nMOD];
  TString DetectorList[nDET];

  char  *ModName[nMOD];
  ModName[0] = "X";
  ModName[1] = "XP";
  ModName[2] = "E";
  ModName[3] = "Y";
  ModName[4] = "YP"; 

  char  *unitsMon[nMOD];
  Double_t unitConv[5] = {1.,1.e-3,1.,1.,1.e-3};
  char  *unitsDet = "frac";
  unitsMon[0] = "mm";
  unitsMon[1] = "mrad";
  unitsMon[2] = "mm";
  unitsMon[3] = "mm";
  unitsMon[4] = "mrad"; 

  if(ppm_per_micro){
    for(int i=0;i<5;i++)
      unitConv[i] *= 1000;                  ;
    unitsDet = "ppm";
    unitsMon[0] = "#mum";
    unitsMon[1] = "#murad";
    unitsMon[2] = "#mum";
    unitsMon[3] = "#mum";
    unitsMon[4] = "#murad"; 
  }


  ifstream file(Form("%s/config/setup_mpsonly_bkgd.config",gSystem->Getenv("BMOD_SRC")));
  for(int i=0;i<5;i++){
    file>>mon>>monitr;
    getline(file, line);
    MonitorList[i] = TString(monitr);
    cout<<MonitorList[i]<<endl;
  }
  getline(file, line);
  for(int i=0;i<nDET;i++){
    file>>det>>detectr;
    getline(file, line);
    DetectorList[i] = TString(detectr);
    cout<<DetectorList[i]<<endl;
  }

  //  TString detector;
  ifstream slugList(Form("%s/macros/runsBySlug.dat", 
			 gSystem->Getenv("BMOD_SRC")));
  Int_t nSlugs = 0, nSlug = 0;;
  if(slugList.is_open())cout<<"File list found.\n";
  else cout<<"File list not found.\n";

  while(slugList.good()){
    Int_t n = 2;//reserve n=0 for slugnumber and n=1 for number of runs in slug
    slugList>>x;
    if(atoi(x)>slug_end) break;
    slug[nSlug][0] = atoi(x);
    slugArray[nSlug] = (Double_t)slug[nSlug][0];
    cout<<atoi(x)<<" ";
    while(slugList.peek()!= '\n'){
      slugList>>x;
      slug[nSlug][n] = atoi(x);
      cout<<atoi(x)<<" ";
      n++;
    }
    getline(slugList, line);
    cout<<endl;
    slug[nSlug][1] = n - 2;
    if(slug_start <= slug[nSlug][0]){
      cout<<slug[nSlug][1]<<" runs in slug "<<slug[nSlug][0]<<" found.\n";
      nSlug++;
    }
  }
  nSlugs = nSlug;
  nSlug = 0;

  //Slopes obtained from data files
  Double_t AvDetectorSlope[nMOD][nDET][nSLUGS], 
    AvDetectorSlopeError[nMOD][nDET][nSLUGS],
    temp[nMOD][nDET][100], tempErr[nMOD][nDET][100];

  Int_t n[nMOD][nDET], nRuns;

  for(int sl = 0; sl < nSlugs; sl++){
    for(int i=0;i<nDET;i++)
      for(int j=0;j<nMOD;j++){
	n[j][i] = 0;
      }
    nRuns = 0;
    for(int r = 0; r<slug[sl][1];r++){
      ifstream slopesFile(Form("%s/slopes/slopes_%i.set4.dat",
			       gSystem->Getenv("BMOD_OUT"), slug[sl][r+2]));
      if(slopesFile.is_open())
	cout<<"Slope file found for run "<<slug[sl][r+2]<<" in slug "<<
	  slug[sl][0]<<".\n";
	else 
	  cout<<"Slope file NOT found for run "<<slug[sl][r+2]<<" in slug "<<
	    slug[sl][0]<<".\n";
      if(slopesFile.good()){
	Int_t allGood = 0;

	for(int i=0;i<nDET;i++){  
	  while(getline(slopesFile, line)){
           if(line.find(DetectorList[i].Data())==std::string::npos){
		continue;
           }
           else
		cout<<"Found slopes for detector "<<DetectorList[i].Data()<<endl;
		break;
	}
	  for(int j=0;j<nMOD;j++){
	    slopesFile>>x>>xe;
	    if(atof(xe)>5e-10){
	      allGood++;
	      temp[j][i][nRuns] = atof(x) * unitConv[j];
	      tempErr[j][i][nRuns] = atof(xe) * unitConv[j];
	      cout<<temp[j][i][nRuns]<<" "<< 
		tempErr[j][i][nRuns]<<"\t";
	    }
	    getline(slopesFile, line);
	  }
	  cout<<endl;
	  slopesFile.clear();
	  slopesFile.seekg(0,std::ios::beg);
	}
	if(allGood==nDET*nMOD){
	  nRuns++;
	}
      }
    }


    if(nRuns>0){
      slugArray[nSlug] = slug[sl][0];
      slugArrayErr[nSlug] = 0;
      Double_t par[2];
      for(int i=0;i<nDET;i++){
	for(int j=0;j<nMOD;j++){
	  getErrorWeightedMean(temp[j][i], tempErr[j][i], par, nRuns);
	  AvDetectorSlope[j][i][nSlug] = par[0];
	  AvDetectorSlopeError[j][i][nSlug] = par[1];
	}
      }
      nSlug++;
    }
  }
  cout<<nSlug<<" good slugs found.\n";
  for(int i=0;i<nMOD;i++)
    MonitorList[i].Replace(0,4,"");
  for(int i=0;i<nDET;i++)
    DetectorList[i].Replace(0,4,"");

  TLine *tl1; 
  TLine *tl2 ;
  TCanvas *cInvertedSlope[nMOD]; 
  TGraphErrors *gr[nMOD][nDET];

  for(int j=0; j<nMOD;j++){
    cInvertedSlope[j] = new TCanvas(Form("cInvertedSlopeMod%i",j),
			  Form("cInvertedSlopeMod%i",j), 0,0,1600,1000);
    cInvertedSlope[j]->Divide(2,2);
    for(int i=0;i<nDET;i++){
      cInvertedSlope[j]->cd(order[i]);
      gr[j][i] = new TGraphErrors(nSlug,slugArray,AvDetectorSlope[j][i],
				   slugArrayErr, AvDetectorSlopeError[j][j]);
      gr[j][i]->SetTitle(Form("(Fully Inverted Slope) %s to %s", 
			      DetectorList[i].Data(), MonitorList[j].Data()));
      gr[j][i]->SetLineColor(kViolet);
      gr[j][i]->SetMarkerColor(kViolet);
      gr[j][i]->SetMarkerStyle(7);
      gr[j][i]->Draw("ap");
      gr[j][i]->GetXaxis()->SetTitle("Slug");
      gr[j][i]->GetYaxis()->SetTitle(Form("%s / %s (%s/%s)",
					  DetectorList[i].Data(), 
					  MonitorList[j].Data(), unitsDet, 
					  unitsMon[j]));
      gr[j][i]->GetYaxis()->SetTitleSize(0.045);
      gr[j][i]->GetYaxis()->SetTitleOffset(1.2);
      gr[j][i]->GetXaxis()->SetTitleSize(0.045);
     //      grS[j][i]->GetYaxis()->SetRangeUser(lY, hY);
       tl1 = new TLine(160,gr[j][i]->GetYaxis()->GetXmin(),160,
		      gr[j][i]->GetYaxis()->GetXmax());
      tl1->SetLineWidth(3);  
      tl1->SetLineStyle(2);        
      tl1->Draw("same");
      tl2 = new TLine(172,gr[j][i]->GetYaxis()->GetXmin(),
		      172,gr[j][i]->GetYaxis()->GetXmax());
      tl2->SetLineWidth(3);  
      tl2->SetLineStyle(2);        
      tl2->Draw("same");
//       gr[j][i]->Fit("f");
//       fit[0][j][i] = f->GetParameter(0);
//       fitError[0][j][i] = f->GetParError(0);
      gPad->Update();

    }
  }


  return 0;
}











