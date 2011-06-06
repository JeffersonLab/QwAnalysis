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
#include "TMultiGraph.h"
#include "TLegend.h"
#include "TF1.h"

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

//Bool_t isNaN(Double_t number)
Int_t decaPlotScaler(Int_t runStart=20300, Int_t runEnd=20409)
{

  const Int_t LEN = 500;
  Bool_t debug = kFALSE, goodRun, foundData;
  string str = "";
  Int_t cutNum[LEN], numEnt[LEN], j = 0, kIn = 0, kOut = 0, runnum;
  Double_t wAvgNumerPD, wAvgDenomPD, wAvgNumerSc, wAvgDenomSc;
  Double_t wAvgNumerLuT, wAvgDenomLuT, wAvgScBkgdNumer, wAvgScBkgdDenom;
  Double_t wAvgPDBkgdNumer, wAvgPDBkgdDenom;
  Double_t wienSign;
  //arrays for HWP IN
  Double_t asymmPDAvgIn[LEN], asymmPDAvgErrIn[LEN], asymmScLwAvgIn[LEN], asymmScLwAvgErrIn[LEN];
  Double_t asymmLuTableIn[LEN], asymmLuTabErrIn[LEN], pD_ScaBkgdIn[LEN],pD_ScaBkgdErrIn[LEN];
  Double_t scintScaBkgdIn[LEN], scintScaBkgdErrIn[LEN], errorXIn[LEN], runNumIn[LEN];;
  //arrays for HWP OUT
  Double_t asymmPDAvgOut[LEN], asymmPDAvgErrOut[LEN], asymmScLwAvgOut[LEN], asymmScLwAvgErrOut[LEN];
  Double_t asymmLuTableOut[LEN], asymmLuTabErrOut[LEN], pD_ScaBkgdOut[LEN],pD_ScaBkgdErrOut[LEN];
  Double_t scintScaBkgdOut[LEN], scintScaBkgdErrOut[LEN], errorXOut[LEN], runNumOut[LEN];;
  //temporary arrays for storing runwise values
  Double_t asymmPDAvg1[LEN], asymmPDAvgErr1[LEN], asymmScLwAvg1[LEN], asymmScLwAvgErr1[LEN];
  Double_t asymmLuTable1[LEN], asymmLuTabErr1[LEN], pD_ScaBkgd1[LEN],pD_ScaBkgdErr1[LEN];
  Double_t scintScaBkgd1[LEN], scintScaBkgdErr1[LEN], cntr[LEN];
  //  Double_t scaLasOnRate[LEN], scaLasOffRate[LEN], meanPosX[LEN], meanPosY[LEN];
 
  //open run list and use the file to determine good runs and cuts
  ifstream runlist("/net/cdaqfs/home/cdaq/compton/photon_runlist.dat");
  runlist>>skipws;//skip whitespaces
  if(!runlist.is_open())
  {
    printf("Run list file not found.\n");
    return 0;
  }

  ////////////////////////////////////////////////////////////
  //Loop through the run list file and use it to select the //
  //useful runs of interest for the plot.                   //
  ////////////////////////////////////////////////////////////
  for(Int_t i=runStart; i<=runEnd; i++)
  // for(Int_t i=0; i<numFile; i++)
  {
    char date[255], disc[255], hv[255], gain[255], hWien[255], vWien[255], phi_FG[255];
    char run[255], time[255], passive[255], hwp[255], cur[255], goodFor[255];


    //skip lines in file beginning with "!"
    while(runlist.peek() == 33)
    {
      getline(runlist, str);
    }

    //extract one line of data from runlist file and use it to form cuts
    runlist>>run>>date>>time>>disc>>hv>>passive>>gain>>hWien>>vWien>>phi_FG>>hwp>>cur
	   >>goodFor;

    //test for end of file
    if(!runlist.eof())
    {
      cout<<run<<" "<<date<<" "<<time<<" "<<disc<<" "<<hv<<" "<<passive<<" "<<gain
	  <<" "<<hWien<<vWien<<phi_FG<<" "<<hwp<<" "<<cur<<" "<<goodFor<<endl;

      runnum = atoi(run);// runs[i];
      i = runnum;
    }
    else 
    {
      cout<<"Error: unexpected end of file."<<endl;
    }
    if(atoi(goodFor) == 1 || atoi(goodFor) == 3) goodRun = kTRUE;
    else goodRun = kFALSE;

    //only first word of comment field in file gets input to "comment" variable
    //so advance until end of line reached
    while(runlist.good() && runlist.get()!='\n' && !runlist.eof()){}



    ///////////////////////////////////////
    //Examples of cuts that might be used//
    ///////////////////////////////////////
    /*
    if(atoi(hv)==1550)goodRun = kTRUE;//cut on HV setting
    else goodRun = kFALSE;
    cout<<goodRun<<endl;
    goodRun = kTRUE;
    if(goodRun && atoi(disc)==20)goodRun = kTRUE;//cut on disciminator threshold setting
    else goodRun = kFALSE;
    cout<<"goodRun="<<goodRun<<endl;
    if(goodRun && strcmp(passive,"passive2")==0)goodRun = kTRUE;//cut on passive/FIFO
    else goodRun = kFALSE;
    cout<<"goodRun="<<goodRun<<endl;
    if(goodRun && atof(gain)>1.0-0.01 && atof(gain)<1.0+0.01)goodRun = kTRUE;//cut on gain
    else goodRun = kFALSE;
    cout<<"goodRun="<<goodRun<<endl;
    if(goodRun && atof(wien)>-62.40-0.01 && atof(wien)<-62.40+0.01)goodRun = kTRUE;//cut on Wien Angle
    else goodRun = kFALSE;
    cout<<"goodRun="<<goodRun<<endl;
    if(goodRun && atoi(cur)==150)goodRun = kTRUE;//cut on nominal current
    else goodRun = kFALSE;
    cout<<"goodRun="<<goodRun<<endl;
    */

    //set local variable values to 0
    j = 0;
    wAvgNumerPD = 0;
    wAvgDenomPD = 0;
    wAvgNumerSc = 0;
    wAvgDenomSc = 0;
    wAvgNumerLuT = 0;
    wAvgDenomLuT = 0;
    wAvgPDBkgdNumer = 0;
    wAvgPDBkgdDenom = 0;
    wAvgScBkgdNumer = 0;
    wAvgScBkgdDenom = 0;
    ifstream file(Form("www/run_%i/analyzeScaler_%i_stats.txt",runnum, runnum));
    cout<<file<<endl;

    if(file && goodRun)//if file is found and run passes cuts
    {
      //fix asymmetry sign from Wien angle changes
      if(atof(hWien)*atof(vWien)*atof(phi_FG) > 0) wienSign = -1.0;
      else wienSign = 1.0;

      printf("Using run %i. i=%i\n", runnum,i);
      getline(file, str);//skip first line
      while(file>>cutNum[j]>>asymmPDAvg1[j]>>asymmPDAvgErr1[j]>>numEnt[j]
	    >>asymmScLwAvg1[j]>>asymmScLwAvgErr1[j]>>asymmLuTable1[j]>>asymmLuTabErr1[j]
	    >>pD_ScaBkgd1[j]>>pD_ScaBkgdErr1[j]>>scintScaBkgd1[j]>>scintScaBkgdErr1[j])
      {
	foundData = kTRUE;
	if(debug)
	{
	  cout<<"cutNum="<<cutNum[j]<<", asymmPDLwAvg="<<asymmPDAvg1[j]<<"+/-"
	      <<asymmPDAvgErr1[j]<<"\n"<<"numEnt="<<numEnt[j]
	      <<"AsymmScLwAvg="<<asymmScLwAvg1[j]<<"+/-"<<asymmScLwAvgErr1[j]<<"\n"
	      <<"AsymmLuTable="<<asymmLuTable1[j]<<"+/-"<<asymmLuTabErr1[j]<<"\n"
	      <<"PD_ScaBkgd="<<pD_ScaBkgd1[j]<<"+/-"<<pD_ScaBkgdErr1[j]<<"\n"
	      <<"ScintScaBkgd="<<scintScaBkgd1[j]<<"+/-"<<scintScaBkgdErr1[j]<<endl;
	}

	//find terms for calculating weighted average for a run
	wAvgNumerPD += asymmPDAvg1[j] / TMath::Power(asymmPDAvgErr1[j],2);
	wAvgNumerSc += asymmScLwAvg1[j] / TMath::Power(asymmScLwAvgErr1[j],2);
	wAvgNumerLuT += asymmLuTable1[j] / TMath::Power(asymmLuTabErr1[j],2);
	wAvgScBkgdNumer += scintScaBkgd1[j] / TMath::Power(scintScaBkgdErr1[j],2);
	wAvgPDBkgdNumer += pD_ScaBkgdErr1[j] / TMath::Power(pD_ScaBkgdErr1[j],2);
	wAvgDenomPD += TMath::Power(asymmPDAvgErr1[j],-2);
	wAvgDenomSc += TMath::Power(asymmScLwAvgErr1[j],-2);
	wAvgDenomLuT += TMath::Power(asymmLuTabErr1[j],-2);
	wAvgScBkgdDenom += TMath::Power(scintScaBkgdErr1[j],-2);
	wAvgPDBkgdDenom += TMath::Power(pD_ScaBkgdErr1[j],-2);


	cntr[j] = j;
	j++;
      }
      if(strcmp(hwp, "OUT")==0 && foundData)
      {
	runNumOut[kOut] = (Double_t)runnum;
	//calculate weighted average
	asymmPDAvgOut[kOut]    = wienSign *  wAvgNumerPD     / wAvgDenomPD;
	asymmScLwAvgOut[kOut]  = wienSign *  wAvgNumerSc     / wAvgDenomSc;
	asymmLuTableOut[kOut]  = wienSign *  wAvgNumerLuT    / wAvgDenomLuT;
	pD_ScaBkgdOut[kOut]    = wienSign *  wAvgPDBkgdNumer / wAvgPDBkgdDenom;
	scintScaBkgdOut[kOut]  = wienSign *  wAvgScBkgdNumer / wAvgScBkgdDenom;
	//calculate weighted average
	asymmPDAvgErrOut[kOut]    = TMath::Sqrt(1 / wAvgDenomPD);
	asymmScLwAvgErrOut[kOut]  = TMath::Sqrt(1 / wAvgDenomSc);
	asymmLuTabErrOut[kOut]    = TMath::Sqrt(1 / wAvgDenomLuT);
	pD_ScaBkgdErrOut[kOut]    = TMath::Sqrt(1 / wAvgPDBkgdDenom);
	scintScaBkgdErrOut[kOut]  = TMath::Sqrt(1 / wAvgScBkgdDenom );

	errorXOut[kOut] = 0;
	kOut++;
      }
      else if(strcmp(hwp,"IN")==0 && foundData)
      {//for plotting purposes change IN values to positive
	runNumIn[kIn] = (Double_t)runnum;
	asymmPDAvgIn[kIn] =   -wienSign * wAvgNumerPD / wAvgDenomPD;
	asymmScLwAvgIn[kIn] = -wienSign * wAvgNumerSc / wAvgDenomSc;
	asymmLuTableIn[kIn] = -wienSign * wAvgNumerLuT / wAvgDenomLuT;
	pD_ScaBkgdIn[kIn]  =   wAvgPDBkgdNumer / wAvgPDBkgdDenom;
	scintScaBkgdIn[kIn]  =  wAvgScBkgdNumer / wAvgScBkgdDenom ;
	//calculate weighted average
	asymmPDAvgErrIn[kIn] = TMath::Sqrt(1/ wAvgDenomPD);
	asymmScLwAvgErrIn[kIn] = TMath::Sqrt(1 / wAvgDenomSc);
	asymmLuTabErrIn[kIn]  = TMath::Sqrt(1/wAvgDenomLuT);
	pD_ScaBkgdErrIn[kIn]  =  TMath::Sqrt(1 / wAvgPDBkgdDenom);
	scintScaBkgdErrIn[kIn]  =  TMath::Sqrt(1 / wAvgScBkgdDenom );
	errorXIn[kIn] = 0;
	kIn++;
      }
      else 
      {
	cout<<endl;
	cout<<"NO DATA FOUND for run "<<runnum<<endl;
	cout<<endl;
      }
      /*
      if(runnum == 20836)printf("  asymmPDAvg = %f +/- %f\n"
				"asymmScLwAvg = %f +/- %f\n"
				"asymmLuTable = %f +/- %f\n",
				asymmPDAvg[k], asymmPDAvgErr[k],
				asymmScLwAvg[k], asymmScLwAvgErr[k],
				asymmLuTable[k], asymmLuTabErr[k] );
      */
      file.close();
    }
    else if(!file) 
      printf("Cannot find\"www/compton/photonsummary/run_%i/analyzeScaler_%i_stats.txt\"\n",
		runnum,runnum);

    //skip lines in file beginning with "!"
    while(runlist.peek() == 33)
    {
      getline(runlist, str);
    }

    //terminate loop if end of file reached
    if(runlist.eof())
    {
      i = runEnd + 1; 
      cout<<"End of file reached."<<endl;
    }

    foundData = kFALSE;

  }//end of for loop

  for(Int_t i=0; i<kIn;i++)
  {
    printf("  asymmPDAvgIn[%5.0f] = %f +/- %f\n",runNumIn[i],asymmPDAvgIn[i], asymmPDAvgErrIn[i]);
    printf("asymmScLwAvgIn[%5.0f] = %f +/- %f\n",runNumIn[i],asymmScLwAvgIn[i], asymmScLwAvgErrIn[i]);
    printf("asymmLuTableIn[%5.0f] = %f +/- %f\n",runNumIn[i],asymmLuTableIn[i],asymmPDAvgErrIn[i]);
  }
  for(Int_t i=0; i<kOut;i++)
  {
    printf("  asymmPDAvgOut[%5.0f] = %f +/- %f\n",runNumOut[i],asymmPDAvgOut[i], asymmPDAvgErrOut[i]);
    printf("asymmScLwAvgOut[%5.0f] = %f +/- %f\n",runNumOut[i],asymmScLwAvgOut[i], asymmScLwAvgErrOut[i]);
    printf("asymmLuTableOut[%5.0f] = %f +/- %f\n",runNumOut[i],asymmLuTableOut[i],asymmPDAvgErrOut[i]);
  }

  gStyle->SetTitleFontSize(0.077);
  TCanvas *c1 = new TCanvas("c1","c1",0,0,1200,1000);
  c1->Divide(1,3);
  c1->cd(3);
  //  gStyle->SetTitleW(0.61); //title width
  //gStyle->SetTitleH(0.08); //title height
  //Graph of PD data with HWP OUT.
  TGraphErrors *gPDOut = new TGraphErrors(kOut,runNumOut,asymmPDAvgOut,errorXOut,asymmPDAvgErrOut);
  gPDOut->SetMarkerColor(kRed);
  gPDOut->SetLineColor(kRed);
  gPDOut->SetMarkerStyle(7);
  gPDOut->SetTitle("HWP OUT");
  //gStyle->SetOptFit(1111);
  //gr->Fit("pol0");

  //Graph of PD data with HWP IN.
  TGraphErrors *gPDIn = new TGraphErrors(kIn,runNumIn,asymmPDAvgIn,errorXIn,asymmPDAvgErrIn);
  gPDIn->SetMarkerColor(kBlue);
  gPDIn->SetLineColor(kBlue);
  gPDIn->SetMarkerStyle(7);
  gPDIn->SetTitle("HWP IN");
  //gStyle->SetOptFit(1111);
  //gr->Fit("pol0");
  TMultiGraph *mgPD = new TMultiGraph();
  mgPD->SetTitle("PD Average Bkgnd Subtracted Scaler Asymmetry(%)");
  mgPD->Add(gPDIn);
  mgPD->Add(gPDOut);
  mgPD->Draw("AP");
  mgPD->GetXaxis()->SetTitle("Run Number");
  mgPD->GetXaxis()->SetTitleSize(0.06);
  mgPD->GetXaxis()->SetTitleOffset(0.61);
  mgPD->GetYaxis()->SetTitleOffset(0.61);
  mgPD->GetYaxis()->SetTitle("Scaler Asymmetry (%)");
  mgPD->GetYaxis()->SetTitleSize(0.07);
  c1->Update();
  TLegend *legend = new TLegend(0.87,0.80,0.93,1);
  //legend1->SetHeader("Some histograms");
  legend->AddEntry(gPDIn,"HWP IN","l");
  legend->AddEntry(gPDOut,"HWP OUT","l");
  legend->SetFillColor(0);
  legend->Draw();
  //c1->BuildLegend();

  c1->cd(2);
  //Graph of scintillator laserwise background subtraction data with HWP OUT.
  TGraphErrors *gScOut = new TGraphErrors(kOut,runNumOut,asymmScLwAvgOut,errorXOut,asymmScLwAvgErrOut);
  gScOut->SetMarkerColor(kRed);
  gScOut->SetLineColor(kRed);
  gScOut->SetMarkerStyle(7);
  gScOut->SetTitle("HWP OUT");
  //  gStyle->SetOptFit(1111);
  //gr1->Fit("pol0");

  //Graph of scintillator laserwise background subtraction data with HWP IN.
  TGraphErrors *gScIn = new TGraphErrors(kIn,runNumIn,asymmScLwAvgIn,errorXIn,asymmScLwAvgErrIn);
  gScIn->SetMarkerColor(kBlue);
  gScIn->SetLineColor(kBlue);
  gScIn->SetMarkerStyle(7);
  gScIn->SetTitle("HWP IN");
  //  gStyle->SetOptFit(1111);
  //gr1->Fit("pol0");
  TMultiGraph *mgSc = new TMultiGraph();
  mgSc->SetTitle("Scint Avg Bkgnd Subtracted Scaler Asymmetry (%)");
  mgSc->Add(gScIn);
  mgSc->Add(gScOut);
  mgSc->Draw("AP");
  mgSc->GetXaxis()->SetTitle("Run Number");
  mgSc->GetXaxis()->SetTitleSize(0.06);
  mgSc->GetXaxis()->SetTitleOffset(0.61);
  mgSc->GetYaxis()->SetTitleOffset(0.61);
  mgSc->GetYaxis()->SetTitle("Scaler Asymmetry (%)");
  mgSc->GetYaxis()->SetTitleSize(0.07);
  c1->Update();
  TLegend *legend1 = new TLegend(0.87,0.80,0.93,1);
  //legend1->SetHeader("Some histograms");
  legend1->AddEntry(gScIn,"HWP IN","l");
  legend1->AddEntry(gScOut,"HWP OUT","l");
  legend1->SetFillColor(0);
  legend1->Draw();

  //c1->BuildLegend();

  c1->cd(1);
  //graph of scintillator lookup table background subtraction with HWP OUT
  TGraphErrors *gLuTOut = new TGraphErrors(kOut,runNumOut,asymmLuTableOut,errorXOut,asymmLuTabErrOut);
  gLuTOut->SetMarkerColor(kRed);
  gLuTOut->SetLineColor(kRed);
  gLuTOut->SetMarkerStyle(7);
  gLuTOut->SetTitle("HWP OUT");
  //  gStyle->SetOptFit(1111);
  //gr2->Fit("pol0");

  //graph of scintillator lookup table background subtraction with HWP IN
  TGraphErrors *gLuTIn = new TGraphErrors(kIn,runNumIn,asymmLuTableIn,errorXIn,asymmLuTabErrIn);
  gLuTIn->SetMarkerColor(kBlue);
  gLuTIn->SetLineColor(kBlue);
  gLuTIn->SetMarkerStyle(7);
  gLuTIn->SetTitle("HWP IN");
  //  gStyle->SetOptFit(1111);
  //gr2->Fit("pol0");
  TMultiGraph *mgLuT = new TMultiGraph();
  mgLuT->SetTitle("Lookup Table Bkgnd Subtracted Scaler Asymmetry (%)");
  mgLuT->Add(gLuTIn);
  mgLuT->Add(gLuTOut);
  mgLuT->Draw("AP");
  mgLuT->GetXaxis()->SetTitle("Run Number");
  mgLuT->GetXaxis()->SetTitleSize(0.061);
  mgLuT->GetXaxis()->SetTitleOffset(0.61);
  mgLuT->GetYaxis()->SetTitleOffset(0.6);
  mgLuT->GetYaxis()->SetTitle("Scaler Asymmetry (%)");
  mgLuT->GetYaxis()->SetTitleSize(0.07);
  c1->Update();
  //c1->BuildLegend();
  TLegend *legend2 = new TLegend(0.87,0.80,0.93,1);
  //legend1->SetHeader("Some histograms");
  legend2->AddEntry(gLuTIn,"HWP IN","l");
  legend2->AddEntry(gLuTOut,"HWP OUT","l");
  legend2->SetFillColor(0);
  legend2->Draw();

  //save plot to file
  c1->Print(Form("~/users/jonesdc/compton_runs/scaler_Runs_%i_to_%i.png",runStart, runEnd));
 
  return 1;
}
