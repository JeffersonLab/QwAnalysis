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
  fDebug = 0; // fDebug level

  if( fDebug )
      cout<<"###### Calling QwTrackingWorker::QwTrackingWorker ()"<<endl;

  // Initialize pattern database

  InitTree();

  /* Reset counters of number of good and bad events */
  ngood = 0;
  nbad = 0;

    /* Reserve space for the bit patterns:

       The options file indicates the number of tree levels (how many times do
       we go to finer binning).  The total number of bits is determined using
       bit shift operators:
         (1UL << levels) == 2^levels
         (1UL << (levels - 1)) == 2^(levels-1)
       For e.g. 4 levels we need 1 + 2 + 4 + 8 = 15 = (2^4 - 1) bits.
    */
    int levels;

    /* Reserve space for region 2 bit patterns and initialize */
    levels = opt.levels[kPackageUp][kRegionID2][kTypeDriftHDC];
    for (int i = 0; i < TLAYERS; i++) {
      channelr2[i]     = (char*) malloc (1UL << levels);
      memset (channelr2[i], 0,          (1UL << levels));
      hashchannelr2[i] =  (int*) malloc (sizeof(int) * (1UL << (levels - 1)));
      memset (hashchannelr2[i], 0,      (sizeof(int) * (1UL << (levels - 1))));
    }

    /* Reserve space for region 3 bit patterns and initialize */
    levels = opt.levels[kPackageUp][kRegionID3][kTypeDriftVDC];
    for (int i = 0; i < NUMWIRESR3 + 1; i++) {
      channelr3[i]     = (char*) malloc (1UL << levels);
      memset (channelr3[i], 0,          (1UL << levels));
      hashchannelr3[i] =  (int*) malloc (sizeof(int) * (1UL << (levels - 1)));
      memset (hashchannelr3[i], 0,      (sizeof(int) * (1UL << (levels - 1))));
    }

  if (fDebug)
      cout<<"###### Leaving QwTrackingWorker::QwTrackingWorker ()"<<endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingWorker::~QwTrackingWorker ()
{
  // Free memory reserved for region 2 and region 3 bit patterns
  for (int i = 0; i < TLAYERS; i++) {
    free(channelr2[i]); free(hashchannelr2[i]);
  }
  for (int i = 0; i < NUMWIRESR3 + 1; i++) {
    free(channelr3[i]); free(hashchannelr3[i]);
  }
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
	    std::cerr << "WARN:  rcDETRegion["<< package
		      << "]["          << region
		      << "]["          << direction
		      << "] is NULL.  Should it be?"
		      << std::endl;
	    continue;
	  }

	  /// Region 3 contains 8 layers
	  if (region == kRegionID3) {
	    numlayers = 8; // TODO replace this with info from the geometry file
	    width = rcDETRegion[package][region][direction]->width[2];
	  }

	  /// Region 2 contains 4 layers
	  if (region == kRegionID2) {
	    numlayers = 4; // TODO replace this with info from the geometry file
	    width = rcDETRegion[package][region][direction]->WireSpacing *
		    rcDETRegion[package][region][direction]->NumOfWires;
	  }

	  /// Set up the filename with the following format
	  ///   tree[numlayers]-[levels]-[u|l]-[1|2|3]-[d|g|t|c]-[n|u|v|x|y].tre
	  sprintf(filename, "tree%d-%d-%c-%c-%c-%c.tre",
		numlayers,
		opt.levels[package][region][type],
		"0ud"[package],
		"0123TCS"[region],
		"0hvgtc"[type],
		"0xyuvrq"[direction]);
	  if (fDebug) cout << "Tree filename: " << filename << endl;

	  /// Each element of fSearchTree will point to a pattern database
