/********************************************
 \author <b>Programmer:</b> Valerie Gray
 \author <b>Assisted By:</b> Juan Carlos Cornejo,
 Wouter Deconinck

 \brief <b>Purpose:</b> This file gets the Chi information

 \date <b>Date:</b> 05-08-2014
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
#include "Chi.h"

//Read in the Chi information for a given run number

std::vector<MyChi_t> GetChi(Int_t run)
{
//  TString filename = TString("path")+Form("Chi_%d.txt",pass);
  TString filename = TString(gSystem->Getenv("VALERIAN")) + Form("/data/pass")
                     + TString(gSystem->Getenv("PASS"))
                     + Form("/Chi/Chi_%d.txt", run);

  // An input stream that contains run list
  ifstream chi_data;
  chi_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyChi_t> chi;
  MyChi_t tmp;

  /*****************************
   Summary of what I will be doing:
   - I have defined a vector of MyChi_t (angle) and just one element
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

  if (chi_data.is_open())
  {
    //Get the information from the text file I need to process
    //while I...
    while (chi_data >> tmp.R2package  //read in the pkg
           && chi_data >> tmp.direction  //read in the direction
           && chi_data >> tmp.Val  //read in the value
           && chi_data >> tmp.RMS  //read in the Error
           && !chi_data.eof())  // am not at the end of the file
    {
      chi.push_back(tmp);
      //this put all this information in the Chi_t
      //vector for the run
    }
    chi_data.close();
  } else
  {
    //debugging
    if (atoi(gSystem->Getenv("DEBUG")) >= 3)
    {
      std::cout << "Chi File " << run << " not open" << std::endl;
    }

    tmp.R2package = -1e6;
    tmp.direction = -1e6;
    tmp.Val = -1e6;
    tmp.RMS = -1e6;
    chi.push_back(tmp);
  }
  return chi;
}

