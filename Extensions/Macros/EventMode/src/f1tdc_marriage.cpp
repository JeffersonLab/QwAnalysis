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
#include <TApplication.h>
#include <TH1F.h>
#include <THStack.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TLegend.h>
#include <TPaveStats.h>

#include "MarryVecs.h"
#include "DetectorMarriage.h"

using namespace std;

double findDoublePulseResolution(TH1F*, double);
void helpscreen(void);
void generate_marriages( MarryVecs *);

/* default run condition:
 *  ./f1marriage <rootfile> <eventLow> <eventHigh>
 * If event range left out, assume process all events.
 *
 * Command line options to be implemented at a later date.
 */
int main(int argc, char* argv[]) {

  //will allow planes to be selected in command line arguments later
  /* Define default planes.
   * TS planes are packages (1 and 2)
   * MD planes are octants (1-8)
   * Default values are octant 1, package 2
   */
  int mdplane = 1;
  int tsplane = 2;

  //define default event range values
  long eventLow = 0;        //default first event to process
  long eventHigh= 4000000;  //default last event to process

  //global_debug is to debug overall program
  bool global_debug=false;

  int runNum = 0;
  std::string filename;

  if (argc<=1) {
    helpscreen();
    return 0;
  }
  //process comand line arguments
  for (int j=1; j<argc; j++) {
    if (j < argc) {//check that we haven't finished parsing already
      if (0 == strcmp("-f",argv[j]))     filename = argv[j+1];         //ROOT file to process
      if (0 == strcmp("--file",argv[j])) filename = argv[j+1];         //ROOT file to process
      if (0 == strcmp("-r",argv[j]))     runNum = atol(argv[j+1]);         //run number to process
      if (0 == strcmp("--run",argv[j]))  runNum = atol(argv[j+1]);         //run number to process
      if (0 == strcmp("--low",argv[j]))  eventLow = atol(argv[j+1]);   //first event to process
      if (0 == strcmp("--high",argv[j])) eventHigh = atol(argv[j+1]);  //last event to process
      if (0 == strcmp("--lowevent",argv[j]))  eventLow = atol(argv[j+1]);   //first event to process
      if (0 == strcmp("--highevent",argv[j])) eventHigh = atol(argv[j+1]);  //last event to process
      if (0 == strcmp("--ts", argv[j]))  tsplane = atoi(argv[j+1]);    //get TS plane/package number
      if (0 == strcmp("--md", argv[j]))  mdplane = atoi(argv[j+1]);    //get MD octant
      if (0 == strcmp("--help", argv[j]))  helpscreen();               //show helpscreen
      if (0 == strcmp("--debug", argv[j]))  global_debug = true;       //debug ON
      }
  } //finish processing options

  if(runNum>0) {
    char buffer[20];
    sprintf(buffer,"Qweak_%i.root",runNum);
    filename = buffer;
  }
  //Open rootfile
  TFile *rootfile = TFile::Open(filename.data());

  if ( rootfile->IsOpen() ) {
    printf("Rootfile %s is open...\n", filename.data());
  } else {
    printf("Rootfile %s isn't open! Exiting!\n", filename.data());
    exit(EXIT_FAILURE);
  }

  if (global_debug) std::cout <<"Global debugger on." <<std::endl;
  //double check parsing
  std::cout <<"You have selected to look at Octant " <<mdplane
    <<" with Package " <<tsplane <<std::endl;
  std::cout <<"Events to process: " <<eventLow <<" to " <<eventHigh <<std::endl;

  //grab the tree from the rootfile
  //TTree *tree = (TTree*) rootfile1->Get("tree");
  TTree *tree = (TTree*) rootfile->Get("event_tree");

  //create DetectorMarriage object
  //initialize with TTree tree
  if (global_debug) printf("Creating DetectorMarriage object...\n");
  DetectorMarriage detMarriage(tree);
  if (global_debug) printf("DetectorMarriage object created...\n");

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
  //vector<double> *groomVec = detMarriage.get_groom_hit_time_diff();
  vector<double> *brideVec = detMarriage.get_bride_hit_time_diff();
//  vector<double> *groomVec = detMarriage.get_raw_grooms();

  //binning information
  const int Nbins = 2000;
  const int xmin  = -600;
  const int xmax  = 1400;

  //canvas1 will plot the coincidence spectra
  TCanvas *canvas1 = new TCanvas("canvas","title");
  canvas1->cd();

  TH1F* hCoincidence = new TH1F("hCoincidence","TS*MD Coincidence Spectra",Nbins,xmin,xmax);
  hCoincidence->FillN(marrVec->size(),marrVec->data(),NULL);

  hCoincidence->Draw();
  canvas1->SetLogy();

  //canvas2 will plot just the peak window
  TCanvas *canvas2 = new TCanvas("canvas2","title");
  canvas2->cd();
  TH1F *hPeak = new TH1F("hPeak","TS*MD Peak", Nbins,xmin,xmax);
  hPeak->FillN(peakVec->size(),peakVec->data(),NULL);
  hPeak->Draw();
  canvas2->SetLogy();

  //canvas3 will plot the accidentals window
  TCanvas *canvas3 = new TCanvas("canvas3","title");
  canvas3->cd();

  TH1F* hAccident = new TH1F("hAccident","TS*MD Accidentals",Nbins,xmin,xmax);
  hAccident->FillN(accVec->size(),accVec->data(),NULL);
  hAccident->Draw();
  canvas3->SetLogy();

  TCanvas *canvas4 = new TCanvas("canvas4","title");
  canvas4->cd();

//  TH1F* hTrigScint = new TH1F("hTrigScint","Trigger Scintillator Spectra",Nbins,xmin,xmax);
//  hTrigScint->FillN(groomVec->size(),groomVec->data(),NULL);
//  hTrigScint->Draw();
//  canvas4->SetLogy();

  TH1F* hDoublePulse= new TH1F("hDoublePulse","Double Pulse Resolution (Run 10709)",2000,0,2000);
  hDoublePulse->FillN(brideVec->size(),brideVec->data(),NULL);
  hDoublePulse->Draw();

//  std::cout <<"The double pulse resolution for MD " <<mdplane <<" is: " <<hitdiff
//    <<" ns." <<std::endl;
  std::cout <<hPeak->GetEntries() <<std::endl;
  std::cout <<hAccident->GetEntries() <<std::endl;

  std::cout <<"Application concluded.";
  app->Run(); //run the TApplication
  return 0;   //exit gracefully
}

