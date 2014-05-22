/********************************************
\author <b>Programmer:</b> Valerie Gray
\author <b>Assisted By:</b>

\brief <b>Purpose:</b> This file gets the number
  treelines chi information

\date <b>Date:</b> 05-19-2015
\date <b>Modified:</b>

\note <b>Entry Conditions:</b>
    - run number
    - Pass number
    - path, path to the data

********************************************/

//ROOT includes
#include <TString.h>

//standard include
#include <fstream>
#include <vector>
#include <iostream>

//Valerian Root include
#include "TreelineSlope.h"

//Read in the tigger information for a given run number

std::vector<MyTreelineSlope_t> GetTreelineSlope(Int_t run, Int_t pass, TString path)
{
//  TString filename = TString("path")+Form("Angle_%d.txt",pass);
  TString filename = TString(Form("/home/vmgray/QweakTracking/data/pass5b/R3_Treeline_match_up_Slope/R3_Treeline_match_up_Slope_%d.txt",run));

  // An input stream that contains run list
  ifstream tslope_data;
  tslope_data.open(filename,std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

//debugging
//  if(!angle_data.is_open()) std::cout << "Angle File " << run << " not open" << std::endl;

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyTreelineSlope_t> tslope;
  MyTreelineSlope_t tmp;

  /*****************************
  Summary of what I will be doing:
  - I have defined a vector of MyBLAH_t (BLAH) and just one element
      of this type (tmp)
  - I read in the file number by number filling the tmp element
  - the tmp element is a "sub"-element of the vector angle (for
      the run run), so push that into the BLAH vector.
  - then celebrate when it works!
  *****************************/

  //Get the information from the text file I need to process
  //while I...
  while (tslope_data >> tmp.package  //read in the pkg
    && tslope_data >> tmp.VDC  //read in the VDC
    && tslope_data >> tmp.plane  //read in the plane
    && tslope_data >> tmp.nevents  //read in the number of events
    && tslope_data >> tmp.Val  //read in the chi Value
    && tslope_data >> tmp.nevents  //read in the error
    && !tslope_data.eof() )      // am not at the end of the file
  {
    tslope.push_back(tmp);
    //this put all this information in the BLAH
    //vector for the run
  }

  tslope_data.close();

  return tslope;
}

