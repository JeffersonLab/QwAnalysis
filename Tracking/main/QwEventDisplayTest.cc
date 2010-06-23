/*------------------------------------------------------------------------*//*!

 \file QwEventDisplayTest.cc
 \ingroup QwTrackingAnl

 \brief Example code for the event display routines

*//*-------------------------------------------------------------------------*/
//
// Standard C and C++ headers
#include <iostream>

// ROOT headers
#include <TApplication.h>

// Qweak headers
#include "QwOptionsTracking.h"
#include "QwLog.h"

// Deprecated Qweak headers
#include "Det.h"
#include "Qset.h"

// Qweak subsystem headers
#include "QwSubsystemFactory.h"
#include "QwSubsystemArrayTracking.h"
#include "QwGasElectronMultiplier.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwTriggerScintillator.h"
#include "QwMainDetector.h"
#include "QwTreeEventBuffer.h"
#include "QwEventBuffer.h"

// Qweak event display header
#include "QwEventDisplay.h"

int main (int argc, char* argv[])
{
  /// First, we set the command line arguments and the configuration filename,
  /// and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qweventdisplay.conf");
  // Define the command line options
  DefineOptionsTracking(gQwOptions);

  /// Now we setup the message logging facilities with the requested loglevels.
  if (gQwOptions.HasValue("QwLog.logfile"))
    gQwLog.InitLogFile(gQwOptions.GetValue<string>("QwLog.logfile"));
  gQwLog.SetFileThreshold(QwLog::QwLogLevel(gQwOptions.GetValue<int>("QwLog.loglevel-file")));
  gQwLog.SetScreenThreshold(QwLog::QwLogLevel(gQwOptions.GetValue<int>("QwLog.loglevel-screen")));
  gQwLog.SetScreenColor(gQwOptions.GetValue<bool>("QwLog.color"));

  /// Fill the search paths for the parameter files
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWSCRATCH")+"/setupfiles");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS")+"/Tracking/prminput");

  // Handle for the list of VQwSubsystemTracking objects
  QwSubsystemArrayTracking* detectors = new QwSubsystemArrayTracking();

  // Region 1 GEM
  detectors->push_back(VQwSubsystemFactory::Create("QwGasElectronMultiplier", "R1"));
  detectors->GetSubsystemByName("R1")->LoadChannelMap("qweak_cosmics_hits.map");
  detectors->GetSubsystemByName("R1")->LoadGeometryDefinition("qweak_new.geo");

  // Region 2 HDC
  detectors->push_back(VQwSubsystemFactory::Create("QwDriftChamberHDC", "R2"));
  detectors->GetSubsystemByName("R2")->LoadChannelMap("qweak_cosmics_hits.map");
  detectors->GetSubsystemByName("R2")->LoadGeometryDefinition("qweak_new.geo");

  // Region 3 VDC
  detectors->push_back(VQwSubsystemFactory::Create("QwDriftChamberVDC", "R3"));
  detectors->GetSubsystemByName("R3")->LoadChannelMap("TDCtoDL.map");
  detectors->GetSubsystemByName("R3")->LoadGeometryDefinition("qweak_new.geo");

  // Region 4 TS
  detectors->push_back(VQwSubsystemFactory::Create("QwTriggerScintillator", "TS"));
  detectors->GetSubsystemByName("TS")->LoadGeometryDefinition("qweak_new.geo");

  // Region 5 MD
  detectors->push_back(VQwSubsystemFactory::Create("QwMainDetector", "MD"));
  detectors->GetSubsystemByName("MD")->LoadGeometryDefinition("qweak_new.geo");

  // Get vector with detector info (by region, plane number)
  std::vector< std::vector< QwDetectorInfo > > detector_info;
  detectors->GetSubsystemByName("R1")->GetDetectorInfo(detector_info);
  detectors->GetSubsystemByName("R2")->GetDetectorInfo(detector_info);
  detectors->GetSubsystemByName("R3")->GetDetectorInfo(detector_info);
  detectors->GetSubsystemByName("TS")->GetDetectorInfo(detector_info);
  detectors->GetSubsystemByName("MD")->GetDetectorInfo(detector_info);
  // TODO This is handled incorrectly, it just adds the three package after the
  // existing three packages from region 2...  GetDetectorInfo should descend
  // into the packages and add only the detectors in those packages.
  // Alternatively, we could implement this with a singly indexed vector (with
  // only an id as primary index) and write a couple of helper functions to
  // select the right subvectors of detectors.

  // Load the simulated event file
  std::string filename = getenv_safe_string("QWANALYSIS")+"/Tracking/prminput/QweakSim.root";
  QwTreeEventBuffer *treebuffer = new QwTreeEventBuffer (filename, detector_info);
  treebuffer->SetDetectorInfo(detector_info);

  // Start this application
  TApplication theApp("QwEventDisplay", &argc, argv);

  // Create a QwEventDisplay application
  QwEventDisplay* display = new QwEventDisplay(gClient->GetRoot(),1200,800);
  display->SetEventBuffer(treebuffer);
  display->SetSubsystemArray(detectors);

  // Load the geometry
  Qset qset;
  qset.FillDetectors((getenv_safe_string("QWANALYSIS")+"/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();
  std::cout << "[QwTracking::main] Geometry loaded" << std::endl; // R3,R2

  // Now run this application
  theApp.Run();

  return 0;

}