void helpscreen (void) {
  string helpstring = "Help for f1marriage.cpp program.\n";
    helpstring += "Program to calculate a proper marriage of meantimes from the\n";
    helpstring += "Qweak main detector and trigger scintillator f1 multi-hit tdc's.\n";
    helpstring += "Usage: \n";
    helpstring += "\t./f1tdc_marriage [options]\n";
    helpstring += "Options: \n";
    helpstring += "\t-f  \t\tfilename\n";
    helpstring += "\t--file  \t\tfilename\n";
    helpstring += "\t-r  \t\tProvide run number instead of filename\n";
    helpstring += "\t--run \t\tProvide run number instead of filename\n";
    helpstring += "\t--lowevent  \tfirst event to process\n";
    helpstring += "\t--highevent \tfirst event to process\n";
    helpstring += "\t--ts\t\tdefines trigger scintillator plane (plane 1=octant 5\n";
    helpstring += "\t    \t\tplane 2 = octant 1) Default is plane 2.\n";
    helpstring += "\t--md\t\tdefines main detector plane (plane 1 = octant 1\n";
    helpstring += "\t    \t\tplane 5 = octant 5) Default is plane 1.\n";
    helpstring += "\t--debug \tfor debugging code (not fully implemented)\n";
    helpstring += "\t--help  \tdisplays this helpful message.\n";
    helpstring += "Example: \n";
    helpstring += "\t./f1marriage -f Qweak_10709.root --low 30000 --high 370000\n";

  std::cout <<helpstring <<endl;
  exit(EXIT_SUCCESS);
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


//Optional routines to plot time difference of successive meantimes.
/*  TCanvas *canvas4 = new TCanvas("canvas4","title");
  canvas4->cd();

  TH1F* hGroomDiff = new TH1F("hGroomDiff","TS Meantime",Nbins,xmin+200,xmax+200);
  hGroomDiff->FillN(groomVec->size(),groomVec->data(),NULL);

  TH1F* hBrideDiff = new TH1F("hBrideDiff","MD Meantime",Nbins,xmin+200,xmax+200);
  hBrideDiff->FillN(brideVec->size(),brideVec->data(),NULL);

  blueHisto(hGroomDiff);
  redHisto(hBrideDiff);
  hGroomDiff->SetStats(true);
  hBrideDiff->SetStats(true);
*/

  /* The following section deals with the plot configuration of the
   * Time difference plots. No new physics here!
   */
/*
  //THStack plots multiple histograms in the same plot
  THStack *stack = new THStack("stack","title");
  stack->Add(hGroomDiff,"sames");
  stack->Add(hBrideDiff,"sames");

  stack->Draw("nostack");
  gPad->Modified();
  gPad->Update();

  placeAxis("Time Difference Between Married MD MT Hit Times",
      "Time difference (ns)","",canvas4,stack);

  TPaveStats *statG = (TPaveStats*) hGroomDiff->GetListOfFunctions()->FindObject("stats");
  if (statG) {
    statG->SetTextColor(kBlue+3);
    setup_stats(statG,0.82,0.66,0.99,0.81);
    statG->Draw("sames");
  }

  TPaveStats *statB = (TPaveStats*) hBrideDiff->GetListOfFunctions()->FindObject("stats");
  if (statB) {
    statB->SetTextColor(kRed+3);
    setup_stats(statB,0.82,0.50,0.99,0.66);
    statB->Draw("sames");
  }

  TLegend *leg = new TLegend(.82,.82,.99,.96);
  leg->SetFillColor(0);
  leg->AddEntry(hGroomDiff,"Trig Scint MT","f");
  leg->AddEntry(hBrideDiff,"Main Det MT","f");
  leg->Draw("sames");

  gPad->Modified();
  gPad->Update();
*/




