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
//   updated Jan 31 2012 (jc2): Code clean up to make it work with the web
//        auto plotter

//  updated July 25, 2012 changed the scrpit so that it goes through the tree only once
//  made the 3 functions each into 3 (so there is a total of 9 functions
//  one "old" function is now an initl_????? function which is passed nothing and initllizes all the 
//  histograms, an fill_??? function witch gets passed a hit from the an event in the
//  rootfile and looks at that this the nesscary critera and fills any necssary histgrams 
//  based on those cuts, and a plot_???? function which is passed nothing and then plots 
//  the histograms created and filled in the corresponding fuctions to a canvas and 
//  saves them.  For all of these functions the global are the historgrams.
//  this was done by Wouter Deconinck and Valerie Gray to save time as now the scrpit only
//  goes through the event_tree once and  now the each website page can be created a bit faster :)
//  updated May 14 2014 DSA: added some include files for compilation
//                         : save the .png plots for the drift times and delaylines.
//                         : fix bug in filling plot for ambiguous hits/plane in package 2

#include "auto_shared_includes.h"

#include "TSystem.h"
#include "TF1.h"
#include "TLeaf.h"

#include "TCanvas.h"
#include "TChain.h"
#include "TF1.h"
#include "TStyle.h"
#include "TPaveStats.h"
#include "TList.h"

#include "QwHit.h"
#include "QwEvent.h"

TString outputPrefix;

//define some (other come later) the needed histograms for the script as globals
TH1F* h_wires_hit_1=NULL;
TH1F* h_wires_hit_2=NULL;
TH1F* h_wires_hit_3=NULL;
TH1F* h_wires_hit_4=NULL;
TH1F* h_wires_hit_5=NULL;
TH1F* h_wires_hit_6=NULL;
TH1F* h_wires_hit_7=NULL;
TH1F* h_wires_hit_8=NULL;

TH1F* h_wires_amb_1=NULL;
TH1F* h_wires_amb_2=NULL;
TH1F* h_wires_amb_3=NULL;
TH1F* h_wires_amb_4=NULL;
TH1F* h_wires_amb_5=NULL;
TH1F* h_wires_amb_6=NULL;
TH1F* h_wires_amb_7=NULL;
TH1F* h_wires_amb_8=NULL;

TH1F* h_planes_hit_1=NULL;
TH1F* h_planes_hit_2=NULL;
TH1F* h_planes_am_hit_1=NULL;
TH1F* h_planes_am_hit_2=NULL;

void init_planes()
{
  h_wires_hit_1= new TH1F("h_wires_hit_1", "Vader wire hits  V-Plane ", 280, 0.5, 280.5);
  h_wires_hit_2= new TH1F("h_wires_hit_2", "Vader wire hits  U-Plane ", 280, 0.5, 280.5);
  h_wires_hit_3= new TH1F("h_wires_hit_3", "Leia wire hits  V-Plane ", 280, 0.5, 280.5);
  h_wires_hit_4= new TH1F("h_wires_hit_4", "Leia wire hits U-Plane ", 280, 0.5, 280.5);
  h_wires_hit_5= new TH1F("h_wires_hit_5", "Yoda wire hits  V-Plane ", 280, 0.5, 280.5);
  h_wires_hit_6= new TH1F("h_wires_hit_6", "Yoda wire hits  U-Plane ", 280, 0.5, 280.5);
  h_wires_hit_7= new TH1F("h_wires_hit_7", "Han wire hits  V-Plane ", 280, 0.5, 280.5);
  h_wires_hit_8= new TH1F("h_wires_hit_8", "Han wire hits U-Plane ", 280, 0.5, 280.5);

  h_wires_amb_1= new TH1F("h_wires_amb_1", "Vader ambiguous hits V-Plane", 280, 0.5, 280.5);
  h_wires_amb_2= new TH1F("h_wires_amb_2", "Vader ambiguous hits  U-Plane", 280, 0.5, 280.5);
  h_wires_amb_3= new TH1F("h_wires_amb_3", "Leia ambiguous hits  V-Plane", 280, 0.5, 280.5);
  h_wires_amb_4= new TH1F("h_wires_amb_4", "Leia ambiguous hits  U-Plane", 280, 0.5, 280.5);
  h_wires_amb_5= new TH1F("h_wires_amb_5", "Yoda ambiguous hits  V-Plane", 280, 0.5, 280.5);
  h_wires_amb_6= new TH1F("h_wires_amb_6", "Yoda ambiguous hits  U-Plane", 280, 0.5, 280.5);
  h_wires_amb_7= new TH1F("h_wires_amb_7", "Han ambiguous hits  V-Plane", 280, 0.5, 280.5);
  h_wires_amb_8= new TH1F("h_wires_amb_8", "Han ambiguous hits  U-Plane", 280, 0.5, 280.5);

  h_planes_hit_1= new TH1F("h_planes_hit_1", "VDC Planes Hit  Package 1", 6, 0, 6);
  h_planes_hit_2= new TH1F("h_planes_hit_2", "VDC Planes Hit  Package 2", 6, 0, 6);

  h_planes_am_hit_1= new TH1F("h_planes_am_hit_1", "VDC Planes Ambiguous Hits  Package 1", 6, 0, 6);
  h_planes_am_hit_2= new TH1F("h_planes_am_hit_2", "VDC Planes Ambiguous Hits  Package 2", 6, 0, 6);
}

