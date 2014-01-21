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
 * January 16, 2014
*/

#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <TMath.h>

#include "MarryVecs.h"

using namespace std;

void helpscreen(void);

int main(int argc, char* argv[]) {

  const int numEntries = 1; //number of entries to process
  const int maxHits    = 4; //maximum number of possible hits per entry

  int tsHits=0;
  int mdHits=0;
  double myHit=0;

  bool debug=false;

  MarryVecs marryVector;

  srand(time(NULL));
  for(int j=0; j<numEntries; j++) {
    //produce random number of hits for TS
    //then generate a random hit time for each hit between -10 and 15 ns
    //max number of hits is maxHits
    tsHits = (rand() % maxHits) + 1;
    for(int j=0; j<tsHits; j++) {
      myHit = (rand() % 25) - 10;
      marryVector.add_entry_groom(myHit);
      if (debug) {
        if (j==0) std::cout <<"TS/groom entry:\t";
        std::cout <<"\t " <<myHit <<std::endl;
      }
    }

    //same for main detector
    mdHits = (rand() % maxHits) + 1;
    for(int j=0; j<mdHits; j++) {
      myHit = (rand() % 25) - 10;
      marryVector.add_entry_bride(myHit);
      if (debug) {
        if (j==0) std::cout <<"MD/bride entry:\t";
        std::cout <<"\t " <<myHit <<std::endl;
      }
    }
  }
/*
    marryVector.add_entry_groom(11);
    marryVector.add_entry_groom(9);
    marryVector.add_entry_bride(-7);
    marryVector.add_entry_bride(4);
    marryVector.add_entry_bride(9);
*/
    marryVector.print_vectors();

    //marry arrays and see what we get
    marryVector.marry_arrays();
    marryVector.print_marriage_vectors();
//  }

  return 0; //terminate successfully
}

/*void helpscreen (void) {
  std::cout <<"Welcome to the marriage-check program. none of these help"
    << "\nptions currently work. Have a nice day!\n";
    string helpstring = "help for f1marriage.cpp program.\n";
    helpstring += "program to calculate a proper marriage of meantimes from the\n";
    helpstring += "qweak main detector and trigger scintillator f1 multi-hit tdc's.\n";
    helpstring += "usage: \n";
    helpstring += "\t./f1tdc_marriage [options]\n";
    helpstring += "options: \n";
    helpstring += "\t-f  \t\tfilename\n";
    helpstring += "\t-e  \t\tevents to look at (delimeter is ':')\n";
    helpstring += "\t-ts \t\tdefines trigger scintillator plane (plane 1=octant 5\n";
    helpstring += "\t    \t\tplane 2 = octant 1) default is plane 2.\n";
    helpstring += "\t-md \t\tdefines main detector plane (plane 1 = octant 1\n";
    helpstring += "\t    \t\tplane 5 = octant 5) default is plane 1.\n";
    helpstring += "\t--debug \tfor debugging code (not fully implemented)\n";
    helpstring += "\t--help  \tdisplays this helpful message.\n";
    helpstring += "example: \n";
    helpstring += "\t./f1marriage -f qweak_10717.root -e 15e3:30e3\n";

  std::cout <<helpstring <<endl;

  exit(0);
}
*/
/*  if (argc<=1) {
    helpscreen();
    exit(0);
    return 0;
  }

  else { //we have to process options
    for (int j=1; j<argc; j++) {
      if (j+1 != argc) {//check that we haven't finished parsing already
        if (argv[j] == "-f") {
          myfile = argv[j+1];
          printf("argument %i is %s\n",j+1,argv[j+1]);
        } else if (argv[j] == "-e") {
          mylow  = long(argv[j+1]);
          myhigh = long(argv[j+2]);
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






