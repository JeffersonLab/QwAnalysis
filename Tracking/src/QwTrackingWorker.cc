/*------------------------------------------------------------------------*//*!

 \file QwTrackingWorker.cc

 \brief Controls all the routines involved in finding tracks in an event.

 \verbatim

 PROGRAM: QTR (Qweak Track Reconstruction)	AUTHOR: Burnham Stokes
							bestokes@jlab.org
						ORIGINAL HRC AUTHOR
							Wolfgang Wander
							wwc@hermes.desy.de


 MODULE: QwTrackingWorker

 \endverbatim

 PURPOSE: This module contains the code for finding tracks in an event.
          The task of finding tracks is sub-divided into three steps:

           (1) for each set of planes with like-pitched wires, possible
               track candidates (called treelines) are located by
               the treesearch algorithm.  For this search, the hit
               information is used to generate a bit pattern for the hits
               seen in each tree-detector.  The bit patterns for all the
               chambers in one detector region are then processed by the
               the treesearch algorithm to identify all combinations of
               hits in the chambers which could correspond to straight
               tracks through the chambers.  Typically, these treelines contain
               quite a few ghost tracks.  To cut down on these ghosts,
               the treelines located by the searching are reduced by
               a connectivity graph. This graph tries to minimize the number
               of overlapping tracks (tracks including the same hits) based
               on their chi^2 fit quality.

           (2) Once the sets of treelines (one set for the u, a second set
               for the v, and a third set for the x) have been located for
               a region of the detector, the treelines are matched together
               to form partial tracks. For the case of VDC reconstruction
               no x treelines can be formed. In this case only
               u/v combinations are formed.

           (3) And, finally, the partial tracks from the front and the back
               region will be bridged (i.e. paired together) to form the final
               tracks.

 CONTENTS: (brief description for now)

 (01) Bcheck() -

 (02) rcLinkUsedTracks()

 (03) ProcessHits() - this function manages the track-finding and track-fitting
                   in QTR.  For each detector region (front or back, top or
                   bottom) this function builds the bit patterns for the hits
                   seen in the detector and then invokes the treesearch
                   algorithm to find the candidate treelines.  After the
                   resulting treelines are filtered by the treesort and
                   treecombine functions, the treelines from the different
                   plane with different wire-pitch are combined together to
                   form the partial tracks seen in each region of the detector.
                   In the final stage of tracking, these partial tracks are
                   bridged together to reconstruct full tracks through the
                   detector.
                   In a second interation treesearch recalibrates the tracking
                   chamber hits with the now known momentum and recalculates
                   track parameters and momentum for all found tracks.

*//*-------------------------------------------------------------------------*/

#include "QwTrackingWorker.h"
// Standard C and C++ headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <sstream>

using std::cout;
using std::cerr;
using std::endl;

// ROOT headers
#include <TVector3.h>

// Qweak headers
#include "QwOptions.h"
#include "QwLog.h"
#include "globals.h"

// Qweak tree search headers
#include "QwHitPattern.h"
#include "QwTrackingTree.h"
#include "QwTrackingTreeRegion.h"

// Qweak tracking modules
#include "QwTrackingTreeSearch.h"
#include "QwTrackingTreeCombine.h"
#include "QwTrackingTreeSort.h"
#include "QwTrackingTreeMatch.h"

