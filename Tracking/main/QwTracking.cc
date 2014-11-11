/*------------------------------------------------------------------------*//*!

 \file QwTracking.cc
 \ingroup QwTracking

 \brief This is the main executable for the tracking analysis.

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <sys/time.h>

// Qweak headers
#include "QwLog.h"
#include "QwRootFile.h"
#include "QwOptionsTracking.h"
#include "QwEventBuffer.h"
#include "QwHistogramHelper.h"
#include "QwEPICSEvent.h"
#include "QwTrackingWorker.h"
#include "QwEvent.h"
#include "QwHitContainer.h"
#include "QwHitRootContainer.h"
#include "QwRunCondition.h"


// Qweak tracking subsystems
#include "QwSubsystemArrayTracking.h"
#include "QwSciFiDetector.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwTriggerScintillator.h"
#include "QwMainDetector.h"
#include "QwScanner.h"
#include "QwRaster.h"


// Qweak parity subsystems
#include "QwSubsystemArrayParity.h"
#include "QwHelicity.h"
#include "QwBeamLine.h"
#include "QwScaler.h"


// Main function
Int_t main(Int_t argc, Char_t* argv[])
{
  /// without anything, print usage
  if(argc == 1){
    gQwOptions.Usage();
    exit(0);
  }
  ///  First, fill the search paths for the parameter files; this sets a static
  ///  variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Tracking/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");

  ///  Then, we set the command line arguments and the configuration filename,
  ///  and we define the options that can be used in them (using QwOptions).

  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.ListConfigFiles();

  ///  TODO:  I have disabled the AddConfigFile("qweak_mysql.conf")
  ///         line below, because the standard version contains
  ///         options which are not defined in the basic QwDatabase
  ///         DefineOptions function.  We should figure out something
  ///         better...
  ///  gQwOptions.AddConfigFile("qweak_mysql.conf");
  ///  Define the command line options
  DefineOptionsTracking(gQwOptions);
  /// Load command line options for the histogram/tree helper class
  //gQwHists.ProcessOptions(gQwOptions);
  /// Setup screen and file logging
  gQwLog.ProcessOptions(&gQwOptions);


  ///  Create the event buffer
  QwEventBuffer eventbuffer;
  eventbuffer.ProcessOptions(gQwOptions);



  Bool_t enablemapfile = gQwOptions.GetValue<bool>("enable-mapfile");
  gQwHists.LoadHistParamsFromFile("parity_hist.in");
  gQwHists.LoadHistParamsFromFile("qweak_tracking_hists.in");


  ///  Start loop over all runs
  while (eventbuffer.OpenNextStream() == CODA_OK) {

    ///  Begin processing for the first run.


    ///  Set the current event number for parameter file lookup
    QwParameterFile::SetCurrentRunNumber(eventbuffer.GetRunNumber());

    ///  Clear options to pick up run number specific config files
    gQwOptions.Parse(true);

    ///  Create an EPICS event
    QwEPICSEvent epics;
    epics.LoadChannelMap("EpicsTable.map");


    ///  Load the tracking detectors from file
    QwSubsystemArrayTracking tracking_detectors(gQwOptions);
    tracking_detectors.ProcessOptions(gQwOptions);
    ///  Get detector geometry
    QwGeometry geometry = tracking_detectors.GetGeometry();
    QwVerbose << geometry << QwLog::endl;

    ///  Load the parity detectors from file
    QwSubsystemArrayParity parity_detectors(gQwOptions);
    parity_detectors.ProcessOptions(gQwOptions);

    ///  Create the tracking worker
    QwTrackingWorker *trackingworker = new QwTrackingWorker(gQwOptions, geometry);


    //  Loop until first EPICS event if no magnetic field is set
    bool current_from_epics = false;
    if (fabs(trackingworker->GetMagneticFieldCurrent()) < 100.0) {

      QwMessage << "Finding first EPICS event" << QwLog::endl;
      while (eventbuffer.GetNextEvent() == CODA_OK) {

        //  First, do quick processing of non-physics events...
        if (eventbuffer.IsEPICSEvent()) {
          eventbuffer.FillEPICSData(epics);
          if (epics.HasDataLoaded()) {

            // Get magnetic field current
            Double_t current = epics.GetDataValue("qw:qt_mps_i_set");
            if (current > 0.0) {
              QwMessage << "Setting magnetic field current to "
                        << current << "A " << QwLog::endl;
              trackingworker->SetMagneticFieldCurrent(current);
              current_from_epics = true;
            }
            // and break out of this event loop
            break;
          }
        }
      }

      //  Rewind stream
      QwMessage << "Rewinding stream" << QwLog::endl;
      eventbuffer.ReOpenStream();

      //  Check whether we found the magnetic field
      if (fabs(trackingworker->GetMagneticFieldCurrent()) < 100.0) {
        QwError << "Error: no magnetic field specified and no EPICS events in range!"
                << QwLog::endl;
      }
    }


    // Open the ROOT file
    QwRootFile* rootfile = new QwRootFile(eventbuffer.GetRunLabel());
    if (! rootfile) QwError << "QwAnalysis made a boo boo!" << QwLog::endl;

    //
    //  Construct a Tree which contains map file names which are used to analyze data
    //
    rootfile->WriteParamFileList("mapfiles", tracking_detectors);

    // Create dummy event for branch creation (memory leak when using null)
    QwEvent* event = new QwEvent();
    event->LoadBeamProperty("beam_property.map");

    if (not enablemapfile) {
      // Create the tracking object branches
      rootfile->NewTree("event_tree", "QwTracking Event-based Tree");
      rootfile->GetTree("event_tree")->Branch("events", "QwEvent", &event);

      // Create the subsystem branches
      rootfile->ConstructTreeBranches("event_tree", "QwTracking Event-based Tree", tracking_detectors);
      rootfile->ConstructTreeBranches("Mps_Tree", "QwParity Helicity-based Tree", parity_detectors);
      rootfile->ConstructTreeBranches("Slow_Tree", "EPICS and slow control tree", epics);
      // Construct indices to get from one tree to the other
      rootfile->ConstructIndices("event_tree","Slow_Tree");
      rootfile->ConstructIndices("event_tree","Mps_Tree");
    }

    // Delete dummy event again
    delete event; event = 0;

    // Create the subsystem histograms
    rootfile->ConstructHistograms("tracking_histo", tracking_detectors);
    rootfile->ConstructHistograms("parity_histo",   parity_detectors);

    // Summarize the ROOT file structure
    rootfile->PrintTrees();
    rootfile->PrintDirs();


    //  Loop over events in this CODA file
    Int_t nevents           = 0;
    while (eventbuffer.GetNextEvent() == CODA_OK) {

      //  First, do processing of non-physics events...
      if (eventbuffer.IsEPICSEvent()) {
        eventbuffer.FillEPICSData(epics);
        if (epics.HasDataLoaded()) {

          // Get magnetic field current
          Double_t current = epics.GetDataValue("qw:qt_mps_i_set");
          if (current_from_epics && current > 0.0) {
            QwMessage << "Setting magnetic field current to "
                << current << "A " << QwLog::endl;
            trackingworker->SetMagneticFieldCurrent(current);
          }

          epics.CalculateRunningValues();

          rootfile->FillTreeBranches(epics);
          rootfile->FillTree("Slow_Tree");
        }
      }

      //  Send ROC configuration event data to the subsystem objects.
      if (eventbuffer.IsROCConfigurationEvent()) {
         eventbuffer.FillSubsystemConfigurationData(tracking_detectors);
         eventbuffer.FillSubsystemConfigurationData(parity_detectors);
      }

      //  Now, if this is not a physics event, go back and get a new event.
      if (! eventbuffer.IsPhysicsEvent()) {
         continue;
      }

      // Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(tracking_detectors);
      eventbuffer.FillSubsystemData(parity_detectors);

      // Process the event
      tracking_detectors.ProcessEvent();
      parity_detectors.ProcessEvent();

      // Fill the tree
      rootfile->FillTreeBranches(tracking_detectors);
      rootfile->FillTreeBranches(parity_detectors);

      // Fill the histograms for the subsystem objects.
      rootfile->FillHistograms(tracking_detectors);
      rootfile->FillHistograms(parity_detectors);


      /// Create a new event structure
      event = new QwEvent();

      // Create the event header with the run and event number
      QwEventHeader header(eventbuffer.GetRunNumber(),eventbuffer.GetEventNumber());
      // Assign the event header to the event
      event->SetEventHeader(header);

      // Create and fill hit list
      QwHitContainer* hitlist = new QwHitContainer();
      tracking_detectors.GetHitList(hitlist);

      // Sorting the hit list
      hitlist->sort();

      // and fill into the event
      event->AddHitContainer(hitlist);

      // Track reconstruction
      trackingworker->ProcessEvent(&tracking_detectors, event);


      // Fill the event tree
      rootfile->FillTree("event_tree");
      rootfile->FillTree("Mps_Tree");

      // Delete objects
      if (hitlist) delete hitlist; hitlist = 0;
      if (event)   delete event;   event = 0;

      nevents++;

    } // end of loop over events

    // Write F1TDC errors histograms into rootfile
    // Print F1TDC errors summary into screen
    // Must be executed before delete a rootfile.

    tracking_detectors.FillHardwareErrorSummary();

    // Print summary information
    QwMessage << "Total number of events processed: " << nevents << QwLog::endl;
    QwMessage << "Number of good partial tracks: "
              << trackingworker->ngood << QwLog::endl;

    /*  Write to the root file, being sure to delete the old cycles *
     *  which were written by Autosave.                             *
     *  Doing this will remove the multiple copies of the ntuples   *
     *  from the root file.                                         */
    //   if (rootfile != NULL) rootfile->Write(0, TObject::kOverwrite);

    // Write and close file (after last access to ROOT tree)
    rootfile->Write(0, TObject::kOverwrite);

    // Close CODA file
    eventbuffer.CloseStream();

    // Close ROOT file
    rootfile->Close();
    // Note: Closing rootfile too early causes segfaults when deleting histos

    // Delete objects (this is confusing: the if only applies to the delete)
    if (rootfile)       delete rootfile;       rootfile = 0;

    // Print run summary information
    eventbuffer.ReportRunSummary();
    eventbuffer.PrintRunTimes();

    // Delete objects
    if (trackingworker) delete trackingworker; trackingworker = 0;

  } // end of loop over runs

  QwMessage << "I have done everything I can do..." << QwLog::endl;

  return 0;
}
