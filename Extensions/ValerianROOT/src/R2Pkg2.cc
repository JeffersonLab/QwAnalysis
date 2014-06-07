/********************************************
 \author <b>Programmer:</b> Valerie Gray
 \author <b>Assisted By:</b>

 \brief <b>Purpose:</b> This file gets the number
 octant value for R2 pkg 2 information

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
#include "R2Pkg2.h"

//Read in the tigger information for a given run number

std::vector<MyR2Pkg2_t> GetR2Pkg2(Int_t run)
{

  TString filename = TString(gSystem->Getenv("VALERIAN"))
      + Form("/data/pass")
      + TString(gSystem->Getenv("PASS"))
      + Form("/OctantDetermination_R2_pkg2/OctantDetermination_R2_pkg2_%d.txt",
          run);

  // An input stream that contains run list
  ifstream pkg_data;
  pkg_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyR2Pkg2_t> pkg;
  MyR2Pkg2_t tmp;

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

  if (pkg_data.is_open())
  {
    //Get the information from the text file I need to process
    //while I...
    while (pkg_data >> tmp.octant  //read in the octant
           && !pkg_data.eof())  // am not at the end of the file
    {
      pkg.push_back(tmp);
      //this put all this information in the BLAH
      //vector for the run
    }
    pkg_data.close();
  } else
  {
    //debugging
    if (atoi(gSystem->Getenv("DEBUG")) >= 3)
    {
      std::cout << "R2 pkg 2 File " << run << " not open" << std::endl;
    }

    tmp.octant = -1e6;
    pkg.push_back(tmp);
  }
  return pkg;
}

