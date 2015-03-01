/*
 * histogram_adcs.cpp
 * This program will construct histograms of main detector ADC spectra.
 * The goal is to take multiple input files and then overlay the
 * spectra from each file to show any gain shifts or changes.
 *
 * The first version of this program will only produce output for one
 * TFile at a time, but constructed in a way that should allow straight-
 * forward development of using multi-TFile options.

 * Usage:
 *    ./histogram_adc <rootfile> <eventLow> <eventHigh>
 *  where eventLow and eventHigh are the first and last event numbers
 *  to process, respectively.
 *
 *  Written by Josh Magee
 *  magee@jlab.org
 *  October 2, 2014
 */

#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <TROOT.h>
#include <Riostream.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TApplication.h>
#include <TH1F.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TPaveStats.h>

#include "ScalerCount.h"

using std::cout;
using std::vector;

void helpscreen(void);

/* default run conditions:
 *  ./histogram_adc <rootfile> <eventLow> <eventHigh>
 * If event range is blank, assume processing all events
 *
 * Command line options not fully implemented yet.
 */

int main(int argc, char* argv[]) {
  //Create a TApplication. This is required for anything using the ROOT GUI.
  TApplication *app = new TApplication("myApp",&argc,argv);
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);

  //TApplication "eats" our command line arguments. Let's get them back.
  int argc_orig    = app->Argc();
  char** argv_orig = app->Argv();

  //debug is to debug overall program
  bool global_debug=false;
  if (global_debug) {  std::cout <<"Global debugger on...\n"; }

  //if no command line arguments given, display helpscreen
  if (argc_orig<=1) {
    helpscreen();
    exit(0);
    return 0;
  }

  //define default run range values
  long eventLow = 0;     //first event to process
  long eventHigh= 4000000;   //last event to process

  //construct necessary data holding elements
  std::vector<std::string> file_names; //list of file names
  std::vector<TFile*> file_vector; //construct vector object to hold TFile pointers

  //handle command line arguments
  TString myFile = argv_orig[1];             //get file name
  eventLow  = argv_orig[2] ? atol(argv_orig[2]) : eventLow;   //2nd option defaults to event low
  eventHigh = argv_orig[3] ? atol(argv_orig[3]) : eventHigh;  //3rd option defaults to event high

  //Open rootfile
  TFile *rootfile = TFile::Open(myFile);

  if ( rootfile->IsOpen() ) {
    printf("Rootfile %s is open. Continuing...!\n", myFile.Data());
  } else {
    printf("Rootfile %s isn't open! Exiting!\n", myFile.Data());
    exit(1);
  }

  //double check parsing
  std::cout <<"Event low is: " <<eventLow <<std::endl;
  std::cout <<"Event high is: " <<eventHigh <<std::endl;

  //grab the tree from the rootfile
  if (global_debug) { std::cout <<"Grabbing tree...\n"; }
  TTree *tree = (TTree*) rootfile->Get("event_tree");

  //for some reason, we have to turn OFF all the branches and only
  //turn on the branches we care about. In this example, we care about
  //the main detectors, so we will turn ON the maindet branch.
  if (global_debug) { std::cout <<"Disabling all branches...\n"; }
  tree->SetBranchStatus("*",0);
  if (global_debug) { std::cout <<"Enabling maindet branch...\n"; }
  tree->SetBranchStatus("maindet",1);

  //now lets assign a pointer to the maindet branch
  TBranch *maindet = (TBranch*) tree->GetBranch("maindet");

  /* Time to create our ScalerCount object.
   * mdCount stands for "main detector counter"
   */
  if (global_debug) { std::cout <<"Creating ScalerCount object...\n"; }
  char buffer[50];
  sprintf(buffer,"Run %i MT LH2",666);
  ScalerCount mdCount(maindet,eventLow,eventHigh,buffer);

  //lets process events in the branch...
  if (global_debug) { std::cout <<"Processing events...\n"; }
  mdCount.processEvents();

  //lets print what we've got...
  if (global_debug) { std::cout <<"Printing rates and calculations...\n"; }
  mdCount.printRawRates();
//  mdCount.printCorrections();
//  mdCount.printCorrectedRates();

  mdCount.createHistograms();
  mdCount.fillHistograms();
  mdCount.createCanvas();
  mdCount.plotHistograms();


  //run TAppication process
  app->Run();
  //end of program, exit gracefully
  return 0;
} //end of main program

void helpscreen(void) {
  std::cout <<"Welcome to the scaler count example program!\n"
    <<"At the moment there are no real command line options, but a few\n"
    <<"things you should keep in mind include the main detector\n"
    <<"discriminator width is 40 ns, with a 10\% uncertainty on that.\n"
    <<"The coincidence condition is 79 ns, but I'm assuming negligible\n"
    <<"uncertainty on that in the accidentals corrections.\n"

    <<"Discussion of the actual corrections in done in the ScalerCount.cpp\n"
    <<"file, in the appropriate methods.\n\n"

    <<"Usage: \n"
    <<"\t ./count_scalers <rootfile> <eventLow> <eventHigh>\n"
    <<"\t ./count_scalers Qweak_10709.root 30000 370000\n\n"
    <<"where eventLow and eventHigh are the first and last events to be\n"
    <<"processed, respectively.\n"
    <<std::endl;
  exit(0);
} //end of helpscreen function




