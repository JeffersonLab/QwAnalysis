/********************************************
 \author <b>Programmer:</b> Valerie Gray
 \author <b>Assisted By:</b> Juan Carlos Cornejo,
 Wouter Deconinck

 \brief <b>Purpose:</b> This file gets the Angle information

 \date <b>Date:</b> 05-08-2015
 \date <b>Modified:</b> 06-03-2014

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
#include "Angle.h"

//Read in the angle information for a given run number

std::vector<MyAngle_t> GetAngle(Int_t run)
{

  TString filename = TString(gSystem->Getenv("VALERIAN")) + Form("/data/pass")
                     + TString(gSystem->Getenv("PASS"))
                     + Form("/Angle/Angle_%d.txt", run);

  // An input stream that contains run list
  ifstream angle_data;
  angle_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyAngle_t> angle;
  MyAngle_t tmp;

  /*****************************
   Summary of what I will be doing:
   - I have defined a vector of MyAngle_t (angle) and just one element
   of this type (tmp)
   - I read in the file number by number filling the tmp element
   - the tmp element is a "sub"-element of the vector angle (for
   the run run), so push that into the angle vector.
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

  if (angle_data.is_open())
  {
    //Get the information from the text file I need to process
    //while I...
    while (angle_data >> tmp.R3package  //read in the pkg
           && angle_data >> tmp.Val  //read in the value
           && angle_data >> tmp.Error  //read in the Error
           && !angle_data.eof())  // am not at the end of the file
    {
      angle.push_back(tmp);
      //this put all this information in the angle
      //vector for the run
    }

    angle_data.close();
  } else
  {
    //debugging
    if (atoi(gSystem->Getenv("DEBUG")) >= 3)
    {
      std::cout << "Angle File " << run << " not open" << std::endl;
    }
    tmp.R3package = -1e6;
    tmp.Val = -1e6;
    tmp.Error = -1e6;
    angle.push_back(tmp);
  }

  return angle;
}

