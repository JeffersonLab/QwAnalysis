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
const Int_t nDET = 32, nMON = 5, nMOD = 5, nCOIL = 10, nSLUG = 322;
const Int_t MIN_ENTRIES_PER_RUN = 5, MIN_ENTRIES_PER_SLUG = 5;
const Double_t  errorFlag = -999999, DegToRad = 0.0174532925199432955;
/////////////////////////////////////////////////////////


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
//January 2014
//Universal program for plotting results tabulated in slopes trees. 
// plot_type: 0 plot detector to monitor slopes
//            1 plot detector to coil sensitivities
//            2 plot residual detector to coil sensitivities
//avg_type: 0 no average
//          1 run average
//          2 slug average
//Program assumes existence of slopes tree.
//////////////////////////////////////////////////////////////////////////


void plotDetector(TString stem = "", int first = 9949, int last = 19000, 
		  int plot_type = 0, int avg_type = 1, bool cosine = 1)
{
  gStyle->SetMarkerStyle(7);
  int run_period = 0;
  Bool_t debug = 1, saveToFile = 1, useMacrocycleSlopesTree = 1; 
  vector<TString>DetectorList, MonitorList, DetectorListFull, MonitorListFull;
  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_13993*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  ch->AddFriend("mps_slug", Form("%s/mps_only_friend_13993.root",
				 gSystem->Getenv("MPS_ONLY_ROOTFILES")));

  TString configFile = Form("%s/config/setup_mpsonly%s.config", 
			    gSystem->Getenv("BMOD_SRC"), stem.Data());
  Int_t nDet = GetMonitorAndDetectorLists(ch, MonitorList, DetectorList, 1,
					  (char*)configFile.Data());
  GetMonitorAndDetectorLists(ch, MonitorListFull, DetectorListFull, 0,
			      (char*)configFile.Data());


  //Get tree of slopes from file.
  /////////////////////////////////////////////////////////
  TFile fSl(Form("%s/../%slopesTree_ChiSqMin.set0%s.root",
		 gSystem->Getenv("BMOD_OUT"), 
     		 (useMacrocycleSlopesTree ? "MacrocycleS" : "s"), stem.Data()));
  TTree *slopes = (TTree*)fSl.Get("slopes");   
  cout<<"Entries: "<<slopes->GetEntries()<<endl;;
  if(debug)std::cout<<"(1)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Set up branch addresses.
  /////////////////////////////////////////////////////////
  Int_t good;
  Double_t slug, run = 0, cycle, **sens, **sens_err;
  sens = new Double_t*[nDet];
  sens_err = new Double_t*[nDet];
  for(int idet=0;idet<nDet;++idet){
    sens[idet] = new Double_t[(plot_type == 0 ? nMON : nCOIL)];
    sens_err[idet] = new Double_t[(plot_type == 0 ? nMON : nCOIL)];
  }
  slopes->SetBranchAddress("good",&good);
  slopes->SetBranchAddress("slug",&slug);
  slopes->SetBranchAddress("run",&run);
  slopes->SetBranchAddress("macrocycle",&cycle);

  switch (plot_type)
  {
    case(0):

     for(int idet=0;idet<nDet;++idet){
	for(int imon=0;imon<nMON;++imon){
	  slopes->SetBranchAddress(Form("%s_%s",DetectorList[idet].Data(),
					MonitorList[imon].Data()), 
				   &sens[idet][imon]);
	  slopes->SetBranchAddress(Form("%s_%s_err",DetectorList[idet].Data(),
					MonitorList[imon].Data()), 
				   &sens_err[idet][imon]);
	}
      }
      break;

    case(1):
      for(int idet=0;idet<nDet;++idet){
	for(int icoil=0;icoil<nCOIL;++icoil){
	  slopes->SetBranchAddress(Form("%s_Coil%i",DetectorList[idet].Data(), 
					icoil), &sens[idet][icoil]);
	  slopes->SetBranchAddress(Form("%s_Coil%i_err",DetectorList[idet].Data(),
					icoil), &sens_err[idet][icoil]);
	}
      }
      break;

    case(2):
      for(int idet=0;idet<nDet;++idet){
	for(int imod=0;imod<nMOD;++imod){
	  slopes->SetBranchAddress(Form("SineRes_%s_Coil%i",		
					DetectorList[idet].Data(), imod),
				   &sens[idet][imod]);
	  slopes->SetBranchAddress(Form("SineRes_%s_Coil%i_err",    	
					DetectorList[idet].Data(), imod),
				   &sens_err[idet][imod]);
	  slopes->SetBranchAddress(Form("CosineRes_%s_Coil%i",	    
					DetectorList[idet].Data(), imod),
				   &sens[idet][imod+nMOD]);
	  slopes->SetBranchAddress(Form("CosineRes_%s_Coil%i_err",  	
					DetectorList[idet].Data(), imod),
				   &sens_err[idet][imod+nMOD]);
	}
      }
      break;

    default:
      std::cout<<"Invalid plot type. Exiting.\n";
      return;
  }
 
  if(debug)std::cout<<"(2)Working.\n"; 
  /////////////////////////////////////////////////////////

  //Loop through tree filling vectors
  /////////////////////////////////////////////////////////
  vector<Double_t>vX, vXErr, vCycle, vCycleErr;
  vector<vector<vector<Double_t> > > vSens, vSensErr, vTempSens;
  Double_t ***tempSens, ***tempSensSq, N[nSLUG]= {0};
  int num = (plot_type==0 ? nMON : nCOIL);
  tempSens = new Double_t **[nDet];
  tempSensSq = new Double_t **[nDet];
  for(int idet=0;idet<nDet;++idet){
    tempSens[idet] = new Double_t *[num];
    tempSensSq[idet] = new Double_t *[num];
    for(int i=0;i<num;++i){
      tempSens[idet][i] = new Double_t [nSLUG]();
      tempSensSq[idet][i] = new Double_t [nSLUG]();
    }
  }


  vSens.resize(nDet);
  vSensErr.resize(nDet);
  vTempSens.resize(nDet);
  for(int idet=0;idet<nDet;++idet){
    vSens[idet].resize(num);
    vSensErr[idet].resize(num);
    vTempSens[idet].resize(num);
  }

  slopes->GetEntry(0);
  Double_t prev_run;
  Int_t nCycle = 0;
  for(int ient = 1; ient<slopes->GetEntries();++ient){
    if(ient%500==0)printf("Processing entry %i\n",ient);
    if(run>=first && good){
      vCycle.push_back((double)nCycle);
      nCycle++;
      ++N[(int)slug];
      Double_t n = N[(int)slug];
      for(int idet=0;idet<nDet;++idet){
	for(int i=0;i<num;++i){
	  vTempSens[idet][i].push_back(sens[idet][i]);
	  //Use running mean for slopes since they are not time ordered
	  tempSens[idet][i][(int)slug] = (tempSens[idet][i][(int)slug] * (n-1) 
					  + sens[idet][i]) / n;
	  tempSensSq[idet][i][(int)slug] = (tempSensSq[idet][i][(int)slug]* (n-1) 
					    + pow(sens[idet][i],2)) / n;
	}
      }
    }
    prev_run = run;
    slopes->GetEntry(ient);
    if(prev_run!=run){
      if(avg_type == 0){
	for(int idet=0;idet<nDet;++idet){
	  for(int i=0;i<num;++i){
	    for(int idx=0;idx<(int)vTempSens[0][0].size();++idx){
	      vSens[idet][i].push_back(vTempSens[idet][i][idx]);
	      vSensErr[idet][i].push_back(0.0);
	    }
	  } 
	}
      }
      else if(avg_type == 1 && (int)vTempSens[0][0].size()>= MIN_ENTRIES_PER_RUN){
	vX.push_back(prev_run);
	vXErr.push_back(0);
	for(int idet=0;idet<nDet;++idet){
	  for(int i=0;i<num;++i){
	    Double_t average[2]={0,0};
	    FindAverageAndError(vTempSens[idet][i].data(),average,
				(int)vTempSens[0][0].size());
	    vSens[idet][i].push_back(average[0]);
	    vSensErr[idet][i].push_back(average[1]);
	  }
	}
      }
      for(int idet=0;idet<nDet;++idet)
	for(int i=0;i<num;++i)
	  vTempSens[idet][i].clear();
    }	

    if(run>last) break;
  }
  if(avg_type == 2){
    for(int islug=0;islug<nSLUG;++islug){
      if(N[islug]>MIN_ENTRIES_PER_SLUG){
	vX.push_back((double)islug);
	vXErr.push_back(0);
	for(int idet=0;idet<nDet;++idet){
	  for(int i=0;i<num;++i){
	    vSens[idet][i].push_back(tempSens[idet][i][islug]);
	    double error = sqrt(tempSensSq[idet][i][islug] - 
				pow(tempSens[idet][i][islug],2));
	    vSensErr[idet][i].push_back(error/sqrt(N[islug]));
	  }
	}
      }
    }
  }
  /////////////////////////////////////////////////////////


  //Plot results.
  /////////////////////////////////////////////////
  DetectorPlots detPlots(nDet, plot_type);
  detPlots.SetDetectorList(DetectorList.data(), nDet);
  detPlots.SetMonitorList(MonitorList.data());
  //  detPlots.SetMarkerColor(0,kBlue);
  detPlots.AppendToFileName((char*)stem.Data());
  detPlots.SetTitleX((avg_type==0 ? "Cycle" : (avg_type==1 ?"Run":"Slug")));
//   cout<<"Would you like Run1, Run2 or neither to appear in the file names? "<<
//     "Type 0, 1, or 2\n";
//   int r;
//   cin>>r;
  detPlots.SetRunPeriod(run_period);
  if(plot_type!=0){
    detPlots.SetCoilType((cosine ? 1 : 0));
    detPlots.SetMarkerColor(0,(cosine ? kBlue : kRed));
    detPlots.SetLineColor(0,(cosine ? kBlue : kRed));
    //copy cosines to first of array
    if(cosine){
      for(int idet=0;idet<nDet;++idet)
	for(int imod=0;imod<nMOD;++imod)
	  for(int i=0;i<(int)vSens[idet][imod].size();++i){
	    vSens[idet][imod][i] = vSens[idet][imod+nMOD][i];
	    vSensErr[idet][imod][i] = vSensErr[idet][imod+nMOD][i];
	  }
    }
  }
  cout<<vSens[0][0].size()<<" good entries.\n";
  if(avg_type==0){
    detPlots.Plot(vCycle, vCycleErr, vSens, vSensErr);
  }
  else if(avg_type!=0)
    detPlots.Plot(vX, vXErr, vSens, vSensErr);
  
  if(saveToFile) detPlots.SaveToFile(stem.Data());
  /////////////////////////////////////////////////

  return;
}
