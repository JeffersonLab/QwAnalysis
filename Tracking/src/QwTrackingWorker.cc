/*------------------------------------------------------------------------*//*!

 \class QwTrackingWorker

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
using namespace std;

// Qweak headers
#include "Qoptions.h"
#include "options.h"
#include "Det.h"
#include "Hit.h"

// Tree search headers
#include "tracking.h"
#include "treeregion.h"

// Tracking modules
#include "treesearch.h"
#include "treecombine.h"
#include "treesort.h"
#include "treematch.h"

using namespace QwTracking;


extern treeregion *rcTreeRegion[kNumPackages][kNumRegions][kNumTypes][kNumDirections];
extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
extern Options opt;



QwTrackingWorker::QwTrackingWorker ()
{
  // Initialize pattern database
  tree thetree;
  thetree.rcInitTree();

  /* Reset counters of number of good and back events */
  ngood = nbad = 0;

  /* Reset debug level */
  debug = 1;
}



/*------------------------------------------------------------------------*//*!

   Bcheck() - this function compares the momentum look-up energy with the
              shooting method result.  This function is for debugging
              purposes only.

    inputs: (1) double E       - the momentum lookup energy
            (2) PartTrack *f   - pointer to the front partial track for the
                                 track
            (3) PartTrack *b   - pointer to the back partial track of the
                                 track
            (4) double TVertex - transverse position of the vertex for the
                                 track
            (5) double ZVertex - Z position of the vertex for the track

   outputs: an ASCII ntuple file.

*//*-------------------------------------------------------------------------*/

double rcShootP (double Pest,PartTrack *front, PartTrack *back, double accuracy)
{
  cerr << "Warning: The function rcShootP is only a stub" << endl;
  return -1000;
}

void QwTrackingWorker::BCheck (double E, PartTrack *f, PartTrack *b, double TVertex, double ZVertex)
{
  double Es = rcShootP(0.0,f,b,0.005);
  //extern physic phys_carlo;
  static FILE *test = 0;

  if( !test )
    test = fopen("magcheck","w");
  if( test && E > 0.0) {
    fprintf(test,"%f %f %f %f %f %f\n",
	    /*phys_carlo.E,*/E,Es,f->mx,f->my, TVertex, ZVertex);
  }
}



/*------------------------------------------------------------------------*//*!

  rcLinkUsedTracks() - this function links the track cross-linked list to
                       a single-linked list to ease the access for later
		       functions. The new link is done via the usenext
		       pointer.

    inputs: (1) Track *track -
            (2) int package   -

   returns: the head to the linked list.

*//*-------------------------------------------------------------------------*/

