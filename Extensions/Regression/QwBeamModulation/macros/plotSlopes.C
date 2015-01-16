#include <TChain.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TAxis.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"

//This program takes the lists of monitor and detector names
//and the arrays of detector to monitor slopes vs whatever
//x-variable you choose and plots them. 15 canvases are produced:
//5 MD canvases (one for each modulation type)
//5 Background detector and lumi  canvases (one for each modulation type)
//5 Charge canvases (one for each modulation type)
/////////////////////////////////////////////////////////////////


const Int_t N_MON = 5, N_DET = 35;

void plotSlopes(TString *MonitorList, TString *DetectorList, 
		Double_t ***DetectorSlopes, Double_t ***DetectorSlopesErr, 
		Double_t *x, Int_t size = 0, Int_t nDet = 0, char *xTitle = "",
		Double_t saveToFile = 0, Bool_t newMonitors = 0){

 
 if(size==0 || nDet==0){
    std::cout<<"Invalid size. Exiting.\n";
    return;
  }

  //Plot slopes arrays versus provided X domain
  /////////////////////////////////////////////
  TCanvas *cMD[N_MON];
  TCanvas *cBkg[N_MON];
  TCanvas *cChg[N_MON];

  TGraphErrors *gr[N_DET*N_MON];

  //Error in x variable is assumed to be 0
  Double_t *xErr;
  xErr = new Double_t[size]();

  int iplot = 0, idx = 0;
  for(int imon=0;imon<N_MON;imon++){
    int pad1[9] = {5,4,1,2,3,6,9,8,7}, pad2 = 1, pad3 = 1;
    cMD[iplot] = new TCanvas(Form("cMD_%i",iplot),
			     Form("cMD%s",MonitorList[imon].Data()),
			     0,0,1500,1000);
    cMD[iplot]->Divide(3,3);
    cBkg[iplot] = new TCanvas(Form("cBkg_%i",iplot),
			      Form("cBkg%s",MonitorList[imon].Data()),
			      0,0,2000,1000);
    cBkg[iplot]->Divide(4,4);
    cChg[iplot] = new TCanvas(Form("cCharge_%i",iplot),
			     Form("charge%s",MonitorList[imon].Data()),
			     0,0,1500,660);
    cChg[iplot]->Divide(3,2);
    for(int idet=0;idet<nDet;idet++){
      if(DetectorList[idet].Contains("md")&& !DetectorList[idet].Contains("9")){
	cMD[iplot]->cd(pad1[idet]);
      }else if(DetectorList[idet].Contains("lumi")||
	       DetectorList[idet].Contains("pmt")||
	       DetectorList[idet].Contains("md9")){
	cBkg[iplot]->cd(pad2);
	pad2++;
      }else{
	cChg[iplot]->cd(pad3);
	pad3++;
      }
      gr[idx] = new TGraphErrors(size,x,DetectorSlopes[idet][imon],
				 xErr,DetectorSlopesErr[idet][imon]);
      gr[idx]->Draw("ap");
      gr[idx]->SetTitle(Form("Slope %s / %s",
			     DetectorList[idet].Data(),
			     MonitorList[imon].Data()));
      gr[idx]->GetYaxis()->SetTitle(Form("%s/%s (%s)",
					 DetectorList[idet].Data(),
					 MonitorList[imon].Data(),
					 (MonitorList[imon].Contains("Slope")
					  ? "ppm/#murad":"ppm/#mum")));
      gr[idx]->GetXaxis()->SetTitle(xTitle);
      double size = 0.049;
      if(DetectorList[idet].Contains("pmt")){
	Double_t val[N_MON]={400,4000, 50, 140, 3000}; 
	cout<<"Resetting axis range of "<<DetectorList[idet].Data()<<"_"<<
			      MonitorList[imon].Data()<<"to +/-"<<val<<endl;
	gr[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);
      }
      gr[idx]->GetXaxis()->SetTitleSize(size);
      gr[idx]->GetYaxis()->SetTitleSize(size);
//       gr[idx]->GetXaxis()->SetTitleColor(col[idet]);
//       gr[idx]->GetYaxis()->SetTitleColor(col[idet]);
      gr[idx]->GetXaxis()->SetTitleOffset(0.85);
      gr[idx]->GetYaxis()->SetTitleOffset(0.85);
      gr[idx]->SetMarkerStyle(7);
      gr[idx]->SetMarkerColor(kRed);
      gr[idx]->SetLineColor(kRed);
      gr[idx]->Draw("ap");
      idx++;
    }
    iplot++;
  }

  gROOT->ForceStyle();
  if(saveToFile){
    for(int imon=0;imon<N_MON;imon++){
      cMD[imon]->SaveAs(Form("%s/plots/MD_slopes_to_%s_vs_%s.png",
			     (newMonitors ? "/net/data1/paschkedata1/bmod_out_new_monitors" : gSystem->Getenv("BMOD_OUT")),
			     MonitorList[imon].Data(), xTitle));
      cBkg[imon]->SaveAs(Form("%s/plots/Bkg_slopes_to_%s_vs_%s.png",
			      (newMonitors ? "/net/data1/paschkedata1/bmod_out_new_monitors" : gSystem->Getenv("BMOD_OUT")),
			      MonitorList[imon].Data(), xTitle));
      cChg[imon]->SaveAs(Form("%s/plots/Charge_slopes_to_%s_vs_%s.png",
			      (newMonitors ? "/net/data1/paschkedata1/bmod_out_new_monitors" : gSystem->Getenv("BMOD_OUT")),
			      MonitorList[imon].Data(), xTitle));
    }
  }
  return;
}


