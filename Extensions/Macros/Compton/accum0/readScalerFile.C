#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include "TROOT.h"
#include "TCint.h"
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TAxis.h"
#include "TStyle.h"

using namespace std;

/*
  fprintf(polstats,"#CutNum\t Asymm\t\t AsymmRMS \tNumEnt\t YieldOverBkgd  "
	  " YOB_RMS  \t Background\t BkgdRMS \t AvgLasPow\t ScaLasOn"
	  " \t ScaLasOff\t MeanPosX\t MeanPosY\n"); 
  fprintf(polstats,"%d\t%12.5e\t%9.2e\t%d\t%12.5e\t%9.2e\t%12.5e\t%9.2e\t"
	  "%12.5e\t%12.5e\t%12.5e\t%12.5e\t%12.5e\n",
	  i, hTempD->GetMean(), hTempD->GetRMS(), numEnt,
	  yieldOverBkgd, hTempD2->GetRMS(),bkg[i], bkgWidth[i],lasAvgPow,
	  scaLasOnRate,scaLasOffRate, meanPosX, meanPosY);
*/

Int_t readScalerFile(Int_t runStart=20300, Int_t runEnd=20409)
{
  //List runs you want to include
  Int_t runs[] = {20885,20886,20887,20889};

  const Int_t LEN = 5000;
  Bool_t debug = kFALSE;
  string str;
  Int_t cutNum[LEN], numEnt[LEN], numFile = 0, j = 0, runnum;
  Double_t asymmPDAvg[LEN], asymmPDAvgErr[LEN], asymmScLwAvg[LEN], asymmScLwErr [LEN];
  Double_t asymmLuTable[LEN], asymmLuTabErr[LEN], pD_ScaBkgd[LEN],pD_ScaBkgdErr[LEN];
  Double_t scintScaBkgd[LEN], scintScaBkgdErr[LEN], errorX[LEN], cntr[LEN];
  //  Double_t scaLasOnRate[LEN], scaLasOffRate[LEN], meanPosX[LEN], meanPosY[LEN]; 
  numFile = (Int_t)((Double_t)sizeof(runs)/(Double_t)sizeof(Int_t));

  for(Int_t i=runStart; i<=runEnd; i++)
    // for(Int_t i=0; i<numFile; i++)
  {
    runnum = i;// runs[i];
    ifstream file(Form("www/run_%i/analyzeScaler_%i_stats.txt",runnum, runnum));
    cout<<file<<endl;

    getline(file, str);//skip first line
    while(file>>cutNum[j]>>asymmPDAvg[j]>>asymmPDAvgErr[j]>>numEnt[j]
	  >>asymmScLwAvg[j]>>asymmScLwErr[j]>>asymmLuTable[j]>>asymmLuTabErr[j]
	  >>pD_ScaBkgd[j]>>pD_ScaBkgdErr[j]>>scintScaBkgd[j]>>scintScaBkgdErr[j])
    {
      
      if(debug)
      {
	cout<<"cutNum="<<cutNum[j]<<", asymmPDLwAvg="<<asymmPDAvg[j]<<"+/-"
	    <<asymmPDAvgErr[j]<<"\n"<<"numEnt="<<numEnt[j]
	    <<"AsymmScLwAvg="<<asymmScLwAvg[j]<<"+/-"<<asymmScLwErr[j]<<"\n"
	    <<"AsymmLuTable="<<asymmLuTable[j]<<"+/-"<<asymmLuTabErr[j]<<"\n"
	    <<"PD_ScaBkgd="<<pD_ScaBkgd[j]<<"+/-"<<pD_ScaBkgdErr[j]<<"\n"
	    <<"ScintScaBkgd="<<scintScaBkgd[j]<<"+/-"<<scintScaBkgdErr[j]<<endl;
      }
      errorX[j] = 0;
      cntr[j] = j;
      j++;
    }
  }

  TCanvas *c1 = new TCanvas("c1","c1",0,0,1400,1000);
  c1->Divide(1,3);
  c1->cd(3);
  //  gStyle->SetTitleW(0.61); //title width
  //gStyle->SetTitleH(0.08); //title height
  TGraphErrors *gr = new TGraphErrors(j,cntr,asymmPDAvg,errorX,asymmPDAvgErr);
  gr->GetXaxis()->SetTitle("Cut Number");
  gr->GetYaxis()->SetTitle("Scaler Asymmetry (%)");
  gr->GetYaxis()->SetTitleOffset(1.2);
  gr->SetTitle("PD Average Bkgnd Subtracted Scaler Asymmetry(%)");
  gr->SetMarkerColor(kRed);
  gr->SetMarkerStyle(6);
  //gStyle->SetOptFit(1111);
  //gr->Fit("pol0");
  gr->Draw("AP");
  c1->cd(2);
  TGraphErrors *gr1 = new TGraphErrors(j,cntr,asymmScLwAvg,errorX,asymmScLwErr);
  gr1->GetXaxis()->SetTitle("Cut Number");
  gr1->GetYaxis()->SetTitle("Scaler Asymmetry (%)");
  gr1->GetYaxis()->SetTitleOffset(1.2);
  gr1->SetTitle("Scint Avg Bkgnd Subtracted Scaler Asymmetry (%)");
  gr1->SetMarkerColor(kRed);
  gr1->SetMarkerStyle(6);
  //  gStyle->SetOptFit(1111);
  //gr1->Fit("pol0");
  gr1->Draw("AP");

  c1->cd(1);
  TGraphErrors *gr2 = new TGraphErrors(j,cntr,asymmLuTable,errorX, asymmLuTabErr);
  gr2->GetXaxis()->SetTitle("Cut Number");
  gr2->GetYaxis()->SetTitle("Scaler Asymmetry (%)");
  gr2->GetYaxis()->SetTitleOffset(1.2);
  gr2->SetTitle("Lookup Table Bkgnd Subtracted Scaler Asymmetry (%)");
  gr2->SetMarkerColor(kRed);
  gr2->SetMarkerStyle(6);
  //  gStyle->SetOptFit(1111);
  //gr2->Fit("pol0");
  gr2->Draw("AP");
  c1->Print(Form("~/users/jonesdc/compton_runs/scaler_Runs_%i_to_%i.png",runStart, runEnd));
  return 1;
}
