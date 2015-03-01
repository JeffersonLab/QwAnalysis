#include "TString.h"
#include "TProfile.h"
#include "TSystem.h"
#include <iostream>
#include <fstream>
#include <cstdio>
#include "TH1D.h"
#include "TH2D.h"
#include "TCut.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TChain.h"
#include "TMath.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TDirectory.h"
#include "TPaveText.h"
#include "TPad.h"
#include "TFile.h"
#include "TTree.h"
#include "TLeaf.h"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#include "DetectorPlots.C"


//Declare constants
/////////////////////////////////////////////////////////
const Int_t nDET = 32, nMON = 5, nMOD = 5, nCOIL = 10, nSLUG = 322, nTYPE = 3;
const Int_t MIN_ENTRIES_PER_RUN = 5, MIN_ENTRIES_PER_SLUG = 5;
const Double_t  errorFlag = -999999, DegToRad = 0.0174532925199432955;
/////////////////////////////////////////////////////////
Int_t GetMonitorAndDetectorLists(TChain *ch, vector<TString> &monitorList, 
				 vector<TString> &detectorList, Bool_t trunc,
				 char *config){
  string str;
  ifstream file(config);

  for(int imon=0;imon<5;imon++){
    char id[4] = "   ", monitr[20] = "                   ";
    file>>id>>monitr;
    getline(file, str);
    monitorList.push_back(monitr);
    if(!ch->GetBranch((monitorList.back()).Data())){
      cout<<(monitorList.back()).Data()<<" missing. Exiting.\n";
      return -1;
    }else{
      if( trunc && (monitorList.back()).Contains("qwk_"))
	monitorList.back().Replace(0,4,"");
      cout<<monitorList.back().Data()<<"\n";
    }
  }
  getline(file, str);
  Int_t nDet = 0;
  for(int idet=0;idet<nDET&&!file.eof();idet++){
    char id[4] = "   ", detectr[20] = "                   ";
    file>>id>>detectr;
    getline(file, str);
    if(!ch->GetBranch(detectr)){
      cout<<detectr<<" missing.\n";
    }else{
      detectorList.push_back(detectr);
      if( trunc && (detectorList.back()).Contains("qwk_"))
	detectorList.back().Replace(0,4,"");
      cout<<detectorList.back().Data()<<endl;
      nDet++;
    }
    file.peek();
  }
  file.close();
  return nDet;
}


///////////////////////////////////////////////////////
//Don Jones 
//Oct 2014
//Program for plotting results of slopes, coefficients and residuals and 
//differences between two analyses all on the same plot. 
//All plots averaged at slug level.
//Program assumes existence of both slopes trees and that they can be "friended".
//////////////////////////////////////////////////////////////////////////

