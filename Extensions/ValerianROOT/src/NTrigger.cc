/********************************************
 \author <b>Programmer:</b> Valerie Gray
 \author <b>Assisted By:</b>

 \brief <b>Purpose:</b> This file gets the number
 trigger information

 \date <b>Date:</b> 05-15-2014
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
#include "NTrigger.h"

//Read in the tigger information for a given run number

std::vector<MyTrigger_t> GetTrigger(Int_t run)
{

  TString filename = TString(gSystem->Getenv("VALERIAN")) + Form("/data/pass")
                     + TString(gSystem->Getenv("PASS"))
                     + Form("/NumTriggers/NumTriggers_%d.txt", run);

  // An input stream that contains run list
  ifstream trigger_data;
  trigger_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyTrigger_t> trigger;
  MyTrigger_t tmp;

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

  if (trigger_data.is_open())
  {
    //Get the information from the text file I need to process
    //while I...
    while (trigger_data >> tmp.number  //read in the number
           && !trigger_data.eof())  // am not at the end of the file
    {
      trigger.push_back(tmp);
      //this put all this information in the BLAH
      //vector for the run
    }
    trigger_data.close();
  } else
  {
    tmp.number = -1e6;
    trigger.push_back(tmp);
  }
  return trigger;
}

