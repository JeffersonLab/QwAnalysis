#include "TString.h"
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


//NOTE: this program uses the run<run_number>SineAmpl.dat and run<run_number>CosineAmpl.dat 
//files and expects the following ordering of the file entries: X, Y, E, XP, YP. If this
//changes, this file needs to be edited.



//Don Jones 
//June 2013
//This macro was created to check the results of the modulation code
//by applying the corrections recorded in the diagnostic file to the
//data and then fitting a sine curve to the corrected data to see if 
//there are any residual correlations. 
//THIS MACRO TESTS THE VALIDITY OF THE FINAL SLOPES AFTER INVERSION.

void getErrorWeightedMean(Double_t *x, Double_t *xe, Double_t *mean, Int_t n){
  Double_t sum = 0, esqinv = 0; 
  for(int i=0;i<n;i++){
    sum += x[i] / (xe[i] * xe[i]);
    esqinv += 1 / (xe[i] * xe[i]);
  }
  mean[0] = sum / esqinv;
  mean[1] = sqrt(1 / esqinv);
}

Int_t plotResidualCosAndSinCompBySlug(Int_t slug_start = 139, Int_t slug_end = 225){
  //  gStyle->SetOptFit(1111);
  char  x[255], xe[255], mon[255], monitor[255];
  string line;

  gStyle->SetOptFit(1111);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.25);
  gStyle->SetOptStat(0);

  const Int_t nDET = 9, nMOD = 5, nSLUGS = 1000;
  Double_t lY = -0.00025, hY = 0.00025;
  Double_t slugArray[nSLUGS];
  Double_t runsSErr[nMOD][nDET][nSLUGS], runsCErr[nMOD][nDET][nSLUGS];
  Int_t order[18] = {5,4,1,2,3,6,9,8,7}, nullErr[nMOD][nDET];
  Int_t slug[nSLUGS][100];
  TString MonitorList[nMOD];
  TString DetectorList[nDET] = {"qwk_mdallbars", "qwk_md1barsum", 
				"qwk_md2barsum", "qwk_md3barsum", 
				"qwk_md4barsum", "qwk_md5barsum",
				"qwk_md6barsum", "qwk_md7barsum", 
				"qwk_md8barsum"};//, "qwk_md9barsum",