//Same as above except X errors provided.
void plotSlopes(TString *MonitorList, TString *DetectorList, 
		Double_t ***DetectorSlopes, Double_t ***DetectorSlopesErr, 
		Double_t *x, Double_t *xErr, Int_t size = 0, Int_t nDet = 0, 
		char *xTitle = "", Double_t saveToFile = 0, 
		Bool_t newMonitors = 0){

 
 if(size==0 || nDet==0){
    std::cout<<"Invalid size. Exiting.\n";
    return;
  }

  //Plot slopes arrays versus provided X domain
  /////////////////////////////////////////////
  TCanvas *cMD[N_MON];
  TCanvas *cBkg[N_MON];
  TCanvas *cChg[N_MON];

  TGraphErrors *gr[N_DET*N_MON];

  int iplot = 0, idx = 0;
  for(int imon=0;imon<N_MON;imon++){
    int pad1[9] = {5,4,1,2,3,6,9,8,7}, pad2 = 1, pad3 = 1;
    cMD[iplot] = new TCanvas(Form("cMD_%i",iplot),
			     Form("cMD%s",MonitorList[imon].Data()),
			     0,0,1500,1000);
    cMD[iplot]->Divide(3,3);
    cBkg[iplot] = new TCanvas(Form("cBkg_%i",iplot),
			      Form("cBkg%s",MonitorList[imon].Data()),
			      0,0,2000,1000);
    cBkg[iplot]->Divide(4,4);
    cChg[iplot] = new TCanvas(Form("cCharge_%i",iplot),
			     Form("charge%s",MonitorList[imon].Data()),
			     0,0,1500,660);
    cChg[iplot]->Divide(3,2);
    for(int idet=0;idet<nDet;idet++){
      if(DetectorList[idet].Contains("md")&& !DetectorList[idet].Contains("9")){
	cMD[iplot]->cd(pad1[idet]);
      }else if(DetectorList[idet].Contains("lumi")||
	       DetectorList[idet].Contains("pmt")||
	       DetectorList[idet].Contains("md9")){
	cBkg[iplot]->cd(pad2);
	pad2++;
      }else{
	cChg[iplot]->cd(pad3);
	pad3++;
      }
      gr[idx] = new TGraphErrors(size,x,DetectorSlopes[idet][imon],
				 xErr,DetectorSlopesErr[idet][imon]);
      gr[idx]->Draw("ap");
      gr[idx]->SetTitle(Form("Slope %s / %s",
			     DetectorList[idet].Data(),
			     MonitorList[imon].Data()));
      gr[idx]->GetYaxis()->SetTitle(Form("%s/%s (%s)",
					 DetectorList[idet].Data(),
					 MonitorList[imon].Data(),
					 (MonitorList[imon].Contains("Slope")
					  ? "ppm/#murad":"ppm/#mum")));
      gr[idx]->GetXaxis()->SetTitle(xTitle);
      double size = 0.049;
      if(DetectorList[idet].Contains("pmt")){
	Double_t val[N_MON]={400,4000, 50, 140, 3000}; 
	cout<<"Resetting axis range of "<<DetectorList[idet].Data()<<"_"<<
			      MonitorList[imon].Data()<<"to +/-"<<val<<endl;
	gr[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);
      }
      gr[idx]->GetXaxis()->SetTitleSize(size);
      gr[idx]->GetYaxis()->SetTitleSize(size);
//       gr[idx]->GetXaxis()->SetTitleColor(col[idet]);
//       gr[idx]->GetYaxis()->SetTitleColor(col[idet]);
      gr[idx]->GetXaxis()->SetTitleOffset(0.85);
      gr[idx]->GetYaxis()->SetTitleOffset(0.85);
      gr[idx]->SetMarkerStyle(7);
      gr[idx]->SetMarkerColor(kRed);
      gr[idx]->SetLineColor(kRed);
      gr[idx]->Draw("ap");
      idx++;
    }
    iplot++;
  }

  gROOT->ForceStyle();
  if(saveToFile){
    for(int imon=0;imon<N_MON;imon++){
      cMD[imon]->SaveAs(Form("%s/plots/MD_slopes_to_%s_vs_%s.png",
			     (newMonitors ? "/net/data1/paschkedata1/bmod_out_new_monitors" : gSystem->Getenv("BMOD_OUT")),
			     MonitorList[imon].Data(), xTitle));
      cBkg[imon]->SaveAs(Form("%s/plots/Bkg_slopes_to_%s_vs_%s.png",
			      (newMonitors ? "/net/data1/paschkedata1/bmod_out_new_monitors" : gSystem->Getenv("BMOD_OUT")),
			      MonitorList[imon].Data(), xTitle));
      cChg[imon]->SaveAs(Form("%s/plots/Charge_slopes_to_%s_vs_%s.png",
			      (newMonitors ? "/net/data1/paschkedata1/bmod_out_new_monitors" : gSystem->Getenv("BMOD_OUT")),
			      MonitorList[imon].Data(), xTitle));
    }
  }
  return;
}
