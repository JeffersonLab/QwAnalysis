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
#include "PlotLib.h"

using namespace std;

void helpscreen(void);

int main(int argc, char* argv[]) {

  //define default values
  //TString myFile;
  long eventLow = 0;     //first event to process
  long eventHigh= 4e6;   //last event to process
  int tsplane = 2;    //tsplane 2 = octant 1; plane 1 = octant 5
  int mdplane = 1;    //mdplane 1 = octant 1; plane 5 = octant 5
  bool debug=0;

  if (argc<=1) {
    helpscreen();
    exit(0);
    return 0;
  }
  /* else { //we have to process options
    for (int j=1; j<argc; j++) {
      if (j+1 != argc) {//check that we haven't finished parsing already
        if (argv[j] == "-f") {
          myFile = argv[j+1];
          printf("Argument %i is %s\n",j+1,argv[j+1]);
        } else if (argv[j] == "-e") {
          myLow  = long(argv[j+1]);
          myHigh = long(argv[j+2]);
        } else if (argv[j] == "-ts") {
          tsplane = int(argv[j+1]);
        } else if (argv[j] == "-md") {
          mdplane = int(argv[j+1]);
        } else if (argv[j] == "--help") {
          helpscreen();
        } else if (argv[j] == "--debug") {
          debug = 1;
        }
      }
    } //loop over options
  } //finish processing options
*/
  //get rootfile name
  TString myFile = argv[1];

  //Open rootfile
  TFile *rootfile1 = TFile::Open(myFile);

  if ( !rootfile1->IsOpen() ) {
    printf("Rootfile %s isn't open! Exiting!\n", myFile.Data());
    exit(1);
  }


  //grab the tree from the rootfile
  //TTree *tree = (TTree*) rootfile1->Get("tree");
  TTree *tree = (TTree*) rootfile1->Get("event_tree");

  //create MarryMe object
  if (debug) printf("Creating DetectorMarriage object...\n");
  DetectorMarriage detMarriage(tree);
  if (debug) printf("DetectorMarriage object created...\n");

  detMarriage.setEventRange(eventLow,eventHigh);
  detMarriage.setTsPlane(tsplane);
  detMarriage.setMdPlane(mdplane);
  detMarriage.processEvents();
  //Create a TApplication. This is required for anything using the ROOT GUI.
  TApplication *app = new TApplication("myApp",&argc,argv);
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  TCanvas *canvas = new TCanvas("canvas","title");
  canvas->cd();

  vector<double> *first_marriage = detMarriage.getFirstMarriage();
  vector<double> *marrVec = detMarriage.getDetectorMarriage();
  vector<double> *diffVec = detMarriage.getDetectorMarriageDifferences();

  int Nbins = 1800;
  int xmin  = -400;
  int xmax  = 1400;
  TH1F* mHist = new TH1F("mHist","Original TS*MD Spectra",Nbins,xmin,xmax);

  //mHist->FillN(marrVec->size(),marrVec->data(),NULL);
  mHist->FillN(first_marriage->size(),first_marriage->data(),NULL);

  mHist->Draw();
  canvas->SetLogy();

  TCanvas *canvas2 = new TCanvas("canvas2","title");
  canvas2->cd();

  TH1F* mHist2 = new TH1F("mHist2","New Marriage Spectra",Nbins,xmin,xmax);
  mHist2->FillN(marrVec->size(),marrVec->data(),NULL);
  mHist2->Draw();
  canvas2->SetLogy();


  app->Run();
  return 0;
}


void helpscreen (void) {
  string helpstring = "Help for f1marriage.cpp program.\n";
    helpstring += "Program to calculate a proper marriage of meantimes from the\n";
    helpstring += "Qweak main detector and trigger scintillator f1 multi-hit tdc's.\n";
    helpstring += "Usage: \n";
    helpstring += "\t./f1tdc_marriage [options]\n";
    helpstring += "Options: \n";
    helpstring += "\t-f  \t\tfilename\n";
    helpstring += "\t-e  \t\tevents to look at (delimeter is ':')\n";
    helpstring += "\t-ts \t\tdefines trigger scintillator plane (plane 1=octant 5\n";
    helpstring += "\t    \t\tplane 2 = octant 1) Default is plane 2.\n";
    helpstring += "\t-md \t\tdefines main detector plane (plane 1 = octant 1\n";
    helpstring += "\t    \t\tplane 5 = octant 5) Default is plane 1.\n";
    helpstring += "\t--debug \tfor debugging code (not fully implemented)\n";
    helpstring += "\t--help  \tdisplays this helpful message.\n";
    helpstring += "Example: \n";
    helpstring += "\t./f1marriage -f Qweak_10717.root -e 15e3:30e3\n";

  std::cout <<helpstring <<endl;
  exit(0);
}







