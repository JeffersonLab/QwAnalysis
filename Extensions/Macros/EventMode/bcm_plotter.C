/* A short script to plot BCM information for a given Qweak run.
 *
 * Calling syntax (within ROOT):
 * .L bcm_plotter.C++g
 * bcm_plotter(run_number,first_event,last_event)
 *
 * Written by:
 * Josh Magee
 * magee@jlab.org
 * Sept 24, 2014
 */

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TLeaf.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TPaveStats.h>

using namespace std;

//define useful function protoypes
//these basically separate plotting styles to shorten the code
void redHisto(TH1F*);
void bluePlot(TGraph*,float);
void placeAxis(std::string, std::string, std::string, TCanvas*, TH1F*);
void placeAxis(std::string, std::string, std::string, TCanvas*, TGraph*);

//CodaEventType (1=mps, 2=TS)
void bcm_plotter(int runNumber, int eventLow=0, int eventHigh=4e6) {
  gROOT -> Reset();
  gROOT -> SetStyle("Modern");

  //open rootfile
  char filename[100];
  sprintf(filename,"Qweak_%i.root",runNumber);
  TFile *file = new TFile(filename);
  if ( file->IsOpen() ) {
    std::cout <<"Successfully opened ROOTFILE " <<file->GetName() <<std::endl;
  } else {
    std::cout <<"Error opening ROOTFILE " << file->GetName() <<std::endl;
    return;
  }

  bool debug = false;
  double bcm_readout=0;

  //vectors to store information
  std::vector<double> bcm_vector;
  std::vector<double> event_vector;

  //grab the trees and set all the addresses we want
  TTree   *etree   = (TTree*) file->Get("event_tree");
  etree->ResetBranchAddresses();

  TLeaf *bcm_leaf = etree->GetLeaf("sca_bcm2");

  int n_entries  = (int) etree->GetEntries();  //get the number of entries

  if (debug)  printf("EventNumber \tEventType \tbcm2 \tibcm2 \n");
  eventHigh = (n_entries<eventHigh) ? n_entries : eventHigh;
  for(int j=eventLow; j<n_entries; j++) {
    etree->GetEntry(j);

    if (j<eventLow) {continue;}
    if (j>eventHigh) {break;}
    if (j % 100000==0) {
      printf("Processing event #%i out of %i\n",j,n_entries);
    }

    if (debug && j>150) {continue;}

    //store bcm information into read out variable
    //also use bcm calibration
    //Run numbers >13000 are Run 2, everything else Run 1
    if (runNumber>13000) {
      bcm_readout = ((bcm_leaf->GetValue()*1e-2)-250.33)/41.42; //ELOG A+S 1126
    } else {
      bcm_readout = ((bcm_leaf->GetValue()*1e-2)-250.40)/44.91; //A+S 1127
    }
    if(bcm_readout>0.002) {
      bcm_vector.push_back(bcm_readout);
      event_vector.push_back(j);
    }

  } //end loop over events

  //create histograms
  TH1F *bcm_hist = new TH1F("bcm_hist","title",1200,0,1.2);
  bcm_hist->FillN(bcm_vector.size(),bcm_vector.data(),NULL); //fill with data
  redHisto(bcm_hist);

  TCanvas *canvas1 = new TCanvas("canvas1");
  canvas1->cd();

  bcm_hist->Draw("");
  char dummy_buffer[100];
  sprintf(dummy_buffer,"BCM 2 Current for Run %i Events: [%i,%i]",runNumber,eventLow,eventHigh);
  std::string title_hist = dummy_buffer;
  std::string xaxis_hist = "Current (#muA)";
  std::string yaxis_hist = "Entries";

  placeAxis(title_hist,xaxis_hist,yaxis_hist,canvas1,bcm_hist);

  canvas1->Modified();
  canvas1->Update();

  TGraph *bcm_time = new TGraph(event_vector.size(),event_vector.data(),bcm_vector.data());

  float size=1;

  bluePlot(bcm_time,size);

  TCanvas *canvas2 = new TCanvas("canvas2","title",1200,900);
  canvas2->cd();
  bcm_time->Draw("ap");

  canvas2->Modified();
  canvas2->Update();

  sprintf(dummy_buffer,"BCM 2 Current vs. Time (Events: [%i,%i])",eventLow,eventHigh);
  std::string title_plot = dummy_buffer;
  std::string xaxis_plot = "CodaEventNumber";
  std::string yaxis_plot = "Current (#muA)";

  placeAxis(title_plot,xaxis_plot,yaxis_plot,canvas2,bcm_time);
  canvas2->Modified();
  canvas2->Update();

}


void redHisto(TH1F *hist) {
  hist->SetLineColor(kRed+3);
  hist->SetFillColor(kRed+3);
}


void bluePlot(TGraph *plot, float size ) {
  plot->SetMarkerColor(kBlue+3);
  plot->SetMarkerStyle(20);
  plot->SetLineColor(kBlue+3);
  plot->SetLineWidth(2);
  plot->SetMarkerSize(size);
}



void placeAxis(std::string title, std::string xaxis, std::string yaxis, TCanvas *canvas, TH1F* hist) {
  float size=0.06;
  //float titlesize=0.04;
  float labelsize = 0.035;
  hist->SetTitle(title.data());
  hist->GetXaxis()->SetTitle(xaxis.data());
  hist->GetYaxis()->SetTitle(yaxis.data());

  canvas->Modified();
  canvas->Update();

  hist->GetYaxis()->SetTitleOffset(0.75);
  hist->GetXaxis()->SetTitleOffset(0.75);
  hist->GetYaxis()->SetTitleSize(size);
  hist->GetXaxis()->SetTitleSize(size);
  hist->GetYaxis()->SetLabelSize(labelsize);
  hist->GetXaxis()->SetLabelSize(labelsize);

  gPad->SetGrid();
  canvas->Modified();
  canvas->Update();

  TPaveStats *stats = (TPaveStats*) hist->GetListOfFunctions()->FindObject("stats");
  stats->SetFillColor(0);
  stats->SetX1NDC(0.70);
  stats->SetY1NDC(0.74);
  stats->SetX2NDC(0.90);
  stats->SetY2NDC(0.90);

  stats->Draw("same");

  canvas->Modified();
  canvas->Update();
}

void placeAxis(std::string title, std::string xaxis, std::string yaxis, TCanvas *canvas, TGraph *graph) {
  float size=0.04;
  //float titlesize=0.19;
  float labelsize = 0.03;
  graph->SetTitle(title.data());
  graph->GetXaxis()->SetTitle(xaxis.data());
  graph->GetYaxis()->SetTitle(yaxis.data());

  canvas->Modified();
  canvas->Update();

  graph->GetYaxis()->SetTitleOffset(0.80);
  graph->GetXaxis()->SetTitleOffset(1.00);
  graph->GetYaxis()->SetTitleSize(size);
  graph->GetXaxis()->SetTitleSize(size);
  graph->GetYaxis()->SetLabelSize(labelsize);
  graph->GetXaxis()->SetLabelSize(labelsize);

  gPad->SetGrid();
  canvas->Modified();
  canvas->Update();
}


