// Name  :   VDC_plots
// Author:   David Armstrong
// email :   armd@jlab.org
// Date:     Fri July 16 19:41:04 EDT 2010
// Version:  0.3
//

//  Utility macros for looking at rootfiles from Tracking analysis for VDC information
//    - all of these are "hit-based", not "event-based"
//    - written rather primitively, with lots of hard-coded specifics, for simplicity
//     (yes, I know, there are more elegant ways of doing much of this)
//
//    Usage:  root -l <root file>  
//         root [1] plot_planes()        
//         root [2] plot_delaylines()
//         root [3] plot_times()
//         root [4] plot_hitnumber()
//         root [5] plot_all()        
  
//   updated Oct 29 2010: changed "tree" to "event_tree" and also, need to us qwroot, not just root tio run this now
void plot_planes(){

gROOT->Reset();

  new TCanvas("wires_c","Vader and Leia (Package 1)    Wires Hit",-20,0,450,450);
  TH1F* h_wires_hit_1=NULL;
  TH1F* h_wires_hit_2=NULL;
  TH1F* h_wires_hit_3=NULL;
  TH1F* h_wires_hit_4=NULL;
  h_wires_hit_1= new TH1F("h_wires_hit_1", "Vader wire hits  V-Plane ", 280, 0.5, 280.5);
  h_wires_hit_2= new TH1F("h_wires_hit_2", "Vader wire hits  U-Plane ", 280, 0.5, 280.5);
  h_wires_hit_3= new TH1F("h_wires_hit_3", "Leia wire hits  V-Plane ", 280, 0.5, 280.5);
  h_wires_hit_4= new TH1F("h_wires_hit_4", "Leia wire hits U-Plane ", 280, 0.5, 280.5);
  event_tree->Draw("fElement>>h_wires_hit_1","fRegion==3&&fPackage==1&&fPlane==1","Q");
  event_tree->Draw("fElement>>h_wires_hit_2","fRegion==3&&fPackage==1&&fPlane==2","Q");
  event_tree->Draw("fElement>>h_wires_hit_3","fRegion==3&&fPackage==1&&fPlane==3","Q");
  event_tree->Draw("fElement>>h_wires_hit_4","fRegion==3&&fPackage==1&&fPlane==4","Q");
  h_wires_hit_1 -> SetLineColor(2);
  h_wires_hit_1 -> GetXaxis()-> SetTitle("Wire #    Plane 1");
  h_wires_hit_2 -> SetLineColor(2);
  h_wires_hit_2 -> GetXaxis()-> SetTitle("Wire #    Plane 2");
  h_wires_hit_3 -> SetLineColor(4);
  h_wires_hit_3 -> GetXaxis()-> SetTitle("Wire #    Plane 3");
  h_wires_hit_4 -> SetLineColor(4);
  h_wires_hit_4 -> GetXaxis()-> SetTitle("Wire #    Plane 4");


  gStyle->SetOptStat(1111);

  wires_c->Clear();
  wires_c->Divide(2,2);
  wires_c->cd(1);
  h_wires_hit_1 -> Draw();
  wires_c->cd(2);
  h_wires_hit_2 -> Draw();
  wires_c->cd(3);
  h_wires_hit_3 -> Draw();
  wires_c->cd(4);
  h_wires_hit_4 -> Draw();

// -----------------------------------------------------------------------------

  new TCanvas("wires_c2","Yoda and Han (Package 2)    Wires Hit",500,0,450,450);
  TH1F* h_wires_hit_5=NULL;
  TH1F* h_wires_hit_6=NULL;
  TH1F* h_wires_hit_7=NULL;
  TH1F* h_wires_hit_8=NULL;
  h_wires_hit_5= new TH1F("h_wires_hit_5", "Yoda wire hits  V-Plane ", 280, 0.5, 280.5);
  h_wires_hit_6= new TH1F("h_wires_hit_6", "Yoda wire hits  U-Plane ", 280, 0.5, 280.5);
  h_wires_hit_7= new TH1F("h_wires_hit_7", "Han wire hits  V-Plane ", 280, 0.5, 280.5);
  h_wires_hit_8= new TH1F("h_wires_hit_8", "Han wire hits U-Plane ", 280, 0.5, 280.5);
  event_tree->Draw("fElement>>h_wires_hit_5","fRegion==3&&fPackage==2&&fPlane==1","Q");
  event_tree->Draw("fElement>>h_wires_hit_6","fRegion==3&&fPackage==2&&fPlane==2","Q");
  event_tree->Draw("fElement>>h_wires_hit_7","fRegion==3&&fPackage==2&&fPlane==3","Q");
  event_tree->Draw("fElement>>h_wires_hit_8","fRegion==3&&fPackage==2&&fPlane==4","Q");
  h_wires_hit_5 -> SetLineColor(3);
  h_wires_hit_5 -> GetXaxis()-> SetTitle("Wire #    Plane 1");
  h_wires_hit_6 -> SetLineColor(3);
  h_wires_hit_6 -> GetXaxis()-> SetTitle("Wire #    Plane 2");
  h_wires_hit_7 -> SetLineColor(6);
  h_wires_hit_7 -> GetXaxis()-> SetTitle("Wire #    Plane 3");
  h_wires_hit_8 -> SetLineColor(6);
  h_wires_hit_8 -> GetXaxis()-> SetTitle("Wire #    Plane 4");

  gStyle->SetOptStat(1111);
  wires_c2->Clear();
  wires_c2->Divide(2,2);
  wires_c2->cd(1);
  h_wires_hit_5 -> Draw();
  wires_c2->cd(2);
  h_wires_hit_6 -> Draw();
  wires_c2->cd(3);
  h_wires_hit_7 -> Draw();
  wires_c2->cd(4);
  h_wires_hit_8 -> Draw();

// -----------------------------------------------------------------------------

  new TCanvas("ambig_c","Vader and Lei (Package 1)    Ambiguous Wires",-20,500,450,450);

  TH1F* h_wires_amb_1=NULL;
  TH1F* h_wires_amb_2=NULL;
  TH1F* h_wires_amb_3=NULL;
  TH1F* h_wires_amb_4=NULL;
  h_wires_amb_1= new TH1F("h_wires_amb_1", "Vader ambiguous hits V-Plane", 280, 0.5, 280.5);
  h_wires_amb_2= new TH1F("h_wires_amb_2", "Vader ambiguous hits  U-Plane", 280, 0.5, 280.5);
  h_wires_amb_3= new TH1F("h_wires_amb_3", "Leia ambiguous hits  V-Plane", 280, 0.5, 280.5);
  h_wires_amb_4= new TH1F("h_wires_amb_4", "Leia ambiguous hits  U-Plane", 280, 0.5, 280.5);
  event_tree->Draw("fElement>>h_wires_amb_1","fRegion==3&&fPackage==1&&fPlane==1&&fAmbiguousElement==1","Q");
  event_tree->Draw("fElement>>h_wires_amb_2","fRegion==3&&fPackage==1&&fPlane==2&&fAmbiguousElement==1","Q");
  event_tree->Draw("fElement>>h_wires_amb_3","fRegion==3&&fPackage==1&&fPlane==3&&fAmbiguousElement==1","Q");
  event_tree->Draw("fElement>>h_wires_amb_4","fRegion==3&&fPackage==1&&fPlane==4&&fAmbiguousElement==1","Q");
  h_wires_amb_1 -> SetLineColor(3);
  h_wires_amb_1 -> GetXaxis()-> SetTitle("Wire #    Plane 1");
  h_wires_amb_2 -> SetLineColor(3);
  h_wires_amb_2 -> GetXaxis()-> SetTitle("Wire #    Plane 2");
  h_wires_amb_3 -> SetLineColor(6);
  h_wires_amb_3 -> GetXaxis()-> SetTitle("Wire #    Plane 3");
  h_wires_amb_4 -> SetLineColor(6);
  h_wires_amb_4 -> GetXaxis()-> SetTitle("Wire #    Plane 4");

  gStyle->SetOptStat(1111);
  ambig_c->Clear();
  ambig_c->Divide(2,2);
  ambig_c->cd(1);
  h_wires_amb_1 -> Draw();
  ambig_c->cd(2);
  h_wires_amb_2 -> Draw();
  ambig_c->cd(3);
  h_wires_amb_3 -> Draw();
  ambig_c->cd(4);
  h_wires_amb_4 -> Draw();
  
// -----------------------------------------------------------------------------

  new TCanvas("ambig_c2","Yoda and Han (Package 2)    Ambiguous Wires",-500,500,450,450);

  TH1F* h_wires_amb_5=NULL;
  TH1F* h_wires_amb_6=NULL;
  TH1F* h_wires_amb_7=NULL;
  TH1F* h_wires_amb_8=NULL;
  h_wires_amb_5= new TH1F("h_wires_amb_5", "Yoda ambiguous hits  V-Plane", 280, 0.5, 280.5);
  h_wires_amb_6= new TH1F("h_wires_amb_6", "Yoda ambiguous hits  U-Plane", 280, 0.5, 280.5);
  h_wires_amb_7= new TH1F("h_wires_amb_7", "Han ambiguous hits  V-Plane", 280, 0.5, 280.5);
  h_wires_amb_8= new TH1F("h_wires_amb_8", "Han ambiguous hits  U-Plane", 280, 0.5, 280.5);
  event_tree->Draw("fElement>>h_wires_amb_5","fRegion==3&&fPackage==2&&fPlane==1&&fAmbiguousElement==1","Q");
  event_tree->Draw("fElement>>h_wires_amb_6","fRegion==3&&fPackage==2&&fPlane==2&&fAmbiguousElement==1","Q");
  event_tree->Draw("fElement>>h_wires_amb_7","fRegion==3&&fPackage==2&&fPlane==3&&fAmbiguousElement==1","Q");
  event_tree->Draw("fElement>>h_wires_amb_8","fRegion==3&&fPackage==2&&fPlane==4&&fAmbiguousElement==1","Q");
  h_wires_amb_5 -> SetLineColor(3);
  h_wires_amb_5 -> GetXaxis()-> SetTitle("Wire #    Plane 1");
  h_wires_amb_6 -> SetLineColor(3);
  h_wires_amb_6 -> GetXaxis()-> SetTitle("Wire #    Plane 2");
  h_wires_amb_7 -> SetLineColor(6);
  h_wires_amb_7 -> GetXaxis()-> SetTitle("Wire #    Plane 3");
  h_wires_amb_8 -> SetLineColor(6);
  h_wires_amb_8 -> GetXaxis()-> SetTitle("Wire #    Plane 4");

  gStyle->SetOptStat(1111);
  ambig_c2->Clear();
  ambig_c2->Divide(2,2);
  ambig_c2->cd(1);
  h_wires_amb_5 -> Draw();
  ambig_c2->cd(2);
  h_wires_amb_6 -> Draw();
  ambig_c2->cd(3);
  h_wires_amb_7 -> Draw();
  ambig_c2->cd(4);
  h_wires_amb_8 -> Draw();
  
// -----------------------------------------------------------------------------

 new TCanvas("planes_c","VDC   Planes with Hits",900,0,350,350);
 TH1F* h_planes_hit_1=NULL;
 TH1F* h_planes_hit_2=NULL;
 h_planes_hit_1= new TH1F("h_planes_hit_1", "VDC Planes Hit  Package 1", 6, 0, 6);
 h_planes_hit_2= new TH1F("h_planes_hit_2", "VDC Planes Hit  Package 2", 6, 0, 6);
 event_tree->Draw("fPlane>>h_planes_hit_1","fRegion==3&&fPackage==1","Q");
 event_tree->Draw("fPlane>>h_planes_hit_2","fRegion==3&&fPackage==2","Q");
 h_planes_hit_1 -> SetLineColor(7);
 h_planes_hit_1 -> SetFillColor(7);
 h_planes_hit_1 -> GetXaxis()-> SetTitle("Plane");
 h_planes_hit_2 -> SetLineColor(8);
 h_planes_hit_2 -> SetFillColor(8);
 h_planes_hit_2 -> GetXaxis()-> SetTitle("Plane");

 gStyle->SetOptStat(1111);
 planes_c->Clear();
 planes_c->Divide(1,2);
 planes_c->cd(1);
 h_planes_hit_1->Draw();
 planes_c->cd(2);
 h_planes_hit_2->Draw();

// -----------------------------------------------------------------------------

 new TCanvas("am_planes_c","VDC   Planes with Ambiguous Hits",900,400,350,350);
 TH1F* h_planes_am_hit_1=NULL;
 TH1F* h_planes_am_hit_2=NULL;
 h_planes_am_hit_1= new TH1F("h_planes_am_hit_1", "VDC Planes Ambiguous Hits  Package 1", 6, 0, 6);
 h_planes_am_hit_2= new TH1F("h_planes_am_hit_2", "VDC Planes Ambiguous Hits  Package 2", 6, 0, 6);
 event_tree->Draw("fPlane>>h_planes_am_hit_1","fRegion==3&&fPackage==1&&fAmbiguousElement==1","Q");
 event_tree->Draw("fPlane>>h_planes_am_hit_2","fRegion==3&&fPackage==2&&fAmbiguousElement==1","Q");
 h_planes_am_hit_1 -> SetLineColor(7);
 h_planes_am_hit_1 -> SetFillColor(7);
 h_planes_am_hit_1 -> GetXaxis()-> SetTitle("Plane");
 h_planes_am_hit_2 -> SetLineColor(8);
 h_planes_am_hit_2 -> SetFillColor(8);
 h_planes_am_hit_2 -> GetXaxis()-> SetTitle("Plane");

 gStyle->SetOptStat(1111);
 am_planes_c->Clear();
 am_planes_c->Divide(1,2);
 am_planes_c->cd(1);
 h_planes_am_hit_1->Draw();
 am_planes_c->cd(2);
 h_planes_am_hit_2->Draw();

}

