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
#include "QwRaster.h"
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


#include "QwEPICSEvent.h"

// Qweak headers (deprecated)
#include "Det.h"
#include "Qset.h"



// Debug level
static const bool kDebug = kFALSE;
// Tracking
static const bool kTracking = kFALSE;
// ROOT file output
static const bool kTree = kTRUE;
static const bool kHisto = kTRUE;

static const bool kUseTDCHits = kTRUE;

// Branching flags for subsystems
static const bool kMainDetBranch = kFALSE;
static const bool kScannerBranch = kFALSE;
static const bool kRasterBranch  = kFALSE;

// Main function
Int_t main(Int_t argc, Char_t* argv[]) {
    /// First, we set the command line arguments and the configuration filename,
    /// and we define the options that can be used in them (using QwOptions).
    gQwOptions.SetCommandLine(argc, argv);
    gQwOptions.SetConfigFile("qwtracking.conf");
    gQwOptions.SetConfigFile("qweak_mysql.conf");

    // Define the command line options
    DefineOptionsTracking(gQwOptions);

    // Message logging facilities
    gQwLog.InitLogFile("QwTracking.log");
    gQwLog.SetScreenThreshold(QwLog::kDebug);
    gQwLog.SetFileThreshold(QwLog::kMessage);

    // Either the DISPLAY is not set or JOB_ID is defined: we take it as in batch mode.
    Bool_t kInQwBatchMode = kFALSE;

    if (getenv("DISPLAY") == NULL
            || getenv("JOB_ID")  != NULL) {
        kInQwBatchMode = kTRUE;
        gROOT->SetBatch(kTRUE);
    }

    //gQwHists.LoadHistParamsFromFile(getenv_safe_string("QWANALYSIS)+"/Tracking/prminput/cosmics_hists.in");

    // Fill the search paths for the parameter files
    QwParameterFile::AppendToSearchPath(getenv_safe_string("QWSCRATCH") + "/setupfiles");
    QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Tracking/prminput");
    QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");

    //  Load the histogram parameter definitions
    gQwHists.LoadHistParamsFromFile("qweak_tracking_hists.in");
    // Handle for the list of VQwSubsystemTracking objects
    QwSubsystemArrayTracking detectors;

    // Region 1 GEM
    detectors.push_back(new QwGasElectronMultiplier("R1"));
    detectors.GetSubsystemByName("R1")->LoadChannelMap("qweak_R1.map");
    detectors.GetSubsystemByName("R1")->LoadGeometryDefinition("qweak_new.geo");




    // Region 2 HDC
    detectors.push_back(new QwDriftChamberHDC("R2"));
    detectors.GetSubsystemByName("R2")->LoadChannelMap("qweak_cosmics_hits.map");
    ((VQwSubsystemTracking*) detectors.GetSubsystemByName("R2"))->LoadGeometryDefinition("qweak_new.geo");
    // Region 3 VDC
    detectors.push_back(new QwDriftChamberVDC("R3"));
    //detectors.GetSubsystemByName("R3")->LoadChannelMap("qweak_cosmics_hits.map");
    detectors.GetSubsystemByName("R3")->LoadChannelMap("TDCtoDL.map");
    ((VQwSubsystemTracking*) detectors.GetSubsystemByName("R3"))->LoadGeometryDefinition("qweak_new.geo");
    // Trigger scintillators
    detectors.push_back(new QwTriggerScintillator("TS"));
    ((VQwSubsystemTracking*) detectors.GetSubsystemByName("TS"))->LoadChannelMap("qweak_trigscint_channel.map");
    // Main detector
    detectors.push_back(new QwMainDetector("MD"));
    detectors.GetSubsystemByName("MD")->LoadChannelMap("qweak_maindet_channel.map");
    QwMainDetector* maindetector = dynamic_cast<QwMainDetector*> (detectors.GetSubsystemByName("MD"));
    // Scanner
    detectors.push_back(new QwScanner("FPS"));
    ((VQwSubsystemTracking*) detectors.GetSubsystemByName("FPS"))->LoadChannelMap("qweak_scanner_channel.map" );
    ((VQwSubsystemTracking*) detectors.GetSubsystemByName("FPS"))->LoadInputParameters("qweak_scanner_parameter.map");
    QwScanner* scanner = dynamic_cast<QwScanner*> (detectors.GetSubsystemByName("FPS")); // Get scanner subsystem
    // Fast Raster
    detectors.push_back(new QwRaster("FR"));
    ((VQwSubsystemTracking*) detectors.GetSubsystemByName("FR"))->LoadChannelMap("qweak_raster_channel.map" );
    ((VQwSubsystemTracking*) detectors.GetSubsystemByName("FR"))->LoadInputParameters("qweak_raster_parameter.map");
    QwRaster* raster = dynamic_cast<QwRaster*> (detectors.GetSubsystemByName("FR"));

    // Get vector with detector info (by region, plane number)
    std::vector< std::vector< QwDetectorInfo > > detector_info;
    detectors.GetSubsystemByName("R1")->GetDetectorInfo(detector_info);
    detectors.GetSubsystemByName("R2")->GetDetectorInfo(detector_info);
    detectors.GetSubsystemByName("R3")->GetDetectorInfo(detector_info);
    // TODO This is handled incorrectly, it just adds the three package after the
    // existing three packages from region 2...  GetDetectorInfo should descend
    // into the packages and add only the detectors in those packages.
    // Alternatively, we could implement this with a singly indexed vector (with
    // only an id as primary index) and write a couple of helper functions to
    // select the right subvectors of detectors.


    // Create and fill old detector structures (deprecated)
    Qset qset;
    qset.FillDetectors((getenv_safe_string("QWANALYSIS") + "/Tracking/prminput/qweak.geo").c_str());
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
    QwEPICSEvent epics;
    //  Load the Epics table
    epics.LoadEpicsVariableMap("EpicsTable.map");
    // Create the event buffer
    QwEventBuffer eventbuffer;
    eventbuffer.ProcessOptions(gQwOptions);


    // Loop over all runs
    while (eventbuffer.OpenNextStream() == CODA_OK) {
        //  Begin processing for the first run.
        //  Start the timer.
        timer.Start();

        TFile *rootfile = 0;
        if (rootfile) {
            if (rootfile-> IsOpen()) rootfile->Close();
            delete rootfile;
            rootfile=0;
        }

        rootfile = new TFile(Form(getenv_safe_TString("QWSCRATCH") + "/rootfiles/Qweak_%s.root",
                                  eventbuffer.GetRunLabel().Data()), "RECREATE",
                                  "QWeak ROOT file with real events");
        //    std::auto_ptr<TFile> rootfile (new TFile(Form(getenv_safe_TString("QWSCRATCH") + "/rootfiles/Qweak_%d.root", run),
        //   					"RECREATE",
        //   					"QWeak ROOT file with real events"));

        //  Create the histograms for the QwDriftChamber subsystem object.
        //  We can create a subfolder in the rootfile first, if we want,
        //  and then pass it into the constructor.
        //
        //  To pass a subdirectory named "subdir", we would do:
        //    detectors.GetSubsystemByName("MD")->ConstructHistograms(rootfile->mkdir("subdir"));

        // Construct histograms
        //detectors.ConstructHistograms(rootfile->mkdir("histos"));

        // Open file

        TTree* tree = 0;
        QwEvent* event = 0;
        QwHitRootContainer* hitlist_root = 0;

        TString prefix = "";

        if (kTree) {
            rootfile->cd(); // back to the top directory
            tree = new TTree("tree", "Hit list");
            hitlist_root = new QwHitRootContainer();
            tree->Branch("hits", "QwHitRootContainer", &hitlist_root);
            tree->Branch("events", "QwEvent", &event);

            if (kMainDetBranch)
               maindetector->ConstructBranchAndVector(tree, prefix);

            if (kScannerBranch) {
//                scanner->StoreRawData(kScannerRaw);
                scanner->ConstructBranchAndVector(tree, prefix);

            if (kRasterBranch) {
                raster->ConstructBranchAndVector(tree, prefix);

            }

        }

        if (kHisto) {
            rootfile->cd();
            detectors.ConstructHistograms(rootfile->mkdir("tracking_histo"));
            rootfile->cd();
        }
        QwHitContainer* hitlist = 0;
        Int_t nevents           = 0;

        while (eventbuffer.GetNextEvent() == CODA_OK) {
            //  Loop over events in this CODA file
            //  First, do processing of non-physics events...


	    if (eventbuffer.IsEPICSEvent()) {
	      eventbuffer.FillEPICSData(epics);
	      epics.CalculateRunningValues();

	    }



            if (eventbuffer.IsROCConfigurationEvent()) {
                //  Send ROC configuration event data to the subsystem objects.
                eventbuffer.FillSubsystemConfigurationData(detectors);
            }

            //  Now, if this is not a physics event, go back and get
            //  a new event.
            if (! eventbuffer.IsPhysicsEvent() ) {
                continue;
            }


            if (eventbuffer.GetEventNumber() % 1000 == 0) {
                QwMessage << "Number of events processed so far: "
                << eventbuffer.GetEventNumber() << QwLog::endl;
            }

            // Fill the subsystem objects with their respective data for this event.
            eventbuffer.FillSubsystemData(detectors);

            // Process the event
            detectors.ProcessEvent();

            if (kMainDetBranch) maindetector->FillTreeVector(nevents);
            if (kScannerBranch) scanner->FillTreeVector();
            if (kRasterBranch) raster->FillTreeVector();

            // Fill the histograms for the subsystem objects.
            if (kHisto) detectors.FillHistograms();



            // Create the event header with the run and event number
            QwEventHeader* header = new QwEventHeader(eventbuffer.GetRunNumber(), eventbuffer.GetEventNumber());

            // Create and fill hit list
            hitlist = new QwHitContainer();
            if (kUseTDCHits)
              detectors.GetTDCHitList(hitlist);
            else
              detectors.GetHitList(hitlist);

            // Sorting the grand hit list
            hitlist->sort();

            // Skip empty events, if we're not creating the other branches
            if (hitlist->size() == 0 && !(kScannerBranch || kMainDetBranch) || kRasterBranch)
	      continue;

            // Print hit list
            if (hitlist->size() > 0 && kDebug) {
                std::cout << "Event " << eventbuffer.GetEventNumber() << std::endl;
                hitlist->Print();
            }

            // Conver the hit list to ROOT output format
            if (kTree) hitlist_root->Build(*hitlist);

            // Track reconstruction
            if (hitlist->size() > 0 && kTracking) {
                // Process the hits
                event = trackingworker->ProcessHits(&detectors, hitlist);

                // Assign the event header
                event->SetEventHeader(header);

                // Print the reconstructed event
                if (kDebug) event->Print();
            }

            // Save the hitlist to the tree
            if (kTree)  tree->Fill();

            // Delete objects
            if (hitlist)    delete hitlist;    hitlist = 0;
            if (event)      delete event;      event = 0;

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

	epics.ReportEPICSData();
	epics.PrintVariableList();
	epics.PrintAverages();
	//TString tag; epics.GetDataValue(tag);

        // Close CODA file
        eventbuffer.CloseStream();
        eventbuffer.ReportRunSummary();

        // Delete histograms in the subsystems
        if (kHisto) detectors.DeleteHistograms();

        // Close ROOT file
        rootfile->Close();
        // Note: Closing rootfile too early causes segfaults when deleting histos

        // Delete objects (this is confusing: the if only applies to the delete)
        if (rootfile)        delete rootfile;        rootfile = 0;
        if (trackingworker)  delete trackingworker;  trackingworker = 0;
        if (hitlist)         delete hitlist;         hitlist = 0;
        if (event)           delete event;           event = 0;
        if (hitlist_root)    delete hitlist_root;    hitlist_root = 0;

        // Print run summary information
        QwMessage << "Analysis of run " << eventbuffer.GetRunNumber() << QwLog::endl
        << "CPU time used:  " << timer.CpuTime() << " s "
        << "(" << timer.CpuTime() / nevents << " s per event)" << QwLog::endl
        << "Real time used: " << timer.RealTime() << " s "
        << "(" << timer.RealTime() / nevents << " s per event)" << QwLog::endl;

    } // end of loop over runs




    return 0;
}
}