void fill_planes(const QwHit* hit, const double w)
{
  if (hit->fRegion==3&&hit->fPackage==1&&hit->fPlane==1)
    h_wires_hit_1->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==1&&hit->fPlane==2)
    h_wires_hit_2->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==1&&hit->fPlane==3)
    h_wires_hit_3->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==1&&hit->fPlane==4)
    h_wires_hit_4->Fill(hit->fElement,w);

  if (hit->fRegion==3&&hit->fPackage==2&&hit->fPlane==1)
    h_wires_hit_5->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==2&&hit->fPlane==2)
    h_wires_hit_6->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==2&&hit->fPlane==3)
    h_wires_hit_7->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==2&&hit->fPlane==4)
    h_wires_hit_8->Fill(hit->fElement,w);

  if (hit->fRegion==3&&hit->fPackage==1&&hit->fPlane==1&&hit->fAmbiguousElement==1)
    h_wires_amb_1->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==1&&hit->fPlane==2&&hit->fAmbiguousElement==1)
    h_wires_amb_2->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==1&&hit->fPlane==3&&hit->fAmbiguousElement==1)
    h_wires_amb_3->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==1&&hit->fPlane==4&&hit->fAmbiguousElement==1)
    h_wires_amb_4->Fill(hit->fElement,w);

  if (hit->fRegion==3&&hit->fPackage==2&&hit->fPlane==1&&hit->fAmbiguousElement==1)
    h_wires_amb_5->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==2&&hit->fPlane==2&&hit->fAmbiguousElement==1)
    h_wires_amb_6->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==2&&hit->fPlane==3&&hit->fAmbiguousElement==1)
    h_wires_amb_7->Fill(hit->fElement,w);
  if (hit->fRegion==3&&hit->fPackage==2&&hit->fPlane==4&&hit->fAmbiguousElement==1)
    h_wires_amb_8->Fill(hit->fElement,w);

  if (hit->fRegion==3&&hit->fPackage==1)
    h_planes_hit_1->Fill(hit->fPlane,w);
  if (hit->fRegion==3&&hit->fPackage==2)
    h_planes_hit_2->Fill(hit->fPlane,w);

  if (hit->fRegion==3&&hit->fPackage==1&&hit->fAmbiguousElement==1)
    h_planes_am_hit_1->Fill(hit->fPlane,w);
  if (hit->fRegion==3&&hit->fPackage==2&&hit->fAmbiguousElement==1)
    h_planes_am_hit_2->Fill(hit->fPlane,w);
}


void plot_planes()
{
  TCanvas *wires_c = new TCanvas("wires_c","Vader and Leia (Package 1)    Wires Hit",-20,0,450,450);
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
  wires_c->SaveAs(outputPrefix+"wires_package1.png");
  wires_c->SaveAs(outputPrefix+"wires_package1.C");

// -----------------------------------------------------------------------------

  TCanvas *wires_c2 = new TCanvas("wires_c2","Yoda and Han (Package 2)    Wires Hit",500,0,450,450);
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
  wires_c2->SaveAs(outputPrefix+"wires_package2.png");
  wires_c2->SaveAs(outputPrefix+"wires_package2.C");

// -----------------------------------------------------------------------------

  TCanvas *ambig_c = new TCanvas("ambig_c","Vader and Lei (Package 1)    Ambiguous Wires",-20,500,450,450);
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
  ambig_c->SaveAs(outputPrefix+"wires_ambig_package1.png");
  ambig_c->SaveAs(outputPrefix+"wires_ambig_package1.C");
  
// -----------------------------------------------------------------------------

  TCanvas *ambig_c2 = new TCanvas("ambig_c2","Yoda and Han (Package 2)    Ambiguous Wires",-500,500,450,450);
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
  ambig_c2->SaveAs(outputPrefix+"wires_ambig_package2.png");
  ambig_c2->SaveAs(outputPrefix+"wires_ambig_package2.C");
  
// -----------------------------------------------------------------------------

  TCanvas *planes_c = new TCanvas("planes_c","VDC   Planes with Hits",900,0,350,350);
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
  planes_c->SaveAs(outputPrefix+"planes_hits.png");
  planes_c->SaveAs(outputPrefix+"planes_hits.C");

// -----------------------------------------------------------------------------

  TCanvas *am_planes_c = new TCanvas("am_planes_c","VDC   Planes with Ambiguous Hits",900,400,350,350);
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
  am_planes_c->SaveAs(outputPrefix+"planes_hit_ambigious.png");
  am_planes_c->SaveAs(outputPrefix+"planes_hit_ambigious.C");

}

