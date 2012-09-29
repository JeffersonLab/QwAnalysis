/*------------------------------------------------------------------------*//*!

 \file QwSimTracking.cc
 \ingroup QwTracking

 \brief Example of the QwTreeEventBuffer class to read QweakSimG4 events

 This example illustrates the use of the QwTreeEventBuffer class.  It loads
 the file Tracking/prminput/QweakSim.root, which was produced with a 3-fold
 trigger in QweakSimG4 (hits in all of HDC, VDC-front, and VDC-back).  For
 every event a QwHitContainer is filled, which is then printed to QwMessage.

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
  QwMessage << "CPU time used:  "  << timer.CpuTime() << " s"
	    << QwLog::endl
	    << "Real time used: " << timer.RealTime() << " s"
	    << QwLog::endl << QwLog::endl;
  return;
}


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
  QwTrackingWorker *trackingworker = new QwTrackingWorker(gQwOptions, geometry);

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

    // Create dummy events for branch creation (memory leak when using null)
    QwEvent* event = new QwEvent();
    QwEvent* original = new QwEvent();

    // Open ROOT file
    TFile* file = 0;
    TTree* event_tree = 0;
    file = new TFile(Form(getenv_safe_TString("QW_ROOTFILES") + "/QwSim_%d.root",
                     treebuffer->GetRunNumber()), "RECREATE",
                     "QWeak ROOT file with simulated event");
    file->cd();
    event_tree = new TTree("event_tree", "QwTracking Event-based Tree");
    event_tree->Branch("events", "QwEvent", &event);
    event_tree->Branch("originals", "QwEvent", &original);

    // Delete dummy events again
    delete event; event = 0;
    delete original; original = 0;

    /// Start timer
    timer.Reset();
    timer.Start();

    /// We loop over all requested events.
    Int_t nevents = 0;
    while (treebuffer->GetNextEvent() == 0) {

      /// Create the event header with the run and event number
      QwEventHeader header(treebuffer->GetRunNumber(),treebuffer->GetEventNumber());


      /// Create the original event
      /// \todo Original event is created in QwTreeEventBuffer already: should just use that
      original = new QwEvent();
      original->SetEventHeader(header);
      /// Add hits without resolution smearing
      QwHitContainer* exacthitlist = treebuffer->CreateHitList(false);
      original->AddHitContainer(exacthitlist);
      delete exacthitlist;
      /// Add exact treelines and tracks
      original->AddTreeLineList(treebuffer->GetTreeLines(kRegionID2));
      original->AddTreeLineList(treebuffer->GetTreeLines(kRegionID3));
      original->AddPartialTrackList(treebuffer->GetPartialTracks(kRegionID2));
      original->AddPartialTrackList(treebuffer->GetPartialTracks(kRegionID3));


      /// Create the to-be-reconstructed event
      event = new QwEvent();
      event->SetEventHeader(header);

      /// Add hits with resolution smearing
      QwHitContainer* smearedhitlist = treebuffer->CreateHitList(true);
      event->AddHitContainer(smearedhitlist);
      delete smearedhitlist;


      /// We process the hit list through the tracking worker
      trackingworker->ProcessEvent(detectors, event);


      // Fill the tree
      event_tree->Fill();


      // Event has been processed
      nevents++;

      // Delete the hit lists and reconstructed event
      delete original;
      delete event;

    } // end of loop over events

    QwMessage << "Number of events processed at end of run: "
              << treebuffer->GetEventNumber() << QwLog::endl;


    /// Stop timer
    timer.Stop();
    // Print timer info
    PrintInfo(timer);

    QwMessage << "Number of good partial tracks found: "
              << trackingworker->ngood << QwLog::endl;
    QwMessage << "Region 2: "
              << trackingworker->R2Good << QwLog::endl;
    QwMessage << "Region 3: "
              << trackingworker->R3Good << QwLog::endl;

    // Write and close file
    file->Write();
    file->Close();
    delete file;

    // Close input file
    treebuffer->CloseFile();

  } // end of loop over runs


  // Delete objects
  delete treebuffer;
  delete detectors;
  delete trackingworker;

  return 0;
}
