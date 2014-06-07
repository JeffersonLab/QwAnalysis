/********************************************
 \author <b>Programmer:</b> Valerie Gray
 \author <b>Assisted By:</b> Juan Carlos Cornejo,
 Wouter Deconinck

 \brief <b>Purpose:</b> This file gets the Log Chi information

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
#include "LogChi.h"

//Read in the log Chi information for a given run number

std::vector<MyLogChi_t> GetLogChi(Int_t run)
{
//  TString filename = TString("path")+Form("LogChi_%d.txt",pass);
  TString filename = TString(gSystem->Getenv("VALERIAN")) + Form("/data/pass")
                     + TString(gSystem->Getenv("PASS"))
                     + Form("/LogChi/LogChi_%d.txt", run);

  // An input stream that contains run list
  ifstream logchi_data;
  logchi_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyLogChi_t> logchi;
  MyLogChi_t tmp;

  /*****************************
   Summary of what I will be doing:
   - I have defined a vector of MyLogChi_t (angle) and just one element
   of this type (tmp)
   - I read in the file number by number filling the tmp element
   - the tmp element is a "sub"-element of the vector angle (for
   the run run), so push that into the angle vector.
   - then celebrate when it works!
   *****************************/
  if (logchi_data.is_open())
  {
    //Get the information from the text file I need to process
    //while I...
    while (logchi_data >> tmp.R2package  //read in the pkg
           && logchi_data >> tmp.direction  //read in the direction
           && logchi_data >> tmp.Val  //read in the value
           && logchi_data >> tmp.RMS  //read in the Error
           && !logchi_data.eof())  // am not at the end of the file
    {
      logchi.push_back(tmp);
      //this put all this information in the LogChi
      //vector for the run
    }

    logchi_data.close();
  } else
  {
    //debugging
    if (atoi(gSystem->Getenv("DEBUG")) >= 3)
    {
      std::cout << "Log Chi File " << run << " not open" << std::endl;
    }

    tmp.R2package = -1e6;
    tmp.direction = -1e6;
    tmp.Val = -1e6;
    tmp.RMS = -1e6;
    logchi.push_back(tmp);
  }

  return logchi;
}