// -----------------------------------------------------------------------------

TH1F* h_delay_line_1=NULL;
TH1F* h_delay_line_2=NULL;
TH1F* h_delay_line_3=NULL;
TH1F* h_delay_line_4=NULL;

void init_delaylines()
{
  h_delay_line_1= new TH1F("h_delay_line_1", "MUX crate 1 (Vader) delay lines", 33, 0, 66);
  h_delay_line_2= new TH1F("h_delay_line_2", "MUX crate 2 (Leia) delay lines", 33,  0, 66);
  h_delay_line_3= new TH1F("h_delay_line_3", "MUX crate 3 (Yoda) delay lines", 33, 0, 66);
  h_delay_line_4= new TH1F("h_delay_line_4", "MUX crate 4 (Han) delay lines", 33, 0, 66);
}

void fill_delaylines(const QwHit* hit, const double w)
{
  if (hit->fRegion==3&&hit->fPackage==1&&(hit->fPlane==1||hit->fPlane==2))
    h_delay_line_1->Fill(hit->fChannel,w);
  if (hit->fRegion==3&&hit->fPackage==1&&(hit->fPlane==3||hit->fPlane==4))
    h_delay_line_2->Fill(hit->fChannel,w);
  if (hit->fRegion==3&&hit->fPackage==2&&(hit->fPlane==1||hit->fPlane==2))
    h_delay_line_3->Fill(hit->fChannel,w);
  if (hit->fRegion==3&&hit->fPackage==2&&(hit->fPlane==3||hit->fPlane==4))
    h_delay_line_4->Fill(hit->fChannel,w);
}

void plot_delaylines()
{
  TCanvas *delaylines_c = new TCanvas("delaylines_c"," Delay Line hits",-20,0,450,450);
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
   delaylines_c->SaveAs(outputPrefix+"delaylines_hit.png");
   delaylines_c->SaveAs(outputPrefix+"delaylines_hit.C");
  //  delaylines_c->Draw(outputPrefix+"delaylines_hit.png");

}

// -----------------------------------------------------------------------------

TH1F* h_time_1=NULL;
TH1F* h_time_2=NULL;
TH1F* h_time_3=NULL;
TH1F* h_time_4=NULL;
TH1F* h_time_5=NULL;
TH1F* h_time_6=NULL;
TH1F* h_time_7=NULL;
TH1F* h_time_8=NULL;

void init_times()
{
  h_time_1= new TH1F("h_time_1", "Vader Drift Time", 250,-100, 500);
  h_time_2= new TH1F("h_time_2", "Leia Drift Time", 250, -100, 500);
  h_time_3= new TH1F("h_time_3", "Yoda Drift Time", 250, -100, 500);
  h_time_4= new TH1F("h_time_4", "Han Drift Time", 250, -100, 500);
  h_time_5= new TH1F("h_time_5", "Vader Drift Time", 250, -300, 1300);
  h_time_6= new TH1F("h_time_6", "Leia Drift Time", 250, -300, 1300);
  h_time_7= new TH1F("h_time_7", "Yoda Drift Time", 250, -300, 1300);
  h_time_8= new TH1F("h_time_8", "Han Drift Time", 250, -300, 1300);
}

void fill_times(const QwHit* hit, const double w)
{
  if (hit->fRegion==3&&hit->fPackage==1&&(hit->fPlane==1||hit->fPlane==2))
    h_time_1->Fill(hit->fTimeNs,w);
  if (hit->fRegion==3&&hit->fPackage==1&&(hit->fPlane==3||hit->fPlane==4))
    h_time_2->Fill(hit->fTimeNs,w);
  if (hit->fRegion==3&&hit->fPackage==2&&(hit->fPlane==1||hit->fPlane==2))
    h_time_3->Fill(hit->fTimeNs,w);
  if (hit->fRegion==3&&hit->fPackage==2&&(hit->fPlane==3||hit->fPlane==4))
    h_time_4->Fill(hit->fTimeNs,w);
  if (hit->fRegion==3&&hit->fPackage==1&&(hit->fPlane==1||hit->fPlane==2))
    h_time_5->Fill(hit->fTimeNs,w);
  if (hit->fRegion==3&&hit->fPackage==1&&(hit->fPlane==3||hit->fPlane==4))
    h_time_6->Fill(hit->fTimeNs,w);
  if (hit->fRegion==3&&hit->fPackage==2&&(hit->fPlane==1||hit->fPlane==2))
    h_time_7->Fill(hit->fTimeNs,w);
  if (hit->fRegion==3&&hit->fPackage==2&&(hit->fPlane==3||hit->fPlane==4))
    h_time_8->Fill(hit->fTimeNs,w);
}

