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

  // Process the options
  std::cout << "About to process arguments:\n\n\n\n\n";
  gQwOptions.SetCommandLine(argc, argv);
  gQwLog.ProcessOptions(&gQwOptions);
  ProcessEventDisplay3DOptions(&gQwOptions);

  // Create a QwEventDisplay application
  QwEventDisplay3D* display = new QwEventDisplay3D(gClient->GetRoot(),1200,800);

  // Check to see if the user asked for a specific roation of the tracking
  // system
  if(gQwOptions.GetValue<int>("rotate")>0)
     display->SetRotation(gQwOptions.GetValue<int>("rotate"));

  // If the user passes the right parameter, open up that file
  if(gQwOptions.GetValue<int>("run")>0)
     display->OpenRoot(gQwOptions.GetValue<int>("run"));

  // Because this gets old real fast, this is only temporary
  //display->OpenRoot(5153);

  // Now run this application
  display->RedrawViews();
  theApp.Run();

  return 0;
}
