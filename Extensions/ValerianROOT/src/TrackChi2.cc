/********************************************
 \author <b>Programmer:</b> Valerie Gray
 \author <b>Assisted By:</b>

 \brief <b>Purpose:</b> This file gets the number
 partial track chi information

 \date <b>Date:</b> 05-19-2014
 \date <b>Modified:</b> 05-25-2014

 \note <b>Entry Conditions:</b>
 - run number
 ********************************************/

//ROOT includes
#include <TString.h>
#include <TSystem.h>

//standard include
#include <fstream>
#include <vector>
#include <iostream>
#include <cstdlib>

//Valerian Root include
#include "TrackChi2.h"

//Read in the tigger information for a given run number

std::vector<MyTrackChi2_t> GetTrackChi2(Int_t run)
{

  TString filename = TString(gSystem->Getenv("VALERIAN"))
      + Form("/data/pass") + TString(gSystem->Getenv("PASS"))
      + Form("/R3_Tracks_match_up_Chi2/R3_Tracks_match_up_Chi2_%d.txt", run);

  // An input stream that contains run list
  ifstream tchi_data;
  tchi_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

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

  /*****************************
   if there is a file then fill it with the information,
   if not then fill it with something that is unreal,
   ie -1e6 cause well that is not likely to happen.

   Then when it put into the tree something is there
   and not garbage.  The garbage is bad as we don't know
   what it and it can play tricks on us.  Bad ones :(
   *****************************/

  if (tchi_data.is_open())
  {
    //Get the information from the text file I need to process
    //while I...
    while (tchi_data >> tmp.R3package  //read in the pkg
           && tchi_data >> tmp.VDC  //read in the VDC
           && tchi_data >> tmp.plane  //read in the plane
           && tchi_data >> tmp.nevents  //read in the number of events
           && tchi_data >> tmp.Val  //read in the chi Value
           && tchi_data >> tmp.RMS  //read in the Error
           && !tchi_data.eof())  // am not at the end of the file
    {
      tchi.push_back(tmp);
      //this put all this information in the BLAH
      //vector for the run
    }
    tchi_data.close();
  } else
  {
    //debugging
    if (atoi(gSystem->Getenv("DEBUG")) >= 3)
    {
      std::cout << "Track Chi2 File " << run << " not open" << std::endl;
    }

    tmp.R3package = -1e6;
    tmp.VDC = "none";
    tmp.plane = "none";
    tmp.nevents = -1e6;
    tmp.Val = -1e6;
    tmp.RMS = -1e6;
    tchi.push_back(tmp);
  }
  return tchi;
}

