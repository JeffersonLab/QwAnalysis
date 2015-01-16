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
#include "TGraphErrors.h"
#include "TF1.h"
#include "TChain.h"
#include "TMath.h"
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
using namespace std;


//Declare constants
/////////////////////////////////////////////////////////
const Int_t nDET = 60, nMON = 5, nMOD = 5;
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

Int_t GetMonitorAndDetectorLists(TChain *ch, TString *monitorList, 
				 TString *detectorList, Bool_t trunc,
				 char *config){
  string str;
  ifstream file(config);

  for(int i=0;i<5;i++){
    char id[4] = "   ", monitr[20] = "                   ";
    file>>id>>monitr;
    getline(file, str);
    monitorList[i] = TString(monitr);
    if(!ch->GetBranch(monitorList[i].Data())){
      cout<<monitorList[i].Data()<<" "<<ch->GetEntries()<<" missing. Exiting.\n";
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

//NOTE:this program creates the 'run<run_number>SineAmpl.dat' and 
//     'run<run_number>CosineAmpl.dat' files and uses the following
//     ordering: X, Y, E, XP, YP, the same as the time ordering.  

//Don Jones 
//June 2013
//This macro was created to check the results of the modulation code
//by applying the corrections recorded in the diagnostic file to the
//data and then fitting a sine curve to the corrected data to see if 
//there are any residual correlations. 
//THIS MACRO TESTS THE VALIDITY OF THE FINAL SLOPES AFTER INVERSION.



Int_t findBModResiduals(Int_t run = 13993, char *config = "", char *output = "",
			Int_t plotAsProfile = 0, Int_t chsq = 1, 
			Int_t phase_set = 0, Bool_t run_average = 0, 
			Bool_t nonlin_cut = 0,Bool_t isTransverseData = 0){
  gStyle->SetOptFit(1111);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.25);
  gStyle->SetOptStat(0);
  Bool_t debug = 0, pretty_plot = 1;
  Int_t minEntriesPerBin = 7, minFullBins = 5;

  //Declare needed variables
  /////////////////////////////////////////////////////////
  char *chisquare = "";
  string line;
  Int_t nDet = 0, n = 0, size = 0;
  Double_t phase[nMOD];  
  Double_t meanMon[nMOD][nMON], meanDet[nMOD][nDET];
  //Slopes read from output of mps only code
  Double_t DetectorSlope[nMOD][nDET], DetectorSlopeError[nMOD][nDET];
  //Slopes determined from profile fits to data
  Double_t CorrectedDetectorSin[nMOD][nDET],
    CorrectedDetectorSinError[nMOD][nDET];
  Double_t CorrectedDetectorCos[nMOD][nDET], 
    CorrectedDetectorCosError[nMOD][nDET];
  TString ModulationList[nMOD] = {"x", "y", "e", "xp", "yp"};
  TString MonitorList[nMOD], DetectorList[nDET], detector;
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
  ch->Add(Form("%s/%smps_only_%i_0*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES"),
	       (isTransverseData ? "transverse/":""), run));
  ch->Add(Form("%s/%smps_only_%i_5*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES"),
	       (isTransverseData ? "transverse/":""), run));
  ch->Add(Form("%s/%smps_only_%i_1*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES"),
	       (isTransverseData ? "transverse/":""), run));
  ch->Add(Form("%s/%smps_only_%i_2*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES"),
	       (isTransverseData ? "transverse/":""), run));
  ch->Add(Form("%s/%smps_only_%i_3*.root",gSystem->Getenv("MPS_ONLY_ROOTFILES"),
	       (isTransverseData ? "transverse/":""), run));
  ch->Add(Form("%s/%smps_only_%i_full.root",gSystem->Getenv("MPS_ONLY_ROOTFILES"),
	       (isTransverseData ? "transverse/":""), run));
  TFile *friendTree = new TFile(Form("%s/%smps_only_friend_%i.root",
				     gSystem->Getenv("MPS_ONLY_ROOTFILES"), 
				     (isTransverseData ? "transverse/":""),run));
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
  char * configFile = Form("%s/config/setup_mpsonly.config",
			   gSystem->Getenv("BMOD_SRC"));
  if(config != "")
    configFile = config;
  nDet = GetMonitorAndDetectorLists(ch, MonitorList, DetectorList,0, configFile);
  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return -1;
  }
  if(debug)std::cout<<"(4)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Open slopesfile for this run
  /////////////////////////////////////////////////////////
  n = 0;
  char *outputDir = Form("%s", gSystem->Getenv("BMOD_OUT"));
  if( output != "")
    outputDir = output;
  ifstream slopesFile(Form("%s/%sslopes/%sslopes_%i%s.%sset0.dat",
			   output, (run_average ? "" : "macrocycle_"),
			   (run_average ? "" : "macrocycle_"),
			   run, chisquare, (nonlin_cut ? "cut180deg_" : "")));
  if (slopesFile.is_open()){
    printf("Slopes file found for run %i.\n", run);
  }else{
    printf("%s/%sslopes/%sslopes_%i%s.%sset0.dat",
	   outputDir, (run_average ? "" : "macrocycle_"),
	   (run_average ? "" : "macrocycle_"),
	   run, chisquare,(nonlin_cut ? "cut180deg_" : ""));
    return -1;
  }
  if(debug)std::cout<<"(5)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Apply correction slopes to data and find remaining sensitivities
  //////////////////////////////////////////////////////////////////
  size = 9;
  FILE *sineData = fopen(Form("%s/%sslopes/run%iSineAmpl%s.%sset0.dat",
			      outputDir, (run_average ? "" : "macrocycle_"),
			      run, chisquare,(nonlin_cut ? "cut180deg_" : "")),
			 "w");
  FILE *cosineData = fopen(Form("%s/%sslopes/run%iCosineAmpl%s.%sset0.dat",
				outputDir, (run_average ? "" : "macrocycle_"),
				run, chisquare, (nonlin_cut ? "cut180deg_" : "")),
			   "w");
  if(sineData == NULL || cosineData == NULL){
    perror ("Error opening cosine and sine files. Exiting.\n");
    return -1;
  }
  n = 0; 
  while(!slopesFile.eof()){
    //1)Get the detector slopes from the slopes file.  
    /////////////////////////////////////////////////////////
    Header_t header;
    if(!run_average){
      ExtractHeader(slopesFile, header, size);
    }

    for(int i=0;i<nDet;i++){
      char  x[255] = {0}, xe[255] = {0};
      if(slopesFile.is_open()&&slopesFile.good()){
	getline(slopesFile, line);
	detector = TString("det ");
	detector += DetectorList[i].Data();
	if(line==detector.Data()){
	  std::cout<<line<<"\n";
	  Double_t prev_x = 0, prev_xe = 0;
	  for(int j=0;j<5;j++){
	    slopesFile>>x>>xe;
	    if(atof(x) == prev_x && atof(xe) == prev_xe)
	      cout<<"WARNING!!! FILE READ SUSPECT!!!\n\n\n";
	    Bool_t notANum = strcmp(xe,"nan")==0 || strcmp( x,"nan")==0;
	    DetectorSlope[j][i] = (notANum ? errorFlag : atof(x));
	    DetectorSlopeError[j][i] = (notANum ? errorFlag : atof(xe));
	    std::cout<<DetectorSlope[j][i]<<" +/- "<<DetectorSlopeError[j][i]
		     <<"\n";
	    getline(slopesFile, line);
	    prev_x = atof(x);
	    prev_x = atof(xe);
	  }
	}
      }
    }
    if(debug)std::cout<<"(6)Working.\n"; 
    /////////////////////////////////////////////////////////


    //2)Get mean monitor and detector yields for periods of interest.
    /////////////////////////////////////////////////////////
    Double_t monit[nMON], detect[nDET], pat_num;
    Int_t nEntries[nMOD] = {0,0,0,0,0};
    for(int imon=0;imon<nMON;imon++){
      ch->SetBranchAddress(Form("%s",MonitorList[imon].Data()),&monit[imon]);
      for(Int_t imod=0;imod<nMOD;imod++)
	meanMon[imod][imon] = 0;
    }
    for(int idet=0;idet<nDet;idet++){
      ch->SetBranchAddress(Form("%s",DetectorList[idet].Data()),&detect[idet]);
      for(Int_t imod=0;imod<nMOD;imod++)
	meanDet[imod][idet] = 0;
    }
    ch->SetBranchAddress("bm_pattern_number", &pat_num);
    for(Int_t ient = header.lEntry;ient<=header.hEntry;ient++){
      ch->GetEntry(ient);
      Int_t pattern = (pat_num<5 ? (int)pat_num : (int)pat_num-11);
      nEntries[pattern]++;
      for(int imon=0;imon<nMON;imon++){
	meanMon[pattern][imon] += monit[imon];
      }
      for(int idet=0;idet<nDet;idet++){
	meanDet[pattern][idet] += detect[idet];
      }
    }
    for(int imod=0;imod<nMOD;imod++)
      for(int imon=0;imon<nMON;imon++){
	meanMon[imod][imon] /= nEntries[imod];
	printf("Mean Monitor[%i][%i](Range:%i-%i) = %f\n", imod, imon,
	       header.lEntry, header.hEntry, meanMon[imod][imon]);
      }
    for(int imod=0;imod<nMOD;imod++)
      for(int idet=0;idet<nDet;idet++){
	meanDet[imod][idet] = TMath::Abs(meanDet[imod][idet]/nEntries[imod]);
	printf("Mean Detector[%i][%i] = %f\n",imod,idet,meanDet[imod][idet]);
      }

    
    if(debug)std::cout<<"(7)Working.\n"; 
    /////////////////////////////////////////////////////////


    //3)Apply the slopes to the detector data 
    //to see if correlations removed.
    /////////////////////////////////////////////////////////
    TGraphErrors gr;
    for(int imod=0;imod<nMOD;imod++){
      cut = Form("bm_pattern_number == %i||bm_pattern_number == %i",imod, 11+imod);
      if(!run_average)
	cut += Form("Entry$>=%i && Entry$<=%i", header.lEntry, header.hEntry);
      cut.Print();
      std::cout<<"Entries passing cut: "<<ch->GetEntries(cut)<<"\n";
      for(int idet=0;idet<nDet;idet++){
	Int_t allGood = 1;
	for(int imon=0;imon<nMOD;imon++){
	  if(DetectorSlope[imon][idet]== errorFlag || 
	     DetectorSlopeError[imon][idet]== errorFlag )allGood = 0;
	}

	if(allGood){
	  TF1 *f = new TF1("f",Form("[0]+[1]*sin(x*%f+%f)+[2]*cos(x*%f+%f)",
				    DegToRad, phase[imod], DegToRad, phase[imod]),
			   -10, 370);
	  f->SetParameter(0,1.0);
	  f->SetParameter(1,0.0);
	  f->SetParameter(2,0.0);
	  if(plotAsProfile){
	    ch->Draw(Form("%s/%e*(1-%e*(%s-%e)-%e*(%s-%e)-%e*(%s-%e)"
			  "-%e*(%s-%e)-%e*(%s-%e)):ramp_filled>>prC%i",
			  DetectorList[idet].Data(),meanDet[imod][idet],
			  DetectorSlope[0][idet],MonitorList[0].Data(),meanMon[imod][0],
			  DetectorSlope[1][idet],MonitorList[1].Data(),meanMon[imod][1],
			  DetectorSlope[2][idet],MonitorList[2].Data(),meanMon[imod][2],
			  DetectorSlope[3][idet],MonitorList[3].Data(),meanMon[imod][3],
			  DetectorSlope[4][idet],MonitorList[4].Data(),meanMon[imod][4],
			  n), cut, "prof");
	    TProfile* prC = (TProfile*)gDirectory->Get(Form("prC%i",n));
	    prC->Approximate(0);
	
	    Double_t max = prC->GetBinContent(0)+2*prC->GetBinError(0);
	    for(int imon=1; imon<prC->GetNbinsX();imon++){
	      Double_t val = prC->GetBinContent(imon)+2*prC->GetBinError(imon);
	      if(max<val&&prC->GetBinContent(imon)!=0){
		max = val;
	      }
	    }
	    Double_t min = 1e6; 
	    for(int imon=1; imon<prC->GetNbinsX();imon++){
	      Double_t val = prC->GetBinContent(imon)-2*prC->GetBinError(imon);
	      if(min>val&&prC->GetBinContent(imon)!=0){
		min = val;
	      }
	    }
	    vector<Double_t> vx, vxe, vy, vye;
	    
	    for(int ibin=1;ibin<prC->GetNbinsX();++ibin){
	      if(prC->GetBinEntries(ibin)>=minEntriesPerBin){
		vx.push_back(prC->GetBinCenter(ibin));
		vxe.push_back(prC->GetBinWidth(ibin)/2.0);
		vy.push_back(prC->GetBinContent(ibin));
		vye.push_back(prC->GetBinError(ibin));
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
	    Int_t size = (int)vx.size();
	    gr = TGraphErrors(size, vx.data(), vy.data(), 
					   vxe.data(), vye.data());
	    gr.Draw("ap");
	    //	    gr.GetYaxis()->SetRangeUser(min,max);
	    if(gr.Fit(f, "QRMEN")<0){
	      cout<<"Fit failure. Exiting.\n";
	      return -1;
	    }

	    f->SetLineColor(kMagenta);
	    TF1 *fcos = new TF1("fcos",Form("%e+%e*cos(x*%f+%f)",f->GetParameter(0),
					    f->GetParameter(2), DegToRad, 
					    phase[imod]),
				0, 360);
	    fcos->SetLineColor(kBlue);
	    TF1 *fsin = new TF1("fsin",Form("%e+%e*sin(x*%f+%f)",f->GetParameter(0),
					    f->GetParameter(1), DegToRad, 
					    phase[imod]), 0, 360);
	    fsin->SetLineColor(kRed);

	    CorrectedDetectorSin[imod][idet]= f->GetParameter(1);
	    CorrectedDetectorSinError[imod][idet]= f->GetParError(1);
	    CorrectedDetectorCos[imod][idet]= f->GetParameter(2);
	    CorrectedDetectorCosError[imod][idet]= f->GetParError(2);
	    if(pretty_plot){
	      gr.SetTitle(Form("Corrected %s ModType=%i(%s)", 
				    DetectorList[idet].Data(), imod, 
				    ModulationList[imod].Data()));
	      gr.SetTitle(Form("Corrected %s ModType=%i(%s)", 
			       DetectorList[idet].Data(), imod, 
			       ModulationList[imod].Data()));
	      gr.GetYaxis()->SetTitle(Form("Corrected %s",DetectorList[idet].Data()));
	      gr.GetXaxis()->SetTitle("ramp_filled");
	      gr.GetXaxis()->SetTitleSize(0.055);
	      gr.GetXaxis()->SetTitleOffset(0.8);
	      gr.Draw("ap");
	    }
	    fsin->Draw("same");
	    fcos->Draw("same");
	    f->Draw("same");
	    gPad->Update();
	  }else{
	    ch->Draw(Form("%s/%e*(1-%e*(%s-%e)-%e*(%s-%e)-%e*(%s-%e)"
			  "-%e*(%s-%e)-%e*(%s-%e)):ramp_filled>>hC%i",
			  DetectorList[idet].Data(),meanDet[imod][idet],
			  DetectorSlope[0][idet],MonitorList[0].Data(),meanMon[imod][0],
			  DetectorSlope[1][idet],MonitorList[1].Data(),meanMon[imod][1],
			  DetectorSlope[2][idet],MonitorList[2].Data(),meanMon[imod][2],
			  DetectorSlope[3][idet],MonitorList[3].Data(),meanMon[imod][3],
			  DetectorSlope[4][idet],MonitorList[4].Data(),meanMon[imod][4],
			  n), cut);

	    TH2D *hC = (TH2D*)gDirectory->Get(Form("hC%i",n));
	    hC->Fit(f,"QRMEN");
	    f->SetLineColor(kMagenta);
	    TF1 *fcos = new TF1("fcos",Form("%e+%e*cos(x*%f+%f)",f->GetParameter(0),
					    f->GetParameter(2), DegToRad, 
					    phase[imod]),
				0, 360);
	    fcos->SetLineColor(kBlue);
	    TF1 *fsin = new TF1("fsin",Form("%e+%e*sin(x*%f+%f)",f->GetParameter(0),
					    f->GetParameter(1), DegToRad, 
					    phase[imod]), 0, 360);
	    fsin->SetLineColor(kRed);

	    CorrectedDetectorSin[imod][idet]= f->GetParameter(1);
	    CorrectedDetectorSinError[imod][idet]= f->GetParError(1);
	    CorrectedDetectorCos[imod][idet]= f->GetParameter(2);
	    CorrectedDetectorCosError[imod][idet]= f->GetParError(2);
	    if(pretty_plot){
	      hC->SetTitle(Form("%s ModType=%i(%s)",
				   DetectorList[idet].Data(), imod,
				   ModulationList[imod].Data()));
	      hC->SetTitle(Form("Corrected %s ModType=%i(%s)",
				   DetectorList[idet].Data(),imod,
				   ModulationList[imod].Data()));
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
	  CorrectedDetectorSin[imod][idet]= errorFlag;
	  CorrectedDetectorSinError[imod][idet]=  errorFlag;
	  CorrectedDetectorCos[imod][idet]=  errorFlag;
	  CorrectedDetectorCosError[imod][idet]=  errorFlag;
	}
	n++;
      }
      scut.Clear();
    }
    /////////////////////////////////////////////////////////


    //4)Record data to file and print to screen.
    ////////////////////////////////////////////////////////
    printf("\nTable 1. Run %i detector slopes from mps_only code%s\n", run,
	   (char *)(chsq ? " using Chi Square minimization.":"."));

    printf("                |     %s       |   %s    |     %s      |"
	   "     %s       |   %s    |\n", MonitorList[0].Data(),  
	   MonitorList[1].Data(), MonitorList[2].Data(), MonitorList[3].Data(),
	   MonitorList[4].Data());
    printf("*********************************************************************"
	   "*******************************************************************"
	   "*\n");

    for(Int_t i=0;i<nDet;i++){
      TString detec = DetectorList[i];
      detec.Resize(16);
      printf("%s%s|", ANSI_COLOR_RESET,detec.Data());
      for(Int_t j=0;j<nMOD;j++){
	Int_t good = 0;
	if(TMath::Abs(DetectorSlope[j][i])<TMath::Abs(DetectorSlopeError[j][i]))
	  good = 1;
	printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	       DetectorSlope[j][i]);
	printf("%s+/-%6.2e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	       DetectorSlopeError[j][i],ANSI_COLOR_RESET);
      }
      printf("\n");
    }
    printf("\n\n");




    printf("Table 2. Run %i Corrected detector residual Sine component from profile fit%s\n", 
	   run, (char *)(chsq ? " using Chi Square minimization.":"."));

    printf("                |         Xmod          |         Ymod          |        Emod"
	   "           |        XPmod          |        YPmod          |\n"
	   "***************************************************************************"
	   "**************************************************************\n");
    if(!run_average){
      fprintf(sineData,"Macrocycle %i  ",header.macrocycle);
      for(int imod = 0; imod<nMOD;imod++)
	fprintf(sineData,"Mod%i %i  ",imod, header.microcycle[imod]);
      fprintf(sineData,"Id %i  ",header.id);
      fprintf(sineData,"first_entry %i  ",header.lEntry);
      fprintf(sineData,"last_entry %i\n",header.hEntry);
    }
    for(Int_t idet=0;idet<nDet;idet++){
      TString detec = DetectorList[idet];
      detec.Resize(16);
      printf("%s%s|", ANSI_COLOR_RESET,detec.Data());
      fprintf(sineData, "%s\t", DetectorList[idet].Data());
      for(Int_t imod=0;imod<nMOD;imod++){
	Int_t good = 0;
	if(TMath::Abs(CorrectedDetectorSin[imod][idet])<
	   TMath::Abs(CorrectedDetectorSinError[imod][idet]))
	  good = 1;
	printf("%s %+8.2e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	       CorrectedDetectorSin[imod][idet]);
	printf("%s +/- %6.1e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	       CorrectedDetectorSinError[imod][idet],ANSI_COLOR_RESET);
	fprintf(sineData, "%+8.3e  %7.3e\t",CorrectedDetectorSin[imod][idet],
		CorrectedDetectorSinError[imod][idet]);
      }
      printf("\n");
      fprintf(sineData, "\n");
    }
    printf("\n\n");



    printf("Table 3. Run %i Corrected detector residual Cosine component from profile fit%s\n",
	   run, (char *)(chsq ? " using Chi Square minimization.":"."));

    printf("                |         Xmod          |         Ymod          |        Emod"
	   "           |        XPmod          |        YPmod          |\n"
	   "***************************************************************************"
	   "**************************************************************\n");
    if(!run_average){
      fprintf(cosineData,"Macrocycle %i  ",header.macrocycle);
      for(int imod = 0; imod<nMOD;imod++)
	fprintf(cosineData,"Mod%i %i  ",imod, header.microcycle[imod]);
      fprintf(cosineData,"Id %i  ",header.id);
      fprintf(cosineData,"first_entry %i  ",header.lEntry);
      fprintf(cosineData,"last_entry %i\n",header.hEntry);
    }
    for(Int_t idet=0;idet<nDet;idet++){
      TString detec = DetectorList[idet];
      detec.Resize(16); 
      printf("%s%s|", ANSI_COLOR_RESET,detec.Data());
      fprintf(cosineData, "%s\t", DetectorList[idet].Data());
      for(Int_t imod=0;imod<nMOD;imod++){
	Int_t good = 0;
	if(TMath::Abs(CorrectedDetectorCos[imod][idet])<
	   TMath::Abs(CorrectedDetectorCosError[imod][idet]))
	  good = 1;
	printf("%s %+8.2e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	       CorrectedDetectorCos[imod][idet]);
	printf("%s +/- %6.1e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	       CorrectedDetectorCosError[imod][idet],ANSI_COLOR_RESET);
	fprintf(cosineData, "%+8.3e  %7.3e\t",CorrectedDetectorCos[imod][idet],
		CorrectedDetectorCosError[imod][idet]);
      }
      printf("\n");
      fprintf(cosineData, "\n");
    }
    printf("\n\n");
    ////////////////////////////////////////////////////////

    slopesFile.peek();
  }
  //////////////////////////////////////////////////////////////////
  cout<<"Saving residual files.\n";
  fclose(sineData);
  fclose(cosineData);

  return 0;
}

///////////////////////////////////////////////////////////
//To compile at the command line                         //
//g++ -Wall `root-config --ldflags --libs --cflags` -O0  //
//findBodResiduals.C -o findBModResiduals               //
///////////////////////////////////////////////////////////
Int_t main(int argc, char *argv[]){
  switch (argc)
    {
    case 0:
      findBModResiduals();
      break;
    case 1:
      findBModResiduals();
      break;
    case 2:
      findBModResiduals(atoi(argv[1]));
      break;
    case 3:
      findBModResiduals(atoi(argv[1]), argv[2]);
      break;
    case 4:
      findBModResiduals(atoi(argv[1]), argv[2], argv[3]);
      break;
    case 5:
      findBModResiduals(atoi(argv[1]), argv[2], argv[3], atoi(argv[4]));
      break;
    case 6:
      findBModResiduals(atoi(argv[1]), argv[2], argv[3], atoi(argv[4]), atoi(argv[5]));
      break;
    case 7:
      findBModResiduals(atoi(argv[1]), argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
      break;
    case 8:
      findBModResiduals(atoi(argv[1]), argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), (bool)atoi(argv[7]));
    case 9:
      findBModResiduals(atoi(argv[1]), argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), (bool)atoi(argv[7]), (bool)atoi(argv[8]));
    case 10:
      findBModResiduals(atoi(argv[1]), argv[2], argv[3], atoi(argv[4]), atoi(argv[5]), atoi(argv[6]), (bool)atoi(argv[7]), (bool)atoi(argv[8]), (bool)atoi(argv[9]));
    default:
      std::cout<<"Usage:findBModResiduals(Int_t run, Int_t plotAsProfile , Int_t chsq, Int_t phase_set = 0,"<<
	" Bool_t run_average = 0, Bool_t nonlin_cut = 0)\n";
      break;
    }
  return 0;
}








