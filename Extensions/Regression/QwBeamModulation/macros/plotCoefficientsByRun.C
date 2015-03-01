#include "TString.h"
#include "TProfile.h"
#include "TSystem.h"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <cstdio>
#include "TH1D.h"
#include "TH2D.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TChain.h"
#include "TFile.h"
#include "TMath.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TDirectory.h"
#include "TMultiGraph.h"
#include "TGraphErrors.h"

void getMonitorAndDetectorList(TString *mon_list, TString *det_list, 
			       Int_t n_mon, Int_t n_det){
  char pref[255], name[255];
  string line;
  ifstream file(Form("%s/config/setup_mpsonly.config",
		     gSystem->Getenv("BMOD_SRC")));

  for(int i=0;i<n_mon;i++){
    file>>pref>>name;
    getline(file, line);
    mon_list[i] = TString(name);
    if(mon_list[i].Contains("qwk_"))
       mon_list[i].Replace(0, 4,"");
    cout<<mon_list[i]<<endl;
  }
  getline(file, line);
  for(int i=0;i<n_det;i++){
    file>>pref>>name;
    getline(file, line);
    det_list[i] = TString(name);
    if(det_list[i].Contains("qwk_"))
       det_list[i].Replace(0, 4,"");
    cout<<det_list[i]<<endl;
  }
  file.close();
}

Int_t runIsInBadList(Int_t run, vector<Int_t> run_list){
  Int_t isInBadList = 0;
  for(int i=0;i<(int)run_list.size();i++){
    if(run == run_list.at(i)){
      isInBadList = 1;
      break;
    }
  }
  return isInBadList;
}

const Int_t nMON = 5, nDET = 29, nCOIL = 10, nRUNS = 1500; 

Int_t plotCoefficientsByRun(Int_t run_start = 13843, Int_t run_end = 19000, 
			    Int_t type = 0){
  //type = 0 plot Monitor coeffs
  //type = 1 plot Detector coeffs
  //type = 2 plot Lumi coeffs
  //type = 3 plot Background detector coeffs
  //type = 4 plot BCM coeffs

  TString MonitorList[nMON];
  TString DetectorList[nDET];
  Double_t run, runs[nRUNS], runsErr[nRUNS];
  Double_t slopes[nDET][nCOIL][nRUNS], slopesErr[nDET][nCOIL][nRUNS];
  Double_t monitor[nMON][nCOIL],monitorErr[nMON][nCOIL];
  Double_t detector[nDET][nCOIL], detectorErr[nDET][nCOIL];
  vector<int> badRuns;
  getMonitorAndDetectorList(MonitorList, DetectorList, nMON, nDET);
  ifstream fileEx(Form("%s/macros/excludedRuns.dat", 
		       gSystem->Getenv("BMOD_SRC")));
  if(fileEx.is_open() && fileEx.good()){
    while(!fileEx.eof() && fileEx.good()){
      char sl[255], rn[255];
      string line;
      fileEx>>sl>>rn;
      if(fileEx.eof())break;
      badRuns.push_back(atoi(rn));
      getline(fileEx, line);
    }
  }
  fileEx.close();
  TFile *file = TFile::Open(Form("%s/../slopesTree.root", 
				 gSystem->Getenv("BMOD_OUT"))); 
  TTree *tr = (TTree*)file->Get("slopes");
  cout<<tr->GetEntries()<<" entries in slopes tree.\n";
  Int_t det_plot_order[10] = {5,4,1,2,3,6,9,8,7};

  tr->SetBranchAddress("run", &run);

  TCanvas *cCoil[nCOIL];
  TGraphErrors *gCoil[nDET][nCOIL];

  if(type == 0){
    for(int i=0; i<nMON; i++){
      for(int j=0;j<nCOIL;j++){
	tr->SetBranchAddress(Form("%s_Coil%i", MonitorList[i].Data(), j),
			     &monitor[i][j]);
	tr->SetBranchAddress(Form("%s_Coil%i_err", MonitorList[i].Data(), j),
			     &monitorErr[i][j]);
      }
    }
    Int_t n = 0;
    for(int k=0;k<tr->GetEntries();k++){
      tr->GetEntry(k);
      if(run>=run_start && run<=run_end){
	for(int i = 0; i<nMON; i++){
	  for(int j=0;j<nCOIL;j++){
	    slopes[i][j][n] = monitor[i][j];
	    slopesErr[i][j][n] = monitorErr[i][j];
	    //cout<<slopes[i][j][n]<<"+/-"<<slopesErr[i][j][n]<<endl;
	  }
	}
	runs[n] = run;
	runsErr[n] = 0;
	if(!runIsInBadList((int)run, badRuns)) n++;
      }
    }

    cout<<"Used "<<n<<" runs.\n";
    for(int i=0;i<nCOIL;i++){
      cCoil[i] = new TCanvas(Form("cCoil%i",i), Form("cCoil%i",i), 
			     0, 0, 1200, 1000);
      cCoil[i]->Divide(3,2);
      for(int j=0;j<nMON;j++){
	cCoil[i]->cd(j+1);
	gCoil[j][i] =  new TGraphErrors(n, runs, slopes[j][i], 
					runsErr, slopesErr[j][i]);
	gCoil[j][i]->Draw("ap");
	gCoil[j][i]->SetMarkerColor((i<5 ? kRed:kBlue));
	gCoil[j][i]->SetLineColor((i<5 ? kRed:kBlue));
	gCoil[j][i]->SetTitle(Form("%s Response to Coil%i",
				     MonitorList[j].Data(), i));
	gCoil[j][i]->GetXaxis()->SetTitle("Run");
	TString units = (MonitorList[j].Contains("Slope") ? 
			 "#murad":"#mum");
	gCoil[j][i]->GetYaxis()->SetTitle(Form("%s_Coil%i (%s)", 
					       MonitorList[j].Data(), i ,
					       units.Data()));
	gCoil[j][i]->Draw("ap");
	gPad->Update();
      }
    }
  }
  return 0;
}
