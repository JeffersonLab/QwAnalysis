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

void auto_HDC_plots(int runnum){
  // groups root files for a run together
  TChain* event_tree = new TChain ("event_tree");

  //add the root files to the chain the event_tree branches
  event_tree->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

  //define the hit pattern histograms
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

  //Define the  drift time histograms
  TH1D* T1 = new TH1D ("T1","Hokie 2: fTimeNS",100,-100,400);
  TH1D* T2 = new TH1D ("T2","Hokie 3: fTimeNS",100,-100,400);
  TH1D* T3 = new TH1D ("T3","Hokie 4: fTimeNS",100,-100,400);
  TH1D* T4 = new TH1D ("T4","Hokie 5: fTimeNS",100,-100,400);
  TH1D* T5 = new TH1D ("T5","Hokie 2: fTimeNS-Log Scale",100,-100,1600);
  TH1D* T6 = new TH1D ("T6","Hokie 2: fTimeNS-Log Scale",100,-100,1600);
  TH1D* T7 = new TH1D ("T7","Hokie 3: fTimeNS-Log Scale",100,-100,1600);
  TH1D* T8 = new TH1D ("T8","Hokie 3: fTimeNS-Log Scale",100,-100,1600);

  
  //Create and Divide canvases
  TCanvas* c1 = new TCanvas("c1","Hit Patterns",1200,700);
  c1->Divide(6,4);
  //fElement plots are on c1

  TCanvas* c2 = new TCanvas("c2","Drift Times",1000,600);
  c2->Divide(4,2);
  //fTime plots are on c2  

  //Define axis
  E1->GetXaxis()->SetTitle("Wire");
  E2->GetXaxis()->SetTitle("Wire");
  E3->GetXaxis()->SetTitle("Wire");
  E4->GetXaxis()->SetTitle("Wire");
  E5->GetXaxis()->SetTitle("Wire");
  E6->GetXaxis()->SetTitle("Wire");
  E7->GetXaxis()->SetTitle("Wire");
  E8->GetXaxis()->SetTitle("Wire");
  E9->GetXaxis()->SetTitle("Wire");
  E10->GetXaxis()->SetTitle("Wire");
  E11->GetXaxis()->SetTitle("Wire");
  E12->GetXaxis()->SetTitle("Wire");
  E13->GetXaxis()->SetTitle("Wire");
  E14->GetXaxis()->SetTitle("Wire");
  E15->GetXaxis()->SetTitle("Wire");
  E16->GetXaxis()->SetTitle("Wire");
  E17->GetXaxis()->SetTitle("Wire");
  E18->GetXaxis()->SetTitle("Wire");
  E19->GetXaxis()->SetTitle("Wire");
  E20->GetXaxis()->SetTitle("Wire");
  E21->GetXaxis()->SetTitle("Wire");
  E22->GetXaxis()->SetTitle("Wire");
  E23->GetXaxis()->SetTitle("Wire");
  E24->GetXaxis()->SetTitle("Wire");

  T1->GetXaxis()->SetTitle("Drift Time (NS)");
  T2->GetXaxis()->SetTitle("Drift Time (NS)");
  T3->GetXaxis()->SetTitle("Drift Time (NS)");
  T4->GetXaxis()->SetTitle("Drift Time (NS)");
  T5->GetXaxis()->SetTitle("Drift Time (NS)");
  T6->GetXaxis()->SetTitle("Drift Time (NS)");
  T7->GetXaxis()->SetTitle("Drift Time (NS)");
  T8->GetXaxis()->SetTitle("Drift Time (NS)");
 
  //start looping over events
  //extract number of events
  Int_t nevents=event_tree->GetEntries();

  //Define QwEvent as a pointer?
  QwEvent* fEvent =0;

  //set the event_tree branch to be on
  event_tree->SetBranchStatus("events",1);

  //extract teh event branch
  TBranch* event_branch=event_tree->GetBranch("events");
  event_branch->SetAddress(&fEvent);

  //Loop through the events, filling histogram as you go
  cout << "Total Events: " << nevents << endl;
  for(int i=0; i<nevents;i++){
    //get event
    event_branch->GetEntry(i);
    //get number of hits
    int nhits = fEvent->GetNumberOfHits();
    //loop over hits
    for(int j=0;j<nhits;j++){
      const QwHit* hit = fEvent->GetHit(j);
      if(hit->GetRegion()==2){
        if(hit->GetPackage()==2){
          if(hit->GetPlane()==1){
            E1->Fill(hit->GetElement());
            T1->Fill(hit->GetTimeNs());
            T5->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==2){
            E2->Fill(hit->GetElement());
            T1->Fill(hit->GetTimeNs());
            T5->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==3){
            E3->Fill(hit->GetElement());
            T1->Fill(hit->GetTimeNs());
            T5->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==4){
            E4->Fill(hit->GetElement());
            T1->Fill(hit->GetTimeNs());
            T5->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==5){
            E5->Fill(hit->GetElement());
            T1->Fill(hit->GetTimeNs());
            T5->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==6){
            E6->Fill(hit->GetElement());
            T1->Fill(hit->GetTimeNs());
            T5->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==7){
            E7->Fill(hit->GetElement());
            T2->Fill(hit->GetTimeNs());
            T6->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==8){
            E8->Fill(hit->GetElement());
            T2->Fill(hit->GetTimeNs());
            T6->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==9){
            E9->Fill(hit->GetElement());
            T2->Fill(hit->GetTimeNs());
            T6->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==10){
            E10->Fill(hit->GetElement());
            T2->Fill(hit->GetTimeNs());
            T6->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==11){
            E11->Fill(hit->GetElement());
            T2->Fill(hit->GetTimeNs());
            T6->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==12){
            E12->Fill(hit->GetElement());
            T2->Fill(hit->GetTimeNs());
            T6->Fill(hit->GetTimeNs());
          }
        }
        if(hit->GetPackage()==1){
           if(hit->GetPlane()==1){
            E13->Fill(hit->GetElement());
            T3->Fill(hit->GetTimeNs());
            T7->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==2){
            E14->Fill(hit->GetElement());
            T3->Fill(hit->GetTimeNs());
            T7->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==3){
            E15->Fill(hit->GetElement());
            T3->Fill(hit->GetTimeNs());
            T7->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==4){
            E16->Fill(hit->GetElement());
            T3->Fill(hit->GetTimeNs());
            T7->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==5){
            E17->Fill(hit->GetElement());
            T3->Fill(hit->GetTimeNs());
            T7->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==6){
            E18->Fill(hit->GetElement());
            T3->Fill(hit->GetTimeNs());
            T7->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==7){
            E19->Fill(hit->GetElement());
            T4->Fill(hit->GetTimeNs());
            T8->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==8){
            E20->Fill(hit->GetElement());
            T4->Fill(hit->GetTimeNs());
            T8->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==9){
            E21->Fill(hit->GetElement());
            T4->Fill(hit->GetTimeNs());
            T8->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==10){
            E22->Fill(hit->GetElement());
            T4->Fill(hit->GetTimeNs());
            T8->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==11){
            E23->Fill(hit->GetElement());
            T4->Fill(hit->GetTimeNs());
            T8->Fill(hit->GetTimeNs());
          }
          if(hit->GetPlane()==12){
            E24->Fill(hit->GetElement());
            T4->Fill(hit->GetTimeNs());
            T8->Fill(hit->GetTimeNs());
          }     
        }
      } 
    }
  }
  //Draw historgrams
  c1->cd(1);
  E1->Draw();
  c1->cd(2);
  E2->Draw();
  c1->cd(3);
  E3->Draw();
  c1->cd(4);
  E4->Draw();
  c1->cd(5);
  E5->Draw();
  c1->cd(6);
  E6->Draw();
  c1->cd(7);
  E7->Draw();
  c1->cd(8);
  E8->Draw();
  c1->cd(9);
  E9->Draw();
  c1->cd(10);
  E10->Draw();
  c1->cd(11);
  E11->Draw();
  c1->cd(12);
  E12->Draw();
  c1->cd(13);
  E13->Draw();
  c1->cd(14);
  E14->Draw();
  c1->cd(15);
  E15->Draw();
  c1->cd(16);
  E16->Draw();
  c1->cd(17);
  E17->Draw();
  c1->cd(18);
  E18->Draw();
  c1->cd(19);
  E19->Draw();
  c1->cd(20);
  E20->Draw();
  c1->cd(21);
  E21->Draw();
  c1->cd(22);
  E22->Draw();
  c1->cd(23);
  E23->Draw();
  c1->cd(24);
  E24->Draw();

  c2->cd(1);
  T1->Draw();
  c2->cd(2);
  T2->Draw();
  c2->cd(3);
  T3->Draw();
  c2->cd(4);
  T4->Draw();
  c2->cd(5);
  T5->Draw();
  gPad->SetLogy(1);
  c2->cd(6);
  T6->Draw();
  gPad->SetLogy(1);
  c2->cd(7);
  T7->Draw();
  gPad->SetLogy(1);
  c2->cd(8);
  T8->Draw();
  gPad->SetLogy(1);


  //Save the plots
  c1->SaveAs(Form("$QWSCRATCH/tracking/www/run_%d/test_hit_patterns_%d.png",runnum,runnum));
  c2->SaveAs(Form("$QWSCRATCH/tracking/www/run_%d/test_drift_times_%d.png",runnum,runnum));

  return;  
}
