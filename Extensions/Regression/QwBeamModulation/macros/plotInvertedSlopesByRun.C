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

Int_t plotInvertedSlopesByRun(Int_t run_start = 13843, Int_t run_end = 19000){
  //  gStyle->SetOptFit(1111);
  char  x[255], xe[255], mon[255], monitr[255], det[255], detectr[255] ,sl[255];
  string line;

  Bool_t ppm_per_micro = 1;
  gStyle->SetOptFit(1111);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.25);
  gStyle->SetOptStat(0);

  const Int_t nDET = 9, nMOD = 5, nRUNS = 5000;
  Int_t order[18] = {5,4,1,2,3,6,9,8,7};
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


  ifstream file(Form("%s/config/setup_mpsonly.config",gSystem->Getenv("BMOD_SRC")));
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
  ifstream runList(Form("%s/macros/runsBySlug.dat", 
			gSystem->Getenv("BMOD_SRC")));


  if(runList.is_open())cout<<"File list found.\n";
  else cout<<"File list not found.\n";

  Int_t n = 0, run = 0, nExcluded = 0, nEmpty = 0, nNotFound = 0;
  //  for(int i=0;i<nMOD;i++)n[i] = 0;
  Double_t runs[nMOD][nRUNS], runsErr[nMOD][nRUNS];
  Double_t slopes[nMOD][nDET][nRUNS], slopesErr[nMOD][nDET][nRUNS];
  while(!runList.eof()&&runList.good()&&run<=run_end){
    runList>>sl>>x;
    run = atoi(x);
    if(run>=run_end)break;
    if(run>=run_start){
      ifstream slopesFile(Form("%s/slopes/slopes_%i.set4.dat",
			       gSystem->Getenv("BMOD_OUT"),run));
      cout<<Form("%s/slopes/slopes_%i.set4.dat",
		 gSystem->Getenv("BMOD_OUT"),run)<<endl;  
      Int_t good = 0;    
      if(slopesFile.is_open()&&slopesFile.good()){
	good = 1;
	slopesFile.peek();
	if(slopesFile.eof())nEmpty++;
      }else nNotFound++;
      if(!slopesFile.eof()&&good){
	cout<<"Slopes file found for run "<<run<<endl;
	Int_t allGood = 0;
	for(int j=0;j<nDET;j++){
	  getline(slopesFile,line);
	  for(int i=0;i<nMOD;i++){
	    slopesFile>>x>>xe;
	    getline(slopesFile,line);
	    runs[i][n] = run;
	    runsErr[i][n] = 0;
	    if(atof(xe)>5e-10&&run!=14424&&run!=16324&&run!=16331&&run!=15973
	       &&run!=16299&&run!=16304&&run!=16554&&run!=16281&&run!=16283){
	      allGood++;
	      slopes[i][j][n] = atof(x) * unitConv[i];
	      slopesErr[i][j][n] = atof(xe) * unitConv[i];
	      cout<<slopes[i][j][n]<<"+/-"<<slopesErr[i][j][n]<<"  "<<n<<endl;
	    }else allGood = 0;
	  }
	}
	if(allGood==nDET*nMOD) n++;
	else nExcluded++;
      }else cout<<"Slopes file NOT found for run "<<run<<endl;
      slopesFile.close();
    }
  }
  cout<<n<<" runs found.\n";
  //  for(int i =0;i<nMOD;i++)cout<<n[i]<<endl;

  for(int i=0;i<nMOD;i++)
    MonitorList[i].Replace(0,4,"");
  for(int i=0;i<nDET;i++)
    DetectorList[i].Replace(0,4,"");

  TLine *tl1; 
  TLine *tl2 ;
  TCanvas *cInvertedSlope[nMOD]; 
  TGraphErrors *gr[nMOD][nDET];

  for(int j=0; j<nMOD;j++){
    cInvertedSlope[j] = new TCanvas(Form("cInvertedSlope%i",j),
			  Form("cInvertedSlope%i",j), 0,0,1600,1000);
    cInvertedSlope[j]->Divide(3,3);
    for(int i=0;i<nDET;i++){
      cInvertedSlope[j]->cd(order[i]);
      gr[j][i] = new TGraphErrors(n,runs[j],slopes[j][i],
				   runsErr[j], slopesErr[j][i]);
      gr[j][i]->SetTitle(Form("(Fully Inverted Slopes) %s to %s", 
			      DetectorList[i].Data(), MonitorList[j].Data()));
      gr[j][i]->SetLineColor(kViolet);
      gr[j][i]->SetMarkerColor(kViolet);
      gr[j][i]->SetMarkerStyle(7);
      gr[j][i]->Draw("ap");
      gr[j][i]->GetXaxis()->SetTitle("Run");
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
  cout<<nExcluded<<" excluded runs due to incomplete slopes list.\n";
  cout<<nEmpty<<" runs with empty slopes files.\n";
  cout<<nNotFound<<" runs not found.\n";
  runList.close();
  return 0;
}











