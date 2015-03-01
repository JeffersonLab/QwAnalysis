#include "TString.h"
#include "TLine.h"
#include "TProfile.h"
#include "TSystem.h"
#include <iostream>
#include <stdio.h>
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
#include "TMultiGraph.h"
#include "TGraphErrors.h"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//Don Jones 
//June 2013
//This macro was created to plot the slopes of the monitors to coils by slug
//and to histogram the slopes by run.


void getErrorWeightedMean(Double_t *x, Double_t *xe, Double_t *mean, Int_t n){
  Double_t sum = 0, esqinv = 0; 
  for(int i=0;i<n;i++){
    sum += x[i] / (xe[i] * xe[i]);
    esqinv += 1 / (xe[i] * xe[i]);
  }
  mean[0] = sum / esqinv;
  mean[1] = sqrt(1 / esqinv);
}



Int_t plotMonitorOrDetectorSlopes(TString stem = "", Int_t slug_start = 139, 
				  Int_t slug_end = 225, Bool_t plotMon = 1){
  //  gStyle->SetOptFit(1111);
  char  x[255], xe[255], mon[255], monitr[255], det[255], detectr[255], slg[255];
  string line;

  Bool_t ppm_per_micro = 1;
  gStyle->SetOptFit(1111);
  gStyle->SetStatW(0.25);
  gStyle->SetStatH(0.25);
  gStyle->SetOptStat(0);

  const Int_t nDET = 9, nMOD = 5, nCOIL = 10, nSLUGS = 1000;
  Double_t lY = -0.00025, hY = 0.00025;
  Double_t slugArray[nSLUGS],slugArrayErr[nSLUGS] ;
  Int_t order[18] = {5,4,1,2,3,6,9,8,7}, nullErr[nMOD][nDET];
  Int_t slug[nSLUGS][100];
  TString MonitorList[nMOD];
  TString DetectorList[nDET]; 

  char  *ModName[nMOD];
  ModName[0] = "X";
  ModName[3] = "XP";
  ModName[2] = "E";
  ModName[1] = "Y";
  ModName[4] = "YP"; 

  char  *unitsMon[nMOD];
  unitsMon[0] = "#mum";
  unitsMon[3] = "#murad";
  unitsMon[2] = "#mum";
  unitsMon[1] = "#mum";
  unitsMon[4] = "#murad"; 
  char  *unitsDet = "frac";
  Double_t unitConv[5] = {1.e3,1.e3,1.e3,1.,1.};

   if(ppm_per_micro&&!plotMon){
     unitConv[0] = 1e6;
     unitConv[1] = 1e6;
     unitConv[2] = 1e6;
     unitConv[3] = 1e6;
     unitConv[4] = 1e6;
     unitsDet = "ppm";
   }
 //
  //Get list of monitors and detectors from config file.
  //
  ifstream file(Form("%s/config/setup_mpsonly%s.config",
		     gSystem->Getenv("BMOD_SRC"), stem.Data()));
  for(int i=0;i<nMOD;i++){
    file>>mon>>monitr;
    getline(file, line);
    MonitorList[i] = TString(monitr);
    cout<<MonitorList[i]<<endl;
  }
  getline(file, line);
  for(int i=0;i<nDET;i++){
    file>>det>>detectr;
    getline(file, line);
    DetectorList[i] = TString(detectr);
    cout<<DetectorList[i]<<endl;
  }

  TString detector;


  //
  //Get runs in each slug from slug list file
  //
  ifstream slugList(Form("%s/macros/runsBySlug.dat", 
			 gSystem->Getenv("BMOD_SRC")));
  Int_t nSlugs = 0, nSlug = 0, prevSlug  = 0, nRn =0;
  if(slugList.is_open())cout<<"File list found.\n";
  else cout<<"File list not found.\n";

  while(slugList.good()){
    slugList>>slg>>x;
    getline(slugList,line);
    if(slugList.eof())break;
    Int_t sl = atoi(slg); 
    if(sl>=slug_start && sl<=slug_end){
      if(sl!=prevSlug){
	slug[nSlug][0] = sl;
	if(nSlug!=0)
	  slug[nSlug - 1][1] = nRn;
	nSlug++;
	nRn = 0;
	cout<<nSlug<<"\n";
      }
      slug[nSlug-1][nRn+2] = atoi(x);
      nRn++;
    }
    prevSlug = sl;
  }
  nSlugs = nSlug;
  nSlug = 0;


  //
  //Create arrays for storing slug averaged slopes
  //
  Double_t slope[nDET][nCOIL][nSLUGS],slopeError[nDET][nCOIL][nSLUGS];
  Double_t temp[nDET][nCOIL][100], tempErr[nDET][nCOIL][100];

  Int_t nInstr = (plotMon ? nMOD : nDET);
  TString instr = (plotMon ? "mon" : "det");

  Int_t **n, **nRuns;
  n = new Int_t *[nInstr];
  nRuns = new Int_t *[nInstr];
  for(int i=0;i<nInstr;i++){
    n[i]= new Int_t [nCOIL];
    nRuns[i] = new Int_t [nCOIL];
  }

  //
  //Loop over runs and fill arrays with slug averages
  //
  for(int sl = 0; sl < nSlugs; sl++){
    Int_t good = 1; 
    for(int i=0;i<nInstr;i++)
      for(int j=0;j<nCOIL;j++){
	n[i][j] = 0;
	nRuns[i][j] = 0;
      }
    Bool_t atLeastOneGoodRun = 0;
    for(int r = 0; r<slug[sl][1];r++){
      TString filename = Form("%s%s/slopes/%s_coil_coeff_%i.dat",
			gSystem->Getenv("BMOD_OUT"),stem.Data(),
			instr.Data(),slug[sl][r+2]);
      ifstream coeffFile(filename.Data());
      if(coeffFile.is_open())
	cout<<"\nCoefficient file found for run "<<slug[sl][r+2]<<" in slug "<<
	  slug[sl][0]<<".  "<<r<<"  "<<slug[sl][1]<<"\n";
	else 
	  cout<<"\nCoefficient file  NOT found for run "<<
	    slug[sl][r+2]<<" in slug "<<slug[sl][0]<<".\n";
      cout<<filename.Data()<<endl;

      if(coeffFile.good()){
	atLeastOneGoodRun = 1;
	for(int i=0;i<nInstr;i++){
	  TString tempInstr = (plotMon ? MonitorList[i].Data() : 
			      DetectorList[i].Data());
	  getline(coeffFile,line);
	  cout<<"\n"<<line<<":"<<endl;
	  char *tmpStr = new char[line.size()+1];
	  strcpy(tmpStr, line.c_str());
	  if(strcmp(tmpStr, tempInstr.Data())==0){
	    for(int j=0;j<nCOIL;j++){
	      coeffFile>>x>>xe;
	      if(atof(xe)>(plotMon ? 5e-6 : 5e-8)){
		temp[i][j][nRuns[i][j]] = atof(x) * unitConv[i];
		tempErr[i][j][nRuns[i][j]] = atof(xe) * unitConv[i];
		printf("%+10.3e+/-%7.2e  ",temp[i][j][nRuns[i][j]],
		       tempErr[i][j][nRuns[i][j]]);
		
	      }else {
		good = 0;
		cout<<"                       "<<endl;
	      }
	      if(good)nRuns[i][j]++;
	      //	      getline(coeffFile, line);
	      char tmpChar = coeffFile.get();
	    }
	  }else printf("\n%s not found in file\n", tempInstr.Data());
	}
      }
      coeffFile.close();
    }

    if(atLeastOneGoodRun&&slug[sl][0]!=224&&slug[sl][0]!=176){
      slugArray[nSlug] = slug[sl][0];
      slugArrayErr[nSlug] = 0;
      Double_t par[2];
      //      cout<<"\nAverage slopes for slug "<<slug[sl][0]<<":\n";
      for(int i=0;i<nInstr;i++){
	for(int j=0;j<nCOIL;j++){
	  getErrorWeightedMean(temp[i][j], tempErr[i][j], par, nRuns[i][j]);
	  slope[i][j][nSlug] = par[0];
	  slopeError[i][j][nSlug] = par[1];
	  //	  cout<<slope[i][j][nSlug]<<endl;
	}
      }
      nSlug++;
    }
  }
  cout<<"\nnSlugs = "<<nSlug<<endl;

  //
  //Plot slug averages and histograms of data
  //
  cout<<nSlug<<" good slugs found.\n";
  TCanvas *cCos[nMOD]; 
  TCanvas *cSin[nMOD]; 

  for(int i=0;i<nMOD;i++)
    MonitorList[i].Replace(0,4,"");
  for(int i=0;i<nDET;i++)
    DetectorList[i].Replace(0,4,"");

  TGraphErrors *grS[nMOD][nDET];
  TGraphErrors *grC[nMOD][nDET];
  Double_t fit[2][nMOD][nDET];
  Double_t fitError[2][nMOD][nDET];
  TLine *tl1; 
  TLine *tl2 ;
  //  TF1 *f = new TF1("f","pol0");
  for(int j=0; j<nMOD;j++){
    cSin[j] = new TCanvas(Form("cSinMod%i",j),Form("cSinMod%i",j), 0,0,1600,1000);
    cSin[j]->Divide(3,(plotMon ? 2 : 3));
    for(int i=0;i<nInstr;i++){
      if(plotMon) cSin[j]->cd(i+1);
      else cSin[j]->cd(order[i]);

      grS[j][i] = new TGraphErrors(nSlug,slugArray,slope[i][j],
				   slugArrayErr, slopeError[i][j]);
      grS[j][i]->SetLineColor(kRed);
      grS[j][i]->SetMarkerColor(kRed);
      grS[j][i]->SetMarkerStyle(7);
      grS[j][i]->Draw("ap");
      if(plotMon){
	grS[j][i]->SetTitle(Form("%s Response to Sine(ramp) [%s modulation]", 
				 MonitorList[i].Data(), ModName[j]));
	grS[j][i]->GetYaxis()->SetTitle(Form("%s Response (%s)", MonitorList[i].Data(), 
					     unitsMon[i]));
      }else{
	grS[j][i]->SetTitle(Form("%s Fractional Response to Sine(ramp) [%s modulation]", 
				 DetectorList[i].Data(), ModName[j]));
	grS[j][i]->GetYaxis()->SetTitle(Form("%s Response (%s)", DetectorList[i].Data(), 
					     unitsDet));
      }
      grS[j][i]->GetYaxis()->SetTitleSize(0.037);
      grS[j][i]->GetYaxis()->SetTitleOffset(1.35);
      grS[j][i]->GetXaxis()->SetTitle("Slug");
 //       grS[j][i]->Fit("f");
//       fit[0][j][i] = f->GetParameter(0);
//       fitError[0][j][i] = f->GetParError(0);
      grS[j][i]->GetXaxis()->SetTitle("Slug");;
      grS[j][i]->GetYaxis()->SetTitleSize(0.037);
      tl1 = new TLine(160,grS[j][i]->GetYaxis()->GetXmin(),160,
		      grS[j][i]->GetYaxis()->GetXmax());
      tl1->SetLineWidth(3);  
      tl1->SetLineStyle(2);        
      tl1->Draw("same");
      tl2 = new TLine(172,grS[j][i]->GetYaxis()->GetXmin(),
		      172,grS[j][i]->GetYaxis()->GetXmax());
      tl2->SetLineWidth(3);  
      tl2->SetLineStyle(2);        
      tl2->Draw("same");
      gPad->Update();
    }
    cCos[j] = new TCanvas(Form("cCosMod%i",j),Form("cCosMod%i",j), 0,0,1600,1000);
    cCos[j]->Divide(3,(plotMon ? 2 : 3));
    for(int i=0;i<nInstr;i++){
      if(plotMon) cCos[j]->cd(i+1);
      else cCos[j]->cd(order[i]);
      grC[j][i] = new TGraphErrors(nSlug,slugArray,slope[i][j+5],slugArrayErr,
				   slopeError[i][j+5]);
      grC[j][i]->SetLineColor(kBlue);
      grC[j][i]->GetXaxis()->SetTitle("Slug");
      grC[j][i]->SetLineColor(kBlue);
      grC[j][i]->SetMarkerColor(kBlue);
      grC[j][i]->SetMarkerStyle(7);
      grC[j][i]->Draw("ap");
      if(plotMon){
	grC[j][i]->SetTitle(Form("%s Response to Cosine(ramp) [%s modulation]", 
				 MonitorList[i].Data(), ModName[j]));
	grC[j][i]->GetYaxis()->SetTitle(Form("%s Response (%s)", MonitorList[i].Data(), 
					     unitsMon[i]));
      }else{
	grC[j][i]->SetTitle(Form("%s Fractional Response to Cosine(ramp) [%s modulation]", 
				 DetectorList[i].Data(), ModName[j]));
	grC[j][i]->GetYaxis()->SetTitle(Form("%s Response (%s)", DetectorList[i].Data(), 
					     unitsDet));
      }
      grC[j][i]->GetYaxis()->SetTitleSize(0.037);
      grC[j][i]->GetYaxis()->SetTitleOffset(1.35);
      tl1 = new TLine(160,grC[j][i]->GetYaxis()->GetXmin(),160,
		      grC[j][i]->GetYaxis()->GetXmax());
      tl1->SetLineWidth(3);  
      tl1->SetLineStyle(2);        
      tl1->Draw("same");
      tl2 = new TLine(172,grC[j][i]->GetYaxis()->GetXmin(),
		      172,grC[j][i]->GetYaxis()->GetXmax());
      tl2->SetLineWidth(3);  
      tl2->SetLineStyle(2);        
      tl2->Draw("same");
//       grC[j][i]->Fit("f");
//       fit[1][j][i] = f->GetParameter(0);
//       fitError[1][j][i] = f->GetParError(0);
      gPad->Update();
   }
  }

  if(plotMon){
    TCanvas *cMon[2];
    cMon[0] = new TCanvas("cMonS","cMonS",0,0,1500,1500);
    cMon[1] = new TCanvas("cMonC","cMonC",0,0,1500,1500);
    cMon[0]->Divide(5,5);
    cMon[1]->Divide(5,5);
    for(int j=0;j<5;j++){
      for(int i=0;i<5;i++){
	cMon[0]->cd(j*5+i+1);
	grS[i][j]->Draw("ap");
	tl1 = new TLine(160,grS[i][j]->GetYaxis()->GetXmin(),160,
			grS[i][j]->GetYaxis()->GetXmax());
	tl1->SetLineWidth(2);  
	tl1->SetLineStyle(2);        
	tl2 = new TLine(172,grS[i][j]->GetYaxis()->GetXmin(),
			172,grS[i][j]->GetYaxis()->GetXmax());
	tl2->SetLineWidth(3);  
	tl2->SetLineStyle(2);        
	tl2->Draw("same");
	tl1->Draw("same");
	cMon[1]->cd(j*5+i+1);
	grC[i][j]->Draw("ap");
	tl1 = new TLine(160,grC[i][j]->GetYaxis()->GetXmin(),160,
			grC[i][j]->GetYaxis()->GetXmax());
	tl1->SetLineWidth(3);  
	tl1->SetLineStyle(2);        
	tl2 = new TLine(172,grC[i][j]->GetYaxis()->GetXmin(),
			172,grC[i][j]->GetYaxis()->GetXmax());
	tl2->SetLineWidth(3);  
	tl2->SetLineStyle(2);        
	tl2->Draw("same");
	tl1->Draw("same");
      }
    }
  }else{
    Int_t order[15] = {1,6,11,2,7,12,3,8,13,4,9,14,5,10,15};
    TCanvas *cDet[2];
    cDet[0] = new TCanvas("cDetS","cDetS",0,0,1500,1000);
    cDet[1] = new TCanvas("cDetC","cDetC",0,0,1500,1000);
    cDet[0]->Divide(5,3);
    cDet[1]->Divide(5,3);
    Int_t g = 0;
    for(int i=0;i<5;i++){
      for(int j=0;j<5;j+=(j==0 ? 1:2)){
	cDet[0]->cd(order[g]);
	grS[i][j]->Draw("ap");
	tl1 = new TLine(160,grS[i][j]->GetYaxis()->GetXmin(),160,
			grS[i][j]->GetYaxis()->GetXmax());
	tl1->SetLineWidth(3);  
	tl1->SetLineStyle(2);        
	tl2 = new TLine(172,grS[i][j]->GetYaxis()->GetXmin(),
			172,grS[i][j]->GetYaxis()->GetXmax());
	tl2->SetLineWidth(3);  
	tl2->SetLineStyle(2);        
	tl2->Draw("same");
	tl1->Draw("same");
	cDet[1]->cd(order[g]);
	grC[i][j]->Draw("ap");
	tl1 = new TLine(160,grC[i][j]->GetYaxis()->GetXmin(),160,
			grC[i][j]->GetYaxis()->GetXmax());
	tl1->SetLineWidth(3);  
	tl1->SetLineStyle(2);        
	tl2 = new TLine(172,grC[i][j]->GetYaxis()->GetXmin(),
			172,grC[i][j]->GetYaxis()->GetXmax());
	tl2->SetLineWidth(3);  
	tl2->SetLineStyle(2);        
	tl2->Draw("same");
	tl1->Draw("same");
	g++;
      }
    }
  }
  return 0;
}
