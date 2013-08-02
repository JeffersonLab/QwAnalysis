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


Int_t checkInvertedSlopes(Int_t run = 13993, Bool_t plotAsProfile = 1){
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

  char  x[255], xe[255], mon[255], monitor[255];
  string line;

  gStyle->SetOptFit(1111);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.25);
  gStyle->SetOptStat(0);

  const Int_t nDET = 9, nMOD = 5;
  Double_t DegToRad = 0.0174532925199432955;
  Double_t phase[nMOD] = {0.26,1.08,0,0.26,1.08};  
  Double_t meanX[nMOD][nMOD], meanY[nMOD][nDET];
  Int_t order[18] = {8,7,1,2,3,9,15,14,13,11,10,4,5,6,12,18,17,16};
  TString ModulationList[nMOD] = {"Xmod", "Ymod", "Emod", "XPmod", "YPmod"};
  TString MonitorList[nMOD] ;

  TString DetectorList[nDET] = {"qwk_mdallbars", "qwk_md1barsum", 
				"qwk_md2barsum", "qwk_md3barsum", 
				"qwk_md4barsum", "qwk_md5barsum",
				"qwk_md6barsum", "qwk_md7barsum", 
				"qwk_md8barsum"};//, "qwk_md9barsum",
// 				"qwk_dslumi1", "qwk_dslumi2",
// 				"qwk_dslumi3", "qwk_dslumi4", 
// 				"qwk_dslumi5", "qwk_dslumi6", 
// 				"qwk_dslumi7","qwk_dslumi8",
// 				"uslumi_sum","uslumi1_sum",
// 				"uslumi3_sum", "uslumi5_sum",
// 				"uslumi7_sum","qwk_pmtltg",
// 				"qwk_pmtonl","qwk_bcm1","qwk_bcm2",
// 				"qwk_bcm5","qwk_bcm6"};

  ifstream file(Form("%s/config/setup_mpsonly.config",gSystem->Getenv("BMOD_SRC")));
  for(int i=0;i<5;i++){
    file>>mon>>monitor;
    getline(file, line);
    MonitorList[i] = TString(monitor);
    cout<<MonitorList[i]<<endl;
  }

  TString detector;

  //Slopes read from output of mps only code
  Double_t AvDetectorSlope[nMOD][nDET], AvDetectorSlopeError[nMOD][nDET];

  //Slopes determined from profile fits to data
  Double_t AvUncorrectedDetectorSin[nMOD][nDET], AvUncorrectedDetectorSinError[nMOD][nDET];
  Double_t AvUncorrectedDetectorCos[nMOD][nDET], AvUncorrectedDetectorCosError[nMOD][nDET];
  Double_t AvCorrectedDetectorSin[nMOD][nDET], AvCorrectedDetectorSinError[nMOD][nDET];
  Double_t AvCorrectedDetectorCos[nMOD][nDET], AvCorrectedDetectorCosError[nMOD][nDET];


  TChain *ch = new TChain("mps_slug");
  ch->Add(Form("mps_only_%i*.root",run));
  TFile *friendTree = new TFile(Form("%s/mps_only_friend_%i.root",
				     gSystem->Getenv("MPS_ONLY_ROOTFILES"), run));
  if(friendTree->IsOpen()){
    ch->AddFriend("mps_slug", friendTree);
  }else{
    cout<<"Friend file not found exiting program.\n";
    return 1;
  }
  cout<<"Run "<<run<<" \"by hand\""<<endl;
  TString scut = TString(1000);
  TCut cut;
  int n = 0;
  ifstream slopesFile(Form("%s/slopes/slopes_%i.set4.dat",
			   gSystem->Getenv("BMOD_OUT"), run));

  if (slopesFile.is_open())cout<<"File found.\n";
  else{
    cout<<slopesFile<<" not found. Exiting.\n";
    return -1;
  }

  for(int i=0;i<nDET;i++){
    if(slopesFile.is_open()&&slopesFile.good()){
      getline(slopesFile, line);
      detector = TString("det ");
      detector += DetectorList[i].Data();
      if(line==detector.Data()){
	cout<<line<<"\n";
	for(int j=0;j<5;j++){
	  slopesFile>>x>>xe;
	  AvDetectorSlope[j][i] = atof(x);
	  AvDetectorSlopeError[j][i] = atof(xe);
	  cout<<AvDetectorSlope[j][i]<<" +/- "<<AvDetectorSlopeError[j][i]<<"\n";
	  getline(slopesFile, line);
	}
      }
    }
  }

 
  //  First show uncorrected data.

  TPaveText *pt;
  pt = new TPaveText(0.2,0.1,0.8,0.3);
  pt->AddText(Form("Run %i",run));
  pt->SetFillColor(-1);
  pt->SetShadowColor(0);
  pt->SetBorderSize(0);
  pt->SetTextColor(kRed);

  TCanvas *cDet[nMOD]; 

  TProfile *pr[nDET*nMOD];

  TH2D *hist[nDET*nMOD];
  n = 0;
  for(int i=0;i<nMOD;i++){
    scut = Form("bm_pattern_number==%i||bm_pattern_number==%i",i,11+i);
    cut = scut;
    cut.Print();
    cout<<"Entries passing cut: "<<ch->GetEntries(cut)<<"\n";
    cDet[i] = new TCanvas(Form("cDet%i",i),Form("cDet%i",i),0,0,1900,1000);
    cDet[i]->Divide(6,3);
    for(int j=0;j<nDET;j++){
      TF1 *f = new TF1("f",Form("[0]+[1]*sin(x*%f+%f)+[2]*cos(x*%f+%f)",DegToRad,
				phase[i], DegToRad, phase[i]),-10, 370);

      cDet[i]->cd(order[j]);
      if(j==0)pt->Draw();
      gPad->Update();
      ch->Draw(Form("%s>>h%i(100)",DetectorList[j].Data(),j),cut,"goff");
      TH1D *h = (TH1D*)gDirectory->Get(Form("h%i",j));
      meanY[i][j] = TMath::Abs(h->GetMean());
      if(plotAsProfile){
	ch->Draw(Form("%s/%e:ramp_filled>>pr[%i]",DetectorList[j].Data(),
		      meanY[i][j],n),cut,"prof");
	pr[n] = (TProfile*)gDirectory->Get(Form("pr[%i]",n));
	pr[n]->Approximate(1);

	Double_t max = pr[n]->GetBinContent(0)+2*pr[n]->GetBinError(0);
	for(int k=1; k<pr[n]->GetNbinsX();k++){
	  Double_t val = pr[n]->GetBinContent(k)+2*pr[n]->GetBinError(k);
	  if(max<val&&pr[n]->GetBinContent(k)!=0){
	    max = val;
	  }
	}
	Double_t min = 1e6; 
	for(int k=1; k<pr[n]->GetNbinsX();k++){
	  Double_t val = pr[n]->GetBinContent(k)-2*pr[n]->GetBinError(k);
	  if(min>val&&pr[n]->GetBinContent(k)!=0){
	    min = val;
	  }
	}
	pr[n]->SetTitle(Form("%s ModType=%i(%s)",DetectorList[j].Data(),i,
			     ModulationList[i].Data()));

	pr[n]->GetYaxis()->SetRangeUser(min,max);
	pr[n]->GetYaxis()->SetTitle(Form("Normalized %s",DetectorList[j].Data()));
	pr[n]->GetXaxis()->SetTitle("ramp_filled");
	pr[n]->GetXaxis()->SetTitleSize(0.055);
	pr[n]->GetXaxis()->SetTitleOffset(0.8);
	pr[n]->Fit(f);
	f->SetLineColor(kMagenta);
	TF1 *fcos = new TF1("fcos",Form("%e+%e*cos(x*%f+%f)",f->GetParameter(0),
					f->GetParameter(2), DegToRad, phase[i]),
			    0, 360);
	fcos->SetLineColor(kBlue);
        TF1 *fsin = new TF1("fsin",Form("%e+%e*sin(x*%f+%f)",f->GetParameter(0),
					f->GetParameter(1), DegToRad, phase[i]),
			    0, 360);
	fsin->SetLineColor(kRed);
	AvUncorrectedDetectorSin[i][j]= f->GetParameter(1);
	AvUncorrectedDetectorSinError[i][j]= f->GetParError(1);
	AvUncorrectedDetectorCos[i][j]= f->GetParameter(2);
	AvUncorrectedDetectorCosError[i][j]= f->GetParError(2);
	pr[n]->Draw();
	fsin->Draw("same");
	fcos->Draw("same");
	f->Draw("same");
 
      }else{
	ch->Draw(Form("%s/%e:ramp_filled>>hist[%i]",DetectorList[j].Data(),
		      meanY[i][j],n),cut);
	hist[n] = (TH2D*)gDirectory->Get(Form("hist[%i]",n));
	hist[n]->SetTitle(Form("%s ModType=%i(%s)",DetectorList[j].Data(),i,
			     ModulationList[i].Data()));
	hist[n]->GetYaxis()->SetTitle(Form("Normalized %s",
					   DetectorList[j].Data()));
	hist[n]->GetXaxis()->SetTitle("ramp_filled");
	hist[n]->GetXaxis()->SetTitleSize(0.055);
	hist[n]->GetXaxis()->SetTitleOffset(0.8);
	hist[n]->Fit(f);
	f->SetLineColor(kMagenta);
	TF1 *fcos = new TF1("fcos",Form("%e+%e*cos(x*%f+%f)",f->GetParameter(0),
					f->GetParameter(2), DegToRad, phase[i]),
			    0, 360);
	fcos->SetLineColor(kBlue);
        TF1 *fsin = new TF1("fsin",Form("%e+%e*sin(x*%f+%f)",f->GetParameter(0),
					f->GetParameter(1), DegToRad, phase[i]),
			    0, 360);
	fsin->SetLineColor(kRed);
	AvUncorrectedDetectorSin[i][j]= f->GetParameter(1);
	AvUncorrectedDetectorSinError[i][j]= f->GetParError(1);
	AvUncorrectedDetectorCos[i][j]= f->GetParameter(2);
	AvUncorrectedDetectorCosError[i][j]= f->GetParError(2);
	hist[n]->Draw();
	fsin->Draw("same");
	fcos->Draw("same");
	f->Draw("same");
      }
      if(j==0)pt->Draw();
      gPad->Update();
      n++;
    }
    scut.Clear();
  }
  


