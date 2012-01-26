//Author: Anna Lee
//Date: Nov 18 2011
//Makes plots of hit pattern and drift time for the use of the shift crew  



void HDC_plots(char* run_name){
//  gSystem->GetEnv();
  TFile *f = new TFile(Form("$QW_ROOTFILES/%s",run_name));
  
  
  TCanvas* c1 = new TCanvas("c1","Hit Patterns",1200,700);
  c1->Divide(6,4);
  //fElement plots are on c1

  TCanvas* c2 = new TCanvas("c2","Drift Times",1000,600);
  c2->Divide(4,2);
  //fTime plots are on c2  

  //start fElement plots
  c1->cd(1);
  event_tree->Draw("events.fQwHits.fElement>>E1","fPlane==1&&fRegion==2&&fPackage==2");
  E1->SetTitle("Hokie 2: Package 2 Plane 1");

  c1->cd(2);
  event_tree->Draw("events.fQwHits.fElement>>E2","fPlane==2&&fRegion==2&&fPackage==2");
  E2->SetTitle("Hokie 2: Package 2 Plane 2");

  c1->cd(3);
  event_tree->Draw("events.fQwHits.fElement>>E3","fPlane==3&&fRegion==2&&fPackage==2");
  E3->SetTitle("Hokie 2: Package 2 Plane 3");

  c1->cd(4);
  event_tree->Draw("events.fQwHits.fElement>>E4","fPlane==4&&fRegion==2&&fPackage==2");
  E4->SetTitle("Hokie 2: Package 2 Plane 4");

  c1->cd(5);
  event_tree->Draw("events.fQwHits.fElement>>E5","fPlane==5&&fRegion==2&&fPackage==2");
  E5->SetTitle("Hokie 2: Package 2 Plane 5");

  c1->cd(6);
  event_tree->Draw("events.fQwHits.fElement>>E6","fPlane==6&&fRegion==2&&fPackage==2");
  E6->SetTitle("Hokie 2: Package 2 Plane 6");

  c1->cd(7);
  event_tree->Draw("events.fQwHits.fElement>>E7","fPlane==7&&fRegion==2&&fPackage==2");
  E7->SetTitle("Hokie 3: Package 2 Plane 7");

  c1->cd(8);
  event_tree->Draw("events.fQwHits.fElement>>E8","fPlane==8&&fRegion==2&&fPackage==2");
  E8->SetTitle("Hokie 3: Package 2 Plane 8");

  c1->cd(9);
  event_tree->Draw("events.fQwHits.fElement>>E9","fPlane==9&&fRegion==2&&fPackage==2");
  E9->SetTitle("Hokie 3: Package 2 Plane 9");

  c1->cd(10);
  event_tree->Draw("events.fQwHits.fElement>>E10","fPlane==10&&fRegion==2&&fPackage==2");
  E10->SetTitle("Hokie 3: Package 2 Plane 10");

  c1->cd(11);
  event_tree->Draw("events.fQwHits.fElement>>E11","fPlane==11&&fRegion==2&&fPackage==2");
  E11->SetTitle("Hokie 3: Package 2 Plane 11");

  c1->cd(12);
  event_tree->Draw("events.fQwHits.fElement>>E12","fPlane==12&&fRegion==2&&fPackage==2");
  E12->SetTitle("Hokie 3: Package 2 Plane 12");
  
  c1->cd(13);
  event_tree->Draw("events.fQwHits.fElement>>E13","fPlane==1&&fRegion==2&&fPackage==1");
  E13->SetTitle("Hokie 4: Package 1 Plane 1");

  c1->cd(14);
  event_tree->Draw("events.fQwHits.fElement>>E14","fPlane==2&&fRegion==2&&fPackage==1");
  E14->SetTitle("Hokie 4: Package 1 Plane 2");

  c1->cd(15);
  event_tree->Draw("events.fQwHits.fElement>>E15","fPlane==3&&fRegion==2&&fPackage==1");
  E15->SetTitle("Hokie 4: Package 1 Plane 3");

  c1->cd(16);
  event_tree->Draw("events.fQwHits.fElement>>E16","fPlane==4&&fRegion==2&&fPackage==1");
  E16->SetTitle("Hokie 4: Package 1 Plane 4");

  c1->cd(17);
  event_tree->Draw("events.fQwHits.fElement>>E17","fPlane==5&&fRegion==2&&fPackage==1");
  E17->SetTitle("Hokie 4: Package 1 Plane 5");

  c1->cd(18);
  event_tree->Draw("events.fQwHits.fElement>>E18","fPlane==6&&fRegion==2&&fPackage==1");
  E18->SetTitle("Hokie 4: Package 1 Plane 6");

  c1->cd(19);
  event_tree->Draw("events.fQwHits.fElement>>E19","fPlane==7&&fRegion==2&&fPackage==1");
  E19->SetTitle("Hokie 5: Package 1 Plane 7");

  c1->cd(20);
  event_tree->Draw("events.fQwHits.fElement>>E20","fPlane==8&&fRegion==2&&fPackage==1");
  E20->SetTitle("Hokie 5: Package 1 Plane 8");

  c1->cd(21);
  event_tree->Draw("events.fQwHits.fElement>>E21","fPlane==9&&fRegion==2&&fPackage==1");
  E21->SetTitle("Hokie 5: Package 1 Plane 9");

  c1->cd(22);
  event_tree->Draw("events.fQwHits.fElement>>E22","fPlane==10&&fRegion==2&&fPackage==1");
  E22->SetTitle("Hokie 5: Package 1 Plane 10");

  c1->cd(23);
  event_tree->Draw("events.fQwHits.fElement>>E23","fPlane==11&&fRegion==2&&fPackage==1");
  E23->SetTitle("Hokie 5: Package 1 Plane 11");

  c1->cd(24);
  event_tree->Draw("events.fQwHits.fElement>>E24","fPlane==12&&fRegion==2&&fPackage==1");
  E24->SetTitle("Hokie 5: Package 1 Plane 12");
  

  //start ftime plots

  int t0 = -100;
  int td = 300;
  int t0_full = 0;
  int td_full = 0;
  c2->cd(1);
  event_tree->Draw("events.fQwHits.fTimeNs>>T1","fPlane<7&&fRegion==2&&fPackage==2");
  T1->GetXaxis()->SetRangeUser(t0,td);
  T1->SetTitle("Hokie 2: fTimeNs");
 

  c2->cd(2);
  event_tree->Draw("events.fQwHits.fTimeNs>>T2","fPlane>6&&fPlane!=10&&fRegion==2&&fPackage==2");
  T2->GetXaxis()->SetRangeUser(t0,td);
  T2->SetTitle("Hokie 3: fTimeNs ");
  
  c2->cd(3);
  event_tree->Draw("events.fQwHits.fTimeNs>>T3","fPlane<7&&fRegion==2&&fPackage==1");
  T3->GetXaxis()->SetRangeUser(t0,td);
  T3->SetTitle("Hokie 4: fTimeNs");

  c2->cd(4);
  event_tree->Draw("events.fQwHits.fTimeNs>>T4","fPlane>6&&fRegion==2&&fPackage==1");
  T4->GetXaxis()->SetRangeUser(t0,td);
  T4->SetTitle("Hokie 5: fTimeNs");

  c2->cd(5);
  event_tree->Draw("events.fQwHits.fTimeNs>>T5","fPlane<7&&fRegion==2&&fPackage==2");
  gPad->SetLogy(1);
  //T5->GetXaxis()->SetRangeUser(t0_full,td_full);
  T5->SetTitle("Hokie 2: fTimeNs-Log Scale");

  c2->cd(6);
  event_tree->Draw("events.fQwHits.fTimeNs>>T6","fPlane>6&&fPlane!=10&&fRegion==2&&fPackage==2");
  gPad->SetLogy(1);
  //T6->GetXaxis()->SetRangeUser(t0_full,td_full);
  T6->SetTitle("Hokie 3: fTimeNs-Log Scale");  

  c2->cd(7);
  event_tree->Draw("events.fQwHits.fTimeNs>>T7","fPlane<7&&fRegion==2&&fPackage==1");
  gPad->SetLogy(1);
  //T7->GetXaxis()->SetRangeUser(t0_full,td_full);
  T7->SetTitle("Hokie 4: fTimeNs-Log Scale");

  c2->cd(8);
  event_tree->Draw("events.fQwHits.fTimeNs>>T8","fPlane>6&&fRegion==2&&fPackage==1");
  gPad->SetLogy(1);
  //T8->GetXaxis()->SetRangeUser(t0_full,td_full);
  T8->SetTitle("Hokie 5: fTimeNs-Log Scale");



}
