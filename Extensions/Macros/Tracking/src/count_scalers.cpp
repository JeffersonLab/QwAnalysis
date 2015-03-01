/*
 * count_scalers.cpp
 * This short program reads a Qweak tracking file and then histograms it.
 * This is achieved by initializing a ScalerCount object, which
 * walks through a provided branch, stores the data in internal
 * vectors, and then calculates the means/uncertainties on the
 * quantities. The ScalerCount class also provides methods for
 * retrieving the vectors, so one can plot them. A sample method
 * for plotting the main detector tubes and allbars is given here.
 *
 * ScalerCount objects also provide a nice output of rates and
 * corrections if one desires. The helpscreen in this program
 * will describe a few other things. For full documentation,
 * one should read the ScalerCount.cpp file, which explains in
 * more depth the calculations.
 *
 * Usage:
 *    ./count_scalers <rootfile> <eventLow> <eventHigh>
 *  where eventLow and eventHigh are the first and last event numbers
 *  to process, respectively.
 *
 *  Written by Josh Magee
 *  magee@jlab.org
 *  May 21, 2014
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
 *  ./count_scalers <rootfile> <eventLow> <eventHigh>
 * If event range is blank, assume processing all events
 *
 * Command line options not fully implemented yet.
 */

int main(int argc, char* argv[]) {
  //define default run range values
  long eventLow = 0;     //first event to process
  long eventHigh= 4000000;   //last event to process

  //debug is to debug overall program
  bool global_debug=false;
  if (global_debug) {  std::cout <<"Global debugger on...\n"; }

  //if no command line arguments given, display helpscreen
  if (argc<=1) {
    helpscreen();
    exit(0);
    return 0;
  }

  //handle command line arguments
  TString myFile = argv[1];             //get file name
  eventLow  = argv[2] ? atol(argv[2]) : eventLow;   //2nd option defaults to event low
  eventHigh = argv[3] ? atol(argv[3]) : eventHigh;  //3rd option defaults to event high

  //double check parsing
  std::cout <<"Event low is: " <<eventLow <<std::endl;
  std::cout <<"Event high is: " <<eventHigh <<std::endl;

  //Open rootfile
  TFile *rootfile = TFile::Open(myFile);

  if ( rootfile->IsOpen() ) {
    printf("Rootfile %s is open. Continuing...!\n", myFile.Data());
  } else {
    printf("Rootfile %s isn't open! Exiting!\n", myFile.Data());
    exit(1);
  }

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
   * mdCount stands for "main detector count"
   */
  if (global_debug) { std::cout <<"Creating ScalerCount object...\n"; }
  ScalerCount mdCount(maindet,eventLow,eventHigh);

  //lets process events in the branch...
  if (global_debug) { std::cout <<"Processing events...\n"; }
  mdCount.processEvents();

  //lets print what we've got...
  if (global_debug) { std::cout <<"Printing rates and calculations...\n"; }
  mdCount.printRawRates();
  mdCount.printCorrections();
  mdCount.printCorrectedRates();


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




