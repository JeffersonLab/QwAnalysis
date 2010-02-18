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
#include "QwTrajectory.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingWorker::QwTrackingWorker (const char* name)
{
  QwDebug << "###### Calling QwTrackingWorker::QwTrackingWorker ()" << QwLog::endl;

  // Use the options to set local flags
  fShowEventPattern = gQwOptions.GetValue<bool>("QwTracking.showeventpattern");
  fShowMatchingPattern = gQwOptions.GetValue<bool>("QwTracking.showmatchingpattern");

  // Debug level
  fDebug = 0;


  // Number of levels (search tree depth) for region 2
  levelsr2 = gQwOptions.GetValue<int>("QwTracking.R2.levels");

  // Number of levels (search tree depth) for region 3
  levelsr3 = gQwOptions.GetValue<int>("QwTracking.R3.levels");

  // Initialize the pattern database
  InitTree();

  trajectory = new QwTrajectory();

  // Load magnetic field map
  //trajectory->LoadMagneticFieldMap();
  //trajectory->LoadMomentumMatrix();

  /* Reset counters of number of good and bad events */
  ngood = 0;
  nbad = 0;

  R2Good = 0;
  R2Bad = 0;
  R3Good = 0;
  R3Bad = 0;


  QwDebug << "###### Leaving QwTrackingWorker::QwTrackingWorker ()" << QwLog::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingWorker::~QwTrackingWorker ()
{
  // TODO This causes a segmentation fault in ROOT's memory management
  for (int i = 0; i < kNumPackages * kNumRegions * kNumTypes * kNumDirections; i++)
    if (fSearchTree[i]) delete fSearchTree[i];

  delete trajectory;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingWorker::DefineOptions()
{
  // Global options for the tracking worker
  gQwOptions.AddConfigFile("Tracking/prminput/qwtracking.conf");

  // General options
  gQwOptions.AddOptions()("QwTracking.showeventpattern",
                          po::value<bool>()->zero_tokens()->default_value(false),
                          "show bit pattern for all events");
  gQwOptions.AddOptions()("QwTracking.showmatchingpattern",
                          po::value<bool>()->zero_tokens()->default_value(false),
                          "show bit pattern for matching tracks");
  // Region 2
  gQwOptions.AddOptions()("QwTracking.R2.levels",
                          po::value<int>()->default_value(8),
                          "number of search tree levels in region 2");
  gQwOptions.AddOptions()("QwTracking.R2.maxslope",
                          po::value<float>()->default_value(0.862),
                          "maximum allowed slope for region 2 tracks");
  gQwOptions.AddOptions()("QwTracking.R2.maxroad",
                          po::value<float>()->default_value(1.4),
                          "maximum allowed road width for region 2 tracks");
  gQwOptions.AddOptions()("QwTracking.R2.maxxroad",
                          po::value<float>()->default_value(25.0),
                          "maximum allowed X road width for region 2 tracks");
  gQwOptions.AddOptions()("QwTracking.R2.MaxMissedPlanes",
                          po::value<int>()->default_value(1),
                          "maximum number of missed planes");

  // Region 3
  gQwOptions.AddOptions()("QwTracking.R3.levels",
                          po::value<int>()->default_value(4),
                          "number of search tree levels in region 3");
  gQwOptions.AddOptions()("QwTracking.R3.MaxMissedWires",
                          po::value<int>()->default_value(4),
                          "maximum number of missed wires");
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

          // Create a new search tree
          QwTrackingTree *thetree = new QwTrackingTree();
          thetree->SetMaxSlope(gQwOptions.GetValue<float>("QwTracking.R2.maxslope"));

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
              levels = levelsr2;
          }

          /// Region 3 contains 8 layers
          if (region == kRegionID3) {
              numlayers = 8; // TODO replace this with info from the geometry file
              width = rcDETRegion[package][region][direction]->width[2];
              levels = levelsr3;
          }

          /// Set up the filename with the following format
          ///   tree[numlayers]-[levels]-[u|l]-[1|2|3]-[d|g|t|c]-[n|u|v|x|y].tre
          std::stringstream filename;
          filename << "tree" << numlayers
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
                                                 direction);

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

   Bcheck() - this function compares the momentum look-up energy with the
              shooting method result.  This function is for fDebugging
              purposes only.

    inputs: (1) double E       - the momentum lookup energy
            (2) QwPartialTrack *f   - pointer to the front partial track for the
                                 track
            (3) QwPartialTrack *b   - pointer to the back partial track of the
                                 track
            (4) double TVertex - transverse position of the vertex for the
                                 track
            (5) double ZVertex - Z position of the vertex for the track

   outputs: an ASCII ntuple file.

*//*-------------------------------------------------------------------------*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double rcShootP (double Pest,QwPartialTrack *front, QwPartialTrack *back, double accuracy) {
    std::cerr << "Warning: The function rcShootP is only a stub" << std::endl;
    return -1000;
}

