/*------------------------------------------------------------------------*//*!

 \file QwEventDisplay3D.cc
 \ingroup QwTrackingAnl

 \brief Example code for the event display routines

*//*-------------------------------------------------------------------------*/

// Standard C and C++ headers
#include <iostream>

// ROOT headers
#include <TApplication.h>

// Qweak headers
//#include "QwOptionsTracking.h"
#include "QwLog.h"
#include "QwOptions.h"

// Qweak subsystem headers
#include "QwSubsystemArrayTracking.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwTriggerScintillator.h"
#include "QwMainDetector.h"
#include "QwTreeEventBuffer.h"
#include "QwParameterFile.h"

// Qweak event display header
#include "QwEventDisplay3D.h"
#include "QwOptionsEventDisplay3D.h"

int main (int argc, char* argv[])
{
  // Start this application
  TApplication theApp("QwEventDisplayTest3D", &argc, argv);

  ///  First, fill the search paths for the parameter files; this sets a static
  ///  variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") +
      "/Extensions/EventDisplay3D/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Tracking/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");

  // Define the options
  DefineOptionsEventDisplay3D(gQwOptions);

  // Process the options
  gQwOptions.SetCommandLine(argc, argv);
  gQwLog.ProcessOptions(&gQwOptions);

  // Create a QwEventDisplay application
  QwEventDisplay3D* display = new QwEventDisplay3D(gClient->GetRoot(),1200,800);

  // Display the tracks? treelines?
  display->SetDrawTracks(!gQwOptions.GetValue<bool>("disable-drawTracks"));
  display->SetDrawTreeLines(!gQwOptions.GetValue<bool>("disable-drawTreeLines"));

  // Display all the wires?
  display->SetShowAllRegion3(gQwOptions.GetValue<bool>("showAllRegion3"));
  display->SetShowAllRegion2(gQwOptions.GetValue<bool>("showAllRegion2"));

  // If the user passes the right parameter, open up that file
  if (gQwOptions.GetIntValuePair("run").first > 0)
     display->OpenRoot(gQwOptions.GetIntValuePair("run").first);

  // Now run this application
  display->RedrawViews();
  theApp.Run();

  return 0;
}
