#include <TChain.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include "TSystem.h"
#include "TCanvas.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TAxis.h"
#include "TFile.h"
#include "TROOT.h"
#include "TTree.h"
#include "TChain.h"
#include "TStyle.h"

//This program takes the lists of monitor and detector names
//and the arrays of detector to monitor slopes vs whatever
//x-variable you choose and plots them. 15 canvases are produced:
//5 MD canvases (one for each modulation type)
//5 Background detector and lumi  canvases (one for each modulation type)
//5 Charge canvases (one for each modulation type)
/////////////////////////////////////////////////////////////////


const Int_t NMON = 5, NDET = 35;

//Same as above except X errors provided.
void plotSlopesMultiGraph(TString *MonitorList, TString *DetectorList, 
			  TString *legend, vector<Double_t>& vX, 
			  vector<Double_t>& vXErr, 
			  vector<vector<vector<Double_t > > > & vSlopes1,
			  vector<vector<vector<Double_t > > > & vSlopesErr1,
			  vector<vector<vector<Double_t > > > & vSlopes2,
			  vector<vector<vector<Double_t > > > & vSlopesErr2,
			  vector<vector<vector<Double_t > > > & vSlopes3,
			  vector<vector<vector<Double_t > > > & vSlopesErr3,
			  char *xTitle = "", Int_t nDet = 30, 
			  Double_t saveToFile = 0){

 
  //  gStyle->SetMarkerStyle(7);
  if(vSlopes1[0][0].size()==0 || vSlopes2[0][0].size()==0 || 
     vSlopes3[0][0].size()==0){
    std::cout<<"Invalid size. Exiting.\n";
    return;
  }

  //Plot slopes arrays versus provided X domain
  /////////////////////////////////////////////
  TCanvas *cMD[NMON];
  TCanvas *cBkg[NMON];
  TCanvas *cChg[NMON];

  TMultiGraph *mg[NDET*NMON];

  TGraphErrors *gr1[NDET*NMON];
  TGraphErrors *gr2[NDET*NMON];
  TGraphErrors *gr3[NDET*NMON];

  int iplot = 0, idx = 0;
  for(int imon=0;imon<NMON;imon++){
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
      Int_t size = (Int_t)vSlopes1[idet][imon].size();
      Double_t *x = vX.data();
      Double_t *xErr = vXErr.data();
      Double_t *slopes1 = vSlopes1[idet][imon].data();
      Double_t *slopes2 = vSlopes2[idet][imon].data();
      Double_t *slopes3 = vSlopes3[idet][imon].data();
      Double_t *slopesErr1 = vSlopesErr1[idet][imon].data();
      Double_t *slopesErr2 = vSlopesErr2[idet][imon].data();
      Double_t *slopesErr3 = vSlopesErr3[idet][imon].data();
      gr1[idx] = new TGraphErrors(size,x,slopes1,xErr,slopesErr1);
      gr1[idx]->SetMarkerColor(kRed);
      gr1[idx]->SetLineColor(kRed);
      gr1[idx]->SetName(legend[0].Data());
      gr2[idx] = new TGraphErrors(size,x,slopes2,xErr,slopesErr2);
      gr2[idx]->SetMarkerColor(kBlue);
      gr2[idx]->SetLineColor(kBlue);
      gr2[idx]->SetName(legend[1].Data());
      gr3[idx] = new TGraphErrors(size,x,slopes3,xErr,slopesErr3);
      gr3[idx]->SetName(legend[2].Data());

      mg[idx] = new TMultiGraph(Form("mg%i",idx),Form("mg%i",idx));

      mg[idx]->Add(gr1[idx]);
      mg[idx]->Add(gr2[idx]);
      mg[idx]->Add(gr3[idx]);
      mg[idx]->Draw("ap");
      mg[idx]->SetTitle(Form("Slope %s / %s",
			     DetectorList[idet].Data(),
			     MonitorList[imon].Data()));
      mg[idx]->GetYaxis()->SetTitle(Form("%s/%s (%s)",
					 DetectorList[idet].Data(),
					 MonitorList[imon].Data(),
					 (MonitorList[imon].Contains("Slope")
					  ? "ppm/#murad":"ppm/#mum")));
      mg[idx]->GetXaxis()->SetTitle(xTitle);
      double font_size = 0.049;
      if(DetectorList[idet].Contains("pmt")){
	Double_t val[NMON]={400,4000, 50, 140, 3000}; 
	cout<<"Resetting axis range of "<<DetectorList[idet].Data()<<"_"<<
			      MonitorList[imon].Data()<<"to +/-"<<val<<endl;
	mg[idx]->GetYaxis()->SetRangeUser(-1.0*val[imon], val[imon]);
      }
      mg[idx]->GetXaxis()->SetTitleSize(font_size);
      mg[idx]->GetYaxis()->SetTitleSize(font_size);
      mg[idx]->GetXaxis()->SetTitleOffset(0.85);
      mg[idx]->GetYaxis()->SetTitleOffset(0.85);
      mg[idx]->Draw("ap");
      idx++;
    }
    iplot++;
  }


  if(saveToFile){
    for(int imon=0;imon<NMON;imon++){
      cMD[imon]->SaveAs(Form("%s/plots/MD_slopes_to_%s_vs_%s.png",
			     gSystem->Getenv("BMOD_OUT"),
			     MonitorList[imon].Data(), xTitle));
      cBkg[imon]->SaveAs(Form("%s/plots/Bkg_slopes_to_%s_vs_%s.png",
			      gSystem->Getenv("BMOD_OUT"),
			      MonitorList[imon].Data(), xTitle));
      cChg[imon]->SaveAs(Form("%s/plots/Charge_slopes_to_%s_vs_%s.png",
			      gSystem->Getenv("BMOD_OUT"),
			      MonitorList[imon].Data(), xTitle));
    }
  }
  return;
}
