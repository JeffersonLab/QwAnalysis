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
#include "QwParameterFile.h"

// Qweak event buffer and tracking worker
#include "QwTreeEventBuffer.h"
#include "QwHitRootContainer.h"
#include "QwTrackingWorker.h"
#include "QwPartialTrack.h"
#include "QwEvent.h"

// Qweak tracking subsystem headers
#include "QwSubsystemArrayTracking.h"
//#include "QwGasElectronMultiplier.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwTriggerScintillator.h"
#include "QwMainDetector.h"


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

  ///  Load the tracking detectors from file
  QwSubsystemArrayTracking* detectors = new QwSubsystemArrayTracking(gQwOptions);
  detectors->ProcessOptions(gQwOptions);

  // Get detector geometry
  QwGeometry geometry = detectors->GetGeometry();

  /// Create a timer
  TStopwatch timer;

  /// Start timer
  timer.Reset();
  timer.Start();

  /// Next, we create the tracking worker that will pull coordinate the tracking.
  QwTrackingWorker *trackingworker = new QwTrackingWorker(geometry);
  if (kDebug) trackingworker->SetDebugLevel(1);

  /// Stop timer
  timer.Stop();
  // Print timer info
  PrintInfo(timer);

  /// Create the event buffer
  QwTreeEventBuffer* treebuffer = new QwTreeEventBuffer(geometry);
  treebuffer->ProcessOptions(gQwOptions);
  treebuffer->SetEntriesPerEvent(1);

  ///  Start loop over all runs
  while (treebuffer->OpenNextFile() == 0) {

    // Create dummy event for branch creation (memory leak when using null)
    QwEvent* event = new QwEvent();

    // Open ROOT file
    TFile* file = 0;
    TTree* hit_tree = 0;
    TTree* event_tree = 0;
    QwHitRootContainer* roothitlist = new QwHitRootContainer();
    if (kHisto || kTree) {
      file = new TFile(Form(getenv_safe_TString("QW_ROOTFILES") + "/QwSim_%d.root",
                       treebuffer->GetRunNumber()), "RECREATE",
                       "QWeak ROOT file with simulated event");
      file->cd();
    }
    if (kTree) {
      hit_tree = new TTree("hit_tree", "QwTracking Hit-based Tree");
      hit_tree->Branch("hits", "QwHitRootContainer", &roothitlist);
      event_tree = new TTree("event_tree", "QwTracking Event-based Tree");
      event_tree->Branch("events", "QwEvent", &event);
    }

    // Delete dummy event again
    delete event; event = 0;

    /// Start timer
    timer.Reset();
    timer.Start();

    /// We loop over all requested events.
    Int_t nevents = 0;
    while (treebuffer->GetNextEvent() == 0) {

      /// Create a new event structure
      event = new QwEvent();

      // Create the event header with the run and event number
      QwEventHeader* header =
          new QwEventHeader(treebuffer->GetRunNumber(),treebuffer->GetEventNumber());

      // Assign the event header
      event->SetEventHeader(header);


      /// Read the hit list from the event buffer
      QwHitContainer* hitlist = treebuffer->GetHitContainer();
      roothitlist->Convert(hitlist);

      // and fill into the event
      event->AddHitContainer(hitlist);


      /// We process the hit list through the tracking worker and get a new
      /// QwEvent object back.
      trackingworker->ProcessEvent(detectors, event);


      // Fill the tree
      if (kTree) {
        hit_tree->Fill();
        event_tree->Fill();
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
