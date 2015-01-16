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
#include "plotSlopesMultiGraph.C"
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


Int_t GetMonitorAndDetectorLists(TString *monitorList, TString *detectorList, Bool_t trunc){
  string line;
  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_13993*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES")));

  ifstream file(Form("%s/config/setup_mpsonly.config",
		     gSystem->Getenv("BMOD_SRC")));

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
//This program plots difference in average slopes as found by run 
//averages and macrocycle averages. Both are averaged over the same
//run range, one using inverted slopes at run level and the other
//using inverted slopes at macrocycle level.
//It utilizes the plotting program "plotSlopes.C".
////////////////////////////////////////////////////////////////////

void plotSlopeDifferences(Int_t start = 13842, Int_t end = 19000, 
			  Int_t saveToFile = 0)
{

  Bool_t debug = 1, plotDifferencesOnly = 0; 


  //Get lists of monitors and detectors.
  /////////////////////////////////////////////////////////
  TString MonitorList[nMON], DetectorList[nDET];
  Int_t nDet = GetMonitorAndDetectorLists(MonitorList, DetectorList,1);
  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return;
  }
  if(debug)std::cout<<"(1)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Get two trees of slopes from files.
  /////////////////////////////////////////////////////////
  TFile fSl(Form("%s/../slopesTree_ChiSqMin.set0.root",
		 gSystem->Getenv("BMOD_OUT")));
  TTree *slopes = (TTree*)fSl.Get("slopes");   

  TFile fMacSl(Form("%s/../MacrocycleSlopesTree_ChiSqMin.set0.root",
		 gSystem->Getenv("BMOD_OUT")));
  TTree *mSlopes = (TTree*)fMacSl.Get("slopes"); 
  if(debug)std::cout<<"(2)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Find run ranges from file.  
  /////////////////////////////////////////////////////////
  gROOT->ProcessLine(Form(".!ls %s/slopelists2/slopeList_asym_qwk_bcm1*"
			  "> slopeList.txt", gSystem->Getenv("BMOD_OUT")));
  ifstream fSlList("slopeList.txt");
  if(!(fSlList.is_open() && fSlList.good())){
    cout<<"Error opening slopeList.txt. Exiting.\n";
    return;
  }else cout<<"slopeList.txt found.\n";
  Int_t size = 0;
  std::string line;
  vector<Int_t>vFirst, vLast;  
  getline(fSlList, line);
  while(!fSlList.eof()){
    ++size;
    std::size_t pos = line.find("bcm1.");
    std::string f_str = line.substr(pos+5, 5);
    Int_t first = atoi(f_str.data());
    std::string l_str = line.substr(pos+11, 5);
    Int_t last = atoi(l_str.data());
    if(first>=start&& first<=end){
      vFirst.push_back(first);
      vLast.push_back((last<=end? last : end));
      if(debug)cout<<"First: "<<vFirst.back()<<"  Last: "<<vLast.back()<<endl;
    }
    getline(fSlList, line);
  }
  if(size-vLast.size()!=0)
    cout<<size-vLast.size()<<"!=0. YIIIIIIIIIKKKKKKKEEEESSSS!!!!\n";
  cout<<size<<" run ranges found.\n";
  fSlList.close(); 
  if(debug)std::cout<<"(3)Working.\n";
  /////////////////////////////////////////////////////////


  //Set up branch addresses.
  /////////////////////////////////////////////////////////
  Int_t good, mGood;
  Double_t run, mRun;
  Double_t slope[nDET][nMON], mSlope[nDET][nMON];
  slopes->SetBranchAddress("good",&good);
  slopes->SetBranchAddress("run",&run);
  mSlopes->SetBranchAddress("good",&mGood);
  mSlopes->SetBranchAddress("run",&mRun);
  for(int idet=0;idet<nDet;++idet){
    for(int imon=0;imon<nMON;++imon){
      slopes->SetBranchAddress(Form("%s_%s",DetectorList[idet].Data(),
				    MonitorList[imon].Data()), 
			       &slope[idet][imon]);
      mSlopes->SetBranchAddress(Form("%s_%s",DetectorList[idet].Data(),
				    MonitorList[imon].Data()), 
			       &mSlope[idet][imon]);
    }
  }
  if(debug)std::cout<<"(4)Working.\n"; 
  /////////////////////////////////////////////////////////





  //Get average slopes from trees and find differences.  
  /////////////////////////////////////////////////////////
  Int_t ent = 0, mEnt = 0;
  vector<Double_t> vRun, vRunErr;
  vector< vector< vector <Double_t> > >vSlope, vSlopeErr, vMSlope, vMSlopeErr;
  vector< vector< vector <Double_t> > >vDiffSlope, vDiffSlopeErr;
  vSlope.resize(nDet);
  vSlopeErr.resize(nDet);
  vMSlope.resize(nDet);
  vMSlopeErr.resize(nDet);
  vDiffSlope.resize(nDet);
  vDiffSlopeErr.resize(nDet);
  for(int idet=0;idet<nDet;++idet){
    vSlope[idet].resize(nMON);
    vSlopeErr[idet].resize(nMON);
    vMSlope[idet].resize(nMON);
    vMSlopeErr[idet].resize(nMON);
    vDiffSlope[idet].resize(nMON);
    vDiffSlopeErr[idet].resize(nMON);
  }

  slopes->GetEntry(0);
  mSlopes->GetEntry(0);
  for(int i=0;i<size;++i){

    //run range
    vRun.push_back(0.5*(vFirst[i]+vLast[i]));
    vRunErr.push_back(0.5*(-vFirst[i]+vLast[i]));

    //slopes tree
    vector< vector< vector <Double_t> > >v_slope;
    v_slope.resize(nDet);
    for(int idet=0;idet<nDet;++idet){
      v_slope[idet].resize(nMON);
    }
    while(run <= vLast[i] && ent<slopes->GetEntries()){
      //      if(run>=vFirst[i] && good){
      if(run>=vFirst[i]){
	for(int idet=0;idet<nDet;++idet){
	  for(int imon=0;imon<nMON;++imon){
	    Double_t slp = (slope[idet][imon]==-99999 ? 0 : slope[idet][imon]);
	    v_slope[idet][imon].push_back(slp);
	  }
	}
      }
      ++ent;
      slopes->GetEntry(ent);
    }

    //macrocycle slopes tree
    vector< vector< vector <Double_t> > >v_m_slope;
    v_m_slope.resize(nDet);
    for(int idet=0;idet<nDet;++idet){
      v_m_slope[idet].resize(nMON);
    }
    while(mRun <= vLast[i] && mEnt<mSlopes->GetEntries()){
      //      if(i==115)cout<<"run: "<<run<<"  mRun: "<<mRun<<"  vlast: "<<vLast[i]<<"\n";
      //      if(mRun>= vFirst[i] && mGood){
      if(mRun>= vFirst[i]){
	for(int idet=0;idet<nDet;++idet){
	  for(int imon=0;imon<nMON;++imon){
	    Double_t slp = (mSlope[idet][imon]==-99999 ? 0 : mSlope[idet][imon]);
	    v_m_slope[idet][imon].push_back(slp);
	  }
	}
      }
      ++mEnt;
      mSlopes->GetEntry(mEnt);
    }

    //difference
    for(int idet=0;idet<nDet;++idet){
      for(int imon=0;imon<nMON;++imon){
	Double_t temp[2] = {0.0,0.0}, m_temp[2] = {0.0,0.0};
	Double_t *temp_slope = v_slope[idet][imon].data();
	Double_t *temp_m_slope = v_m_slope[idet][imon].data();
	FindAverageAndError(temp_slope, temp, v_slope[idet][imon].size());
	FindAverageAndError(temp_m_slope, m_temp, v_m_slope[idet][imon].size());
	vSlope[idet][imon].push_back(temp[0]);
	vSlopeErr[idet][imon].push_back(temp[1]);
	vMSlope[idet][imon].push_back(m_temp[0]);
	vMSlopeErr[idet][imon].push_back(m_temp[1]);
	vDiffSlope[idet][imon].push_back(temp[0] - m_temp[0]);
	vDiffSlopeErr[idet][imon].push_back(TMath::Sqrt(temp[1]*temp[1] + 
						    m_temp[1]*m_temp[1]));
	if(debug&&idet==imon&&imon==0)
	  cout<<DetectorList[idet].Data()<<"_"<<MonitorList[imon].Data()<<"("<<
	    i<<"): "<<temp[0]<<"-"<<m_temp[0]<<"="<<vSlope[idet][imon].back()<<
	    "+/-"<<vSlopeErr[idet][imon].back()<<"    sizes:"<<
	    v_slope[idet][imon].size()<<" "<<v_m_slope[idet][imon].size()<<
	    " "<<vFirst[i]<<"  "<<vLast[i]<<" entry:"<<ent<<"  "<<mEnt<<endl;
      }
    }
  }
  if(debug)std::cout<<"(5)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Plot results
  /////////////////////////////////////////////////////////
  if(plotDifferencesOnly){
    Double_t ***detSlope, ***detSlopeErr, *x, *xErr;
    x = new Double_t [size];
    xErr = new Double_t [size];
    VectorToArray(vRun, x);
    VectorToArray(vRunErr, xErr);
    detSlope = new Double_t**[nDet];
    detSlopeErr = new Double_t**[nDet];
    for(int idet=0;idet<nDet;++idet){
      detSlope[idet] = new Double_t *[nMON];
      detSlopeErr[idet] = new Double_t *[nMON];
      for(int imon=0;imon<nMON;++imon){
	detSlope[idet][imon] = new Double_t [size];
	detSlopeErr[idet][imon] = new Double_t [size];
	VectorToArray(vDiffSlope[idet][imon], detSlope[idet][imon]);
	VectorToArray(vDiffSlopeErr[idet][imon], detSlopeErr[idet][imon]);
      }
    }
    plotSlopes(MonitorList, DetectorList, detSlope, detSlopeErr, 
	       x, xErr, size, nDet, "Run", saveToFile); 
  }else{
    TString legend[3] = {"Run level","Cycle level", "Difference"};
    plotSlopesMultiGraph(MonitorList, DetectorList, legend, vRun, vRunErr,
			 vSlope, vSlopeErr, vMSlope, vMSlopeErr,
			 vDiffSlope, vDiffSlopeErr, "Run", nDet, saveToFile);
  }
  if(debug)std::cout<<"(6)Working.\n"; 
  /////////////////////////////////////////////////////////

  return;
}
