/********************************************
 \author <b>Programmer:</b> Valerie Gray
 \author <b>Assisted By:</b> Juan Carlos Cornejo,
 Wouter Deconinck

 \brief <b>Purpose:</b> This file gets the run information

 Single function that creates a vector of the type MyRun_t

 \date <b>Date:</b> 05-08-2014
 \date <b>Modified:</b> 06-03-2014

 \note <b>Entry Conditions:</b>
 filename - file that has the list of all the
 runs in a pass
 ********************************************/

//standard include
#include <fstream>
#include <vector>
#include <iostream>
#include <stdlib.h>

//Valerian Root include
#include "Run.h"

//read in the run information
std::vector<MyRun_t> GetRun(const char* filename)
{
  // An input stream that contains run list
  ifstream run_data;
  run_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

//debugging - always on since this is the crux of getting any output
  if (!run_data.is_open())
    std::cout << "Run File: " << filename << "  not open" << std::endl;

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyRun_t> runs;
  MyRun_t tmp;

  // Get a list of the runs we need to process
  while (run_data >> tmp.run  // Read one number
         && !run_data.eof())  //&&  Make sure we are not at the End of File
  {
    runs.push_back(tmp);  // Push back to our runlist vector
  }

  run_data.close();
  return runs;
}
