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

Bool_t view_mps = kFALSE;
//Float_t scale_mps = 1;
Float_t scale_mps = 1e-3/60;
Bool_t save_file = kFALSE;

const string run_num = "5615";
Int_t run_segments = 1;

const string cut1 = "mps_counter>500";
const string cut2 = "mps_counter<2400e6";

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

TChain chain("Hel_Tree");
for (Int_t i=0; i<run_segments; i++)
{
  chain->Add(Form("$QWSCRATCH/rootfiles/Qweak_%s.00%i.root",run_num,i));
  cout << "loaded run seg # " << i << endl;
}


gStyle->SetOptStat("neMmRr");
gStyle->SetStatW(0.32);
//gStyle->SetOptStat("0");

/*
Mps_Tree->Draw("qwk_dslumi1.block0:mps_counter","mps_counter>500&& mps_counter<506","*")
Mps_Tree->Draw("qwk_dslumi1.block1:mps_counter+.25","mps_counter>500&& mps_counter<506","*same") 
Mps_Tree->Draw("qwk_dslumi1.block2:mps_counter+.5","mps_counter>500&& mps_counter<506","*same") 
Mps_Tree->Draw("qwk_dslumi1.block3:mps_counter+.75","mps_counter>500&& mps_counter<506","*same")
*/



if (view_ds == kTRUE)
{

TH1F *dshst[9];
TCanvas *mc_ds = new TCanvas("mc_ds", "canvas_ds",1400,1100);
//QwGUISuperCanvas *mc_ds = new QwGUISuperCanvas("mc_ds", 10,10, 0);
mc_ds->Divide(3,3);

  for (Int_t p=0; p < 9; p++)
    {           
      dshst[p] = new TH1F(Form("%s_h_%s",run_num,DS[p]),"",100,0,0);
      mc_ds->cd(p+1);
      dshst[p]->SetDirectory(0);     
      if (p==4) continue;
      if (view_mps == kTRUE)
        {
          chain->Draw(Form("asym_qwk_%s*1e6:mps_counter*%f>>run%s_h_%s",DS[p],scale_mps,run_num,DS[p]),Form("%s && %s",cut1,cut2),""); 
        }
      else
        { 
          chain->Draw(Form("asym_qwk_%s*1e6>>run%s_h_%s",DS[p],run_num,DS[p]),Form("%s && %s",cut1,cut2)); 
        }
      //TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%s_h_%s",run_num,DS[p]));               
      //cout<<htemp->GetRMS()<<endl;
      
    }
if (save_file == kTRUE && view_mps == kTRUE)
  {
    mc_ds->SaveAs(Form("$QWSCRATCH/rootfiles/run%s_asym_dslumis_mps.png",run_num));
    mc_ds->SaveAs(Form("$QWSCRATCH/rootfiles/run%s_asym_dslumis_mps.ps",run_num));
  }
if (save_file == kTRUE && view_mps == kFALSE)
  {
    mc_ds->SaveAs(Form("$QWSCRATCH/rootfiles/run%s_asym_dslumis.png",run_num));
    mc_ds->SaveAs(Form("$QWSCRATCH/rootfiles/run%s_asym_dslumis.ps",run_num));
  }




}


if (view_us == kTRUE)
{
TH1F *ushst[16];
TCanvas *mc_us = new TCanvas("mc_us", "canvas_us",1400,1100);
//QwGUISuperCanvas *mc_us = new QwGUISuperCanvas("mc_us", 10,10, 1);
mc_us->Divide(4,4);

  for (Int_t p=0; p < 16; p++)
    {           
      ushst[p] = new TH1F(Form("%s_h_%s",run_num,US[p]),"",100,0,0);
      mc_us->cd(p+1);
      ushst[p]->SetDirectory(0);     
      if (p==0 || p==3 || p==5 || p==6 || p==9 || p==10 || p==12 || p==15) continue;
      if (view_mps == kTRUE)
        {
          chain->Draw(Form("asym_qwk_%s*1e6:mps_counter*%f>>run%s_h_%s",US[p],scale_mps,run_num,US[p]),Form("%s && %s",cut1,cut2),""); 
        }
      else
        {
          chain->Draw(Form("asym_qwk_%s*1e6>>run%s_h_%s",US[p],run_num,US[p]),Form("%s && %s",cut1,cut2)); 
        }
      //TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%s_h_%s",run_num,US[p]));               
      //cout<<htemp->GetRMS()<<endl;
      
    }  
if (save_file == kTRUE && view_mps == kTRUE)
  {
    mc_us->SaveAs(Form("$QWSCRATCH/rootfiles/run%s_asym_uslumis_mps.png",run_num));
    mc_us->SaveAs(Form("$QWSCRATCH/rootfiles/run%s_asym_uslumis_mps.ps",run_num));
  }
if (save_file == kTRUE && view_mps == kFALSE)
  {
    mc_us->SaveAs(Form("$QWSCRATCH/rootfiles/run%s_asym_uslumis.png",run_num));
    mc_us->SaveAs(Form("$QWSCRATCH/rootfiles/run%s_asym_uslumis.ps",run_num));
  }




}

}