// -----------------------------------------------------------------------------

void plot_delaylines(){

  gROOT->Reset();

  new TCanvas("delaylines_c"," Delay Line hits",-20,0,450,450);

  TH1F* h_delay_line_1=NULL;
  TH1F* h_delay_line_2=NULL;
  TH1F* h_delay_line_3=NULL;
  TH1F* h_delay_line_4=NULL;
  h_delay_line_1= new TH1F("h_delay_line_1", "MUX crate 1 (Vader) delay lines", 33, 0, 66);
  h_delay_line_2= new TH1F("h_delay_line_2", "MUX crate 2 (Leia) delay lines", 33,  0, 66);
  h_delay_line_3= new TH1F("h_delay_line_3", "MUX crate 3 (Yoda) delay lines", 33, 0, 66);
  h_delay_line_4= new TH1F("h_delay_line_4", "MUX crate 4 (Han) delay lines", 33, 0, 66);
  event_tree->Draw("fChannel>>h_delay_line_1","fRegion==3&&fPackage==1&&(fPlane==1||fPlane==2)","Q");
  event_tree->Draw("fChannel>>h_delay_line_2","fRegion==3&&fPackage==1&&(fPlane==3||fPlane==4)","Q");
  event_tree->Draw("fChannel>>h_delay_line_3","fRegion==3&&fPackage==2&&(fPlane==1||fPlane==2)","Q");
  event_tree->Draw("fChannel>>h_delay_line_4","fRegion==3&&fPackage==2&&(fPlane==3||fPlane==4)","Q");
  h_delay_line_1 -> SetLineColor(2);
  h_delay_line_1 -> SetFillColor(2);
  h_delay_line_1 -> GetXaxis()-> SetTitle("TDC Channel");
  h_delay_line_2 -> SetLineColor(4);
  h_delay_line_2 -> SetFillColor(4);
  h_delay_line_2 -> GetXaxis()-> SetTitle("TDC Channel");
  h_delay_line_3 -> SetLineColor(3);
  h_delay_line_3 -> SetFillColor(3);
  h_delay_line_3 -> GetXaxis()-> SetTitle("TDC Channel");
  h_delay_line_4 -> SetLineColor(6);
  h_delay_line_4 -> SetFillColor(6);
  h_delay_line_4 -> GetXaxis()-> SetTitle("TDC Channel");

  gStyle->SetOptStat(10);
  delaylines_c->Clear();
  delaylines_c->Divide(2,2);
  delaylines_c->cd(1);
  h_delay_line_1 -> Draw();
  delaylines_c->cd(2);
  h_delay_line_2 -> Draw();
  delaylines_c->cd(3);
  h_delay_line_3 -> Draw();
  delaylines_c->cd(4);
  h_delay_line_4 -> Draw();

}

