//
// Author : Jeong Han Lee / John leacock
// Date   : Sunday, December  5 21:31:38 EST 2010
// 
// 
//          0.0.1 : Sunday, December  5 21:31:43 EST 2010, jhlee
//                 - copied John script to modify them
//                   to fit plot_HDC
//
//          0.0.2 " Tuesday, January  4 16:08:42 EST 2011, jhlee
//                 - extended to check the different planes
//        
#include "TSystem.h"
#include "TString.h"
#include "TTree.h"
#include "TCut.h"

TTree *event_tree;

void 
plot_histo(TString name)
{
  tracking_histo->cd();

  TCanvas *c1 = new TCanvas(Form("histograms_%s", name.Data()), Form("R2 HitOnEachWire in %s", name.Data()), 1100, 700);
  c1->Divide(4,3);
  c1->cd(1);
  R2HitsOnEachWirePlane1->Draw();
  c1->cd(2);
  R2HitsOnEachWirePlane2->Draw();
  c1->cd(3);
  R2HitsOnEachWirePlane3->Draw();
  c1->cd(4);
  R2HitsOnEachWirePlane4->Draw();
  c1->cd(5);
  R2HitsOnEachWirePlane5->Draw();
  c1->cd(6);
  R2HitsOnEachWirePlane6->Draw();
  c1->cd(7);
  R2HitsOnEachWirePlane7->Draw();
  c1->cd(8);
  R2HitsOnEachWirePlane8->Draw();
  c1->cd(9);
  R2HitsOnEachWirePlane9->Draw();
  c1->cd(10);
  R2HitsOnEachWirePlane10->Draw();
  c1->cd(11);
  R2HitsOnEachWirePlane11->Draw();
  c1->cd(12);
  R2HitsOnEachWirePlane12->Draw();

  return;
};
  

void 
plot_hits(TString name)
{
  
  TCut region2 = "fRegion==2";

  TCanvas *c2 = new TCanvas(Form("hits_%s", name.Data()), Form("R2 Hits in %s", name.Data()), 1100, 700);

  c2->Divide(6,4);

  Int_t idx = 0;
  Int_t package = 0;
  for (Int_t i=0;i<24;i++)
    {
      idx = i+1;
      c2->cd(idx);
      if(i < 12) { package = 1; }
      else       { package = 2; idx -= 12;}

      TCut cut = Form("fPackage==%d&&fPlane==%d", package, idx);
      cut+=region2;
      event_tree->Draw("fElement",  cut);
    }

  return;
};
  


void 
all_plots_hdc(TString name="")
{
  TString file_dir;
  file_dir = gSystem->Getenv("QW_ROOTFILES");
 
  if(name.IsNull()) {
    printf("There is no root file\n");
    exit(-1);
  }
  else {
    TFile *file =  new TFile(Form("%s%s", file_dir.Data(), name.Data()));
  }

  gStyle->SetPalette(1); 

  event_tree = (TTree*) file->Get("event_tree");

  plot_histo(name);
  plot_hits(name);

  return;
};