int plotDiffCoilSelection(TString stem1 = "", TString stem2 = "", 
			  int first = 42, int last = 321)
{
  gStyle->SetMarkerStyle(7);
  Bool_t debug = 1, saveToFile = 1, useMacrocycleSlopesTree = 1; 
  vector<TString>DetectorList, MonitorList, DetectorListFull, MonitorListFull;
  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_13993*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  ch->AddFriend("mps_slug", Form("%s/mps_only_friend_13993.root",
				 gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  TString configFile = Form("%s/config/setup_mpsonly%s.config", 
			    gSystem->Getenv("BMOD_SRC"), stem1.Data());
  Int_t nDet = GetMonitorAndDetectorLists(ch, MonitorList, DetectorList, 1,
					  (char*)configFile.Data());
  GetMonitorAndDetectorLists(ch, MonitorListFull, DetectorListFull, 0,
			      (char*)configFile.Data());
  delete ch;


  //Get tree of slopes from file.
  /////////////////////////////////////////////////////////
  TString treeFileName = Form("%s/../%slopesTree_ChiSqMin.set0%s.root",
			      gSystem->Getenv("BMOD_OUT"), 
			      (useMacrocycleSlopesTree ? "MacrocycleS" : "s"),
			      stem1.Data());
  TFile fSl1(treeFileName.Data());
  treeFileName = Form("%s/../%slopesTree_ChiSqMin.set0%s.root",
		      gSystem->Getenv("BMOD_OUT"), 
		      (useMacrocycleSlopesTree ? "MacrocycleS" : "s"),
		      stem2.Data());  
  TFile fSl2(treeFileName.Data());
  TTree *slopes_tree[2];
  slopes_tree[0] = (TTree*)fSl1.Get("slopes");   
  slopes_tree[1] = (TTree*)fSl2.Get("slopes");   
  cout<<"Entries: "<<slopes_tree[0]->GetEntries()<<" = "<<
    slopes_tree[1]->GetEntries()<<endl;;
  if(debug)std::cout<<"(1)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Set up vectors for storing slopes, coefficients and residuals
  /////////////////////////////////////////////////////////
  double slopes[2][nDET][nMON], coeff[2][nDET][nCOIL], sinRes[2][nDET][nMON], 
    cosRes[2][nDET][nMON];
  //vector format is [det][mon/coil]
  vector<vector<vector<vector<double> > > >vSlopes, vSlopesErr, vSinCoeff, 
    vSinCoeffErr, vSinRes, vSinResErr, vCosCoeff, vCosCoeffErr, vCosRes, 
    vCosResErr;
  vSlopes.resize(nTYPE);
  vSlopesErr.resize(nTYPE);
  vSinCoeff.resize(nTYPE);
  vSinCoeffErr.resize(nTYPE);
  vSinRes.resize(nTYPE);
  vSinResErr.resize(nTYPE);
  vCosCoeff.resize(nTYPE);
  vCosCoeffErr.resize(nTYPE);
  vCosRes.resize(nTYPE);
  vCosResErr.resize(nTYPE);
  for(int itype=0;itype<nTYPE;++itype){
    vSlopes[itype].resize(nDet);
    vSlopesErr[itype].resize(nDet);
    vSinCoeff[itype].resize(nDet);
    vSinCoeffErr[itype].resize(nDet);
    vCosCoeff[itype].resize(nDet);
    vCosCoeffErr[itype].resize(nDet);
    vSinRes[itype].resize(nDet);
    vSinResErr[itype].resize(nDet);
    vCosRes[itype].resize(nDet);
    vCosResErr[itype].resize(nDet);
    for(int idet=0; idet<nDet;++idet){
      vSlopes[itype][idet].resize(nMON);
      vSlopesErr[itype][idet].resize(nMON);
      vSinCoeff[itype][idet].resize(nMON);
      vSinCoeffErr[itype][idet].resize(nMON);
      vSinRes[itype][idet].resize(nMON);
      vSinResErr[itype][idet].resize(nMON);
      vCosCoeff[itype][idet].resize(nMON);
      vCosCoeffErr[itype][idet].resize(nMON);
      vCosRes[itype][idet].resize(nMON);
      vCosResErr[itype][idet].resize(nMON);
      for(int imon=0; imon<nMON;++imon){
	for(int islug=0;islug<nSLUG;++islug){
	  vSlopes[itype][idet][imon].push_back(0);
	  vSlopesErr[itype][idet][imon].push_back(0);
	  vSinCoeff[itype][idet][imon].push_back(0);
	  vSinCoeffErr[itype][idet][imon].push_back(0);
	  vSinRes[itype][idet][imon].push_back(0);
	  vSinResErr[itype][idet][imon].push_back(0);
	  vCosCoeff[itype][idet][imon].push_back(0);
	  vCosCoeffErr[itype][idet][imon].push_back(0);
	  vCosRes[itype][idet][imon].push_back(0);
	  vCosResErr[itype][idet][imon].push_back(0);
	}
      }
    }
  }
  vector<double>vSlug ,vSlugErr, vN;
  for(int islug=0;islug<nSLUG;++islug){
    vN.push_back(0);
    vSlug.push_back(islug);
    vSlugErr.push_back(0);
  }
  /////////////////////////////////////////////////////////

  double slug[2];
  int good[2];
  for(int i=0;i<2;i++){
    slopes_tree[i]->SetBranchStatus("*",0);
    slopes_tree[i]->SetBranchStatus("slug", 1);
    slopes_tree[i]->SetBranchStatus("good", 1);
    for(int idet=0; idet<nDet;++idet){
      for(int imon=0;imon<nMON;++imon){
	TString name = Form("%s_%s", DetectorList[idet].Data(), MonitorList[imon].Data());
	slopes_tree[i]->SetBranchStatus(name.Data(), 1);
	slopes_tree[i]->SetBranchAddress(name.Data(), &slopes[i][idet][imon]);
	name = Form("SineRes_%s_Coil%i",DetectorList[idet].Data(),imon);
	slopes_tree[i]->SetBranchStatus(name.Data(), 1);
	slopes_tree[i]->SetBranchAddress(name.Data(), &sinRes[i][idet][imon]);
	name = Form("CosineRes_%s_Coil%i",DetectorList[idet].Data(),imon);
	slopes_tree[i]->SetBranchStatus(name.Data(), 1);
	slopes_tree[i]->SetBranchAddress(name.Data(), &cosRes[i][idet][imon]);
      }
      for(int icoil=0;icoil<nCOIL;++icoil){
	TString name = Form("%s_Coil%i",DetectorList[idet].Data(),icoil);
	slopes_tree[i]->SetBranchStatus(name.Data(), 1);
	slopes_tree[i]->SetBranchAddress(name.Data(), &coeff[i][idet][icoil]);
      }
    }
  }
  slopes_tree[0]->SetBranchAddress("slug",&slug[0]);
  slopes_tree[1]->SetBranchAddress("slug",&slug[1]);
  slopes_tree[0]->SetBranchAddress("good",&good[0]);
  slopes_tree[1]->SetBranchAddress("good",&good[1]);


  for(int i=0;i<slopes_tree[0]->GetEntries();++i){
    slopes_tree[0]->GetEntry(i);
    slopes_tree[1]->GetEntry(i);
    if(slug[0] < first) continue;
    if(slug[0] > last) break;
    if(i%1000==0)cout<<"Processing entry "<<i<<endl;
    if(!(good[0] && good[1]))continue;
    if(slug[1]!=slug[0]){
      cout<<"Slug numbers not equal for entry "<<i<<endl;
      return -1;
    }
    int slg = (int)slug[0];
    ++vN[slg];
    for(int idet=0;idet<nDet;++idet){
      for(int imon=0;imon<nMON;++imon){
	for(int itree=0;itree<nTYPE-1;++itree){
	  vSlopes[itree][idet][imon][slg] += slopes[itree][idet][imon]; 
	  //	  cout<<slopes[itree][idet][imon]<<endl;
	  vSlopesErr[itree][idet][imon][slg] += pow(slopes[itree][idet][imon], 2); 

	  vSinCoeff[itree][idet][imon][slg] += coeff[itree][idet][imon];
	  vSinCoeffErr[itree][idet][imon][slg] += pow(coeff[itree][idet][imon], 2); 
	  vCosCoeff[itree][idet][imon][slg] += coeff[itree][idet][imon+nMON];
	  vCosCoeffErr[itree][idet][imon][slg] += pow(coeff[itree][idet][imon+nMON], 2); 

	  vSinRes[itree][idet][imon][slg] += sinRes[itree][idet][imon];
	  vSinResErr[itree][idet][imon][slg] += pow(sinRes[itree][idet][imon], 2); 
	  vCosRes[itree][idet][imon][slg] += cosRes[itree][idet][imon];
	  vCosResErr[itree][idet][imon][slg] += pow(cosRes[itree][idet][imon], 2); 
	}
	double val =  slopes[0][idet][imon] - slopes[1][idet][imon];
	vSlopes[2][idet][imon][slg] += val;
	vSlopesErr[2][idet][imon][slg] += pow(val, 2);

	val = coeff[0][idet][imon] - coeff[1][idet][imon];
	vSinCoeff[2][idet][imon][slg] += val;
	vSinCoeffErr[2][idet][imon][slg] += pow(val, 2);
	val = coeff[0][idet][imon+nMON] - coeff[1][idet][imon+nMON];
	vCosCoeff[2][idet][imon][slg] += val;
	vCosCoeffErr[2][idet][imon][slg] += pow(val, 2);

	val = sinRes[0][idet][imon] - sinRes[1][idet][imon];
	vSinRes[2][idet][imon][slg] += val;
	vSinResErr[2][idet][imon][slg] += pow(val, 2);
	val = cosRes[0][idet][imon] - cosRes[1][idet][imon];
	vCosRes[2][idet][imon][slg] += val;
	vCosResErr[2][idet][imon][slg] += pow(val, 2);
      }
    }
  }
  //Finish mean and error calculations
  for(int idet=0;idet<nDet;++idet){
    for(int imon=0;imon<nMON;++imon){
      for(int itype=0;itype<nTYPE;++itype){
	for(int islug=0;islug<nSLUG;++islug){
	  vSlopes[itype][idet][imon][islug] /= vN[islug];
	  vSlopesErr[itype][idet][imon][islug] /= vN[islug];
	  vSlopesErr[itype][idet][imon][islug] = 
	    sqrt( (vSlopesErr[itype][idet][imon][islug] - 
		   pow(vSlopes[itype][idet][imon][islug] ,2) ) / vN[islug]);

	  vSinCoeff[itype][idet][imon][islug] /= vN[islug];
	  vSinCoeffErr[itype][idet][imon][islug] /= vN[islug];
	  vSinCoeffErr[itype][idet][imon][islug] = 
	    sqrt( (vSinCoeffErr[itype][idet][imon][islug] - 
		   pow(vSinCoeff[itype][idet][imon][islug] ,2) ) / vN[islug]);
	  vCosCoeff[itype][idet][imon][islug] /= vN[islug];
	  vCosCoeffErr[itype][idet][imon][islug] /= vN[islug];
	  vCosCoeffErr[itype][idet][imon][islug] = 
	    sqrt( (vCosCoeffErr[itype][idet][imon][islug] - 
		   pow(vCosCoeff[itype][idet][imon][islug] ,2) ) / vN[islug]);

	  vSinRes[itype][idet][imon][islug] /= vN[islug];
	  vSinResErr[itype][idet][imon][islug] /= vN[islug];
	  vSinResErr[itype][idet][imon][islug] = 
	    sqrt( (vSinResErr[itype][idet][imon][islug] - 
		   pow(vSinRes[itype][idet][imon][islug] ,2) ) / vN[islug]);
	  vCosRes[itype][idet][imon][islug] /= vN[islug];
	  vCosResErr[itype][idet][imon][islug] /= vN[islug];
	  vCosResErr[itype][idet][imon][islug] = 
	    sqrt( (vCosResErr[itype][idet][imon][islug] - 
		   pow(vCosRes[itype][idet][imon][islug] ,2) ) / vN[islug]);
	}
      }
    }
  }

  //Shift arrays to remove slugs without sufficient entries
  /////////////////////////////////////////////////////////
  int n=0;
  for(int islug=0;islug<nSLUG;++islug){
    if(vN[islug]<MIN_ENTRIES_PER_RUN)continue;
    vSlug[n] = vSlug[islug];
    vN[n] = vN[islug];
    for(int idet=0;idet<nDet;++idet){
      for(int imon=0;imon<nMON;++imon){
	for(int itype=0;itype<nTYPE;++itype){
	  vSlopes[itype][idet][imon][n] = vSlopes[itype][idet][imon][islug];
	  vSlopesErr[itype][idet][imon][n] = vSlopesErr[itype][idet][imon][islug];
	  //	  cout<<vSlopes[itype][idet][imon][n]<<" +/- "<<
	  //	    vSlopesErr[itype][idet][imon][n]<<endl;
	  vSinCoeff[itype][idet][imon][n] = vSinCoeff[itype][idet][imon][islug];
	  vSinCoeffErr[itype][idet][imon][n] = vSinCoeffErr[itype][idet][imon][islug];	  
	  vCosCoeff[itype][idet][imon][n] = vCosCoeff[itype][idet][imon][islug];
	  vCosCoeffErr[itype][idet][imon][n] = vCosCoeffErr[itype][idet][imon][islug];
	  vSinRes[itype][idet][imon][n] = vSinRes[itype][idet][imon][islug];
	  vSinResErr[itype][idet][imon][n] = vSinResErr[itype][idet][imon][islug];	  
	  vCosRes[itype][idet][imon][n] = vCosRes[itype][idet][imon][islug];
	  vCosResErr[itype][idet][imon][n] = vCosResErr[itype][idet][imon][islug];
	}
      }
    }
    ++n;
  }
  for(int idet=0;idet<nDet;++idet){
    for(int imon=0;imon<nMON;++imon){
      for(int itype=0;itype<nTYPE;++itype){
	vSlopes[itype][idet][imon].resize(n);
	vSlopesErr[itype][idet][imon].resize(n);
	vSinCoeff[itype][idet][imon].resize(n);
	vSinCoeffErr[itype][idet][imon].resize(n);
	vCosCoeff[itype][idet][imon].resize(n);
	vCosCoeffErr[itype][idet][imon].resize(n);
	vSinRes[itype][idet][imon].resize(n);
	vSinResErr[itype][idet][imon].resize(n);
	vCosRes[itype][idet][imon].resize(n);
	vCosResErr[itype][idet][imon].resize(n);
      }
    }
  }
  cout<<n<<" good slugs.\n";
  /////////////////////////////////////////////////////////

  //Plot results
  /////////////////////////////////////////////////////////
  TString stm1 = (char*)(stem1.CompareTo("")==0 ? "10Coil": stem1.Data());
  TString stm2 = (char*)(stem2.CompareTo("")==0 ? "10Coil": stem2.Data());

  //Slopes
  DetectorPlots slopePlots(nDet, 0);
  slopePlots.SetMonitorList(MonitorList.data());
  slopePlots.SetDetectorList(DetectorList.data(), nDet);
  slopePlots.AppendToFileName(Form("_%sand%s",stm1.Data(), stm2.Data()));
  slopePlots.SetTitleX("Slug");
  if(first>=137)
    slopePlots.SetRunPeriod(2);
  else if(last<=136)
    slopePlots.SetRunPeriod(1);
  TString legend[nTYPE] = {stm1,stm2,"Diff"};
  slopePlots.SetDrawLegend(&legend[0], nTYPE); 
  slopePlots.Plot(vSlug, vSlugErr, vSlopes[0], vSlopesErr[0], vSlopes[1], 
		  vSlopesErr[1], vSlopes[2], vSlopesErr[2]);
  if(saveToFile) slopePlots.SaveToFile(stem2.Data());

  //Sine Coefficients
  DetectorPlots sinCoeffPlots(nDet, 1);
  sinCoeffPlots.SetMonitorList(MonitorList.data());
  sinCoeffPlots.SetDetectorList(DetectorList.data(), nDet);
  sinCoeffPlots.AppendToFileName(Form("_%sand%s",stm1.Data(), stm2.Data()));
  sinCoeffPlots.SetTitleX("Slug");
  sinCoeffPlots.SetCoilType(0);
  if(first>=137)
    sinCoeffPlots.SetRunPeriod(2);
  else if(last<=136)
    sinCoeffPlots.SetRunPeriod(1);
  sinCoeffPlots.SetDrawLegend(&legend[0], nTYPE); 
  sinCoeffPlots.Plot(vSlug, vSlugErr, vSinCoeff[0], vSinCoeffErr[0], vSinCoeff[1],
		  vSinCoeffErr[1], vSinCoeff[2], vSinCoeffErr[2]);
  if(saveToFile) sinCoeffPlots.SaveToFile(stem2.Data());

  //Cosine Coefficients
  DetectorPlots cosCoeffPlots(nDet, 1);
  cosCoeffPlots.SetMonitorList(MonitorList.data());
  cosCoeffPlots.SetDetectorList(DetectorList.data(), nDet);
  cosCoeffPlots.AppendToFileName(Form("_%sand%s",stm1.Data(), stm2.Data()));
  cosCoeffPlots.SetTitleX("Slug");
  cosCoeffPlots.SetCoilType(1);
  if(first>=137)
    cosCoeffPlots.SetRunPeriod(2);
  else if(last<=136)
    cosCoeffPlots.SetRunPeriod(1);
  cosCoeffPlots.SetDrawLegend(&legend[0], nTYPE); 
  cosCoeffPlots.Plot(vSlug, vSlugErr, vCosCoeff[0], vCosCoeffErr[0], vCosCoeff[1],
		  vCosCoeffErr[1], vCosCoeff[2], vCosCoeffErr[2]);
  if(saveToFile) cosCoeffPlots.SaveToFile(stem2.Data());


  //Sine Residuals
  DetectorPlots sinResPlots(nDet, 2);
  sinResPlots.SetMonitorList(MonitorList.data());
  sinResPlots.SetDetectorList(DetectorList.data(), nDet);
  sinResPlots.AppendToFileName(Form("_%sand%s",stm1.Data(), stm2.Data()));
  sinResPlots.SetTitleX("Slug");
  sinResPlots.SetCoilType(0);
  if(first>=137)
    sinResPlots.SetRunPeriod(2);
  else if(last<=136)
    sinResPlots.SetRunPeriod(1);
  sinResPlots.SetDrawLegend(&legend[0], nTYPE); 
  sinResPlots.Plot(vSlug, vSlugErr, vSinRes[0], vSinResErr[0], vSinRes[1], 
		  vSinResErr[1], vSinRes[2], vSinResErr[2]);
  if(saveToFile) sinResPlots.SaveToFile(stem2.Data());

  //Cosine Coefficients
  DetectorPlots cosResPlots(nDet, 2);
  cosResPlots.SetMonitorList(MonitorList.data());
  cosResPlots.SetDetectorList(DetectorList.data(), nDet);
  cosResPlots.AppendToFileName(Form("_%sand%s",stm1.Data(), stm2.Data()));
  cosResPlots.SetTitleX("Slug");
  cosResPlots.SetCoilType(1);
  if(first>=137)
    cosResPlots.SetRunPeriod(2);
  else if(last<=136)
    cosResPlots.SetRunPeriod(1);
  cosResPlots.SetDrawLegend(&legend[0], nTYPE); 
  cosResPlots.Plot(vSlug, vSlugErr, vCosRes[0], vCosResErr[0], vCosRes[1], 
		  vCosResErr[1], vCosRes[2], vCosResErr[2]);
  if(saveToFile) cosResPlots.SaveToFile(stem2.Data());
  /////////////////////////////////////////////////////////

  return 0;
}
