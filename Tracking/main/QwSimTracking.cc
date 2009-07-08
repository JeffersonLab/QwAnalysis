/*------------------------------------------------------------------------*//*!

 \file QwSimTracking.cc \ingroup QwTrackingAnl

 \brief Example of the QwTreeEventBuffer class to read QweakSimG4 events

 This example illustrates the use of the QwTreeEventBuffer class.  It loads
 the file Tracking/prminput/QweakSim.root, which was produced with a 3-fold
 trigger in QweakSimG4 (hits in all of HDC, VDC-front, and VDC-back).  For
 every event a QwHitContainer is filled, which is then printed to std::cout.

*//*-------------------------------------------------------------------------*/

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>

// Qweak Tracking headers
#include "Det.h"
#include "Qset.h"
#include "Qoptions.h"
#include "options.h"

#include "QwTrackingWorker.h"
#include "QwEvent.h"

// Qweak Tree event buffer header
#include "QwTreeEventBuffer.h"

//Temporary global variables for sub-programs
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
Options opt;


int main (int argc, char* argv[])
{
  // Load the simulated event file
  QwTreeEventBuffer* treebuffer =
	new QwTreeEventBuffer (std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QweakSim.root");
  treebuffer->SetDebugLevel(1);

  // Load the geometry
  Qset qset;
  qset.FillDetectors((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();
  cout << "[QwTracking::main] Geometry loaded" << endl; // R3,R2

  // Set global options
  Qoptions qoptions;
  qoptions.Get((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.options").c_str());

  // Create the tracking worker
  QwTrackingWorker *trackingworker = new QwTrackingWorker("qwtrackingworker");
  trackingworker->SetDebugLevel(1);

    // Get hit list
    QwHitContainer* hitlist = treebuffer->GetHitList();

    // Print hit list
   hitlist->Print();

    // Process the hit list through the tracking worker
    QwEvent *event = trackingworker->ProcessHits(hitlist);

  return 0;

}
