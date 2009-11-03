/*------------------------------------------------------------------------*//*!

 \defgroup QwTrackingAnl QwTracking

 \section overview Overview of the Qweak Track Reconstruction package

    The Qweak Track Reconstruction package (QTR) was originally based upon
    software developed for the HERMES experiment.  In both experiments,
    charged particles pass through drift chambers without the presence of a
    magnetic field.  In each experiment, upstream and downstream straight track
    segments are connected by a magnetic field inwhich there are no detectors.
    Therefore track reconstruction is performed separately for the upstream and
    downstream regions, and then a fit is performed to match the tracks in the
    magnetic field.

    The main difference between the experiments in terms of track
    reconstruction, is that in HERMES there were multiple types of tracked
    particles, and thus particle identification was an additional requirement.
    in Qweak, only electrons will be tracked, therefore there is no need for
    particle identification algorithms.  Additionally, due to the nature of the
    Qweak experiment, precision is of the utmost importance.  This requires the
    development of calibration routines which include but are not limited to:
    fitting drift-time-to-distance functions, detector alignment, detector plane
    alignment, and possibly drift chamber wire alignment.

    QTR makes use of pattern recognition in order to identify good tracks from
    a set of hits.  In the simplest case, a single electron track produces a
    nominal set of hits in each drift chamber (DC) wire it passes.  In reality,
    DC wires may suffer from various inefficiencies.  They may fire all the
    time, without the presence of a charged particle track, giving rise to false
    hits.  They may also fail and not fire at all, creating gaps in the data
    set.  Additionally, in the case of multiple tracks, pattern recognition must
    be used to identify which hits belong to which track.  While it is not
    currently the plan to run at a beam current which would produce a
    significant percentage of multi-track events, the reconstruction software
    should be flexible in this aspect so that running conditions are not
    constrained by software capabilities.

 \section HDCvsVDC Differences between Region 2 HDCs and Region 3 VDCs

    Due to the fundamental differences between the HDCs in region 2 and the
    VDCs in region 3, they are treated differently in the tracking code too.

    \subsection HDCvsVDC-HDC The region 2 HDCs

    In the HDCs (Horizontal Drift Chambers), there are four planes of wires in
    each direction, and the nominal particle track is very rougly perpendicular
    to the wire planes.  Due to the HDC design, an individual hit determines the
    distance of closest approach to the wire.  From this distance and a rough
    calculation of the entrance angle, the position of ht eparticle on the wire
    plane may be determined.  Therefore, each plane is divided into a bit array
    with the hit position(s) corresponding to the 'on' bit(s) in the array.
    The 2D pattern is then created from the set of planes that lie in the same
    direction.

    \subsection HDCvsVDC-VDC The region 3 VDCs

    The VDCs (Vertical Drift Chambers) are constructed with an array of signal
    wires that lie between two cathode planes.  The chambers are oriented such
    that the nominal particle trajectories traverse four to eight of the drift
    cells in a single plane.  Given an approximate entrance angle, the drift
    time (measured using a TDC) is correlated with the distance of the track
    to the wire, on a line between the wire and the cathode plane.  Therefore,
    each plane of wires has its own 2D bit pattern, consisting of the wire
    number on one axis, and the drift distance on the other axis.  Currently,
    the bit pattern contains eight columns, corresponding to the maximum number
    of hits for ideal tracks.

    Due to the large number of sense wires, limited space and funding, and the
    complication of rotating the detectors in the lab, each TDC measurement will
    correspond to a group of eight non-consecutive sense wires.  The
    multiplicity arising from this setup may generate ambiguities in the
    identification of the correct wire hit by an electron.  In the case of
    multiple tracks in a single event, these ambiguities must be resolved by
    the tracking code.

    In the region 3 part of the tracking code method QwTrackingWorker::ProcessHits,
    there is a loop over the two VDC planes.  The QwTrackingTreeSearch::TsSetPoint
    method is called for each plane to map the hits in the event to a bit pattern.
    Next, the QwTrackingTreeSearch::TsSearch method is called to find all matching
    patterns for each plane. Finally, treecombine::TlTreeLineSort is called to
    obtain the track segment candidates for this wire plane.

    treecombine::TlTreeLineSort first matches a subset of the hits in the event
    to the matching patterns.  This is done by checking which hits are closest
    to the bits in the patterns.  Once the correct hits are identified, they
    are fit to a line and a chi-square value is calculated.
    treecombine::TlTreeLineSort then uses the treesort class to sort the track
    candidates by likelihood.  Each track candidate is strung together into
    a linked list and is available to the QwTrackingWorker::ProcessHits.

    At this point there are sets of track candidates in the upstream and
    downstream planes of a single wire direction.  QwTrackingWorker::ProcessHits
    next calls the QwTrackingTreeMatch::MatchR3 method which loops over the
    upstream and downstream track candidates to identify which best line up
    according to their slopes and intercepts.  QwTrackingTreeMatch::MatchR3
    returns a new set of track candidates which represent both planes in the same
    wire direction. The loop  over the two wire directions is ended, with tracks
    in the <i>u</i> and <i>v</i> directions.

 \section code-overview Code Overview

    The Qweak Tracking code is built around four main tracking modules:
    - QwTrackingWorker (the main entry point)
    - QwTrackingTreeSearch
    - QwTrackingTreecombine
    - QwTrackingTreeSort
    - QwTrackingTreeMatch

    The organization is done by the module QwTrackingWorker.

*//*-------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!

 \file QwTracking.cc
 \ingroup QwTrackingAnl

 \brief This is the main executable for the tracking analysis.

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <sys/time.h>

// ROOT headers
#include <TFile.h>
#include <TTree.h>
#include <TStopwatch.h>

// Qweak headers
#include "QwCommandLine.h"
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
#include "Qoptions.h"
#include "options.h"


// Global variables for tracking modules (deprecated)
Options opt;
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];


// Debug level
static const bool kDebug = false;
// Tracking
static const bool kTracking = false;
// ROOT file output
static const bool kTree = true;
static const bool kHisto = true;


// Main function
int main(Int_t argc,Char_t* argv[])
{
  // Either the DISPLAY is not set or JOB_ID is defined: we take it as in batch mode.
  Bool_t kInQwBatchMode = kFALSE;
  if (getenv("DISPLAY") == NULL
   || getenv("JOB_ID")  != NULL) {
    kInQwBatchMode = kTRUE;
    gROOT->SetBatch(kTRUE);
  }

  // Fill the search paths for the parameter files
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH")) + "/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS")) + "/Tracking/prminput");

  // Parse command line options
  QwCommandLine cmdline;
  cmdline.Parse(argc, argv);


  // Handle for the list of VQwSubsystemTracking objects
  QwSubsystemArrayTracking detectors;
  // Region 1 GEM
  //detectors.push_back(new QwGasElectronMultiplier("R1"));
  // Region 2 HDC
  detectors.push_back(new QwDriftChamberHDC("R2"));
  detectors.GetSubsystem("R2")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) detectors.GetSubsystem("R2"))->LoadQweakGeometry("qweak_new.geo");
  // Region 3 VDC
  detectors.push_back(new QwDriftChamberVDC("R3"));
  //detectors.GetSubsystem("R3")->LoadChannelMap("qweak_cosmics_hits.map");
  //detectors.GetSubsystem("R3")->LoadChannelMap("TDCtoDL.map");
  ((VQwSubsystemTracking*) detectors.GetSubsystem("R3"))->LoadQweakGeometry("qweak_new.geo");
  // Trigger scintillators
  //detectors.push_back(new QwTriggerScintillator("TS"));
  // Main detector
  //detectors.push_back(new QwMainDetector("MD"));
  //detectors.GetSubsystem("MD")->LoadChannelMap("maindet_cosmics.map");
  // Focal plane scanner (needs explicit cast because inherits from both)
  //detectors.push_back(new QwScanner("FPS"));
  //((VQwSubsystemTracking*) detectors.GetSubsystem("FPS"))->LoadChannelMap("scanner_channel.map" );
  //((VQwSubsystemTracking*) detectors.GetSubsystem("FPS"))->LoadInputParameters("scanner_pedestal.map");


  // Get vector with detector info (by region, plane number)
  std::vector< std::vector< QwDetectorInfo > > detector_info;
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


  // Set global options (deprecated)
  Qoptions qoptions;
  qoptions.Get((std::string(getenv("QWANALYSIS")) + "/Tracking/prminput/qweak.options").c_str());


  // Create the tracking worker
  QwTrackingWorker *trackingworker = new QwTrackingWorker("qwtrackingworker");
  if (kDebug) trackingworker->SetDebugLevel(1);


  // Create a timer
  TStopwatch timer;


  // Create the event buffer
  QwEventBuffer eventbuffer;


  char *hostname, *session;
  const char *tmp;

  // Loop over all runs
  for (UInt_t run =  (UInt_t) cmdline.GetFirstRun();
              run <= (UInt_t) cmdline.GetLastRun(); run++) {

    //  Begin processing for the first run.
    //  Start the timer.
    timer.Start();
    /* Does OnlineAnaysis need several runs? by jhlee */
    if (cmdline.DoOnlineAnalysis()) {
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

        std::cerr << "ERROR:  the" << tmp
                  << "variable(s) is(are) not defined in your environment.\n"
                  << "        This is needed to run the online analysis."
                  << std::endl;
        exit(EXIT_FAILURE);
      } else {
        std::cout << "Try to open the ET station. " << std::endl;
        if (eventbuffer.OpenETStream(hostname, session, 0) == CODA_ERROR ) {
          std::cerr << "ERROR:  Unable to open the ET station "
                    << run << ".  Moving to the next run.\n"
                    << std::endl;
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
        std::cerr << "ERROR:  Unable to find data files for run "
                  << run << ".  Moving to the next run.\n"
                  << std::endl;
        timer.Stop();
        continue;
      }
    }

    eventbuffer.ResetControlParameters();

    //     //  Configure database access mode, and load the calibrations
    //     //  from the database.

    //  Create the root file
    boost::shared_ptr<TFile>
      rootfile(new TFile(Form(TString(getenv("QWSCRATCH")) + "/rootfiles/Qweak_%d.root", run),
                         "RECREATE",
                         "QWeak ROOT file with real events"));

    //  Create the histograms for the QwDriftChamber subsystem object.
    //  We can create a subfolder in the rootfile first, if we want,
    //  and then pass it into the constructor.
    //
    //  To pass a subdirectory named "subdir", we would do:
    //    detectors.GetSubsystem("MD")->ConstructHistograms(rootfile->mkdir("subdir"));

    // Open file
    TTree* tree;
    QwHitRootContainer* rootlist = new QwHitRootContainer();
    if (kTree) {
      tree = new TTree("tree", "Hit list");
      tree->Branch("events", "QwHitRootContainer", &rootlist);
    }

    // Construct histograms
    detectors.ConstructHistograms();

    while (eventbuffer.GetEvent() == CODA_OK){
      //  Loop over events in this CODA file
      //  First, do processing of non-physics events...

      //  Now, if this is not a physics event, go back and get
      //  a new event.
      if (! eventbuffer.IsPhysicsEvent()) continue;

      //  Check to see if we want to process this event.
      int eventnumber = eventbuffer.GetEventNumber();
      if      (eventnumber < cmdline.GetFirstEvent()) continue;
      else if (eventnumber > cmdline.GetLastEvent())  break;

      if (eventnumber % 1000 == 0) {
        std::cout << "Number of events processed so far: "
                  << eventnumber << std::endl;
      }


      // Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(detectors);

      // Process the event
      detectors.ProcessEvent();

      // Fill the histograms for the subsystem objects.
      detectors.FillHistograms();

      // Create and fill hit list
      QwHitContainer* hitlist = new QwHitContainer();
      detectors.GetHitList(hitlist);

      // Sorting the grand hit list
      hitlist->sort();

      // Print hit list
      if (kDebug)
        hitlist->Print();

      // Convert the hit list to ROOT output format
      rootlist->Convert(hitlist);

      // Save the hitlist to the tree
      if (kTree)
        tree->Fill();


      // Process the hit list through the tracking worker (i.e. do track reconstruction)
      QwEvent* event = 0;
      if (kTracking)
        event = trackingworker->ProcessHits(&detectors, hitlist);


      // Delete objects
      if (hitlist) delete hitlist;
      if (event)   delete event;

    } // end of loop over events

    // Delete objects
    if (rootlist) delete rootlist;


    // Print summary information
    std::cout << "Total number of events processed: "
              << eventbuffer.GetEventNumber() << std::endl;
    std::cout << "Number of good partial tracks: "
              << trackingworker->ngood << std::endl;
    timer.Stop();


    /*  Write to the root file, being sure to delete the old cycles  *
     *  which were written by Autosave.                              *
     *  Doing this will remove the multiple copies of the ntuples    *
     *  from the root file.                                          */
    if (rootfile != NULL) rootfile->Write(0, TObject::kOverwrite);

    // Delete histograms in the subsystems
    detectors.DeleteHistograms();

    // Close CODA file
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

    // Write and close file (after last access to ROOT tree)
    rootfile->Close();


    // Print run summary information
    std::cout << "Analysis of run " << run << std::endl
              << "CPU time used:  " << timer.CpuTime() << " s" << std::endl
              << "Real time used: " << timer.RealTime() << " s" << std::endl;


  } // end of loop over runs



  return 0;
}

