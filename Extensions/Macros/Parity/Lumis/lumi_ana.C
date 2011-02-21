#include <iostream>
#include <iomanip>  
#include <stdio.h>
#include <stdlib.h>
#include <fstream.h>

void lumi_ana(int run_num, int start_mps, int stop_mps, string ana_type, bool get_name, bool get_mean)
{
gROOT->Reset();
gROOT->SetStyle("Plain");

Bool_t get_ds = kTRUE;
Bool_t get_us = kTRUE;
Bool_t get_bcm = kTRUE;

//Bool_t get_name = kFALSE;
//Bool_t get_mean = kTRUE;
//Bool_t get_rms = kTRUE;
Bool_t get_entries = kFALSE;
Bool_t save_file = kFALSE;
Bool_t term_output = kTRUE;

Bool_t view_volt = kFALSE;
Float_t scale_val;
if (ana_type == "asym") scale_val = 1e6;
 if (ana_type == "yield") scale_val = (20./(1<<18));
if (ana_type == "yield") view_volt = kTRUE;

/*
char * pPath;
pPath = getenv("QW_ROOTFILES");
ofstream ana_out;
ana_out.open(Form("%s/dat_files/%i_%s_mps_%i_%i.dat",pPath,run_num,ana_type,start_mps,stop_mps));
*/

const string leaf_name[18] =
{
"dslumi1",
"dslumi2",
"dslumi3",
"dslumi4",
"dslumi5",
"dslumi6",
"dslumi7", 
"dslumi8", 
"uslumi1neg",
"uslumi1pos",
"uslumi3neg",
"uslumi3pos",
"uslumi5neg",
"uslumi5pos",
"uslumi7neg",
"uslumi7pos", 
"bcm1", 
"bcm2"};

Int_t run_segments = 1;
TChain chain("Hel_Tree");
for (Int_t i=0; i<run_segments; i++)
{
  chain.Add(Form("$QW_ROOTFILES/Qweak_%i.00%i.root",run_num,i));
  cout << "loaded run seg # " << i << endl;
}

if (get_ds == kTRUE)
{
  TH1F *dshst[30];
  for (Int_t p=0; p < 8; p++)
    {           
      dshst[p] = new TH1F(Form("%i_h_%s",run_num,leaf_name[p]),"",100,0,0);
      dshst[p]->SetDirectory(0);     
      chain.Draw(Form("%s_qwk_%s.hw_sum*%f>>run%i_h_%s",ana_type,leaf_name[p],scale_val,run_num,leaf_name[p]),Form("mps_counter>%i && mps_counter<%i",start_mps,stop_mps),"");      
      TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%i_h_%s",run_num,leaf_name[p]));
      if(get_name==kTRUE)
        {
            if(get_mean==kTRUE) cout<<leaf_name[p]<<"\t"<<htemp->GetMean()<<endl;
            if(get_mean==kFALSE && get_entries==kFALSE) cout<<leaf_name[p]<<"\t"<<htemp->GetRMS()<<endl;
            if(get_entries==kTRUE) cout<<leaf_name[p]<<"\t"<<htemp->GetEntries()<<endl;
        }
      else 
        {
            if(get_mean==kTRUE) cout<<htemp->GetMean()<<endl;
            if(get_mean==kFALSE && get_entries==kFALSE) cout<<htemp->GetRMS()<<endl;
            if(get_entries==kTRUE) cout<<htemp->GetEntries()<<endl;
        }
    }
}

if (get_us == kTRUE)
{
  TH1F *ushst[30];
  for (Int_t p=8; p < 16; p++)
    {           
      ushst[p] = new TH1F(Form("%i_h_%s",run_num,leaf_name[p]),"",100,0,0);
      ushst[p]->SetDirectory(0);     
      chain.Draw(Form("%s_qwk_%s.hw_sum*%f>>run%i_h_%s",ana_type,leaf_name[p],scale_val,run_num,leaf_name[p]),Form("mps_counter>%i && mps_counter<%i",start_mps,stop_mps),"");      
      TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%i_h_%s",run_num,leaf_name[p]));
      if(get_name==kTRUE)
        {
            if(get_mean==kTRUE) cout<<leaf_name[p]<<"\t"<<htemp->GetMean()<<endl;
            if(get_mean==kFALSE && get_entries==kFALSE) cout<<leaf_name[p]<<"\t"<<htemp->GetRMS()<<endl;
            if(get_entries==kTRUE) cout<<leaf_name[p]<<"\t"<<htemp->GetEntries()<<endl;
        }
      else 
        {
            if(get_mean==kTRUE) cout<<htemp->GetMean()<<endl;
            if(get_mean==kFALSE && get_entries==kFALSE) cout<<htemp->GetRMS()<<endl;
            if(get_entries==kTRUE) cout<<htemp->GetEntries()<<endl;
        }
    }  
}

if (get_bcm == kTRUE)
{
  TH1F *bcmhst[30];
  for (Int_t p=16; p < 18; p++)
    {           
      bcmhst[p] = new TH1F(Form("%i_h_%s",run_num,leaf_name[p]),"",100,0,0);
      bcmhst[p]->SetDirectory(0);     
      chain.Draw(Form("%s_qwk_%s.hw_sum*%f>>run%i_h_%s",ana_type,leaf_name[p],scale_val,run_num,leaf_name[p]),Form("mps_counter>%i && mps_counter<%i",start_mps,stop_mps),"");      
      TH1F *htemp = (TH1F*)gPad->GetPrimitive(Form("run%i_h_%s",run_num,leaf_name[p]));
      if(get_name==kTRUE)
        {
            if(get_mean==kTRUE) cout<<leaf_name[p]<<"\t"<<htemp->GetMean()<<endl;
            if(get_mean==kFALSE && get_entries==kFALSE) cout<<leaf_name[p]<<"\t"<<htemp->GetRMS()<<endl;
            if(get_entries==kTRUE) cout<<leaf_name[p]<<"\t"<<htemp->GetEntries()<<endl;
        }
      else 
        {
            if(get_mean==kTRUE) cout<<htemp->GetMean()<<endl;
            if(get_mean==kFALSE && get_entries==kFALSE) cout<<htemp->GetRMS()<<endl;
            if(get_entries==kTRUE) cout<<htemp->GetEntries()<<endl;
        }
    }  
}
if (get_name==kTRUE) cout<<"\t"<<htemp->GetEntries()<<endl;
if (get_name==kFALSE) cout<<htemp->GetEntries()<<endl;
}


