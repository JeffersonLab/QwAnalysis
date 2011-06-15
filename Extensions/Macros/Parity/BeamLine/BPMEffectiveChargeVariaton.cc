//********************************************************************//
// Author : B. Waidyawansa (buddhini@jlab.org)
// Date   : December 15th, 2010
//********************************************************************//
/*
 This macro plots the bpm effective charge asymmetry along the beamline vs bpm.
 It has 3 inputs.
 1. run number
 2. Lower limit to asymmetry in ppm. Use 0 for no limit.
 3. Upper limit for asymmetry in ppm. Use 0 for no limit.

To use this macro,  load it in to root and do (e.g)
root [11] PlotBPM_effectiveq_asyms(8231,-200,200)


*/



//********************************************
// Main function
//********************************************


#include "TMath.h"
#include <vector>
#include "TRandom.h"
#include "TGraphErrors.h"
#include "TStyle.h"
#include <TVector.h>
#include <TApplication.h>
#include <math.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <new>
#include <TF1.h>
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TProfile.h>
#include <TString.h>
#include <stdexcept>
#include <TLine.h>
#include <time.h>
#include <stdio.h>
#include <TBox.h>
#include <TPaveText.h>
#include <TPaveLabel.h>
#include <TGraphErrors.h>  
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TPaveStats.h>
#include <TChain.h>
#include <cstdlib>


