// Name:   Lumi.C
// Author: David Armstrong
// Email:  armd@jlab.org
// Date:   Monday Mar 4,2013
// Version:1.1
//
// simple, non-intelligent script to plot various Lumi monitor
// scalers and ratios of scalers for use in event-mode tracking runs analysis
//
// Goal: to a) use Lumis as current monitors at low (50 pA) currents
//       and b) use Ratios of Lumis as beam position monitors at these currents
//
//  - ranges of histograms need to be tweaked by hand based on running conditions
//  - don't include uslumi7pos in sums: seems is counting huge noise
//
//  usage: qwroot
//         .L Lumi.C
//          lumi(15121);   (will look for Qweak_15121.root in $QW_ROOTFILES and  
//                          produce plots for run 15121).

#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>

void lumi(int run=8658, TString stem="Qweak_", string suffix=""){

   string folder=gSystem->Getenv("QW_ROOTFILES");
   ostringstream ss;
   ss << folder << "/";
   ss << stem.Data();
   ss << run << suffix;
   ss << ".root";
   string file_name=ss.str();
   cout <<  file_name << endl;

   TFile* file=new TFile(file_name.c_str());

   TH1F* dsratio15=new TH1F("dsratio15","DS Lumi 1/5 ratio",100,0.1,4);
   TH1F* dsratiox=new TH1F("dsratiox","DS Lumi 128/456 ratio",100,0.1,4);
   TH1F* dsratioy=new TH1F("dsratioy","DS Lumi 234/678 ratio",100,0.1,15);
   TH1F* dsratio37=new TH1F("dsratio37","DS Lumi 3/7 ratio",100,0.1,15);

   TH1F* dsrate=new TH1F("dsrate","sum DS Lumi Rate",150,0.1,20000);
   TH1F* usrate=new TH1F("usrate","sum US Lumi Rate",150,0.1,600);

   TH2F* r15time=new TH2F("r15time","DS Lumi 1/5 ratio vs. time",100,0,3000000,50,0,4);
   TH2F* r37time=new TH2F("r37time","DS Lumi 3/7 ratio vs. time",100,0,3000000,50,0,15);

   QwEvent* fEvent=0;
   QwTrack* track=0;
   QwKinematics* kinematic=0;
   TTree* mps_tree= ( TTree* ) file->Get ( "Mps_Tree" );
   
   Mps_Tree->SetAlias("sum_ds","sca_dslumi1+sca_dslumi2+sca_dslumi3+sca_dslumi4+sca_dslumi5+sca_dslumi6+sca_dslumi7+sca_dslumi8");
   Mps_Tree->SetAlias("sum_us","sca_uslumi1pos+sca_uslumi1neg+sca_uslumi3pos+sca_uslumi3neg+sca_uslumi5pos+sca_uslumi5neg+sca_uslumi7neg");


   TCanvas* c1=new TCanvas("c1","Lumi Monitor totals (rates)",800,600);
   gStyle->SetStatW(0.45);
   gStyle->SetStatH(0.25);

   c1->Divide(2,2);
   c1->cd(1);
   Mps_Tree->Draw("sca_dslumi1+sca_dslumi2+sca_dslumi3+sca_dslumi4+sca_dslumi5+sca_dslumi6+sca_dslumi7+sca_dslumi8:CodaEventNumber");
   c1->cd(2);
   Mps_Tree->Draw("sca_uslumi1pos+sca_uslumi1neg+sca_uslumi3pos+sca_uslumi3neg+sca_uslumi5pos+sca_uslumi5neg+sca_uslumi7neg:CodaEventNumber");
   c1->cd(3);
   Mps_Tree->Draw("sum_ds>>dsrate");
   c1->cd(4);
   Mps_Tree->Draw("sum_us>>usrate");


   Mps_Tree->SetAlias("Ratio15","sca_dslumi1/sca_dslumi5");
   Mps_Tree->SetAlias("Ratiox","(sca_dslumi1+sca_dslumi2+sca_dslumi8)/(sca_dslumi4+sca_dslumi5+sca_dslumi6)");
   Mps_Tree->SetAlias("Ratioy","(sca_dslumi2+sca_dslumi3+sca_dslumi4)/(sca_dslumi6+sca_dslumi7+sca_dslumi8)");
   Mps_Tree->SetAlias("Ratio37","sca_dslumi3/sca_dslumi7");

   TCanvas* c2=new TCanvas("c2","US Lumi Monitor rates vs. time",800,600);

   c2->Divide(2,4);
   c2->cd(1);
   Mps_Tree->Draw("sca_uslumi1pos:CodaEventNumber");
   c2->cd(2);
   Mps_Tree->Draw("sca_uslumi1neg:CodaEventNumber");
   c2->cd(3);
   Mps_Tree->Draw("sca_uslumi3pos:CodaEventNumber");
   c2->cd(4);
   Mps_Tree->Draw("sca_uslumi3neg:CodaEventNumber");
   c2->cd(5);
   Mps_Tree->Draw("sca_uslumi5pos:CodaEventNumber");
   c2->cd(6);
   Mps_Tree->Draw("sca_uslumi5neg:CodaEventNumber");
   c2->cd(7);
   Mps_Tree->Draw("sca_uslumi7pos:CodaEventNumber");
   c2->cd(8);
   Mps_Tree->Draw("sca_uslumi7neg:CodaEventNumber");



   TCanvas* c=new TCanvas("c3","DS Lumi Monitors vs Time",800,600);
   c3->Divide(4,2);
   c3->cd(1);
   Mps_Tree->Draw("sca_dslumi1:CodaEventNumber");
   c3->cd(2);
   Mps_Tree->Draw("sca_dslumi2:CodaEventNumber");
   c3->cd(3);
   Mps_Tree->Draw("sca_dslumi3:CodaEventNumber");
   c3->cd(4);
   Mps_Tree->Draw("sca_dslumi4:CodaEventNumber");
   c3->cd(5);
   Mps_Tree->Draw("sca_dslumi5:CodaEventNumber");
   c3->cd(6);
   Mps_Tree->Draw("sca_dslumi6:CodaEventNumber");
   c3->cd(7);
   Mps_Tree->Draw("sca_dslumi7:CodaEventNumber");
   c3->cd(8);
   Mps_Tree->Draw("sca_dslumi8:CodaEventNumber");


   TCanvas* c=new TCanvas("c","DS Lumi Monitor ratios",800,600);
   c->Divide(2,3);
   c->cd(1);
   Mps_Tree->Draw("Ratio15:CodaEventNumber>>r15time");
   c->cd(3);
   Mps_Tree->Draw("Ratio15>>dsratio15");
   c->cd(5);
   Mps_Tree->Draw("Ratiox>>dsratiox");
   c->cd(2);
   Mps_Tree->Draw("Ratio37:CodaEventNumber>>r37time");
   c->cd(6);
   Mps_Tree->Draw("Ratioy>>dsratioy");
   c->cd(4);
   Mps_Tree->Draw("Ratio37>>dsratio37");

}