//   //////////////////////////////////////////////////////////////////////////
//   //Now apply the slopes to the detector data to see if correlations removed.
  
  
  TProfile *prC[150];

  TH2D *hC[150];
  n = 0;
  for(int i=0;i<nMOD;i++){
    cut = Form("bm_pattern_number == %i||bm_pattern_number == %i",i,11+i);
    for(int j=0;j<nMOD;j++){
      ch->Draw(Form("%s>>hX%i(100)",MonitorList[j].Data(),n),cut,"goff");
      TH1D *hX = (TH1D*)gDirectory->Get(Form("hX%i",n));
      meanX[i][j] = hX->GetMean();
      cout<<"meanX["<<i<<"]["<<j<<"]="<<meanX[i][j]<<"\n";
      n++;
    }
  }

  n = 0;
  for(int i=0;i<nMOD;i++){
    scut = Form("bm_pattern_number==%i||bm_pattern_number==%i",i,11+i);
    cut = scut;
    cut.Print();
    cout<<"Entries passing cut: "<<ch->GetEntries(cut)<<"\n";

 
    for(int j=0;j<nDET;j++){
      TF1 *f = new TF1("f",Form("[0]+[1]*sin(x*%f+%f)+[2]*cos(x*%f+%f)",DegToRad,
				phase[i], DegToRad, phase[i]),-10, 370);
      prC[n] = new TProfile(Form("pr%i",j+(i*nDET)),
			    Form("pr%i",j+(i*nDET)),100,-1.1,1.1);
      (cDet[i]->cd(order[j+9]))->SetFillColor(20);
      //      cDetCorrectedModType[i]->cd(j+1);
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
			     DetectorList[j].Data(),i,ModulationList[i].Data()));
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
      n++;
    }
    scut.Clear();
  }

  FILE *sineData, *cosineData;
  sineData = fopen(Form("%s/slopes/run%iSineAmpl.dat",
			gSystem->Getenv("BMOD_OUT"),run), "w");
  cosineData= fopen(Form("%s/slopes/run%iCosineAmpl.dat",
			 gSystem->Getenv("BMOD_OUT"),run), "w");


  printf("\nTable 1. Run %i detector slopes from mps_only code.\n", run);
  printf("                 |      %s       |   %s     |      %s      |"
	 "      %s       |   %s     |\n", MonitorList[0].Data(),  
	 MonitorList[1].Data(), MonitorList[2].Data(), MonitorList[3].Data(),
	 MonitorList[4].Data());
  printf("***********************************************************************"
	 "***********************************************************************"
	 "*\n");

  for(Int_t i=0;i<nDET;i++){
    TString det = DetectorList[i];
    det.Resize(16);
    printf("%s%s |", ANSI_COLOR_RESET,det.Data());
    for(Int_t j=0;j<nMOD;j++){
      Int_t good = 0;
      if(TMath::Abs(AvDetectorSlope[j][i])<TMath::Abs(AvDetectorSlopeError[j][i]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     AvDetectorSlope[j][i]);
      printf("%s+/-%7.3e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     AvDetectorSlopeError[j][i],ANSI_COLOR_RESET);
    }
    printf("\n");
  }
  printf("\n\n");



  printf("Table 2. Run %i Uncorrected detector Sine component from profile fit.\n"
	 "                 |          Xmod          |           Ymod         |"
	 "          Emod          |"
	 "          XPmod         |          YPmod         |\n", run);
  printf("************************************************************************"
	 "***********************************************************************\n");

  for(Int_t i=0;i<nDET;i++){
    TString det = DetectorList[i];
    det.Resize(16);
    printf("%s%s |", ANSI_COLOR_RESET,det.Data());
    for(Int_t j=0;j<nMOD;j++){
      Int_t good = 0;
      if(TMath::Abs(AvUncorrectedDetectorSin[j][i])<
	 TMath::Abs(AvUncorrectedDetectorSinError[j][i]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     AvUncorrectedDetectorSin[j][i]);
      printf("%s+/-%7.3e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     AvUncorrectedDetectorSinError[j][i],ANSI_COLOR_RESET);
    }
    printf("\n");
  }
  printf("\n\n");

  printf("Table 3. Run %i Corrected detector residual Sine component from profile fit.\n"
	 "                 |          Xmod          |           Ymod         |"
	 "          Emod          |"
	 "         XPmod          |         YPmod          |\n", run);
  printf("************************************************************************"
	 "***********************************************************************\n");

  for(Int_t i=0;i<nDET;i++){
    TString det = DetectorList[i];
    det.Resize(16);
    printf("%s%s |", ANSI_COLOR_RESET,det.Data());
    for(Int_t j=0;j<nMOD;j++){
      Int_t good = 0;
      if(TMath::Abs(AvCorrectedDetectorSin[j][i])<
	 TMath::Abs(AvCorrectedDetectorSinError[j][i]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     AvCorrectedDetectorSin[j][i]);
      printf("%s+/-%7.3e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     AvCorrectedDetectorSinError[j][i],ANSI_COLOR_RESET);
      fprintf(sineData, "%+8.3e\t",AvCorrectedDetectorSin[j][i]);
      fprintf(sineData, "%7.3e\t",AvCorrectedDetectorSinError[j][i]);
    }
    printf("\n");
    fprintf(sineData, "\n");
  }
  printf("\n\n");


  printf("Table 4. Run %i Uncorrected detector Cosine component from profile fit.\n"
	 "                 |          Xmod          |           Ymod         |"
	 "          Emod          |"
	 "          XPmod         |          YPmod         |\n", run);
  printf("************************************************************************"
	 "***********************************************************************\n");

  for(Int_t i=0;i<nDET;i++){
    TString det = DetectorList[i];
    det.Resize(16);
    printf("%s%s |", ANSI_COLOR_RESET,det.Data());
    for(Int_t j=0;j<nMOD;j++){
      Int_t good = 0;
      if(TMath::Abs(AvUncorrectedDetectorCos[j][i])<
	 TMath::Abs(AvUncorrectedDetectorCosError[j][i]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     AvUncorrectedDetectorCos[j][i]);
      printf("%s+/-%7.3e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
	     AvUncorrectedDetectorCosError[j][i],ANSI_COLOR_RESET);
    }
    printf("\n");
  }
  printf("\n\n");


  printf("Table 5. Run %i Corrected detector residual Cosine component from profile fit."
	 "\n                 |          Xmod          |           Ymod         |"
	 "          Emod          |"
	 "         XPmod          |         YPmod          |\n", run);
  printf("************************************************************************"
	 "***********************************************************************\n");

  for(Int_t i=0;i<nDET;i++){
    TString det = DetectorList[i];
    det.Resize(16); 
   printf("%s%s |", ANSI_COLOR_RESET,det.Data());
    for(Int_t j=0;j<nMOD;j++){
      Int_t good = 0;
      if(TMath::Abs(AvCorrectedDetectorCos[j][i])<
	 TMath::Abs(AvCorrectedDetectorCosError[j][i]))
	good = 1;
      printf("%s %+8.3e",(!good ? ANSI_COLOR_RED: ANSI_COLOR_RESET),
	     AvCorrectedDetectorCos[j][i]);
      printf("%s+/-%7.3e %s|",(!good ? ANSI_COLOR_RED:ANSI_COLOR_RESET),
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