// 				"qwk_dslumi1", "qwk_dslumi2",
// 				"qwk_dslumi3", "qwk_dslumi4", 
// 				"qwk_dslumi5", "qwk_dslumi6", 
// 				"qwk_dslumi7","qwk_dslumi8",
// 				"uslumi_sum","uslumi1_sum",
// 				"uslumi3_sum", "uslumi5_sum",
// 				"uslumi7_sum","qwk_pmtltg",
// 				"qwk_pmtonl","qwk_bcm1","qwk_bcm2",
// 				"qwk_bcm5","qwk_bcm6"};

  ifstream file(Form("%s/config/setup_mpsonly.config",gSystem->Getenv("BMOD_SRC")));
  for(int i=0;i<5;i++){
    file>>mon>>monitor;
    getline(file, line);
    MonitorList[i] = TString(monitor);
    cout<<MonitorList[i]<<endl;
  }

  TString detector;
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
  Double_t AvCorrectedDetectorSin[nMOD][nDET][nSLUGS], 
    AvCorrectedDetectorSinError[nMOD][nDET][nSLUGS],
    AvCorrectedDetectorCos[nMOD][nDET][nSLUGS], 
    AvCorrectedDetectorCosError[nMOD][nDET][nSLUGS],
    tempSin[nMOD][nDET][100], tempSinErr[nMOD][nDET][100],
    tempCos[nMOD][nDET][100], tempCosErr[nMOD][nDET][100];

  Int_t n[nMOD][nDET], nRuns[nMOD][nDET];

  for(int sl = 0; sl < nSlugs; sl++){
    Int_t good = 1; 
    for(int i=0;i<nDET;i++)
      for(int j=0;j<nMOD;j++){
	n[j][i] = 0;
	nRuns[j][i] = 0;
      }
    Bool_t atLeastOneGoodRun = 0;
    for(int r = 0; r<slug[sl][1];r++){
      ifstream cosFile(Form("%s/slopes/run%iCosineAmpl.dat",gSystem->Getenv("BMOD_OUT"),
			    slug[sl][r+2]));
      ifstream sinFile(Form("%s/slopes/run%iSineAmpl.dat",gSystem->Getenv("BMOD_OUT"),
			    slug[sl][r+2]));
      if(sinFile.is_open() && cosFile.is_open())
	cout<<"Sine and cosine files found for run "<<slug[sl][r+2]<<" in slug "<<
	  slug[sl][0]<<".\n";
	else 
	  cout<<"Sine and cosine files NOT found for run "<<slug[sl][r+2]<<" in slug "<<
	    slug[sl][0]<<".\n";
      if(sinFile.good()&& cosFile.good()){
	atLeastOneGoodRun = 1;
	for(int i=0;i<nDET;i++){
	  for(int j=0;j<nMOD;j++){
	    sinFile>>x>>xe;
	    if(atof(xe)>5e-6){
	      tempSin[j][i][nRuns[j][i]] = atof(x);
	      tempSinErr[j][i][nRuns[j][i]] = atof(xe);
// 	      cout<<tempSin[j][i][nRuns[j][i]]<<" "<< 
// 		tempSinErr[j][i][nRuns[j][i]]<<"\t";
	    }else good = 0;

	    cosFile>>x>>xe;
	    if(atof(xe)>5e-6){
	      tempCos[j][i][nRuns[j][i]] = atof(x);
	      tempCosErr[j][i][nRuns[j][i]] = atof(xe);
// 	      cout<<tempCos[j][i][nRuns[j][i]]<<" "<< 
// 		tempCosErr[j][i][nRuns[j][i]]<<"\t";
	    }else good = 0;
	    if(good) nRuns[j][i]++;
	  }
	  //	  cout<<"\n\n";
	}
      }
    }
    if(atLeastOneGoodRun&&slug[sl][0]!=224){
      slugArray[nSlug] = slug[sl][0];
      Double_t par[2];
      for(int i=0;i<nDET;i++){
	for(int j=0;j<nMOD;j++){
	  getErrorWeightedMean(tempSin[j][i], tempSinErr[j][i], par, nRuns[j][i]);
	  AvCorrectedDetectorSin[j][i][nSlug] = par[0]*1e6;
	  AvCorrectedDetectorSinError[j][i][nSlug] = par[1]*1e6;
	  getErrorWeightedMean(tempCos[j][i], tempCosErr[j][i], par, nRuns[j][i]);
	  AvCorrectedDetectorCos[j][i][nSlug] = par[0]*1e6;
	  AvCorrectedDetectorCosError[j][i][nSlug] = par[1]*1e6;
	}
      }
      nSlug++;
    }
  }

  cout<<nSlug<<" good slugs found.\n";
  TCanvas *cTalkSin = new TCanvas("cTalkSin", "cTalkSin",0,0,1500,1000);
  cTalkSin->Divide(5,3);
  TCanvas *cTalkCos = new TCanvas("cTalkCos", "cTalkCos",0,0,1500,1000);
  cTalkCos->Divide(5,3);
  TCanvas *cCos[nMOD]; 
  TCanvas *cSin[nMOD]; 
  //  TMultiGraph *mg[nMOD][nDET];
  TGraphErrors *grS[nMOD][nDET];
  TGraphErrors *grC[nMOD][nDET];
  Double_t fit[2][nMOD][nDET];
  Double_t fitError[2][nMOD][nDET];
  TF1 *f = new TF1("f","pol0");
  for(int j=0; j<nMOD;j++){
    cSin[j] = new TCanvas(Form("cSinMod%i",j),Form("cSinMod%i",j), 0,0,1600,1000);
    cSin[j]->Divide(3,3);
    for(int i=0;i<nDET;i++){
      cSin[j]->cd(order[i]);
      //      mg[j][i] = new TMultiGraph();
      //      mg[j][i]->SetTitle(Form("Residual %s Cosine & Sine vs Run", 
      //			      DetectorList[i].Data()));
      grS[j][i] = new TGraphErrors(nSlug,slugArray,AvCorrectedDetectorSin[j][i],runsSErr[j][i],
				   AvCorrectedDetectorSinError[j][j]);
      grS[j][i]->SetTitle(Form("Residual %s Sine Response vs Slug", 
			      DetectorList[i].Data()));
      grS[j][i]->SetLineColor(kRed);
      grS[j][i]->SetMarkerColor(kRed);
      grS[j][i]->SetMarkerStyle(7);
      grS[j][i]->Draw("ap");
      grS[j][i]->GetXaxis()->SetTitle("Slug");
      grS[j][i]->GetYaxis()->SetTitle(Form("%s(ppm)",DetectorList[i].Data()));
      grS[j][i]->GetYaxis()->SetTitleSize(0.05);
      grS[j][i]->GetXaxis()->SetTitleSize(0.05);
      //     grS[j][i]->GetYaxis()->SetTitleOffset(1.25);
      //      grS[j][i]->GetYaxis()->SetRangeUser(lY, hY);
      grS[j][i]->Draw("ap");
      grS[j][i]->Fit("f");
      fit[0][j][i] = f->GetParameter(0);
      fitError[0][j][i] = f->GetParError(0);
      gPad->Update();
      //      mg[j][i]->Add(grS[j][i]);
      if(i==0){
	cTalkSin->cd(1+j);
	grS[j][i]->Draw("ap");
	grS[j][i]->GetYaxis()->SetRangeUser(-100,100);
      }
      if(i==1){
	cTalkSin->cd(6+j);
	grS[j][i]->Draw("ap");
	grS[j][i]->GetYaxis()->SetRangeUser(-100,100);
      }
      if(i==3){
	cTalkSin->cd(11+j);
	grS[j][i]->Draw("ap");
	grS[j][i]->GetYaxis()->SetRangeUser(-100,100);
      }
      gPad->Update();
    }
    cCos[j] = new TCanvas(Form("cCosMod%i",j),Form("cCosMod%i",j), 0,0,1600,1000);
    cCos[j]->Divide(3,3);
    for(int i=0;i<nDET;i++){
      cCos[j]->cd(order[i]);
      grC[j][i] = new TGraphErrors(nSlug,slugArray,AvCorrectedDetectorCos[j][i],runsCErr[j][i],
				   AvCorrectedDetectorCosError[j][i]);
      grC[j][i]->SetTitle(Form("Residual %s Cosine Response vs Slug", 
			      DetectorList[i].Data()));
      grC[j][i]->SetLineColor(kBlue);
      grC[j][i]->SetMarkerColor(kBlue);
      grC[j][i]->SetMarkerStyle(7);
      //      mg[j][i]->Add(grC[j][i]);
    
      grC[j][i]->Draw("ap");
      grC[j][i]->GetXaxis()->SetTitle("Slug");
      grC[j][i]->GetYaxis()->SetTitle(Form("%s(ppm)",DetectorList[i].Data()));
      grC[j][i]->GetYaxis()->SetTitleSize(0.05);
      grC[j][i]->GetXaxis()->SetTitleSize(0.05);
      //      grC[j][i]->GetYaxis()->SetTitleOffset(1.1);
      //      grC[j][i]->GetYaxis()->SetRangeUser(lY, hY);
      grC[j][i]->Draw("ap");
      grC[j][i]->Fit("f");
      fit[1][j][i] = f->GetParameter(0);
      fitError[1][j][i] = f->GetParError(0);
      gPad->Update();
      if(i==0){
	cTalkCos->cd(1+j);
	grC[j][i]->Draw("ap");
	grC[j][i]->GetYaxis()->SetRangeUser(-100,100);
      }
      if(i==1){
	cTalkCos->cd(6+j);
	grC[j][i]->Draw("ap");
 	grC[j][i]->GetYaxis()->SetRangeUser(-100,100);
     }
      if(i==3){
	cTalkCos->cd(11+j);
	grC[j][i]->Draw("ap");
	grC[j][i]->GetYaxis()->SetRangeUser(-100,100);
      }
      gPad->Update();
   }
  }

  
