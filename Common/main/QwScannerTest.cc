#include "QwSubsystemArrayTracking.h"
#include "QwSubsystemArrayParity.h"
#include "QwGasElectronMultiplier.h"
#include "QwBeamLine.h"
#include "QwScanner.h"

#include "Det.h"
#include "options.h"

// Temporary global variables for sub-programs
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
Options opt;


int main(Int_t argc, Char_t* argv[])
{
  QwScanner* scanner = new QwScanner("FPS");

  QwSubsystemArrayTracking QwTrackingDetectors;
  QwTrackingDetectors.push_back(new QwGasElectronMultiplier("GEM"));
  QwTrackingDetectors.push_back(scanner);
  QwTrackingDetectors.Print();

  QwSubsystemArrayParity QwParityDetectors;
  QwParityDetectors.push_back(new QwBeamLine("BL"));
  QwParityDetectors.push_back(scanner);
  QwParityDetectors.Print();

  // Crashes because the shared_pointer deletes scanner twice on out of scope.
  // This example code would never be used in a crash-inducing way.

  return 0;
}
