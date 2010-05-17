/*!
 * \file QwTrackingControl.cc
 * \ingroup QwThreadSafe
 *
 * \brief Executable which starts a dataserver, an analyzer and a Qw-Root prompt
 */

// Qweak headers
#include "QwRint.h"
#include "QwControl.h"
#include "QwOptionsTracking.h"
#include "QwTrackingDataserver.h"
#include "QwTrackingAnalyzer.h"

int main (int argc, char** argv)
{
  /// Define the command line options
  DefineOptionsTracking(gQwOptions);

  /// Start "Qw-Root []" command prompt
  QwRint* qwrint = new QwRint("Qweak-Root Analyzer", &argc, argv);

  /// Start control in different thread
  QwControl* qwcontrol = new QwControl("Qweak Control", &argc, argv);
  /// Start dataserver
  qwcontrol->StartDataserver(new QwTrackingDataserver("dataserver"));
  /// Start single analyzer
  qwcontrol->StartAnalyzer(new QwTrackingAnalyzer("analyzer"));
  /// Start control (in a different thread)
  qwcontrol->StartControl();

  if (gQwOptions.GetValue<bool>("batch") == false) qwrint->Run();

  // Delete prompt
  delete qwrint;
}
