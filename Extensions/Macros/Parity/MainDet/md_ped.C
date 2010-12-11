#include <iostream>
#include <iomanip>  
#include <stdio.h>
#include <stdlib.h>


void mdPed(int runNum){
gROOT->Reset();
gROOT->SetStyle("Plain");

Bool_t makeMDPed = kTRUE;
Bool_t makeBKGDPed = kTRUE;
Bool_t save_file = kTRUE;

const string cut1 = "mps_counter>500";
const string cut2 = "mps_counter<2400e6";

const string md[16] ={
"qwk_md1neg","qwk_md2neg","qwk_md3neg","qwk_md4neg",
"qwk_md5neg","qwk_md6neg","qwk_md7neg","qwk_md8neg",
"qwk_md1pos","qwk_md2pos","qwk_md3pos","qwk_md4pos",
"qwk_md5pos","qwk_md6pos","qwk_md7pos","qwk_md8pos"};

const string mdbkgd[8] ={
"qwk_pmtled","qwk_pmtonl","qwk_pmtltg","qwk_md9neg",
"qwk_md9pos","qwk_isourc","qwk_preamp","qwk_cagesr"};

Char_t filename[100];
sprintf(filename,"$QW_ROOTFILES/mdPed_%i.root",runNum);
f = new TFile(filename);
if(!f->IsOpen()){
	std::cerr<<"Error opening ROOTFile "<<filename<<".\n"<<endl;
	return 0;
}
std::cout<<"Successfully opened ROOTFile "<<filename<<".\n"<<endl;

TTree *mps_tree = f->Get("Mps_Tree");

gStyle->SetOptStat("neMmRr");
gStyle->SetStatW(0.32);

char * pPath;
pPath = getenv("QWANALYSIS");

ofstream md_ped_file;
md_ped_file.open(Form("%s/Parity/prminput/qweak_maindet_pedestal.%i-.map",pPath,runNum));
std::cout<<"Pedestal file "<<Form("%s/Parity/prminput/qweak_maindet_pedestal.%i-.map",pPath,runNum)<<" opened.\n"<<endl;

if (makeMDPed == kTRUE)
{
TH1F *mdHist[16];
TCanvas *mc_md = new TCanvas("mc_md", "canvas_md",1400,1100);
mc_md->Divide(4,4);

  for (Int_t p=0; p < 16; p++)
    {           
      mdHist[p] = new TH1F(Form("%i_h_%s",runNum,md[p]),"",100,0,0);
      mc_md->cd(p+1);
      mdHist[p]->SetDirectory(0);     
      mps_tree->Draw(Form("%s.hw_sum_raw/%s.num_samples>>run%i_h_%s",md[p],md[p],runNum,md[p]),Form("%s && %s",cut1,cut2));
      TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%i_h_%s",runNum,md[p]));
    
      if (save_file == kTRUE)
        {      
          if (p==0)
            {
              md_ped_file<<endl;
              md_ped_file<<Form("!The following pedestals were recorded from RUN %i",runNum)<<endl;
              md_ped_file<<"!channel name , Mps channelname.hw_sum_raw/num_samples , gain"<<endl;
            }
          md_ped_file<<Form("%s  ,  ",md[p])<<htemp->GetMean()<<"  ,  0.00007629"<<endl;
        }
     }
}


if (makeBKGDPed == kTRUE)
{
TH1F *mkgdHist[8];
TCanvas *mc_bkgd = new TCanvas("mc_bkgd", "canvas_bkgd",1400,1100);
mc_bkgd->Divide(3,3);
md_ped_file<<endl;
  for (Int_t p=0; p < 8; p++)
    {           
      mkgdHist[p] = new TH1F(Form("%i_h_%s",runNum,mdbkgd[p]),"",100,0,0);
      mc_bkgd->cd(p+1);
      mkgdHist[p]->SetDirectory(0);     
      mps_tree->Draw(Form("%s.hw_sum_raw/%s.num_samples>>run%i_h_%s",mdbkgd[p],mdbkgd[p],runNum,mdbkgd[p]),Form("%s && %s",cut1,cut2));
      TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%i_h_%s",runNum,mdbkgd[p]));
           
      if (save_file == kTRUE)
        {
          md_ped_file<<Form("%s  ,  ",mdbkgd[p])<<htemp->GetMean()<<"  ,  0.00007629"<<endl;
        }
    } // end of for statement
md_ped_file<<endl;
} // end of view_us
md_ped_file.close();


} // end of macro