//	  fSearchTree[package][region][type][direction] =
          int index = package*kNumRegions*kNumTypes*kNumDirections
                      +region*kNumTypes*kNumDirections
                      +type*kNumDirections+direction;
	  fSearchTree[index] = thetree->inittree(filename,
		opt.levels[package][region][type],
		numlayers,
		width,
		package,
		type,
		region,
		direction);

          if (fDebug){

	  //cout << "fSearchTree(" << package << ","
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

double rcShootP (double Pest,QwPartialTrack *front, QwPartialTrack *back, double accuracy)
{
  std::cerr << "Warning: The function rcShootP is only a stub" << std::endl;
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
  double A[kNumDirections][2];	/* conversion between xy and uv */

  QwEvent *event = new QwEvent(); // event structure

  // Tracking functionality is provided by these four sub-blocks.
  QwTrackingTreeSearch  *TreeSearch  = new QwTrackingTreeSearch();
  TreeSearch->SetDebugLevel(fDebug);
  QwTrackingTreeCombine *TreeCombine = new QwTrackingTreeCombine();
  TreeCombine->SetDebugLevel(fDebug);
  QwTrackingTreeSort    *TreeSort    = new QwTrackingTreeSort();
  TreeSort->SetDebugLevel(fDebug);
  QwTrackingTreeMatch   *TreeMatch   = new QwTrackingTreeMatch();
  TreeMatch->SetDebugLevel(fDebug);

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
    if (fDebug) cout << "[QwTrackingWorker::ProcessHits] Package: " << package << endl;

    // Currently assume that only the up and down octants contain the tracking
    // detectors.  When rotation is included, this will have to be modified to
    // take into account that the tracking detectors are in different octants.
    if (package != kPackageUp && package != kPackageDown) continue;

    // TODO (wdc) Also, only tracks in the up octant detector are available in
    // the MC generated hit lists.  Therefore we throw out the down octant.

    // TODO jpan: The Geant4 now can generate any octant data, you just need to command it through
    // a macro file. I'll put the tracking detector ratation commands onto the UI manu later.
    if (package != kPackageUp) continue;

    /// Loop through the detector regions
    for (EQwRegionID region  = kRegionID2;
		     region <= kRegionID3; region++) {
      if (fDebug) cout << "[QwTrackingWorker::ProcessHits]  Region: " << region << endl;


      /// Loop over the detector types (only drift chambers are used)
      for (EQwDetectorType type  = kTypeDriftHDC;
			   type <= kTypeDriftVDC; type++) {
	if (fDebug) cout << "[QwTrackingWorker::ProcessHits]   Detector: " << type << endl;


	/// Loop through the detector directions
	for (EQwDirectionID dir  = kDirectionX;
			    dir <= kDirectionV; dir++) {
          if (fDebug) cout << "[QwTrackingWorker::ProcessHits]    Direction: " << dir << endl;

	  // Skip wire direction Y for region 2
	  if (region == kRegionID2
	      && dir == kDirectionY) continue;
	  // Skip wire direction X and Y for region 3
	  if (region == kRegionID3
	      && (dir == kDirectionX || dir == kDirectionY)) continue;

	  // Check whether there are any detectors defined in that geometry
	  if (! rcDETRegion[package][region][dir]) {
	    if (fDebug) cout << "[QwTrackingWorker::ProcessHits]     No such detector!" << endl;
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
	    if (fDebug) cout << "[QwTrackingWorker::ProcessHits]     No such tree!" << endl;
	    continue;
	  }

	  // Check whether the search tree is searchable
	  if (searchtree->searchable == false) {
	    event->treeline[package][region][type][dir] = 0;
	    if (fDebug) cout << "[QwTrackingWorker::ProcessHits]     Search tree not searchable!" << endl;
	    continue;
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
	  int levels = opt.levels[package][region][type];

	  /* Region 3 VDC */
	  if (region == kRegionID3 && type == kTypeDriftVDC) {

            dlayer = 0; /* set "number of detectors" to zero            */
	    QwTrackingTreeLine *treelinelist1 = 0, *treelinelist2 = 0;

	    // Set hit pattern for this region
	    if (fDebug) cout << "Setting hit pattern (region 3)" << endl;

	    /* Loop over the like-pitched planes in a region */
	    int plane = 0;
	    for (Det* rd = rcDETRegion[package][region][dir];
	              rd; rd = rd->nextsame, plane++) {

	      // Print detector info
	      if (fDebug) cout << "      ";
	      if (fDebug) rd->print();

	      // If detector is inactive for tracking, skip it
	      if (rd->IsInactive()) continue;

	      // Set angles for this direction (U or V)
	      A[dir][0] = rd->rCos; /* cos (angle of wire pitch) */
	      A[dir][1] = rd->rSin; /* sin (angle of wire pitch) */

	      // Reset hit pattern to zero for this layer
	      for (int i = 0; i < NUMWIRESR3 + 1; i++) {
	        memset(channelr3[i],     0,                1UL <<  levels      );
	        memset(hashchannelr3[i], 0, sizeof(int) * (1UL << (levels - 1)));
	      }

	      /// Get the subhitlist of hits in this detector
	      QwHitContainer *subhitlist = hitlist->GetSubList_Plane(region, package, rd->plane);
	      if (fDebug) subhitlist->Print();
	      // If no hits in this detector, skip to the next detector.
	      if (! subhitlist) continue;

	      // Loop over the hits in the subhitlist
	      for (QwHitContainer::iterator hit = subhitlist->begin();
		hit != subhitlist->end(); hit++) {

		// See QwTrackingTreeSearch.cc for the different ways in which TsSetPoint
		// can be called.
		int wire = hit->GetElement();
		TreeSearch->TsSetPoint(
			searchtree->rWidth,
			&(*hit),
			channelr3[wire],
			hashchannelr3[wire],
			1UL << (levels - 1));

		// Print hit pattern, if requested
		if (opt.showEventPattern) {
		  cout << "w" << wire << ":";
		  for (int i = 0; i < (signed int) (1UL << levels) - 1; i++) {
		    if (channelr3[wire][i] == 1)
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
	      if (fDebug) cout << "Searching for matching patterns (direction " << dir << ")" << endl;
	      TreeSearch->TsSearch(&(searchtree->node),
				channelr3, hashchannelr3,
				levels, NUMWIRESR3, TLAYERS);
	      treelinelist = TreeSearch->GetListOfTreeLines();
	      if (fDebug) {
	        cout << "List of treelines:" << endl;
	        treelinelist->Print();
	      }

	      if (fDebug) cout << "Sort patterns" <<  endl;
              if (fSearchTree[package*kNumRegions*kNumTypes*kNumDirections
                             +region*kNumTypes*kNumDirections+type*kNumDirections+dir]) {

		double width = searchtree->rWidth;
		TreeCombine->TlTreeLineSort (treelinelist, subhitlist,
					package, region, type, dir,
					1UL << (levels - 1), 0, dlayer, width);

		if (plane == 0) {
		  treelinelist1 = treelinelist;
		} else if (plane == 1) {
		  treelinelist2 = treelinelist;
		}

	      }
	      dlayer++;

	      // End the search for this set of like-pitched planes
	      TreeSearch->EndSearch();

	      // Delete subhitlist
	      delete subhitlist;

	    } // end of loop over like-pitched planes in a region

	    if (fDebug) cout << endl;
	    if (fDebug) cout << "Matching region 3 segments" << endl;
	    // (wdc) If no treelinelist1 or treelinelist2 is found, then skip matching.
	    //       Otherwise this gets confused due to the scintillators.
	    if (treelinelist1 || treelinelist2) {
	      if (fDebug) {
	        cout << "VDC1:" << endl; treelinelist1->Print();
	        cout << "VDC2:" << endl; treelinelist2->Print();
	      }
	      treelinelist = TreeMatch->MatchR3 (treelinelist1, treelinelist2, package, region, dir);
	    }
	    event->treeline[package][region][type][dir] = treelinelist;
	    tlayers = TLAYERS;     /* remember the number of tree-detector */
	    tlaym1  = tlayers - 1; /* remember tlayers - 1 for convenience */



	  /* Region 2 HDC */
	  } else if (region == kRegionID2 && type == kTypeDriftHDC) {

	    // Set ht pattern for this region
	    if (fDebug) cout << "Setting hit pattern (region 2)" << endl;

	    /* Loop over the like-pitched planes in a region */
	    tlayers = 0;
	    for (Det* rd = rcDETRegion[package][region][dir];
	              rd; rd = rd->nextsame, tlayers++) {

	      // Print detector info
	      if (fDebug) rd->print();

	      // If detector is inactive for tracking, skip it
	      if (rd->IsInactive()) continue;

	      // Reset hit pattern to zero for this layer
	      memset(channelr2[tlayers],     0,                1UL <<  levels      );
	      memset(hashchannelr2[tlayers], 0, sizeof(int) * (1UL << (levels - 1)));

	      // Get the subhitlist of hits in this detector plane
	      QwHitContainer *subhitlist = hitlist->GetSubList_Plane(region, package, rd->plane);
	      if (fDebug) subhitlist->Print();
	      // If no hits in this detector, skip to the next detector.
	      if (! subhitlist) continue;
	      // Loop over the hits in the subhitlist
	      for (QwHitContainer::iterator hit = subhitlist->begin();
		hit != subhitlist->end(); hit++) {

		// See QwTrackingTreeearch.cc for the different ways in which TsSetPoint
		// can be called.
		int wire = hit->GetElement();
		TreeSearch->TsSetPoint(
			searchtree->rWidth,
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

	      // Delete subhitlist
	      delete subhitlist;

            } // end of loop over like-pitched planes in a region

	    if (fDebug) cout << "Search for matching patterns (direction " << dir << ")" << endl;
	    TreeSearch->TsSearch(&(searchtree->node),
				channelr2, hashchannelr2,
				levels, 0, tlayers);
	    treelinelist = TreeSearch->GetListOfTreeLines();
	    if (fDebug) {
	      cout << "List of treelines:" << endl;
	      treelinelist->Print();
	    }

	    // Get the hit list for this package/region/direction
	    QwHitContainer *subhitlist = hitlist->GetSubList_Dir(region, package, dir);
	    if (fDebug) subhitlist->Print();

	    if (fDebug) cout << "Sort patterns" << endl;
            if (searchtree) {

	      double width = searchtree->rWidth;
	      TreeCombine->TlTreeLineSort (treelinelist, subhitlist,
					package, region, type, dir,
					1UL << (levels - 1),
					tlayers, 0, width);
	    }
	    event->treeline[package][region][type][dir] = treelinelist;
	    if (fDebug) {
	      cout << "List of treelines:" << endl;
	      treelinelist->Print();
	    }

	    // End the search for this set of like-pitched planes
	    TreeSearch->EndSearch();

	    // Delete subhitlist
	    delete subhitlist;

	  /* Any other region */
	  } else {
	    std::cerr << "[QwTrackingWorker::ProcessHits] Warning: no support for this detector." << std::endl;
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
			1L << (opt.levels[package][region][type] - 1),
			package, region, type,
			tlayers,
			dlayer,
			fSearchTree);
	  // TODO (wdc) Haven't changed opt.levels here because would need to
	  // assign it before we know that a detector even exists
	  // (i.e. opt.levels could contain garbage)

	} else continue;


/*! ---- TASK 3: Sort out the Partial Tracks                          ---- */

	if (parttrack) TreeSort->rcPartConnSort(parttrack);

/*! ---- TASK 4: Hook up the partial track info to the event info     ---- */

	event->parttrack[package][region][type] = parttrack;

        if (parttrack) {
	  if (fDebug) parttrack->Print();
          ngood++;
        } else {
          cout << "Couldn't find a good partial track in region " << region << endl;
          nbad++;
        }

      } /* end of loop over the detector types */

    } /* end of loop over the regions */


    /* ==============================
    * Correlate front and back
    * tracks from x, y and y' infor-
    * mation
    * ============================== */



  } /* end of loop over the detector packages */


  // Delete tracking objects
  if (TreeSearch)  delete TreeSearch;
  if (TreeCombine) delete TreeCombine;
  if (TreeSort)    delete TreeSort;
  if (TreeMatch)   delete TreeMatch;

  // Return the event structure
  return event;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
