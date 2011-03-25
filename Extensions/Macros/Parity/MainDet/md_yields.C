#include <iostream>
#include <iomanip>  
#include <stdio.h>
#include <stdlib.h>


void md_yields(int runNum){
gROOT->Reset();
gROOT->SetStyle("Plain");

Bool_t save_file = kTRUE;

const string cut1 = "mps_counter<100000";
const string cut2 = "mps_counter>1000";
//const string cut2 = "mps_counter>0";
//const string cut3 = "qwk_md1pos>0.005 && qwk_md1neg>0.005";
double mean[16],rms[16];

 const TString md[16] ={
   "qwk_md1neg","qwk_md2neg","qwk_md3neg","qwk_md4neg",
   "qwk_md5neg","qwk_md6neg","qwk_md7neg","qwk_md8neg",
   "qwk_md1pos","qwk_md2pos","qwk_md3pos","qwk_md4pos",
   "qwk_md5pos","qwk_md6pos","qwk_md7pos","qwk_md8pos"};
  
 Char_t filename[100];
 sprintf(filename,"$QW_ROOTFILES/first100k_%i.root",runNum);
 f = new TFile(filename);
 if(!f->IsOpen()){
   std::cerr<<"Error opening ROOTFile "<<filename<<".\n"<<endl;
   return 0;
 }
 std::cout<<"Successfully opened ROOTFile "<<filename<<".\n"<<endl;
 
 TTree *mps_tree = f->Get("Mps_Tree");
  
 gStyle->SetOptStat("neMmRr");
 gStyle->SetStatW(0.32);
 
 
 TH1F *mdHist[16];
 TCanvas *mc_md = new TCanvas("mc_md", "canvas_md",1400,1100);
 mc_md->Divide(4,4);
 
 for (Int_t p=0; p < 16; p++)
   {           
      mdHist[p] = new TH1F(Form("%i_h_%s",runNum,md[p].Data()),"",100,0,0);
      mc_md->cd(p+1);
      mdHist[p]->SetDirectory(0);     
      mps_tree->Draw(Form("%s.hw_sum>>run%i_h_%s",md[p].Data(),runNum,md[p].Data()),Form("%s && %s && %s.Device_Error_Code==0",cut1,cut2,md[p].Data()));
      TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%i_h_%s",runNum,md[p].Data()));
      mean[p]=htemp->GetMean(); rms[p]=htemp->GetRMS();
      std::cout<<md[p].Data()<<": "<<htemp->GetMean()<<" "<<htemp->GetRMS()<<std::endl;
   }
  
} // end of macro


