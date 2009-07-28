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

#include "QwSubsystemArrayTracking.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"


//Temporary global variables for sub-programs
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
Options opt;


int main (int argc, char* argv[])
{
  // Fill the search paths for the parameter files
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH"))+"/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput");

  // Handle for the list of VQwSubsystemTracking objects
  QwSubsystemArrayTracking* QwDetectors = new QwSubsystemArrayTracking();
  // Region 2 HDC
  QwDetectors->push_back(new QwDriftChamberHDC("R2"));
  QwDetectors->GetSubsystem("R2")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) QwDetectors->GetSubsystem("R2"))->LoadQweakGeometry("qweak_new.geo");
  // Region 3 VDC
  QwDetectors->push_back(new QwDriftChamberVDC("R3"));
  QwDetectors->GetSubsystem("R3")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) QwDetectors->GetSubsystem("R3"))->LoadQweakGeometry("qweak_new.geo");

  // Get vector with detector info (by region, plane number)
  std::vector< std::vector< QwDetectorInfo > > detector_info;
  ((VQwSubsystemTracking*) QwDetectors->GetSubsystem("R2"))->GetDetectorInfo(detector_info);
  ((VQwSubsystemTracking*) QwDetectors->GetSubsystem("R3"))->GetDetectorInfo(detector_info);
  // TODO This is handled incorrectly, it just adds the three package after the
  // existing three packages from region 2...  GetDetectorInfo should descend
  // into the packages and add only the detectors in those packages.
  // Alternatively, we could implement this with a singly indexed vector (with
  // only an id as primary index) and write a couple of helper functions to
  // select the right subvectors of detectors.

  // Load the simulated event file
  QwTreeEventBuffer* treebuffer =
	new QwTreeEventBuffer (std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QweakSim.root");
  treebuffer->SetDetectorInfo(detector_info);
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

  int fEntries = treebuffer->GetEntries();
  for (int fEvtNum = 0; fEvtNum < fEntries && fEvtNum < 100; fEvtNum++) {
    // Get hit list
    QwHitContainer* hitlist = treebuffer->GetHitList(fEvtNum);

    // Print hit list
    std::cout << "Printing hitlist ..." << std::endl;
    hitlist->Print();

    // Process the hit list through the tracking worker
    //QwEvent *event = trackingworker->ProcessHits(hitlist);
  }

  return 0;

}
