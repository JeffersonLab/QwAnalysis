// Name:   ScalerCheck.C
// Author: Martin McHugh	
// Email:  mjmchugh@jlab.org
// Date:   2014-12-12
// Version:1.1
//
// simple, non-intelligent script to plot various Scaler monitor
// scalers and ratios of scalers for use in event-mode tracking runs analysis
//
// Goal: to a) use Scalers as current monitors at low (10 - 100 nA) currents
// 	    b) provide rate information to possibly normalize neutral fraction information
//
//
//  usage: qwroot
//         .L ScalerCheck.C
//          Scaler(15121,5,1);   (will look for Qweak_15121.root in $QW_ROOTFILES and  
//                                produce plots for run 15121, looks at octants 5 and
//				  1 MDs in partcular).

#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>

void Scaler(int run=8658, int TS1_Octant=7, int TS2_Octant=3, TString stem="Qweak_", string suffix=""){

   string folder=gSystem->Getenv("QW_ROOTFILES");
   ostringstream ss;
   ss << folder << "/";
   ss << stem.Data();
   ss << run << suffix;
   ss << ".root";
   string file_name=ss.str();
   //cout <<  file_name << endl;

   TFile* file=new TFile(file_name.c_str());

   //TH1F* dsrate=new TH1F("dsrate","sum DS Scaler Rate",1500,0,1.5e6);
   //TH1F* mdrate=new TH1F("mdrate","MD bar sum scaler rate",500,0,5000);

   TTree* mps_tree= ( TTree* ) file->Get ( "Mps_Tree" );
   TTree* event_tree= ( TTree* ) file->Get ( "event_tree" );
   TH1F* htemp;

   event_tree->SetAlias("sum_MD_sca","maindet.md1bar_sca+maindet.md2bar_sca+maindet.md3bar_sca+maindet.md4bar_sca+maindet.md5bar_sca+maindet.md6bar_sca+maindet.md7bar_sca+maindet.md8bar_sca");

   TCanvas* c2=new TCanvas("c2","MD rates",800,400);
   c2->Divide(2,1);
   c2->cd(1);
   event_tree->Draw("sum_MD_sca:CodaEventNumber","sum_MD_sca>0","");
   c2->cd(2);
   event_tree->Draw("sum_MD_sca","sum_MD_sca>0","");
   c2->Update();
   htemp = (TH1F*)gPad->GetPrimitive("htemp");
   std::cout << run << " " << htemp->GetEntries() << " " << htemp->GetMean() << std::endl;
   c2->Print(Form("MDRates_%i.png",run));

   TCanvas* c3=new TCanvas("c3", "Important Octant MD Rates", 1200, 800);
   c3->Divide(3,2);
   c3->cd(1);
   event_tree->Draw(Form("((maindet.md%ibar_sca+maindet.md%ibar_sca)/2):CodaEventNumber", TS1_Octant, TS2_Octant),"sum_MD_sca>0","");
   c3->cd(2);
   event_tree->Draw(Form("maindet.md%ibar_sca:CodaEventNumber", TS1_Octant),"sum_MD_sca>0","");
   c3->cd(3);
   event_tree->Draw(Form("maindet.md%ibar_sca:CodaEventNumber", TS2_Octant),"sum_MD_sca>0","");
   c3->cd(4);
   event_tree->Draw(Form("(maindet.md%ibar_sca+maindet.md%ibar_sca)/2", TS1_Octant, TS2_Octant),"sum_MD_sca>0","");
   c3->Update();
   htemp = (TH1F*)gPad->GetPrimitive("htemp");
   std::cout << run << " " << htemp->GetEntries() << " " << htemp->GetMean() << " " << htemp->GetMeanError() << " " ;
   c3->cd(5);
   event_tree->Draw(Form("maindet.md%ibar_sca", TS1_Octant),"sum_MD_sca>0","");
   c3->Update();
   htemp = (TH1F*)gPad->GetPrimitive("htemp");
   std::cout << TS1_Octant << " " << htemp->GetEntries() << " " << htemp->GetMean() << " " << htemp->GetMeanError() << " " ;
   c3->cd(6);
   event_tree->Draw(Form("maindet.md%ibar_sca", TS2_Octant),"sum_MD_sca>0","");
   c3->Update();
   htemp = (TH1F*)gPad->GetPrimitive("htemp");
   std::cout << TS2_Octant << " " << htemp->GetEntries() << " " << htemp->GetMean() << " " << htemp->GetMeanError() << std::endl;
   c3->Print(Form("TrackingOctantMDRates_%i.png", run));

   TCanvas* c4 = new TCanvas("c4", "TS Rates vs Event", 1200, 800);
   c4->Divide(3,2);
   c4->cd(1);
   event_tree->Draw("(trigscint.ts1m_sca+trigscint.ts1m_sca)/2.0:CodaEventNumber","(trigscint.ts1m_sca+trigscint.ts1m_sca)/2 > 0","");
   c4->cd(2);
   event_tree->Draw("trigscint.ts1m_sca:CodaEventNumber","trigscint.ts1m_sca > 0","");
   c4->cd(3);
   event_tree->Draw("trigscint.ts1p_sca:CodaEventNumber","trigscint.ts1p_sca > 0","");
   c4->cd(4);
   event_tree->Draw("(trigscint.ts2m_sca+trigscint.ts2m_sca)/2.0:CodaEventNumber","(trigscint.ts2m_sca+trigscint.ts2m_sca)/2 > 0","");
   c4->cd(5);
   event_tree->Draw("trigscint.ts2m_sca:CodaEventNumber","trigscint.ts2m_sca > 0","");
   c4->cd(6);
   event_tree->Draw("trigscint.ts2p_sca:CodaEventNumber","trigscint.ts2p_sca > 0","");
   c4->Print(Form("TSRatesByEvent_%i.png", run));

   TCanvas* c5 = new TCanvas("c5", "TS Rates", 1200, 800);
   c5->Divide(3,2);
   c5->cd(1);
   event_tree->Draw("(trigscint.ts1m_sca+trigscint.ts1p_sca)/2.0","(trigscint.ts1m_sca+trigscint.ts1m_sca)/2 > 0","");
   c5->Update();
   htemp = (TH1F*)gPad->GetPrimitive("htemp");
   std::cout << run << " " << TS1_Octant <<  " " << htemp->GetEntries() << " " << htemp->GetMean() << " " << htemp->GetMeanError() << " " ;
   c5->cd(2);
   event_tree->Draw("trigscint.ts1m_sca","trigscint.ts1m_sca > 0","");
   c5->Update();
   htemp = (TH1F*)gPad->GetPrimitive("htemp");
   std::cout << " " << htemp->GetEntries() << " " << htemp->GetMean() << " " << htemp->GetMeanError() << " " ;
   c5->cd(3);
   event_tree->Draw("trigscint.ts1p_sca","trigscint.ts1p_sca > 0","");
   c5->Update();
   htemp = (TH1F*)gPad->GetPrimitive("htemp");
   std::cout << " " << htemp->GetEntries() << " " << htemp->GetMean() << " " << htemp->GetMeanError() << std::endl;
   c5->cd(4);
   event_tree->Draw("(trigscint.ts2m_sca+trigscint.ts2p_sca)/2.0","(trigscint.ts2m_sca+trigscint.ts2m_sca)/2 > 0","");
   c5->Update();
   htemp = (TH1F*)gPad->GetPrimitive("htemp");
   std::cout << run << " " << TS1_Octant <<  " " << htemp->GetEntries() << " " << htemp->GetMean() << " " << htemp->GetMeanError() << " " ;
   c5->cd(5);
   event_tree->Draw("trigscint.ts2m_sca","trigscint.ts2m_sca > 0","");
   c5->Update();
   htemp = (TH1F*)gPad->GetPrimitive("htemp");
   std::cout << " " << htemp->GetEntries() << " " << htemp->GetMean() << " " << htemp->GetMeanError() << " " ;
   c5->cd(6);
   event_tree->Draw("trigscint.ts2p_sca","trigscint.ts2p_sca > 0","");
   c5->Update();
   htemp = (TH1F*)gPad->GetPrimitive("htemp");
   std::cout << " " << htemp->GetEntries() << " " << htemp->GetMean() << " " << htemp->GetMeanError() << std::endl;
   c5->Print(Form("TSRates_%i.png", run)); 
  
}

