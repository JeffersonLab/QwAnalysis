/********************************************
 \author <b>Programmer:</b> Valerie Gray
 \author <b>Assisted By:</b>

 \brief <b>Purpose:</b> This file gets the number
 position on bar information

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
#include "PosOnBar.h"

//Read in the tigger information for a given run number

std::vector<MyPosOnBar_t> GetPosOnBar(Int_t run)
{

  TString filename = TString(gSystem->Getenv("VALERIAN"))
      + Form("/data/pass")
      + TString(gSystem->Getenv("PASS"))
      + Form(
          "/Everything_vs_position_on_bar/Everything_vs_position_on_bar_%d.txt",
          run);

  // An input stream that contains run list
  ifstream pos_data;
  pos_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

//debugging
//  if(!angle_data.is_open()) std::cout << "Angle File " << run << " not open" << std::endl;

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyPosOnBar_t> pos;
  MyPosOnBar_t tmp;

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

  if (pos_data.is_open())
  {
    //Get the information from the text file I need to process
    //while I...
    while (pos_data >> tmp.R3package  //read in the pkg
           && pos_data >> tmp.position  //read in the position on the bar
           && pos_data >> tmp.tracks  //read in the number of tracks
           && pos_data >> tmp.q2Val  //read in the Q2 value without weighting
           && pos_data >> tmp.q2Error  //read in the Error on the q2
           && pos_data >> tmp.lightVal  //read in the light yeild
           && pos_data >> tmp.lightError  //read in the Error on the light yeild
           && pos_data >> tmp.angleVal  //read in the angle value
           && pos_data >> tmp.angleError  //read in the Error on the angle values
           && !pos_data.eof())  // am not at the end of the file
    {
      pos.push_back(tmp);
      //this put all this information in the BLAH
      //vector for the run
    }

    pos_data.close();
  } else
  {
    //debugging
    if (atoi(gSystem->Getenv("DEBUG")) >= 3)
    {
      std::cout << "Position on Bar File " << run << " not open" << std::endl;
    }

    tmp.R3package = -1e6;
    tmp.position = -1e6;
    tmp.tracks = -1e6;
    tmp.q2Val = -1e6;
    tmp.q2Error = -1e6;
    tmp.lightVal = -1e6;
    tmp.lightError = -1e6;
    tmp.angleVal = -1e6;
    tmp.angleError = -1e6;

    pos.push_back(tmp);
  }
  return pos;
}

