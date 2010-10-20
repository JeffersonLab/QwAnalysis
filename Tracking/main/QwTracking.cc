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
#include "Det.h"
#include "Qset.h"


// Debug level
static const bool kDebug = false;
// ROOT file output
static const bool kEPICS = kFALSE;

// Main function
Int_t main(Int_t argc, Char_t* argv[])
{
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
  gQwOptions.AddConfigFile("qwtracking.conf");
  gQwOptions.AddConfigFile("qweak_mysql.conf");
  ///  Define the command line options
  DefineOptionsTracking(gQwOptions);

  ///  Setup screen and file logging
  gQwLog.ProcessOptions(&gQwOptions);

  // Either the DISPLAY is not set or JOB_ID is defined: we take it as in batch mode.
  Bool_t kInQwBatchMode = kFALSE;

  if (getenv("DISPLAY") == NULL
   || getenv("JOB_ID")  != NULL) {
     kInQwBatchMode = kTRUE;
     gROOT->SetBatch(kTRUE);
  }

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

  ///  Create the event buffer
  QwEventBuffer eventbuffer;
  eventbuffer.ProcessOptions(gQwOptions);

  ///  Create an EPICS event
  QwEPICSEvent epics;
  if (kEPICS)
    epics.LoadEpicsVariableMap("EpicsTable.map");


  ///  Load the tracking detectors from file
  QwSubsystemArrayTracking tracking_detectors(gQwOptions);
  tracking_detectors.ProcessOptions(gQwOptions);

  ///  Load the parity detectors from file
  QwSubsystemArrayParity parity_detectors(gQwOptions);
  parity_detectors.ProcessOptions(gQwOptions);

  // Get vector with detector info (by region, plane number)
  //std::vector< std::vector< QwDetectorInfo > > detector_info;
  //tracking_detectors.GetSubsystemByName("R1")->GetDetectorInfo(detector_info);
  //tracking_detectors.GetSubsystemByName("R2")->GetDetectorInfo(detector_info);
  //tracking_detectors.GetSubsystemByName("R3")->GetDetectorInfo(detector_info);
  // TODO This is handled incorrectly, it just adds the three package after the
  // existing three packages from region 2...  GetDetectorInfo should descend
  // into the packages and add only the tracking_detectors in those packages.
  // Alternatively, we could implement this with a singly indexed vector (with
  // only an id as primary index) and write a couple of helper functions to
  // select the right subvectors of tracking_detectors.


  // Create and fill old detector structures (deprecated)
  Qset qset;
  qset.FillDetectors((getenv_safe_string("QWANALYSIS") + "/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();


  // Create the tracking worker
  QwTrackingWorker *trackingworker = new QwTrackingWorker("qwtrackingworker");

  ///  Set up the database connection
  QwDatabase database(gQwOptions);

  ///  Start loop over all runs
  QwRootFile* rootfile = 0;
  while (eventbuffer.OpenNextStream() == CODA_OK) {

    //  Begin processing for the first run.

    //  Open the ROOT file
    rootfile = new QwRootFile(eventbuffer.GetRunLabel());
    if (! rootfile) QwError << "QwAnalysis made a boo boo!" << QwLog::endl;
    QwEvent* event = 0;
    
    if(not enablemapfile) {
    // Create the tracking object branches
   
    rootfile->NewTree("event_tree", "QwTracking Event-based Tree");
    rootfile->GetTree("event_tree")->Branch("events", "QwEvent", &event);

    // Create the subsystem branches
    rootfile->ConstructTreeBranches("event_tree", "QwTracking Event-based Tree", tracking_detectors);
    rootfile->ConstructTreeBranches("event_tree", "QwTracking Event-based Tree", parity_detectors);
  }
    // Create the subsystem histograms
    rootfile->ConstructHistograms("tracking_histo", tracking_detectors);
    rootfile->ConstructHistograms("parity_histo",   parity_detectors);

    // Summarize the ROOT file structure
    rootfile->PrintTrees();
    rootfile->PrintDirs();


    QwHitContainer* hitlist = 0;
    Int_t nevents           = 0;
    while (eventbuffer.GetNextEvent() == CODA_OK) {
      //  Loop over events in this CODA file
      //  First, do processing of non-physics events...

      if (kEPICS) {
        if (eventbuffer.IsEPICSEvent()) {
          eventbuffer.FillEPICSData(epics);
          epics.CalculateRunningValues();
        }
      }


      if (eventbuffer.IsROCConfigurationEvent()) {
         //  Send ROC configuration event data to the subsystem objects.
         eventbuffer.FillSubsystemConfigurationData(tracking_detectors);
         eventbuffer.FillSubsystemConfigurationData(parity_detectors);
      }

      //  Now, if this is not a physics event, go back and get
      //  a new event.
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



      // Create the event header with the run and event number
      QwEventHeader* header = new QwEventHeader(
        eventbuffer.GetRunNumber(),
        eventbuffer.GetEventNumber());

      // Create and fill hit list
      hitlist = new QwHitContainer();

      tracking_detectors.GetHitList(hitlist);

      // Sorting the grand hit list
      hitlist->sort();

      // Print hit list
      if (hitlist->size() > 0 && kDebug) {
        std::cout << "Event " << eventbuffer.GetEventNumber() << std::endl;
        hitlist->Print();
      }

      // Track reconstruction
      if (hitlist->size() > 0) {
        // Process the hits
        event = trackingworker->ProcessHits(&tracking_detectors, hitlist);

        // Assign the event header
        event->SetEventHeader(header);

        // Print the reconstructed event
        if (kDebug) event->Print();
      }

      // Save the hitlist to the tree
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

    if (kEPICS) {
      epics.ReportEPICSData();
      epics.PrintVariableList();
      epics.PrintAverages();
      //TString tag; epics.GetDataValue(tag);
    }

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
    if (hitlist)        delete hitlist;        hitlist = 0;
    if (event)          delete event;          event = 0;

    // Print run summary information
    eventbuffer.ReportRunSummary();
    eventbuffer.PrintRunTimes();

  } // end of loop over runs

  // Delete objects
  if (trackingworker) delete trackingworker; trackingworker = 0;

  QwMessage << "I have done everything I can do..." << QwLog::endl;

  return 0;
}
