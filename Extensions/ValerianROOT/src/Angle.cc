/********************************************
\author <b>Programmer:</b> Valerie Gray
\author <b>Assisted By:</b> Juan Carlos Cornejo,
    Wouter Deconinck

\brief <b>Purpose:</b> This file gets the Angle information

\date <b>Date:</b> 05-08-2015
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
#include "Angle.h"

//Read in the angle information for a given run number

std::vector<MyAngle_t> GetAngle(Int_t run, Int_t pass, TString path)
{
//  TString filename = TString("path")+Form("Angle_%d.txt",pass);
  TString filename = TString(Form("/home/vmgray/QweakTracking/data/pass5b/Angle/Angle_%d.txt",run));

  // An input stream that contains run list
  ifstream angle_data;
  angle_data.open(filename,std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

//debugging
//  if(!angle_data.is_open()) std::cout << "Angle File " << run << " not open" << std::endl;

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

  //Get the information from the text file I need to process
  //while I...
  while (angle_data >> tmp.package  //read in the pkg
    && angle_data >> tmp.Val  //read in the value
    && angle_data >> tmp.error  //read in the error
    && !angle_data.eof() )      // am not at the end of the file
  {
    angle.push_back(tmp);
    //this put all this information in the angle
    //vector for the run
  }

  angle_data.close();

  return angle;
}