// -----------------------------------------------------------------------------

void plot_times(){

  gROOT->Reset();

  new TCanvas("times_c","Drift Times for VDCs",0,50,1000,500);

  // should be fancy and use histo copies here

  TH1F* h_time_1=NULL;
  TH1F* h_time_2=NULL;
  TH1F* h_time_3=NULL;
  TH1F* h_time_4=NULL;
  TH1F* h_time_5=NULL;
  TH1F* h_time_6=NULL;
  TH1F* h_time_7=NULL;
  TH1F* h_time_8=NULL;
  
  h_time_1= new TH1F("h_time_1", "Vader Drift Time", 250,-100, 500);
  h_time_2= new TH1F("h_time_2", "Leia Drift Time", 250, -100, 500);
  h_time_3= new TH1F("h_time_3", "Yoda Drift Time", 250, -100, 500);
  h_time_4= new TH1F("h_time_4", "Han Drift Time", 250, -100, 500);
  h_time_5= new TH1F("h_time_5", "Vader Drift Time", 250, -300, 1300);
  h_time_6= new TH1F("h_time_6", "Leia Drift Time", 250, -300, 1300);
  h_time_7= new TH1F("h_time_7", "Yoda Drift Time", 250, -300, 1300);
  h_time_8= new TH1F("h_time_8", "Han Drift Time", 250, -300, 1300);

  event_tree->Draw("fTimeNs>>h_time_1","fRegion==3&&fPackage==1&&(fPlane==1||fPlane==2)","Q");
  event_tree->Draw("fTimeNs>>h_time_2","fRegion==3&&fPackage==1&&(fPlane==3||fPlane==4)","Q");
  event_tree->Draw("fTimeNs>>h_time_3","fRegion==3&&fPackage==2&&(fPlane==1||fPlane==2)","Q");
  event_tree->Draw("fTimeNs>>h_time_4","fRegion==3&&fPackage==2&&(fPlane==3||fPlane==4)","Q");
  event_tree->Draw("fTimeNs>>h_time_5","fRegion==3&&fPackage==1&&(fPlane==1||fPlane==2)","Q");
  event_tree->Draw("fTimeNs>>h_time_6","fRegion==3&&fPackage==1&&(fPlane==3||fPlane==4)","Q");
  event_tree->Draw("fTimeNs>>h_time_7","fRegion==3&&fPackage==2&&(fPlane==1||fPlane==2)","Q");
  event_tree->Draw("fTimeNs>>h_time_8","fRegion==3&&fPackage==2&&(fPlane==3||fPlane==4)","Q");
  h_time_1 -> SetLineColor(2);
  h_time_1 -> GetXaxis()-> SetTitle("Drift Time   (ns)  Vader");
  h_time_2 -> SetLineColor(4);
  h_time_2 -> GetXaxis()-> SetTitle("Drift Time   (ns)  Leia");
  h_time_3 -> SetLineColor(3);
  h_time_3 -> GetXaxis()-> SetTitle("Drift Time   (ns)  Yoda");
  h_time_4 -> SetLineColor(6);
  h_time_4 -> GetXaxis()-> SetTitle("Drift Time   (ns)  Han");
  h_time_5 -> SetLineColor(2);
  h_time_5 -> GetXaxis()-> SetTitle("Drift Time   (ns)  Vader");
  h_time_6 -> SetLineColor(4);
  h_time_6 -> GetXaxis()-> SetTitle("Drift Time   (ns)  Leia");
  h_time_7 -> SetLineColor(3);
  h_time_7 -> GetXaxis()-> SetTitle("Drift Time   (ns)  Yoda");
  h_time_8 -> SetLineColor(6);
  h_time_8 -> GetXaxis()-> SetTitle("Drift Time   (ns)  Han");

  gStyle->SetOptStat(10);
  times_c->Clear();
  times_c->Divide(4,2);
  times_c->cd(1);
  h_time_1 -> Draw();
  times_c->cd(2);
  h_time_2 -> Draw();
  times_c->cd(3);
  h_time_3 -> Draw();
  times_c->cd(4);
  h_time_4 -> Draw();
  times_c->cd(5);
  gPad->SetLogy();
  h_time_5 -> Draw();
  times_c->cd(6);
  gPad->SetLogy();
  h_time_6 -> Draw();
  times_c->cd(7);
  gPad->SetLogy();
  h_time_7 -> Draw();
  times_c->cd(8);
  gPad->SetLogy();
  h_time_8 -> Draw();

}

