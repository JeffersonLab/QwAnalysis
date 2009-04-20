/*------------------------------------------------------------------------*//*!

 \class treedo

 \brief Controls all the routines involved in finding tracks in an event.

 \verbatim

 PROGRAM: QTR (Qweak Track Reconstruction)	AUTHOR: Burnham Stokes
							bestokes@jlab.org
						ORIGINAL HRC AUTHOR
							Wolfgang Wander
							wwc@hermes.desy.de


 MODULE: treedo

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

 (03) rcTreeDo() - this function manages the track-finding and track-fitting
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

#include "treedo.h"

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


// Enumerator increments
extern EPackage operator++(enum EPackage &rs, int );
extern Etype operator++(enum Etype &rs, int );
/*extern Eorientation operator++(enum Eorientation &rs, int );*/

extern TreeLine  *trelin;
extern int trelinanz;
extern treeregion *rcTreeRegion[kNumPackages][kNumRegions][kNumPlanes][kNumDirections];
extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
extern Options opt;



treedo::treedo ()
{
  ngood = nbad = 0;
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

void treedo::BCheck (double E, PartTrack *f, PartTrack *b, double TVertex, double ZVertex)
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

Track * treedo::rcLinkUsedTracks( Track *track, int package )
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

  rcTreeDo() - this function is the main tracking function. It
               performes tracking in projections (u/v/x) to form treelines,
               it combines projection tracks to tracks in space and bridges
               tracks in space before and after the magnet to form recon-
               structed particle tracks. The function calculates the momentum
               of the tracks and afterwards reiterates the track parameters
               using the now known track position and momentum for iterating
               the hit positions in the tracking chambers.

   inputs:  (1) int iEventNo - the event number

   outputs: (1) Event *rcTreeDo() - pointer to the structure with all
                                    of the reconstruction information for
                                    this event.

*//*-------------------------------------------------------------------------*/

Event * treedo::rcTreeDo (int iEventNo)
{
  enum EPackage package;	/* Loop counter for TOP and BOTTOM detectors */
//  enum Emethod method;	/* Loop counter over reconstruction method   */
//  enum EFrontBack frback;	/* Loop counter over FRONT or BACK regions   */
  EQwDirectionID dir;		/* Loop counter over wire pitches (U,V,X)    */
  EQwRegionID region;		/* Loop counter over regions                 */
  enum Etype type;		/* Loop counter over detector types          */
//  enum Eorientation orient;

  int i,k;
  int debug = 1;
  int dlayer = 0;	      /* number of detector planes in the search    */
  double A[3][2];	      /* conversion between xy and uv */
  Event *event;               /* area for storing the reconstruction info   */
  PartTrack *area = 0;
  Det *rd/*, *rnd*/;              /* pointers for moving through the linked
                                 lists of detector id and hit information   */
  TreeLine *trelin1,*trelin2;
  Hit *H;

  treesearch  TreeSearch;
  treecombine TreeCombine;
  treesort    TreeSort;
  treematch   TreeMatch;

  /*int charge;
  int found_front = 0;
  double ZVertex, bending, theta, phi, ESpec;
  int outbounds = 0;*/
  int levelmax;
  static int init = 0;

  // Region 2 bit patterns
  static char *channel[TLAYERS];
  static int  *hashchannel[TLAYERS];

  // Number of wires in region 3
  const int numWiresr3 = 281;
  // Region 3 bit patterns
  static char *channelr3[numWiresr3 + 1];
  static int  *hashchannelr3[numWiresr3 + 1];



  event = (Event *) calloc(1, sizeof(Event));
  assert(event);

  // Initialize stuff if first event
  if (!init) {
    levelmax = opt.MaxLevels + 1;      /* determine the bin-division depth
                                          of the tree-search               */

    /* reserve space for region 2 bit patterns */
    for (i = 0; i < TLAYERS; i++) {
      channel[i]     = (char*) malloc( 1UL << levelmax );
      hashchannel[i] =  (int*) malloc( (sizeof(int) * (1UL << (levelmax-1))) );
      assert (channel[i] && hashchannel[i]);
    }

    /* reserve space for region 3 bit patterns */
    for (i = 0; i < numWiresr3 + 1; i++) {
      channelr3[i]     = (char*) malloc( 1UL << (opt.levels[0][2][0]) );
      hashchannelr3[i] =  (int*) malloc( (sizeof(int) * (1UL << (opt.levels[0][2][0]-1))) );
      assert (channelr3[i] && hashchannelr3[i]);
    }

    if (debug) cout << "rcTreeDo: initialized" << endl;
    init++;
  }


  // Loop through all detector packages / regions / directions
  for (package = w_upper; package <= w_upper /*w_lower*/; package++) {
    for (region = kRegionID2 /*r1*/; region <= kRegionID3; region++) {
      for (type = d_drift; type <= d_drift /*d_cerenkov*/; type++) {

	// Old order was: [U, V, X], Y
	// New order is: [X, Y, U, V], R, Theta
	// with included directions in brackets
	for (dir = kDirectionX; dir <= kDirectionV; dir++) {

	  // Skip wire direction Y
	  if (dir == kDirectionY) continue;

	  // Debug output
	  if (debug) cout << endl;
	  if (debug) rcDETRegion[package][region-1][dir]->print();

/*! ---- 1st: check that the direction is tree-searchable               ---- */

	  if (rcTreeRegion[package][region-1][type][dir]->searchable == false) {
	    (event->treeline[package][region-1][type][dir]) = 0;
	    // printf("%i %i %i %i\n",package,region,type,dir);
	    continue; // 'searchable' is set by tree.cc
	  }


/*! ---- 2nd: do some variable initialization for the found treeline
              linked list                                              ---- */

	  trelin = 0;    /* clear pointer to linked list of treelines       */
	  trelinanz = 0; /* clear the "number of treelines found" counter   */


/*! ---- 3rd: create the bit patterns for the hits                     ---- */

	  /* Region 3 */
	  if (region == kRegionID3) {

            dlayer = 0; /* set "number of detectors" to zero            */
	    int decrease;
	    trelin1 = 0; trelin2 = 0;

	    /* Loop over the like-pitched planes in a region */
	    for (k = 0, rd = rcDETRegion[package][region-1][dir], decrease = 0;
	         rd; rd = rd->nextsame, decrease += numWiresr3, k++ ) {

	      if (debug) cout << "-> ";
	      if (debug) rd->print();

	      trelin = 0; // clear the linked list for the 2nd layer
	      A[dir][0] = rd->rCos; /* cos (angle of wire pitch) */
	      A[dir][1] = rd->rSin; /* sin (angle of wire pitch) */

	      if (debug) cout << "Setting pattern hits (region 3)" << endl;
	      for (i = 0; i < numWiresr3 + 1; i++) {
	        memset(channelr3[i],     0,                1UL << (opt.levels[0][2][0]   ));
	        memset(hashchannelr3[i], 0, sizeof(int) * (1UL << (opt.levels[0][2][0]-1)));
	      }

	      // Loop over hit list in this detector
	      H = rd->hitbydet;
	      while (H && H->wire <= numWiresr3 + decrease) {
		// (See treesearch.cc for the different ways in which TsSetPoint
		//  can be called.)
		TreeSearch.TsSetPoint(
			rcTreeRegion[package][region-1][type][dir]->rWidth,
			H,
			channelr3[H->wire-decrease], hashchannelr3[H->wire-decrease],
			1U << (opt.levels[package][region-1][type]-1));

		// Print hit pattern, if requested
		if (opt.showEventPattern) {
		  cout << "w" << H->wire << ":";
		  for (int i = 0; i < (signed int) (1UL << (opt.levels[package][region-1][type]))-1; i++) {
		    if (channelr3[H->wire-decrease][i] == 1)
		      cout << "|";
		    else
		      cout << ".";
		  }
		  cout << endl;
		}

		// Next hit in same detector
		H = H->nextdet;
	      }

	      // somewhere around here a memory leak lurks
	      // you don't say!!! (wdconinc)

	      if (debug) cout << "Search for matching patterns (direction " << dir << ")" << endl;
	      TreeSearch.TsSearch(&(rcTreeRegion[package][region-1][type][dir]->node),
				channelr3, hashchannelr3,
				opt.levels[package][region-1][type], numWiresr3, TLAYERS);


	      if (debug) cout << "Sort patterns" <<  endl;
              if (rcTreeRegion[package][region-1][type][dir]) {

		if (k == 0) {
		  TreeCombine.TlTreeLineSort (trelin, package, region, type, dir,
			1UL << (opt.levels[package][region-1][type]-1), 0, dlayer);
		  trelin1 = trelin;

		} else if (k == 1) {
		  TreeCombine.TlTreeLineSort (trelin, package, region, type, dir,
			1UL << (opt.levels[package][region-1][type]-1), 0, dlayer);
		  trelin2 = trelin;
		}

	      }
	      dlayer++;

	    } // end of loop over like-pitched planes in a region


	    if (debug) cout << "Found " << trelinanz << " treelines.";
	    if (debug) cout << endl;
	    if (debug) cout << "Matching region 3 segments" << endl;
	    // (wdc) If no trelin1 or trelin2 is found, then skip matching.
	    //       Otherwise this gets confused due to the scintillators.
	    if (trelin1 || trelin2)
	      trelin = TreeMatch.MatchR3 (trelin1, trelin2, package, region, dir);
	    event->treeline[package][region-1][type][dir] = trelin;
	    tlayers = TLAYERS;     /* remember the number of tree-detector */
	    tlaym1  = tlayers - 1; /* remember tlayers - 1 for convenience */



	  /* Region 2 */
	  } else if (region == kRegionID2) {

	    /* Loop over the like-pitched planes in a region */
	    for (rd = rcDETRegion[package][region-1][dir], tlayers = 0;
	         rd; rd = rd->nextsame, tlayers++) {

	      if (debug) cout << "-> ";
	      if (debug) rd->print();

	      if (debug) cout << "Setting pattern hits (region 2)" << endl;
	      memset(channel[tlayers],     0,                1UL<<(opt.levels[package][region-1][type]   ));
	      memset(hashchannel[tlayers], 0, sizeof(int) * (1UL<<(opt.levels[package][region-1][type]-1)));

	      // Loop over hit list in this detector
	      H = rd->hitbydet;
	      while (H) {
		// (See treesearch.cc for the different ways in which TsSetPoint
		//  can be called.)
		TreeSearch.TsSetPoint(
			rcTreeRegion[package][region-1][type][dir]->rWidth,
			rd->WireSpacing,
			H, H->wire,
			channel[tlayers], hashchannel[tlayers],
			1U << (opt.levels[package][region-1][type]-1));

		// Print hit pattern, if requested
		if (opt.showEventPattern) {
		  cout << "w" << H->wire << ":";
		  for (int i = 0; i < (signed int) (1UL << (opt.levels[package][region-1][type]))-1; i++) {
		    if (channel[tlayers][i] == 1)
		      cout << "|";
		    else
		      cout << ".";
		  }
		  cout << endl;
		}

		// Next hit
		H = H->next;
	      }

            } // end of loop over like-pitched planes in a region


	    if (debug) cout << "Search for matching patterns (direction " << dir << ")" << endl;
	    TreeSearch.TsSearch(&(rcTreeRegion[package][region-1][type][dir]->node),
				channel, hashchannel,
				opt.levels[package][region-1][type], 0, tlayers);


	    if (debug) cout << "Sort patterns" << endl;
            if (rcTreeRegion[package][region-1][type][dir]) {
	      TreeCombine.TlTreeLineSort(trelin, package, region, type, dir,
					1UL << (opt.levels[package][region-1][type]-1),
					tlayers, 0);
            }
	    event->treeline[package][region-1][type][dir] = trelin;


	  /* Any other region */
	  } else {
	    cerr << "TreeDo: Error: no code for this type of detector here." << endl;
	    return event;
	  }


	} // end of loop over the three wire-pitch directions


/*! ---- TASK 2: Combine the treelines into partial tracks             ---- */

	// This if statement may be done wrong
	if (rcTreeRegion[package][region-1][type] && tlayers) {
	  area = TreeCombine.TlTreeCombine(event->treeline[package][region-1][type],
			       1L << (opt.levels[package][region-1][type]-1),
			       package,
			       region,
			       type,
			       tlayers,
			       dlayer);
	}


/*! ---- TASK 3: Sort out the Partial Tracks                          ---- */

	TreeSort.rcPartConnSort(area);


/*! ---- TASK 4: Hook up the partial track info to the event info     ---- */

	event->parttrack[package][region][type] = area;

      } /* end of loop over the detector types */

    } /* end of loop over the regions */


    /* ==============================
    * Correlate front and back
    * tracks from x, y and y' infor-
    * mation
    * ============================== */




  } /* end of loop over the detector packages */

  if (area) {
    cout << "iEventNo: " << iEventNo << ", area: " << area->x << " " << area->mx << ", " << area->y << " " << area->my << endl;
    ngood++;
  } else {
    cout << "iEventNo: " << iEventNo << endl;
    nbad++;
  }

  return event;
}
