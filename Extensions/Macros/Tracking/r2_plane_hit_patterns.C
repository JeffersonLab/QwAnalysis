//Name: r2_plane_hit_patterns
//Author: Anna Lee (annarlee@vt.edu)
//Makes plots of hit patterns for the individual planes 
//of the R2 drift chambers

//Use:
//root[0]   .L Extensions/Macros/Tracking/r2_plane_hit_patterns.C
//root[1]   hit_pattern(15151)
//produces the hit patterns for the run requested


void hit_pattern(int run=15140){

  string folder=gSystem->Getenv("QW_ROOTFILES");
  ostringstream strg;
  strg << folder << "/Qweak_";
  strg << run << ".root";
  string file_name=strg.str();
  cout <<  file_name << endl;
  TFile* file=new TFile(file_name.c_str());

  TCanvas* c1 = new TCanvas("c1","c1",1000,600);
  c1->Divide(6,4);
  
  
  c1->cd(1);
  event_tree->Draw("events.fQwHits.fElement>>E1","fPlane==1&&fRegion==2&&fPackage==1");
  E1->SetTitle("fElement: Package 1 Plane 1");

  c1->cd(2);
  event_tree->Draw("events.fQwHits.fElement>>E2","fPlane==2&&fRegion==2&&fPackage==1");
  E2->SetTitle("fElement: Package 1 Plane 2");

  c1->cd(3);
  event_tree->Draw("events.fQwHits.fElement>>E3","fPlane==3&&fRegion==2&&fPackage==1");
  E3->SetTitle("fElement: Package 1 Plane 3");

  c1->cd(4);
  event_tree->Draw("events.fQwHits.fElement>>E4","fPlane==4&&fRegion==2&&fPackage==1");
  E4->SetTitle("fElement: Package 1 Plane 4");

  c1->cd(5);
  event_tree->Draw("events.fQwHits.fElement>>E5","fPlane==5&&fRegion==2&&fPackage==1");
  E5->SetTitle("fElement: Package 1 Plane 5");

  c1->cd(6);
  event_tree->Draw("events.fQwHits.fElement>>E6","fPlane==6&&fRegion==2&&fPackage==1");
  E6->SetTitle("fElement: Package 1 Plane 6");

  c1->cd(7);
  event_tree->Draw("events.fQwHits.fElement>>E7","fPlane==7&&fRegion==2&&fPackage==1");
  E7->SetTitle("fElement: Package 1 Plane 7");

  c1->cd(8);
  event_tree->Draw("events.fQwHits.fElement>>E8","fPlane==8&&fRegion==2&&fPackage==1");
  E8->SetTitle("fElement: Package 1 Plane 8");

  c1->cd(9);
  event_tree->Draw("events.fQwHits.fElement>>E9","fPlane==9&&fRegion==2&&fPackage==1");
  E9->SetTitle("fElement: Package 1 Plane 9");

  c1->cd(10);
  event_tree->Draw("events.fQwHits.fElement>>E10","fPlane==10&&fRegion==2&&fPackage==1");
  E10->SetTitle("fElement: Package 1 Plane 10");

  c1->cd(11);
  event_tree->Draw("events.fQwHits.fElement>>E11","fPlane==11&&fRegion==2&&fPackage==1");
  E11->SetTitle("fElement: Package 1 Plane 11");

  c1->cd(12);
  event_tree->Draw("events.fQwHits.fElement>>E12","fPlane==12&&fRegion==2&&fPackage==1");
  E12->SetTitle("fElement: Package 1 Plane 12");
  
  c1->cd(13);
  event_tree->Draw("events.fQwHits.fElement>>E13","fPlane==1&&fRegion==2&&fPackage==2");
  E13->SetTitle("fElement: Package 2 Plane 1");

  c1->cd(14);
  event_tree->Draw("events.fQwHits.fElement>>E14","fPlane==2&&fRegion==2&&fPackage==2");
  E14->SetTitle("fElement: Package 2 Plane 2");

  c1->cd(15);
  event_tree->Draw("events.fQwHits.fElement>>E15","fPlane==3&&fRegion==2&&fPackage==2");
  E15->SetTitle("fElement: Package 2 Plane 3");

  c1->cd(16);
  event_tree->Draw("events.fQwHits.fElement>>E16","fPlane==4&&fRegion==2&&fPackage==2");
  E16->SetTitle("fElement: Package 2 Plane 4");

  c1->cd(17);
  event_tree->Draw("events.fQwHits.fElement>>E17","fPlane==5&&fRegion==2&&fPackage==2");
  E17->SetTitle("fElement: Package 2 Plane 5");

  c1->cd(18);
  event_tree->Draw("events.fQwHits.fElement>>E18","fPlane==6&&fRegion==2&&fPackage==2");
  E18->SetTitle("fElement: Package 2 Plane 6");

  c1->cd(19);
  event_tree->Draw("events.fQwHits.fElement>>E19","fPlane==7&&fRegion==2&&fPackage==2");
  E19->SetTitle("fElement: Package 2 Plane 7");

  c1->cd(20);
  event_tree->Draw("events.fQwHits.fElement>>E20","fPlane==8&&fRegion==2&&fPackage==2");
  E20->SetTitle("fElement: Package 2 Plane 8");

  c1->cd(21);
  event_tree->Draw("events.fQwHits.fElement>>E21","fPlane==9&&fRegion==2&&fPackage==2");
  E21->SetTitle("fElement: Package 2 Plane 9");

  c1->cd(22);
  event_tree->Draw("events.fQwHits.fElement>>E22","fPlane==10&&fRegion==2&&fPackage==2");
  E22->SetTitle("fElement: Package 2 Plane 10");

  c1->cd(23);
  event_tree->Draw("events.fQwHits.fElement>>E23","fPlane==11&&fRegion==2&&fPackage==2");
  E23->SetTitle("fElement: Package 2 Plane 11");

  c1->cd(24);
  event_tree->Draw("events.fQwHits.fElement>>E24","fPlane==12&&fRegion==2&&fPackage==2");
  E24->SetTitle("fElement: Package 2 Plane 12");
  

}