// ----------------------------------------------------------------------------

void plot_hitnumber(){

  gROOT->Reset();

  new TCanvas("nhits_c"," VDC Hit Multiplicity (hits/wire in an event)",-20,0,450,450);
  TH1F* h_nhits_1=NULL;
  TH1F* h_nhits_2=NULL;
  TH1F* h_nhits_3=NULL;
  TH1F* h_nhits_4=NULL;
  h_nhits_1= new TH1F("h_nhits_1", "Hit Multiplicity (Vader) ", 12, 0, 12);
  h_nhits_2= new TH1F("h_nhits_2", "Hit Multiplicity (Leia) ", 12,  0, 12);
  h_nhits_3= new TH1F("h_nhits_3", "Hit Multiplicity (Yoda) ", 12, 0, 12);
  h_nhits_4= new TH1F("h_nhits_4", "Hit Multiplicity (Han) ", 12, 0, 12);
  event_tree->Draw("fHitNumber>>h_nhits_1","fRegion==3&&fPackage==1&&(fPlane==1||fPlane==2)","Q");
  event_tree->Draw("fHitNumber>>h_nhits_2","fRegion==3&&fPackage==1&&(fPlane==3||fPlane==4)","Q");
  event_tree->Draw("fHitNumber>>h_nhits_3","fRegion==3&&fPackage==2&&(fPlane==1||fPlane==2)","Q");
  event_tree->Draw("fHitNumber>>h_nhits_4","fRegion==3&&fPackage==2&&(fPlane==3||fPlane==4)","Q");
  h_nhits_1 -> SetLineColor(2);
  h_nhits_1 -> SetFillColor(2);
  h_nhits_1 -> GetXaxis()-> SetTitle("Number of Hits");
  h_nhits_2 -> SetLineColor(4);
  h_nhits_2 -> SetFillColor(4);
  h_nhits_2 -> GetXaxis()-> SetTitle("Number of Hits");
  h_nhits_3 -> SetLineColor(3);
  h_nhits_3 -> SetFillColor(3);
  h_nhits_3 -> GetXaxis()-> SetTitle("Number of Hits");
  h_nhits_4 -> SetLineColor(6);
  h_nhits_4 -> SetFillColor(6);
  h_nhits_4 -> GetXaxis()-> SetTitle("Number of Hits");

  gStyle->SetOptStat(1111);
  nhits_c->Clear();
  nhits_c->Divide(2,2);
  nhits_c->cd(1);
  gPad->SetLogy();
  h_nhits_1 -> Draw();
  nhits_c->cd(2);
  gPad->SetLogy();
  h_nhits_2 -> Draw();
  nhits_c->cd(3);
  gPad->SetLogy();
  h_nhits_3 -> Draw();
  nhits_c->cd(4);
  gPad->SetLogy();
  h_nhits_4 -> Draw();
}

void all_plots_vdc(TString name="")
{ 
  TString file_dir;
  file_dir = gSystem->Getenv("QWSCRATCH");
  file_dir += "/rootfiles/";
 
  if(name.IsNull()) {
    printf("There is no root file\n");
    exit(-1);
  }
  else {
    TFile *file =  new TFile(Form("%s%s", file_dir.Data(), name.Data()));
  }
  gStyle->SetPalette(1); 

  //plots them all - many canvases!!

  gROOT->Reset();

  plot_planes();
  plot_delaylines();
  plot_times();
  plot_hitnumber();
}