//   printf("Table 1. Average residual Sine component for slugs %i to %i.\n"
// 	 "                 |          Xmod          |           Ymod         |"
// 	 "          Emod          |"
// 	 "          XPmod         |          YPmod         |", 
// 	 slug_start, slug_end);
//   printf("\n*********************************************************************"
// 	 "***********************************************************************"
// 	 "***\n");
//   for(Int_t i=0;i<nDET;i++){
//     TString det = DetectorList[i];
//     Bool_t good = 0;
//     det.Resize(16);
//     printf("%s%s |", ANSI_COLOR_RESET,det.Data());
//     for(Int_t j=0;j<nMOD;j++){
//       if(TMath::Abs(fit[0][j][i])<TMath::Abs(fitError[0][j][i]))
// 	good = 1;
//       printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
// 	     fit[0][j][i]);
//       printf("%s+/-%7.3e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
// 	     fitError[0][j][i],ANSI_COLOR_RESET);
//     }
//     printf("\n");
//   }
//   printf("\n\n");

//   printf("Table 2. Average residual Cosine component for slugs %i to %i.\n"
// 	 "                 |          Xmod          |           Ymod         |"
// 	 "          Emod          |"
// 	 "          XPmod         |          YPmod         |", 
// 	 slug_start, slug_end);
//   printf("\n*********************************************************************"
// 	 "***********************************************************************"
// 	 "***\n");
//   for(Int_t i=0;i<nDET;i++){
//     TString det = DetectorList[i];
//     det.Resize(16);
//     Bool_t good = 0;
//     printf("%s%s |", ANSI_COLOR_RESET,det.Data());
//     for(Int_t j=0;j<nMOD;j++){
//       if(TMath::Abs(fit[1][j][i])<TMath::Abs(fitError[1][j][i]))
// 	good = 1;
//       printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
// 	     fit[1][j][i]);
//       printf("%s+/-%7.3e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
// 	     fitError[1][j][i],ANSI_COLOR_RESET);
//     }
//     printf("\n");
//   }
//   printf("\n\n");

  return 0;
}











