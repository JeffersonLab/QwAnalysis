#include <TChain.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TMath.h"
#include "TString.h"
#include "TCanvas.h"
#include "TChain.h"
#include "TFile.h"
#include "TCut.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TTree.h"
#include "TGraphErrors.h"
#include "TH1D.h"
#include "plotSlopes.C"

const Int_t nMON = 5, nDET = 32;


void FindAverageAndError(Double_t *x, Double_t *avg, Int_t n){
  Double_t mean = 0, err = 0;
  for(int i=0;i<n;i++){
    mean += x[i] / (Double_t)n;
  }
  avg[0] = mean;
  for(int i=0;i<n;i++){
    err += (x[i] - mean)*(x[i] - mean) / (Double_t)n;
  }
  avg[1] = TMath::Sqrt(err /(Double_t)n);
}


Int_t GetMonitorAndDetectorLists(TString *monitorList, TString *detectorList, 
				 Bool_t trunc, char* config){
  string line;
  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_13993*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  ch->AddFriend("mps_slug",Form("%s/mps_only_friend_13993.root",
				gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  ifstream file(config);
  if(!(file.is_open()&&file.good())){
    cout<<"Configuration file not found. Exiting.\n";
    cout<<config<<endl;
    return -1;
  }
  for(int i=0;i<5;i++){
    char id[4] = "   ", monitr[20] = "                   ";
    file>>id>>monitr;
    getline(file, line);
    monitorList[i] = TString(monitr);
    if(!ch->GetBranch(monitorList[i].Data())){
      cout<<monitorList[i].Data()<<" missing. Exiting.\n";
      return -1;
    }else{
      if( trunc && monitorList[i].Contains("qwk_"))
	monitorList[i].Replace(0,4,""); 
     cout<<monitorList[i].Data()<<"\n";
    }
  }
  getline(file, line);
  Int_t nDet = 0;
  for(int i=0;i<nDET&&!file.eof();i++){
    char id[4] = "   ", detectr[20] = "                   ";
    file>>id>>detectr;
    getline(file, line);
    detectorList[nDet] = TString(detectr);
    if(!ch->GetBranch(detectorList[nDet].Data())){
      cout<<detectorList[nDet].Data()<<" missing.\n";
    }else{
      if( trunc && detectorList[nDet].Contains("qwk_"))
	detectorList[nDet].Replace(0,4,"");
      cout<<detectorList[nDet].Data()<<endl;
      nDet++;
    }
    file.peek();
  }
  delete ch;
  return nDet;
}


void VectorToArray(vector<Double_t> vec, Double_t *arr){
  for(int i=0;i<(int)vec.size();i++)
    arr[i] = vec.at(i);
  return;
}


////////////////////////////////////////////////////////////////////
//This program plots slopes either by macrocycle or by slug average.
//It utilizes the plotting program "plotSlopes.C".
//xType == 0 plot by macrocycle
//xType == 1 plot by run
//xType == 2 plot by slug
////////////////////////////////////////////////////////////////////

void plotMacrocycleSlopes(Bool_t newMonitors = 0, Int_t start = 0, Int_t end = 0, 
			  Int_t xType = 0, Int_t saveToFile = 0){
  Int_t debug = 0; 

  //Declare variables and data structures
  /////////////////////////////////////////////////////////
  Int_t size = 0, good, nDet, minEntries;

  Double_t run = 0, slug = 0, slope[nDET][nMON], slopeErr[nDET][nMON];
  Double_t *x, ***detSlope, ***detSlopeErr;

  vector<Double_t> vX;
  vector< vector< vector<Double_t> > > vSlope, vSlopeErr;

  TString MonitorList[nMON], MonitorListFull[nMON];
  TString DetectorList[nDET], DetectorListFull[nDET];
  TString sCut = "", name = "macrocycle";

  TCut cut = "";

  TChain *slopes;
  if(debug)std::cout<<"(1)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Get lists of monitors and detectors
  /////////////////////////////////////////////////////////
  char *configFile = Form("%s/config/setup_mpsonly%s.config",
			  gSystem->Getenv("BMOD_SRC"),
			  (newMonitors ? "_new_monitors" : ""));

  GetMonitorAndDetectorLists(MonitorListFull, DetectorListFull, 0, configFile);
  nDet = GetMonitorAndDetectorLists(MonitorList, DetectorList,1, configFile);
  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return;
  }
  if(debug)std::cout<<"(2)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Initialize vectors
  /////////////////////////////////////////////////////////
  vSlope.resize(nDet);
  vSlopeErr.resize(nDet);
  for(int idet=0;idet<nDet;idet++){
    vSlope[idet].resize(nMON);
    vSlopeErr[idet].resize(nMON);
  }
  if(debug)std::cout<<"(3)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Open macrocycle slopes tree
  /////////////////////////////////////////////////////////
  slopes = new TChain("slopes");
  slopes->Add(Form("%s/../MacrocycleSlopesTree_ChiSqMin.set0%s.root",
		   gSystem->Getenv("BMOD_OUT"),
		   (newMonitors ? "_new_monitors" : "")));
  if(debug)std::cout<<"(4)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Decide what global cuts to apply 
  //(Only applies to plots vs run or slug) 
  //(Macrocycle cuts applied below *)
  /////////////////////////////////////////////////////////
  sCut = "good";
  cut += sCut;
  minEntries = 3;
  if(debug)std::cout<<"(5)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Set branch addresses
  /////////////////////////////////////////////////////////
  for(int idet=0; idet<nDet; idet++){
    for(int imon=0; imon<nMON; imon++){
      slopes->SetBranchAddress(Form( "%s_%s", DetectorList[idet].Data(), 
				     MonitorList[imon].Data()), 
				     &slope[idet][imon]);
      slopes->SetBranchAddress(Form( "%s_%s_err", DetectorList[idet].Data(), 
				     MonitorList[imon].Data()), 
				     &slopeErr[idet][imon]);
    }
  }
  slopes->SetBranchAddress("good", &good);
  slopes->SetBranchAddress("run", &run);
  slopes->SetBranchAddress("slug", &slug);
  if(debug)std::cout<<"(6)Working.\n"; 
  /////////////////////////////////////////////////////////


  //If plotting by macrocycle loop through 
  //slopes tree filling vectors with slopes
  /////////////////////////////////////////////////////////
  if(xType == 0){//plot by macrocycle
    for(int i=start;i<slopes->GetEntries()&&i<end;i++){
      slopes->GetEntry(i);
      //* APPLY MACROCYCLE CUTS HERE
      Bool_t isGood = good;
      //      cout<<good<<" ";
      if(isGood){
	size++;
	vX.push_back(i);
	for(int idet=0; idet<nDet; idet++){
	  for(int imon=0; imon<nMON; imon++){
	    vSlope[idet][imon].push_back(slope[idet][imon]);
	    //	    vSlopeErr[idet][imon].push_back(slopeErr[idet][imon]);
	    vSlopeErr[idet][imon].push_back(0);
	  }
	}
      }
    }
    if(debug)std::cout<<"(7)Working.\n"; 
    cout<<size<<" entries passing cuts.\n";
  }
  /////////////////////////////////////////////////////////


  //Else if plotting by run
  /////////////////////////////////////////////////////////
  else if(xType==1){
    run = 0;
    name = "run";
    Int_t num = 0, ent = 0;
    Double_t prev_run = 0, temp_slope[nDET][nMON][20];
    while(run<start &&ent<slopes->GetEntries()){
      slopes->GetEntry(ent); 
      ent++;
    }
    prev_run = run;
    for(int ient = ent;ient<=slopes->GetEntries() && run<=end;ient++){
      if(ient<slopes->GetEntries())
	slopes->GetEntry(ient);
      //* APPLY MACROCYCLE CUTS HERE
      Bool_t isGood = good;
      //      cout<<good<<" ";
      if(run != prev_run || ient==slopes->GetEntries()){
	if(((int)run)%100==0)cout<<"Processing run "<<(int)run<<endl;
	if(num>minEntries){
	  size++;
	  for(int idet=0; idet<nDet; idet++){
	    for(int imon=0; imon<nMON; imon++){
	      Double_t avg[2] = {0,0};
	      FindAverageAndError(temp_slope[idet][imon], avg, num);
	      cout<<"Run "<<prev_run<<" "<<DetectorList[idet].Data()<<"_"
		  <<MonitorList[imon].Data()<<" "<<avg[0]<<" "<<avg[1]<<endl;
	      vSlope[idet][imon].push_back(avg[0]);
	      vSlopeErr[idet][imon].push_back(avg[1]);
	    }
	  }
	  vX.push_back(prev_run);
	}
	num = 0;
      }
      if(isGood){
	for(int idet=0; idet<nDet; idet++){
	  for(int imon=0; imon<nMON; imon++){
	    temp_slope[idet][imon][num] = slope[idet][imon];
	  }
	}
	num++;
      }
      prev_run = run;
    }
    name = "Run";
    if(debug)std::cout<<"(8)Working.\n"; 
    cout<<size<<" entries passing cuts.\n";
  }
  /////////////////////////////////////////////////////////


  //Else plotting by slug 
  /////////////////////////////////////////////////////////
  else{
    name = "slug";
    Int_t num = 0, ent = 0;
    Double_t prev_slug = 0, temp_slope[nDET][nMON][200];
    while(slug<start &&ent<slopes->GetEntries()){
      slopes->GetEntry(ent); 
      ent++;
    }
    prev_slug = slug;
    for(int ient = ent;ient<=slopes->GetEntries() && slug<=end;ient++){
      if(ient<slopes->GetEntries())
	slopes->GetEntry(ient);
      //* APPLY MACROCYCLE CUTS HERE
      Bool_t isGood = good;
      //      cout<<good<<" ";
      if(slug != prev_slug || ient==slopes->GetEntries()){
	if(num>minEntries && prev_slug!=170){
	  size++;
	  for(int idet=0; idet<nDet; idet++){
	    for(int imon=0; imon<nMON; imon++){
	      Double_t avg[2] = {0,0};
	      FindAverageAndError(temp_slope[idet][imon], avg, num);
	      cout<<"Slug "<<prev_slug<<" "<<DetectorList[idet].Data()<<"_"
		  <<MonitorList[imon].Data()<<" "<<avg[0]<<" "<<avg[1]<<" "
		  <<num<<endl;
	      vSlope[idet][imon].push_back(avg[0]);
	      vSlopeErr[idet][imon].push_back(avg[1]);
	    }
	  }
	  vX.push_back(prev_slug);
	}
	num = 0;
      }
      if(isGood){
	for(int idet=0; idet<nDet; idet++){
	  for(int imon=0; imon<nMON; imon++){
	    temp_slope[idet][imon][num] = slope[idet][imon];
	  }
	}
	num++;
      }
      prev_slug = slug;
    }
    name = "Slug";
    if(debug)std::cout<<"(9)Working.\n"; 
    cout<<size<<" entries passing cuts.\n";
  }
  /////////////////////////////////////////////////////////


  //Transfer data from vectors to arrays for use in TGraph
  /////////////////////////////////////////////////////////
  detSlope = new Double_t**[nDet];
  detSlopeErr = new Double_t**[nDet];
  for(int idet = 0; idet<nDet; idet++){
    detSlope[idet] = new Double_t*[nMON];
    detSlopeErr[idet] = new Double_t*[nMON];
    for(int imon = 0; imon<nMON; imon++){
      detSlope[idet][imon] = new Double_t[size];
      detSlopeErr[idet][imon] = new Double_t[size];
    }
  }
  for(int idet = 0; idet<nDet; idet++){
    for(int imon = 0; imon<nMON; imon++){
      VectorToArray(vSlope[idet][imon], detSlope[idet][imon]);
      VectorToArray(vSlopeErr[idet][imon], detSlopeErr[idet][imon]);
    }
  }
  x = new Double_t[size]; 
  VectorToArray(vX, x);
  if(debug)std::cout<<"(10)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Plot results
  /////////////////////////////////////////////////////////
  plotSlopes(MonitorList, DetectorList, detSlope, detSlopeErr, 
	     x, size, nDet, (char*)name.Data(), saveToFile, newMonitors); 
  if(debug)std::cout<<"(11)Working.\n"; 
  /////////////////////////////////////////////////////////


  return;
}
