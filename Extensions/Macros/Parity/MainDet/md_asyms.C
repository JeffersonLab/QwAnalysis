#include <iostream>
#include <iomanip>  
#include <stdio.h>
#include <stdlib.h>


void mdAsym(int runNum){
gROOT->Reset();
gROOT->SetStyle("Plain");

Bool_t makeMDPed = kTRUE;
Bool_t makeBKGDPed = kTRUE;
Bool_t save_file = kTRUE;

const string cut1 = "mps_counter>500";
const string cut2 = "mps_counter<2400e6";

const string md[16] ={
"md1neg","md2neg","md3neg","md4neg",
"md5neg","md6neg","md7neg","md8neg",
"md1pos","md2pos","md3pos","md4pos",
"md5pos","md6pos","md7pos","md8pos"};

Char_t filename[100];
sprintf(filename,"$QW_ROOTFILES/Qweak_%i.root",runNum);
f = new TFile(filename);
if(!f->IsOpen()){
	std::cerr<<"Error opening ROOTFile "<<filename<<".\n"<<endl;
	return 0;
}
std::cout<<"Successfully opened ROOTFile "<<filename<<".\n"<<endl;

TTree *hel_tree = f->Get("Hel_Tree");

gStyle->SetOptStat("neMmRr");
gStyle->SetStatW(0.32);


TH1F *mdHist[16];
TCanvas *mc_md = new TCanvas("mc_md", "canvas_md",1400,1100);
mc_md->Divide(4,4);

  for (Int_t p=0; p < 16; p++)
    {           
      mdHist[p] = new TH1F(Form("%i_h_%s",runNum,md[p]),"",100,0,0);
      mc_md->cd(p+1);
      mdHist[p]->SetDirectory(0);     
      hel_tree->Draw(Form("asym_%s.hw_sum_raw*1e6>>run%i_h_%s",md[p],runNum,md[p]),Form("%s && %s",cut1,cut2));
      TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%i_h_%s",runNum,md[p]));
     }

} // end of macro


