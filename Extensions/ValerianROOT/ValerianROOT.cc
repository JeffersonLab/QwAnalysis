/***********************************************************
Programmer: Valerie Gray
Purpose:

  Welcome to ValerianROOT, where you get more ROOT for your Commands
  This version of root has the stuctures for the total tracking
  run information.

Entry Conditions:
  - int argc
  - char** arg
Date: 05-08-2014
Modified: 05-09-2014
Assisted By:
*********************************************************/

/*********************************************************
 \file ValerianROOT.cc

 \ingroup QwAnalysis

 \brief ROOT wrapper with Qweak functionality for anayisis of
        all the tracking runs
*********************************************************/

// ROOT headers
#include <TSystem.h>
#include <TROOT.h>
#include <TString.h>
#include <TRint.h>

//standard includes
#include <iostream>

int main(int argc, char** argv)
{
  // Start CINT command prompt - which is then
  // The Valerian ROOT command prompt
  TRint* valerianrootrint =
    new TRint("\n \n Valerian ROOT - For a calm relaxing ROOT Experience \n \n", &argc, argv);
  // So that it can work with the installed version too
  valerianrootrint->SetPrompt("Valerian[%d] ");
  valerianrootrint->ProcessLine(".include include");
//  valerianrootrint->ProcessLine("gSystem->Load(\"libCint.so\");");
  //Run the interface
  std::cout << "For a calm and relaxing ROOT Experience" << std::endl;
  valerianrootrint->Run();
  //delete the object
  delete valerianrootrint;

  return 0;
}

