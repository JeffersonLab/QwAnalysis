// C and C++ headers
#include <sys/time.h>

// Qweak headers
#include "QwLog.h"
#include "QwOptions.h"
#include "QwOptionsTracking.h"
#include "QwParameterFile.h"
#include "QwEventBuffer.h"

// Qweak tracking subsystems
#include "QwSubsystemArrayTracking.h"
#include "QwSciFiDetector.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwTriggerScintillator.h"
#include "QwMainDetector.h"
#include "QwScanner.h"
#include "QwRaster.h"


// Main function
Int_t main(Int_t argc, Char_t* argv[])
{
  ///  First, fill the search paths for the parameter files; this sets a static
  ///  variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Tracking/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");

  ///  Then, we set the command line arguments and the configuration filename,
  ///  and we define the options that can be used in them (using QwOptions).
  ///
  ///  false: do not try to add a configuration file with the program name
  gQwOptions.SetCommandLine(argc, argv, false);
  gQwOptions.ListConfigFiles();

  /// Setup screen and file logging
  gQwLog.ProcessOptions(&gQwOptions);


  ///  Create the event buffer
  QwEventBuffer eventbuffer;
  eventbuffer.ProcessOptions(gQwOptions);
  ///  Fail when run cannot be found
  if (eventbuffer.OpenNextStream() != CODA_OK) {
    return -1;
  }
  ///  Set the current event number for parameter file lookup
  QwParameterFile::SetCurrentRunNumber(eventbuffer.GetRunNumber());


  ///  Load the tracking detectors from file
  QwSubsystemArrayTracking tracking_detectors(gQwOptions);
  tracking_detectors.ProcessOptions(gQwOptions);
  ///  Get detector geometry
  QwGeometry geometry = tracking_detectors.GetGeometry();
  QwMessage << geometry << QwLog::endl;



  QwMessage << "Copy and paste the following lines in your QwGeant4 session or macro:" << QwLog::endl;
  QwMessage << "--------------------" << QwLog::endl;

  // Trigger scintillator
  QwGeometry trigscint = geometry.in(kPackageUp).in(kRegionIDTrig);
  if (trigscint.size() > 0) {
    QwMessage << "# Trigger Scintillator" << QwLog::endl;
    QwMessage << "/TriggerScintillator/SetCenterPositionInX "
        << trigscint.front()->GetXPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/TriggerScintillator/SetCenterPositionInY "
        << trigscint.front()->GetYPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/TriggerScintillator/SetCenterPositionInZ "
        << trigscint.front()->GetZPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/TriggerScintillator/SetTiltingAngle "
        << trigscint.front()->GetDetectorTiltInDeg()
        << " degree" << QwLog::endl;
    QwMessage << QwLog::endl;
  }


  // HDC
  QwGeometry hdc = geometry.in(kPackageUp).in(kRegionID2);
  if (hdc.size() > 0) {
    QwMessage << "# Horizontal Drift Chamber" << QwLog::endl;

    QwMessage << "/HDC/SetFrontCenterPositionInX "
        << hdc.front()->GetXPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/HDC/SetFrontCenterPositionInY "
        << hdc.front()->GetYPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/HDC/SetFrontCenterPositionInZ "
        << hdc.front()->GetZPosition()
        << " cm" << QwLog::endl;
    QwMessage << QwLog::endl;

    QwMessage << "/HDC/SetBackCenterPositionInX "
        << hdc.back()->GetXPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/HDC/SetBackCenterPositionInY "
        << hdc.back()->GetYPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/HDC/SetBackCenterPositionInZ "
        << hdc.back()->GetZPosition()
        << " cm" << QwLog::endl;
    QwMessage << QwLog::endl;
  }


  // VDC
  QwGeometry vdc = geometry.in(kPackageUp).in(kRegionID3);
  if (vdc.size() > 0) {
    QwMessage << "# Vertical Drift Chamber" << QwLog::endl;

    QwMessage << "/VDC/SetFrontCenterPositionInX "
        << vdc.front()->GetXPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/VDC/SetFrontCenterPositionInY "
        << vdc.front()->GetYPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/VDC/SetFrontCenterPositionInZ "
        << vdc.front()->GetZPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/VDC/DriftCell/SetWireAngleFront "
        << vdc.front()->GetElementAngleInDeg()
        << " degree" << QwLog::endl;
    QwMessage << QwLog::endl;

    QwMessage << "/VDC/SetBackCenterPositionInX "
        << vdc.back()->GetXPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/VDC/SetBackCenterPositionInY "
        << vdc.back()->GetYPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/VDC/SetBackCenterPositionInZ "
        << vdc.back()->GetZPosition()
        << " cm" << QwLog::endl;
    QwMessage << "/VDC/DriftCell/SetWireAngleBack "
        << vdc.back()->GetElementAngleInDeg()
        << " degree" << QwLog::endl;
    QwMessage << QwLog::endl;
  }

  QwMessage << "--------------------" << QwLog::endl;
}
