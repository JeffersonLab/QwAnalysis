{
gROOT->Reset();
gROOT->SetStyle("Plain");

//#include "/home/leacock/QwAnalysis/Extensions/GUI/QwGUIView/include/QwGUISuperCanvas.h";
//#include "/home/leacock/QwAnalysis/Extensions/GUI/QwGUIView/include/QwColor.h";
#include <iostream>
#include <iomanip>  
#include <stdio.h>
#include <stdlib.h>

Bool_t view_ds = kTRUE;
Bool_t view_us = kTRUE;

Bool_t save_file = kTRUE;

const string run_num = "5615";
Int_t run_segments = 1;

const string cut1 = "mps_counter>500";
const string cut2 = "mps_counter<2400e6";
/*
const string US[16] = 
{
"","uslumi3neg","uslumi3pos","",
"uslumi1pos","","","uslumi5neg",
"uslumi1neg","","","uslumi5pos",
"","uslumi7pos","uslumi7neg",""};

const string DS[9] = 
{
"dslumi2","dslumi3","dslumi4",
"dslumi1","","dslumi5",
"dslumi8","dslumi7","dslumi6"};
*/

const string US[8] =
{
"uslumi1neg","uslumi1pos","uslumi3neg",
"uslumi3pos","uslumi5neg","uslumi5pos",
"uslumi7neg","uslumi7pos"};

const string DS[8] =
{
"dslumi1","dslumi2","dslumi3",
"dslumi4","dslumi5","dslumi6",
"dslumi7","dslumi8"};

TChain chain("Mps_Tree");
for (Int_t i=0; i<run_segments; i++)
{
  chain->Add(Form("$QWSCRATCH/rootfiles/Qweak_%s.00%i.root",run_num,i));
  cout << "loaded run seg # " << i << endl;
}


gStyle->SetOptStat("neMmRr");
gStyle->SetStatW(0.32);
//gStyle->SetOptStat("0");

char * pPath;
pPath = getenv("QWANALYSIS");
//if (pPath!=NULL) printf ("The current save path is: %s",pPath);

ofstream lumi_ped_file;
lumi_ped_file.open(Form("%s/Parity/prminput/qweak_lumi_pedestal.%s-.map",pPath,run_num));

if (view_ds == kTRUE)
{

TH1F *dshst[8];
TCanvas *mc_ds = new TCanvas("mc_ds", "canvas_ds",1400,1100);
//QwGUISuperCanvas *mc_ds = new QwGUISuperCanvas("mc_ds", 10,10, 0);
mc_ds->Divide(3,3);

  for (Int_t p=0; p < 8; p++)
    {           
      dshst[p] = new TH1F(Form("%s_h_%s",run_num,DS[p]),"",100,0,0);
      mc_ds->cd(p+1);
      dshst[p]->SetDirectory(0);     
//      if (p==4) continue;
      chain->Draw(Form("qwk_%s.hw_sum_raw/qwk_%s.num_samples>>run%s_h_%s",DS[p],DS[p],run_num,DS[p]),Form("%s && %s",cut1,cut2)); 
   
      TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%s_h_%s",run_num,DS[p]));  
    
      if (save_file == kTRUE)
        {      
          if (p==0)
            {
              lumi_ped_file<<endl;
              lumi_ped_file<<Form("!The following pedestals were recorded from RUN %s",run_num)<<endl;
              lumi_ped_file<<"!channel name , Mps channelname.hw_sum_raw/num_samples , gain"<<endl;
            }
          lumi_ped_file<<Form("qwk_%s  ,  %f , %f\n",DS[p],htemp->GetMean(),(20./(1<<18)));
        }
     }
}


if (view_us == kTRUE)
{
TH1F *ushst[8];
TCanvas *mc_us = new TCanvas("mc_us", "canvas_us",1400,1100);
//QwGUISuperCanvas *mc_us = new QwGUISuperCanvas("mc_us", 10,10, 1);
mc_us->Divide(3,3);
lumi_ped_file<<endl;
  for (Int_t p=0; p < 8; p++)
    {           
      ushst[p] = new TH1F(Form("%s_h_%s",run_num,US[p]),"",100,0,0);
      mc_us->cd(p+1);
      ushst[p]->SetDirectory(0);     
      //if (p==0 || p==3 || p==5 || p==6 || p==9 || p==10 || p==12 || p==15) continue;
      chain->Draw(Form("qwk_%s.hw_sum_raw/qwk_%s.num_samples>>run%s_h_%s",US[p],US[p],run_num,US[p]),Form("%s && %s",cut1,cut2)); 

      TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%s_h_%s",run_num,US[p]));               
           
      if (save_file == kTRUE)
        {
          lumi_ped_file<<Form("qwk_%s  ,  ",US[p])<<htemp->GetMean()<<"  ,  1.0"<<endl;
        }
    } // end of for statement
lumi_ped_file<<endl;
} // end of view_us
lumi_ped_file.close();


} // end of macro


