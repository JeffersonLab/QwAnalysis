/********************************************
\author <b>Programmer:</b> Valerie Gray
\author <b>Assisted By:</b>

\brief <b>Purpose:</b> This file gets the number
  octant value for R3 pkg 2 information

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
#include "R3Pkg2.h"

//Read in the tigger information for a given run number

std::vector<MyR3Pkg2_t> GetR3Pkg2(Int_t run, Int_t pass, TString path)
{
//  TString filename = TString("path")+Form("Angle_%d.txt",pass);
  TString filename = TString(Form("/home/vmgray/QweakTracking/data/pass5b/OctantDetermination_R3_pkg2/OctantDetermination_R3_pkg2_%d.txt",run));

  // An input stream that contains run list
  ifstream pkg_data;
  pkg_data.open(filename,std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

//debugging
//  if(!angle_data.is_open()) std::cout << "Angle File " << run << " not open" << std::endl;

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyR3Pkg2_t> pkg;
  MyR3Pkg2_t tmp;

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
  while (pkg_data >> tmp.octant  //read in the octant
    && !pkg_data.eof() )      // am not at the end of the file
  {
    pkg.push_back(tmp);
    //this put all this information in the BLAH
    //vector for the run
  }

  pkg_data.close();

  return pkg;
}

