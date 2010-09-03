/*------------------------------------------------------------------------*//*!

 \file QwTracking.cc
 \ingroup QwTracking

 \brief This is the main executable for the tracking analysis.

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <sys/time.h>

// ROOT headers
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

// Qweak headers
#include "QwLog.h"
#include "QwOptionsTracking.h"
#include "QwEventBuffer.h"
#include "QwHistogramHelper.h"
#include "QwEPICSEvent.h"
#include "QwTrackingWorker.h"
#include "QwEvent.h"
#include "QwHitContainer.h"
#include "QwHitRootContainer.h"

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

// Qweak headers (deprecated)
#include "Det.h"
#include "Qset.h"



// Debug level
static const bool kDebug = kFALSE;
// ROOT file output
static const bool kTree = kTRUE;
static const bool kHisto = kFALSE;
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

  ///  Load the histogram parameter definitions
  gQwHists.LoadHistParamsFromFile("qweak_parity_hists.in");
  gQwHists.LoadHistParamsFromFile("qweak_tracking_hists.in");

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


  QwTrackingWorker *trackingworker = 0;
  // Create the tracking worker
  trackingworker = new QwTrackingWorker("qwtrackingworker");
  if (kDebug) trackingworker->SetDebugLevel(1);


  ///  Set up the database connection
  QwDatabase database(gQwOptions);

  ///  Start loop over all runs
  while (eventbuffer.OpenNextStream() == CODA_OK) {

    //  Begin processing for the first run.

    //  Open the ROOT file
    TFile *rootfile = 0;
    if (rootfile) {
       if (rootfile-> IsOpen()) rootfile->Close();
       delete rootfile;
       rootfile=0;
    }

    rootfile = new TFile(Form(getenv_safe_TString("QW_ROOTFILES") + "/Qweak_%s.root",
                        eventbuffer.GetRunLabel().Data()), "RECREATE",
                        "QWeak ROOT file with real events");
    //   std::auto_ptr<TFile> rootfile (new TFile(Form(getenv_safe_TString("QWSCRATCH") + "/rootfiles/Qweak_%d.root", run),
    //   					"RECREATE",
    //   					"QWeak ROOT file with real events"));

    //  Create the histograms for the QwDriftChamber subsystem object.
    //  We can create a subfolder in the rootfile first, if we want,
    //  and then pass it into the constructor.
    //
    //  To pass a subdirectory named "subdir", we would do:
    //   tracking_detectors.GetSubsystemByName("MD")->ConstructHistograms(rootfile->mkdir("subdir"));

    // Construct histograms
    //tracking_detectors.ConstructHistograms(rootfile->mkdir("histos"));

    // Open file

    TTree* hit_tree = 0;
    TTree* event_tree = 0;
    QwEvent* event = 0;
    QwHitRootContainer* hitlist_root = new QwHitRootContainer();
    std::vector<double> tracking_vector, parity_vector;


    if (kTree) {
       rootfile->cd(); // back to the top directory
       hit_tree = new TTree("hit_tree", "QwTracking Hit-based Tree");
       hit_tree->Branch("hits", "QwHitRootContainer", &hitlist_root);
       event_tree = new TTree("event_tree", "QwTracking Event-based Tree");
       event_tree->Branch("hits", "QwHitRootContainer", &hitlist_root);
       event_tree->Branch("events", "QwEvent", &event);

       // Create the branches and tree vector
       TString prefix = "";
       tracking_vector.reserve(6000);
       tracking_detectors.ConstructBranchAndVector(event_tree, prefix, tracking_vector);
       parity_vector.reserve(6000);
       parity_detectors.ConstructBranchAndVector(event_tree, prefix, parity_vector);
    }

    if (kHisto) {
       rootfile->cd();
       tracking_detectors.ConstructHistograms(rootfile->mkdir("tracking_histo"));
       rootfile->cd();
       parity_detectors.ConstructHistograms(rootfile->mkdir("parity_histo"));
       rootfile->cd();
    }

    QwHitContainer* hitlist = 0;

    Int_t nevents           = 0;
    Int_t hit_fill_retval   = 0;
    Int_t event_fill_retval = 0;

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
      if (! eventbuffer.IsPhysicsEvent() ) {
         continue;
      }

      // Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(tracking_detectors);
      eventbuffer.FillSubsystemData(parity_detectors);

      // Process the event
      tracking_detectors.ProcessEvent();
      parity_detectors.ProcessEvent();

      // Fill the tree
      tracking_detectors.FillTreeVector(tracking_vector);
      parity_detectors.FillTreeVector(parity_vector);

      // Fill the histograms for the subsystem objects.
      if (kHisto) tracking_detectors.FillHistograms();
      if (kHisto) parity_detectors.FillHistograms();



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

      // Conver the hit list to ROOT output format
      if (kTree) hitlist_root->Build(*hitlist);

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
      if (kTree) {
	hit_fill_retval   = hit_tree->Fill();
	event_fill_retval = event_tree->Fill();
	
	if ((hit_fill_retval==-1) or (event_fill_retval==-1)) {
	  QwError << "Please check your disk space in order to save ROOT file properly."
		  << QwLog::endl;
	  exit(-1);
	}
      }

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
    if (kHisto) tracking_detectors.DeleteHistograms();
    if (kHisto) parity_detectors.DeleteHistograms();

    // Close ROOT file
    rootfile->Close();
    // Note: Closing rootfile too early causes segfaults when deleting histos

    // Delete objects (this is confusing: the if only applies to the delete)
    if (rootfile)       delete rootfile;       rootfile = 0;
    if (hitlist)        delete hitlist;        hitlist = 0;
    if (event)          delete event;          event = 0;
    if (hitlist_root)   delete hitlist_root;   hitlist_root = 0;

    // Print run summary information
    eventbuffer.ReportRunSummary();
    eventbuffer.PrintRunTimes();

  } // end of loop over runs

  // Delete objects
  if (trackingworker) delete trackingworker; trackingworker = 0;

  QwMessage << "I have done everything I can do..." << QwLog::endl;

  return 0;
}
