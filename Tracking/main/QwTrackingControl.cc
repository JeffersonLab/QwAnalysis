/*!
 * \file QwTrackingControl.cc
 * \ingroup QwThreadSafe
 *
 * \brief Executable which starts a dataserver, an analyzer and a Qw-Root prompt
 */

// Qweak headers
#include "QwControl.h"
#include "QwOptionsTracking.h"
#include "QwTrackingDataserver.h"
#include "QwTrackingAnalyzer.h"

int main (int argc, char** argv)
{
  /// Define the command line options
  gQwOptions.DefineOptions();
  /// Start "Qw-Root>" command prompt
  QwControl* qwctrl = new QwControl ("Qweak-Root Analyzer", &argc, argv);
  /// Start dataserver
  qwctrl->StartDataserver(new QwTrackingDataserver("dataserver"));
  /// Start analyzer
  qwctrl->StartAnalyzer(new QwTrackingAnalyzer("analyzer"));
  /// Run the analysis
  qwctrl->Run();
  delete qwctrl;
}
