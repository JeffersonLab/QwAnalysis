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


//Declare constants
/////////////////////////////////////////////////////////
const Int_t nDET = 32, nMON = 5, nMOD = 5;
const Double_t  errorFlag = -999999, DegToRad = 0.0174532925199432955;
/////////////////////////////////////////////////////////

struct Header_t{
  Int_t macrocycle;
  Int_t microcycle[nMOD];
  Int_t id;
  Int_t lEntry;
  Int_t hEntry;
};

Int_t ExtractHeader(ifstream &file, Header_t &header, Int_t size = 9){
  string str;
  char *correct_name[9] = {"Macrocycle", "Mod0", "Mod1","Mod2", "Mod3", "Mod4",
		     "Id", "first_entry", "last_entry"};
  Int_t *temp = new Int_t[size];
  for(int i=0;i<size;i++){
    char name[30]={0}, num[30]={0};
    file>>name>>num;
    if(strcmp(name, correct_name[i])==0)
      temp[i] = atoi(num);
    else{
      cout<<"Unexpected header format. Exiting.\n";
      return -1;
    }
  }
  getline(file, str);
  Int_t idx = 0;
  header.macrocycle = temp[idx];
  idx++;
  for(int i=0;i<nMOD;i++){
    header.microcycle[i] = temp[idx];
    idx++;
  }
  header.id = temp[idx];
  idx++;
  header.lEntry = temp[idx];
  idx++;
  header.hEntry = temp[idx];
  return 0;
}


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


