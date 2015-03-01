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

int excludedRun( vector<int> list, int run, int length){
  int found = 0;
  for(int i=0;i<length&&!found;i++){
    if(run == list[i])found = 1;
  }
  return found;
}

Int_t plotResidualCosAndSinCompBySlug(Int_t slug_start = 139, Int_t slug_end = 225){
  //  gStyle->SetOptFit(1111);
  char slg[255], x[255], xe[255], det[255], detectr[255], mon[255], monitor[255];
  string line;

  gStyle->SetOptFit(1111);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.25);
  gStyle->SetOptStat(0);

  const Int_t nDET = 9, nMOD = 5, nSLUGS = 1000;
  Double_t lY = -0.00025, hY = 0.00025;
  Double_t slugArray[nSLUGS],slugArrayErr[nSLUGS];
  Double_t runsSErr[nMOD][nDET][nSLUGS], runsCErr[nMOD][nDET][nSLUGS];
  Int_t order[18] = {5,4,1,2,3,6,9,8,7}, nullErr[nMOD][nDET];
  Int_t slug[nSLUGS][100];
  TString MonitorList[nMOD];
  TString DetectorList[nDET];

  ifstream exclRunsFile(Form("%s/macros/excludedRuns.dat",gSystem->Getenv("BMOD_SRC")));
  if(!exclRunsFile.is_open()){
    cout<<"Excluded runs file not found. Exiting.\n";
    return 1;
  }
  vector<int> exclRuns;
  int nExcl = 0;
  while(!exclRunsFile.eof()){
    exclRunsFile>>slg>>x;
    getline(exclRunsFile, line);
    exclRuns.push_back(atoi(x));
    nExcl++;
    exclRunsFile.peek();
  }
  exclRunsFile.close();
  cout<<nExcl<<" excluded runs.\n";

  ifstream file(Form("%s/config/setup_mpsonly.config",gSystem->Getenv("BMOD_SRC")));
  for(int i=0;i<5;i++){
    file>>mon>>monitor;
    getline(file, line);
    MonitorList[i] = TString(monitor);
    cout<<MonitorList[i]<<endl;
  }
  getline(file, line);
  for(int i=0;i<nDET;i++){
    file>>det>>detectr;
    getline(file, line);
    DetectorList[i] = TString(detectr);
    cout<<DetectorList[i]<<endl;
  }

  TString detector;
  ifstream slugList(Form("%s/macros/runsBySlug.dat", 
			 gSystem->Getenv("BMOD_SRC")));
  Int_t nSlugs = 0, nSlug = 0, prevSlug = 0;
  if(slugList.is_open())cout<<"File list found.\n";
  else cout<<"File list not found.\n";

  Int_t nRn = 0, done = 0;
  while(slugList.good()){
    slugList>>slg>>x;
    getline(slugList,line);
    if(slugList.eof())break;
    Int_t sl = atoi(slg); 
    if(sl>slug_end) break;
    if(sl>=slug_start && sl<=slug_end){
      if(sl!=prevSlug){
	slug[nSlug][0] = sl;
	nSlug++;
	nRn = 0;
      }
      cout<<"Her"<<nSlug<<endl;
      slug[nSlug-1][nRn+2] = atoi(x);
      cout<<slug[nSlug-1][nRn+2]<<" : nRn "<<nRn<<" : nSlug "<<nSlug<<endl; 
    }
    prevSlug = sl;
    nRn++;
    slug[nSlug-1][1] = nRn;
  }
  nSlugs = nSlug;
  nSlug = 0;
  cout<<"nRn "<<nRn<<endl;
  //  cout<<"slug[0][1]= "<<slug[0][1]<<endl;
  //Slopes obtained from data files
  Double_t AvCorrectedDetectorSin[nMOD][nDET][nSLUGS], 
    AvCorrectedDetectorSinError[nMOD][nDET][nSLUGS],
    AvCorrectedDetectorCos[nMOD][nDET][nSLUGS], 
    AvCorrectedDetectorCosError[nMOD][nDET][nSLUGS],
    tempSin[nMOD][nDET][100], tempSinErr[nMOD][nDET][100],
    tempCos[nMOD][nDET][100], tempCosErr[nMOD][nDET][100];

  Int_t nRuns = 0;

  for(int sl = 0; sl < nSlugs; sl++){
    Int_t good = 1; 
    Bool_t atLeastOneGoodRun = 0;
    cout<<"slug[sl][1] "<<slug[sl][1]<<endl;
    nRuns = 0;

    for(int r = 0; r<slug[sl][1];r++){
      if(!excludedRun(exclRuns, slug[sl][r+2], nExcl)){
	ifstream cosFile(Form("%s/slopes/run%iCosineAmpl.dat",
			      gSystem->Getenv("BMOD_OUT"), slug[sl][r+2]));
	ifstream sinFile(Form("%s/slopes/run%iSineAmpl.dat",
			      gSystem->Getenv("BMOD_OUT"), slug[sl][r+2]));
	if(sinFile.is_open() && cosFile.is_open())
	  cout<<"Sine and cosine files found for run "<<slug[sl][r+2]<<" in slug "<<
	    slug[sl][0]<<".\n";
	else 
	  cout<<"Sine and cosine files NOT found for run "
	      <<slug[sl][r+2]<<" in slug "<<slug[sl][0]<<".\n";
	sinFile.peek();//if file exists but is empty this will set eofbit
	cosFile.peek();//if file exists but is empty this will set eofbit
	if(sinFile.good()&& cosFile.good()&&!sinFile.eof()&&!cosFile.eof()){
	  atLeastOneGoodRun = 1;
	  for(int i=0;i<nDET;i++){
	    for(int j=0;j<nMOD;j++){
	      sinFile>>x>>xe;
	      tempSin[j][i][nRuns] = atof(x);
	      tempSinErr[j][i][nRuns] = atof(xe);
	      if(slug[sl][r+2]==15343)
		cout<<tempSin[j][i][nRuns]<<" "<< 
		  tempSinErr[j][i][nRuns]<<"\t";
	      cout<<tempSin[j][i][nRuns]<<" "<<
		tempSinErr[j][i][nRuns]<<"\t";
	      cosFile>>x>>xe;
	      tempCos[j][i][nRuns] = atof(x);
	      tempCosErr[j][i][nRuns] = atof(xe);
	      if(slug[sl][r+2]==15343)
		cout<<tempCos[j][i][nRuns]<<" "<< 
		  tempCosErr[j][i][nRuns]<<"\t";

	    }
	  cout<<"\n";
	  }
	  nRuns++;
	}
      }
    }
    cout<<nRuns<<" runs found.\n";

    if(atLeastOneGoodRun&&slug[sl][0]){
      slugArray[nSlug] = slug[sl][0];
      slugArrayErr[nSlug] = 0;
      Double_t par[2];
      for(int i=0;i<nDET;i++){
	for(int j=0;j<nMOD;j++){
	  getErrorWeightedMean(tempSin[j][i], tempSinErr[j][i], par, nRuns);
	  AvCorrectedDetectorSin[j][i][nSlug] = par[0]*1.e6;
	  AvCorrectedDetectorSinError[j][i][nSlug] = par[1]*1.e6;
	  getErrorWeightedMean(tempCos[j][i], tempCosErr[j][i], par, nRuns);
	  AvCorrectedDetectorCos[j][i][nSlug] = par[0]*1.e6;
	  AvCorrectedDetectorCosError[j][i][nSlug] = par[1]*1.e6;
	}
      }
      nSlug++;
    }
  }

  cout<<nSlug<<" good slugs found.\n";
  
