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
#include "QwDatabase.h"
#include "QwHistogramHelper.h"
#include "QwEPICSEvent.h"
#include "QwTrackingWorker.h"
#include "QwEvent.h"
#include "QwHitContainer.h"
#include "QwHitRootContainer.h"
#include "QwRunCondition.h"


// Qweak tracking subsystems
#include "QwSubsystemArrayTracking.h"
#include "QwGasElectronMultiplier.h"
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

// Qweak headers (deprecated)
#include "Qset.h"


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
  gQwOptions.AddConfigFile("qweak_mysql.conf");
  ///  Define the command line options
  DefineOptionsTracking(gQwOptions);
  /// Load command line options for the histogram/tree helper class
  //gQwHists.ProcessOptions(gQwOptions);
  /// Setup screen and file logging
  gQwLog.ProcessOptions(&gQwOptions);


  ///  Create the event buffer
  QwEventBuffer eventbuffer;
  eventbuffer.ProcessOptions(gQwOptions);

  ///  Set up the database connection
  QwDatabase database(gQwOptions);



  Bool_t enablemapfile = gQwOptions.GetValue<bool>("enable-mapfile");
  if(enablemapfile) {
    //std::cout << ">>>>>>>>>>> map file " << std::endl;
    gQwHists.LoadHistParamsFromFile("parity_hist.in");
    gQwHists.LoadHistParamsFromFile("qweak_tracking_hists.in");

  }
  else {
    //     std::cout << ">>>>>>>>>>> root file " << std::endl;
    ///  Load the histogram parameter definitions
    gQwHists.LoadHistParamsFromFile("parity_hist.in");
    gQwHists.LoadHistParamsFromFile("qweak_tracking_hists.in");
  }

  // Create and fill old detector structures (deprecated)
  Qset qset;
  qset.FillDetectors((getenv_safe_string("QWANALYSIS") + "/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();


  ///  Start loop over all runs
  QwRootFile* rootfile = 0;
  while (eventbuffer.OpenNextStream() == CODA_OK) {

    ///  Begin processing for the first run.


    ///  Set the current event number for parameter file lookup
    QwParameterFile::SetCurrentRunNumber(eventbuffer.GetRunNumber());


    ///  Create an EPICS event
    QwEPICSEvent epics;
    epics.LoadChannelMap("EpicsTable.map");


    ///  Load the tracking detectors from file
    QwSubsystemArrayTracking tracking_detectors(gQwOptions);
    tracking_detectors.ProcessOptions(gQwOptions);

    ///  Load the parity detectors from file
    QwSubsystemArrayParity parity_detectors(gQwOptions);
    parity_detectors.ProcessOptions(gQwOptions);

    ///  Create the tracking worker
    QwTrackingWorker *trackingworker = new QwTrackingWorker("qwtrackingworker");


    // Get vector with detector info (by region, plane number)
    std::vector< std::vector< QwDetectorInfo > > detector_info;
    tracking_detectors.GetSubsystemByName("R1")->GetDetectorInfo(detector_info);
    tracking_detectors.GetSubsystemByName("R2")->GetDetectorInfo(detector_info);
    tracking_detectors.GetSubsystemByName("R3")->GetDetectorInfo(detector_info);
    tracking_detectors.GetSubsystemByName("TS")->GetDetectorInfo(detector_info);
    tracking_detectors.GetSubsystemByName("MD")->GetDetectorInfo(detector_info);
    // TODO This is handled incorrectly, it just adds the three package after the
    // existing three packages from region 2...  GetDetectorInfo should descend
    // into the packages and add only the tracking_detectors in those packages.
    // Alternatively, we could implement this with a singly indexed vector (with
    // only an id as primary index) and write a couple of helper functions to
    // select the right subvectors of tracking_detectors.

    QwGeometry geometry;
    for (size_t i = 0; i < detector_info.size(); i++)
      geometry.push_back(detector_info.at(i));
    QwMessage << "Geometry:" << QwLog::endl;
    QwMessage << geometry << QwLog::endl;


    //  Initialize the database connection.
    database.SetupOneRun(eventbuffer);


    // Open the ROOT file
    rootfile = new QwRootFile(eventbuffer.GetRunLabel());
    if (! rootfile) QwError << "QwAnalysis made a boo boo!" << QwLog::endl;

    // Create dummy event for branch creation (memory leak when using null)
    QwEvent* event = new QwEvent();

    if (not enablemapfile) {
      // Create the tracking object branches
      rootfile->NewTree("event_tree", "QwTracking Event-based Tree");
      rootfile->GetTree("event_tree")->Branch("events", "QwEvent", &event);

      // Create the subsystem branches
      rootfile->ConstructTreeBranches("event_tree", "QwTracking Event-based Tree", tracking_detectors);
      rootfile->ConstructTreeBranches("Mps_Tree", "QwTracking Event-based Tree", parity_detectors);
      rootfile->ConstructTreeBranches("Slow_Tree", "EPICS and slow control tree", epics);
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
        epics.CalculateRunningValues();
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
      rootfile->FillTrees();

      // Delete objects
      if (hitlist) delete hitlist; hitlist = 0;
      if (event)   delete event;   event = 0;

      nevents++;

    } // end of loop over events



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

    // Delete histograms in the subsystems
    tracking_detectors.DeleteHistograms();
    parity_detectors.DeleteHistograms();

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

  QwMessage << QwLog::endl;
  QwMessage << "Number of tracking objects still alive:" << QwLog::endl;
  QwMessage << "  QwEvent: "        << QwEvent::GetObjectsAlive()
            << " (of " << QwEvent::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << "  QwEventHeader: "  << QwEventHeader::GetObjectsAlive()
            << " (of " << QwEventHeader::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << "  QwHit: "          << QwHit::GetObjectsAlive()
            << " (of " << QwHit::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << "  QwHitPattern: "   << QwHitPattern::GetObjectsAlive()
            << " (of " << QwHitPattern::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << "  QwTreeLine: "     << QwTrackingTreeLine::GetObjectsAlive()
            << " (of " << QwTrackingTreeLine::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << "  QwPartialTrack: " << QwPartialTrack::GetObjectsAlive()
            << " (of " << QwPartialTrack::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << "  QwTrack: "        << QwTrack::GetObjectsAlive()
            << " (of " << QwTrack::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << "  QwBridge: "        << QwBridge::GetObjectsAlive()
            << " (of " << QwBridge::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << QwLog::endl;

  QwMessage << "I have done everything I can do..." << QwLog::endl;

  return 0;
}
