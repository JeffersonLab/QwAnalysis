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

Int_t readFile(Int_t runStart=20300, Int_t runEnd=20409, Bool_t wwwdon=kFALSE)
{
  //List runs you want to include
  Int_t runs[] = {20885,20886,20887,20889};

  const Int_t LEN = 1000;
  Bool_t debug = kFALSE;
  string str;
  Int_t cutNum[LEN], numEnt[LEN], numFile = 0, j = 0, runnum;
  Double_t  asymm[LEN], asymmRMS[LEN], yob[LEN], yobRMS[LEN],cntr[LEN];
  Double_t bkgd[LEN], bkgdRMS[LEN], avgLasPow[LEN], errorX[LEN], errorY[LEN];
  Double_t scaLasOnRate[LEN], scaLasOffRate[LEN], meanPosX[LEN], meanPosY[LEN]; 
  numFile = (Int_t)((Double_t)sizeof(runs)/(Double_t)sizeof(Int_t));

  for(Int_t i=runStart; i<=runEnd; i++)
    // for(Int_t i=0; i<numFile; i++)
  {
    runnum = i;// runs[i];
    ifstream file(Form("wwwdon/Run_%i/Run_%i_stats.txt",runnum, runnum));
    ifstream file1(Form("www/run_%i/accum0_%i_stats.txt",runnum, runnum));
    cout<<file<<endl;
    cout<<file1<<endl;

    if(wwwdon)
    {    
      getline(file, str);//skip first line
      while(file>>cutNum[j]>>asymm[j]>>asymmRMS[j]>>numEnt[j]>>yob[j]>>yobRMS[j]
	    >>bkgd[j]>>bkgdRMS[j]>>avgLasPow[j]>>scaLasOnRate[j]>>
	    scaLasOffRate[j]>>meanPosX[j]>>meanPosY[j])
      {

	if(debug)
	{
	  cout<<"cutNum="<<cutNum[j]<<", asymm="<<asymm[j]<<", asymmRMS="
	      <<asymmRMS[j]<<",\n"<<"numEnt="<<numEnt[j]<<", yob="<<yob[j]
	      <<", yobRMS="<<yobRMS[j]<<",\n"<<"bkgd="<<bkgd[j]<<", bkgdRMS="
	      <<bkgdRMS[j]<<", avgLasPow="<<avgLasPow[j]<<",\n"
	      <<"scaLasOnRate="<<scaLasOnRate[j]<<", scaLasOffRate="
	      <<scaLasOffRate[j]<<", meanPosX="<<meanPosX[j]<<", meanPosY="
	      <<meanPosY[j]<<"\n"<<endl;
	}
	cntr[j] = j;
	errorX[j] = 0;//yobRMS[j]/TMath::Sqrt((Double_t)numEnt[j]);
	errorY[j] = 0;//asymmRMS[j]/TMath::Sqrt((Double_t)numEnt[j]);
	j++;
      }
    }
    else
    {    
      getline(file1, str);//skip first line
      while(file1>>cutNum[j]>>asymm[j]>>asymmRMS[j]>>numEnt[j]>>yob[j]>>yobRMS[j]
	    >>bkgd[j]>>bkgdRMS[j]>>avgLasPow[j]>>scaLasOnRate[j]>>
	    scaLasOffRate[j]>>meanPosX[j]>>meanPosY[j])
      {

	if(debug)
	{
	  cout<<"cutNum="<<cutNum[j]<<", asymm="<<asymm[j]<<", asymmRMS="
	      <<asymmRMS[j]<<",\n"<<"numEnt="<<numEnt[j]<<", yob="<<yob[j]
	      <<", yobRMS="<<yobRMS[j]<<",\n"<<"bkgd="<<bkgd[j]<<", bkgdRMS="
	      <<bkgdRMS[j]<<", avgLasPow="<<avgLasPow[j]<<",\n"
	      <<"scaLasOnRate="<<scaLasOnRate[j]<<", scaLasOffRate="
	      <<scaLasOffRate[j]<<", meanPosX="<<meanPosX[j]<<", meanPosY="
	      <<meanPosY[j]<<"\n"<<endl;
	}
	cntr[j] = j;
	errorX[j] = 0;//yobRMS[j]/TMath::Sqrt((Double_t)numEnt[j]);
	errorY[j] = 0;//asymmRMS[j]/TMath::Sqrt((Double_t)numEnt[j]);
	j++;
      }
    }
  }
  TGraphErrors *gr = new TGraphErrors(j+1, yob, asymm, errorX, errorY);
  TCanvas *cPlot = new TCanvas("cPlot", "cPlot", 0, 0, 700,500);
  gr->GetXaxis()->SetTitle("Yield over Background");
  gr->GetYaxis()->SetTitle("Background Subtracted Accum0 Asymmetry");
  gr->GetYaxis()->SetTitleOffset(1.7);
  gr->SetTitle(Form("Background Subtracted Acumm0 Asymmetry vs Yield over Background  (Runs %i-%i)",
		    runStart, runEnd));
  gr->SetMarkerColor(kBlue);
  gr->SetMarkerStyle(7);
  cPlot->SetFillColor(0);
  cPlot->SetLeftMargin(0.14);
  cPlot->SetRightMargin(0.06);
  gr->Draw("AP"); 
  return 1;

}