//   TCanvas *cTalkSin = new TCanvas("cTalkSin", "cTalkSin",0,0,1500,1000);
//   cTalkSin->Divide(5,3);
//   TCanvas *cTalkCos = new TCanvas("cTalkCos", "cTalkCos",0,0,1500,1000);
//   cTalkCos->Divide(5,3);
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
      grS[j][i] = new TGraphErrors(nSlug,slugArray,AvCorrectedDetectorSin[j][i],
				   slugArrayErr,
				   AvCorrectedDetectorSinError[j][j]);
      grS[j][i]->SetTitle(Form("Residual %s Sine Response vs Slug", 
			      DetectorList[i].Data()));
      grS[j][i]->SetLineColor(kRed);
      //      grS[j][i]->SetMarkerColor(kRed);
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
//       if(i==0){
// 	cTalkSin->cd(1+j);
// 	grS[j][i]->Draw("ap");
// 	grS[j][i]->GetYaxis()->SetRangeUser(-100,100);
//       }
//       if(i==1){
// 	cTalkSin->cd(6+j);
// 	grS[j][i]->Draw("ap");
// 	grS[j][i]->GetYaxis()->SetRangeUser(-100,100);
//       }
//       if(i==3){
// 	cTalkSin->cd(11+j);
// 	grS[j][i]->Draw("ap");
// 	grS[j][i]->GetYaxis()->SetRangeUser(-100,100);
//       }
//      gPad->Update();
    }
    cSin[j]->SaveAs(Form("~/plots/SineResSlugs%ito%iMod%i.png", 
			 slug_start,slug_end,j));

    cCos[j] = new TCanvas(Form("cCosMod%i",j),Form("cCosMod%i",j), 0,0,1600,1000);
    cCos[j]->Divide(3,3);
    for(int i=0;i<nDET;i++){
      cCos[j]->cd(order[i]);
      grC[j][i] = new TGraphErrors(nSlug,slugArray,AvCorrectedDetectorCos[j][i],
				   slugArrayErr,AvCorrectedDetectorCosError[j][i]);
      grC[j][i]->SetTitle(Form("Residual %s Cosine Response vs Slug", 
			      DetectorList[i].Data()));
      grC[j][i]->SetLineColor(kBlue);
      //      grC[j][i]->SetMarkerColor(kBlue);
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
//       if(i==0){
// 	cTalkCos->cd(1+j);
// 	grC[j][i]->Draw("ap");
// 	grC[j][i]->GetYaxis()->SetRangeUser(-100,100);
//       }
//       if(i==1){
// 	cTalkCos->cd(6+j);
// 	grC[j][i]->Draw("ap");
//  	grC[j][i]->GetYaxis()->SetRangeUser(-100,100);
//      }
//       if(i==3){
// 	cTalkCos->cd(11+j);
// 	grC[j][i]->Draw("ap");
// 	grC[j][i]->GetYaxis()->SetRangeUser(-100,100);
//       }
//       gPad->Update();
    }
    cCos[j]->SaveAs(Form("~/plots/CosineResSlugs%ito%iMod%i.png", 
			 slug_start,slug_end,j));
  }

  
  printf("Table 1. Average residual Sine component for slugs %i to %i.\n"
	 "                 |          Xmod          |           Ymod         |"
	 "          Emod          |"
	 "          XPmod         |          YPmod         |", 
	 slug_start, slug_end);
  printf("\n*********************************************************************"
	 "***********************************************************************"
	 "***\n");
  for(Int_t i=0;i<nDET;i++){
    TString det = DetectorList[i];
    Bool_t good = 0;
    det.Resize(16);
    printf("%s%s |", ANSI_COLOR_RESET,det.Data());
    for(Int_t j=0;j<nMOD;j++){
      if(TMath::Abs(fit[0][j][i])<TMath::Abs(fitError[0][j][i]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     fit[0][j][i]);
      printf("%s+/-%7.3e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     fitError[0][j][i],ANSI_COLOR_RESET);
    }
    printf("\n");
  }
  printf("\n\n");

  printf("Table 2. Average residual Cosine component for slugs %i to %i.\n"
	 "                 |          Xmod          |           Ymod         |"
	 "          Emod          |"
	 "          XPmod         |          YPmod         |", 
	 slug_start, slug_end);
  printf("\n*********************************************************************"
	 "***********************************************************************"
	 "***\n");
  for(Int_t i=0;i<nDET;i++){
    TString det = DetectorList[i];
    det.Resize(16);
    Bool_t good = 0;
    printf("%s%s |", ANSI_COLOR_RESET,det.Data());
    for(Int_t j=0;j<nMOD;j++){
      if(TMath::Abs(fit[1][j][i])<TMath::Abs(fitError[1][j][i]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     fit[1][j][i]);
      printf("%s+/-%7.3e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     fitError[1][j][i],ANSI_COLOR_RESET);
    }
    printf("\n");
  }
  printf("\n\n");


  return 0;
}











