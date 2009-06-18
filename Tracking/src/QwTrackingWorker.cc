//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwTrackingWorker.h"

extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
extern Options opt;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingWorker::QwTrackingWorker (const char* name) : VQwSystem(name)
{
 /* Set debug level */
  debug = 1;

  if( debug )
      cout<<"###### Calling QwTrackingWorker::QwTrackingWorker ()"<<endl;

  // Initialize pattern database

  InitTree();

  /* Reset counters of number of good and bad events */
  ngood = 0;
  nbad = 0;

    /* Determine the bin-division depth of the tree-search */
    int levelmax = opt.MaxLevels + 1;

    /* Reserve space for the bit patterns:

       The options file indicates the number of tree levels (how many times do
       we go to finer binning).  The total number of bits is determined using
       bit shift operators:
         (1UL << levels) == 2^levels
         (1UL << (levels - 1)) == 2^(levels-1)
       For e.g. 4 levels we need 1 + 2 + 4 + 8 = 15 = (2^4 - 1) bits.
    */
    int levels;

    /* Reserve space for region 2 bit patterns */
    levels = opt.levels[kPackageUp][kRegionID2-1][kTypeDriftHDC];
    for (int i = 0; i < TLAYERS; i++) {
      channelr2[i]     = (char*) malloc (1UL << levels);
      hashchannelr2[i] =  (int*) malloc ((sizeof(int) * (1UL << (levels - 1))));
      assert (channelr2[i] && hashchannelr2[i]);
    }

    /* Reserve space for region 3 bit patterns */
    levels = opt.levels[kPackageUp][kRegionID3-1][kTypeDriftVDC];
    for (int i = 0; i < NUMWIRESR3 + 1; i++) {
      channelr3[i]     = (char*) malloc (1UL << levels);
      hashchannelr3[i] =  (int*) malloc ((sizeof(int) * (1UL << (levels - 1))) );
      assert (channelr3[i] && hashchannelr3[i]);
    }

  if( debug )
      cout<<"###### Leaving QwTrackingWorker::QwTrackingWorker ()"<<endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingWorker::~QwTrackingWorker ()
{
  if( debug )
      cout<<"###### Calling QwTrackingWorker::~QwTrackingWorker ()"<<endl;

  if( debug )
      cout<<"###### Leaving QwTrackingWorker::~QwTrackingWorker ()"<<endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingWorker::InitTree()
{

// rcInitTree() used to start here
  char filename[256];
  int numlayers = 0;
  double width = 0;

  QwTrackingTree *thetree = new QwTrackingTree();

  /// For each region (1, 2, 3, trigger, cerenkov, scanner)
  for (EQwRegionID region  = kRegionID1;
		   region <= kRegionID3; region++) {

    // TODO Skip region 1 for now
    if (region < kRegionID2) continue;

    /// ... and for each package
    for (EQwDetectorPackage package  = kPackageUp;
			    package <= kPackageDown; package++) {

      // TODO DEBUG Skip everything except up and down for now...
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

	  // Skip wire direction Y
	  if (direction == kDirectionY) continue;

	  // Skip wire direction X for region 3
	  if (direction == kDirectionX && region == kRegionID3) continue;

	  ///  Skip the NULL rcDETRegion pointers.
	  //   pking:  This is probably a configuration error,
	  //           which the user may want to be warned about.
	  //   wdc:   If those pointers would be initialized to null,
	  //          this test would be a lot more useful --> another TODO
	  if (rcDETRegion[package][region-1][direction] == NULL) {
	    std::cerr << "WARN:  rcDETRegion["<< package
		      << "]["          << region-1
		      << "]["          << direction
		      << "] is NULL.  Should it be?"
		      << std::endl;
	    continue;
	  }

	  /// Region 3 contains 8 layers
	  if (region == kRegionID3) {
	    numlayers = 8; // TODO replace this with info from the geometry file
	    width = rcDETRegion[package][region-1][direction]->width[2];
	  }

	  /// Region 2 contains 4 layers
	  if (region == kRegionID2) {
	    numlayers = 4; // TODO replace this with info from the geometry file
	    width = rcDETRegion[package][region-1][direction]->WireSpacing *
		    rcDETRegion[package][region-1][direction]->NumOfWires;
	  }

	  /// Set up the filename with the following format
	  ///   tree[numlayers]-[levels]-[u|l]-[1|2|3]-[d|g|t|c]-[n|u|v|x|y].tre
	  sprintf(filename, "%s/tree%d-%d-%c-%c-%c-%c.tre",
		thetree->TREEDIR.c_str(),
		numlayers,
		opt.levels[package][region-1][type],
		"0ud"[package],
		"0123TCS"[region],
		"0hvgtc"[type],
		"0xyuvrq"[direction]);
	  if (debug) cout << "Tree filename: " << filename << endl;

	  /// Each element of rcTreeRegion will point to a pattern database
//	  rcTreeRegion[package][region-1][type][direction] =
          int index = package*kNumRegions*kNumTypes*kNumDirections
                      +(region-1)*kNumTypes*kNumDirections
                      +type*kNumDirections+direction;
	  rcTreeRegion[index] = thetree->inittree(filename,
		opt.levels[package][region-1][type],
		numlayers,
		width,
		package,
		type,
		region,
		direction);

          if (debug){

	  //cout << "rcTreeRegion(" << package << ","
	  //			    << region << ","
	  //			    << type << ","
	  //			    << direction << ")" << endl;

          }

        } /// end of loop over wire directions

      } /// end of loop over detector types

    } /// end of loop over packages

  } /// end of loop over regions

  if (thetree) delete thetree;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

   Bcheck() - this function compares the momentum look-up energy with the
              shooting method result.  This function is for debugging
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

double rcShootP (double Pest,QwPartialTrack *front, QwPartialTrack *back, double accuracy)
{
  cerr << "Warning: The function rcShootP is only a stub" << endl;
  return -1000;
}

void QwTrackingWorker::BCheck (double E, QwPartialTrack *f, QwPartialTrack *b, double TVertex, double ZVertex)
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

QwTrack* QwTrackingWorker::rcLinkUsedTracks (QwTrack *track, int package )
{
  QwTrack *ret = 0, *usedwalk = 0;
  QwTrack *trackwalk, *ytrack;

  /* loop over all found tracks */
  for (trackwalk = track; trackwalk; trackwalk = trackwalk->next ) {
    /* and the possible y-tracks */
    for (ytrack = trackwalk; ytrack; ytrack = ytrack->ynext ) {
      // Statist[ytrack->method].TracksGenerated[package] ++;
      if (!ytrack->Used) continue;
      //Statist[ytrack->method].TracksUsed[package] ++;
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
QwEvent* QwTrackingWorker::ProcessHits (QwHitContainer *hitlist)
{
  int k;
  int dlayer = 0;	      /* number of detector planes in the search    */
  double A[kNumDirections][2];	/* conversion between xy and uv */
  QwEvent *event;               /* area for storing the reconstruction info   */
  QwPartialTrack *area = 0;
  Det *rd/*, *rnd*/;          /* pointers for moving through the linked
                                 lists of detector id and hit information   */
  QwTrackingTreeLine *treelines1, *treelines2;

  QwTrackingTreeSearch  *TreeSearch  = new QwTrackingTreeSearch();
  QwTrackingTreeCombine *TreeCombine = new QwTrackingTreeCombine();
  QwTrackingTreeSort    *TreeSort    = new QwTrackingTreeSort();
  QwTrackingTreeMatch   *TreeMatch   = new QwTrackingTreeMatch();

  /*
  int charge;
  int found_front = 0;
  double ZVertex, bending, theta, phi, ESpec;
  int outbounds = 0;
  */

  event = new QwEvent;
  assert(event);

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

    // TODO (wdc) Also, only tracks in the up octant detector are available in
    // the MC generated hit lists.  Therefore we throw out the down octant.
    if (package != kPackageUp) continue;

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
	  } //kNumPackages*kNumRegions*kNumTypes*kNumDirections
	  if (! rcTreeRegion[package*kNumRegions*kNumTypes*kNumDirections
                             +(region-1)*kNumTypes*kNumDirections+type*kNumDirections+dir]) {
	    if (debug) cout << "[QwTrackingWorker::ProcessHits]     No such tree!" << endl;
	    continue;
	  }

/*! ---- 1st: check that the direction is tree-searchable               ---- */
	  //cout << "rcTreeRegion(" << package << ","
	  //			    << region << ","
	  //			    << type << ","
	  //			    << dir << ")" << endl;
	  if (rcTreeRegion[package*kNumRegions*kNumTypes*kNumDirections
                             +(region-1)*kNumTypes*kNumDirections
                             +type*kNumDirections+dir]->searchable == false) {
	    (event->treeline[package][region-1][type][dir]) = 0;
	    // printf("%i %i %i %i\n",package,region,type,dir);
	    continue; // 'searchable' is set by tree.cc
	  }


/*! ---- 2nd: do some variable initialization for the found tree line
              linked list                                              ---- */

	  // Start the search for this set of like-pitched planes
	  // TODO (wdc) take a careful look at where TreeSearch should be
	  // instantiated and where BeginSearch and EndSearch should go
	  TreeSearch->BeginSearch();
	  QwTrackingTreeLine* treelinelist = 0; // local list of found tree lines

/*! ---- 3rd: create the bit patterns for the hits                     ---- */

	  // Get the number of tree levels that we want for this detector.
	  int levels = opt.levels[package][region-1][type];

	  /* Region 3 VDC */
	  if (region == kRegionID3 && type == kTypeDriftVDC) {

            dlayer = 0; /* set "number of detectors" to zero            */
	    int decrease;
	    treelines1 = 0; treelines2 = 0;

	    /* Loop over the like-pitched planes in a region */
	    for (k = 0, rd = rcDETRegion[package][region-1][dir], decrease = 0;
	         rd; rd = rd->nextsame, decrease += NUMWIRESR3, k++) {

	      // Print detector info
	      if (debug) cout << "      ";
	      if (debug) rd->print();

	      // Set angles for this direction (U or V)
	      A[dir][0] = rd->rCos; /* cos (angle of wire pitch) */
	      A[dir][1] = rd->rSin; /* sin (angle of wire pitch) */

	      if (debug) cout << "Setting pattern hits (region 3)" << endl;
	      for (int i = 0; i < NUMWIRESR3 + 1; i++) {
	        memset(channelr3[i],     0,                1UL <<  levels      );
	        memset(hashchannelr3[i], 0, sizeof(int) * (1UL << (levels - 1)));
	      }

	      /// Get the sublist of hits in this detector
	      QwHitContainer *sublist = hitlist->GetSubList(rd->ID);
	      // If no hits in this detector, skip to the next detector.
	      if (! sublist) continue;
	      // Loop over the hits in the sublist
	      for (QwHitContainer::iterator hit = sublist->begin();
		hit != sublist->end(); hit++) {

		// See QwTrackingTreeSearch.cc for the different ways in which TsSetPoint
		// can be called.
		int wire = hit->GetDetectorID().fElement;
		TreeSearch->TsSetPoint(
			rcTreeRegion[package*kNumRegions*kNumTypes*kNumDirections
                        +(region-1)*kNumTypes*kNumDirections+type*kNumDirections+dir]->rWidth,
			&(*hit),
			channelr3[wire - decrease],
			hashchannelr3[wire - decrease],
			1U << (levels - 1));

		// Print hit pattern, if requested
		if (opt.showEventPattern) {
		  cout << "w" << wire << ":";
		  for (int i = 0; i < (signed int) (1UL << levels) - 1; i++) {
		    if (channelr3[wire - decrease][i] == 1)
		      cout << "|";
		    else
		      cout << ".";
		    }
		  cout << endl;
		}

	      } // end of loop over hits in this event

	      // Start the search for this set of like-pitched planes
	      TreeSearch->BeginSearch();

	      // All hits in this detector (VDC) are added to the bit pattern.
	      // We can start the tree search now.
	      // NOTE Somewhere around here a memory leak lurks
	      if (debug) cout << "Searching for matching patterns (direction " << dir << ")" << endl;
	      TreeSearch->TsSearch(&(rcTreeRegion[package*kNumRegions*kNumTypes*kNumDirections
                             +(region-1)*kNumTypes*kNumDirections+type*kNumDirections+dir]->node),
				channelr3, hashchannelr3,
				levels, NUMWIRESR3, TLAYERS);
	      treelinelist = TreeSearch->GetListOfTreeLines();

	      // DEBUG section
	      // Did this succeed as intended?
	      // - what does rcTreeRegion->node contain?
	      shortnode* dbg_testnode = &(rcTreeRegion[package*kNumRegions*kNumTypes*kNumDirections
                             +(region-1)*kNumTypes*kNumDirections+type*kNumDirections+dir]->node);
	      // - are all the hits filled?


	      if (debug) cout << "Sort patterns" <<  endl;
              if (rcTreeRegion[package*kNumRegions*kNumTypes*kNumDirections
                             +(region-1)*kNumTypes*kNumDirections+type*kNumDirections+dir]) {

		TreeCombine->TlTreeLineSort (treelinelist, package, region, type, dir,
			1UL << (levels - 1), 0, dlayer, rcTreeRegion);
		if (k == 0) {
		  treelines1 = treelinelist;
		} else if (k == 1) {
		  treelines2 = treelinelist;
		}

	      }
	      dlayer++;

	      // End the search for this set of like-pitched planes
	      TreeSearch->EndSearch();

	    } // end of loop over like-pitched planes in a region

	    if (debug) cout << endl;
	    if (debug) cout << "Matching region 3 segments" << endl;
	    // (wdc) If no treelines1 or treelines2 is found, then skip matching.
	    //       Otherwise this gets confused due to the scintillators.
	    if (treelines1 || treelines2)
	      treelinelist = TreeMatch->MatchR3 (treelines1, treelines2, package, region, dir);
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
	      memset(channelr2[tlayers],     0,                1UL <<  levels      );
	      memset(hashchannelr2[tlayers], 0, sizeof(int) * (1UL << (levels - 1)));

	      /// Get the sublist of hits in this detector
	      QwHitContainer *sublist = hitlist->GetSubList(rd->ID);
	      for (QwHitContainer::iterator hit = sublist->begin(); hit != sublist->end(); hit++)
	        cout << hit->GetDetectorID().fElement << endl;
	      // If no hits in this detector, skip to the next detector.
	      if (! sublist) continue;
	      // Loop over the hits in the sublist
	      for (QwHitContainer::iterator hit = sublist->begin();
		hit != sublist->end(); hit++) {

		// See QwTrackingTreeearch.cc for the different ways in which TsSetPoint
		// can be called.
		int wire = hit->GetDetectorID().fElement;
		TreeSearch->TsSetPoint(
			rcTreeRegion[package*kNumRegions*kNumTypes*kNumDirections
                        +(region-1)*kNumTypes*kNumDirections+type*kNumDirections+dir]->rWidth,
			rd->WireSpacing,
			&(*hit),
			wire,
			channelr2[tlayers],
			hashchannelr2[tlayers],
			1U << (levels - 1));

		// Print hit pattern, if requested
		if (opt.showEventPattern) {
		  cout << "w" << wire << ":";
		  for (int i = 0; i < (signed int) (1UL << levels) - 1; i++) {
		    if (channelr2[tlayers][i] == 1)
		      cout << "|";
		    else
		      cout << ".";
		  }
		  cout << endl;
		}

	      } // end of loop over hits in this event

            } // end of loop over like-pitched planes in a region

	    if (debug) cout << "Search for matching patterns (direction " << dir << ")" << endl;
	    TreeSearch->TsSearch(&(rcTreeRegion[package*kNumRegions*kNumTypes*kNumDirections
                             +(region-1)*kNumTypes*kNumDirections+type*kNumDirections+dir]->node),
				channelr2, hashchannelr2,
				levels, 0, tlayers);
	    treelinelist = TreeSearch->GetListOfTreeLines();

	    if (debug) cout << "Sort patterns" << endl;
            if (rcTreeRegion[package*kNumRegions*kNumTypes*kNumDirections
                             +(region-1)*kNumTypes*kNumDirections+type*kNumDirections+dir]) {
	      TreeCombine->TlTreeLineSort (treelinelist, package, region, type, dir,
					1UL << (levels - 1),
					tlayers, 0, rcTreeRegion);
            }
	    event->treeline[package][region-1][type][dir] = treelinelist;

	    // End the search for this set of like-pitched planes
	    TreeSearch->EndSearch();

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
	 && rcTreeRegion[package*kNumRegions*kNumTypes*kNumDirections
                         +(region-1)*kNumTypes*kNumDirections+type*kNumDirections+kDirectionU]
	 && tlayers) {
	  area = TreeCombine->TlTreeCombine(event->treeline[package][region-1][type],
			1L << (opt.levels[package][region-1][type] - 1),
			package,
			region,
			type,
			tlayers,
			dlayer,
			rcTreeRegion);
	  // TODO (wdc) Haven't changed opt.levels here because would need to
	  // assign it before we know that a detector even exists
	  // (i.e. opt.levels could contain garbage)
	} else continue;


/*! ---- TASK 3: Sort out the Partial Tracks                          ---- */

	if (area) TreeSort->rcPartConnSort(area);

/*! ---- TASK 4: Hook up the partial track info to the event info     ---- */

	event->parttrack[package][region-1][type] = area;

      } /* end of loop over the detector types */

      if (area) {
        cout << "=== Partial track === x,mx / y,my = " << area->x << ", " << area->mx << " / " << area->y << ", " << area->my << endl;
        ngood++;
      } else {
        cout << "Couldn't find a good partial track." << endl;
        nbad++;
      }

    } /* end of loop over the regions */


    /* ==============================
    * Correlate front and back
    * tracks from x, y and y' infor-
    * mation
    * ============================== */



  } /* end of loop over the detector packages */

  if (TreeSearch)  delete TreeSearch;
  if (TreeCombine) delete TreeCombine;
  if (TreeSort)    delete TreeSort;
  if (TreeMatch)   delete TreeMatch;

  return event;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
