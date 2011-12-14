//********************************************************************//
// Author : B. Waidyawansa
// Date   : Saturday 5th o, November 2011
//********************************************************************//
//
// This script can be used to plot data from rootfiles analyzed using the vqwk_check configuration 
// of the analyzer. It draws the signal distribution in each adc channel after converting it to a
// voltage using the conversion factor 76.29 muV/sample. All the cavases are saved in to *ong files.
// 
// e.g.
// root[0] .L vqwk_channel_check.C
// root[1]  vqwk_channel_check_on(13226,1)
// 

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <TStyle.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TText.h>
#include <TFile.h>
#include <TROOT.h>
#include <TH1.h>

// Number of VQWK modules in each roc as of 11/04/2011
const int MOD_0 =  2;
const int MOD_1 = 12;
const int MOD_2 =  8;

// Serial numbers of the modules in each roc as of 11/04/2011
//roc0(TS)
Int_t roc0sn[MOD_0]={24,14};
//roc1
Int_t roc1sn[MOD_1]={26,22,8,37,32,04,31,20,30,29,1,5};
//roc2
Int_t roc2sn[MOD_2]={33,13,15,36,35,18,3,6};


void vqwk_channel_check_on( int run_number,int roc){

  gStyle->Reset();
  gStyle->SetOptTitle(0);
  gStyle->SetOptFit(1111);
  gStyle->SetStatColor(10);  
  gStyle->SetStatH(0.3);
  gStyle->SetStatW(0.3);     
  gStyle->SetOptStat(0); 

  //Pad parameters
  gStyle->SetPadColor(0); 
  gStyle->SetPadBorderMode(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetPadBorderSize(0);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.18);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.05);



  gStyle->SetTitleYOffset(1.2);
  gStyle->SetTitleXOffset(0.08);
  gStyle->SetLabelSize(0.08,"x");
  gStyle->SetLabelSize(0.08,"y");
  gStyle->SetTitleSize(0.08,"x");
  gStyle->SetTitleSize(0.08,"y");
  gStyle->SetTitleX(0.01);
  gStyle->SetTitleW(0.7);
  gStyle->SetTitleBorderSize(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetTitleFontSize(0.09);
  gDirectory->Delete("*");

  Int_t * mod_array;
  Int_t mod_count;
  TString plotcommand;
  TString ch_name;

  if(roc == 0) {
    mod_array = roc0sn;
    mod_count = MOD_0;
  }
  if(roc == 1) {
    mod_array = roc1sn; 
    mod_count = MOD_1;
  }
  if(roc == 2) {
    mod_array = roc2sn;
    mod_count = MOD_2;
  }

 /*Open the rootfile/rootfiles*/
  TChain * tree      = new TChain("Mps_Tree");
  TString  Rfilename = Form("vqwk_check_%i.000.trees.root", run_number);
  TFile  * Rfile     = new TFile(Form("$QW_ROOTFILES/%s", Rfilename.Data()));
      
  if (Rfile->IsZombie()){
    std::cout << "File exit failure."<<std::endl; 
    tree = NULL;
  }
  else
    tree->Add(Form("$QW_ROOTFILES/vqwk_check_%i.*.trees.root", run_number));

  std::cout<<"Opened rootfile/s "<<Rfilename<<std::endl;

  if(tree == NULL){
    std::cout<< "Unable to find the Mps_Tree." << std::endl; 
    exit(1);
  }

  TCanvas* c = new TCanvas("c","vqwk_channels",1500,800);
  c->Draw();
    
  TPad*pad1 = new TPad("pad1","pad1",0.005,0.935,0.995,0.995);
  TPad*pad2 = new TPad("pad2","pad2",0.005,0.005,0.995,0.945);
  pad1->SetFillColor(23);
  pad1->Draw();
  pad2->Draw();
  
  pad1->cd();
  TString text = Form("VQWK channel readings from roc %i in run %i",roc,run_number);
  TText*t1 = new TText(0.1,0.3,text);
  t1->SetTextSize(0.6);
  t1->Draw();
  
  pad2->cd();
  pad2->Divide(4,4); //single plot, 16 channels (2 modules)

  Int_t j=0;
  Int_t m=0;
  for(Int_t i =0;i<mod_count;i++){

    for(Int_t k=0;k<8;k++){
      pad2->cd(j*8+k+1);
      gPad->SetLogy(); 
      std::cout<<mod_array[i]<<" ch "<<k<<std::endl;
      if(mod_array[i]<10)
	ch_name = Form("qwk_roc%isn0%ich%i",roc,mod_array[i],k);  
      else
	ch_name = Form("qwk_roc%isn%ich%i",roc,mod_array[i],k);  

      plotcommand = Form("%s.hw_sum_raw*76.29e-6/%s.num_samples",ch_name.Data(),ch_name.Data());  
      tree->Draw(plotcommand,"mps_counter>50");  
      TH1D* h = (TH1D*)gPad->FindObject("htemp");
      if(h ==NULL) continue;
      h->Fit("gaus");
      h->GetXaxis()->SetTitleOffset(1.0);
      h->GetYaxis()->SetTitleOffset(1.0);
      h->GetXaxis()->SetTitle(Form("%s (V)",ch_name.Data()));
      h->GetXaxis()->SetNdivisions(506);
      gPad->Update();
    }
    j++;
    if(j==2){
      c->Print(Form("%i_vqwk_responce_in_roc_%i_plot_%i.png",run_number,roc,m));
      m++;
      j=0;
    }
  }
	       

}
