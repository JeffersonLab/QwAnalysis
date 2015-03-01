/********************************************
 \author <b>Programmer:</b> Valerie Gray
 \author <b>Assisted By:</b>

 \brief <b>Purpose:</b> This file gets the number
 Q2 with cut information information

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
#include "Q2WithCut.h"

//Read in the tigger information for a given run number

std::vector<MyQ2Cut_t> GetQ2Cut(Int_t run)
{
  TString filename = TString(gSystem->Getenv("VALERIAN")) + Form("/data/pass")
                     + TString(gSystem->Getenv("PASS"))
                     + Form("/Q2_with_cuts/Q2_with_cuts_%d.txt", run);

  // An input stream that contains run list
  ifstream q2_data;
  q2_data.open(filename, std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyQ2Cut_t> q2;
  MyQ2Cut_t tmp;

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

  if (q2_data.is_open())
  {
    //Get the information from the text file I need to process
    //while I...
    while (q2_data >> tmp.R3package  //read in the pkg
           && q2_data >> tmp.q2Val  //read in the Q2 value without weighting
           && q2_data >> tmp.q2Error  //read in the Error on the q2
           && q2_data >> tmp.LWq2Val  //read in the light weighted q2
           && q2_data >> tmp.LWq2Error  //read in the Error on the light weighted q2
           && q2_data >> tmp.LWPq2Val  //read in the light weighted pedistal subtrackted value
           && q2_data >> tmp.LWPq2Error  //read in the Error on the Lightweighted pedistal subrtacted  values
           && !q2_data.eof())  // am not at the end of the file
    {
      q2.push_back(tmp);
      //this put all this information in the BLAH
      //vector for the run
    }
    q2_data.close();
  } else
  {
    //debugging
    if (atoi(gSystem->Getenv("DEBUG")) >= 3)
    {
      std::cout << "Q2 with cuts File " << run << " not open" << std::endl;
    }

    tmp.R3package = -1e6;
    tmp.q2Val = -1e6;
    tmp.q2Error = -1e6;
    tmp.LWq2Val = -1e6;
    tmp.LWq2Error = -1e6;
    tmp.LWPq2Val = -1e6;
    tmp.LWPq2Error = -1e6;
    q2.push_back(tmp);

  }
  return q2;
}

