/*------------------------------------------------------------------------*//*!

 \file QwEventDisplayTest.cc
 \ingroup QwTrackingAnl

 \brief Example code for the event display routines

*//*-------------------------------------------------------------------------*/

// Standard C and C++ headers
#include <iostream>

// ROOT headers
#include <TApplication.h>

// Qweak subsystem headers
#include "QwSubsystemArrayTracking.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
// Qweak tree event buffer header
#include "QwTreeEventBuffer.h"

// Qweak event display header
#include "QwEventDisplay.h"

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
  std::string filename = std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QweakSim.root";
  QwTreeEventBuffer *treebuffer = new QwTreeEventBuffer (filename, detector_info);
  treebuffer->SetDetectorInfo(detector_info);
  treebuffer->SetDebugLevel(0);

  // Start this application
  TApplication theApp("QwEventDisplay", &argc, argv);

  // Create a QwEventDisplay application
  QwEventDisplay* display = new QwEventDisplay(gClient->GetRoot(),1200,800);
  display->SetEventBuffer(treebuffer);
  display->SetSubsystemArray(QwDetectors);

  // Load the geometry (ignore)
  Qset qset;
  qset.FillDetectors((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();

  // Loop over the events in the file
  int fEntries = treebuffer->GetEntries();
  for (int fEvtNum = 0; fEvtNum < 0 /*fEntries*/; fEvtNum++) {

    // Get hit list
    QwHitContainer* hitlist = treebuffer->GetHitList(fEvtNum);

    // Print hit list
    std::cout << "Printing hitlist ..." << std::endl;
    std::cout << *hitlist << std::endl;

    // Display the event
  }

  // Now run this application
  theApp.Run();

  return 0;

}