Int_t GetMonitorAndDetectorLists(TChain *ch, TString *monitorList, 
				 TString *detectorList, Bool_t trunc){
  string str;
  ifstream file(Form("%s/config/setup_mpsonly.config",
		     gSystem->Getenv("BMOD_SRC")));

  for(int i=0;i<5;i++){
    char id[4] = "   ", monitr[20] = "                   ";
    file>>id>>monitr;
    getline(file, str);
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
  getline(file, str);
  Int_t nDet = 0;
  for(int i=0;i<nDET&&!file.eof();i++){
    char id[4] = "   ", detectr[20] = "                   ";
    file>>id>>detectr;
    getline(file, str);
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
  file.close();
  return nDet;
}

//NOTE:this program creates 'run<run_number>SineAmpl.dat' and 
//     'run<run_number>CosineAmpl.dat' files and uses the following
//     ordering: X, Y, E, XP, YP, the same as the time ordering.  

//Don Jones 
//January 2014
//This macro was created to check the slopes found by the modulation code
//by applying correction slopes in the slopes files averaged over a given
//run range and correcting the data to find residual sensitivities to coils
//on a cycle by cycle basis. 
//Program assumes existence of MacrocyleSlopesTree_ChSqMin.set0.root
//////////////////////////////////////////////////////////////////////////


Int_t variableRunRangeBModResiduals(Int_t run = 13993, Int_t plotAsProfile = 1, 
				    Int_t chsq = 1, Int_t phase_set = 0, 
				    Int_t run_average = 0){
  gStyle->SetOptFit(1111);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.25);
  gStyle->SetOptStat(0);
  Bool_t debug = 1, pretty_plot = 1;
  Bool_t useMacrocycleSlopesTree = 1;
  const Int_t minEntriesPerCycle = 100;
  const Int_t minEntriesPerBin = 5;
  const Int_t minFullBins = 7;

  //Declare needed variables
  /////////////////////////////////////////////////////////
  char *chisquare = "";
  string line;
  Int_t nDet = 0, size = 0;
  Double_t phase[nMOD];  
  TString ModulationList[nMOD] = {"x", "y", "e", "xp", "yp"};
  TString MonitorList[nMON], DetectorList[nDET],detector;
  TString MonitorListFull[nMON], DetectorListFull[nDET];
  TString scut = TString(1000);
  TCut cut;
  TChain *ch;
  if(debug)std::cout<<"(1)Working.\n"; 
  /////////////////////////////////////////////////////////


  if(chsq)chisquare = (char *)"_ChiSqMin";


  //Get phase offsets from configuration file.
  /////////////////////////////////////////////////////////
  ifstream phaseFile(Form("%s/config/phase_set%i.config",
			  gSystem->Getenv("BMOD_SRC"),phase_set));
  if(phaseFile.is_open() && phaseFile.good()){
    char type[nMOD][255], num[nMOD][255];
    for(int j=0; j<nMOD; j++){
	phaseFile>>type[j]>>num[j];
	getline(phaseFile, line);
    }
    for(int i=0; i<nMOD; i++){
      for(int j=0; j<nMOD; j++){
	if(ModulationList[i].CompareTo(type[j])==0){
	  phase[i] = atof(num[j]);
	  printf("phase[%i(%s)]=%f\n",i, type[j], phase[i]);
	}
      }
    }

  }else{
    cout<<phaseFile<<" not found. Exiting.\n";
  }
  phaseFile.close(); 
  for(int imod=0;imod<nMOD;imod++)
    ModulationList[imod].ToUpper();

  if(debug)std::cout<<"(2)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Create chain of mps_only runlets.
  /////////////////////////////////////////////////////////
  ch = new TChain("mps_slug");
  ch->Add(Form("%s/mps_only_%i*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES"),run));
  TFile *friendTree = new TFile(Form("%s/mps_only_friend_%i.root",
				     gSystem->Getenv("MPS_ONLY_ROOTFILES"), run));
  if(friendTree->IsOpen()){
    ch->AddFriend("mps_slug", friendTree);
  }else{
    std::cout<<"Friend file not found exiting program.\n";
    return -1;
  }
  if(debug)std::cout<<"(3)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Get lists of monitors and detectors
  /////////////////////////////////////////////////////////
  GetMonitorAndDetectorLists(ch, MonitorListFull, DetectorListFull,0);
  nDet = GetMonitorAndDetectorLists(ch, MonitorList, DetectorList,1);
  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return -1;
  }
  if(debug)std::cout<<"(4)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Open files to store residuals.
  //////////////////////////////////////////////////////////////////
  size = 9;
  FILE *residualFile = fopen(Form("%s/%sslopes/variable_run_range_residuals/"
			      "residuals%i%s.set0.dat", gSystem->Getenv("BMOD_OUT"),
			      (run_average ? "" : "macrocycle_"),run,chisquare), "w");
  if(residualFile == NULL){
    perror ("Error opening residual file. Exiting.\n");
    return -1;
  }
  if(debug)std::cout<<"(5)Working.\n"; 
  //////////////////////////////////////////////////////////////////


  //Get tree of slopes from file.
  /////////////////////////////////////////////////////////
  TFile fSl(Form("%s/../%slopesTree_ChiSqMin.set0.root",
		 gSystem->Getenv("BMOD_OUT"), 
		 (useMacrocycleSlopesTree ? "MacrocycleS" : "s")));
  TTree *slopes = (TTree*)fSl.Get("slopes");   

  if(debug)std::cout<<"(6)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Find proper run range from file.  
  /////////////////////////////////////////////////////////
  gROOT->ProcessLine(Form(".!ls %s/slopelists2/slopeList_asym_qwk_bcm1*"
			  "> slopeList.txt", gSystem->Getenv("BMOD_OUT")));
  ifstream fSlList("slopeList.txt");
  if(!(fSlList.is_open() && fSlList.good())){
    cout<<"Error opening slopeList.txt. Exiting.\n";
    return -1;
  }else cout<<"slopeList.txt found.\n";
  size = 0;
  Int_t lRun, hRun;
  getline(fSlList, line);
  while(!fSlList.eof()){
    ++size;
    std::size_t pos = line.find("bcm1.");
    std::string f_str = line.substr(pos+5, 5);
    lRun = atoi(f_str.data());
    std::string l_str = line.substr(pos+11, 5);
    hRun = atoi(l_str.data());
    if(run >= lRun && run <= hRun){
      cout<<"Run range: "<<lRun<<" to "<<hRun<<endl;
      break;
    }
    getline(fSlList, line);
  }
  fSlList.close(); 
  if(debug)std::cout<<"(7)Working.\n";
  /////////////////////////////////////////////////////////


  //Set up branch addresses.
  /////////////////////////////////////////////////////////
  Int_t good;
  Double_t rn;
  Double_t slope[nDET][nMON];
  slopes->SetBranchAddress("good",&good);
  slopes->SetBranchAddress("run",&rn);
  for(int idet=0;idet<nDet;++idet){
    for(int imon=0;imon<nMON;++imon){
      slopes->SetBranchAddress(Form("%s_%s",DetectorList[idet].Data(),
				    MonitorList[imon].Data()), 
			       &slope[idet][imon]);
    }
  }
  if(debug)std::cout<<"(8)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Get average slopes from trees and find differences.  
  /////////////////////////////////////////////////////////
  Int_t ent = 0;
  Double_t slopeErr[nDET][nMON];    
  //slopes tree
  vector< vector< vector <Double_t> > >v_slope;
  v_slope.resize(nDet);
  for(int idet=0;idet<nDet;++idet){
    v_slope[idet].resize(nMON);
  }

  slopes->GetEntry(0);
  while(rn <= hRun && ent<slopes->GetEntries()){
    if(rn >= lRun && good){
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
  delete slopes;

  for(int idet=0;idet<nDet;++idet){
    for(int imon=0;imon<nMON;++imon){
      Double_t temp[2] = {0.0,0.0};
      Double_t *temp_slope = v_slope[idet][imon].data();
      FindAverageAndError(temp_slope, temp, v_slope[idet][imon].size());
      slope[idet][imon] = temp[0]*(MonitorList[imon].Contains("Slope")? 1.0 : 1.0e-3);
      slopeErr[idet][imon]= temp[1]*(MonitorList[imon].Contains("Slope")? 1.0 : 1.0e-3);
      if(debug&&idet==imon&&imon==0)
	cout<<DetectorList[idet].Data()<<"_"<<MonitorList[imon].Data()<<": "<<
	  slope[idet][imon]<<"+/-"<<slopeErr[idet][imon]<<
	  "    size:"<<v_slope[idet][imon].size()<<"   lRun: "<<lRun<<
	  "   hRun: "<<hRun<<endl;
    }
  }
  if(debug)std::cout<<"(9)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Set up branch addresses.
  /////////////////////////////////////////////////////////
  Double_t monit[nMON], detect[nDET], pat_num;
  for(int imon=0;imon<nMON;imon++){
    ch->SetBranchAddress(Form("%s",MonitorListFull[imon].Data()),&monit[imon]);
  }
  for(int idet=0;idet<nDet;idet++){
    ch->SetBranchAddress(Form("%s",DetectorListFull[idet].Data()),&detect[idet]);
  }
  ch->SetBranchAddress("bm_pattern_number", &pat_num);
  /////////////////////////////////////////////////////////


  //Correct data with average slopes cycle by cycle.  
  /////////////////////////////////////////////////////////
  Int_t nCycle = 0, ient = 0; 
  ch->GetEntry(ient);
  Int_t pattern = (pat_num<5 ? (int)pat_num : (int)pat_num-11);
  while(ient<ch->GetEntries()){

    //1)Get mean monitor and detector yields for current cycle.
    /////////////////////////////////////////////////////////
    Double_t meanMon[nMON] = {0,0,0,0,0}, meanDet[nDET];
    Double_t resSin[nDET], resSinErr[nDET];
    Double_t resCos[nDET], resCosErr[nDET];
    Int_t nEntries = 0;
    for(int idet=0;idet<nDet;++idet){
      meanDet[idet] = 0;
      resSin[idet] = 0;
      resSinErr[idet] = 0;
      resCos[idet] = 0;
      resCosErr[idet] = 0;
    }

    Int_t prev_pattern = pattern, first = ient;
    while(prev_pattern==pattern && ient<ch->GetEntries()){
      nEntries++;
      for(int imon=0;imon<nMON;++imon){
	meanMon[imon] += monit[imon];
      }
      for(int idet=0;idet<nDet;++idet){
	meanDet[idet] += detect[idet];
      }
      prev_pattern = pattern;
      ient++;
      ch->GetEntry(ient);
      pattern = (pat_num<5 ? (int)pat_num : (int)pat_num-11);
    }
    cout<<"Pattern:"<<pattern<<"   Previous: "<<prev_pattern<<endl;
    Int_t last = ient - 1, mod_type = prev_pattern;;

    for(int imon=0;imon<nMON;imon++){
      meanMon[imon] /= nEntries;
      printf("Mean %s (Entries:%i-%i) = %f   Mod type: %i\n", MonitorList[imon].Data(),
	     first, last, meanMon[imon], mod_type);
    }
    for(int idet=0;idet<nDet;idet++){
      meanDet[idet] /= nEntries;
      printf("Mean %s (Entries:%i-%i) = %f   Mod type: %i\n", DetectorList[idet].Data(),
	     first, last, meanDet[idet],  mod_type);
    }
   
    if(debug)std::cout<<"(11)Working.\n"; 
    /////////////////////////////////////////////////////////


    //2)Apply the slopes to the detector data 
    //to see if correlations removed.
    /////////////////////////////////////////////////////////
    cut = Form("bm_pattern_number == %i||bm_pattern_number == %i",mod_type, 
	       11+mod_type);
    if(!run_average)
      cut += Form("Entry$>=%i && Entry$<=%i", first, last);
    cut.Print();
    std::cout<<"Entries passing cut: "<<ch->GetEntries(cut)<<"\n";
    if(ch->GetEntries(cut)>minEntriesPerCycle){
      for(int idet=0;idet<nDet;idet++){
	Int_t allGood = 1;
	for(int imon=0;imon<nMON;++imon){
	  if(slope[idet][imon]== errorFlag || 
	     slopeErr[idet][imon]== errorFlag )allGood = 0;
	}
	if(allGood){
	  TF1 *f = new TF1("f",Form("[0]+[1]*sin(x*%f+%f)+[2]*cos(x*%f+%f)",
				    DegToRad, phase[mod_type], DegToRad, 
				    phase[mod_type]), 0, 360);
	  if(plotAsProfile){
	    cout<<Form("%s/%e*(1-%e*(%s-%e)-%e*(%s-%e)-%e*(%s-%e)"
			  "-%e*(%s-%e)-%e*(%s-%e)):ramp_filled>>prC%i\n",
			  DetectorListFull[idet].Data(),meanDet[idet],
			  slope[idet][0],MonitorListFull[0].Data(),meanMon[0],
			  slope[idet][1],MonitorListFull[1].Data(),meanMon[1],
			  slope[idet][2],MonitorListFull[2].Data(),meanMon[2],
			  slope[idet][3],MonitorListFull[3].Data(),meanMon[3],
			  slope[idet][4],MonitorListFull[4].Data(),meanMon[4],
		       nCycle);
	    ch->Draw(Form("%s/%e*(1-%e*(%s-%e)-%e*(%s-%e)-%e*(%s-%e)"
			  "-%e*(%s-%e)-%e*(%s-%e)):ramp_filled>>prC%i",
			  DetectorListFull[idet].Data(),meanDet[idet],
			  slope[idet][0],MonitorListFull[0].Data(),meanMon[0],
			  slope[idet][1],MonitorListFull[1].Data(),meanMon[1],
			  slope[idet][2],MonitorListFull[2].Data(),meanMon[2],
			  slope[idet][3],MonitorListFull[3].Data(),meanMon[3],
			  slope[idet][4],MonitorListFull[4].Data(),meanMon[4],
			  nCycle), cut, "prof");
	    TProfile* prC = (TProfile*)gDirectory->Get(Form("prC%i",nCycle));
	    vector<Double_t> vx, vxe, vy, vye;

	    Double_t max = prC->GetBinContent(1)+2*prC->GetBinError(1);
	    Double_t min = prC->GetBinContent(1)-2*prC->GetBinError(1);
	    for(int ibin=1;ibin<prC->GetNbinsX();++ibin){
	      if(prC->GetBinEntries(ibin) >= minEntriesPerBin){
		Double_t val = prC->GetBinContent(ibin)+2*prC->GetBinError(ibin);
		if(max<val && prC->GetBinContent(ibin)!=0){
		  max = val;
		}
		val = prC->GetBinContent(ibin)-2*prC->GetBinError(ibin);
		if(min>val&&prC->GetBinContent(ibin)!=0){
		  min = val;
		}
		vx.push_back(prC->GetBinCenter(ibin));
		vxe.push_back(prC->GetBinWidth(ibin)/2.0);
		vy.push_back(prC->GetBinContent(ibin));
		vye.push_back(prC->GetBinError(ibin));
		cout<<ibin<<": "<<prC->GetBinContent(ibin)<<" +/- "
		    << prC->GetBinError(ibin)<<"  N: "<<prC->GetBinEntries(ibin)
		    <<"   "<<prC->GetBinCenter(ibin)<<endl;
	      }	    
	    }
	    if((int)vx.size() < minFullBins){
	      std::cout<<"Number of good events too small for graph."<<
		" Using TProfile::Approximate() for errors."<< std::endl;
	      prC->Approximate(1);
	      vx.clear();
	      vxe.clear();
	      vy.clear();
	      vye.clear();
	      for(int ibin=1; ibin<prC->GetNbinsX(); ++ibin){
		if(prC->GetBinEntries(ibin)>0){
		  vx.push_back(prC->GetBinCenter(ibin));
		  vxe.push_back(prC->GetBinWidth(ibin)/2.0);
		  vy.push_back(prC->GetBinContent(ibin));
		  vye.push_back(prC->GetBinError(ibin));
		}
	      }
	    }
	    
	    TGraphErrors *grC = new TGraphErrors((int)vx.size(), vx.data(), vy.data(), 
						vxe.data(), vye.data());
	    grC->Draw("ap");
	
	    grC->GetYaxis()->SetRangeUser(min,max);
	    f->SetParameter(0,1);
	    f->SetParameter(1,0);
	    f->SetParameter(2,0);
	    if(grC->Fit(f, "REX0")<0){
	      cout<<"Fit failure. Exiting.\n";
	      return -1;
	    }
	    f->SetLineColor(kMagenta);
	    TF1 *fcos = new TF1("fcos",Form("%e+%e*cos(x*%f+%f)",f->GetParameter(0),
					    f->GetParameter(2), DegToRad, 
					    phase[mod_type]),
				0, 360);
	    fcos->SetLineColor(kBlue);
	    TF1 *fsin = new TF1("fsin",Form("%e+%e*sin(x*%f+%f)",f->GetParameter(0),
					    f->GetParameter(1), DegToRad, 
					    phase[mod_type]), 0, 360);
	    fsin->SetLineColor(kRed);

	    resSin[idet]= f->GetParameter(1);
	    resSinErr[idet]= f->GetParError(1);
	    resCos[idet]= f->GetParameter(2);
	    resCosErr[idet]= f->GetParError(2);
	    if(pretty_plot){
	      grC->SetTitle(Form("Corrected %s ModType=%i(%s)", 
				 DetectorList[idet].Data(), mod_type, 
				 ModulationList[mod_type].Data()));
	      grC->SetTitle(Form("Corrected %s ModType=%i(%s)", 
				 DetectorList[idet].Data(), mod_type, 
				 ModulationList[mod_type].Data()));
	      grC->GetYaxis()->SetTitle(Form("Corrected %s",DetectorList[idet].Data()));
	      grC->GetXaxis()->SetTitle("ramp_filled");
	      grC->GetXaxis()->SetTitleSize(0.055);
	      grC->GetXaxis()->SetTitleOffset(0.8);
	      grC->Draw("ap");
	    }
	    fsin->Draw("same");
	    fcos->Draw("same");
	    f->Draw("same");
	  }else{
	    ch->Draw(Form("%s/%e*(1-%e*(%s-%e)-%e*(%s-%e)-%e*(%s-%e)"
			  "-%e*(%s-%e)-%e*(%s-%e)):ramp_filled>>hC%i",
			  DetectorListFull[idet].Data(),meanDet[idet],
			  slope[idet][0],MonitorListFull[0].Data(),meanMon[0],
			  slope[idet][1],MonitorListFull[1].Data(),meanMon[1],
			  slope[idet][2],MonitorListFull[2].Data(),meanMon[2],
			  slope[idet][3],MonitorListFull[3].Data(),meanMon[3],
			  slope[idet][4],MonitorListFull[4].Data(),meanMon[4],
			  nCycle), cut);

	    TH2D *hC = (TH2D*)gDirectory->Get(Form("hC%i",nCycle));
	    hC->Fit(f);
	    f->SetLineColor(kMagenta);
	    TF1 *fcos = new TF1("fcos",Form("%e+%e*cos(x*%f+%f)",f->GetParameter(0),
					    f->GetParameter(2), DegToRad, 
					    phase[mod_type]),
				0, 360);
	    fcos->SetLineColor(kBlue);
	    TF1 *fsin = new TF1("fsin",Form("%e+%e*sin(x*%f+%f)",f->GetParameter(0),
					    f->GetParameter(1), DegToRad, 
					    phase[mod_type]), 0, 360);
	    fsin->SetLineColor(kRed);

	    resSin[idet]= f->GetParameter(1);
	    resSinErr[idet]= f->GetParError(1);
	    resCos[idet]= f->GetParameter(2);
	    resCosErr[idet]= f->GetParError(2);
	    if(pretty_plot){
	      hC->SetTitle(Form("%s ModType=%i(%s)",
				DetectorList[idet].Data(), mod_type,
				ModulationList[mod_type].Data()));
	      hC->SetTitle(Form("Corrected %s ModType=%i(%s)",
				DetectorList[idet].Data(),mod_type,
				ModulationList[mod_type].Data()));
	      hC->GetYaxis()->SetTitle(Form("Corrected %s",
					    DetectorList[idet].Data()));
	      hC->GetXaxis()->SetTitle("ramp_filled");
	      hC->GetXaxis()->SetTitleSize(0.055);
	      hC->GetXaxis()->SetTitleOffset(0.8);
	      hC->Draw();
	    }
	    fsin->Draw("same");
	    fcos->Draw("same");
	    f->Draw("same");
	  }
	  gPad->Update();
	}else{
	  resSin[idet]= errorFlag;
	  resSinErr[idet]=  errorFlag;
	  resCos[idet]=  errorFlag;
	  resCosErr[idet]=  errorFlag;
	}
      }
      nCycle++;
      scut.Clear();
      /////////////////////////////////////////////////////////


      //3)Record data to file and print to screen.
      ////////////////////////////////////////////////////////
      printf("Corrected detector residual sensitivities to coils for cycle %i.\n",
	     nCycle);
      printf("Mod type: %i    Cycle: %i    Entry range: %i  %i\n", mod_type, nCycle, 
	     first, last);
      printf("Detector           Sine Response                   Cosine Response\n");

      fprintf(residualFile, "Mod_type: %i    Cycle: %i    Entry_range: %i  %i\n", 
	      mod_type, nCycle, first, last);
      fprintf(residualFile, "Detector           Sine Response"
	     "                   Cosine Response\n");

      for(int idet = 0;idet<nDet;++idet){
	TString detec = DetectorList[idet];
	detec.Resize(16);
	Bool_t goodSin = TMath::Abs(resSin[idet]) < resSinErr[idet];
	Bool_t goodCos = TMath::Abs(resCos[idet]) < resCosErr[idet];
	printf("%s%s %+13.6e +/-%11.4e %s   %+13.6e +/-%11.4e %s\n", 
	       detec.Data(), (!goodSin ? ANSI_COLOR_RED: ANSI_COLOR_RESET), 
	       resSin[idet], resSinErr[idet],
	       (!goodCos ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	       resCos[idet],resCosErr[idet],ANSI_COLOR_RESET);
	fprintf(residualFile,"%s %+13.6e +/-%11.4e   %+13.6e +/-%11.4e\n", 
		detec.Data(), resSin[idet], resSinErr[idet],
		resCos[idet],resCosErr[idet]);
      }
      fprintf(residualFile,"\n");
      ////////////////////////////////////////////////////////
    }
  }
  //////////////////////////////////////////////////////////////////



  printf("\nAverage detector slopes over run range %i to %i.\n", lRun, hRun);

  printf("                |     %s       |   %s    |     %s      |"
	 "     %s       |   %s    |\n", MonitorList[0].Data(),  
	 MonitorList[1].Data(), MonitorList[2].Data(), MonitorList[3].Data(),
	 MonitorList[4].Data());
  printf("*********************************************************************"
	 "*******************************************************************"
	 "*\n");

  for(Int_t idet=0;idet<nDet;idet++){
    TString detec = DetectorList[idet];
    detec.Resize(16);
    printf("%s%s|", ANSI_COLOR_RESET,detec.Data());
    for(Int_t imon=0;imon<nMON;imon++){
      Int_t good = 0;
      if(TMath::Abs(slope[idet][imon])<
	 TMath::Abs(slopeErr[idet][imon]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     slope[idet][imon]);
      printf("%s+/-%6.2e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     slopeErr[idet][imon], ANSI_COLOR_RESET);
    }
    printf("\n");
  }
  printf("\n\n");

  fclose(residualFile);

  return 0;
}











