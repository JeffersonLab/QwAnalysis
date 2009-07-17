/*------------------------------------------------------------------------*//*!

 \file QwEventDisplay.cc
 \ingroup QwTrackingAnl

 \brief Example code for the event display routines

*//*-------------------------------------------------------------------------*/

// Standard C and C++ headers
#include <iostream>

// ROOT headers
#include <TApplication.h>

// Qweak event display header
#include "QwEventDisplay.h"

// Qweak tree event buffer header
#include "QwTreeEventBuffer.h"

// Ignore the next few lines
#include "Qoptions.h"
#include "options.h"
#include "Qset.h"
#include "Det.h"
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
Options opt;

int main (int argc, char* argv[])
{
  // Load the simulated event file
  QwTreeEventBuffer* treebuffer =
    new QwTreeEventBuffer (std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QweakSim.root");
  treebuffer->SetDebugLevel(0);

  // Start this application
  TApplication theApp("QwEventDisplay", &argc, argv);

  // Create a QwEventDisplay application
  QwEventDisplay* display = new QwEventDisplay(gClient->GetRoot(),1200,800);

  // Load the geometry (ignore)
  Qset qset;
  qset.FillDetectors((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();

  // Loop over the events in the file
  int fEntries = treebuffer->GetEntries();
  for (int fEvtNum = 0; fEvtNum < fEntries; fEvtNum++) {

    // Get hit list
    QwHitContainer* hitlist = treebuffer->GetHitList(fEvtNum);

    // Print hit list
    std::cout << "Printing hitlist ..." << std::cout;
    hitlist->Print();

    // Display the event
  }

  // Now run this application
  theApp.Run();

  return 0;

}
