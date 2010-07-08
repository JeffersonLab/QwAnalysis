/*------------------------------------------------------------------------*//*!

 \file QwSimTracking.cc
 \ingroup QwTracking

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

// Qweak headers
#include "QwOptionsTracking.h"
#include "QwLog.h"

// Deprecated Qweak headers
#include "Det.h"
#include "Qset.h"

// Qweak event buffer and tracking worker
#include "QwTreeEventBuffer.h"
#include "QwHitRootContainer.h"
#include "QwTrackingWorker.h"
#include "QwPartialTrack.h"
#include "QwEvent.h"

// Qweak tracking subsystem headers
#include "QwSubsystemArrayTracking.h"
#include "QwGasElectronMultiplier.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwTriggerScintillator.h"
#include "QwMainDetector.h"

// Qweak subsystem factory
#include "QwSubsystemFactory.h"


#include <TStopwatch.h>
void PrintInfo(TStopwatch& timer)
{
  std::cout << "CPU time used:  "  << timer.CpuTime() << " s"
	    << std::endl
	    << "Real time used: " << timer.RealTime() << " s"
	    << std::endl << std::endl;
  return;
}


// Debug level
static const bool kDebug = false;
// ROOT file output
static const bool kTree = true;
static const bool kHisto = true;

int main (int argc, char* argv[])
{
  /// First, we fill the search paths for the parameter files.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Tracking/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");

  /// We set the command line arguments and the configuration filename,
  /// and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  // Define the command line options
  DefineOptionsTracking(gQwOptions);

  ///  Setup screen and file logging
  gQwLog.ProcessOptions(&gQwOptions);

  /// For the tracking analysis we create the QwSubsystemArrayTracking list
  /// which contains the VQwSubsystemTracking objects.
  QwSubsystemArrayTracking* detectors = new QwSubsystemArrayTracking(gQwOptions);

  // Get vector with detector info (by region, plane number)
  std::vector< std::vector< QwDetectorInfo > > detector_info;
  detectors->GetSubsystemByName("R1")->GetDetectorInfo(detector_info);
  detectors->GetSubsystemByName("R2")->GetDetectorInfo(detector_info);
  detectors->GetSubsystemByName("R3")->GetDetectorInfo(detector_info);
  detectors->GetSubsystemByName("TS")->GetDetectorInfo(detector_info);
  detectors->GetSubsystemByName("MD")->GetDetectorInfo(detector_info);
  // TODO This is handled incorrectly, it just adds the three package after the
  // existing three packages from region 2...  GetDetectorInfo should descend
  // into the packages and add only the detectors in those packages.
  // Alternatively, we could implement this with a singly indexed vector (with
  // only an id as primary index) and write a couple of helper functions to
  // select the right subvectors of detectors.

  // Load the geometry
  Qset qset;
  qset.FillDetectors((getenv_safe_string("QWANALYSIS")+"/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();
  std::cout << "[QwTracking::main] Geometry loaded" << std::endl; // R3,R2

  /// Create a timer
  TStopwatch timer;

  /// Start timer
  timer.Reset();
  timer.Start();

  /// Next, we create the tracking worker that will pull coordinate the tracking.
  QwTrackingWorker *trackingworker = new QwTrackingWorker("qwtrackingworker");
  if (kDebug) trackingworker->SetDebugLevel(1);

  /// Stop timer
  timer.Stop();
  // Print timer info
  PrintInfo(timer);

  /// Create the event buffer
  QwTreeEventBuffer* treebuffer = new QwTreeEventBuffer(detector_info);
  treebuffer->ProcessOptions(gQwOptions);
  treebuffer->SetEntriesPerEvent(1);

  ///  Start loop over all runs
  while (treebuffer->OpenNextFile() == 0) {

    // Open ROOT file
    TFile* file = 0;
    TTree* tree_hits = 0;
    TTree* tree_events = 0;
    QwEvent* event = new QwEvent();
    QwHitRootContainer* roothitlist = new QwHitRootContainer();
    if (kHisto || kTree) {
      file = new TFile(Form(getenv_safe_TString("QWSCRATCH") + "/rootfiles/QwSim_%d.root", treebuffer->GetRunNumber()),
                       "RECREATE",
                       "QWeak ROOT file with simulated event");
      file->cd();
    }
    if (kTree) {
      tree_hits = new TTree("hit_tree", "QwTracking Hit-based Tree");
      tree_hits->Branch("hits", "QwHitRootContainer", &roothitlist);
      tree_events = new TTree("event_tree", "QwTracking Event-based Tree");
      tree_events->Branch("events", "QwEvent", &event);
    }

    /// Start timer
    timer.Reset();
    timer.Start();

    /// We loop over all requested events.
    Int_t nevents = 0;
    while (treebuffer->GetNextEvent() == 0) {

      /// Read the hit list from the event buffer
      QwHitContainer* hitlist = treebuffer->GetHitContainer();
      roothitlist->Convert(hitlist);

      // Print hit list
      if (kDebug) {
        std::cout << "Printing hitlist..." << std::endl;
        hitlist->Print();
      }

      /// We process the hit list through the tracking worker and get a new
      /// QwEvent object back.
      event = trackingworker->ProcessHits(detectors, hitlist);


      // Do something with this event
      event->GetEventHeader()->SetRunNumber(treebuffer->GetRunNumber());
      event->GetEventHeader()->SetEventNumber(treebuffer->GetEventNumber());
      if (kDebug) event->Print();


      // Fill the tree
      if (kTree) {
        tree_hits->Fill();
        tree_events->Fill();
      }


      // Event has been processed
      nevents++;

      // Delete the hit lists and reconstructed event
      delete hitlist;
      delete event;

    } // end of loop over events

    QwMessage << "Number of events processed at end of run: "
              << treebuffer->GetEventNumber() << std::endl;


    /// Stop timer
    timer.Stop();
    // Print timer info
    PrintInfo(timer);

    // Delete the ROOT hit list
    delete roothitlist;

    std::cout << "Number of good partial tracks found: "
              << trackingworker->ngood << std::endl;
    std::cout << "Region 2: "
              << trackingworker->R2Good << std::endl;
    std::cout << "Region 3: "
              << trackingworker->R3Good << std::endl;

    // Print results
    //if (kDebug) tree->Print();

    // Write and close file
    if (kTree || kHisto) {
      file->Write();
      file->Close();
      delete file;
    }

    // Close input file
    treebuffer->CloseFile();

  } // end of loop over runs


  // Delete objects
  delete treebuffer;
  delete detectors;
  delete trackingworker;

  return 0;
}