void PlotBPM_effectiveq_asyms(Int_t runnum, TString device, Int_t llimit, Int_t ulimit)
{
    
  const Int_t ndevices = 24;
  
  TString devicelist[ndevices]=
  {
    "bpm3c07","bpm3c07a","bpm3c08","bpm3c11","bpm3c12","bpm3c14","bpm3c16","bpm3c17",
    "bpm3c18","bpm3c19","bpm3p02a","bpm3p02b","bpm3p03a","bpm3c20","bpm3c21","bpm3h02",
    "bpm3h04","bpm3h07a","bpm3h07b","bpm3h07c","bpm3h08","bpm3h09","bpm3h09b","target"
  };


  gStyle->Reset();
  gStyle->SetOptTitle(1);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.4);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat(1110); 
  gStyle->SetOptStat(1); 


  // canvas parameters
  gStyle->SetFrameBorderMode(0);
  gStyle->SetFrameBorderSize(0);
  gStyle->SetFrameFillColor(10);

  // pads parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.2);
  gStyle->SetPadLeftMargin(0.07);  
  gStyle->SetPadRightMargin(0.05);  
  gStyle->SetLabelSize(0.07,"x");
  gStyle->SetLabelSize(0.04,"y");
  gStyle->SetPadGridX(kTRUE);
  gStyle->SetPadGridY(kTRUE);


  /*Open the rootfile/rootfiles*/
  TChain * tree      = new TChain("Hel_Tree");
  TString  Rfilename = Form("QwPass1_%i.000.root", runnum);
  TFile  * Rfile     = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));

  if (Rfile->IsZombie()) {
    std::cout << "Error opening root file chain "<< Rfilename << std::endl;
    Rfilename = Form("first100k_%i.root", runnum);
    std::cout << "Try to open chain " << Rfilename << std::endl;
    Rfile = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));

    if (Rfile->IsZombie()) {
      std::cout << "Error opening root file chain "<< Rfilename << std::endl;
      Rfilename = Form("Qweak_%i.000.root", runnum);
      std::cout << "Try to open chain " << Rfilename << std::endl;
      Rfile = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));
      
      if (Rfile->IsZombie()){
	std::cout << "File exit failure."<<std::endl; 
	tree = NULL;
      }
      else
	tree->Add(Form("$QW_ROOTFILES/Qweak_%i.000.root", runnum));
    }
    else
      tree->Add(Form("$QW_ROOTFILES/first100k_%i.root", runnum));
  }
  else
    tree->Add(Form("$QW_ROOTFILES/QwPass1_%i.*.root", runnum));
  
    std::cout<<"Opened rootfile/s "<<Rfilename<<std::endl;

  if(tree == NULL){
    std::cout<< "Unable to find the Mps_Tree." << std::endl; 
    exit(1);
  }

  
  TString ctitle = Form("Effective Charge Double Difference (w.r.t %s) Variation (%i < Aq < %i ppm) Across C Beam Line in run %i",device.Data(),llimit,ulimit,runnum);
  TCanvas *c = new TCanvas("c",ctitle,1200,600);


  c->SetFillColor(0);

  TPad * pad1 = new TPad("pad1","pad1",0.01,0.93,0.99,0.99);
  TPad * pad2 = new TPad("pad2","pad2",0.01,0.02,0.99,0.94);
  pad1->SetFillColor(11);
  pad1->Draw();
  pad2->Draw();

  pad1->cd();
  TText * t1 = new TText(0.02,0.3,ctitle);
  t1->SetTextSize(0.5);
  t1->Draw();
  pad2->Divide(1,2);

  TString command;
  TString cut;
  std::vector<TString>  names;
  TVectorD  mean(0);
  TVectorD  width(0);	
  TVectorD  meanerr(0);
  TVectorD  fakeerr(0);
  TVectorD  fakename(0);
  names.clear();

  Int_t k = 0;

  for(Int_t i=0; i<ndevices; i++) {  
      command = Form("(asym_qwk_%s_EffectiveCharge.hw_sum-asym_qwk_%s_EffectiveCharge.hw_sum)*1e6>>htemp",
		     device.Data(),devicelist[i].Data());
      cut =  Form("asym_qwk_%s_EffectiveCharge.Device_Error_Code == 0 && asym_qwk_%s_EffectiveCharge.Device_Error_Code == 0 && ErrorFlag == 0",
		   device.Data(),devicelist[i].Data());
      tree->Draw(command, cut, "goff");

      TH1 * h1 = (TH1D*)gDirectory->Get("htemp");
      if(!h1) exit(1);
      mean.ResizeTo(k+1);
      width.ResizeTo(k+1);	
      meanerr.ResizeTo(k+1);
      fakeerr.ResizeTo(k+1);
      fakename.ResizeTo(k+1);

      Double_t meanl = h1->GetMean();
	
	if((TMath::Abs(ulimit)>0) && (TMath::Abs(llimit)>0)){
	if((meanl > llimit) && (meanl < ulimit) ){
	  std::cout<<"Getting data from : "<<devicelist[i]<<std::endl;
	  mean.operator()(k) = meanl;
          width.operator()(k) = h1->GetRMS();
	  meanerr.operator()(k) = (h1->GetMeanError());
	  fakeerr.operator()(k) = (0.0);
	  fakename.operator()(k) =(k+1);
	  names.push_back(devicelist[i]);
	  k++;
	}
	else {
	  std::cout<<devicelist[i]<<" has an charge Aq out of the given limits"<<std::endl;
	  std::cout<<"Ingored this device. "<<std::endl;
	}
      }
      if(ulimit == 0 && llimit == 0){
	std::cout<<"Getting data from : "<<devicelist[i]<<std::endl;
	mean.operator()(i) = (h1->GetMean());
	meanerr.operator()(i) = (h1->GetMeanError());
	fakeerr.operator()(i) = (0.0);
	fakename.operator()(i) = (i+1);
	names.push_back(devicelist[i]);
	k++;
      }

      delete h1;
  }

  Int_t size = mean.GetNoElements();
  if(mean.Norm1() == 0  ) {
    std::cout<<"There are no data of the type requested in this run!"<<std::endl;
    exit(1);
  };

  TGraphErrors* grp = new TGraphErrors(fakename, mean, fakeerr,meanerr);
  grp->SetMarkerColor(2);
  grp->SetMarkerStyle(21);
  grp->SetMarkerSize(1);
  grp->SetTitle("");
  grp->GetYaxis()->SetTitle("Asymmetry in Double Difference (ppm)");
  grp->GetYaxis()->SetTitleOffset(0.9);

  TGraph* grw = new TGraph(fakename, width);
  grw->SetMarkerColor(2);
  grw->SetMarkerStyle(21);
  grw->SetMarkerSize(1);
  grw->SetTitle("");
  grw->GetYaxis()->SetTitle("Width (ppm)");
  grw->GetYaxis()->SetTitleOffset(0.9);

  pad2->cd(1);
  TAxis *axis = grp->GetHistogram()->GetXaxis();
  axis->SetLabelOffset(999);
  Double_t ypos =  grp->GetHistogram()->GetMaximum()-grp->GetHistogram()->GetMinimum();
  Double_t ylabel = grp->GetHistogram()->GetMinimum() - 0.01*ypos;
  grp->Draw("AEP");

  TText t;
  t.SetTextSize(0.04);
  t.SetTextAlign(31);
  t.SetTextAngle(90);
  for (Int_t i=0;i<size;i++){
    t.DrawText(fakename[i],ylabel,names[i]);
  }
  c->Modified();
  c->Update();

  pad2->cd(2);
  axis = grw->GetHistogram()->GetXaxis();
  axis->SetLabelOffset(999);
  ypos =  grw->GetHistogram()->GetMaximum()-grw->GetHistogram()->GetMinimum();
  ylabel = grp->GetHistogram()->GetMinimum() - 0.01*ypos;
  grw->Draw("APB");

  t.SetTextSize(0.04);
  t.SetTextAlign(31);
  t.SetTextAngle(90);
  for (Int_t i=0;i<size;i++){
    t.DrawText(fakename[i],ylabel,names[i]);
  }

  c->Modified();
  c->Update();
  
  // Save the canvas on to a .gif file
  c->Print(Form("%i_bpm_eff_q_double_diffs_%i_to_%i.gif",runnum,llimit,ulimit));
  std::cout<<"Done plotting!\n";
};




