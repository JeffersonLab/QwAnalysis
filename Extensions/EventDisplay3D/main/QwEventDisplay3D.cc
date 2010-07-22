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

// Deprecated Qweak headers
#include "Det.h"
#include "Qset.h"

// Qweak subsystem headers
#include "QwSubsystemArrayTracking.h"
#include "QwGasElectronMultiplier.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwTriggerScintillator.h"
#include "QwMainDetector.h"
#include "QwTreeEventBuffer.h"

// Qweak event display header
#include "QwEventDisplay3D.h"
#include "QwOptionsEventDisplay3D.h"

int main (int argc, char* argv[])
{
  // Start this application
  TApplication theApp("QwEventDisplayTest3D", &argc, argv);

  // Create a QwEventDisplay application
  QwEventDisplay3D* display = new QwEventDisplay3D(gClient->GetRoot(),1200,800);

  // Process the options
  gQwOptions.SetCommandLine(argc, argv);
  gQwLog.ProcessOptions(&gQwOptions);
  ProcessEventDisplay3DOptions(&gQwOptions);
  display->OpenRoot(gQwOptions.GetValue<int>("run"));

  // Now run this application
  display->RedrawViews();
  theApp.Run();

  return 0;
}
