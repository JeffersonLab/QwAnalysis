#include <iostream>
#include <iomanip>  
#include <stdio.h>
#include <stdlib.h>

/*
This script runs on first100k_ files and produces maindet asymmetry plots for combintations by averaging the asymmetries instead
of using the combinationpmt objects that summed the yields.
*/
void averageAsyms(int runNum){
gROOT->Reset();
gROOT->SetStyle("Plain");

Bool_t save_file = kTRUE;

const string cut1 = "mps_counter>1e3";
const string cut2 = "mps_counter<500e3";

const TString md[15] ={
  "(1e6/2)*(asym_qwk_md1pos+asym_qwk_md1neg)",
  "(1e6/2)*(asym_qwk_md2pos+asym_qwk_md2neg)",
  "(1e6/2)*(asym_qwk_md3pos+asym_qwk_md3neg)",
  "(1e6/2)*(asym_qwk_md4pos+asym_qwk_md4neg)",
  "(1e6/2)*(asym_qwk_md5pos+asym_qwk_md5neg)",
  "(1e6/2)*(asym_qwk_md6pos+asym_qwk_md6neg)",
  "(1e6/2)*(asym_qwk_md7pos+asym_qwk_md7neg)",
  "(1e6/2)*(asym_qwk_md8pos+asym_qwk_md8neg)",
  "(1e6/4)*(asym_qwk_md1pos+asym_qwk_md1neg+asym_qwk_md5neg+asym_qwk_md5pos)",
  "(1e6/4)*(asym_qwk_md2pos+asym_qwk_md2neg+asym_qwk_md6neg+asym_qwk_md6pos)",
  "(1e6/4)*(asym_qwk_md3pos+asym_qwk_md3neg+asym_qwk_md7neg+asym_qwk_md7pos)",  
  "(1e6/4)*(asym_qwk_md4pos+asym_qwk_md4neg+asym_qwk_md8neg+asym_qwk_md8pos)",
  "(1e6/8)*(asym_qwk_md1pos+asym_qwk_md1neg+asym_qwk_md5neg+asym_qwk_md5pos+asym_qwk_md3pos+asym_qwk_md3neg+asym_qwk_md7neg+asym_qwk_md7pos)",
  "(1e6/8)*(asym_qwk_md2pos+asym_qwk_md2neg+asym_qwk_md4neg+asym_qwk_md4pos+asym_qwk_md6pos+asym_qwk_md6neg+asym_qwk_md8neg+asym_qwk_md8pos)",
  "(1e6/16)*(asym_qwk_md1pos+asym_qwk_md1neg+asym_qwk_md2neg+asym_qwk_md2pos+asym_qwk_md3pos+asym_qwk_md3neg+asym_qwk_md4neg+asym_qwk_md4pos+asym_qwk_md5pos+asym_qwk_md5neg+asym_qwk_md6neg+asym_qwk_md6pos+asym_qwk_md7pos+asym_qwk_md7neg+asym_qwk_md8neg+asym_qwk_md8pos)"};

/*
const string md[8] ={
"md1barsum","md2barsum","md3barsum","md4barsum",
"md5barsum","md6barsum","md7barsum","md8barsum"}
*/

Char_t filename[100];

sprintf(filename,"$QW_ROOTFILES/first100k_%i.root",runNum);
f = new TFile(filename);
if(!f->IsOpen()){
	std::cerr<<"Error opening ROOTFile "<<filename<<".\n"<<endl;
	return 0;
}
std::cout<<"Successfully opened ROOTFile "<<filename<<".\n"<<endl;

TTree *hel_tree = f->Get("Hel_Tree");

gStyle->SetOptStat("neMmRr");
gStyle->SetStatW(0.32);


TH1F *mdHist[15];
TCanvas *mc_md = new TCanvas("mc_md", "canvas_md",1400,1100);
mc_md->Divide(4,4);

  for (Int_t p=0; p < 15; p++)
    {           
      mdHist[p] = new TH1F(Form("%i_h_%i",runNum,md[p].Data()),"",100,0,0);
      mc_md->cd(p+1);
      mdHist[p]->SetDirectory(0);     
      hel_tree->Draw(Form("%s>>run%i_h_%i",md[p].Data(),runNum,md[p].Data()),"asym_qwk_mdallbars.Device_Error_Code==0&&ErrorFlag==0");
      TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%i_h_%i",runNum,md[p].Data()));
      std::cout<<htemp->GetMean()<<" "<<htemp->GetRMS()<<std::endl;

     }

} // end of macro
