/********************************************
 \author <b>Programmer:</b> Valerie Gray
 \author <b>Assisted By:</b>

 \brief <b>Purpose:</b> This file gets the number
 of scattering energy information

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
#include "ScatEnergy.h"

//Read in the good tracks information for a given run number

std::vector<MyScatEnergy_t> GetScatEnergy(Int_t run)
{

  TString filename = TString(gSystem->Getenv("VALERIAN")) + Form("/data/pass")
                     + TString(gSystem->Getenv("PASS"))
                     + Form("/ScatteringEnergy/ScatteringEnergy_%d.txt", run);

  // An input stream that contains run list
  ifstream energy_data;
  energy_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyScatEnergy_t> energy;
  MyScatEnergy_t tmp;

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

  if (energy_data.is_open())
  {
    //Get the information from the text file I need to process
    //while I...
    while (energy_data >> tmp.R3package  //read in the pkg
           && energy_data >> tmp.Val  //read in Value
           && energy_data >> tmp.Error  //read in Error
           && !energy_data.eof())  // am not at the end of the file
    {
      energy.push_back(tmp);
      //this put all this information in the BLAH
      //vector for the run
    }
    energy_data.close();
  } else
  {
    //debugging
    if (atoi(gSystem->Getenv("DEBUG")) >= 3)
    {
      std::cout << "Scattering energy File " << run << " not open" << std::endl;
    }

    tmp.R3package = -1e6;
    tmp.Val = -1e6;
    tmp.Error = -1e6;
    energy.push_back(tmp);
  }
  return energy;
}

