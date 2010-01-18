//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
// C++ Interface: QwTrackingTreeSearch
//
// Description:
//
//
// Author: Burnham Stocks <bestokes@jlab.org>
// Original HRC Author: wolfgang Wander <wwc@hermes.desy.de>
//
// Modified by: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//              Jie Pan <jpan@jlab.org>, Mon May 25 10:48:12 CDT 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \class QwTrackingTreeSearch

    \file QwTrackingTreeSearch.cc
--------------------------------------------------------------------------*\

 PROGRAM: QTR (Qweak Track Reconstruction)       AUTHOR: Burnham Stokes
                                                          bestokes@jlab.org
						ORIGINAL HRC AUTHOR
							Wolfgang Wander
							wwc@hermes.desy.de


  MODULE: QwTrackingTreeSearch.c                         COMMENTS: Brendan Fox
                                                         foxb@hermes.desy.de

 PURPOSE: This module contains the code for performing the treesearch
          algorithm to generate one treeline.  The code first use the hit
          information for the planes to construct the bit pattern for each
          tree-planes.  Then, it searches through the treesearch database
          and identifies each treenode in the database which is present
          in the bit patterns for the tree-planes.  These treenodes are
          then used to generate the link-listed of possible treelines.

 CONTENTS: (brief description for now)

 (01) wireselection() - this function is called by TsSetPoints.  It steps
                        through the hits from the unprimed and primed planes
                        for a tree-plane to decide whether hits should or
                        should not be paired together when the hit pattern
                        for the tree-plane is constructed.

 (02) _setpoints()    - this function sets the bins in a hit pattern for
                        a range of positions.  The range of hit patterns
                        is specified by a start and a stop position in
                        the detector.  This function turns on the bins
                        in the hit pattern for each level of the
                        bin-division used in the treesearch algorithm.

 (03) _setpoint()     - this function sets the bins in the hit pattern for
                        a range of positions around a central point within
                        a specified distance/resolution by calling the
                        _setpoints() function.

 (04) setpoint()      - this function sets the bins in the hit pattern for a
                        range of positions specified by a center point and
                        a half-distance around the center point by calling
                        the setpoint() function.

 (05) TsSetPoint()    - this function sets the bins in the hit pattern for
                        a range of positions around a central point within
                        a specified distance/resolution.  This function
                        turns on the bins in the hit pattern for each level
                        of the bin-division in the treesearch algorithm.

 (06) exists()        - this function searches through the link-list of
                        valid treelines to see if the bit pattern for the
                        specified treenode has already been accepted as
                        a valid treeline.

 (07) _SearchTreeLines() - this highly recursive function implements the
                        treesearch algorithm.  For a specified list of
                        nodenodes, this function examines the attached
                        treenode.  If the bit pattern in the treenode
                        does not match the bit pattern from the event,
                        the function looks at the next nodenode.  Otherwise,
                        the function will call itself to see if any of
                        the sons of this treenode at the next level of
                        bin-division match the bit pattern from the event.
                        This recursive calling will continue until a
                        treenode at the deepest level of bin-division is
                        located inside the bit pattern from the event.
                        Since the pattern in this treenode is represents
                        a valid treeline for the event, a treeline is
                        constructed from the treenode and then appended
                        to the linked list of treelines being accumulated
                        by the treesearch.

 (08) SearchTreeLines() - this function initiates the treesearch for a set
                        of tree-planes by calling the _SearchTreeLines() function
                        described above.

    $date: Mon May 25 10:48:12 CDT 2009 $

\brief This module contains the code for performing the treesearch
          algorithm to generate one treeline.
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwTrackingTreeSearch.h"

// Standard C and C++ headers
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>

// Qweak headers
#include "QwLog.h"
#include "globals.h"
#include "QwHit.h"
#include "QwDetectorInfo.h"
#include "QwTrackingTreeLine.h"
#include "QwTrackingTreeRegion.h"

// Qweak tracking tree headers
#include "shortnode.h"
#include "shorttree.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

static int _hashgen = 1;

