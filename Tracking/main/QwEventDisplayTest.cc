/*------------------------------------------------------------------------*//*!

 \file QwEventDisplayTest.cc
 \ingroup QwTrackingAnl

 \brief Example code for the event display routines

 \author Wouter Deconinck; MIT
 \author Derek Jones; The George Washington University; dwjones@jlab.org

*//*-------------------------------------------------------------------------*/
//
// Standard C and C++ headers
#include <iostream>

// ROOT headers
#include <TApplication.h>

// Qweak headers
#include "QwOptionsTracking.h"
#include "QwLog.h"

// Qweak subsystem headers
#include "QwSubsystemArrayTracking.h"
//#include "QwGasElectronMultiplier.h"
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

  ///  Load the tracking detectors from file
  QwSubsystemArrayTracking* detectors = new QwSubsystemArrayTracking(gQwOptions);
  detectors->ProcessOptions(gQwOptions);

  // Get detector geometry
  QwGeometry geometry = detectors->GetGeometry();


  // Load the simulated event file
  QwTreeEventBuffer *treebuffer = new QwTreeEventBuffer(geometry);
  treebuffer->ProcessOptions(gQwOptions);
  treebuffer->OpenNextFile();

  // Start this application
  TApplication theApp("QwEventDisplay", &argc, argv);

  // Create a QwEventDisplay application
  QwEventDisplay* display = new QwEventDisplay(gClient->GetRoot(),1200,800);
  display->SetEventBuffer(treebuffer);
  display->SetSubsystemArray(detectors);

  // Now run this application
  theApp.Run();

  return 0;

}
