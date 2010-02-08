/*!
 *
 * \file QwControl.cc
 *
 * \brief Program which starts a dataserver, an analyzer and a Qw-Root prompt
 *
 * \ingroup QwTracking
 *
 */

// Qweak headers
#include "QwControl.h"
#include "QwOptionsTracking.h"
#include "QwTrackingDataserver.h"
#include "QwTrackingAnalyzer.h"

int main (int argc, char** argv)
{
  // Define the command line options
  gQwOptions.DefineOptions();
  // Start Qw-Root command prompt
  QwControl* qwctrl = new QwControl ("Qweak-Root Analyzer", &argc, argv);
  // Start dataserver
  qwctrl->StartDataserver(new QwTrackingDataserver("dataserver"));
  // Start analyzer
  qwctrl->StartAnalyzer(new QwTrackingAnalyzer("analyzer"));
  qwctrl->Run();
  delete qwctrl;
}