static int hashgen(void) {
  _hashgen += 2;
  _hashgen &= 0x7ffffff;
  return _hashgen;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeSearch::QwTrackingTreeSearch ()
{
  fDebug = 1;	// Reset debug level

  fNumLayers = TLAYERS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeSearch::~QwTrackingTreeSearch ()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeLine* QwTrackingTreeSearch::GetListOfTreeLines ()
{
  return lTreeLines;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/**
  wireselection() - this function steps through the hits from the unprimed
                    and primed planes for a tree-plane to decide whether
                    hits should or should not be paired together when the
                    hit pattern for the tree-plane is constructed.

    inputs: (1) Hit **x        - pointer to the hit in the linked list of
                                 hits for the unprimed plane at which the
                                 scan is to start.
            (2) Hit **X        - pointer to the hit in the linked list of
                                 hits for the primed plane at which the
                                 scan is to start.
            (3) double maxdist - maximum separation between hits on the
                                 primed and unprimed hits for these hits
                                 to paired together when making the bit
                                 pattern is formed.

   outputs: (1) Hit **x        - pointer to the hit in the linked list of
                                 for the unprimed plane which should be
                                 used for generating the bit pattern; =0 if
                                 the hit on the unprimed plane should not
                                 be used at all.
            (2) Hit **X        - pointer to the hit in the linked list of
                                 for the primed plane which should be
                                 used for generating the bit pattern; =0 if
                                 the hit on the primed plane should not
                                 be used at all.
            (3) Hit **xn       - pointer to the next hit to consider for
                                 the unprimed plane
            (4) Hit **Xn       - pointer to the next hit to consider for
                                 the primed plane

 */
void QwTrackingTreeSearch::wireselection (QwHit **x, QwHit **X, QwHit **xn, QwHit **Xn, double maxdist)
{
  std::cerr << "[QwTrackingTreeSearch::wireselection] Warning: This needs revision!" << std::endl;

  double wireDistance1;
  double wireDistance2;

  if (*x && *X) { /* there are hits on both primed and unprimed planes */

  /* ---- compute the distance between the hits on the two planes      ---- */

    wireDistance2 = (*x)->rPos2 - (*X)->GetDriftDistance(); /* unprimed left-primed right */
    wireDistance1 = (*x)->GetDriftDistance() - (*X)->rPos2; /* unprimed right-primed left */

    if (wireDistance2 < -maxdist) {

    /* ---- CASE 1: the unprimed hit cannot be paired with any of the
                    primed hits.  So, it needs to be considered by
                    itself as the bit pattern is constructed.  Also,
                    the next scan should begin with the next hit on
		    this unprimed plane and see if it can be paired
                    with the current hit on the primed plane.          ---- */

      *Xn = *X;            /* keep current point as starting point for
                              the primed hits in the next scan            */
      *xn = (*x)->nextdet; /* use next hit as the starting point for the
                              unprimed hits in the next scan              */
      *X  = 0;             /* no primed match, so return the unprimed hit
                              for use as the bit pattern is generated.    */
    } else if (wireDistance1 > maxdist) {

    /* ---- CASE 2: the primed hit cannot be paired with any of the
                    unprimed hits.  So, it needs to be considered by
                    itself as the bit pattern is constructed.  Also,
                    the next scan should begin with the next hit on
		    this primed plane and see if it can be paired
		    with the current hit on the unprimed plane.        ---- */

      *xn = *x;            /* keep current point as starting point for
                              the unprimed hits in the next scan          */
      *Xn = (*X)->nextdet; /* use next hit as the starting point for the
                              primed hits in the next scan                */
      *x  = 0;             /* no unprimed match, so return the primed hit
                              for use as the bit pattern is generated.    */
    } else {

    /* ---- CASE 3: the primed hit and the unprimed hit are paired.
                    So, both need to be considered as the hit pattern
		    is constructed.  Also, the next scan should begin
		    with the next hit on each of these planes.         ---- */

      *xn = (*x)->nextdet; /* use next hit as the starting point for the
                              unprimed hits in the next scan             */
      *Xn = (*X)->nextdet; /* use next hit as the starting point for the
                              primed hits in the next scan               */
    }
  } else if (*x) { /* only hits on primed plane   */
    *Xn = 0;             /* no unprimed plane for the next scan        */
    *xn = (*x)->nextdet; /* use next hit as the starting point for the
                            primed hits in the next scan               */
  } else if (*X) { /* only hits on unprimed plane */
    *xn = 0;             /* no primed plane for the next scan          */
    *Xn = (*X)->nextdet; /* use next hit as the starting point for the
                            primed hits in the next scan               */
  } else                 /* no hits on either plane     */
    *xn = *Xn = 0;       /* no next scan! */

  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo...
/**
  _setpoints() - this function sets the bins in a hit pattern for a
                 range of positions.  The range of hit patterns is specified
                 by a start and a stop position in the detector.  This
                 function turns on the bins in the hit pattern for each
                 level of the bin-division used in the treesearch algorithm.

    inputs: (1) double posStart      - position (in cm) of the start of the
                                       range for which bins are to be turned
                                       on
            (2) double posEnd        - position (in cm) of the stop of the
                                       range for which bins are to be turned
                                       on
            (3) double detectorwidth - width of the tree-detector (in cm)
            (4) unsigned binwidth    - width of a bin at the deepest level
                                       of bin-division in the treesearch.
            (5) char *pattern        - pointer to the hit pattern for this
                                       tree-detector
            (6) int  *hash           - pointer to ???

   outputs: (1) char *pattern        - pointer to the hit pattern for this
                                       tree-detector
            (2) int  *hash           - pointer to ???

*/
void QwTrackingTreeSearch::_setpoints (
	double pos_start,
	double pos_end,
	double detectorwidth,
	unsigned binwidth,
	char *pattern,
	int  *hash)
{
  int ia, ie, hashint = hashgen();
  unsigned oldwidth = binwidth;
  char *oldpattern = pattern;

/* ---- compute the first bin in the deepest tree level to turn on     ---- */

  ia = (int) floor (pos_start / detectorwidth * binwidth);

/* ---- compute the last bin in the deepest tree level to turn on      ---- */

  ie = (int) floor (pos_end   / detectorwidth * binwidth);

/* ---- step through each of the bins at the deepest bin-division
        level in the hit pattern and turn on the bits in the
        pattern at all the bin-division levels for this bin.           ---- */

  for (int j = ia; j <= ie; j++) { /* loop over the bins to be set */

    int i = j;  /* remember the bin at the deepest level which is being turn on */

    binwidth = oldwidth;   /* the size of the bit pattern for the detector
                              at the deepest level of bin-division.         */
    pattern  = oldpattern; /* pointer to start of the bit pattern           */

/* ---- check if the bin is inside the detector                        ---- */
    	//I added "(signed int)" to the
	//following line to prevent the warning from comparing signed and
	//unsigned integers
    if (i >= (signed int) binwidth)
      return;
    if (i < 0)
      continue;
/* ---- compute some hash value                                        ---- */
    hash[i] = ((hash[i]<<1) + hashint)|1 ;
/* ---- set the bits in the bit pattern at all depths of bin-division  ---- */

    if (pattern) {
      while (binwidth) {       /* starting at maximum depth, loop over
                                  each depth of the treesearch           */
	pattern[i] = 1;        /* turn on the bit in this bin            */
	pattern   += binwidth; /* set ahead to the part of the bit
                                  pattern in which the bits for the
                                  next higher bin-division are stored    */
	i        >>= 1;        /* go up one depth of the depth           */
	binwidth >>= 1;        /* size of the bit pattern for the next
                                  higher bin-division is half of the
                                  size of the bit pattern for a given
                                  level of bin-division.                 */

      } /* end of loop over the depths of the treesearch */
    }
  } /* end of loop over the bins to be set */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/**
  _setpoint() - this function sets the bins in the hit pattern for a
                range of positions around a central point within a specified
                distance/resolution by calling the setpoint() function
                described above.

    inputs: (1) double position      - position (in cm) of the center point
                                       around which the bins are to be turned
                                       on
            (2) double resolution    - distance (in cm) around the central
                                       point for which bins are to be turned
                                       on
            (3) double detectorwidth - width of the tree-detector (in cm)
            (4) unsigned binwidth    - width of a bin at the deepest level
                                       of bin-division in the treesearch.
            (5) char *pattern        - pointer to the hit pattern for this
                                       tree-detector
            (6) int  *hash           - ???

   outputs: (1) char *pattern        - pointer to the hit pattern for this
                                       tree-detector
            (2) int  *hash           - pointer to ???

 */
void QwTrackingTreeSearch::_setpoint (
	double position,
	double resolution,
	double detectorwidth,
	unsigned binwidth,
	char *pattern,
	int *hash)
{
  _setpoints (position - resolution, position + resolution,
              detectorwidth, binwidth, pattern, hash);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/**
  setpoint() - this function sets the bins in the hit pattern for a range
               of positions specified by a center point and a half-distance
               around the center point by calling the _setpoint() function
               described above.

               If hit patterns are supplied to this function for both the
               primed and unprimed planes, the two planes are treated as
               separate tree-detectors.  In this case, the hit patterns
               are updated for each plane individually.  This feature is
               used for the front region when the VCs are not included
               in the fit because, for these fits, each FC plane is treated
               as a single tree-detector.

               Otherwise, the two planes are treated as a single
               tree-detector and a single hit pattern is formed from the
               hits seen in the two planes.

    inputs: (01) double off        - alignment offset of the
                                     tree-detector (in cm)
            (02) double h1         - center point of hit range on
                                     plane 1 (in cm)
            (03) double res1       - width of hit range on plane 1 (in cm)
            (04) double h2         - center point of hit range on
                                     plane 2 (in cm)
            (05) double res2       - width of hit range on plane 2 (in cm)
            (06) double width      - width of the tree-detector (in cm)
            (07) unsigned binwidth - width of a bin at the deepest level
                                     of bin-division in the treesearch.
            (08) char *pa          - pointer to the hit pattern for
                                     tree-detector 1
            (09) char *pb          - pointer to the hit pattern for
                                     tree-detector 2, =0 if there is no
                                     tree-detector 2 because the planes are
                                     being combined into one tree-detector
            (10) int  *hasha       - pointer to ???
            (11) int  *hashb       - pointer to ???

   outputs: (01) char *pa          - pointer to the updated hit pattern
                                     for tree-detector 1
            (02) char *pb          - pointer to the updated hit pattern for
                                     tree-detector 2, = 0 if there is no
                                     tree-detector 2 because the planes are
                                     being combined into one tree-detector
            (03) int  *hasha       - pointer to ???
            (04) int  *hashb       - pointer to ???

 */
void QwTrackingTreeSearch::setpoint (
	double off,
	double h1,
	double res1,
	double h2,
	double res2,
	double width,
	unsigned binwidth,
	char *pa,
	char *pb,
	int *hasha,
	int *hashb)
{
  if (pb) {  /* NOT combining two planes into one tree-detector, so
                set bins in the hit pattern for each plane separately */
    _setpoint (off + h1, res1, width, binwidth, pa, hasha);
    _setpoint (off + h2, res2, width, binwidth, pb, hashb);
  } else     /* Combining two planes into one tree-detector, so just
                set the bins for the overlap between the hit on the
                two planes.  Here, the resolutions are added linearly
                instead of in quadrature to save cputime              */
    _setpoint (off + 0.5 * (h1+h2), 0.5 * (res1+res2), width, binwidth, pa, hasha);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/**
  TsSetPoint() - this function creates the bit patterns for two partner
                 planes (planes with the like-pitched wires in the
                 same chamber.

                 This function can treat the two planes in two different
                 fashions, specifically:

                 (1) the planes can be treated as one tree-detector.  In
                     this case, a check is performed to see if a hit on
                     one of the planes can be matched to a hit on the
                     other plane (within the allowances of the maximum
                     track slope specified by the HRCset parameter
                     MaxSlope).  If the hits are paired together by this
                     search, the bit pattern is set around the allowable
                     tracks through both hits.  If a hit is not paired
                     in this search, the bit pattern is set for the region
                     around the hit.  (This method is the standard for
                     the back chambers since they have eight planes per
                     treeline).

                 (2) the planes are treated as individual tree-detectors.
                     In this case, a separate hit pattern is created for
                     both planes and the above described searching for
                     paired hits is not employed.  (This method is the
                     standard for the front chambers since they have only
                     four planes per treeline).

    inputs: (01) double detectorwidth - width of the tree-detector (in cm)
            (02) double zdistance     - distance between the two planes
                                        forming this tree-detector (in cm)
            (03) Hit *Ha              - linked list of hits for the unprimed
                                        plane of this tree-detector
            (04) Hit *Hb              - linked list of hits for the primed
                                        plane of this tree-detector
	    (05) unsigned binwidth    - width of a bin (in cm) at the deepest
                                        bin-division in the treesearch.

   outputs: (01) int TsSetPoint()     - the number of single and paired hits
                                        processed by this return
            (02) char *patterna       - pointer to the hit pattern for
                                        tree-detector
            (03) char *patternb       - pointer to the hit pattern for
                                        the optional second tree-detector
            (04) int  *hasha          - pointer to ???
            (05) int  *hashb          - pointer to ???

 */
// This TsSetPoint version is designed for setting the pattern
// for one hit at a time, using the QwHit class.
int QwTrackingTreeSearch::TsSetPoint (
	double detectorwidth,
	QwHit *hit,
	char *pattern,
	int *hash,
	unsigned binwidth)
{
  double dw2 = (detectorwidth / 2.0); /* half-width of the detector (in cm) */

  // Set the points on the front/top side of the wire (R3/R2)
  _setpoints(dw2 - hit->GetDriftDistance() - hit->GetTrackResolution(),
	     dw2 - hit->GetDriftDistance() + hit->GetTrackResolution(),
	     detectorwidth, binwidth, pattern, hash);

  // Set the points on the back/bottom side of the wire (R3/R2)
  _setpoints(dw2 + hit->GetDriftDistance() - hit->GetTrackResolution(),
	     dw2 + hit->GetDriftDistance() + hit->GetTrackResolution(),
	     detectorwidth, binwidth, pattern, hash);

  return 1;
}

// This TsSetPoint version is designed for setting the pattern for Region 2
// doing one hit at a time, using the QwHit class.
int QwTrackingTreeSearch::TsSetPoint (
	double detectorwidth,
	double wirespacing,
	QwHit *hit,
	char *pattern,
	int *hash,
	unsigned binwidth)
{
  // Get the wire number
  int wire = hit->GetElement();
  wirespacing = hit->GetDetectorInfo()->GetElementSpacing();

  // Set the points on the front/top side of the wire (R3/R2)
  _setpoints(wirespacing * (wire+1) - hit->GetDriftDistance() - hit->GetTrackResolution(),
	     wirespacing * (wire+1) - hit->GetDriftDistance() + hit->GetTrackResolution(),
	     detectorwidth, binwidth, pattern, hash);

  // Set the points on the back/bottom side of the wire (R3/R2)
  _setpoints(wirespacing * (wire+1) + hit->GetDriftDistance() - hit->GetTrackResolution(),
	     wirespacing * (wire+1) + hit->GetDriftDistance() + hit->GetTrackResolution(),
	     detectorwidth, binwidth, pattern, hash);

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
// (jp) This is the QwHit version for setting the paired hits
// TsSetPoint(double,double,QwHit*,QwHit*,char*,char*,int*,int*,unsigned)

int QwTrackingTreeSearch::TsSetPoint (
	double detectorwidth,
	double zdistance,
	QwHit *Ha,
	QwHit *Hb,
	char *patterna,
	char *patternb,
	int *hasha,
	int *hashb,
	unsigned binwidth)
{
  std::cerr << "[QwTrackingTreeSearch::TsSetPoint] Warning: This needs revision!" << std::endl;

  int num = 0;


  double dw2 = (detectorwidth / 2.0); /* half-width of the detector (in cm) */
  QwHit *Hanext, *Hbnext;

/* ---- Compute the maximum separation between hits on the two
        planes such that these hits are paired together when
        forming the hit pattern.  If the hits are separated by
        more than this distance, the hits are treated as unmatched
        hits when the hit pattern is formed.                        ---- */

  double rcSETrMaxSlope = 0.60;//THIS VALUE COMES FROM hrcset.h
  std::cerr << "rcSET not defined in the code. Error. CODE NEEDS REPLACING" << std::endl;


  /*double maxdistance = rcSET.rMaxSlope * zdistance; */ /* maximum separation
						       for hits (in cm)   */
  double maxdistance = rcSETrMaxSlope * zdistance;//I inserted this
	//line to fill in the gap due to the rcSET object not being
	//defined.

/* ---- Go through the hit lists for each plane and set the hit
        pattern.  For each set of paired hits, the hit pattern
        is updated with the overlap between the hits.  For the
        unpaired hits, the hit pattern is updated with just the
        possible tracks through the hit.                            ---- */

  while (Ha || Hb) { /* for each hit in Ha and Hb */
    num++;           /* Keep count of how many hits are processed */

/* ---- Call wireselection() to pick out a hit pair from the
        linked lists of hits for the two planes (*Ha and *Hb)

        This function searches for hits in plane A (stored in the
        linked list of hits) starting with the hit pointed to by
        *Ha and plane B starting with the hit pointed to by *Hb.
        Its returns: (1) pointers to the next hits to consider
        for pairing (*Hanext and *Hbnext) and (2) links to the
        hits (in *Ha and *Hb) to be inserted into the hit
        pattern (for unpair hits, either *Ha or *Hb will be zero    ---- */

    wireselection (&Ha, &Hb, &Hanext, &Hbnext, maxdistance);

/* ---- Update the hit pattern to include these hits                ---- */

    if (Ha && Hb) {

/* ---- CASE 1: If the planes have hits within the HRCset Maxslope,
                then resolve the left-right ambiguity by checking
                the four combinations for matching.  When a
                combination matches, the hit pattern is updated by
                treating the hits together.  If no matching
                combinations are found, the hit pattern is updated
                by treated the hits separately.                     ---- */

      int found = 0; /* clear "I found a pairing" flag */
      if (fabs(Ha->GetDriftDistance() - Hb->GetDriftDistance()) < maxdistance){ /* left A w/ left B */
	found = 1; /* set "I found a pairing" flag */
	setpoint (dw2,
		  Ha->GetDriftDistance(), Ha->GetTrackResolution(),
		  Hb->GetDriftDistance(), Hb->GetTrackResolution(),
		  detectorwidth, binwidth, patterna, patternb,
		  hasha, hashb);
      }
      if (fabs(Ha->rPos2 - Hb->GetDriftDistance()) < maxdistance) { /* rght A w/left B */
	found = 1; /* set "I found a pairing" flag */
	setpoint (dw2,
		  Ha->rPos2, Ha->GetTrackResolution(),
		  Hb->GetDriftDistance(), Hb->GetTrackResolution(),
		  detectorwidth, binwidth, patterna, patternb,
		  hasha, hashb);
      }
      if (fabs(Ha->GetDriftDistance() - Hb->rPos2) < maxdistance) { /* left B w/rght B */
	found = 1; /* set "I found a pairing flag */
	setpoint (dw2,
		  Ha->GetDriftDistance(), Ha->GetTrackResolution(),
		  Hb->rPos2, Hb->GetTrackResolution(),
		  detectorwidth, binwidth, patterna, patternb,
		  hasha, hashb);
      }
      /* 270996 - correct Hb to Ha below - finally ,-) */
      if (fabs(Ha->rPos2 - Hb->rPos2) < maxdistance) { /* rght A w/rght B */
	found = 1; /* set "I found a pairing" flag */
	setpoint (dw2,
		  Ha->rPos2, Ha->GetTrackResolution(),
		  Hb->rPos2, Hb->GetTrackResolution(),
		  detectorwidth, binwidth, patterna, patternb,
		  hasha, hashb);
      }

/* ---- CASE 2: The search by wireselection() paired these two
                hits, but the left-right ambigiuity check failed
                find a combination trough which an allowable track
                could traverse.  So, these hits are added into
                bit pattern as unpaired hits.                       ---- */

      if (! found) {
	if (patternb) {  /* treating the two planes as individual
                            tree-planes, so put hit into both patterns */
	  setpoint (dw2,
		    Ha->GetDriftDistance(), Ha->GetTrackResolution(),
		    Hb->GetDriftDistance(), Hb->GetTrackResolution(),
		    detectorwidth, binwidth, patterna, patternb,
		    hasha, hashb);
	  setpoint (dw2,
		    Ha->rPos2, Ha->GetTrackResolution(),
		    Hb->rPos2, Hb->GetTrackResolution(),
		    detectorwidth, binwidth, patterna, patternb,
		    hasha, hashb);
	} else {         /* treating the two planes as one tree-plane,
                            so just put it into the one pattern        */
	  _setpoints (Ha->GetDriftDistance()+dw2 - 0.5 * maxdistance - Ha->GetTrackResolution(),
		      Ha->GetDriftDistance()+dw2 + 0.5 * maxdistance + Ha->GetTrackResolution(),
		      detectorwidth, binwidth, patterna, hasha);
	  _setpoints (Ha->rPos2+dw2 - 0.5 * maxdistance - Ha->GetTrackResolution(),
		      Ha->rPos2+dw2 + 0.5 * maxdistance + Ha->GetTrackResolution(),
		      detectorwidth, binwidth, patterna, hasha);
	  _setpoints (Hb->GetDriftDistance()+dw2 - 0.5 * maxdistance - Hb->GetTrackResolution(),
		      Hb->GetDriftDistance()+dw2 + 0.5 * maxdistance + Hb->GetTrackResolution(),
		      detectorwidth, binwidth, patterna, hasha);
	  _setpoints (Hb->rPos2+dw2 - 0.5 * maxdistance - Hb->GetTrackResolution(),
		      Hb->rPos2+dw2 + 0.5 * maxdistance + Hb->GetTrackResolution(),
		      detectorwidth, binwidth, patterna, hasha);
	}
      }

/* ---- CASE 3: If, after the search, a hit on a plane was not
                paired with a hit on the other plane (because the
                other plane didn't have a hit within the HRCset
                MaxSlope), this hit is treated individually when
                being included in the bit pattern.                  ---- */


    } else if (Ha) { /* this hit's on plane A */
      if( patternb ) { /* treating the two planes as two tree-planes,
                          so insert the hit into the bit pattern for
                          this plane                                    */
 	_setpoint( dw2 + Ha->GetDriftDistance(), Ha->GetTrackResolution(),
		   detectorwidth, binwidth, patterna, hasha);
	_setpoint( dw2 + Ha->rPos2, Ha->GetTrackResolution(),
		   detectorwidth, binwidth, patterna, hasha);
      } else {         /* treating the two planes as one tree-plane,
                          so insert this hit into the bit pattern for
                          this single tree-plane                        */
	_setpoints(Ha->GetDriftDistance()+dw2 - 0.5 * maxdistance - Ha->GetTrackResolution(),
		   Ha->GetDriftDistance()+dw2 + 0.5 * maxdistance + Ha->GetTrackResolution(),
		   detectorwidth, binwidth, patterna, hasha);
	_setpoints(Ha->rPos2+dw2 - 0.5 * maxdistance - Ha->GetTrackResolution(),
		   Ha->rPos2+dw2 + 0.5 * maxdistance + Ha->GetTrackResolution(),
		   detectorwidth, binwidth, patterna, hasha);
      }
    } else {           /* this hit's on plane B */
      if (patternb) {  /* treating the two planes as two tree-planes,
                          so insert the hit into the bit pattern for
                          this plane                                    */
	_setpoint (dw2 + Hb->GetDriftDistance(), Hb->GetTrackResolution(),
		   detectorwidth, binwidth, patternb, hashb);
	_setpoint (dw2 + Hb->rPos2, Hb->GetTrackResolution(),
		   detectorwidth, binwidth, patternb, hashb);
      } else {         /* treating the two planes as one tree-planes,
                          so insert the hit into the hit pattern for
                          this single tree-plane                        */
	_setpoints (Hb->GetDriftDistance()+dw2 - 0.5 * maxdistance - Hb->GetTrackResolution(),
		    Hb->GetDriftDistance()+dw2 + 0.5 * maxdistance + Hb->GetTrackResolution(),
		    detectorwidth, binwidth, patterna, hasha);
	_setpoints (Hb->rPos2+dw2 - 0.5 * maxdistance - Hb->GetTrackResolution(),
		    Hb->rPos2+dw2 + 0.5 * maxdistance + Hb->GetTrackResolution(),
		    detectorwidth, binwidth, patterna, hasha);
      }
    }
    Ha = Hanext;  /* Continue with the next set of hits */
    Hb = Hbnext;
  }

  return num;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/**
  exists() - this function searches through the link-list of valid treelines
             to see if the bit pattern for the specified treenode has already
             been accepted as a valid treeline.

    inputs: (1) int *newa          -
            (2) int front          -
            (3) int back           -
            (4) QwTrackingTreeLine *treeline - pointer to the linked-list of treelines

   outputs: (1) int exists()       - =0 if a treeline is not located
                                     =1 otherwise

 */
int QwTrackingTreeSearch::exists (int *newa, int front, int back, QwTrackingTreeLine *treeline)
{
  int *olda;
  int i, newmiss, oldmiss, diff;
  QwTrackingTreeLine *tl;
  int over;

  newmiss = 0;
  for (i = 0; i < fNumLayers; i++)
    if (! newa[i])
      newmiss++;

  for (tl = treeline ; tl; tl = tl->next) { /* loop over the treelines */

    if (tl->IsVoid()) /* if the treeline has been voided, go onto next one */
      continue;
    over = 0;
    if (tl->a_beg <= front && front <= tl->a_end)
      over++;
    if (tl->b_beg <= back  && back  <= tl->b_end)
      over++;

    if (over == 2) {
      //std::cerr << "over = 2" << std::endl;
      return 1;
    }
    if (over == 0)
      continue;

    olda = tl->hasharray;
    oldmiss = 0;
    diff    = 0;

    for (i = 0; i < fNumLayers; i++) {
      if (! olda[i]) {
	oldmiss++;
      } else {
	if (newa[i] && olda[i] != newa[i]) {
	  diff = 1;
	  break;
	}
      }
    }
    if (! diff) {
      if ((newmiss == 0 && oldmiss == 0) ||
	  (!newa[0] && !olda[0]) ||
          (!newa[fNumLayers-1] && !olda[fNumLayers-1]) ||
	  (newmiss && !oldmiss && (!newa[0] || !newa[fNumLayers-1])) ||
	  (oldmiss && !newmiss && (!olda[0] || !olda[fNumLayers-1]))
	) {
	if (tl->a_beg > front)
	  tl->a_beg = front;
	if (tl->a_end < front)
	  tl->a_end = front;
	if (tl->b_beg > back)
	  tl->b_beg = back;
	if (tl->b_end < back)
	  tl->b_end = back;
        //std::cerr << "!diff" << std::endl;
	return 1;
      }
    }
  }
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/**
  _SearchTreeLines() - this highly recursive function implements the treesearch
                algorithm.  For a specified list of nodenodes, this function
                examines the attached treenode.  If the bit pattern in the
                treenode does not match the bit pattern from the event, the
                function looks at the next nodenode.  Otherwise, the function
                will call itself to see if any of the sons of this treenode
                at the next level of bin-division match the bit pattern from
                the event.  This recursive calling will continue until a
                treenode at the deepest level of bin-division is located
                inside the bit pattern from the event.  Since the pattern in
                this treenode is represents a valid treeline for the event, a
                treeline is constructed from the treenode and then appended
                to the linked list of treelines being accumulated by the
                treesearch.

    inputs: (1) shortnode *node    - pointer the first nodenode in the linked
                                     list of nodenodes to be checked.
            (2) int level          - depth of the treesearch, i.e. the
                                     level of the bin-division
            (3) int offset         - the offset of the treenode within the
                                     pattern
            (4) int reverse        - the "pattern is flipped" flag

    global
    inputs: (1) char **static_pattern - the bit pattern from the event
            (2) char **static_hash    -
	    (3) int static_maxlevel   -
	    (4) int static_front      -

   outputs: there are no explicit output from this function.

    global
   outputs: Treeline *lTreeLines - pointer to the link-list of valid treelines.

 */
void QwTrackingTreeSearch::_SearchTreeLines (
	shortnode *node,
	int level,
	int offset,
	int row_offset,
	int reverse,
	int numWires)
{
  unsigned long pattern_start = (unsigned long) 0xffffffffL;
  /*                               to add to the offset   */
  unsigned long pattern_offset;
  shorttree *tree;	/* for searching in children of node*/
  int       *tree_pattern;
  int hashpat[TLAYERS];
  int nextlevel = level + 1, x;
  int miss, matched;
  int nullhits = 0;
  int firstwire = 1000, lastwire = -1;
  //int front_miss = 0;

  // Array that contains whether the HDC layer (or VDC wire) had a hit
  static int has_hits[TLAYERS];

  if (nTreeLines > MAX_TREELINES)
    return;


  /* ---- Compute the offset in the bit pattern for the start
          position of this level of bin-division                     ---- */
  pattern_start <<= level + 1;
  pattern_start &= (unsigned long) 0xffffffffL >> (32 - static_maxlevel);
  //if (fDebug) std::cout << "pattern start = " << pattern_start << std::endl;
  if (level == 0) {
    for (int u = nullhits = 0; u < fNumLayers; u++) {
      //if (fDebug && level > 0) std::cerr << u+row_offset << "," << pattern_start << std::endl;
      if (static_pattern[u+row_offset][pattern_start])
        has_hits[u] = 1;
      else {
        has_hits[u] = 0;
        nullhits++;
      }
    }
  }

  /* ---- Look at the trees attached to each nodenode on the
          specified nodenode linked list                             ---- */

  /* Region 3 */
  if (numWires > 0) {

    while (node) {      /* search in all nodes */

      tree = node->tree;
      //if (fDebug) std::cout << *tree << std::endl;

      /* ---- Is the hit pattern in this treenode valid for this level
            of the treesearch?                                         ---- */
      //std::cout << "minlevel = " << tree->minlevel << std::endl;
      //std::cout << "row_offset = " << row_offset << std::endl;
      if (tree->fMinLevel > level + 1) { /* check for level boundaries */
        std::cerr << "hrm..." << std::endl;
        node = node->next; /* no, so look at the next nodenode */
        continue;
      }

      /* ---- Match the hit pattern to this treenode by checking the
      hit pattern for each tree-plane to see if the bits
      specified in the treenode are on                           ---- */
      pattern_offset = pattern_start + offset;
      tree_pattern = tree->bit;
      matched = 0;
      if (reverse) {
        //std::cout << "reversed..." << std::endl;
        for (int i = 0; i < fNumLayers; i++) {  /* loop over tree-planes */
          x = (*tree_pattern++);
          std::cerr << "ERROR : reversed patterns need checking/debugging" << std::endl;
          if (static_pattern[i+row_offset][pattern_offset - x]) {
            matched++; /* number of matched tree-planes */
            if (i < firstwire) firstwire = i;
            if (i > lastwire) lastwire = i;
          }
        }
      } else {
        //std::cout << numWires << "," << fNumLayers << std::endl;
        for (int i = 0; i < fNumLayers; i++) {  /* loop over tree-planes */
          x = (*tree_pattern++);
          //if (fDebug && level > 0) std::cerr << i+row_offset << "," << pattern_offset+x << std::endl;
          if (static_pattern[i+row_offset][pattern_offset + x]) {
            matched++; /* number of matched tree-planes */
            if (i < firstwire) firstwire = i;
            if (i > lastwire && x != 0) lastwire = i;

          } else if (x == 0 && has_hits[i] == 0) {
            matched++; //matching null hits which are allowed in these patterns
          }
        }
      }
      //std::cout << "matched = " << matched << std::endl;
      /* ---- Check if there was the treenode is match now that the
      matching has been completely tested.                      ---- */

      if (matched == fNumLayers && nullhits < MAX_MISSED_VDC_WIRES) {
        //std::cout << "match found " << std::endl;
        //std::cout << "sons are " << std::endl;
        //std::cout << "-----------" << std::endl;
        /*
        shortnode** matchsons = tree->son;
        for(int a=0;a<4;a++){
          if(matchsons){
            (*matchsons)->tree->print();
            *matchsonsnext = (*matchsons)->next;
            matchsons = matchsonsnext;
          }
          else
            break;
        }
        std::cout << "-----------"<< std::endl;
        */

        /* ---- Yes, there is a match, so now check if all the levels
                of the treesearch have been done.  If so, then we have
                found a valid treeline.                                 ---- */

        if (level == static_maxlevel - 1) {

          int backbin = reverse ? offset - tree->bit[fNumLayers-1]
                                : offset + tree->bit[fNumLayers-1];
          if (reverse) {
            backbin = 0;
            for (int i = 0; i < fNumLayers; i++) {
              if (offset - tree->bit[i] > backbin)
                backbin = offset - tree->bit[i];
            }
          } else {
            backbin = 0;
            for (int i = 0; i < fNumLayers; i++) {
              if (offset + tree->bit[i] > backbin)
                backbin = offset + tree->bit[i];
            }
          }

          int frontbin = reverse ? offset - tree->bit[0]
                                 : offset + tree->bit[0];
          for (int i = 0; i < fNumLayers; i++) {
            int bin = reverse ? offset - tree->bit[i]
                              : offset + tree->bit[i];
            hashpat[i] = static_hash[i][bin];
          }

          miss = 0;
          if (static_pattern[0+row_offset][frontbin] == 0)
            miss = 1;
          else if( static_pattern[fNumLayers-1+row_offset][backbin] == 0)
            miss = 1;

          /* Check whether this treeline already exists */
          if (! exists(hashpat, frontbin, backbin, lTreeLines)) {
            /* Print tree */
            if (fShowMatchingPatterns) tree->Print();

            /* Create new treeline */
            QwTrackingTreeLine* treeline = new QwTrackingTreeLine (frontbin, frontbin, backbin, backbin);

            /* Number of treelines found */
            nTreeLines++;

            /* Copy hash pattern */
            memcpy(treeline->hasharray, hashpat, sizeof(int) * fNumLayers);

            /* Missed front or back planes (?) */
            /* (only used until TreeLineSort) */
            treeline->fNumMiss = miss;

            /* Region 3 specific treeline info */
            treeline->r3offset = row_offset;
            treeline->firstwire = firstwire;
            treeline->lastwire  = lastwire;

            /* Add this treeline to the linked-list */
            treeline->next = lTreeLines;
            lTreeLines = treeline;
          }

        } else {                  /* check son patterns */
          for (int rev = 0; rev < 4; rev += 2) {
            shortnode** cnode = tree->son + rev;
            if (rev ^ reverse) {
              int off2 = (offset << 1) + 1;
              for (int off = 0; off < 2; off++)
                _SearchTreeLines (*cnode++, nextlevel, off2 - off, row_offset, 2, numWires);
            } else {
              int off2 = offset << 1;
              for (int off = 0; off < 2; off++)
                _SearchTreeLines (*cnode++, nextlevel, off2 + off, row_offset, 0, numWires);
            }
          } /* highly optimized - time critical */
        }
      }
      node = node->next; /* ok, there wasn't a match, so go onto the
                                next nodenode                         */
    } /* end of loop over nodenodes */

  } else { /* Region 2 */

    while (node) {			/* search in all nodes */
      tree = node->tree;


      /* ---- Is the hit pattern in this treenode valid for this level
              of the treesearch?                                         ---- */

      if (tree->fMinLevel >= level) { /* check for level boundaries */
        node = node->next; /* no, so look at the next nodenode */
        continue;
      }


      /* ---- Match the hit pattern to this treenode by checking the
              hit pattern for each tree-plane to see if the bits
              specified in the treenode are on                           ---- */

      pattern_offset = pattern_start + offset;
      tree_pattern = tree->bit;
      matched = 0;
      if (reverse) {
        for (int i = 0; i < fNumLayers; i++) {  /* loop over tree-planes */
          if (static_pattern[i][pattern_offset - *tree_pattern++]) {
            matched++; /* number of matched tree-planes */
          }
        }
      } else {
        for (int i = 0; i < fNumLayers; i++) {  /* loop over tree-planes */
          if (static_pattern[i][pattern_offset + *tree_pattern++]) {
            matched++; /* number of matched tree-planes */
          }
        }
      }


      /* ---- Check if there was a treenode match now that the
              matching has been completely tested.                      ---- */

      // (wdc) Allow for two planes without hits.

      if (matched >= fNumLayers - MAX_MISSED_HDC_PLANES) {

        /* ---- Yes, there is a match, so now check if all the levels
                of the treesearch have been done.  If so, then we have
                found a valid treeline.                                 ---- */

        if (level == static_maxlevel - 1) {

          /* all levels done -> now insert treeline */
          int frontbin = reverse ? offset - tree->bit[0]
                                 : offset + tree->bit[0];
          int backbin  = reverse ? offset - tree->bit[fNumLayers-1]
                                 : offset + tree->bit[fNumLayers-1];
          for (int i = 0; i < fNumLayers; i++) {
            int bin = reverse ? offset - tree->bit[i]
                              : offset + tree->bit[i];
            hashpat[i] = static_hash[i][bin];
          }

          /* If the front or back bin are null, this is considered a miss (?) */
          miss = 0;
          if (static_pattern[0][frontbin] == 0)
            miss = 1;
          else if (static_pattern[fNumLayers-1][backbin] == 0)
            miss = 1;

          /* Check whether this treeline already exists */
          if (! exists(hashpat, frontbin, backbin, lTreeLines)) {

            /* Print tree */
            if (fShowMatchingPatterns) tree->Print();

            /* Create new treeline */
            QwTrackingTreeLine* treeline = new QwTrackingTreeLine (frontbin, frontbin, backbin, backbin);

            /* Number of treelines found */
            nTreeLines++;

            /* Copy hash pattern */
            memcpy(treeline->hasharray, hashpat, sizeof(int) * fNumLayers);

            /* Missed front or back planes (?) */
            /* (only used until TreeLineSort) */
            treeline->fNumMiss = miss;

            /* Add this treeline to the linked-list */
            treeline->next = lTreeLines;
            lTreeLines = treeline;
          }

        } else {                        /* check son patterns */

          for (int rev = 0; rev < 4; rev += 2) {
            shortnode** cnode = tree->son + rev;
            if (rev ^ reverse) {
              int off2 = (offset << 1) + 1;
              for (int off = 0; off < 2; off++)
                _SearchTreeLines (*cnode++, nextlevel, off2 - off, 0, 2, 0);
            } else {
              int off2 = offset << 1;
              for (int off = 0; off < 2; off++) {
                _SearchTreeLines (*cnode++, nextlevel, off2 + off, 0, 0, 0);
              }
            }
          } /* highly optimized - time critical */
        }
      }
      node = node->next; /* ok, there wasn't a match, so go onto the
                              next nodenode                         */
    } /* end of loop over nodenodes */
  }
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Initiate the treesearch for a set of tree detectors by calling the
 * recursive _SearchTreeLines() function.
 *
 * @param searchtree Pattern search tree
 * @param pattern Hit pattern
 * @param hashpat Hash pattern
 * @param maxlevel Maximum number of levels
 * @param numwires Number of wires in region 3
 * @param numlayers Number of layers in region 3
 * @return Linked list of treelines
 */
QwTrackingTreeLine* QwTrackingTreeSearch::SearchTreeLines (
	QwTrackingTreeRegion* searchtree,
	char **pattern,
	int  **hashpat,
	int maxlevel,
	int numwires,
	int numlayers)
{
  // Determine the top node of the search tree for the recursive search
  shortnode* topnode = searchtree->GetNode();

  fNumLayers = numlayers;

  static_maxlevel = maxlevel;
  static_pattern  = pattern;
  static_hash     = hashpat;

  // Reset the list of tree lines (this could cause memory leaks)
  nTreeLines = 0; // number of tree lines
  lTreeLines = 0; // list of tree lines

  /// For every wire we perform a recursive search.  For region 2 the number of
  /// wires is set to zero, so this will only execute once for every plane.
  /// For region 3 we will run over all the wires in the plane, and consider the
  /// next fNumLayers wires (so we need to end early).

  if (numwires > 0) {
    // NOTE (wdc) This seems horribly inefficient.  When there are eight empty
    // wires, _SearchTreeLines checks all eight and returns.  Next step is to
    // do this the same for the eight wires displaced by ONE wire!  Seven of the
    // wires are redone without any use.  Maybe _SearchTreeLines could return
    // a cumulative number of hits in that set, and increment by 8 if there was
    // not a single hit.  Or even better return the first wire with hit in the
    // set of fNumLayers wires, so we can increment immediately to that one.

    // Determine which groups of fNumLayers wires need to be considered:
    // only those groups of fNumLayers wires with at least one hit are to be
    // considered
    bool consider[numwires + fNumLayers]; // too long, not initialized
    for (int wire = 0; wire < numwires; wire++) {
      if (pattern[wire][(1UL << maxlevel) - 2] == 1)
        for (int i = 0; i < fNumLayers; i++)
          consider[wire+i] = true;
    }

    // The region 3 version of SearchTreeLines (search for every set of fNumLayers wires)

    for (int wire = 0; wire <= numwires - fNumLayers; wire++)
      _SearchTreeLines (topnode, 0, 0, wire, 0, numwires);

  } else
    // The region 2 version of SearchTreeLines
      _SearchTreeLines (topnode, 0, 0, 0, 0, 0);

  // Write out the number of tree lines
  QwDebug << "Found " << nTreeLines << " tree line(s)." << QwLog::endl;

  // Return the list of tree lines
  return lTreeLines;
}
