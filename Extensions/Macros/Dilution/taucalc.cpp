/*
 * This program was designed to compute the average
 * "effective" deadtime from Q-weak current scans.
 * It uses TauCalc objects to actually compute the deadtimes.
 *
 * useage:
 *      compile taucalc, including the TauCalc libraries
 *      ./taucalc [options]
 * Options include:
 *      providing a file name
 *
 * Written by Josh Magee
 * magee@jlab.org
 * October 13, 2014
*/

#include <iostream>
#include <fstream>

#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>

#include "TauCalc.h"

using namespace std;

void helpscreen(void);

/* default run condition:
 * To Be Determined
*/
int main(int argc, char* argv[]) {
  //Create a TApplication. This is required for anything using the ROOT GUI.
  TApplication *app = new TApplication("myApp",&argc,argv);
  gROOT->Reset();
  gROOT->SetStyle("Modern");
  gStyle->SetOptStat(1);
  gStyle->SetOptFit(1111);

  //debug is to debug overall program
  bool debug=false;

  if (debug)
   std::cout <<"Debug on..." <<std::endl;

  if (argc>=2) {
    helpscreen();
    exit(0);
    return 0;
  }


  /* Each TauCalc object calculates the effective deadtime
   * for a given main detector (MD). So, I create an array
   * of TauCalcs, and then a separate array of doubles to
   * store each MD's individual deadtime.
   */

  const int num_md = 8;
  double tau_list[num_md];
  TauCalc tau_calcs[num_md];

  //initialize tau array to 0
  //and set each TauCalc objects name
  char buffer[10];
  std::string object_name;
  for(int j=0; j<num_md; j++) {
    tau_list[j] = 0;
    sprintf(buffer,"MD %i",j+1); //set internal names/graph titles
    object_name = buffer;
    tau_calcs[j].setName(object_name);
    tau_calcs[j].createCanvas();
    tau_calcs[j].setPlotColor(j+1);
  }

  /* Now our TauCalc object array is initialized and ready to roll.
   * Now we must process a filename, if given. If no filename is given,
   * one needs to insert the data by hand here, or expand the code.
   *
   * Sample code to insert by hand:
   * tau.push_data(0.05994,77.99,0.23);
   * tau.push_data(0.09118,118.56,0.25);
   * tau.push_data(1.1660,1213.55,2.26);
   */

  //list of things to read in from file
  int n, line=1;
  double i, rate, err;

  //process file name information
  string filename = "dt.txt";
  const char* filename_str  = filename.c_str();
  ifstream file;
  file.open(filename_str,ios::in);

  //check if file is open. If so, continue, if not, exit.
  if ( file.is_open() ) {
    //loop through each line and store information
    if (debug) std::cout <<"File open...\n";
    while ( file.good() ) {
      file >> n;
      file >> i;
      file >> rate;
      file >> err;
      if ( file.eof() ) break;

      //check to make sure main detector numbers are valid (ie, 1-8)
      if ( !(n<=num_md && n>0) ) {
        std::cout <<"One of your main detector numbers is incorrect...\n"
          <<"Error on line " <<line <<std::endl;
        std::cout <<"Exiting program." <<std::endl;
        exit(EXIT_FAILURE);
      }

      if (debug) {
        std::cout <<n <<" \t" <<i <<" \t"
          <<rate <<" \t" <<err <<std::endl;
      }
      line++;
      //store in appropriate tau_calc object
      tau_calcs[n-1].push_data(i,rate,err);
    }
  } else { //else file isn't open
    std::cout <<"Your file " <<filename <<" isn't open. \nExiting!"
      <<std::endl;
    exit(EXIT_FAILURE);
  }

  file.close();
  std::cout <<"File closed..." <<std::endl;

  for(int j=0; j<num_md; j++) {
    tau_calcs[j].processDataAutomatically();
    tau_list[j] = tau_calcs[j].getTauNs();
  }

  std::cout <<"The list of deadtimes by Main Detector number are:" <<std::endl
    <<"\tNumber \tTau (ns)" <<std::endl;
  for(int j=0; j<num_md; j++) {
    std::cout <<"\t" <<j+1 <<" \t" <<tau_list[j] <<std::endl;
  }

  //run process
  app->Run();

  return 0;
}


void helpscreen (void) {
  std::cout <<"Welcome to the Q-weak dead time calculator.\n"
    <<"This code is still a work in progress, but is functional.\n"
    <<"The help function isn't complete at the moment.\n"
    <<"However, to run, first compile the libraries, and then run.\n"
    <<std::endl
    <<"Usage example:\n\t"
    <<"g++ -Wall -g `root-config --libs --cflags` -Iinclude \n"
    <<"\tsrc/TauCalc.cpp taucalc.cpp -o test\n"
    <<"./test\n";
  exit(0);
}




