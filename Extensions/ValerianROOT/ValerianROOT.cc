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

//Load the Valerian ROOT includes
/*
#include "Angle.h"
#include "BeamPositionX.h"
#include "BeamPositionY.h"
#include "Chi.h"
#include "GoodTrack.h"
#include "LogChi.h"
#include "NTrigger.h"
#include "PartialTrackChi2.h"
#include "PartialTrackSlope.h"
#include "PosOnBar.h"
#include "Q2PhiMatch.h"
#include "Q2PhiMatchProj.h"
#include "Q2ThetaMatch.h"
#include "Q2ThetaMatchProj.h"
#include "Q2WithCut.h"
#include "Q2WithLoss.h"
#include "Q2WithOutLoss.h"
#include "QTOR.h"
#include "R2Pkg1.h"
#include "R2Pkg2.h"
#include "R3PhiOffset.h"
#include "R3Pkg1.h"
#include "R3Pkg2.h"
#include "R3ThetaOffset.h"
#include "RawTrack.h"
#include "Residual.h"
#include "ScatEnergy.h"
#include "TrackChi2.h"
#include "TrackSlope.h"
#include "TreelineChi2.h"
#include "TreelineSlope.h"

#include "Run.h"
*/

int main(int argc, char** argv)
{

  // Start CINT command prompt - which is then
  // The Valerian ROOT command prompt
  TRint* valerianrootrint =
    new TRint("Valerian ROOT - For a calm relaxing ROOT Experience", &argc, argv);
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

