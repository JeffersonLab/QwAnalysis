//Author: Anna Lee
//Date: Nov 18 2011
//Makes plots of hit pattern and drift time for the use of the shift crew  
//Edited: Nov 2013 to be able to save plots automatically in order to be used in 
//tracking pass5 replay -- Anna 

#include "TH1D.h"
#include "TCanvas.h"
#include "TChain.h"
#include "QwEvent.h"
#include "TF1.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TList.h"

void auto_HDC_plots(int runnum, Bool_t isFirst100K = kFALSE){
  // groups root files for a run together
  TChain* event_tree = new TChain ("event_tree");

  //add the root files to the chain the event_tree branches
  event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

  TH1D* E1 = new TH1D ("E1","Hokie 2: Package 2 Plane 1",34,0,33);
  TH1D* E2 = new TH1D ("E2","Hokie 2: Package 2 Plane 2",34,0,33);
  TH1D* E3 = new TH1D ("E3","Hokie 2: Package 2 Plane 3",34,0,33);
  TH1D* E4 = new TH1D ("E4","Hokie 2: Package 2 Plane 4",34,0,33);
  TH1D* E5 = new TH1D ("E5","Hokie 2: Package 2 Plane 5",34,0,33);
  TH1D* E6 = new TH1D ("E6","Hokie 2: Package 2 Plane 6",34,0,33);
  TH1D* E7 = new TH1D ("E7","Hokie 3: Package 2 Plane 7",34,0,33);
  TH1D* E8 = new TH1D ("E8","Hokie 3: Package 2 Plane 8",34,0,33);
  TH1D* E9 = new TH1D ("E9","Hokie 3: Package 2 Plane 9",34,0,33);
  TH1D* E10 = new TH1D ("E10","Hokie 3: Package 2 Plane 10",34,0,33);
  TH1D* E11 = new TH1D ("E11","Hokie 3: Package 2 Plane 11",34,0,33);
  TH1D* E12 = new TH1D ("E12","Hokie 3: Package 2 Plane 12",34,0,33);
  TH1D* E13 = new TH1D ("E13","Hokie 4: Package 1 Plane 1",34,0,33);
  TH1D* E14 = new TH1D ("E14","Hokie 4: Package 1 Plane 2",34,0,33);
  TH1D* E15 = new TH1D ("E15","Hokie 4: Package 1 Plane 3",34,0,33);
  TH1D* E16 = new TH1D ("E16","Hokie 4: Package 1 Plane 4",34,0,33);
  TH1D* E17 = new TH1D ("E17","Hokie 4: Package 1 Plane 5",34,0,33);
  TH1D* E18 = new TH1D ("E18","Hokie 4: Package 1 Plane 6",34,0,33);
  TH1D* E19 = new TH1D ("E19","Hokie 5: Package 1 Plane 7",34,0,33);
  TH1D* E20 = new TH1D ("E20","Hokie 5: Package 1 Plane 8",34,0,33);
  TH1D* E21 = new TH1D ("E21","Hokie 5: Package 1 Plane 9",34,0,33);
  TH1D* E22 = new TH1D ("E22","Hokie 5: Package 1 Plane 10",34,0,33);
  TH1D* E23 = new TH1D ("E23","Hokie 5: Package 1 Plane 11",34,0,33);
  TH1D* E24 = new TH1D ("E24","Hokie 5: Package 1 Plane 12",34,0,33);

  TH1D* T1 = new TH1D ("T1","Hokie 2: fTimeNS",100,-100,400);
  TH1D* T2 = new TH1D ("T2","Hokie 3: fTimeNS",100,-100,400);
  TH1D* T3 = new TH1D ("T3","Hokie 4: fTimeNS",100,-100,400);
  TH1D* T4 = new TH1D ("T4","Hokie 5: fTimeNS",100,-100,400);
  TH1D* T5 = new TH1D ("T5","Hokie 2: fTimeNS-Log Scale",100,-100,1600);
  TH1D* T6 = new TH1D ("T6","Hokie 2: fTimeNS-Log Scale",100,-100,1600);
  TH1D* T7 = new TH1D ("T7","Hokie 3: fTimeNS-Log Scale",100,-100,1600);
  TH1D* T8 = new TH1D ("T8","Hokie 3: fTimeNS-Log Scale",100,-100,1600);

  
  TCanvas* c1 = new TCanvas("c1","Hit Patterns",1200,700);
  c1->Divide(6,4);
  //fElement plots are on c1

  TCanvas* c2 = new TCanvas("c2","Drift Times",1000,600);
  c2->Divide(4,2);
  //fTime plots are on c2  

  //start fElement plots
  c1->cd(1);
  event_tree->Draw("events.fQwHits.fElement>>E1","events.fQwHits.fPlane==1 && events.fQwHits.fRegion==2 && events.fQwHits.fPackage==2");
  E1->SetTitle("Hokie 2: Package 2 Plane 1");
  E1->GetXaxis()->SetTitle("Wire");

  c1->cd(2);
  event_tree->Draw("events.fQwHits.fElement>>E2","fPlane==2&&fRegion==2&&fPackage==2");
  E2->SetTitle("Hokie 2: Package 2 Plane 2");
  E2->GetXaxis()->SetTitle("Wire");

  c1->cd(3);
  event_tree->Draw("events.fQwHits.fElement>>E3","fPlane==3&&fRegion==2&&fPackage==2");
  E3->SetTitle("Hokie 2: Package 2 Plane 3");
  E3->GetXaxis()->SetTitle("Wire");

  c1->cd(4);
  event_tree->Draw("events.fQwHits.fElement>>E4","fPlane==4&&fRegion==2&&fPackage==2");
  E4->SetTitle("Hokie 2: Package 2 Plane 4");
  E4->GetXaxis()->SetTitle("Wire");


  c1->cd(5);
  event_tree->Draw("events.fQwHits.fElement>>E5","fPlane==5&&fRegion==2&&fPackage==2");
  E5->SetTitle("Hokie 2: Package 2 Plane 5");
  E5->GetXaxis()->SetTitle("Wire");

  c1->cd(6);
  event_tree->Draw("events.fQwHits.fElement>>E6","fPlane==6&&fRegion==2&&fPackage==2");
  E6->SetTitle("Hokie 2: Package 2 Plane 6");
  E6->GetXaxis()->SetTitle("Wire");

  c1->cd(7);
  event_tree->Draw("events.fQwHits.fElement>>E7","fPlane==7&&fRegion==2&&fPackage==2");
  E7->SetTitle("Hokie 3: Package 2 Plane 7");
  E7->GetXaxis()->SetTitle("Wire");

  c1->cd(8);
  event_tree->Draw("events.fQwHits.fElement>>E8","fPlane==8&&fRegion==2&&fPackage==2");
  E8->SetTitle("Hokie 3: Package 2 Plane 8");
  E8->GetXaxis()->SetTitle("Wire");

  c1->cd(9);
  event_tree->Draw("events.fQwHits.fElement>>E9","fPlane==9&&fRegion==2&&fPackage==2");
  E9->SetTitle("Hokie 3: Package 2 Plane 9");
  E9->GetXaxis()->SetTitle("Wire");

  c1->cd(10);
  event_tree->Draw("events.fQwHits.fElement>>E10","fPlane==10&&fRegion==2&&fPackage==2");
  E10->SetTitle("Hokie 3: Package 2 Plane 10");
  E10->GetXaxis()->SetTitle("Wire");

  c1->cd(11);
  event_tree->Draw("events.fQwHits.fElement>>E11","fPlane==11&&fRegion==2&&fPackage==2");
  E11->SetTitle("Hokie 3: Package 2 Plane 11");
  E11->GetXaxis()->SetTitle("Wire");

  c1->cd(12);
  event_tree->Draw("events.fQwHits.fElement>>E12","fPlane==12&&fRegion==2&&fPackage==2");
  E12->SetTitle("Hokie 3: Package 2 Plane 12");
  E12->GetXaxis()->SetTitle("Wire");
 
  c1->cd(13);
  event_tree->Draw("events.fQwHits.fElement>>E13","fPlane==1&&fRegion==2&&fPackage==1");
  E13->SetTitle("Hokie 4: Package 1 Plane 1");
  E13->GetXaxis()->SetTitle("Wire");

  c1->cd(14);
  event_tree->Draw("events.fQwHits.fElement>>E14","fPlane==2&&fRegion==2&&fPackage==1");
  E14->SetTitle("Hokie 4: Package 1 Plane 2");
  E14->GetXaxis()->SetTitle("Wire");

  c1->cd(15);
  event_tree->Draw("events.fQwHits.fElement>>E15","fPlane==3&&fRegion==2&&fPackage==1");
  E15->SetTitle("Hokie 4: Package 1 Plane 3");
  E15->GetXaxis()->SetTitle("Wire");

  c1->cd(16);
  event_tree->Draw("events.fQwHits.fElement>>E16","fPlane==4&&fRegion==2&&fPackage==1");
  E16->SetTitle("Hokie 4: Package 1 Plane 4");
  E16->GetXaxis()->SetTitle("Wire");

  c1->cd(17);
  event_tree->Draw("events.fQwHits.fElement>>E17","fPlane==5&&fRegion==2&&fPackage==1");
  E17->SetTitle("Hokie 4: Package 1 Plane 5");
  E17->GetXaxis()->SetTitle("Wire");

  c1->cd(18);
  event_tree->Draw("events.fQwHits.fElement>>E18","fPlane==6&&fRegion==2&&fPackage==1");
  E18->SetTitle("Hokie 4: Package 1 Plane 6");
  E18->GetXaxis()->SetTitle("Wire");

  c1->cd(19);
  event_tree->Draw("events.fQwHits.fElement>>E19","fPlane==7&&fRegion==2&&fPackage==1");
  E19->SetTitle("Hokie 5: Package 1 Plane 7");
  E19->GetXaxis()->SetTitle("Wire");

  c1->cd(20);
  event_tree->Draw("events.fQwHits.fElement>>E20","fPlane==8&&fRegion==2&&fPackage==1");
  E20->SetTitle("Hokie 5: Package 1 Plane 8");
  E20->GetXaxis()->SetTitle("Wire");

  c1->cd(21);
  event_tree->Draw("events.fQwHits.fElement>>E21","fPlane==9&&fRegion==2&&fPackage==1");
  E21->SetTitle("Hokie 5: Package 1 Plane 9");
  E21->GetXaxis()->SetTitle("Wire");

  c1->cd(22);
  event_tree->Draw("events.fQwHits.fElement>>E22","fPlane==10&&fRegion==2&&fPackage==1");
  E22->SetTitle("Hokie 5: Package 1 Plane 10");
  E22->GetXaxis()->SetTitle("Wire");

  c1->cd(23);
  event_tree->Draw("events.fQwHits.fElement>>E23","fPlane==11&&fRegion==2&&fPackage==1");
  E23->SetTitle("Hokie 5: Package 1 Plane 11");
  E23->GetXaxis()->SetTitle("Wire");

  c1->cd(24);
  event_tree->Draw("events.fQwHits.fElement>>E24","fPlane==12&&fRegion==2&&fPackage==1");
  E24->SetTitle("Hokie 5: Package 1 Plane 12");
  E24->GetXaxis()->SetTitle("Wire");
 
  c1->SaveAs(Form("$QWSCRATCH/tracking/www/run_%d/hit_patterns_%d.png",runnum,runnum));

  //start ftime plots

  int t0 = -100;
  int td = 300;
  int t0_full = 0;
  int td_full = 0;
  c2->cd(1);
  event_tree->Draw("events.fQwHits.fTimeNs>>T1","fPlane<7&&fRegion==2&&fPackage==2");
  T1->GetXaxis()->SetRangeUser(t0,td);
  T1->SetTitle("Hokie 2: fTimeNs");
  T1->GetXaxis()->SetTitle("Drift Time (ns)");

  c2->cd(2);
  event_tree->Draw("events.fQwHits.fTimeNs>>T2","fPlane>6&&fPlane!=10&&fRegion==2&&fPackage==2");
  T2->GetXaxis()->SetRangeUser(t0,td);
  T2->SetTitle("Hokie 3: fTimeNs ");
  T2->GetXaxis()->SetTitle("Drift Time (ns)");
 
  c2->cd(3);
  event_tree->Draw("events.fQwHits.fTimeNs>>T3","fPlane<7&&fRegion==2&&fPackage==1");
  T3->GetXaxis()->SetRangeUser(t0,td);
  T3->SetTitle("Hokie 4: fTimeNs");
  T3->GetXaxis()->SetTitle("Drift Time (ns)");

  c2->cd(4);
  event_tree->Draw("events.fQwHits.fTimeNs>>T4","fPlane>6&&fRegion==2&&fPackage==1");
  T4->GetXaxis()->SetRangeUser(t0,td);
  T4->SetTitle("Hokie 5: fTimeNs");
  T4->GetXaxis()->SetTitle("Drift Time (ns)");

  c2->cd(5);
  event_tree->Draw("events.fQwHits.fTimeNs>>T5","fPlane<7&&fRegion==2&&fPackage==2");
  gPad->SetLogy(1);
  T5->SetTitle("Hokie 2: fTimeNs-Log Scale");
  T5->GetXaxis()->SetTitle("Drift Time (ns)");

  c2->cd(6);
  event_tree->Draw("events.fQwHits.fTimeNs>>T6","fPlane>6&&fPlane!=10&&fRegion==2&&fPackage==2");
  gPad->SetLogy(1);
  T6->SetTitle("Hokie 3: fTimeNs-Log Scale");  
  T6->GetXaxis()->SetTitle("Drift Time (ns)");

  c2->cd(7);
  event_tree->Draw("events.fQwHits.fTimeNs>>T7","fPlane<7&&fRegion==2&&fPackage==1");
  gPad->SetLogy(1);
  T7->SetTitle("Hokie 4: fTimeNs-Log Scale");
  T7->GetXaxis()->SetTitle("Drift Time (ns)");

  c2->cd(8);
  event_tree->Draw("events.fQwHits.fTimeNs>>T8","fPlane>6&&fRegion==2&&fPackage==1");
  gPad->SetLogy(1);
  T8->SetTitle("Hokie 5: fTimeNs-Log Scale");
  T8->GetXaxis()->SetTitle("Drift Time (ns)");

  c2->SaveAs(Form("$QWSCRATCH/tracking/www/run_%d/drift_times_%d.png",runnum,runnum));

  return;  
}
