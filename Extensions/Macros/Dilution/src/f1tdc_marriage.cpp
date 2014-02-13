/*
 * f1tdc_marriage.cpp
 * This short program should create a series of data to
 * test the marriage vector class. Instead of writing a
 * rootfile with random data, it will create a series of
 * "events" each with a random number of hits in two
 * detectors ("trigger scintillator" and "main detector").
 *
 * Processing of each event entails filling each detector's
 * hit vectors and then marrying them to the other detector.
 *
 * useage:
 *    To be determined
 *  ./f1marriage <rootfile> <eventLow> <eventHigh>
 *
 * Written by Josh Magee
 * magee@jlab.org
 * November 19, 2013
*/

#include <math.h>
#include <stdlib.h> 

#include <TROOT.h>
#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TApplication.h>
#include <TGraphErrors.h>
#include <TGraph.h>
#include <TH2F.h>
#include <TMultiGraph.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TPaveStats.h>
#include <TArrow.h>

#include "MarryVecs.h"
#include "DetectorMarriage.h"
#include "PlotLib.h"

using namespace std;

void helpscreen(void);
void generate_marriages( MarryVecs *);

/* default run condition:
 *  ./f1marriage <rootfile> <eventLow> <eventHigh>
 * If event range left out, assume process all events.
 *
 * Command line options to be implemented at a later date.
 */
int main(int argc, char* argv[]) {
  //define default values
  long eventLow = 0;     //first event to process
  long eventHigh= 4000000;   //last event to process

  //will allow planes to be selected in command line arguments later
  int tsplane = 2;    //tsplane 2 = octant 1; plane 1 = octant 5
  int mdplane = 1;    //mdplane 1 = octant 1; plane 5 = octant 5

  //debug is to debug overall program
  bool debug=false;

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
  //handle command line arguments
  TString myFile = argv[1];             //get file
  eventLow  = argv[2] ? atol(argv[2]) : eventLow;   //2nd option defaults to event low
  eventHigh = argv[3] ? atol(argv[3]) : eventHigh;  //3rd option defaults to event high

  //double check parsing
  std::cout <<"Event low is: " <<eventLow <<std::endl;
  std::cout <<"Event high is: " <<eventHigh <<std::endl;

  //Open rootfile
  TFile *rootfile1 = TFile::Open(myFile);

  if ( !rootfile1->IsOpen() ) {
    printf("Rootfile %s isn't open! Exiting!\n", myFile.Data());
    exit(1);
  }

  //grab the tree from the rootfile
  //TTree *tree = (TTree*) rootfile1->Get("tree");
  TTree *tree = (TTree*) rootfile1->Get("event_tree");

  //create DetectorMarriage object
  //initialize with TTree tree
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

  /* create vector pointers to hold data returned from DetectorMarriage object
   * marrVec    = vector of ALL married hits
   * peakVec    = vector of only events in prompt peak
   * accVec     = vector of only accidentals
   */
  vector<double> *marrVec = detMarriage.getDetectorMarriage();
  vector<double> *peakVec = detMarriage.getDetectorMarriagePeak();
  vector<double> *accVec  = detMarriage.getDetectorMarriageAccidentals();

  //binning information
  int Nbins = 1800;
  int xmin  = -400;
  int xmax  = 1400;

  TCanvas *canvas = new TCanvas("canvas","title");
  canvas->cd();

  TH1F* hCoincidence = new TH1F("hCoincidence","TS*MD Coincidence Spectra",Nbins,xmin,xmax);

  hCoincidence->FillN(marrVec->size(),marrVec->data(),NULL);

  hCoincidence->Draw();
  canvas->SetLogy();

  TCanvas *canvas2 = new TCanvas("canvas2","title");
  TH1F *hPeak = new TH1F("hPeak","TS*MD Peak", Nbins,xmin,xmax);
  hPeak->FillN(peakVec->size(),peakVec->data(),NULL);
  hPeak->Draw();
  canvas2->SetLogy();

  TCanvas *canvas3 = new TCanvas("canvas3","title");
  canvas3->cd();

  TH1F* hAccident = new TH1F("hAccident","TS*MD Accidentals",Nbins,xmin,xmax);
  hAccident->FillN(accVec->size(),accVec->data(),NULL);
  hAccident->Draw();
  canvas3->SetLogy();


  app->Run();
  return 0;
}


void helpscreen (void) {
  std::cout <<"Welcome to the f1marriage program. Past Josh says \"you knew you should've filled this out!\"."
    <<"\nUsage ./f1marriage Qweak_10709.root 30000 370000"
    <<"\nHave a nice day!\n";
/*  string helpstring = "Help for f1marriage.cpp program.\n";
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
*/
  exit(0);
}


void generate_marriage(MarryVecs *myMarriageVec) {
  /* This function randomly generates detector hits and marries them
   * appropriately. It's useful as a debugging aid.
   * First, one should define the number of entries to generate;
   * this code has not be developed to do multi-event processing
   * at the moment. Then define the number of hits per event.
   * Note that MarryVec objects only accept 7 hits/event max.
   *
   * Then, a random number of hits are generated for two detectors
   * and the results are married.
   */
  const int numEntries = 1; //number of entries to process
  const int maxHits    = 4; //maximum number of possible hits per entry

  int tsHits=0;
  int mdHits=0;
  double myHit=0;

  srand(time(NULL));
  for(int j=0; j<numEntries; j++) {
    //produce random number of hits for TS
    //then generate a random hit time for each hit between -10 and 15 ns
    //max number of hits is maxHits
    tsHits = (rand() % maxHits) + 1;
    for(int j=0; j<tsHits; j++) {
      myHit = (rand() % 25) - 10;
      myMarriageVec->add_entry_groom(myHit);
//     if (debug) {
//        if (j==0) std::cout <<"TS/groom entry:\t";
//        std::cout <<"\t " <<myHit <<std::endl;
//      }
    }

    //same for main detector
    mdHits = (rand() % maxHits) + 1;
    for(int j=0; j<mdHits; j++) {
      myHit = (rand() % 25) - 10;
      myMarriageVec->add_entry_bride(myHit);
//      if (debug) {
//        if (j==0) std::cout <<"MD/bride entry:\t";
//        std::cout <<"\t " <<myHit <<std::endl;
//      }
    }
  }
  return;
}





