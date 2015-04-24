/********************************************
 * \author <b>Programmer:<\b> Valerie Gray
 * \author <b>Assisted By:<\b>
 *
 * \brief <b>Purpose:</b> This is header file
 *      The Read in Run number function.
 *
 *      As it is used by all the macros that
 *      will be a part of Valerian Root this is
 *      the best way to deal with that
 *
 * \date <b>Date:</b> 10-01-2014
 * \date <b>Modified:</b>
 *
 * \note <b>Entry Conditions:</b> none
 *
********************************************/

/***********************************************************
 Function: Read_In_Run_Numbers
 Purpose:
 - To read in all the run numbers of interest
 - sort the values in that file from lowest to highest
 which is needed for filling the histogram

 Entry Conditions:
 - file_name - the name of the text file with the run numbers of
 interest in it

 Global:
 - RUNNUMLIST - vector that will store these run numbers

 Exit Conditions: none
 Called By: Q2_vs_Run
 Date: 06-04-2014
 Modified:
 *********************************************************/

#include <vector>
#include <Rtypes.h>
#include <fstream>
#include <algorithm>
#include <iostream>

std::vector<Int_t> RUNNUMLIST;

void Read_In_Run_Numbers(std::string file_name)
{
  Int_t tmp;

  TString filename = TString(gSystem->Getenv("QWANALYSIS"))
      + "/Extensions/ValerianROOT/runlists/" + file_name;

  // debugging
//  std::cout << "filename: " << filename << std::endl;

  // An input stream that contains run list
  ifstream runnum_data;
  runnum_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

  //if this file is open and while it is open fill the vector with
  //the run numbers
  if (runnum_data.is_open())
  {
    //debugging
 //   std::cout << "run data is open made it in the if loop" << std::endl;

    while (runnum_data >> tmp  //while reading in the file - to temporary variable tmp
           && !runnum_data.eof())  //and not at the end of the file
    {
      //debugging
//      std::cout << "in the while loop - this means there has been something read in..."
//           << std::endl;
      RUNNUMLIST.push_back(tmp);  //put the read in run number into the run list vector
    }

  } else
  {
    std::cout << "Run File: " << filename << "  not open" << std::endl;
  }

  //debugging
//  std::cout << "the size of the vector is: " << RUNNUMLIST.size() << std::endl;

   //debugging!

  //sort the vector so that the entries go form low to high
  std::sort(RUNNUMLIST.begin(), RUNNUMLIST.end());

   //debugging!

  return;
}
