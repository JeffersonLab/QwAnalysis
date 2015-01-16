#include <TChain.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TMath.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TAxis.h"

 
Int_t plotSlopesVsUnitConversion(int run=13993){
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
  const Int_t nDET = 9, nMOD = 5, nCONV = 8;

  char  mon[255], monitr[255], det[255], detectr[255], x[255], xe[255];
  char e1[255], e2[255], e3[255];
  string line;
  TString MonitorList[nMOD];
  TString DetectorList[nDET];

  ifstream file(Form("%s/config/setup_mpsonly.config",
		     gSystem->Getenv("BMOD_SRC")));
  for(int i=0;i<5;i++){
    file>>mon>>monitr;
    getline(file, line);
    MonitorList[i] = TString(monitr);
  }
  getline(file, line);
  for(int i=0;i<nDET;i++){
    file>>det>>detectr;
    getline(file, line);
    DetectorList[i] = TString(detectr);
    cout<<DetectorList[i].Data()<<endl;
  }

  Double_t eigVals[nMOD][nCONV], slopes[nDET][nMOD][nCONV], 
    slopesErr[nDET][nMOD][nCONV], unitConv[nCONV];

  TString detector;

  ifstream diagnosticsFile(Form("/home/dcj4v/diag.txt"));
  for(int iConv=0;iConv<nCONV;iConv++){
    if(diagnosticsFile.is_open()&&diagnosticsFile.good()){
      getline(diagnosticsFile,line);
      while(line != "RMatrix Eigenvalues and Eigenvectors:")
	getline(diagnosticsFile,line);
      diagnosticsFile>>e1>>e2>>e3;
      unitConv[iConv] = atof(e3);
      getline(diagnosticsFile,line);
      for(int iMod=0;iMod<nMOD;iMod++){
	diagnosticsFile>>x;
	eigVals[iMod][iConv] = atof(x);
	cout<<atof(x)<<", ";
      }
    }else return -1;
    cout<<"\n";
    ifstream slopesFile(Form("%s/slopes/slopes_%i.set4_UC%i.dat",
			     gSystem->Getenv("BMOD_OUT"), run, atoi(e3)));

    if (slopesFile.is_open()&&slopesFile.good()){
      cout<<Form("%s/slopes/slopes_%i.set4_UC%i.dat",
			     gSystem->Getenv("BMOD_OUT"), run, atoi(e3))
	     <<" found.\n";
    }
    else{
      cout<<Form("%s/slopes/slopes_%i.set4_UC%i.dat",
		  gSystem->Getenv("BMOD_OUT"), run, atoi(e3))<<
	     "not found. Exiting.\n";
      return -2;
    }


    for(int idet=0;idet<nDET;idet++){
      if(slopesFile.is_open()&&slopesFile.is_open()){
	getline(slopesFile, line);
	detector = TString("det ");
	detector += DetectorList[idet].Data();
	if(line==detector.Data()){
	  cout<<"\n"<<line<<"  ";
	  for(int imon=0;imon<5;imon++){
	    slopesFile>>x>>xe;
	    Bool_t notANum = strcmp(xe,"nan")==0 || strcmp( x,"nan")==0;
	    slopes[idet][imon][iConv] = (notANum ? -99999.0 : atof(x));
	    slopesErr[idet][imon][iConv] = (notANum ? -99999.0 : atof(xe));
	    cout<<slopes[idet][imon][iConv]<<"+/-"
		<<slopesErr[idet][imon][iConv]<<"    ";
	    getline(slopesFile, line);
	  }
	}
      }
    }
    cout<<"\n";
  }

  diagnosticsFile.close();

  TCanvas *cEval = new TCanvas("cEval","cEval",0,0,1200,800);
  TGraph *gr[nMOD];
  cEval->Divide(3,2);
  for(int imod=0;imod<nMOD;imod++){
    cEval->cd(imod+1);
    gr[imod] = new TGraph(nCONV,unitConv,eigVals[imod]);
    gr[imod]->Draw("ap");
    gr[imod]->SetTitle(Form("Mod %i Eigenvalue vs XP and YP Unit"
			    " Conversion Factor", imod));
    gr[imod]->GetYaxis()->SetTitle(Form("Mod %i Eigenvalue", imod));
    gr[imod]->GetXaxis()->SetTitle(Form("XP and YP Conversion Factor"));
    gr[imod]->SetMarkerColor(kBlue);
    gr[imod]->SetMarkerStyle(20);
    gPad->SetLogx();
    if(imod==1|imod==4){
      gPad->SetLogy();
    }
    gr[imod]->Draw("ap");
  }
  int order[nDET] = {5,4,1,2,3,6,9,8,7};
  TCanvas *cSlopes[nMOD];
  TGraph *grSl[nMOD][nDET];
  for(int imod=0;imod<nMOD;imod++){
    cSlopes[imod] = new TCanvas(Form("cSlopes%i",imod),
				Form("cSlopes%i",imod),0,0,1500,950);
    cSlopes[imod]->Divide(3,3);
    for(int idet=0;idet<nDET;idet++){
      cSlopes[imod]->cd(order[idet]);
      grSl[imod][idet] = new TGraph(nCONV,unitConv,slopes[idet][imod]);
      grSl[imod][idet]->Draw("ap");
      grSl[imod][idet]->SetTitle(Form("d%sd%s vs Unit Conversion Factor",
				      DetectorList[idet].Data(),
				      MonitorList[imod].Data()));
      grSl[imod][idet]->GetYaxis()->SetTitle(Form("d%sd%s", 
						  DetectorList[idet].Data(),
						  MonitorList[imod].Data()));
      grSl[imod][idet]->GetXaxis()->SetTitle(Form("XP and YP Conversion Factor"));
      grSl[imod][idet]->SetMarkerColor(kBlue);
      grSl[imod][idet]->SetMarkerStyle(20);
      gPad->SetLogx();
      grSl[imod][idet]->Draw("ap");
    }
  }
  TCanvas *cSlopesErr[nMOD];
  TGraph *grSlErr[nMOD][nDET];
  Double_t error[nCONV] = {0,0,0,0,0,0,0,0};
  for(int imod=0;imod<nMOD;imod++){
    cSlopesErr[imod] = new TCanvas(Form("cSlopesErr%i",imod),
				Form("cSlopesErr%i",imod),0,0,1500,950);
    cSlopesErr[imod]->Divide(3,3);
    for(int idet=0;idet<nDET;idet++){
      cSlopesErr[imod]->cd(order[idet]);
      grSlErr[imod][idet] = new TGraph(nCONV,unitConv,slopesErr[idet][imod]);
      grSlErr[imod][idet]->Draw("ap");
      grSlErr[imod][idet]->SetTitle(Form("d%sd%s Error vs Unit Conversion Factor",
				      DetectorList[idet].Data(),
				      MonitorList[imod].Data()));
      grSlErr[imod][idet]->GetYaxis()->SetTitle(Form("d%sd%s Error", 
						  DetectorList[idet].Data(),
						  MonitorList[imod].Data()));
      grSlErr[imod][idet]->GetXaxis()->SetTitle(Form("XP and YP Conversion Factor"));
      grSlErr[imod][idet]->SetMarkerColor(kBlue);
      grSlErr[imod][idet]->SetMarkerStyle(20);
      gPad->SetLogx();
      gPad->SetLogy();
      grSlErr[imod][idet]->Draw("ap");
    }
  }
  
  return 0;
}
