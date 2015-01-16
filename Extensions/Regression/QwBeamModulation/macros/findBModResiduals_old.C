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
#include "TStyle.h"
#include "TROOT.h"
#include "TDirectory.h"
#include "TPaveText.h"
#include "TPad.h"
#include "TFile.h"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"


//Declare constants
/////////////////////////////////////////////////////////
const Int_t nDET = 32, nMOD = 5;
const Double_t  errorFlag = -999999, DegToRad = 0.0174532925199432955;
/////////////////////////////////////////////////////////

Int_t GetMonitorAndDetectorLists(TString *monitorList, TString *detectorList, 
				 Bool_t trunc){
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


Int_t findBModResiduals_old(Int_t run = 13993, Int_t plotAsProfile = 1, 
			Int_t chsq = 1, Int_t phase_set = 0){
  gStyle->SetOptFit(1111);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.25);
  gStyle->SetOptStat(0);
  Bool_t debug = 0;


  //Declare needed variables
  /////////////////////////////////////////////////////////
  char  x[255], xe[255];
  char *chisquare = "";
  string line;
  Int_t nDet = 0, n = 0;
  Double_t phase[nMOD];  
  Double_t meanX[nMOD][nMOD], meanY[nMOD][nDET];
  //Slopes read from output of mps only code
  Double_t AvDetectorSlope[nMOD][nDET], AvDetectorSlopeError[nMOD][nDET];
  //Slopes determined from profile fits to data
  Double_t AvCorrectedDetectorSin[nMOD][nDET],
    AvCorrectedDetectorSinError[nMOD][nDET];
  Double_t AvCorrectedDetectorCos[nMOD][nDET], 
    AvCorrectedDetectorCosError[nMOD][nDET];
  FILE *sineData, *cosineData;
  TString ModulationList[nMOD] = {"x", "y", "e", "xp", "yp"};
  TString MonitorList[nMOD], DetectorList[nDET], detector;
  TString scut = TString(1000);
  TProfile *prC[200];
  TH2D *hC[200];
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
  nDet = GetMonitorAndDetectorLists(MonitorList, DetectorList,0);
  if(nDet == -1){
    cout<<"Detector list not found. Exiting.\n"<<endl;
    return -1;
  }
  if(debug)std::cout<<"(4)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Open slopesfile for this run
  /////////////////////////////////////////////////////////
  n = 0;
  ifstream slopesFile(Form("%s/slopes/slopes_%i%s.set0.dat",
			   gSystem->Getenv("BMOD_OUT"), 
			   run, chisquare));
  if (slopesFile.is_open()){
    printf("Slopes file found for run %i.\n", run);
  }else{
    printf("%s/slopes/slopes_%i%s.set0.dat",
	   gSystem->Getenv("BMOD_OUT"), run, chisquare);
    return -1;
  }
  if(debug)std::cout<<"(5)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Get the detector slopes from the slopes file.  
  /////////////////////////////////////////////////////////
  for(int i=0;i<nDet;i++){
    if(slopesFile.is_open()&&slopesFile.good()){
      getline(slopesFile, line);
      detector = TString("det ");
      detector += DetectorList[i].Data();
      if(line==detector.Data()){
	std::cout<<line<<"\n";
	for(int j=0;j<5;j++){
	  slopesFile>>x>>xe;
	  Bool_t notANum = strcmp(xe,"nan")==0 || strcmp( x,"nan")==0;
	  AvDetectorSlope[j][i] = (notANum ? errorFlag : atof(x));
	  AvDetectorSlopeError[j][i] = (notANum ? errorFlag : atof(xe));
	  std::cout<<AvDetectorSlope[j][i]<<" +/- "<<AvDetectorSlopeError[j][i]
		   <<"\n";
	  getline(slopesFile, line);
	}
      }
    }
  }
  if(debug)std::cout<<"(6)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Get mean detector yields for periods of interest.
  /////////////////////////////////////////////////////////
  n = 0;
  for(int i=0;i<nMOD;i++){
    cut = Form("bm_pattern_number == %i||bm_pattern_number == %i",i,11+i);
    for(int j=0;j<nMOD;j++){
      ch->Draw(Form("%s>>hX%i(100)",MonitorList[j].Data(),n),cut,"goff");
      TH1D *hX = (TH1D*)gDirectory->Get(Form("hX%i",n));
      meanX[i][j] = hX->GetMean();
      std::cout<<"meanX["<<i<<"]["<<j<<"]="<<meanX[i][j]<<"\n";
      n++;
    }
  }
  if(debug)std::cout<<"(7)Working.\n"; 
  /////////////////////////////////////////////////////////


  //Apply the slopes to the detector data 
  //to see if correlations removed.
  /////////////////////////////////////////////////////////
  n = 0;
  for(int i=0;i<nMOD;i++){
    scut = Form("bm_pattern_number==%i||bm_pattern_number==%i",i,11+i);
    cut = scut;
    cut.Print();
    std::cout<<"Entries passing cut: "<<ch->GetEntries(cut)<<"\n";

    for(int j=0;j<nDet;j++){
 
      Int_t allGood = 1;
      for(int k=0;k<nMOD;k++){
	if(AvDetectorSlope[k][j]== errorFlag || 
	   AvDetectorSlopeError[k][j]== errorFlag )allGood = 0;
      }

      if(allGood){
	TF1 *f = new TF1("f",Form("[0]+[1]*sin(x*%f+%f)+[2]*cos(x*%f+%f)",
				  DegToRad, phase[i], DegToRad, phase[i]),
			 -10, 370);
	prC[n] = new TProfile(Form("pr%i",j+(i*nDet)),
			      Form("pr%i",j+(i*nDet)),100,-1.1,1.1);

	ch->Draw(Form("%s>>h%i(100)",DetectorList[j].Data(),j),cut,"goff");
	TH1D *h = (TH1D*)gDirectory->Get(Form("h%i",j));
      
	meanY[i][j] = TMath::Abs(h->GetMean());
 
	if(plotAsProfile){
	  ch->Draw(Form("%s/%e*(1-%e*(%s-%e)-%e*(%s-%e)-%e*(%s-%e)"
			"-%e*(%s-%e)-%e*(%s-%e)):ramp_filled>>prC[%i]",
			DetectorList[j].Data(),meanY[i][j],AvDetectorSlope[0][j],
			MonitorList[0].Data(),meanX[i][0],AvDetectorSlope[1][j],
			MonitorList[1].Data(),meanX[i][1],AvDetectorSlope[2][j],
			MonitorList[2].Data(),meanX[i][2],AvDetectorSlope[3][j],
			MonitorList[3].Data(),meanX[i][3],AvDetectorSlope[4][j],
			MonitorList[4].Data(),meanX[i][4], n), 
		   cut, "prof");
	  prC[n] = (TProfile*)gDirectory->Get(Form("prC[%i]",n));
	  prC[n]->Approximate(1);
	
	  Double_t max = prC[n]->GetBinContent(0)+2*prC[n]->GetBinError(0);
	  for(int k=1; k<prC[n]->GetNbinsX();k++){
	    Double_t val = prC[n]->GetBinContent(k)+2*prC[n]->GetBinError(k);
	    if(max<val&&prC[n]->GetBinContent(k)!=0){
	      max = val;
	    }
	  }
	  Double_t min = 1e6; 
	  for(int k=1; k<prC[n]->GetNbinsX();k++){
	    Double_t val = prC[n]->GetBinContent(k)-2*prC[n]->GetBinError(k);
	    if(min>val&&prC[n]->GetBinContent(k)!=0){
	      min = val;
	    }
	  }
	  prC[n]->SetTitle(Form("Corrected %s ModType=%i(%s)", 
				DetectorList[j].Data(), i, ModulationList[i].Data()));
	  prC[n]->Fit(f);
	  f->SetLineColor(kMagenta);
	  TF1 *fcos = new TF1("fcos",Form("%e+%e*cos(x*%f+%f)",f->GetParameter(0),
					  f->GetParameter(2), DegToRad, phase[i]),
			      0, 360);
	  fcos->SetLineColor(kBlue);
	  TF1 *fsin = new TF1("fsin",Form("%e+%e*sin(x*%f+%f)",f->GetParameter(0),
					  f->GetParameter(1), DegToRad, phase[i]),
			      0, 360);
	  fsin->SetLineColor(kRed);

	  AvCorrectedDetectorSin[i][j]= f->GetParameter(1);
	  AvCorrectedDetectorSinError[i][j]= f->GetParError(1);
	  AvCorrectedDetectorCos[i][j]= f->GetParameter(2);
	  AvCorrectedDetectorCosError[i][j]= f->GetParError(2);
	  prC[n]->SetTitle(Form("Corrected %s ModType=%i(%s)", 
				DetectorList[j].Data(), i, ModulationList[i].Data()));
	  prC[n]->GetYaxis()->SetRangeUser(min,max);
	  prC[n]->GetYaxis()->SetTitle(Form("Corrected %s",DetectorList[j].Data()));
	  prC[n]->GetXaxis()->SetTitle("ramp_filled");
	  prC[n]->GetXaxis()->SetTitleSize(0.055);
	  prC[n]->GetXaxis()->SetTitleOffset(0.8);
	  prC[n]->Draw();
	  fsin->Draw("same");
	  fcos->Draw("same");
	  f->Draw("same");
	}else{
	  ch->Draw(Form("%s/%e*(1-%e*(%s-%e)-%e*(%s-%e)-%e*(%s-%e)"
			"-%e*(%s-%e)-%e*(%s-%e)):ramp_filled>>hC[%i]",
			DetectorList[j].Data(),meanY[i][j],AvDetectorSlope[0][j],
			MonitorList[0].Data(),meanX[i][0],AvDetectorSlope[1][j],
			MonitorList[1].Data(),meanX[i][1],AvDetectorSlope[2][j],
			MonitorList[2].Data(),meanX[i][2],AvDetectorSlope[3][j],
			MonitorList[3].Data(),meanX[i][3],AvDetectorSlope[4][j],
			MonitorList[4].Data(),meanX[i][4], n), cut);

	  hC[n] = (TH2D*)gDirectory->Get(Form("hC[%i]",n));
	  hC[n]->SetTitle(Form("%s ModType=%i(%s)",
			       DetectorList[j].Data(), i,
			       ModulationList[i].Data()));
	  hC[n]->Fit(f);
	  f->SetLineColor(kMagenta);
	  TF1 *fcos = new TF1("fcos",Form("%e+%e*cos(x*%f+%f)",f->GetParameter(0),
					  f->GetParameter(2), DegToRad, phase[i]),
			      0, 360);
	  fcos->SetLineColor(kBlue);
	  TF1 *fsin = new TF1("fsin",Form("%e+%e*sin(x*%f+%f)",f->GetParameter(0),
					  f->GetParameter(1), DegToRad, phase[i]),
			      0, 360);
	  fsin->SetLineColor(kRed);

	  AvCorrectedDetectorSin[i][j]= f->GetParameter(1);
	  AvCorrectedDetectorSinError[i][j]= f->GetParError(1);
	  AvCorrectedDetectorCos[i][j]= f->GetParameter(2);
	  AvCorrectedDetectorCosError[i][j]= f->GetParError(2);
	  hC[n]->SetTitle(Form("Corrected %s ModType=%i(%s)",
			       DetectorList[j].Data(),i,ModulationList[i].Data()));
	  hC[n]->GetYaxis()->SetTitle(Form("Corrected %s",DetectorList[j].Data()));
	  hC[n]->GetXaxis()->SetTitle("ramp_filled");
	  hC[n]->GetXaxis()->SetTitleSize(0.055);
	  hC[n]->GetXaxis()->SetTitleOffset(0.8);
	  hC[n]->Draw();
	  fsin->Draw("same");
	  fcos->Draw("same");
	  f->Draw("same");
	}
	gPad->Update();
      }else{
	AvCorrectedDetectorSin[i][j]= errorFlag;
	AvCorrectedDetectorSinError[i][j]=  errorFlag;
	AvCorrectedDetectorCos[i][j]=  errorFlag;
	AvCorrectedDetectorCosError[i][j]=  errorFlag;
      }
      n++;
    }
    scut.Clear();
  }

  sineData = fopen(Form("%s/slopes/run%iSineAmpl%s.set0.dat",
			gSystem->Getenv("BMOD_OUT"),run,chisquare), "w");
  cosineData= fopen(Form("%s/slopes/run%iCosineAmpl%s.set0.dat",
			 gSystem->Getenv("BMOD_OUT"),run,chisquare), "w");


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
      if(TMath::Abs(AvDetectorSlope[j][i])<TMath::Abs(AvDetectorSlopeError[j][i]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     AvDetectorSlope[j][i]);
      printf("%s+/-%6.2e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     AvDetectorSlopeError[j][i],ANSI_COLOR_RESET);
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

  for(Int_t i=0;i<nDet;i++){
    TString detec = DetectorList[i];
    detec.Resize(16);
    printf("%s%s|", ANSI_COLOR_RESET,detec.Data());
    for(Int_t j=0;j<nMOD;j++){
      Int_t good = 0;
      if(TMath::Abs(AvCorrectedDetectorSin[j][i])<
	 TMath::Abs(AvCorrectedDetectorSinError[j][i]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     AvCorrectedDetectorSin[j][i]);
      printf("%s+/-%6.2e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     AvCorrectedDetectorSinError[j][i],ANSI_COLOR_RESET);
      fprintf(sineData, "%+8.3e\t",AvCorrectedDetectorSin[j][i]);
      fprintf(sineData, "%7.3e\t",AvCorrectedDetectorSinError[j][i]);
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

  for(Int_t i=0;i<nDet;i++){
    TString detec = DetectorList[i];
    detec.Resize(16); 
    printf("%s%s|", ANSI_COLOR_RESET,detec.Data());
    for(Int_t j=0;j<nMOD;j++){
      Int_t good = 0;
      if(TMath::Abs(AvCorrectedDetectorCos[j][i])<
	 TMath::Abs(AvCorrectedDetectorCosError[j][i]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     AvCorrectedDetectorCos[j][i]);
      printf("%s+/-%6.2e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     AvCorrectedDetectorCosError[j][i],ANSI_COLOR_RESET);
      fprintf(cosineData, "%+8.3e\t",AvCorrectedDetectorCos[j][i]);
      fprintf(cosineData, "%7.3e\t",AvCorrectedDetectorCosError[j][i]);
    }
    printf("\n");
    fprintf(cosineData, "\n");
  }
  printf("\n\n");

 
  fclose(sineData);
  fclose(cosineData);

  return 0;
}











