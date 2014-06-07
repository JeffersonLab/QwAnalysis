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
#include "TrackSlope.h"

//Read in the tigger information for a given run number

std::vector<MyTrackSlope_t> GetTrackSlope(Int_t run)
{

  TString filename = TString(gSystem->Getenv("VALERIAN"))
      + Form("/data/pass") + TString(gSystem->Getenv("PASS"))
      + Form("/R3_Tracks_match_up_Slope/R3_Tracks_match_up_Slope_%d.txt", run);

  // An input stream that contains run list
  ifstream tslope_data;
  tslope_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyTrackSlope_t> tslope;
  MyTrackSlope_t tmp;

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

  if (tslope_data.is_open())
  {
    //Get the information from the text file I need to process
    //while I...
    while (tslope_data >> tmp.R3package  //read in the pkg
           && tslope_data >> tmp.VDC  //read in the VDC
           && tslope_data >> tmp.plane  //read in the plane
           && tslope_data >> tmp.nevents  //read in the number of events
           && tslope_data >> tmp.Val  //read in the chi Value
           && tslope_data >> tmp.Error  //read in the Error
           && !tslope_data.eof())  // am not at the end of the file
    {
      tslope.push_back(tmp);
      //this put all this information in the BLAH
      //vector for the run
    }
    tslope_data.close();
  } else
  {
    //debugging
    if (atoi(gSystem->Getenv("DEBUG")) >= 3)
    {
      std::cout << "Track slope File " << run << " not open" << std::endl;
    }

    tmp.R3package = -1e6;
    tmp.VDC = "none";
    tmp.plane = "none";
    tmp.nevents = -1e6;
    tmp.Val = -1e6;
    tmp.Error = -1e6;
    tslope.push_back(tmp);
  }
  return tslope;
}

