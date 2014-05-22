/********************************************
\author <b>Programmer:</b> Valerie Gray
\author <b>Assisted By:</b>

\brief <b>Purpose:</b> This file gets the number
  track slope information

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
#include "TrackChi2.h"

//Read in the tigger information for a given run number

std::vector<MyTrackChi2_t> GetTrackChi2(Int_t run, Int_t pass, TString path)
{
//  TString filename = TString("path")+Form("Angle_%d.txt",pass);
  TString filename = TString(Form("/home/vmgray/QweakTracking/data/pass5b/R3_Tracks_match_up_Chi2/R3_Tracks_match_up_Chi2_%d.txt",run));

  // An input stream that contains run list
  ifstream tchi_data;
  tchi_data.open(filename,std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

//debugging
//  if(!angle_data.is_open()) std::cout << "Angle File " << run << " not open" << std::endl;

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyTrackChi2_t> tchi;
  MyTrackChi2_t tmp;

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
  while (tchi_data >> tmp.package  //read in the pkg
    && tchi_data >> tmp.VDC  //read in the VDC
    && tchi_data >> tmp.plane  //read in the plane
    && tchi_data >> tmp.nevents  //read in the number of events
    && tchi_data >> tmp.Val  //read in the chi Value
    && tchi_data >> tmp.nevents  //read in the error
    && !tchi_data.eof() )      // am not at the end of the file
  {
    tchi.push_back(tmp);
    //this put all this information in the BLAH
    //vector for the run
  }

  tchi_data.close();

  return tchi;
}