void plot_times()
{
  TCanvas *times_c = new TCanvas("times_c","Drift Times for VDCs",0,50,1000,500);
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
  //  times_c->Draw(outputPrefix+"drifttimes.png");
  times_c->SaveAs(outputPrefix+"drifttimes.png");
  times_c->SaveAs(outputPrefix+"drifttimes.C");

}

// ----------------------------------------------------------------------------

TH1F* h_nhits_1=NULL;
TH1F* h_nhits_2=NULL;
TH1F* h_nhits_3=NULL;
TH1F* h_nhits_4=NULL;

void init_hitnumber()
{
  h_nhits_1= new TH1F("h_nhits_1", "Hit Multiplicity (Vader) ", 12, 0, 12);
  h_nhits_2= new TH1F("h_nhits_2", "Hit Multiplicity (Leia) ", 12,  0, 12);
  h_nhits_3= new TH1F("h_nhits_3", "Hit Multiplicity (Yoda) ", 12, 0, 12);
  h_nhits_4= new TH1F("h_nhits_4", "Hit Multiplicity (Han) ", 12, 0, 12);
}

void fill_hitnumber(const QwHit* hit, const double w)
{
  if (hit->fRegion==3&&hit->fPackage==1&&(hit->fPlane==1||hit->fPlane==2))
    h_nhits_1->Fill(hit->fHitNumber,w);
  if (hit->fRegion==3&&hit->fPackage==1&&(hit->fPlane==3||hit->fPlane==4))
    h_nhits_2->Fill(hit->fHitNumber,w);
  if (hit->fRegion==3&&hit->fPackage==2&&(hit->fPlane==1||hit->fPlane==2))
    h_nhits_3->Fill(hit->fHitNumber,w);
  if (hit->fRegion==3&&hit->fPackage==2&&(hit->fPlane==3||hit->fPlane==4))
    h_nhits_4->Fill(hit->fHitNumber,w);
}

void plot_hitnumber()
{
  TCanvas *nhits_c = new TCanvas("nhits_c"," VDC Hit Multiplicity (hits/wire in an event)",-20,0,450,450);
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
  nhits_c->SaveAs(outputPrefix+"hit_number.png");
  nhits_c->SaveAs(outputPrefix+"hit_number.C");
}

void auto_vdc(Int_t runnum, Bool_t isFirst100K = kFALSE)
{
  //outputPrefix = Form("$QWSCRATCH/tracking/www/run_%d/auto_vdc_%d_",runnum,runnum);

  outputPrefix = Form(
      TString(gSystem->Getenv("WEBDIR")) + "/run_%d/%d_auto_vdc_",
      runnum, runnum);

  // Create and load the chain
  TChain *chain = new TChain("event_tree");
  chain->Add(Form("$QW_ROOTFILES/Qweak_%d.root",runnum));

  // Configure root
  gStyle->SetPalette(1);

//These are the functions which initllize the histograms for the different type of plots one is making
  init_planes();
  init_delaylines();
  init_times();
  init_hitnumber();

  // Event loop
  QwEvent* event = 0;
  chain->SetBranchStatus("events",1);
  chain->SetBranchAddress("events",&event);
  for (int i = 0; i < chain->GetEntries(); i++) {
    if (i%10000 == 0) cout << "Processing event " << i << "..." << endl;
    chain->GetEntry(i);

    double w = event->fCrossSection? event->fCrossSection: 1;

    for (int j = 0; j < event->GetNumberOfHits(); j++) {
      const QwHit* hit = event->GetHit(j);
      //these are the funcitons that fill the histgrams with data, each being passed a hit
      fill_planes(hit,w);
      fill_delaylines(hit,w);
      fill_times(hit,w);
      fill_hitnumber(hit,w);
    }

  }

  //these histograms are the ones the plot the histrogram that are defined and created above
  plot_planes();
  plot_delaylines();
  plot_times();
  plot_hitnumber();
}