Track * QwTrackingWorker::rcLinkUsedTracks( Track *track, int package )
{
  Track *ret = 0, *usedwalk = 0;
  Track *trackwalk, *ytrack;

  /* loop over all found tracks */
  for(trackwalk = track; trackwalk; trackwalk = trackwalk->next ) {
    /* and the possible y-tracks */
    for( ytrack = trackwalk; ytrack; ytrack = ytrack->ynext ) {
      // Statist[ytrack->method].TracksGenerated[package] ++;
      if(!ytrack->Used) continue;
      //Statist[ytrack->method].TracksUsed[package] ++;
      if( !ret ) /* return the first used track */
	ret = usedwalk = ytrack;
      else {
	usedwalk->usednext = ytrack; /* and link them together */
	usedwalk = ytrack;
      }
    }
  }
  return ret;
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
// TODO Should QwHitContainer be passed as const? (wdc)
Event* QwTrackingWorker::ProcessHits (QwHitContainer &hitlist)
{
  int k;
  int dlayer = 0;	      /* number of detector planes in the search    */
  double A[3][2];	      /* conversion between xy and uv */
  Event *event;               /* area for storing the reconstruction info   */
  PartTrack *area = 0;
  Det *rd/*, *rnd*/;          /* pointers for moving through the linked
                                 lists of detector id and hit information   */
  TreeLine *treelines1, *treelines2;

  treesearch  TreeSearch;
  treecombine TreeCombine;
  treesort    TreeSort;
  treematch   TreeMatch;

  /*
  int charge;
  int found_front = 0;
  double ZVertex, bending, theta, phi, ESpec;
  int outbounds = 0;
  */


  // Region 2 bit patterns
  static char *channelr2[TLAYERS];
  static int  *hashchannelr2[TLAYERS];

  // Number of wires in region 3
  const int numWiresr3 = 281;
  // Region 3 bit patterns
  static char *channelr3[numWiresr3 + 1];
  static int  *hashchannelr3[numWiresr3 + 1];


  event = (Event*) calloc(1, sizeof(Event));
  assert(event);

  /// Initialize if this is the first event.
  // TODO Could we put this in a constructor method? (wdc)
  static int init = 0;
  if (!init) {
    int levelmax;
    int levels;

    /* Determine the bin-division depth of the tree-search */
    levelmax = opt.MaxLevels + 1;

    /* Reserve space for region 2 bit patterns */
    int dbg_levels = opt.levels[kPackageUp][kRegionID2-1][kTypeDriftHDC];
    levels = levelmax;
    for (int i = 0; i < TLAYERS; i++) {
      channelr2[i]     = (char*) malloc (1UL << levels);
      hashchannelr2[i] =  (int*) malloc ((sizeof(int) * (1UL << (levels - 1))));
      assert (channelr2[i] && hashchannelr2[i]);
    }

    /* Reserve space for region 3 bit patterns */
    levels = opt.levels[kPackageUp][kRegionID3-1][kTypeDriftHDC];
    for (int i = 0; i < numWiresr3 + 1; i++) {
      channelr3[i]     = (char*) malloc (1UL << levels);
      hashchannelr3[i] =  (int*) malloc ((sizeof(int) * (1UL << (levels - 1))) );
      assert (channelr3[i] && hashchannelr3[i]);
    }

    if (debug) cout << "[QwTrackingWorker::ProcessHits] Initialization complete." << endl;
    init++;
  }


  /// Loop through all detector packages
  // Normally only two of these will generate actual tracks,
  // since there are only two tracking packages for the eight
  // possible positions
  for (EQwDetectorPackage package  = kPackageUp;
			  package <= kPackageDown; package++) {
    if (debug) cout << "[QwTrackingWorker::ProcessHits] Package: " << package << endl;

    // Currently assume that only the up and down octants contain the tracking
    // detectors.  When rotation is included, this will have to be modified to
    // take into account that the tracking detectors are in different octants.
    if (package != kPackageUp && package != kPackageDown) continue;


    /// Loop through the detector regions
    for (EQwRegionID region  = kRegionID2;
		     region <= kRegionID3; region++) {
      if (debug) cout << "[QwTrackingWorker::ProcessHits]  Region: " << region << endl;


      /// Loop over the detector types (only drift chambers are used)
      for (EQwDetectorType type  = kTypeDriftHDC;
			   type <= kTypeDriftVDC; type++) {
	if (debug) cout << "[QwTrackingWorker::ProcessHits]   Detector: " << type << endl;


	/// Loop through the detector directions
	for (EQwDirectionID dir  = kDirectionX;
			    dir <= kDirectionV; dir++) {
          if (debug) cout << "[QwTrackingWorker::ProcessHits]    Direction: " << dir << endl;


	  // Skip wire direction Y for region 2
	  if (region == kRegionID2
	      && dir == kDirectionY) continue;
	  // Skip wire direction X and Y for region 3
	  if (region == kRegionID3
	      && (dir == kDirectionX || dir == kDirectionY)) continue;

	  // Check whether there are any detectors defined in that geometry
	  if (! rcDETRegion[package][region-1][dir]) {
	    if (debug) cout << "[QwTrackingWorker::ProcessHits]     No such detector!" << endl;
	    continue;
	  }
	  if (! rcTreeRegion[package][region-1][type][dir]) {
	    if (debug) cout << "[QwTrackingWorker::ProcessHits]     No such tree!" << endl;
	    continue;
	  }

/*! ---- 1st: check that the direction is tree-searchable               ---- */
	  //cout << "rcTreeRegion(" << package << ","
	  //			    << region << ","
	  //			    << type << ","
	  //			    << dir << ")" << endl;
	  if (rcTreeRegion[package][region-1][type][dir]->searchable == false) {
	    (event->treeline[package][region-1][type][dir]) = 0;
	    // printf("%i %i %i %i\n",package,region,type,dir);
	    continue; // 'searchable' is set by tree.cc
	  }


/*! ---- 2nd: do some variable initialization for the found tree line
              linked list                                              ---- */

	  // Start the search for this set of like-pitched planes
	  // TODO (wdc) take a careful look at where TreeSearch should be
	  // instantiated and where BeginSearch and EndSearch should go
	  TreeSearch.BeginSearch();
	  TreeLine* treelinelist = 0; // local list of found tree lines

/*! ---- 3rd: create the bit patterns for the hits                     ---- */

	  /* Region 3 VDC */
	  if (region == kRegionID3 && type == kTypeDriftVDC) {

            dlayer = 0; /* set "number of detectors" to zero            */
	    int decrease;
	    treelines1 = 0; treelines2 = 0;

	    /* Loop over the like-pitched planes in a region */
	    for (k = 0, rd = rcDETRegion[package][region-1][dir], decrease = 0;
	         rd; rd = rd->nextsame, decrease += numWiresr3, k++) {

	      // Print detector info
	      if (debug) cout << "      ";
	      if (debug) rd->print();

	      // Set angles for this direction (U or V)
	      A[dir][0] = rd->rCos; /* cos (angle of wire pitch) */
	      A[dir][1] = rd->rSin; /* sin (angle of wire pitch) */

	      if (debug) cout << "Setting pattern hits (region 3)" << endl;
	      for (int i = 0; i < numWiresr3 + 1; i++) {
	        memset(channelr3[i],     0,                1UL << (opt.levels[0][2][0]   ));
	        memset(hashchannelr3[i], 0, sizeof(int) * (1UL << (opt.levels[0][2][0]-1)));
	      }

	      /// Get the corresponding hit from the hitlist
	      QwHitContainer::iterator qwhit;
	      QwHit* hit = 0;
	      if (debug) cout << "Looping over QwHitContainer" << endl;
	      for (qwhit  = hitlist.begin();
		   qwhit != hitlist.end(); qwhit++) {

		// TODO This will be reorganized so we don't need to do this!
		QwDetectorID detector = qwhit->GetDetectorID();
		if (detector.fPackage == package
		 && detector.fRegion == region
		 && detector.fDirection == dir
		 && detector.fPlane == rd->ID) {
		  hit = &(*qwhit);

		  // See treesearch.cc for the different ways in which TsSetPoint
		  // can be called.
		  int wire = hit->GetDetectorID().fElement;
		  TreeSearch.TsSetPoint(
			rcTreeRegion[package][region-1][type][dir]->rWidth,
			hit,
			channelr3[wire - decrease],
			hashchannelr3[wire - decrease],
			1U << (opt.levels[package][region-1][type]-1));

		  // Print hit pattern, if requested
		  if (opt.showEventPattern) {
		    cout << "w" << wire << ":";
		    for (int i = 0; i < (signed int) (1UL << (opt.levels[package][region-1][type]))-1; i++) {
		      if (channelr3[wire - decrease][i] == 1)
			cout << "|";
		      else
			cout << ".";
		      }
		    cout << endl;
		  }

		} // end of if for hits in this detector (TODO need to be removed)

	      } // end of loop over hits in this event

	      // If no hits in this detector, skip to the next detector.
	      if (! hit) continue;

	      // Start the search for this set of like-pitched planes
	      TreeSearch.BeginSearch();

	      // All hits in this detector (VDC) are added to the bit pattern.
	      // We can start the tree search now.
	      // NOTE Somewhere around here a memory leak lurks
	      if (debug) cout << "Searching for matching patterns (direction " << dir << ")" << endl;
	      TreeSearch.TsSearch(&(rcTreeRegion[package][region-1][type][dir]->node),
				channelr3, hashchannelr3,
				opt.levels[package][region-1][type], numWiresr3, TLAYERS);
	      treelinelist = TreeSearch.GetListOfTreeLines();

	      // DEBUG section
	      // Did this succeed as intended?
	      // - what does rcTreeRegion->node contain?
	      shortnode* dbg_testnode = &(rcTreeRegion[package][region-1][type][dir]->node);
	      // - are all the hits filled?


	      if (debug) cout << "Sort patterns" <<  endl;
              if (rcTreeRegion[package][region-1][type][dir]) {

		TreeCombine.TlTreeLineSort (treelinelist, package, region, type, dir,
			1UL << (opt.levels[package][region-1][type]-1), 0, dlayer);
		if (k == 0) {
		  treelines1 = treelinelist;
		} else if (k == 1) {
		  treelines2 = treelinelist;
		}

	      }
	      dlayer++;

	      // End the search for this set of like-pitched planes
	      TreeSearch.EndSearch();

	    } // end of loop over like-pitched planes in a region

	    if (debug) cout << endl;
	    if (debug) cout << "Matching region 3 segments" << endl;
	    // (wdc) If no treelines1 or treelines2 is found, then skip matching.
	    //       Otherwise this gets confused due to the scintillators.
	    if (treelines1 || treelines2)
	      treelinelist = TreeMatch.MatchR3 (treelines1, treelines2, package, region, dir);
	    event->treeline[package][region-1][type][dir] = treelinelist;
	    tlayers = TLAYERS;     /* remember the number of tree-detector */
	    tlaym1  = tlayers - 1; /* remember tlayers - 1 for convenience */



	  /* Region 2 HDC */
	  } else if (region == kRegionID2 && type == kTypeDriftHDC) {

	    /* Loop over the like-pitched planes in a region */
	    for (rd = rcDETRegion[package][region-1][dir], tlayers = 0;
	         rd; rd = rd->nextsame, tlayers++) {

	      if (debug) cout << "      ";
	      if (debug) rd->print();

	      if (debug) cout << "Setting pattern hits (region 2)" << endl;
	      memset(channelr2[tlayers],     0,                1UL<<(opt.levels[package][region-1][type]   ));
	      memset(hashchannelr2[tlayers], 0, sizeof(int) * (1UL<<(opt.levels[package][region-1][type]-1)));

	      // Get the corresponding hit from the hitlist
	      QwHitContainer::iterator qwhit;
	      QwHit* hit = 0;
	      if (debug) cout << "Looping over QwHitContainer" << endl;
	      for (qwhit  = hitlist.begin();
		   qwhit != hitlist.end(); qwhit++) {

		// TODO This will be reorganized so we don't need to do this!
		QwDetectorID detector = qwhit->GetDetectorID();
		if (detector.fPackage == package
		 && detector.fRegion == region
		 && detector.fDirection == dir
		 && detector.fPlane == rd->ID) {
		  hit = &(*qwhit);

		  // See treesearch.cc for the different ways in which TsSetPoint
		  // can be called.
		  int wire = hit->GetDetectorID().fElement;
		  TreeSearch.TsSetPoint(
			rcTreeRegion[package][region-1][type][dir]->rWidth,
			rd->WireSpacing,
			hit,
			wire,
			channelr2[tlayers],
			hashchannelr2[tlayers],
			1U << (opt.levels[package][region-1][type]-1));

		  // Print hit pattern, if requested
		  if (opt.showEventPattern) {
		    cout << "w" << wire << ":";
		    for (int i = 0; i < (signed int) (1UL << (opt.levels[package][region-1][type]))-1; i++) {
		      if (channelr2[tlayers][i] == 1)
			cout << "|";
		      else
			cout << ".";
		    }
		    cout << endl;
		  }

		} // end of if for hits in this detector

	      } // end of loop over hits in this event

            } // end of loop over like-pitched planes in a region


	    if (debug) cout << "Search for matching patterns (direction " << dir << ")" << endl;
	    TreeSearch.TsSearch(&(rcTreeRegion[package][region-1][type][dir]->node),
				channelr2, hashchannelr2,
				opt.levels[package][region-1][type], 0, tlayers);

	    if (debug) cout << "Sort patterns" << endl;
            if (rcTreeRegion[package][region-1][type][dir]) {
	      TreeCombine.TlTreeLineSort (treelinelist, package, region, type, dir,
					1UL << (opt.levels[package][region-1][type]-1),
					tlayers, 0);
            }
	    event->treeline[package][region-1][type][dir] = treelinelist;

	    // End the search for this set of like-pitched planes
	    TreeSearch.EndSearch();

	  /* Any other region */
	  } else {
	    cerr << "[QwTrackingWorker::ProcessHits] Warning: no support for this detector." << endl;
	    return event;
	  }


	} // end of loop over the three wire-pitch directions


/*! ---- TASK 2: Combine the treelines into partial tracks             ---- */

	// This if statement may be done wrong
	// TODO (wdc) why does this have last index dir instead of something in scope?
	if (rcDETRegion[package][region-1][kDirectionU]
	 && rcTreeRegion[package][region-1][type][kDirectionU]
	 && tlayers) {
	  area = TreeCombine.TlTreeCombine(event->treeline[package][region-1][type],
			1L << (opt.levels[package][region-1][type]-1),
			package,
			region,
			type,
			tlayers,
			dlayer);
	}


/*! ---- TASK 3: Sort out the Partial Tracks                          ---- */

	if (area) TreeSort.rcPartConnSort(area);


/*! ---- TASK 4: Hook up the partial track info to the event info     ---- */

	event->parttrack[package][region][type] = area;

// (wdc) Loop over detector types disables
      } /* end of loop over the detector types */

    } /* end of loop over the regions */


    /* ==============================
    * Correlate front and back
    * tracks from x, y and y' infor-
    * mation
    * ============================== */




  } /* end of loop over the detector packages */

  if (area) {
    cout << "area: " << area->x << " " << area->mx << ", " << area->y << " " << area->my << endl;
    ngood++;
  } else {
    cout << "Couldn't find a good partial track." << endl;
    nbad++;
  }

  return event;
}
