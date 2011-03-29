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
#include "Det.h"

// Qweak GEM cluster finding
#include "QwGEMClusterFinder.h"

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
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwEvent.h"
#include "QwBridge.h"
#include "QwDetectorInfo.h"
#include "QwBridgingTrackFilter.h"
#include "QwRayTracer.h"
#include "QwMatrixLookup.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingWorker::QwTrackingWorker (const char* name)
{
  QwDebug << "###### Calling QwTrackingWorker::QwTrackingWorker ()" << QwLog::endl;

  // Debug level
  fDebug = 0;

  // Process options
  ProcessOptions(gQwOptions);

  // If tracking is disabled, stop here
  if (fDisableTracking) return;

  // Initialize the pattern database
  InitTree();

  // Initialize a bridging filter
  fBridgingTrackFilter = new QwBridgingTrackFilter();

  // Initialize a lookup table bridging method
  if (! fDisableMomentum && ! fDisableMatrixLookup) {
    fMatrixLookup = new QwMatrixLookup();
    // Determine lookup table file from environment variables
    std::string trajmatrix = "";
    if (getenv("QW_LOOKUP"))
      trajmatrix = std::string(getenv("QW_LOOKUP")) + "/" + fFilenameLookupTable;
    else {
      QwWarning << "Environment variable QW_LOOKUP not defined." << QwLog::endl;
      QwWarning << "It should point to the directory with the file"
                << fFilenameLookupTable << QwLog::endl;
    }
    // Load lookup table
    if (! fMatrixLookup->LoadTrajMatrix(trajmatrix))
      QwError << "Could not load trajectory lookup table!" << QwLog::endl;
  // or set to null if disabled
  } else fMatrixLookup = 0;

  // Initialize a ray tracer bridging method
  if (! fDisableMomentum && ! fDisableRayTracer) {
    fRayTracer = new QwRayTracer();
    // Determine magnetic field file from environment variables
    std::string fieldmap = "";
    if (getenv("QW_FIELDMAP"))
      fieldmap = std::string(getenv("QW_FIELDMAP")) + "/" + fFilenameFieldmap;
    else {
      QwWarning << "Environment variable QW_FIELDMAP not defined." << QwLog::endl;
      QwWarning << "It should point to the directory with the file"
                << fFilenameFieldmap << QwLog::endl;
    }
    // Load magnetic field map
    if (! fRayTracer->LoadMagneticFieldMap(fieldmap))
      QwError << "Could not load magnetic field map!" << QwLog::endl;
  // or set to null if disabled
  } else fRayTracer = 0;

  /* Reset counters of number of good and bad events */
  ngood = 0;
  nbad = 0;

  R2Good = 0;
  R2Bad = 0;
  R3Good = 0;
  R3Bad = 0;


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

  // Process the options and set the respective flags in the modules
  fTreeSearch->SetShowMatchingPatterns(fShowMatchingPattern);
  fTreeCombine->SetMaxRoad(gQwOptions.GetValue<float>("QwTracking.R2.maxroad"));
  fTreeCombine->SetMaxXRoad(gQwOptions.GetValue<float>("QwTracking.R2.maxxroad"));

  QwDebug << "###### Leaving QwTrackingWorker::QwTrackingWorker ()" << QwLog::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingWorker::~QwTrackingWorker ()
{
  // TODO This causes a segmentation fault in ROOT's memory management
  for (int i = 0; i < kNumPackages * kNumRegions * kNumTypes * kNumDirections; i++)
    if (fSearchTree[i]) delete fSearchTree[i];

  if (fBridgingTrackFilter) delete fBridgingTrackFilter;
  if (fMatrixLookup) delete fMatrixLookup;
  if (fRayTracer)    delete fRayTracer;

  // Delete tracking modules
  if (fTreeSearch)  delete fTreeSearch;
  if (fTreeCombine) delete fTreeCombine;
  if (fTreeSort)    delete fTreeSort;
  if (fTreeMatch)   delete fTreeMatch;
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
  options.AddOptions("Tracking options")("QwTracking.showeventpattern",
                          po::value<bool>()->default_bool_value(false),
                          "show bit pattern for all events");
  options.AddOptions("Tracking options")("QwTracking.showmatchingpattern",
                          po::value<bool>()->default_bool_value(false),
                          "show bit pattern for matching tracks");

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

  // Region 3
  options.AddOptions("Tracking options")("QwTracking.R3.levels",
                          po::value<int>()->default_value(4),
                          "number of search tree levels in region 3");
  options.AddOptions("Tracking options")("QwTracking.R3.MaxMissedWires",
                          po::value<int>()->default_value(4),
                          "maximum number of missed wires");

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
  options.AddOptions("Tracking options")("QwTracking.fieldmap",
                          po::value<std::string>()->default_value("peiqing_2007.dat"),
                          "filename of the fieldmap file in QW_FIELDMAP");
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

  // Set the flags for showing the matching event patterns
  fShowEventPattern    = options.GetValue<bool>("QwTracking.showeventpattern");
  fShowMatchingPattern = options.GetValue<bool>("QwTracking.showmatchingpattern");

  // Enable/disable the lookup table and raytracer momentum reconstruction methods
  fDisableMatrixLookup = options.GetValue<bool>("QwTracking.disable-matrixlookup");
  fDisableRayTracer    = options.GetValue<bool>("QwTracking.disable-raytracer");

  // Fieldmap and lookup table filenames
  fFilenameFieldmap    = options.GetValue<std::string>("QwTracking.fieldmap");
  fFilenameLookupTable = options.GetValue<std::string>("QwTracking.lookuptable");

  // Number of levels (search tree depth) for region 2
  fLevelsR2 = options.GetValue<int>("QwTracking.R2.levels");
  // Number of levels (search tree depth) for region 3
  fLevelsR3 = options.GetValue<int>("QwTracking.R3.levels");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingWorker::InitTree()
{
  /// For each region (1, 2, 3, trigger, cerenkov, scanner)
  for (EQwRegionID region  = kRegionID1;
                   region <= kRegionID3; region++) {

    // TODO Skip region 1 for now
    if (region < kRegionID2) continue;

    /// ... and for each package
    for (EQwDetectorPackage package  = kPackageUp;
                            package <= kPackageDown; package++) {

      // TODO fDebug Skip everything except up and down for now...
      if (package != kPackageUp && package != kPackageDown) continue;

      /// ... and for each detector type
      for (EQwDetectorType type  = kTypeDriftHDC;
                           type <= kTypeDriftVDC; type++) {

        /// In region 2 there are only HDCs
        if (region == kRegionID2 && type != kTypeDriftHDC) continue;

        /// In region 3 there are only VDCs
        if (region == kRegionID3 && type != kTypeDriftVDC) continue;


        /// ... and for each wire direction (X, Y, U, V, R, theta)
        for (EQwDirectionID direction  = kDirectionX;
                            direction <= kDirectionV; direction++) {

          int levels = 0;
          int numlayers = 0;
          double width = 0.0;

          // Skip wire direction Y
          if (direction == kDirectionY) continue;

          // Skip wire direction X for region 3
          if (direction == kDirectionX && region == kRegionID3) continue;

          ///  Skip the NULL rcDETRegion pointers.
          //   pking:  This is probably a configuration error,
          //           which the user may want to be warned about.
          //   wdc:   If those pointers would be initialized to null,
          //          this test would be a lot more useful --> another TODO
          if (rcDETRegion[package][region][direction] == NULL) {
            QwWarning << "rcDETRegion["<< package
                      << "]["          << region
                      << "]["          << direction
                      << "] is NULL.  Should it be?"
                      << QwLog::endl;
            continue;
          }

          /// Region 2 contains 4 layers
          if (region == kRegionID2) {
              numlayers = 4; // TODO replace this with info from the geometry file
              width = rcDETRegion[package][region][direction]->WireSpacing *
                      rcDETRegion[package][region][direction]->NumOfWires;
              levels = fLevelsR2;
          }

          /// Region 3 contains 8 layers
          if (region == kRegionID3) {
              numlayers = 8; // TODO replace this with info from the geometry file
              width = rcDETRegion[package][region][direction]->width[2];
              levels = fLevelsR3;
          }

          /// Create a new search tree
          QwTrackingTree *thetree = new QwTrackingTree(numlayers);
          thetree->SetMaxSlope(gQwOptions.GetValue<float>("QwTracking.R2.maxslope"));

          /// Set up the filename with the following format
          ///   tree[numlayers]-[levels]-[u|l]-[1|2|3]-[d|g|t|c]-[n|u|v|x|y].tre
          std::stringstream filename;
          filename << getenv_safe_string("QW_SEARCHTREE")
                       << "/tree" << numlayers
                       << "-" << levels
                       << "-" << "0ud"[package]
                       << "-" << "0123TCS"[region]
                       << "-" << "0hvgtc"[type]
                       << "-" << "0xyuvrq"[direction] << ".tre";

          QwDebug << "Tree filename: " << filename.str() << QwLog::endl;

          /// Each element of fSearchTree will point to a pattern database
          int index = package*kNumRegions*kNumTypes*kNumDirections
                      +region*kNumTypes*kNumDirections
                      +type*kNumDirections+direction;
          fSearchTree[index] = thetree->inittree(filename.str(),
                                                 levels,
                                                 numlayers,
                                                 width,
                                                 package,
                                                 type,
                                                 region,
                                                 direction,
                                                 fRegenerate);

          // Set the detector identification
          fSearchTree[index]->SetRegion(region);
          fSearchTree[index]->SetPackage(package);
          fSearchTree[index]->SetDirection(direction);
          // Plane unknown, element not applicable: keep them at null values

          // Delete the tree object
          delete thetree;

        } // end of loop over wire directions

      } // end of loop over detector types

    } // end of loop over packages

  } // end of loop over regions

}

/*------------------------------------------------------------------------*//*!

  ProcessHits() - this function is the main tracking function. It
               performes tracking in projections (u/v/x) to form treelines,
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
    for (EQwDetectorPackage package  = kPackageUp;
            package <= kPackageDown; package++) {
        QwDebug << "[QwTrackingWorker::ProcessHits] Package: " << package << QwLog::endl;

        // Currently assume that only the up and down octants contain the tracking
        // detectors.  When rotation is included, this will have to be modified to
        // take into account that the tracking detectors are in different octants.
        if (package != kPackageUp && package != kPackageDown) continue;

        /// Find the region 1 clusters in this package
        QwHitContainer *hitlist_region1_r = hitlist->GetSubList_Plane(kRegionID1, package, 1);
        QwHitContainer *hitlist_region1_phi = hitlist->GetSubList_Plane(kRegionID1, package, 2);
        QwGEMClusterFinder* clusterfinder = new QwGEMClusterFinder();
        std::vector<QwGEMCluster> clusters_r;
        std::vector<QwGEMCluster> clusters_phi;
        if (hitlist_region1_r->size() > 0) {
            clusters_r = clusterfinder->FindClusters(hitlist_region1_r);
        }
        if (hitlist_region1_phi->size() > 0) {
            clusters_phi = clusterfinder->FindClusters(hitlist_region1_phi);
        }
        for (std::vector<QwGEMCluster>::iterator cluster = clusters_r.begin();
                cluster != clusters_r.end(); cluster++) {
            QwDebug << *cluster << QwLog::endl;
        }
        for (std::vector<QwGEMCluster>::iterator cluster = clusters_phi.begin();
                cluster != clusters_phi.end(); cluster++) {
            QwDebug << *cluster << QwLog::endl;
        }
        delete clusterfinder; // TODO (wdc) should go somewhere else
        delete hitlist_region1_r;
        delete hitlist_region1_phi;

        /// Loop through the detector regions
        for (EQwRegionID region  = kRegionID2;
                region <= kRegionID3; region++) {
            QwDebug << "[QwTrackingWorker::ProcessHits]  Region: " << region << QwLog::endl;


            /// Loop over the detector types (only drift chambers are used)
            for (EQwDetectorType type  = kTypeDriftHDC;
                    type <= kTypeDriftVDC; type++) {
                QwDebug << "[QwTrackingWorker::ProcessHits]   Detector: " << type << QwLog::endl;


                int dlayer = 0; // number of detector planes in the search
                int tlayers = 0; // number of tracking layers

                /// Loop through the detector directions
                for (EQwDirectionID dir  = kDirectionX;
                        dir <= kDirectionV; dir++) {
                    QwDebug << "[QwTrackingWorker::ProcessHits]    Direction: " << dir << QwLog::endl;

                    // Skip wire direction Y for region 2
                    if (region == kRegionID2
                            && dir == kDirectionY) continue;
                    // Skip wire direction X and Y for region 3
                    if (region == kRegionID3
                            && (dir == kDirectionX || dir == kDirectionY)) continue;

                    // Check whether there are any detectors defined in that geometry
                    if (! rcDETRegion[package][region][dir]) {
                        QwDebug << "[QwTrackingWorker::ProcessHits]     No such detector!" << QwLog::endl;
                        continue;
                    }

                    /*! ---- 1st: check that the direction is tree-searchable               ---- */

                    // The search tree for this detector will be stored in searchtree
                    QwTrackingTreeRegion* searchtree
                    = fSearchTree[package*kNumRegions*kNumTypes*kNumDirections
                                  + region*kNumTypes*kNumDirections
                                  + type*kNumDirections
                                  + dir];

                    // Check whether the search tree exists
                    if (! searchtree) {
                        QwDebug << "[QwTrackingWorker::ProcessHits]     No such tree!" << QwLog::endl;
                        continue;
                    }

                    // Check whether the search tree is searchable
                    if (! searchtree->IsSearchable()) {
                        event->fTreeLine[package][region][type][dir] = 0;
                        QwDebug << "[QwTrackingWorker::ProcessHits]     Search tree not searchable!" << QwLog::endl;
                        continue;
                    }


                    /*! ---- 2nd: do some variable initialization for the found tree line
                                  linked list                                              ---- */

                    // Start the search for this set of like-pitched planes
                    QwTrackingTreeLine* treelinelist = 0; // local list of found tree lines

                    /*! ---- 3rd: create the bit patterns for the hits                     ---- */

                    /* Region 3 VDC */
                    if (region == kRegionID3 && type == kTypeDriftVDC) {

                        dlayer = 0; /* set "number of detectors" to zero */
                        QwTrackingTreeLine *treelinelist1 = 0, *treelinelist2 = 0;

                        // Set hit pattern for this region
                        QwDebug << "Setting hit pattern (region 3)" << QwLog::endl;

                        /* Loop over the like-pitched planes in a region */
                        for (Det* rd = rcDETRegion[package][region][dir];
                                rd; rd = rd->nextsame) {

                            // Get plane number
                            Int_t plane = rd->plane;

                            // Print detector info
                            //if (fDebug) cout << "      ";
                            //if (fDebug) rd->print();

                            // If detector is inactive for tracking, skip it
                            if (rd->IsInactive()) continue;

                            // Create a vector of hit patterns
                            std::vector<QwHitPattern> patterns;
                            patterns.resize(NUMWIRESR3+1); // wires are counted from 1
                            if (patterns.at(0).GetNumberOfBins() == 0)
                              for (size_t wire = 0; wire < patterns.size(); wire++)
                                patterns.at(wire).SetNumberOfLevels(fLevelsR3);


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
                            // Loop over the hits in the subhitlist
                            for (QwHitContainer::iterator hit = subhitlist->begin();
                                    hit != subhitlist->end(); hit++) {

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
                                                 NUMWIRESR3, MAX_LAYERS);

                            // Delete the old array structures
                            for (size_t wire = 0; wire < patterns.size(); wire++) {
                              delete[] channel[wire];
                              delete[] hashchannel[wire];
                            }
                            delete channel;
                            delete hashchannel;

                            // TODO These treelines should contain the region id etc
                            // We should set the QwDetectorInfo link here already,
                            // or manually set the QwDetectorID fields.  Also, we
                            // should put QwDetectorInfo in the searchtree object.
                            for (QwTrackingTreeLine* treeline = treelinelist;
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
                            event->fTreeLine[package][region][type][dir] = treelinelist;
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
                        QwTrackingTreeLine* tl = 0;
                        QwTrackingTreeLine* tl_next = 0;
                        tl = treelinelist1;
                        while (tl) { tl_next = tl->next; delete tl; tl = tl_next; }
                        tl = treelinelist2;
                        while (tl) { tl_next = tl->next; delete tl; tl = tl_next; }

                        tlayers = MAX_LAYERS;  /* remember the number of tree-detector */


                    /* Region 2 HDC */
                    } else if (region == kRegionID2 && type == kTypeDriftHDC) {

                        // Set hit pattern for this region
                        QwDebug << "Setting hit pattern (region 2)" << QwLog::endl;

                        // Create a vector of hit patterns
                        std::vector<QwHitPattern> patterns;

                        /* Loop over the like-pitched planes in a region */
                        tlayers = 0;
                        for (Det* rd = rcDETRegion[package][region][dir];
                                rd; rd = rd->nextsame, tlayers++) {

                            // Get plane number
                            Int_t plane = rd->plane;

                            // Print detector info
                            //if (fDebug) rd->print();

                            // If detector is inactive for tracking, skip it
                            if (rd->IsInactive()) {
                              patterns.push_back(QwHitPattern(fLevelsR2));
                              continue;
                            }

                            // Get the subhitlist of hits in this detector plane
                            QwHitContainer *subhitlist = hitlist->GetSubList_Plane(region, package, plane);
                            if (fDebug) subhitlist->Print();

                            // Construct the hit pattern for this set of hits
                            QwHitPattern hitpattern(fLevelsR2);
                            // Set the detector identification
                            hitpattern.SetRegion(region);
                            hitpattern.SetPackage(package);
                            hitpattern.SetDirection(dir);
                            hitpattern.SetPlane(plane);
                            // Set the hit pattern
                            hitpattern.SetHDCHitList(searchtree->GetWidth(), subhitlist);
                            // Add hit pattern to the vector
                            patterns.push_back(hitpattern);

                            // Delete subhitlist
                            delete subhitlist;

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
                        delete channel;
                        delete hashchannel;

                        // TODO These treelines should contain the region id etc
                        // We should set the QwDetectorInfo link here already,
                        // or manually set the QwDetectorID fields.  Also, we
                        // should put QwDetectorInfo in the searchtree object.
                        for (QwTrackingTreeLine* treeline = treelinelist;
                             treeline; treeline = treeline->next) {
                          treeline->SetRegion(region);
                          treeline->SetPackage(package);
                          treeline->SetDirection(dir);
                        }

                        // Print the list of tree lines
                        if (fDebug) {
                            cout << "List of treelines:" << endl;
                            if (treelinelist) treelinelist->Print();
                        }

                        // Get the hit list for this package/region/direction
                        QwHitContainer *subhitlist = hitlist->GetSubList_Dir(region, package, dir);
                        if (fDebug) subhitlist->Print();

                        QwDebug << "Calculate chi^2" << QwLog::endl;
                        if (searchtree) {

                            double width = searchtree->GetWidth();
                            fTreeCombine->TlTreeLineSort (treelinelist, subhitlist,
                                                         package, region, dir,
                                                         1UL << (fLevelsR2 - 1),
                                                         tlayers, 0, width);
                        }

                        QwDebug << "Sort patterns" << QwLog::endl;
                        fTreeSort->rcTreeConnSort (treelinelist, region);

                        if (fDebug) {
                            cout << "List of treelines:" << endl;
                            if (treelinelist) treelinelist->Print();
                        }
                        event->fTreeLine[package][region][type][dir] = treelinelist;
                        event->AddTreeLineList(treelinelist);

                        // Delete subhitlist
                        delete subhitlist;

                        /* Any other region */
                    } else {
                        QwWarning << "[QwTrackingWorker::ProcessHits] Warning: no support for this detector." << QwLog::endl;
                        return;
                    }


                } // end of loop over the three wire-pitch directions


                /*! ---- TASK 2: Combine the treelines into partial tracks             ---- */

                QwPartialTrack* parttrack = 0; // list of partial tracks



                // This if statement may be done wrong
                // TODO (wdc) why does this have last index dir instead of something in scope?
                if (region == kRegionID3) {

                    if (event->fTreeLine[package][region][type][kDirectionU]
                     && event->fTreeLine[package][region][type][kDirectionV]
                     && tlayers) {
                        parttrack = fTreeCombine->TlTreeCombine(
                                    event->fTreeLine[package][region][type],
                                    package, region,
                                    tlayers,
                                    dlayer,
                                    fSearchTree);
                    }

                } else if(region == kRegionID2){

                    if (event->fTreeLine[package][region][type][kDirectionU]
                     && event->fTreeLine[package][region][type][kDirectionV]
                     && event->fTreeLine[package][region][type][kDirectionX]
                     && tlayers) {
                        parttrack = fTreeCombine->TlTreeCombine(
                                    event->fTreeLine[package][region][type],
                                    package, region,
                                    tlayers,
                                    dlayer,
                                    fSearchTree);
                    }

                } else continue;




                /*! ---- TASK 3: Sort out the Partial Tracks                          ---- */

                if (parttrack) fTreeSort->rcPartConnSort(parttrack);


                /*! ---- TASK 4: Hook up the partial track info to the event info     ---- */


                if (parttrack) {
                        event->fPartialTrack[package][region][type] = parttrack;
                        event->AddPartialTrackList(parttrack);
                }


                if (parttrack) {
                    if (fDebug) parttrack->Print();
                    QwVerbose << "Found a good partial track in region " << region << QwLog::endl;
                    ngood++;
                    if (region==2) R2Good++;
                    if (region==3) R3Good++;
                } else {
                    QwVerbose << "Couldn't find a good partial track in region " << region << QwLog::endl;
                    if (region ==2) R2Bad++;
                    if (region ==3) R3Bad++;
                    nbad++;
                }

            } /* end of loop over the detector types */

        } /* end of loop over the regions */


        /* ==============================
        * Correlate front and back
        * tracks from x, y and y' infor-
        * mation
        * ============================== */

        // If there were partial tracks in the HDC and VDC regions

        if (! fDisableMomentum
         && event->fPartialTrack[package][kRegionID2][kTypeDriftHDC]
         && event->fPartialTrack[package][kRegionID3][kTypeDriftVDC]) {

            QwMessage << "Bridging front and back partial tracks..." << QwLog::endl;

            // Local copies of front and back track
            QwPartialTrack* front = event->fPartialTrack[package][kRegionID2][kTypeDriftHDC];
            QwPartialTrack* back  = event->fPartialTrack[package][kRegionID3][kTypeDriftVDC];

            // Loop over all good front and back partial tracks
            while (front) {
              while (back) {

                int status = 0;

                // Filter reasonable pairs
                status = fBridgingTrackFilter->Filter(front, back);
                QwMessage << "Filter: " << status << QwLog::endl;
                if (status != 0) {
                  QwMessage << "Tracks did not pass filter." << QwLog::endl;
                  back = back->next;
                  continue;
                }

                // Attempt to bridge tracks using lookup table
                if (! fDisableMatrixLookup) {
                  status = fMatrixLookup->Bridge(front, back);
                  QwMessage << "Matrix lookup: " << status << QwLog::endl;
                  if (status == 0) {
                    event->AddTrackList(fMatrixLookup->GetListOfTracks());
                    back = back->next;
                    continue;
                  }
                }

                // Attempt to bridge tracks using ray-tracing
                if (! fDisableRayTracer) {
                  status = fRayTracer->Bridge(front, back);
                  QwMessage << "Ray tracer: " << status << QwLog::endl;
                  if (status == 0) {
                    event->AddTrackList(fRayTracer->GetListOfTracks());
                    //fRayTracer->PrintInfo();
                    double buff[14];
                    fRayTracer->GetBridgingResult(buff);
                    event->AddBridgingResult(buff);
                    //event->Print();
                    back = back->next;
                    continue;
                  }
                }

                // Next back track
                back = back->next;

              } // end of loop over back tracks

              // Next front track
              front = front->next;

            } // end of loop over front tracks

        } /* end of */

    } /* end of loop over the detector packages */


//   if (fDebug)
//     cout<<"R2Good, R2Bad, R3Good, R3Bad: "<<R2Good<<" "<<R2Bad<<" "<<R3Good<<" "<<R3Bad<<endl;
//     cout<<"Efficiency:     region 2  "<<float(R2Good)/(R2Good+R2Bad)*100.0
//         <<"%,     region 3  "<<float(R3Good)/(R3Good+R3Bad)*100.0<<"%"<<endl;


    // Delete local objects
    delete hitlist;

    // Print the result
    if (fDebug) event->Print();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
