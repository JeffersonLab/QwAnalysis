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
#include "QwOptions.h"
#include "globals.h"
#include "QwHit.h"
#include "QwDetectorInfo.h"
#include "QwTreeLine.h"
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
: fNumPlanes(fNumLayers),fNumWires(fNumLayers)
{
  fDebug = 1; // Reset debug level

  fNumLayers = MAX_LAYERS;

  fMaxMissedPlanes = gQwOptions.GetValue<int>("QwTracking.R2.MaxMissedPlanes");
  fMaxMissedWires  = gQwOptions.GetValue<int>("QwTracking.R3.MaxMissedWires");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeSearch::~QwTrackingTreeSearch ()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

std::vector<QwTreeLine*> QwTrackingTreeSearch::GetListOfTreeLines ()
{
  return fTreeLineList;
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
  _setpoints(wirespacing * (wire+1) - hit->GetDriftDistance() - hit->GetDetectorInfo()->GetTrackResolution(),
             wirespacing * (wire+1) - hit->GetDriftDistance() + hit->GetDetectorInfo()->GetTrackResolution(),
             detectorwidth, binwidth, pattern, hash);

  // Set the points on the back/bottom side of the wire (R3/R2)
  _setpoints(wirespacing * (wire+1) + hit->GetDriftDistance() - hit->GetDetectorInfo()->GetTrackResolution(),
             wirespacing * (wire+1) + hit->GetDriftDistance() + hit->GetDetectorInfo()->GetTrackResolution(),
             detectorwidth, binwidth, pattern, hash);

  return 0;
}

/**
 * This function searches through the link-list of valid treelines
 * to see if the bit pattern for the specified treenode has already
 * been accepted as a valid treeline.
 *
 * @param newa
 * @param front
 * @param back
 * @param offset
 * @param treelinelist Pointer to the linked list of treelines
 * @return 0 if no such tree line exists, 1 otherwise
 */
int QwTrackingTreeSearch::exists (
	int *newa,
	int front,
	int back,
	int offset)
{
  int *olda;
  int oldmiss, diff;

  int newmiss = 0;
  for (unsigned int row = 0; row < fNumPlanes; row++)
    if (! newa[row])
      newmiss++;

  // Loop over the treelines
  for (size_t i = 0; i < fTreeLineList.size(); i++) {
    QwTreeLine* tl = fTreeLineList[fTreeLineList.size() - 1 - i];

    // If the treeline has been voided, go onto next one
    if (tl->IsVoid())
      continue;

    int over = 0;

    int wire_diff=0;
    wire_diff=abs(offset-tl->fR3Offset);
    
    if(wire_diff<5 && offset!=-1){
      if (tl->a_beg == front && front == tl->a_end )
        over++;
      if (tl->b_beg == back  && back  == tl->b_end )
        over++;
    }

    // r2
    if(offset==-1){
      if(tl->a_beg == front && front == tl->a_end )
        ++over;
      if(tl->b_beg == back && back == tl->b_end )
        ++over;
    }
    if (over == 2) {
      return 1;
    }
    if (over == 0)
      continue;

    olda = tl->fHashArray;
    oldmiss = 0;
    diff    = 0;

    for (unsigned int row = 0; row < fNumLayers; ++row) {
      if (! olda[row]) {
        oldmiss++;
      } else {
        if (newa[row] && olda[row] != newa[row]) {
          diff = 1;
          break;
        }
      }
    }


    if (! diff && offset!=-1) {
      if ((newmiss == 0 && oldmiss == 0) ||
          (!newa[0] && !olda[0]) ||
          (!newa[fNumPlanes-1] && !olda[fNumPlanes-1]) ||
          (newmiss && !oldmiss && (!newa[0] || !newa[fNumPlanes-1])) ||
          (oldmiss && !newmiss && (!olda[0] || !olda[fNumPlanes-1]))
        ) {
        if (tl->a_beg > front)
          tl->a_beg = front;
        if (tl->a_end < front)
          tl->a_end = front;
        if (tl->b_beg > back)
          tl->b_beg = back;
        if (tl->b_end < back)
          tl->b_end = back;
//         std::cerr << "!diff" << std::endl;
        return 1;
      }
    }
  }
  return 0;
}


/**
 *  This highly recursive function implements the tree search
 *  algorithm.  For a specified list of nodenodes, this function
 *  examines the attached treenode.  If the bit pattern in the
 *  treenode does not match the bit pattern from the event, the
 *  function looks at the next nodenode.  Otherwise, the function
 *  will call itself to see if any of the sons of this treenode
 *  at the next level of bin division match the bit pattern from
 *  the event.  This recursive calling will continue until a
 *  treenode at the deepest level of bin division is located
 *  inside the bit pattern from the event.  Since the pattern in
 *  this treenode represents a valid treeline for the event, a
 *  treeline is constructed from the treenode and then appended
 *  to the linked list of treelines being accumulated by the
 *  tree search.
 *
 * @param node Pointer to the first node in the linked list
 * @param level Depth of the treesearch, i.e. the level of the bin-division
 * @param offset Offset of the treenode within the pattern
 * @param row_offset Offset of the wire group in region 3
 * @param reverse Flag "pattern is flipped"
 * @param numwires Number of wires in region 3
 */
void QwTrackingTreeSearch::_SearchTreeLines (
	shortnode *node,
	int level,
	int offset,
	int row_offset,
	int reverse,
	int numwires)
{
  
  
  // Next level in the recursive search
  bool search_debug_level=0;
  if(search_debug_level)
  std::cout << "entering _SearchTreeLines with level" << level << " and row_offset " << row_offset << std::endl;
  int nextlevel = level + 1;
  std::vector<int> patterns(MAX_LAYERS);

  // Fail if we have already found enough treelines
  if (fNTreeLines > TREESEARCH_MAX_TREELINES)
    return;

  /**
   * Reminder about the bit pattern:
   * - there are n levels of bin division, so 8,4,2,1 bins for 4 levels,
   * - there are rows corresponding with the HDC planes or VDC wires.
   *
   * HDC planes (spaces between the 5 levels of bin division)
   * for 16 bins in wire coordinate (e.g. 16 wires)
   * and zero distance resolution
   * \code
   *  plane 1: ......|.........  ...|....  .|..  |.  |
   *  plane 2: .......|........  ...|....  .|..  |.  |
   *  plane 3: ........|.......  ....|...  ..|.  .|  |
   *  plane 4: .........|......  ....|...  ..|.  .|  |
   * \endcode
   *
   * VDC wires (spaces between the 4 levels of bin division)
   * for 8 bins in drift distance and zero distance resolution
   * \code
   *  wire 159: |......|  |..|  ||  |
   *  wire 160: .|....|.  |..|  ||  |
   *  wire 161: ..|..|..  .||.  ||  |
   *  wire 162: ...||...  .||.  ||  |
   *  wire 163: ..|..|..  .||.  ||  |
   *  wire 164: .|....|.  |..|  ||  |
   *  wire 165: |......|  |..|  ||  |
   * \endcode
   */

  /* Compute the offset in the bit pattern for the start position of this level
   * of bin division.  For n levels, there are 2^n - 1 used bins.  Each level
   * has 2^k bins (i.e. 1 bin at level 0, 2 at level 1).  The start of level k
   * is at position position (2^n - 1) - 2^k + 1 (counting from bin 0).
   * and Sum (i = k..n) 2^i = (2^n - 1) - (2^k - 1)
   *
   * E.g.: For n = 4 there are 8,4,2,1 bins, so the start bin positions are
   *       0,8,12,14 for k = 0,1,2,3,4.
   */
  unsigned long pattern_start = (unsigned long) 0xffffffffL;
  pattern_start <<= level + 1;
  pattern_start &= (unsigned long) 0xffffffffL >> (32 - fMaxLevel);
  // Warn when this pattern_start puts us past the bit pattern
  if (pattern_start > fPattern_fMaxBins)
    QwWarning << "pattern start past end: " << pattern_start << QwLog::endl;
  // Debug output
  if (fDebug)
    QwDebug << "pattern start = " << pattern_start << QwLog::endl;

 // NOTE: this procedure is used in region2, and number 4 is hard-coded(might be removed later)
  static int has_planes[4];
  unsigned int missed_planes =0 ;
  if(level == 0 && numwires==0){
        for(unsigned int plane=0;plane< fNumPlanes;plane++){
         if(static_pattern[plane][pattern_start])
            has_planes[plane]=1;
         else{
            has_planes[plane]=0;
            missed_planes++;
             }
         }
   }
  /** Determine the rows which have hits first.  This is used to determine how
   *  many wires in a region 3 group have been hit.  If there are fewer than a
   *  preset number of wire hits, then the search is not even started.
   */
  static int has_hits[MAX_LAYERS];
  unsigned int missed_rows = 0;
  if (level == 0 && numwires>0 ) {
    for (unsigned int row = 0; row < fNumPlanes; row++) {
      // Bounds checking
      assert(row_offset + row < fPattern_fMaxRows);
      assert(pattern_start    < fPattern_fMaxBins);
      // Has this row a hit?
      if (static_pattern[row_offset+row][pattern_start])
        has_hits[row] = 1;
      else {
        has_hits[row] = 0;
        missed_rows++;
      }
    }
  }

 
  /* Region 3 */
  if (numwires > 0) {
    while (node) {

      /* Look at the trees attached to each nodenode on the
       * specified nodenode linked list
       */
      shorttree* tree = node->GetTree();

      /* Is the hit pattern in this treenode valid for this level
       * of the treesearch? (i.e. check  boundaries)
       */
      if (tree->fMinLevel > level + 1) { /* check for level boundaries */
        QwError << "Tree invalid for this treesearch!" << QwLog::endl;
        node = node->GetNext(); /* no, so look at the next nodenode */
        continue;
      }

      /* Match the hit pattern to this treenode by checking the
       * hit pattern for each tree plane to see if the bits
       * specified in the treenode are on.
       */
      unsigned long pattern_offset = pattern_start + offset;
      int* tree_pattern = tree->fBit;

      unsigned int matched_wires = 0;

      // Reset the first and last wire
      int firstwire = fPattern_fMaxRows;
      int lastwire  = -1;

      // Different treatment for reversed trees
      if (reverse) {

        // This is not completely tested yet
        QwError << "reversed patterns need checking/debugging" << QwLog::endl;

        // Loop over tree-planes
        for (unsigned int row = 0; row < fNumWires; row++) {
          int bin = (*tree_pattern++);
          // Bounds checking
          assert(row_offset + row     < fPattern_fMaxRows);
          assert(pattern_offset - bin < fPattern_fMaxBins);
          // If the bin is set
          if (static_pattern[row_offset + row][pattern_offset - bin]) {
            matched_wires++; /* number of matched tree-planes */
            if ((int) row < firstwire) firstwire = row;
            if ((int) row > lastwire)  lastwire = row;
          }
        } // end of loop over wires

      } 
	else {
        /* loop over tree-planes */
        for (unsigned int row = 0; row < fNumWires; row++) {
          int bin = (*tree_pattern++);
 	  if(search_debug_level)
           std::cout << "for level" << level <<" bin:" << bin << std::endl;
          // Bounds checking
	  patterns.at(row)=bin;
          assert(row_offset + row     < fPattern_fMaxRows);
          assert(pattern_offset + bin < fPattern_fMaxBins);
          // If the bin is set
          if (static_pattern[row_offset + row][pattern_offset + bin]) {
            matched_wires++; /* number of matched tree-planes */
            if ((int) row < firstwire) firstwire = row;
            if ((int) row > lastwire && bin != 0) lastwire = row;

          // But matching null hits is allowed in these patterns, i.e.
          // missing wires are not treated as bad when the bin is not set
          } 
 	    else if (bin == 0 && has_hits[row] == 0) {
		matched_wires++;
	}
        } // end of loop over wires
      } // end of if reversed
	
	// chcke if there's any missing wires in the middle or allow only 1 goofy wires

	if(matched_wires < fNumWires ){
           int goofy=0;       
	   int* tree_pattern_copy = tree->fBit;
           if(reverse){
              // needs to do something?
		}
		else{
		    for(unsigned int row=0;row<fNumWires;row++){
			int bin = (*tree_pattern_copy++);
			assert( row_offset + row < fPattern_fMaxRows);
                        assert(pattern_offset + bin < fPattern_fMaxBins);
			if ((int)row > firstwire && (int)row <= lastwire){
                            if (static_pattern[row_offset + row][pattern_offset + bin])
				continue;                            
                            else if (has_hits[row] == 0) matched_wires++;
                            else if(goofy<1) {matched_wires++;goofy++;}
			}
		}
	    }
	}

	if(search_debug_level){
      std::cout << "matched_wires: " << matched_wires << std::endl;
      std::cout << "missed rows: " << missed_rows << std::endl;
	}

	
      /// Check if there was a treenode match now that the matching has been
      /// completely tested, but allow for some missing wires.
      if (matched_wires == fNumWires && missed_rows <= fMaxMissedWires) {

        /// Yes, there is a match, so now check if all the levels of the
        /// tree search have been done.

        if (level == fMaxLevel - 1) {

          /// If so, then we have found a valid treeline.
          
          // Calculate the bin in the last layer
          int backbin = reverse ? offset - tree->fBit[fNumPlanes-1]
                                : offset + tree->fBit[fNumPlanes-1];
          if (reverse) {
            backbin = 0;
            for (unsigned int wire = 0; wire < fNumWires; wire++) {
              if (offset - tree->fBit[wire] > backbin)
                backbin = offset - tree->fBit[wire];
            }
          } 
	    else {
            backbin = 0;
            for (unsigned int wire = 0; wire < fNumWires; wire++) {
              if (offset + tree->fBit[wire] > backbin)
                backbin = offset + tree->fBit[wire];
            }
          }

          // Calculate the bin in the first layer
          int hashpat[fNumWires];
          int frontbin = reverse ? offset - tree->fBit[0]
                                 : offset + tree->fBit[0];
          for (unsigned int wire = 0; wire < fNumWires; wire++) {
            int bin = reverse ? offset - tree->fBit[wire]
                              : offset + tree->fBit[wire];
            // And set the hash for this pattern
            hashpat[wire] = static_hash[wire][bin];
          }

          // Consider this a miss if the front layer or back layer did not
          // have a hit
          int miss = 0;
          if (static_pattern[0+row_offset][frontbin] == 0)
            miss = 1;
          else if (static_pattern[fNumPlanes-1+row_offset][backbin] == 0)
            miss = 1;

	  
          /* Check whether this treeline already exists */
	  
          if (! exists(hashpat, frontbin, backbin, row_offset)) {
	     
            /* Print tree */
            if (fShowMatchingPatterns) tree->Print();

            /* Create new treeline */ 
            QwTreeLine* treeline = new QwTreeLine (frontbin, frontbin, backbin, backbin);

            /* Number of treelines found */
            fNTreeLines++;

            /* Copy hash pattern */
            memcpy(treeline->fHashArray, hashpat, sizeof(int) * fNumWires);

            /* Missed front or back planes (?) */
            /* (only used until TreeLineSort) */
            treeline->fNumMiss = miss;

            /* Region 3 specific treeline info */
            treeline->fR3Offset = row_offset;
            treeline->fR3FirstWire = firstwire;
            treeline->fR3LastWire  = lastwire;
	    treeline->SetMatchingPattern(patterns);
// 	    for(int i=0;i< patterns.size();i++)
// 		std::cout << "bin value: "<< patterns.at(i) << std::endl;
	    if (search_debug_level) {
	      std::cout << "first wire: " << firstwire << std::endl;
	      std::cout << "last wire: " << lastwire << std::endl;
	    }
            /* Add this treeline to the linked-list */
	    if (firstwire != lastwire) {
              fTreeLineList.push_back(treeline);
	    } else delete treeline;
          }


        } else { // if (level != fMaxLevel - 1)

          /// If not, then we descend to check the son patterns

	  if(search_debug_level)
	  std::cout << "not the deepest level,begin recursive call!" << std::endl;
          for (int rev = 0; rev < 4; rev += 2) {

            shortnode** cnode = tree->son + rev;


            if (rev ^ reverse) {
	      
              int off2 = (offset << 1) + 1;
              for (int off = 0; off < 2; off++)
                _SearchTreeLines (*cnode++, nextlevel, off2 - off, row_offset, 2, numwires);
            } else {
	      
              int off2 = (offset << 1);
              for (int off = 0; off < 2; off++)
                _SearchTreeLines (*cnode++, nextlevel, off2 + off, row_offset, 0, numwires);
		
            }

          } // end of for over son nodes

        } // end of if we have reached the maximum level (level == fMaxLevel - 1)

      }

      // There was no match, so go onto the next nodenode
      node = node->GetNext();

    } // end of loop over nodes


  } else { /* Region 2 */
    
    while (node) { /* search in all nodes */

      
      shorttree* tree = node->GetTree();


      /* ---- Is the hit pattern in this treenode valid for this level
              of the treesearch?                                         ---- */

      if (tree->fMinLevel >= level) { /* check for level boundaries */
        node = node->GetNext(); /* no, so look at the next nodenode */
        continue;
      }


      /* ---- Match the hit pattern to this treenode by checking the
              hit pattern for each tree-plane to see if the bits
              specified in the treenode are on                           ---- */

      //NOTE:to print out the final binoffset value
      //int final[4] = {0}; // unused
      unsigned long pattern_offset = pattern_start + offset;
      int* tree_pattern = tree->fBit;
      unsigned int matched_planes = 0;
      int goofy_r2=0;
      if (reverse) {
        /* loop over tree-planes */
        for (unsigned int plane = 0; plane < fNumPlanes; ++plane) {
          int bin=(*tree_pattern++);
          if (static_pattern[plane][pattern_offset - bin]) {
            matched_planes++; /* number of matched tree-planes */
          }
        }
      } else {
        /* loop over tree-planes */
        for (unsigned int plane = 0; plane < fNumPlanes; ++plane) {
          int bin=(*tree_pattern++);
          patterns.at(plane)=pattern_offset + bin;
          if (static_pattern[plane][pattern_offset + bin] || has_planes[plane]==0) {
            ++matched_planes; /* number of matched tree-planes */
            //final[plane] = pattern_offset+bin; // unused
          }
          //else if(has_planes[plane]==0){
          //  ++matched_planes;
          //  final[plane]=pattern_offset+bin;
          //}
	  else if(goofy_r2==0 && missed_planes==0 && level==fMaxLevel-1){
	    ++matched_planes;
	    ++goofy_r2;
	  }
        }
      }

        
      /* ---- Check if there was a treenode match now that the
              matching has been completely tested.                      ---- */
//       if (matched_planes >= fNumPlanes - fMaxMissedPlanes) {

      
         if (matched_planes == fNumPlanes && missed_planes <= fMaxMissedPlanes ){
        /* ---- Yes, there is a match, so now check if all the levels
                of the treesearch have been done.  If so, then we have
                found a valid treeline.                                 ---- */

      
        if (level == fMaxLevel - 1) {
          /* all levels done -> now insert treeline */
          int hashpat[fNumPlanes];
          int frontbin = reverse ? offset - tree->fBit[0]
                                 : offset + tree->fBit[0];
          int backbin  = reverse ? offset - tree->fBit[fNumPlanes-1]
                                 : offset + tree->fBit[fNumPlanes-1];
          for (unsigned int plane = 0; plane < fNumPlanes; plane++) {
            int bin = reverse ? offset - tree->fBit[plane]
                              : offset + tree->fBit[plane];
            hashpat[plane] = static_hash[plane][bin];
          }

          /* If the front or back bin are null, this is considered a miss (?) */
          int miss = 0;
          if (static_pattern[0][frontbin] == 0)
            miss = 1;
          else if (static_pattern[fNumPlanes-1][backbin] == 0)
            miss = 1;
          /* Check whether this treeline already exists */
          if (! exists(hashpat, frontbin, backbin, -1)) {
                
            /* Print tree */
            if (fShowMatchingPatterns) tree->Print();

            /* Create new treeline */
                                
            QwTreeLine* treeline = new QwTreeLine (frontbin, frontbin, backbin, backbin);
            /* Number of treelines found */
            ++fNTreeLines;

            /* Copy hash pattern */
            memcpy(treeline->fHashArray, hashpat, sizeof(int) * fNumPlanes);
            
            /* Missed front or back planes (?) */
            /* (only used until TreeLineSort) */
            treeline->fNumMiss = miss;
	    treeline->SetMatchingPattern(patterns);
            /* Add this treeline to the linked-list */
	    fTreeLineList.push_back(treeline);
          }

        } else {                        /* check son patterns */

          for (int rev = 0; rev < 4; rev += 2) {
            shortnode** cnode = tree->son + rev;
            if (rev ^ reverse) {
              int off2 = (offset << 1) + 1;
              for (int off = 0; off < 2; off++){
                _SearchTreeLines (*cnode++, nextlevel, off2 - off, 0, 2, 0);
                }
            } else {
              int off2 = offset << 1;
              for (int off = 0; off < 2; off++) {
                _SearchTreeLines (*cnode++, nextlevel, off2 + off, 0, 0, 0);
              }
            }
          } /* highly optimized - time critical */
        }
      }
      node = node->GetNext(); /* ok, there wasn't a match, so go onto the
                                 next nodenode                         */

    } // end of loop over nodes

  } // end of region 2

  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Initiate the treesearch for a set of tree detectors by calling the
 * recursive _SearchTreeLines() function.  The tree search algorithm is
 * explained in QwTrackingTreeSearch::_SearchTreeLine().
 *
 * @param searchtree Pattern search tree
 * @param pattern Hit pattern
 * @param hashpat Hash pattern
 * @param maxlevel Maximum number of levels
 * @param numwires Number of wires in region 3
 * @param numlayers Number of layers in region 3
 * @return Linked list of treelines
 */
QwTreeLine* QwTrackingTreeSearch::SearchTreeLines (
	QwTrackingTreeRegion* searchtree,
	char **pattern,
	int  **hashpat,
	int maxlevel,
	int numwires,
	int numlayers)
{
  // Determine the top node of the search tree for the recursive search
  shortnode* topnode = searchtree->GetNode();

  // Store the number of rows (accessible through references named
  // fNumPlanes and fNumWires)
  fNumLayers = numlayers;

  // Store the maximum level, pattern and hash
  fMaxLevel = maxlevel;

  static_pattern  = pattern;
  static_hash     = hashpat;

  // Store the maximum bin index in a pattern:
  // total number of bins = 2^levels - 1 = (1UL << levels) - 1
  fPattern_fMaxBins = (1UL << maxlevel) - 1;

  // Reset the list of tree lines (this could cause memory leaks)
  fNTreeLines = 0; // number of tree lines
  fTreeLineList.clear();

  /// For every wire we perform a recursive search.  For region 2 the number of
  /// wires is set to zero, so this will only execute once for every plane.
  /// For region 3 we will run over all the wires in the plane, and consider the
  /// next fNumWires wires (so we need to end early).

  if (numwires > 0) { // region 3

    // Store the maximum number of rows
    fPattern_fMaxRows = numwires;

    /// For region 3, we determine which groups of wires need to be considered:
    /// only those groups of numlayers wires with at least one hit are to be
    /// considered; the others are empty and the tree search should not even
    /// be started.
    std::vector<int> wiregroups; // list of wire groups to consider
    int last_wire_with_hit = -1; // last wire with a hit
    for (int wire = 0; wire < numwires; wire++) {
      // If this wire was hit (check in the single bin at the end of the pattern)
      if (pattern[wire][(1UL << maxlevel) - 2] == 1) {
        // Set all previous numlayers-1 groups active (including this wire)
        for (int wiregroup = std::max(wire - numlayers + 1, last_wire_with_hit);
                 wiregroup < std::min(numwires - numlayers + 1, wire); wiregroup++) {
          if (wiregroup < 0) continue; // ignore negative wires
          wiregroups.push_back(wiregroup);
        }
        // Keep track of which wire had the last hit
        last_wire_with_hit = wire;
      }
    }	
    // The region 3 version of SearchTreeLines
    // (search for groups of numlayers wires)
    for (size_t i = 0; i < wiregroups.size(); i++) {
      _SearchTreeLines (topnode, 0, 0, wiregroups.at(i), 0, numwires);
    }

  } else { // region 2

    // Store the maximum number of rows
    fPattern_fMaxRows = numlayers;
    // The region 2 version of SearchTreeLines
    _SearchTreeLines (topnode, 0, 0, 0, 0, 0);

  } // region 2

  // Write out the number of tree lines
  QwDebug << "Found " << fNTreeLines << " tree line(s)." << QwLog::endl;


  // Put in linked list
  QwTreeLine* list = 0;
  if (fTreeLineList.size() > 0) {
    for (size_t tl = 0; tl < fTreeLineList.size(); tl++) {
      fTreeLineList[tl]->next = list;
      list = fTreeLineList[tl];
    }
  }

  // Return the list of tree lines
  return list;
}
