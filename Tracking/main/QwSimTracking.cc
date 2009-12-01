/*------------------------------------------------------------------------*//*!

 \file QwSimTracking.cc \ingroup QwTrackingAnl

 \brief Example of the QwTreeEventBuffer class to read QweakSimG4 events

 This example illustrates the use of the QwTreeEventBuffer class.  It loads
 the file Tracking/prminput/QweakSim.root, which was produced with a 3-fold
 trigger in QweakSimG4 (hits in all of HDC, VDC-front, and VDC-back).  For
 every event a QwHitContainer is filled, which is then printed to std::cout.

*//*-------------------------------------------------------------------------*/

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>

// ROOT headers
#include <TFile.h>
#include <TTree.h>

// Qweak Tracking headers
#include "Det.h"
#include "Qset.h"
#include "Qoptions.h"
#include "options.h"

#include "QwTrackingWorker.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwEvent.h"

// Qweak Tree event buffer header
#include "QwTreeEventBuffer.h"

#include "QwCommandLine.h"
#include "QwSubsystemArrayTracking.h"
#include "QwGasElectronMultiplier.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"

#include "QwLog.h"
#include "QwOptions.h"

// Temporary global variables for sub-programs
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
Options opt;


// Debug level
static const bool kDebug = false;
// ROOT file output
static const bool kTree = true;
static const bool kHisto = true;

int main (int argc, char* argv[])
{
  // Set the command line arguments and configuration file
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwsimtracking.conf");
  // Define some options
  gQwOptions.AddOptions()("run,r", po::value<string>(), "run range in format #[:#]");
  gQwOptions.AddOptions()("event,e", po::value<string>(), "event range in format #[:#]");


  // Message logging facilities
  if (gQwOptions.HasValue("logfile"))
    gQwLog.InitLogFile(gQwOptions.GetValue<string>("logfile"));
  gQwLog.SetFileThreshold(QwLog::QwLogLevel(gQwOptions.GetValue<int>("loglevel-file")));
  gQwLog.SetScreenThreshold(QwLog::QwLogLevel(gQwOptions.GetValue<int>("loglevel-screen")));


  // Fill the search paths for the parameter files
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH"))+"/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput");

  // Handle for the list of VQwSubsystemTracking objects
  QwSubsystemArrayTracking* detectors = new QwSubsystemArrayTracking();

  // Region 1 GEM
  detectors->push_back(new QwGasElectronMultiplier("R1"));
//  detectors->GetSubsystem("R1")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R1"))->LoadQweakGeometry("qweak_new.geo");

  // Region 2 HDC
  detectors->push_back(new QwDriftChamberHDC("R2"));
  detectors->GetSubsystem("R2")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R2"))->LoadQweakGeometry("qweak_new.geo");

  // Region 3 VDC
  detectors->push_back(new QwDriftChamberVDC("R3"));
  // detectors->GetSubsystem("R3")->LoadChannelMap("qweak_cosmics_hits.map");
  detectors->GetSubsystem("R3")->LoadChannelMap("TDCtoDL.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R3"))->LoadQweakGeometry("qweak_new.geo");


  // Get vector with detector info (by region, plane number)
  std::vector< std::vector< QwDetectorInfo > > detector_info;
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R2"))->GetDetectorInfo(detector_info);
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R3"))->GetDetectorInfo(detector_info);
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R1"))->GetDetectorInfo(detector_info);
  // TODO This is handled incorrectly, it just adds the three package after the
  // existing three packages from region 2...  GetDetectorInfo should descend
  // into the packages and add only the detectors in those packages.
  // Alternatively, we could implement this with a singly indexed vector (with
  // only an id as primary index) and write a couple of helper functions to
  // select the right subvectors of detectors.

  // Load the geometry
  Qset qset;
  qset.FillDetectors((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();
  std::cout << "[QwTracking::main] Geometry loaded" << std::endl; // R3,R2

  // Set global options
  Qoptions qoptions;
  qoptions.Get((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.options").c_str());

  // Create the tracking worker
  QwTrackingWorker *trackingworker = new QwTrackingWorker("qwtrackingworker");
  if (kDebug) trackingworker->SetDebugLevel(1);


  // Loop over all runs
  for (UInt_t run =  (UInt_t) gQwOptions.GetIntValuePairFirst("run");
              run <= (UInt_t) gQwOptions.GetIntValuePairLast("run"); run++) {

    // Load the simulated event file
    TString filename = Form(TString(getenv("QWSCRATCH")) + "/data/QwSim_%d.root", run);
    QwTreeEventBuffer* treebuffer = new QwTreeEventBuffer (filename, detector_info);
    treebuffer->SetDebugLevel(1);

    // Open ROOT file
    TFile* file = 0;
    if (kHisto || kTree) {
      file = new TFile(Form(TString(getenv("QWSCRATCH")) + "/rootfiles/QwSim_%d.root", run),
                       "RECREATE",
                       "QWeak ROOT file with simulated event");
      file->cd();
    }

    // Loop over the events
    int fEntries = treebuffer->GetEntries();
    for (int fEvtNum  = gQwOptions.GetIntValuePairFirst("event");
             fEvtNum <= gQwOptions.GetIntValuePairLast("event") && fEvtNum < fEntries; fEvtNum++) {

      // Get hit list
      QwHitContainer* hitlist = treebuffer->GetHitList(fEvtNum);

      // Print hit list
      if (kDebug) {
        std::cout << "Printing hitlist ..." << std::endl;
        hitlist->Print();
      }

      // Process the hit list through the tracking worker
      QwEvent *event = trackingworker->ProcessHits(detectors, hitlist);

      // Do something with this event

      // Delete the event again
      delete hitlist;
      delete event;
    }

    std::cout << "Number of good partial tracks found: "
              << trackingworker->ngood << std::endl;

    // Print results
    //tree->Print();

    // Write and close file
    if (kTree || kHisto) {
      file->Write();
      file->Close();
      delete file;
    }

    // Delete objects
   delete treebuffer;

  } // end of loop over runs


  // Delete objects
  delete detectors;
  delete trackingworker;

  return 0;
}
