/*
 * f1tdc_marriage.cpp 
 * Patches together f1adc data by time, "marrying" them.
 *
 * Written by Josh Magee
 * magee@jlab.org
 * November 19, 2013
*/

#include <TROOT.h>
#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <math.h>
#include <stdlib.h> 

#include "MarryVecs.h"
#include "DetectorMarriage.h"

using namespace std;

int main(int argc, char* argv[]) {
  bool debug=0;
  if (argc==1) {
    printf("You MUST specify a rootfile name!\n");
    exit(1);
    return 1;
  }

  //get rootfile name
  TString var_1 = argv[1];

  //Open rootfile
  TFile *rootfile1 = TFile::Open(var_1);

  if ( !rootfile1->IsOpen() ) {
    printf("Rootfile %s isn't open! Exiting!\n", var_1.Data());
    exit(1);
  }


  //grab the tree from the rootfile
  //TTree *tree = (TTree*) rootfile1->Get("tree");
  TTree *tree = (TTree*) rootfile1->Get("event_tree");

  //create MarryMe object
  if (debug) printf("Creating DetectorMarriage object...\n");
  DetectorMarriage detMarriage(tree);
  if (debug) printf("DetectorMarriage object created...\n");

//  DetectorMarriage detMarriage();
//  detMarriage.setTree(tree);
  detMarriage.setEventRange(14090,15010);
  detMarriage.setTsPlane(2);
  detMarriage.setMdPlane(1);
  detMarriage.processEvents();
  //Create a TApplication. This is required for anything using the ROOT GUI.
  TApplication *app = new TApplication("myApp",&argc,argv);
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TCanvas *canvas = new TCanvas("canvas","title");
  canvas->cd();


  app->Run();
  return 0;
}



