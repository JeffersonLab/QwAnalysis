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
#include "QwRootFile.h"
#include "QwParameterFile.h"

// Qweak event buffer and tracking worker
#include "QwTreeEventBuffer.h"
#include "QwHitRootContainer.h"
#include "QwTrackingWorker.h"
#include "QwPartialTrack.h"
#include "QwEvent.h"

// Qweak tracking subsystem headers
#include "QwSubsystemArrayTracking.h"
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
  // Set default ROOT file stem
  QwRootFile::SetDefaultRootFileStem("QwSim_");

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
  QwSubsystemArrayTracking detectors(gQwOptions);
  detectors.ProcessOptions(gQwOptions);

  // Get detector geometry
  QwGeometry geometry = detectors.GetGeometry();

  /// Create a timer
  TStopwatch timer;

  /// Start timer
  timer.Reset();
  timer.Start();

  /// Next, we create the tracking worker that will pull coordinate the tracking.
  QwTrackingWorker trackingworker(gQwOptions, geometry);

  /// Stop timer
  timer.Stop();
  // Print timer info
  PrintInfo(timer);

  /// Create the event buffer
  QwTreeEventBuffer treebuffer(geometry);
  treebuffer.ProcessOptions(gQwOptions);
  treebuffer.SetEntriesPerEvent(1);

  ///  Start loop over all runs
  while (treebuffer.OpenNextFile() == 0) {

    // Open the ROOT file
    QwRootFile* rootfile = new QwRootFile(treebuffer.GetRunLabel());
    if (! rootfile) QwError << "QwAnalysis made a boo boo!" << QwLog::endl;

    //  Construct a Tree which contains map file names which are used to analyze data
    rootfile->WriteParamFileList("mapfiles", detectors);

    // Create dummy events for branch creation (memory leak when using null)
    QwEvent* event = new QwEvent();
    QwEvent* original = new QwEvent();

    // Create the tracking object branches
    rootfile->NewTree("event_tree", "QwTracking Event-based Tree");
    rootfile->GetTree("event_tree")->Branch("events", "QwEvent", &event);
    rootfile->GetTree("event_tree")->Branch("originals", "QwEvent", &original);

    // Delete dummy events again
    delete event; event = 0;
    delete original; original = 0;

    /// Start timer
    timer.Reset();
    timer.Start();

    /// We loop over all requested events.
    Int_t nevents = 0;
    while (treebuffer.GetNextEvent() == 0) {


      /// Create the original event
      original = treebuffer.GetOriginalEvent();

      /// Create the to-be-reconstructed event
      event = treebuffer.GetCurrentEvent();

      /// We process the hit list through the tracking worker
      trackingworker.ProcessEvent(&detectors, event);

      // Fill the tree
      rootfile->FillTree("event_tree");

      // Event has been processed
      nevents++;

    } // end of loop over events

    /// Stop timer
    timer.Stop();
    // Print timer info
    PrintInfo(timer);

    QwMessage << "\nNumber of events processed at end of run: "
              << treebuffer.GetEventNumber() << QwLog::endl;

    treebuffer.PrintStatInfo(trackingworker.R2Good, trackingworker.R3Good, trackingworker.nbridged);

    // Write and close file (after last access to ROOT tree)
    rootfile->Write(0, TObject::kOverwrite);

    // Close ROOT file
    rootfile->Close();

    // Delete objects (this is confusing: the if only applies to the delete)
    if (rootfile) delete rootfile; rootfile = 0;

    // Close input file
    treebuffer.CloseFile();

  } // end of loop over runs

  return 0;
}
