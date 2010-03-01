/*------------------------------------------------------------------------*//*!

 \file QwTracking.cc
 \ingroup QwTracking

 \brief This is the main executable for the tracking analysis.

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <sys/time.h>

// ROOT headers
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>
#include <TStopwatch.h>

// Qweak logging facility
#include "QwLog.h"
#include "QwOptionsTracking.h"

// Qweak headers
#include "QwParameterFile.h"
#include "QwSubsystemArrayTracking.h"
#include "QwGasElectronMultiplier.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwTriggerScintillator.h"
#include "QwMainDetector.h"
#include "QwScanner.h"
//
#include "QwEventBuffer.h"
//
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwHitRootContainer.h"
#include "QwTrackingTreeRegion.h"
#include "QwTrackingWorker.h"
#include "QwTrackingTree.h"
#include "QwTrackingTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwEvent.h"

// Qweak headers (deprecated)
#include "Det.h"
#include "Qset.h"



// Debug level
static const bool kDebug = kFALSE;
// Tracking
static const bool kTracking = kTRUE;
// ROOT file output
static const bool kTree = kTRUE;
static const bool kHisto = kTRUE;


// Main function
Int_t main(Int_t argc, Char_t* argv[])
{
  /// First, we set the command line arguments and the configuration filename,
  /// and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwtracking.conf");
  // Define the command line options
  DefineOptionsTracking(gQwOptions);

  // Message logging facilities
  gQwLog.InitLogFile("QwTracking.log");
  gQwLog.SetScreenThreshold(QwLog::kMessage);
  gQwLog.SetFileThreshold(QwLog::kMessage);

  // Either the DISPLAY is not set or JOB_ID is defined: we take it as in batch mode.
  Bool_t kInQwBatchMode = kFALSE;

  if (getenv("DISPLAY") == NULL
      || getenv("JOB_ID")  != NULL) {
    kInQwBatchMode = kTRUE;
    gROOT->SetBatch(kTRUE);
  }

  gQwHists.LoadHistParamsFromFile(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/cosmics_hists.in");

  // Fill the search paths for the parameter files
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH")) + "/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS")) + "/Tracking/prminput");

  //  Load the histogram parameter definitions
  gQwHists.LoadHistParamsFromFile("cosmics_hists.in");

  // Handle for the list of VQwSubsystemTracking objects
  QwSubsystemArrayTracking detectors;

  // Region 1 GEM
  detectors.push_back(new QwGasElectronMultiplier("R1"));
  detectors.GetSubsystem("R1")->LoadChannelMap("qweak_R1.map");
  detectors.GetSubsystem("R1")->LoadQweakGeometry("qweak_new.geo");




  // Region 2 HDC
  detectors.push_back(new QwDriftChamberHDC("R2"));
  detectors.GetSubsystem("R2")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) detectors.GetSubsystem("R2"))->LoadQweakGeometry("qweak_new.geo");
  // Region 3 VDC
  detectors.push_back(new QwDriftChamberVDC("R3"));
  //detectors.GetSubsystem("R3")->LoadChannelMap("qweak_cosmics_hits.map");
  detectors.GetSubsystem("R3")->LoadChannelMap("TDCtoDL.map");
  ((VQwSubsystemTracking*) detectors.GetSubsystem("R3"))->LoadQweakGeometry("qweak_new.geo");
  // Trigger scintillators
  detectors.push_back(new QwTriggerScintillator("TS"));
  ((VQwSubsystemTracking*) detectors.GetSubsystem("TS"))->LoadChannelMap("trigscint_cosmics.map");
  // Main detector
  //detectors.push_back(new QwMainDetector("MD"));
  //detectors.GetSubsystem("MD")->LoadChannelMap("maindet_cosmics.map");
  // Focal plane scanner (needs explicit cast because inherits from both)
  //detectors.push_back(new QwScanner("FPS"));
  //((VQwSubsystemTracking*) detectors.GetSubsystem("FPS"))->LoadChannelMap("scanner_channel.map" );
  //((VQwSubsystemTracking*) detectors.GetSubsystem("FPS"))->LoadInputParameters("scanner_pedestal.map");


  // Get vector with detector info (by region, plane number)
  std::vector< std::vector< QwDetectorInfo > > detector_info;
  detectors.GetSubsystem("R1")->GetDetectorInfo(detector_info);
  detectors.GetSubsystem("R2")->GetDetectorInfo(detector_info);
  detectors.GetSubsystem("R3")->GetDetectorInfo(detector_info);
  // TODO This is handled incorrectly, it just adds the three package after the
  // existing three packages from region 2...  GetDetectorInfo should descend
  // into the packages and add only the detectors in those packages.
  // Alternatively, we could implement this with a singly indexed vector (with
  // only an id as primary index) and write a couple of helper functions to
  // select the right subvectors of detectors.


  // Create and fill old detector structures (deprecated)
  Qset qset;
  qset.FillDetectors((std::string(getenv("QWANALYSIS")) + "/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();

  // Disable 'fake' detectors in region 2 cosmic data:
  //   third and fourth plane for region 2 direction X, U, V
  //   (they are trigger channels, always firing on wire 1)
  rcDETRegion[kPackageUp][kRegionID2][kDirectionX]->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionX]->nextsame->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->nextsame->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->nextsame->nextsame->nextsame->SetInactive();



  QwTrackingWorker *trackingworker = 0;
  // Create the tracking worker
  if (kTracking) {
    trackingworker = new QwTrackingWorker("qwtrackingworker");
    if (kDebug) trackingworker->SetDebugLevel(1);
  }



  // Create a timer
  TStopwatch timer;


  // Create the event buffer
  QwEventBuffer eventbuffer;




  char *hostname, *session;
  const char *tmp;

  // Loop over all runs
  UInt_t runnumber_min = (UInt_t) gQwOptions.GetIntValuePairFirst("run");
  UInt_t runnumber_max = (UInt_t) gQwOptions.GetIntValuePairLast("run");
  for (UInt_t run  = runnumber_min;
              run <= runnumber_max;
              run++) {

    //  Begin processing for the first run.
    //  Start the timer.
    timer.Start();

    /* Does OnlineAnaysis need several runs? by jhlee */
    if (gQwOptions.GetValue<bool>("online")) {

      // Check whether CODA ET streams are compiled in
      #ifndef __CODA_ET
        std::cerr << "\nERROR:  Online mode will not work without the CODA libraries!"
                  << std::endl;
        break;
      #endif

      /* Modify the call below for your ET system, if needed.
         OpenETStream( ET host name , $SESSION , mode)
         mode=0: wait forever
         mode=1: timeout quickly
      */

      hostname = getenv("HOSTNAME");
      session  = getenv("SESSION");
      /* std::cout << "hostname is "<< hostname <<" and session is "<< session << ". " << std::endl; */
      if (hostname == NULL || session == NULL) {
        timer.Stop(); /*  don't need the timer, thus Stop; */

        if      (hostname == NULL && session != NULL) tmp = " \"HOSTNAME\" ";
        else if (hostname != NULL && session == NULL) tmp = " ET \"SESSION\" ";
        else                                          tmp = " \"HOSTNAME\" and ET \"SESSION\" ";

        QwError << "ERROR:  the" << tmp
                << "variable(s) is(are) not defined in your environment.\n"
                << "        This is needed to run the online analysis."
                << QwLog::endl;
        exit(EXIT_FAILURE);
      }
      else {
        QwMessage << "Try to open the ET station. " << QwLog::endl;
        if (eventbuffer.OpenETStream(hostname, session, 0) == CODA_ERROR ) {
          QwError << "ERROR:  Unable to open the ET station "
                  << run << ".  Moving to the next run.\n"
                  << QwLog::endl;
          timer.Stop();
          continue;
        }
      }
    }
    else {
      //  Try to open the data file.
      if (eventbuffer.OpenDataFile(run) == CODA_ERROR){
        //  The data file can't be opened.
        //  Get ready to process the next run.
        QwError << "ERROR:  Unable to find data files for run "
                << run << ".  Moving to the next run.\n"
                << QwLog::endl;
        timer.Stop();
        continue;
      }
    }

    eventbuffer.ResetControlParameters();

    TFile *rootfile = 0;
    if(rootfile) {
      if(rootfile-> IsOpen()) rootfile->Close();
      delete rootfile; rootfile=0;
    }

    rootfile = new TFile(Form(TString(getenv("QWSCRATCH")) + "/rootfiles/Qweak_%d.root", run),
			 "RECREATE",
			 "QWeak ROOT file with real events");
    //    std::auto_ptr<TFile> rootfile (new TFile(Form(TString(getenv("QWSCRATCH")) + "/rootfiles/Qweak_%d.root", run),
    //   					"RECREATE",
    //   					"QWeak ROOT file with real events"));

    //  Create the histograms for the QwDriftChamber subsystem object.
    //  We can create a subfolder in the rootfile first, if we want,
    //  and then pass it into the constructor.
    //
    //  To pass a subdirectory named "subdir", we would do:
    //    detectors.GetSubsystem("MD")->ConstructHistograms(rootfile->mkdir("subdir"));

    // Construct histograms
    //detectors.ConstructHistograms(rootfile->mkdir("histos"));

    // Open file

    TTree* tree = 0;
    QwEvent* event = 0;
    QwHitRootContainer* rootlist = 0;

    if (kTree) {
      rootfile->cd(); // back to the top directory
      tree = new TTree("tree", "Hit list");
      rootlist = new QwHitRootContainer();
      tree->Branch("hits", "QwHitRootContainer", &rootlist);
      tree->Branch("events", "QwEvent", &event);
    }

    if(kHisto){
	rootfile->cd();
	detectors.ConstructHistograms(rootfile->mkdir("tracking_histo"));
	rootfile->cd();
    }
    QwHitContainer* hitlist = 0;
    Int_t nevents           = 0;

    Int_t eventnumber = -1;
    Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
    Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");

    while (eventbuffer.GetEvent() == CODA_OK) {
      //  Loop over events in this CODA file
      //  First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()){
	//  Send ROC configuration event data to the subsystem objects.
	eventbuffer.FillSubsystemConfigurationData(detectors);
      }
      //  Now, if this is not a physics event, go back and get
      //  a new event.
      if (! eventbuffer.IsPhysicsEvent() ) {
	
	continue;
      }

      //  Check to see if we want to process this event.
      eventnumber = eventbuffer.GetEventNumber();
      if      (eventnumber < eventnumber_min) continue;
      else if (eventnumber > eventnumber_max) break;

      if (eventnumber % 1000 == 0) {
	QwMessage << "Number of events processed so far: "
		  << eventnumber << QwLog::endl;
      }

      // Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(detectors);

      // Process the event
      detectors.ProcessEvent();

      // Fill the histograms for the subsystem objects.
      if(kHisto) detectors.FillHistograms();



      // Create the event header with the run and event number
      QwEventHeader* header = new QwEventHeader(run, eventnumber);

      // Create and fill hit list
      hitlist = new QwHitContainer();
      detectors.GetHitList(hitlist);
      // Sorting the grand hit list
      hitlist->sort();

      // Skip empty events
      if (hitlist->size() == 0) continue;

      // Print hit list
      if (kDebug) {
        std::cout << "Event " << eventnumber << std::endl;
        hitlist->Print();
      }

      // Conver the hit list to ROOT output format
      //if (kTree) rootlist->Convert(hitlist);
      if (kTree) rootlist->Build(*hitlist);


      // Track reconstruction
      if (kTracking) {
        // Process the hits
        event = trackingworker->ProcessHits(&detectors, hitlist);

        // Assign the event header
        event->SetEventHeader(header);

        // Print the reconstructed event
        if (kDebug) event->Print();
      }


      // Save the hitlist to the tree
      if (kTree)
        tree->Fill();


      // Delete objects
      if (hitlist)  delete hitlist; hitlist = 0;
      if (event)    delete event; event = 0;

      nevents++;

    } // end of loop over events



    // Print summary information
    QwMessage << "Total number of events processed: " << nevents << QwLog::endl;
    if (kTracking) {
      QwMessage << "Number of good partial tracks: "
                << trackingworker->ngood << QwLog::endl;
    }

    timer.Stop();

    /*  Write to the root file, being sure to delete the old cycles  *
     *  which were written by Autosave.                              *
     *  Doing this will remove the multiple copies of the ntuples    *
     *  from the root file.                                          */
    //    if (rootfile != NULL) rootfile->Write(0, TObject::kOverwrite);

    // Write and close file (after last access to ROOT tree)
    rootfile->Write(0, TObject::kOverwrite);

    // Close CODA file
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

    // Delete histograms in the subsystems
    if(kHisto) detectors.DeleteHistograms();

    // Close ROOT file
    rootfile->Close();
    // Note: Closing rootfile too early causes segfaults when deleting histos

    // Delete objects (this is confusing: the if only applies to the delete)
    if (rootfile)       delete rootfile; rootfile = 0;
    if (trackingworker) delete trackingworker; trackingworker = 0;
    if (hitlist)        delete hitlist; hitlist = 0;
    if (event)          delete event; event = 0;
    if (rootlist)       delete rootlist; rootlist = 0;

    // Print run summary information
    QwMessage << "Analysis of run " << run << QwLog::endl
              << "CPU time used:  " << timer.CpuTime() << " s "
              << "(" << timer.CpuTime() / nevents << " s per event)" << QwLog::endl
              << "Real time used: " << timer.RealTime() << " s "
              << "(" << timer.RealTime() / nevents << " s per event)" << QwLog::endl;

  } // end of loop over runs



  return 0;
}

