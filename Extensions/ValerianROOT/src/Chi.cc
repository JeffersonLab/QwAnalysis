/********************************************
\author <b>Programmer:</b> Valerie Gray
\author <b>Assisted By:</b> Juan Carlos Cornejo,
    Wouter Deconinck

\brief <b>Purpose:</b> This file gets the Chi information

\date <b>Date:</b> 05-08-2015
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

//Valerian Root include
#include "Chi.h"

//Read in the Chi information for a given run number

std::vector<MyChi_t> GetChi(Int_t run, Int_t pass, TString path)
{
//  TString filename = TString("path")+Form("Chi_%d.txt",pass);
  TString filename = TString(Form("/home/vmgray/QweakTracking/data/pass5b/Chi/Chi_%d.txt",run));

  // An input stream that contains run list
  ifstream chi_data;
  chi_data.open(filename,std::ios::in);
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

  //Get the information from the text file I need to process
  //while I...
  while (chi_data >> tmp.package  //read in the pkg
    && chi_data >> tmp.direction  //read in the direction
    && chi_data >> tmp.Val  //read in the value
    && chi_data >> tmp.RMS  //read in the error
    && !chi_data.eof() )      // am not at the end of the file
  {
    chi.push_back(tmp);
    //this put all this information in the Chi_t
    //vector for the run
  }

  chi_data.close();

  return chi;
}

