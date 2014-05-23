/********************************************
\author <b>Programmer:</b> Valerie Gray
\author <b>Assisted By:</b>

\brief <b>Purpose:</b> This file gets the number
  Phi match of  bar information

\date <b>Date:</b> 05-19-2015
\date <b>Modified:</b>

\note <b>Entry Conditions:</b>
    - run number
    - Pass number
    - path, path to the data

********************************************/

//ROOT includes
#include <TString.h>
#include <TSystem.h>

//standard include
#include <fstream>
#include <vector>
#include <iostream>

//Valerian Root include
#include "Q2PhiMatch.h"

//Read in the tigger information for a given run number

std::vector<MyQ2PhiMatch_t> GetQ2PhiMatch(Int_t run, Int_t pass, TString path)
{
//  TString filename = TString("path")+Form("Angle_%d.txt",pass);
  TString filename = TString(gSystem->Getenv("VALERIAN")) + Form("/data/pass") + TString(gSystem->Getenv("PASS")) + Form("/PhiMatch/PhiMatch_%d.txt",run);

  // An input stream that contains run list
  ifstream phi_data;
  phi_data.open(filename,std::ios::in);
  //this has an input mode of std::ios::in
  //and be keyboard, or file input, and read only

//debugging
//  if(!angle_data.is_open()) std::cout << "Angle File " << run << " not open" << std::endl;

  // A vector that will contain all the runs
  // To make organization for now easier, we will define a special
  // structure called MyRun_t which will include what we need
  std::vector<MyQ2PhiMatch_t> phi;
  MyQ2PhiMatch_t tmp;

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
  while (phi_data >> tmp.package  //read in the pkg
    && phi_data >> tmp.Val  //read in the value of phi match
    && phi_data >> tmp.error  //read in the error on phi match
    && !phi_data.eof() )      // am not at the end of the file
  {
    phi.push_back(tmp);
    //this put all this information in the BLAH
    //vector for the run
  }

  phi_data.close();

  return phi;
}

