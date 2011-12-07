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
Int_t plotAccum0ByCut(Int_t runStart=20300, Int_t runEnd=20409)
{
  //List runs you want to include
  Int_t runs[] = {20885,20886,20887,20889};


  const Int_t LEN = 500;
  Bool_t debug = kFALSE, goodRun;
  string str = "";
  Int_t numFile = 0, k = 0, kIn = 0, kOut = 0, runnum;
  Double_t wienSign;
  //arrays for HWP IN
  Double_t asymmIN[LEN], asymmErrIN[LEN], yldAbvBkgdIN[LEN], yldAbvBkgdErrIN[LEN];
  Double_t bkgdIN[LEN], bkgdErrIN[LEN], avgLasPowIN[LEN], scaLasOnIN[LEN],scaLasOffIN[LEN];
  Double_t meanPosXIN[LEN], meanPosYIN[LEN], cutNumIN[LEN], numEntIN[LEN], cntrIN[LEN];
  Double_t errorXIN[LEN];
  //arrays for HWP OUT
  Double_t asymmOUT[LEN], asymmErrOUT[LEN], yldAbvBkgdOUT[LEN], yldAbvBkgdErrOUT[LEN];
  Double_t bkgdOUT[LEN], bkgdErrOUT[LEN], avgLasPowOUT[LEN], scaLasOnOUT[LEN],scaLasOffOUT[LEN];
  Double_t meanPosXOUT[LEN], meanPosYOUT[LEN],cutNumOUT[LEN], numEntOUT[LEN], cntrOUT[LEN]; 
  Double_t errorXOUT[LEN];


  numFile = (Int_t)((Double_t)sizeof(runs)/(Double_t)sizeof(Int_t));

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

    while(atoi(run) < runStart)//advance until runStart position reached in runlist file
    {
      runlist>>run>>date>>time>>disc>>hv>>passive>>gain>>hWien>>vWien>>phi_FG>>hwp>>cur
	     >>goodFor;
      //comment field not read 
      //so advance until end of line reached
      while(runlist.good() && runlist.get()!='\n' && !runlist.eof()){}
    }

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

    //comment field not read 
    //so advance until end of line reached
    while(runlist.good() && runlist.get()!='\n' && !runlist.eof()){}


    if(atoi(goodFor)==1 || atoi(goodFor)==3) goodRun = kTRUE;
    else goodRun = kFALSE;

    ///////////////////////////////////////
    //Examples of cuts that might be used//
    ///////////////////////////////////////

    if(atoi(hv)==1900 && goodRun)goodRun = kTRUE;//cut on HV setting
    else goodRun = kFALSE;
    cout<<goodRun<<endl;
    /*
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

    ifstream file(Form("www/run_%i/accum0_%i_stats.txt",runnum, runnum));
    cout<<"file open = "<<file<<endl;

    if(file && goodRun)//if file is found and run passes cuts
    {
      //fix asymmetry sign from Wien angle changes
      if(atof(hWien)*atof(vWien)*atof(phi_FG) > 0) wienSign = -1.0;
      else wienSign = 1.0;

      printf("Using run %i. i=%i\n", runnum,i);
      getline(file, str);//skip first line
      if(strcmp(hwp, "OUT")==0)
      {
	while(file>>cutNumOUT[kOut]>>asymmOUT[kOut]>>asymmErrOUT[kOut]>>numEntOUT[kOut]
	      >>yldAbvBkgdOUT[kOut]>>yldAbvBkgdErrOUT[kOut]>>bkgdOUT[kOut]>>bkgdErrOUT[kOut]
	      >>avgLasPowOUT[kOut]>>scaLasOnOUT[kOut]>>scaLasOffOUT[kOut]>>meanPosXOUT[kOut]
	      >>meanPosYOUT[kOut])
	{
	  asymmOUT[kOut] *= 100 * wienSign;
	  asymmErrOUT[kOut] *= 100 * TMath::Power(numEntOUT[kOut],-0.5);
	  yldAbvBkgdErrOUT[kOut] *= TMath::Power(numEntOUT[kOut],-0.5);
	  bkgdErrOUT[kOut] *= TMath::Power(numEntOUT[kOut],-0.5);
	  if(debug)
	    {
	      cout<<"cutNum="<<cutNumOUT[kOut]<<", asymmOUT="<<asymmOUT[kOut]<<"+/-"
		  <<asymmErrOUT[kOut]<<"\n"<<"numEnt="<<numEntOUT[kOut]
		  <<"yldAbvBkgdOUT="<<yldAbvBkgdOUT[kOut]<<"+/-"<<yldAbvBkgdErrOUT[kOut]<<"\n"
		  <<"bkgdOUT="<<bkgdOUT[kOut]<<"+/-"<<bkgdErrOUT[kOut]<<"\n"
		  <<"avgLasPowOUT="<<avgLasPowOUT[kOut]<<"    scaLasOnOUT="<<scaLasOnOUT[kOut]
		  <<"    scaLasOffOUT="<<scaLasOffOUT[kOut]<<"\n"<<"meanPosXOUT="<<meanPosXOUT[kOut]
		  <<"    meanPosYOUT="<<meanPosYOUT[kOut]<<endl;
	    }
	  cntrOUT[kOut] = k;
	  errorXOUT[kOut] = 0;
	  kOut++;
	  k++;
	}
      }

      else if(strcmp(hwp,"IN")==0)
      {//for plotting purposes change IN values to positive
	while(file>>cutNumIN[kIn]>>asymmIN[kIn]>>asymmErrIN[kIn]>>numEntIN[kIn]
	      >>yldAbvBkgdIN[kIn]>>yldAbvBkgdErrIN[kIn]>>bkgdIN[kIn]>>bkgdErrIN[kIn]
	      >>avgLasPowIN[kIn]>>scaLasOnIN[kIn]>>scaLasOffIN[kIn]>>meanPosXIN[kIn]
	      >>meanPosYIN[kIn])
	{
	  asymmIN[kIn] *= -100 * wienSign;
	  asymmErrIN[kIn] *= 100 * TMath::Power(numEntIN[kIn],-0.5);
	  yldAbvBkgdErrIN[kIn] *= TMath::Power(numEntIN[kIn],-0.5);
	  bkgdErrIN[kIn] *= TMath::Power(numEntIN[kIn],-0.5);
	  if(debug)
	    {
	      cout<<"cutNum="<<cutNumIN[kIn]<<", asymmIN="<<asymmIN[kIn]<<"+/-"
		  <<asymmErrIN[kIn]<<"\n"<<"numEnt="<<numEntIN[kIn]
		  <<"yldAbvBkgdIN="<<yldAbvBkgdIN[kIn]<<"+/-"<<yldAbvBkgdErrIN[kIn]<<"\n"
		  <<"bkgdIN="<<bkgdIN[kIn]<<"+/-"<<bkgdErrIN[kIn]<<"\n"
		  <<"avgLasPowIN="<<avgLasPowIN[kIn]<<"    scaLasOnIN="<<scaLasOnIN[kIn]
		  <<"    scaLasOffIN="<<scaLasOffIN[kIn]<<"\n"<<"meanPosXIN="<<meanPosXIN[kIn]
		  <<"    meanPosYIN="<<meanPosYIN[kIn]<<endl;
	    }
	  cntrIN[kIn] = k;
	  errorXIN[kIn] = 0;
	  kIn++;
	  k++;
	}
      }
      else 
      {
	cout<<endl;
	cout<<"NO DATA FOUND for run "<<runnum<<endl;
	cout<<endl;
      }
      file.close();
    }
    else if(!file) 
      printf("Cannot find\"www/compton/photonsummary/run_%i/accum0_%i_stats.txt\"\n",
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


  }//end of for loop
  /*
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
  */
  gStyle->SetTitleFontSize(0.077);
  TCanvas *c1 = new TCanvas("c1","c1",0,0,1200,500);
  gStyle->SetTitleW(0.61); //title width
  //gStyle->SetTitleH(0.08); //title height
  //Graph of PD data with HWP OUT.
  TGraphErrors *gPDOut = new TGraphErrors(kOut, cntrOUT, asymmOUT, errorXOUT, asymmErrOUT);
  gPDOut->SetMarkerColor(kRed);
  gPDOut->SetLineColor(kRed);
  gPDOut->SetMarkerStyle(7);
  gPDOut->SetTitle("HWP OUT");
  //gStyle->SetOptFit(1111);
  //gr->Fit("pol0");

  //Graph of PD data with HWP IN.
  TGraphErrors *gPDIn = new TGraphErrors(kIn,cntrIN,asymmIN, errorXIN, asymmErrIN);
  gPDIn->SetMarkerColor(kBlue);
  gPDIn->SetLineColor(kBlue);
  gPDIn->SetMarkerStyle(7);
  gPDIn->SetTitle("HWP IN");

  TMultiGraph *mgPD = new TMultiGraph();
  mgPD->SetTitle("GSO Accum0  Asymmetry(%) vs Cut Number (HV=1900)");
  mgPD->Add(gPDIn);
  mgPD->Add(gPDOut);
  mgPD->Draw("AP");
  mgPD->GetXaxis()->SetTitle("Cut Number");
  mgPD->GetXaxis()->SetTitleSize(0.06);
  mgPD->GetXaxis()->SetTitleOffset(0.62);
  mgPD->GetYaxis()->SetTitleOffset(0.61);
  mgPD->GetYaxis()->SetTitle("Accum0 Asymmetry (%)");
  mgPD->GetYaxis()->SetTitleSize(0.07);
  gStyle->SetOptFit(1111);
  mgPD->Fit("pol0");
  mgPD->Draw("AP");

  c1->Update();
  TLegend *legend = new TLegend(0.87,0.67,0.98,0.835);
  //legend1->SetHeader("Some histograms");
  legend->AddEntry(gPDIn,"HWP IN","l");
  legend->AddEntry(gPDOut,"HWP OUT","l");
  legend->SetFillColor(0);
  legend->Draw();
  //c1->BuildLegend();
/*
  c1->cd(2);
  //Graph of scintillator laserwise background subtraction data with HWP OUT.
  TGraphErrors *gScOut = new TGraphErrors(kOut,cntrOUT,asymmScLwAvgOut,errorXOut,asymmScLwAvgErrOut);
  gScOut->SetMarkerColor(kRed);
  gScOut->SetLineColor(kRed);
  gScOut->SetMarkerStyle(7);
  gScOut->SetTitle("HWP OUT");
  //  gStyle->SetOptFit(1111);
  //gr1->Fit("pol0");

  //Graph of scintillator laserwise background subtraction data with HWP IN.
  TGraphErrors *gScIn = new TGraphErrors(kIn,cntrIN,asymmScLwAvgIn,errorXIn,asymmScLwAvgErrIn);
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
*/
  //save plot to file
  c1->Print(Form("~/users/jonesdc/compton_runs/accum0_Runs_%i_to_%i.png",runStart, runEnd));
 
  return 1;
}
