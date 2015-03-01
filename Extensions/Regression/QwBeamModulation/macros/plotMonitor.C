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
const Int_t MIN_ENTRIES_PER_RUN = 5, MIN_ENTRIES_PER_SLUG = 10;
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
//avg_type: 0 no average
//          1 run average
//          2 slug average
//Program assumes existence of slopes tree.
//////////////////////////////////////////////////////////////////////////


void plotMonitor(int first = 13842, int last = 19000, int avg_type = 1, 
		 int non_lin_cut = 0, bool new_monitors = 0)
{
  Bool_t debug = 1, saveToFile = 1, useMacrocycleSlopesTree = 1; 
  vector<TString>DetectorList, MonitorList, DetectorListFull, MonitorListFull;
  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_13993*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES")));
  ch->AddFriend("mps_slug", Form("%s/mps_only_friend_13993.root",
				 gSystem->Getenv("MPS_ONLY_ROOTFILES")));


  TString configFile = Form("%s/config/setup_mpsonly%s.config", 
			    gSystem->Getenv("BMOD_SRC"),
			    (new_monitors ? "_new_monitors" : ""));
  GetMonitorAndDetectorLists(ch, MonitorList, DetectorList, 1,
			     (char*)configFile.Data());
  GetMonitorAndDetectorLists(ch, MonitorListFull, DetectorListFull, 0,
			      (char*)configFile.Data());



  //Get tree of slopes from file.
  /////////////////////////////////////////////////////////
  TFile fSl(Form("%s/../%slopesTree_ChiSqMin.%sset0%s.root",
		 gSystem->Getenv("BMOD_OUT"), 
		 (useMacrocycleSlopesTree ? "MacrocycleS" : "s"),
		 (non_lin_cut ? "cut180deg_" : ""),
		 (new_monitors? "_new_monitors" : "")));
  TTree *slopes = (TTree*)fSl.Get("slopes");   
  cout<<"Entries: "<<slopes->GetEntries()<<endl;;
  cout<<"Good entries: "<<slopes->GetEntries("good")<<endl;;
  if(debug)std::cout<<"(1)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Set up branch addresses.
  /////////////////////////////////////////////////////////
  Int_t good;
  Double_t slug, run = 0, cycle, sens[nMON][nCOIL], sens_err[nMON][nCOIL];
  slopes->SetBranchAddress("good",&good);
  slopes->SetBranchAddress("slug",&slug);
  slopes->SetBranchAddress("run",&run);
  slopes->SetBranchAddress("macrocycle",&cycle);

  for(int imon=0;imon<nMON;++imon){
    for(int icoil=0;icoil<nCOIL;++icoil){
      slopes->SetBranchAddress(Form("%s_Coil%i",MonitorList[imon].Data(),icoil), 
			       &sens[imon][icoil]);
      slopes->SetBranchAddress(Form("%s_Coil%i_err",MonitorList[imon].Data(),
				    icoil),  &sens_err[imon][icoil]);
    }
  }
 
  if(debug)std::cout<<"(2)Working.\n"; 
  /////////////////////////////////////////////////////////

  //Loop through tree filling vectors
  /////////////////////////////////////////////////////////
  vector<Double_t>vX, vXErr, vCycle, vCycleErr;
  vector<vector<vector<Double_t> > > vSens, vSensErr, vTempSens;
  Double_t tempSens[nMON][nCOIL][nSLUG], tempSensSq[nMON][nCOIL][nSLUG], N[nSLUG];
  for(int i=0;i<nSLUG;++i) N[i] = 0.0;
  vSens.resize(nMON);
  vSensErr.resize(nMON);
  vTempSens.resize(nMON);
  for(int imon=0;imon<nMON;++imon){
    vSens[imon].resize(nCOIL);
    vSensErr[imon].resize(nCOIL);
    vTempSens[imon].resize(nCOIL);
  }

  slopes->GetEntry(0);
  Double_t prev_run = run;
  Int_t nCycle = 0;
  for(int ient = 1; ient<slopes->GetEntries()+1;++ient){
    if(ient%500==0)printf("Processing entry %i\n",ient);
    if(run>=first && good){
      vCycle.push_back((double)nCycle);
      vCycleErr.push_back(0.0);
      nCycle++;
      ++N[(int)slug];
      Double_t n = N[(int)slug];
      for(int imon=0;imon<nMON;++imon){
	for(int icoil=0;icoil<nCOIL;++icoil){
	  vTempSens[imon][icoil].push_back(sens[imon][icoil]);
	  //Use running mean for slopes since they are not time ordered
	  tempSens[imon][icoil][(int)slug] = (tempSens[imon][icoil][(int)slug] * 
					      (n-1) + sens[imon][icoil]) / n;
	  tempSensSq[imon][icoil][(int)slug] = (tempSensSq[imon][icoil][(int)slug]
						*(n-1) + 
						pow(sens[imon][icoil],2)) / n;
	  if(slug == 141&&imon==3&&icoil==1)
	    cout<< tempSensSq[imon][icoil][(int)slug]<<endl;
	}
      }
    }
    prev_run = run;
    if(ient<slopes->GetEntries()) slopes->GetEntry(ient);
    if(prev_run!=run || ient==slopes->GetEntries()){
      if(avg_type == 0){
	for(int imon=0;imon<nMON;++imon){
	  for(int icoil=0;icoil<nCOIL;++icoil){
	    for(int idx=0;idx<(int)vTempSens[0][0].size();++idx){
	      vSens[imon][icoil].push_back(vTempSens[imon][icoil][idx]);
	      vSensErr[imon][icoil].push_back(0.0);
	    }
	  } 
	}
      }
      else if(avg_type == 1 && (int)vTempSens[0][0].size()>= MIN_ENTRIES_PER_RUN){
	vX.push_back(prev_run);
	vXErr.push_back(0);
	for(int imon=0;imon<nMON;++imon){
	  for(int icoil=0;icoil<nCOIL;++icoil){
	    Double_t average[2]={0,0};
	    FindAverageAndError(vTempSens[imon][icoil].data(),average,
				(int)vTempSens[0][0].size());
	    vSens[imon][icoil].push_back(average[0]);
	    vSensErr[imon][icoil].push_back(average[1]);
	  }
	}
      }
      for(int imon=0;imon<nMON;++imon)
	for(int icoil=0;icoil<nCOIL;++icoil)
	  vTempSens[imon][icoil].clear();
    }	

    if(run>last) break;
  }
  if(avg_type == 2){
    for(int islug=0;islug<nSLUG;++islug){
      if(N[islug]>MIN_ENTRIES_PER_SLUG){
	vX.push_back((double)islug);
	vXErr.push_back(0);
	for(int imon=0;imon<nMON;++imon){
	  for(int icoil=0;icoil<nCOIL;++icoil){
	    vSens[imon][icoil].push_back(tempSens[imon][icoil][islug]);
	    double error = sqrt(tempSensSq[imon][icoil][islug] - 
				pow(tempSens[imon][icoil][islug],2));
	    vSensErr[imon][icoil].push_back(error/sqrt(N[islug]));
	    if(icoil==9&&imon==1)
	      printf("slug:%i (%i) %s_Coil%i:%f +/- %f\n",(int)vX.back(), 
		     (int)N[islug],MonitorList[imon].Data(),icoil,
		     tempSens[imon][icoil][islug], error/sqrt(N[islug]));
	  }
	}
      }
    }
  }
  cout<<"Last: "<<vCycleErr.back()<<endl;
  /////////////////////////////////////////////////////////


  //Plot results.
  /////////////////////////////////////////////////
  TGraphErrors *gr[nMOD][nCOIL];
  TCanvas *cCoil[nCOIL];
  char *mod_type[nMOD] = {"X", "Y", "E", "XSlope", "YSlope"};
  const char *title = (avg_type==0? "Cycle" : (avg_type==1 ? "Run" : "Slug"));
  for(int icoil=0;icoil<nCOIL;++icoil){
    char *name = Form("cCoil%i",icoil);
    int color = (icoil<nMOD ? kRed : kBlue);
    cCoil[icoil] = new TCanvas(name, name, 0, 0, 1400,900);
    cCoil[icoil]->Divide(3,2);
    for(int imon=0;imon<nMON;++imon){
      cCoil[icoil]->cd(imon+1);
      int size = (int)(avg_type==0 ? vCycle.size() : vX.size());
      //      cout<<"size: "<<size<<" "<<vSens[imon][icoil].size()<<endl;
      gr[imon][icoil]= new TGraphErrors(size, (avg_type==0 ? vCycle.data() : 
					       vX.data()),
					vSens[imon][icoil].data(),
					(avg_type==0 ? vCycleErr.data() : 
					 vXErr.data()),
					vSensErr[imon][icoil].data());
      gr[imon][icoil]->Draw("ap");
      gr[imon][icoil]->SetLineColor(color);
      gr[imon][icoil]->SetMarkerColor(color);
      gr[imon][icoil]->SetMarkerStyle(0);
      gr[imon][icoil]->SetTitle(Form("Monitor %s Coefficient: %s_Coil%i (%s)",
				     (icoil<nMOD ? "Sine" : "Cosine"),
				     MonitorList[imon].Data(), icoil%nMOD, 
				     mod_type[icoil%nMOD]));
      gr[imon][icoil]->GetYaxis()->SetTitle(Form("%s_Coil%i (ppm)",
						 MonitorList[imon].Data(), 
						 icoil%nMOD));
      gr[imon][icoil]->GetXaxis()->SetTitle(title);

    }
    if(saveToFile)
      cCoil[icoil]->SaveAs(Form("%s/plots/Monitor%sCoefficients_%smod_vs_%s."
				"png", (new_monitors ? "/net/data1/paschkedata1"
					"/bmod_out_new_monitors" : 
					gSystem->Getenv("BMOD_OUT")), 
				(icoil<nMOD ? "Sine" : "Cosine"),
				mod_type[icoil%nMOD], title));
  }
  /////////////////////////////////////////////////

  return;
}