void QwTrackingWorker::BCheck (double E, QwPartialTrack *f, QwPartialTrack *b, double TVertex, double ZVertex) {
    double Es = rcShootP(0.0,f,b,0.005);
    //extern physic phys_carlo;
    static FILE *test = 0;

    if ( !test )
        test = fopen("magcheck","w");
    if ( test && E > 0.0) {
        fprintf(test,"%f %f %f %f %f %f\n",
                /*phys_carlo.E,*/E,Es, f->fSlopeX, f->fSlopeY, TVertex, ZVertex);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

  rcLinkUsedTracks() - this function links the track cross-linked list to
                       a single-linked list to ease the access for later
		       functions. The new link is done via the usenext
		       pointer.

    inputs: (1) Track *track -
            (2) int package   -

   returns: the head to the linked list.

*//*-------------------------------------------------------------------------*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrack* QwTrackingWorker::rcLinkUsedTracks (QwTrack *track, int package ) {
    QwTrack *ret =0, *usedwalk = 0;
    QwTrack *trackwalk = NULL, *ytrack = NULL;

    /* loop over all found tracks */
    for (trackwalk = track; trackwalk; trackwalk = trackwalk->next ) {
        /* and the possible y-tracks */
        for (ytrack = trackwalk; ytrack; ytrack = ytrack->ynext ) {
            if (!ytrack->isused) continue;
            if ( !ret ) /* return the first used track */
                ret = usedwalk = ytrack;
            else {
                usedwalk->usednext = ytrack; /* and link them together */
                usedwalk = ytrack;
            }
        }
    }
    delete usedwalk;
    delete trackwalk;
    delete ytrack;
    return ret;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
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

// TODO Should QwHitContainer be passed as const? (wdc)
QwEvent* QwTrackingWorker::ProcessHits (
    QwSubsystemArrayTracking *detectors,
    QwHitContainer *hitlist)
{
    int dlayer = 0;		/* number of detector planes in the search    */

    // Create a new event structure
    QwEvent *event = new QwEvent();
    // and fill it with the hitlist
    event->AddHitContainer(hitlist);

    // Tracking functionality is provided by these four sub-blocks.
    QwTrackingTreeSearch  *TreeSearch  = new QwTrackingTreeSearch();
    TreeSearch->SetDebugLevel(fDebug);
    QwTrackingTreeCombine *TreeCombine = new QwTrackingTreeCombine();
    TreeCombine->SetDebugLevel(fDebug);
    QwTrackingTreeSort    *TreeSort    = new QwTrackingTreeSort();
    TreeSort->SetDebugLevel(fDebug);
    QwTrackingTreeMatch   *TreeMatch   = new QwTrackingTreeMatch();
    TreeMatch->SetDebugLevel(fDebug);

    // Process the options and set the respective flags in the modules
    if (fShowMatchingPattern) TreeSearch->SetShowMatchingPatterns();
    TreeCombine->SetMaxRoad(gQwOptions.GetValue<float>("QwTracking.R2.maxroad"));
    TreeCombine->SetMaxXRoad(gQwOptions.GetValue<float>("QwTracking.R2.maxxroad"));

    /*
    int charge;
    int found_front = 0;
    double ZVertex, bending, theta, phi, ESpec;
    int outbounds = 0;
    */

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

        // TODO (wdc) Also, only tracks in the up octant detector are available in
        // the MC generated hit lists.  Therefore we throw out the down octant.

        // TODO jpan: The Geant4 now can generate any octant data, you just need to command it through
        // a macro file. I'll put the tracking detector ratation commands onto the UI manu later.
        if (package != kPackageUp) continue;

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

        /// Loop through the detector regions
        for (EQwRegionID region  = kRegionID2;
                region <= kRegionID3; region++) {
            QwDebug << "[QwTrackingWorker::ProcessHits]  Region: " << region << QwLog::endl;


            /// Loop over the detector types (only drift chambers are used)
            for (EQwDetectorType type  = kTypeDriftHDC;
                    type <= kTypeDriftVDC; type++) {
                QwDebug << "[QwTrackingWorker::ProcessHits]   Detector: " << type << QwLog::endl;


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
                        event->treeline[package][region][type][dir] = 0;
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
                            if (fDebug) cout << "      ";
                            if (fDebug) rd->print();

                            // If detector is inactive for tracking, skip it
                            if (rd->IsInactive()) continue;

                            // Create a vector of hit patterns
                            std::vector<QwHitPattern> patterns;
                            patterns.resize(NUMWIRESR3);
                            if (patterns.at(0).GetNumberOfBins() == 0)
                              for (size_t wire = 0; wire < patterns.size(); wire++)
                                patterns.at(wire).SetNumberOfLevels(levelsr3);


                            /// Get the subhitlist of hits in this detector
                            QwHitContainer *subhitlist = hitlist->GetSubList_Plane(region, package, plane);
                            if (fDebug) subhitlist->Print();
                            // If no hits in this detector, skip to the next detector.
                            if (! subhitlist) continue;

                            // Loop over the hits in the subhitlist
                            for (QwHitContainer::iterator hit = subhitlist->begin();
                                    hit != subhitlist->end(); hit++) {

                                // Construct the hit pattern for this set of hits
                                QwHitPattern hitpattern(levelsr3);
                                hitpattern.SetVDCHit(searchtree->GetWidth(), &(*hit));
                                // Add hit pattern to the vector
                                int wire = hit->GetElement();
                                patterns.at(wire) += hitpattern;

                            } // end of loop over hits in this event

                            // Print hit pattern, if requested
                            if (fShowEventPattern)
                              for (size_t wire = 0; wire < patterns.size(); wire++)
                                if (patterns.at(wire).HasHits())
                                  QwMessage << wire << ":" << patterns.at(wire) << QwLog::endl;

                            // Copy the new hit patterns into the old array structure
                            // TODO This is temporary
                            char* channel[patterns.size()];
                            int*  hashchannel[patterns.size()];
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
                            treelinelist = TreeSearch->SearchTreeLines(searchtree,
                                                 channel, hashchannel, levelsr3,
                                                 NUMWIRESR3, TLAYERS);

                            // Delete the old array structures
                            for (size_t wire = 0; wire < patterns.size(); wire++) {
                              delete[] channel[wire];
                              delete[] hashchannel[wire];
                            }

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
                                treelinelist->Print();
                            }

                            QwDebug << "Calculate chi^2" << QwLog::endl;
                            double width = searchtree->GetWidth();
                            TreeCombine->TlTreeLineSort (treelinelist, subhitlist,
                                                         package, region, dir,
                                                         1UL << (levelsr3 - 1), 0, dlayer, width);

                            QwDebug << "Sort patterns" << QwLog::endl;
                            TreeSort->rcTreeConnSort (treelinelist, region);

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

                        QwDebug << "Matching region 3 segments" << QwLog::endl;
                        if (treelinelist1 && treelinelist2) {
                            if (fDebug) {
                                cout << "VDC1:" << endl;
                                treelinelist1->Print();
                                cout << "VDC2:" << endl;
                                treelinelist2->Print();
                            }
                            treelinelist = TreeMatch->MatchRegion3 (treelinelist1, treelinelist2);
                        }
                        event->treeline[package][region][type][dir] = treelinelist;
                        event->AddTreeLineList(treelinelist);

                        tlayers = TLAYERS;     /* remember the number of tree-detector */
                        tlaym1  = tlayers - 1; /* remember tlayers - 1 for convenience */



                    /* Region 2 HDC */
                    } else if (region == kRegionID2 && type == kTypeDriftHDC) {

                        // Set ht pattern for this region
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
                            if (fDebug) rd->print();

                            // If detector is inactive for tracking, skip it
                            if (rd->IsInactive()) {
                              patterns.push_back(QwHitPattern(levelsr2));
                              continue;
                            }

                            // Get the subhitlist of hits in this detector plane
                            QwHitContainer *subhitlist = hitlist->GetSubList_Plane(region, package, plane);
                            if (fDebug) subhitlist->Print();

                            // Construct the hit pattern for this set of hits
                            QwHitPattern hitpattern(levelsr2);
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
                        char* channel[patterns.size()];
                        int*  hashchannel[patterns.size()];
                        for (size_t layer = 0; layer < patterns.size(); layer++) {
                          channel[layer] = new char[patterns.at(layer).GetNumberOfBins()];
                          hashchannel[layer] = new int[patterns.at(layer).GetFinestBinWidth()];
                          patterns.at(layer).GetPattern(channel[layer]);
                          patterns.at(layer).GetPatternHash(hashchannel[layer]);
                        }

                        QwDebug << "Search for matching patterns (direction " << dir << ")" << QwLog::endl;
                        treelinelist = TreeSearch->SearchTreeLines(searchtree,
                                             channel, hashchannel, levels,
                                             0, tlayers);

                        // Delete the old array structures
                        for (size_t layer = 0; layer < patterns.size(); layer++) {
                          delete[] channel[layer];
                          delete[] hashchannel[layer];
                        }

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
                            treelinelist->Print();
                        }

                        // Get the hit list for this package/region/direction
                        QwHitContainer *subhitlist = hitlist->GetSubList_Dir(region, package, dir);
                        if (fDebug) subhitlist->Print();

                        QwDebug << "Calculate chi^2" << QwLog::endl;
                        if (searchtree) {

                            double width = searchtree->GetWidth();
                            TreeCombine->TlTreeLineSort (treelinelist, subhitlist,
                                                         package, region, dir,
                                                         1UL << (levelsr2 - 1),
                                                         tlayers, 0, width);
                        }

                        QwDebug << "Sort patterns" << QwLog::endl;
                        TreeSort->rcTreeConnSort (treelinelist, region);

                        if (fDebug) {
                            cout << "List of treelines:" << endl;
                            treelinelist->Print();
                        }
                        event->treeline[package][region][type][dir] = treelinelist;
                        event->AddTreeLineList(treelinelist);

                        // Delete subhitlist
                        delete subhitlist;

                        /* Any other region */
                    } else {
                        QwWarning << "[QwTrackingWorker::ProcessHits] Warning: no support for this detector." << QwLog::endl;
                        return event;
                    }


                } // end of loop over the three wire-pitch directions


                /*! ---- TASK 2: Combine the treelines into partial tracks             ---- */

                QwPartialTrack* parttrack = 0; // list of partial tracks

                // This if statement may be done wrong
                // TODO (wdc) why does this have last index dir instead of something in scope?
                if (rcDETRegion[package][region][kDirectionU]
                        && fSearchTree[package*kNumRegions*kNumTypes*kNumDirections
                                       +region*kNumTypes*kNumDirections+type*kNumDirections+kDirectionU]
                        && tlayers) {
                    parttrack = TreeCombine->TlTreeCombine(
                                    event->treeline[package][region][type],
                                    package, region,
                                    tlayers,
                                    dlayer,
                                    fSearchTree);

                } else continue;


                /*! ---- TASK 3: Sort out the Partial Tracks                          ---- */

                if (parttrack) TreeSort->rcPartConnSort(parttrack);

                /*! ---- TASK 4: Hook up the partial track info to the event info     ---- */

                event->parttrack[package][region][type] = parttrack;
                event->AddPartialTrackList(parttrack);

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

        //jpan: The following code is for testing the raytrace class

        if (false && event->parttrack[package][kRegionID2][kTypeDriftHDC]
                  && event->parttrack[package][kRegionID3][kTypeDriftVDC]) {

            if (fDebug) std::cout<<"Bridging front and back partialtrack:"<<std::endl;
            if (event->parttrack[package][kRegionID2][kTypeDriftHDC]->IsGood()
                    && event->parttrack[package][kRegionID3][kTypeDriftVDC]->IsGood()
                    && event->parttrack[package][kRegionID3][kTypeDriftVDC]->cerenkovhit==1 ) {

                TVector3 R2hit(event->parttrack[package][kRegionID2][kTypeDriftHDC]->pR2hit[0],
                               event->parttrack[package][kRegionID2][kTypeDriftHDC]->pR2hit[1],
                               event->parttrack[package][kRegionID2][kTypeDriftHDC]->pR2hit[2]);
                TVector3 R2direction(event->parttrack[package][kRegionID2][kTypeDriftHDC]->uvR2hit[0],
                                     event->parttrack[package][kRegionID2][kTypeDriftHDC]->uvR2hit[1],
                                     event->parttrack[package][kRegionID2][kTypeDriftHDC]->uvR2hit[2]);
                TVector3 R3hit(event->parttrack[package][kRegionID3][kTypeDriftVDC]->pR3hit[0],
                               event->parttrack[package][kRegionID3][kTypeDriftVDC]->pR3hit[1],
                               event->parttrack[package][kRegionID3][kTypeDriftVDC]->pR3hit[2]);
                TVector3 R3direction(event->parttrack[package][kRegionID3][kTypeDriftVDC]->uvR3hit[0],
                                     event->parttrack[package][kRegionID3][kTypeDriftVDC]->uvR3hit[1],
                                     event->parttrack[package][kRegionID3][kTypeDriftVDC]->uvR3hit[2]);

                trajectory->SetStartAndEndPoints(R2hit, R2direction, R3hit, R3direction);
                int status = trajectory->BridgeFrontBackPartialTrack();

                if (status == 0){
                  std::cout<<"======>>>> Bridged a track"<<std::endl;
                  trajectory->PrintInfo();
                }
                else std::cout<<"======>>>> No luck on bridging this track."<<std::endl;
            }
        }

    } /* end of loop over the detector packages */


    // Delete tracking objects
    if (TreeSearch)  delete TreeSearch;
    if (TreeCombine) delete TreeCombine;
    if (TreeSort)    delete TreeSort;
    if (TreeMatch)   delete TreeMatch;

//   if (fDebug)
//     cout<<"R2Good, R2Bad, R3Good, R3Bad: "<<R2Good<<" "<<R2Bad<<" "<<R3Good<<" "<<R3Bad<<endl;
//     cout<<"Efficiency:     region 2  "<<float(R2Good)/(R2Good+R2Bad)*100.0
//         <<"%,     region 3  "<<float(R3Good)/(R3Good+R3Bad)*100.0<<"%"<<endl;

    // Return the event structure
    return event;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