// Qweak track/event headers
#include "QwHit.h"
#include "QwHitContainer.h"
#include "QwTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwEvent.h"
#include "QwDetectorInfo.h"
#include "QwBridgingTrackFilter.h"
#include "QwRayTracer.h"
#include "QwMatrixLookup.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingWorker::QwTrackingWorker(QwOptions& options, const QwGeometry& geometry)
{
  QwDebug << "###### Calling QwTrackingWorker::QwTrackingWorker ()" << QwLog::endl;

  // Debug level
  fDebug = 0;

  // Process options
  ProcessOptions(options);

  // Store geometry
  SetGeometry(geometry);

  // If tracking is disabled, stop here
  if (fDisableTracking) return;

  // Initialize the pattern database
  InitTree(geometry);
  // Initialize a bridging filter
  fBridgingTrackFilter = new QwBridgingTrackFilter();

  // Initialize a ray tracer bridging method
  if (! fDisableMomentum && ! fDisableRayTracer) {
    fRayTracer = new QwRayTracer(options);
  // or set to null if disabled
  } else fRayTracer = 0;

  /* Reset counters of number of good and bad events */
  ngood = 0;
  nbad = 0;

  R2Good = 0;
  R2Bad = 0;
  R3Good = 0;
  R3Bad = 0;

  nbridged = 0;

  // Initialize the tracking modules
  // Tracking functionality is provided by these four sub-blocks.
  fTreeSearch  = new QwTrackingTreeSearch();
  fTreeCombine = new QwTrackingTreeCombine();
  fTreeSort    = new QwTrackingTreeSort();
  fTreeMatch   = new QwTrackingTreeMatch();

  // Debug level of the various tracking modules
  fTreeSearch->SetDebugLevel(fDebug);
  fTreeCombine->SetDebugLevel(fDebug);
  fTreeSort->SetDebugLevel(fDebug);
  fTreeMatch->SetDebugLevel(fDebug);

  // Pass the geometry
  fTreeCombine->SetGeometry(geometry);

  // Process the options and set the respective flags in the modules
  fTreeSearch->SetShowMatchingPatterns(fShowMatchingPattern);
  fTreeCombine->SetMaxRoad(options.GetValue<float>("QwTracking.R2.maxroad"));
  fTreeCombine->SetMaxXRoad(options.GetValue<float>("QwTracking.R2.maxxroad"));

  QwDebug << "###### Leaving QwTrackingWorker::QwTrackingWorker ()" << QwLog::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingWorker::~QwTrackingWorker ()
{
  // Delete helper objects
  if (fBridgingTrackFilter) delete fBridgingTrackFilter;
  if (fRayTracer)    delete fRayTracer;

  // Delete tracking modules
  if (fTreeSearch)  delete fTreeSearch;
  if (fTreeCombine) delete fTreeCombine;
  if (fTreeSort)    delete fTreeSort;
  if (fTreeMatch)   delete fTreeMatch;

  // Summary of tracking objects that are still alive
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
  QwMessage << "  QwTreeLine: "     << QwTreeLine::GetObjectsAlive()
            << " (of " << QwTreeLine::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << "  QwPartialTrack: " << QwPartialTrack::GetObjectsAlive()
            << " (of " << QwPartialTrack::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << "  QwTrack: "        << QwTrack::GetObjectsAlive()
            << " (of " << QwTrack::GetObjectsCreated() << ")" << QwLog::endl;
  QwMessage << QwLog::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingWorker::DefineOptions(QwOptions& options)
{
  // General options
  options.AddOptions("Tracking options")("QwTracking.debug",
                          po::value<int>()->default_value(0),
                          "track reconstruction debug level");
  options.AddOptions("Tracking options")("QwTracking.regenerate",
                          po::value<bool>()->default_bool_value(false),
                          "regenerate search trees");
  options.AddOptions("Tracking options")("QwTracking.disable-tracking",
                          po::value<bool>()->default_bool_value(false),
                          "disable all tracking analysis");
  options.AddOptions("Tracking options")("QwTracking.print-pattern-db",
                          po::value<bool>()->default_bool_value(false),
                          "print pattern database");
  options.AddOptions("Tracking options")("QwTracking.showeventpattern",
                          po::value<bool>()->default_bool_value(false),
                          "show bit pattern for all events");
  options.AddOptions("Tracking options")("QwTracking.showmatchingpattern",
                          po::value<bool>()->default_bool_value(false),
                          "show bit pattern for matching tracks");

  options.AddOptions("Tracking options")("QwTracking.package-mismatch",
                          po::value<bool>()->default_bool_value(false),
                          "if the package number is different for R2 and R3 at the same octant");

  // Region 2
  options.AddOptions("Tracking options")("QwTracking.R2.levels",
                          po::value<int>()->default_value(8),
                          "number of search tree levels in region 2");
  options.AddOptions("Tracking options")("QwTracking.R2.maxslope",
                          po::value<float>()->default_value(0.862),
                          "maximum allowed slope for region 2 tracks");
  options.AddOptions("Tracking options")("QwTracking.R2.maxroad",
                          po::value<float>()->default_value(1.4),
                          "maximum allowed road width for region 2 tracks");
  options.AddOptions("Tracking options")("QwTracking.R2.maxxroad",
                          po::value<float>()->default_value(25.0),
                          "maximum allowed X road width for region 2 tracks");
  options.AddOptions("Tracking options")("QwTracking.R2.MaxMissedPlanes",
                          po::value<int>()->default_value(1),
                          "maximum number of missed planes");
  options.AddOptions("Tracking options")("QwTracking.R2.DropWorstHit",
                          po::value<bool>()->default_bool_value(false),
                          "attempt partial track fit without worst hit");

  // Region 3
  options.AddOptions("Tracking options")("QwTracking.R3.levels",
                          po::value<int>()->default_value(4),
                          "number of search tree levels in region 3");
  options.AddOptions("Tracking options")("QwTracking.R3.MaxMissedWires",
                          po::value<int>()->default_value(4),
                          "maximum number of missed wires");
  options.AddOptions("Tracking options")("QwTracking.R3.RotatorTilt",
			  po::value<bool>()->default_value(true),
			  "apply region 3 rotator tilt correction");

  // Momentum reconstruction
  options.AddOptions("Tracking options")("QwTracking.disable-momentum",
                          po::value<bool>()->default_bool_value(false),
                          "disable the momentum reconstruction");
  options.AddOptions("Tracking options")("QwTracking.disable-matrixlookup",
                          po::value<bool>()->default_bool_value(false),
                          "disable the use of the momentum lookup table");
  options.AddOptions("Tracking options")("QwTracking.disable-raytracer",
                          po::value<bool>()->default_bool_value(false),
                          "disable the magnetic field map tracking");
  options.AddOptions("Tracking options")("QwTracking.lookuptable",
                          po::value<std::string>()->default_value("QwTrajMatrix.root"),
                          "filename of the lookup table in QW_LOOKUP");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingWorker::ProcessOptions(QwOptions& options)
{
  // Enable tracking debug flag
  fDebug = options.GetValue<int>("QwTracking.debug");
  fRegenerate = options.GetValue<bool>("QwTracking.regenerate");

  // Disable tracking and/or momentu reconstruction
  fDisableTracking = options.GetValue<bool>("QwTracking.disable-tracking");
  fDisableMomentum = options.GetValue<bool>("QwTracking.disable-momentum");
  fMismatchPkg=options.GetValue<bool>("QwTracking.package-mismatch");

  // Set the flags for printing the pattern database
  fPrintPatternDatabase = options.GetValue<bool>("QwTracking.print-pattern-db");

  // Set the flags for showing the matching event patterns
  fShowEventPattern    = options.GetValue<bool>("QwTracking.showeventpattern");
  fShowMatchingPattern = options.GetValue<bool>("QwTracking.showmatchingpattern");

  // Enable/disable the lookup table and raytracer momentum reconstruction methods
  fDisableMatrixLookup = options.GetValue<bool>("QwTracking.disable-matrixlookup");
  fDisableRayTracer    = options.GetValue<bool>("QwTracking.disable-raytracer");

  // Lookup table filename
  fFilenameLookupTable = options.GetValue<std::string>("QwTracking.lookuptable");

  // Number of levels (search tree depth) for region 2
  fLevelsR2 = options.GetValue<int>("QwTracking.R2.levels");
  // Number of levels (search tree depth) for region 3
  fLevelsR3 = options.GetValue<int>("QwTracking.R3.levels");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingWorker::InitTree(const QwGeometry& geometry)
{
  // Loop over packages
  for (EQwDetectorPackage package = kPackage1;
      package <= kPackage2; package++) {
    // Loop over regions
    for (EQwRegionID region = kRegionID2;
        region <= kRegionID3; region++) {
      // Loop over directions
      for (EQwDirectionID direction = kDirectionX;
          direction <= kDirectionV; direction++) {

        // Get the detectors, and skip if none are active
        QwGeometry detectors(geometry.in(region).in(package).in(direction));
        if (detectors.size() == 0) continue;

        // Get the first plane
        QwDetectorInfo* det = detectors.front();

        int levels = 0;
        int numlayers = 0;
        double width = 0.0;

        /// Region 2 contains 4 layers
        if (det->GetRegion() == kRegionID2) {
          numlayers = 4; // TODO replace this with info from the geometry file
          width = det->GetElementSpacing() * det->GetNumberOfElements();
          levels = fLevelsR2;
        }

        /// Region 3 contains 8 layers
        if (det->GetRegion() == kRegionID3) {
          numlayers = 8; // TODO replace this with info from the geometry file
          width = det->GetActiveWidthZ();
          levels = fLevelsR3;
        }

        /// Create a new search tree
        QwTrackingTree thetree(numlayers);

        /// Set the maximum slope
        thetree.SetMaxSlope(gQwOptions.GetValue<float>("QwTracking.R2.maxslope"));

        /// Set the geometry
        thetree.SetGeometry(detectors);

        /// Set up the filename with the following format
        ///   tree[numlayers]-[levels]-[u|l]-[1|2|3]-[n|u|v|x|y].tre
        std::stringstream filename;
        filename << getenv_safe_string("QW_SEARCHTREE")
                 << "/tree" << numlayers << "-" << levels
                 << "-" << package << "-" << region << "-" << direction << ".tre";
        QwDebug << "Tree filename: " << filename.str() << QwLog::endl;

        /// Each element of search tree will point to a pattern database
        // TODO leaking trees here
        QwTrackingTreeRegion* searchtree = thetree.inittree(filename.str(),
                                               levels,
                                               numlayers,
                                               width,
                                               det,
                                               fRegenerate);
        if (fPrintPatternDatabase && region == kRegionID2) searchtree->PrintNodes();
        if (fPrintPatternDatabase && region == kRegionID3) searchtree->PrintTrees();

        // Set the detector identification
        searchtree->SetRegion(region);
        searchtree->SetPackage(package);
        searchtree->SetDirection(direction);

        // Store pointer to tree in detector objects
        for (size_t i = 0; i < detectors.size(); i++) {
          detectors.at(i)->SetTrackingSearchTree(searchtree);
        }

      } // end of loop over directions
    } // end of loop over regions
  } // end of loop over packages
}

/*------------------------------------------------------------------------*//*!

  ProcessHits() - this function is the main tracking function. It
               performs tracking in projections (u/v/x) to form treelines,
               it combines projection tracks to tracks in space and bridges
               tracks in space before and after the magnet to form recon-
               structed particle tracks. The function calculates the momentum
               of the tracks and afterwards reiterates the track parameters
               using the now known track position and momentum for iterating
               the hit positions in the tracking chambers.

   inputs:  (1) QwHitContainer &hitlist - pointer to the QwHitContainer object
                                          with the hit list.

   outputs: (1) Event* ProcessHits() - pointer to the structure with all
                                    of the reconstruction information for
                                    this event.

*//*-------------------------------------------------------------------------*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingWorker::ProcessEvent (
    const QwSubsystemArrayTracking *detectors, QwEvent *event)
{
    // Get hit list from event
    QwHitContainer* hitlist = event->GetHitContainer();

    // Print hitlist
    if (fDebug) hitlist->Print();

    /// If no hits, return
    if (hitlist->size() == 0) {
        delete hitlist;
        return;
    }

    /// If tracking is disabled, return
    if (fDisableTracking) {
        delete hitlist;
        return;
    }


    /// Loop through all detector packages
    // Normally only two of these will generate actual tracks,
    // since there are only two tracking packages for the eight
    // possible positions
    for (EQwDetectorPackage package  = kPackage1;
                            package <= kPackage2;
                            package++) {
        QwDebug << "[QwTrackingWorker::ProcessHits] Package: " << package << QwLog::endl;

        /// Loop through the detector regions
        for (EQwRegionID region  = kRegionID2;
                         region <= kRegionID3;
                         region++) {
            QwDebug << "[QwTrackingWorker::ProcessHits]  Region: " << region << QwLog::endl;


            int dlayer = 0; // number of detector planes in the search
            int tlayers = 0; // number of tracking layers

            /// Loop through the detector directions
            for (EQwDirectionID dir  = kDirectionX;
                                dir <= kDirectionV;
                                dir++) {
              QwDebug << "[QwTrackingWorker::ProcessHits]    Direction: " << dir << QwLog::endl;

              // Find these detectors
              QwGeometry detectors(fGeometry.in(region).in(package).in(dir));
              if (detectors.size() == 0) continue;

              /*! ---- 1st: check that the direction is tree-searchable               ---- */

              // The search tree for this detector will be stored in searchtree
              QwTrackingTreeRegion* searchtree = detectors.at(0)->GetTrackingSearchTree();

              // Check whether the search tree exists
              if (! searchtree) {
                QwDebug << "[QwTrackingWorker::ProcessHits]     No such tree!" << QwLog::endl;
                continue;
              }

              // Check whether the search tree is searchable
              if (! searchtree->IsSearchable()) {
                event->fTreeLine[package][region][dir] = 0;
                QwDebug << "[QwTrackingWorker::ProcessHits]     Search tree not searchable!" << QwLog::endl;
                continue;
              }


              /*! ---- 2nd: do some variable initialization for the found tree line
                                  linked list                                              ---- */

              // Start the search for this set of like-pitched planes
              QwTreeLine* treelinelist = 0; // local list of found tree lines

              /*! ---- 3rd: create the bit patterns for the hits                     ---- */

              /* Region 3 VDC */
              if (region == kRegionID3) {

                dlayer = 0; /* set "number of detectors" to zero */
                QwTreeLine *treelinelist1 = 0, *treelinelist2 = 0;

                // Set hit pattern for this region
                QwDebug << "Setting hit pattern (region 3)" << QwLog::endl;

                /* Loop over the like-pitched planes in a region */
                for (std::vector<QwDetectorInfo*>::iterator iter = detectors.begin();
                    iter != detectors.end(); iter++) {
                  QwDetectorInfo* det = (*iter);

                  // Get plane number
                  int plane = det->GetPlane();
                  // Get number of wires
                  int numwires = det->GetNumberOfElements();

                  // Print detector info
                  if (fDebug) QwOut << *det << QwLog::endl;

                  // If detector is inactive for tracking, skip it
                  if (det->IsInactive()) continue;

                  /// Get the subhitlist of hits in this detector
                  QwHitContainer *subhitlist = hitlist->GetSubList_Plane(region, package, plane);
                  // If no hits in this detector, skip to the next detector.
                  if (subhitlist->size() == 0) {
                    delete subhitlist;
                    continue;
                  }
                  // Print the hit list for this detector
                  if (fDebug) {
                    std::cout << "region: " << region << " package: " << package << " plane: " << plane << std::endl;
                    subhitlist->Print();
                  }

                  // Create a vector of hit patterns
                  std::vector<QwHitPattern> patterns(numwires+1,QwHitPattern()); // wires are counted from 1
                  if (patterns.at(0).GetNumberOfBins() == 0)
                    for (size_t wire = 0; wire < patterns.size(); wire++)
                      patterns.at(wire).SetNumberOfLevels(fLevelsR3);

                  // Loop over the hits in the subhitlist
                  for (QwHitContainer::iterator hit  = subhitlist->begin();
                                                hit != subhitlist->end();
                                                hit++) {

                    // Construct the hit pattern for this set of hits
                    QwHitPattern hitpattern(fLevelsR3);
                    hitpattern.SetVDCHit(searchtree->GetWidth(), &(*hit));
                    // Add hit pattern to the vector
                    int wire = hit->GetElement();
                    patterns.at(wire) += hitpattern;

                  } // end of loop over hits in this event

                  // Print hit pattern, if requested
                  if (fShowEventPattern) {
                    std::cout << "event pattern: " << std::endl;
                    for (size_t wire = 0; wire < patterns.size(); wire++)
                      if (patterns.at(wire).HasHits())
                        QwMessage << wire << ":" << patterns.at(wire) << QwLog::endl;
                  }

                  // Copy the new hit patterns into the old array structure
                  // TODO This is temporary
                  char** channel = new char*[patterns.size()];
                  int**  hashchannel = new int*[patterns.size()];
                  for (size_t wire = 0; wire < patterns.size(); wire++) {
                    channel[wire] = new char[patterns.at(wire).GetNumberOfBins()];
                    hashchannel[wire] = new int[patterns.at(wire).GetFinestBinWidth()];
                    patterns.at(wire).GetPattern(channel[wire]);
                    patterns.at(wire).GetPatternHash(hashchannel[wire]);
                  }

                  // All hits in this detector (VDC) are added to the bit pattern.
                  // We can start the tree search now.
                  // NOTE Somewhere around here a memory leak lurks
                  QwDebug << "Searching for matching patterns (direction " << dir << ")" << QwLog::endl;
                  treelinelist = fTreeSearch->SearchTreeLines(searchtree,
                      channel, hashchannel, fLevelsR3,
                      numwires, MAX_LAYERS);

                  // Delete the old array structures
                  for (size_t wire = 0; wire < patterns.size(); wire++) {
                    delete[] channel[wire];
                    delete[] hashchannel[wire];
                  }
                  delete[] channel;
                  delete[] hashchannel;

                  // TODO These treelines should contain the region id etc
                  // We should set the QwDetectorInfo link here already,
                  // or manually set the QwDetectorID fields.  Also, we
                  // should put QwDetectorInfo in the searchtree object.
                  for (QwTreeLine* treeline = treelinelist;
                      treeline; treeline = treeline->next) {
                    treeline->SetRegion(region);
                    treeline->SetPackage(package);
                    treeline->SetDirection(dir);
                    treeline->SetPlane(plane);
                  }

                  // Print list of tree lines
                  if (fDebug) {
                    cout << "List of treelines:" << endl;
                    if (treelinelist) treelinelist->Print();
                  }

                  QwDebug << "Calculate chi^2" << QwLog::endl;
                  double width = searchtree->GetWidth();
                  fTreeCombine->TlTreeLineSort(treelinelist, subhitlist,
                      package, region, dir,
                      1UL << (fLevelsR3 - 1), 0, dlayer, width);

                  QwDebug << "Sort patterns" << QwLog::endl;
                  fTreeSort->rcTreeConnSort (treelinelist, region);

                  if (plane < 3)
                    treelinelist1 = treelinelist;
                  else
                    treelinelist2 = treelinelist;

                  dlayer++;

                  // Delete subhitlist
                  delete subhitlist;

                } // end of loop over like-pitched planes in a region
                event->AddTreeLineList(treelinelist1);
                event->AddTreeLineList(treelinelist2);
                treelinelist = 0;
                // treelinelist 1 and treelinelist 2 are in the same dir
                QwDebug << "Matching region 3 segments" << QwLog::endl;
                if (treelinelist1 && treelinelist2) {
                  treelinelist = fTreeMatch->MatchRegion3 (treelinelist1, treelinelist2);
                  event->fTreeLine[package][region][dir] = treelinelist;
                  event->AddTreeLineList(treelinelist);

                  if (fDebug) {
                    cout << "VDC1:" << endl;
                    if (treelinelist1) treelinelist1->Print();
                    cout << "VDC2:" << endl;
                    if (treelinelist2) treelinelist2->Print();
                  }
                  if (fDebug) {
                    cout << "VDC1+2:" << endl;
                    if (treelinelist) treelinelist->Print();
                  }

                }

                // Delete tree line lists after storing results in event structure
                QwTreeLine* tl = 0;
                QwTreeLine* tl_next = 0;
                tl = treelinelist1;
                while (tl) { tl_next = tl->next; delete tl; tl = tl_next; }
                tl = treelinelist2;
                while (tl) { tl_next = tl->next; delete tl; tl = tl_next; }

                tlayers = MAX_LAYERS;  /* remember the number of tree-detector */


                /* Region 2 HDC */
              } else if (region == kRegionID2) {

                // Set hit pattern for this region
                QwDebug << "Setting hit pattern (region 2)" << QwLog::endl;

                // Create a vector of hit patterns
                std::vector<QwHitPattern> patterns;

                // Get the hit list for this package/region/direction
                QwHitContainer *treelinehits = new QwHitContainer();

                /* Loop over the like-pitched planes in a region */
                tlayers = 0;
                for (std::vector<QwDetectorInfo*>::iterator iter = detectors.begin();
                    iter != detectors.end(); iter++, tlayers++) {
                  QwDetectorInfo* det = (*iter);

                  // Get plane number
                  Int_t plane = det->GetPlane();

                  // Print detector info
                  if (fDebug) QwOut << plane << ": " << *det << QwLog::endl;

                  // Get the subhitlist of hits in this detector plane
                  QwHitContainer *planehits = hitlist->GetSubList_Plane(region, package, plane);
                  treelinehits->Append(planehits);
                  if (fDebug) planehits->Print();
                  // Construct the hit pattern for this set of hits
                  QwHitPattern hitpattern(fLevelsR2);
                  // Set the detector identification
                  hitpattern.SetRegion(region);
                  hitpattern.SetPackage(package);
                  hitpattern.SetDirection(dir);
                  hitpattern.SetPlane(plane);
                  // Set the hit pattern
                  hitpattern.SetHDCHitList(searchtree->GetWidth(), planehits);
                  // Add hit pattern to the vector
                  patterns.push_back(hitpattern);

                  // Delete subhitlist
                  delete planehits;

                } // end of loop over like-pitched planes in a region

                // Print hit pattern, if requested
                if (fShowEventPattern)
                  for (size_t layer = 0; layer < patterns.size(); layer++)
                    QwMessage << patterns.at(layer) << QwLog::endl;

                // Copy the new hit patterns into the old array structure
                // TODO This is temporary
                int levels = patterns.at(0).GetNumberOfLevels();
                char** channel = new char*[patterns.size()];
                int**  hashchannel = new int*[patterns.size()];
                for (size_t layer = 0; layer < patterns.size(); layer++) {
                  channel[layer] = new char[patterns.at(layer).GetNumberOfBins()];
                  hashchannel[layer] = new int[patterns.at(layer).GetFinestBinWidth()];
                  patterns.at(layer).GetPattern(channel[layer]);
                  patterns.at(layer).GetPatternHash(hashchannel[layer]);
                }

                QwDebug << "Search for matching patterns (direction " << dir << ")" << QwLog::endl;
                treelinelist = fTreeSearch->SearchTreeLines(searchtree,
                    channel, hashchannel, levels,
                    0, tlayers);

                // Delete the old array structures
                for (size_t layer = 0; layer < patterns.size(); layer++) {
                  delete[] channel[layer];
                  delete[] hashchannel[layer];
                }
                delete[] channel;
                delete[] hashchannel;

                // TODO These treelines should contain the region id etc
                // We should set the QwDetectorInfo link here already,
                // or manually set the QwDetectorID fields.  Also, we
                // should put QwDetectorInfo in the searchtree object.
                for (QwTreeLine* treeline = treelinelist;
                    treeline; treeline = treeline->next) {
                  treeline->SetRegion(region);
                  treeline->SetPackage(package);
                  treeline->SetDirection(dir);
                }

                QwDebug << "Calculate chi^2" << QwLog::endl;
                if (searchtree) {

                  double width = searchtree->GetWidth();
                  fTreeCombine->TlTreeLineSort (treelinelist, treelinehits,
                      package, region, dir,
                      1UL << (fLevelsR2 - 1),
                      tlayers, 0, width);
                }

                //QwDebug << "Sort patterns" << QwLog::endl;
                //fTreeSort->rcTreeConnSort (treelinelist, region);

                if (fDebug) {
                  cout << "List of treelines:" << endl;
                  if (treelinelist) treelinelist->Print();
                }
                event->fTreeLine[package][region][dir] = treelinelist;
                event->AddTreeLineList(treelinelist);

                // Delete treelinehits
                delete treelinehits;

                /* Any other region */
              } else {
                QwWarning << "[QwTrackingWorker::ProcessHits] Warning: no support for this detector." << QwLog::endl;
                return;
              }


            } // end of loop over the three wire-pitch directions


            /*! ---- TASK 2: Combine the treelines into partial tracks             ---- */

            // List of partial tracks to return
            std::vector<QwPartialTrack*> parttracklist;

            if (event->GetNumberOfTreeLines() > 0 && tlayers)
            {
              std::vector<QwTreeLine*> treelines_x =
                  event->fTreeLine[package][region][kDirectionX]->GetListAsVector();
              std::vector<QwTreeLine*> treelines_u =
                  event->fTreeLine[package][region][kDirectionU]->GetListAsVector();
              std::vector<QwTreeLine*> treelines_v =
                  event->fTreeLine[package][region][kDirectionV]->GetListAsVector();

              parttracklist = fTreeCombine->TlTreeCombine(
                  treelines_x, treelines_u, treelines_v,
                  package, region,
                  tlayers, dlayer);
            } else continue;


            // If we found partial tracks in this event
            if (parttracklist.size() > 0) {

              /*! ---- TASK 3: Sort out the Partial Tracks                          ---- */

              fTreeSort->rcPartConnSort(parttracklist);


              /*! ---- TASK 4: Hook up the partial track info to the event info     ---- */

              // Add partial tracks to the event
              event->AddPartialTrackList(parttracklist);

              // Debug output
              if (fDebug) {
                QwOut << "List of partial tracks:" << QwLog::endl;
                for (size_t pt = 0; pt < parttracklist.size(); pt++)
                  QwOut << *parttracklist[pt] << QwLog::endl;
              }

              // Delete partial tracks
              for (size_t pt = 0; pt < parttracklist.size(); pt++)
                delete parttracklist[pt];


              // Count this as a good event
              QwVerbose << "Found a good partial track in region " << region << QwLog::endl;
              if (region == 2) ++R2Good;
              if (region == 3) ++R3Good;
              ngood++;

            } else {

              // Count this as a bad event
              QwVerbose << "Couldn't find a good partial track in region " << region << QwLog::endl;
              if (region == 2) ++R2Bad;
              if (region == 3) ++R3Bad;
              nbad++;
            }

        } /* end of loop over the regions */

    } /* end of loop over the detector packages */

    // Delete local copy of the hit list
    delete hitlist;


    /* ==============================
     * Correlate front and back
     * tracks from x, y and y' infor-
     * mation
     * ============================== */

    /// If momentum reconstruction is disabled
    if (fDisableMomentum) return;

    // Loop over packages in region 3
    for (EQwDetectorPackage R3package = kPackage1;
        R3package <= kPackage2; R3package++) {

      // Determine package in region 2 (considering possibility of reversed run)
      EQwDetectorPackage R2package;
      if (fMismatchPkg)
        R2package = (R3package == kPackage1)? kPackage2: kPackage1;
      else
        R2package = (R3package == kPackage1)? kPackage1: kPackage2;

      // Get the lists of partial tracks in the front and back detectors
      std::vector<QwPartialTrack*> frontlist = event->GetListOfPartialTracks(kRegionID2,R2package);
      std::vector<QwPartialTrack*> backlist  = event->GetListOfPartialTracks(kRegionID3,R3package);

      // Loop over all good front and back partial tracks
      for (size_t ifront = 0; ifront < frontlist.size(); ifront++) {
        QwPartialTrack* front = frontlist[ifront];

        for (size_t iback = 0; iback < backlist.size(); iback++) {
          QwPartialTrack* back = backlist[iback];

          // Filter reasonable pairs
          int status = fBridgingTrackFilter->Filter(front, back);
          status = 0;
          if (status != 0) {
            QwMessage << "Tracks did not pass filter." << QwLog::endl;
            continue;
          }

          // Attempt to bridge tracks using ray-tracing
          if (! fDisableRayTracer) {
            const QwTrack* track = fRayTracer->Bridge(front, back);
            if (track) {
              event->AddTrack(track);
              delete track;
            }
          }

        } // end of loop over back tracks

      } // end of loop over front tracks

    } // end of loop over packages


    // Calculate kinematics
    int num_of_bridged = event->GetNumberOfTracks();
    if ( num_of_bridged > 0) {
      nbridged += num_of_bridged;
      event->CalculateKinematics(event->GetTrack(0));
    }

    // Print the result
    if (fDebug) event->Print();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
