//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
// C++ Interface: QwTrackingTreeSort
//
// Description:
//
//
// Modified by: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
//              Jie Pan <jpan@jlab.org>, Sun May 24 14:29:42 CDT 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \class QwTrackingTreeSort

    \file QwTrackingTreeSort.cc

    $Date: Sun May 24 14:29:42 CDT 2009 $

    \brief This module is used to identify good track segments versus ghost tracks/hits.
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include "QwTrackingTreeSort.h"

#include "QwDetectorInfo.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Initializes the module responsible for sorting tracks according to chi^2
 */
QwTrackingTreeSort::QwTrackingTreeSort ()
{
  fDebug = 0; // Debug level

  doubletrack = 0.2;
  good = 2;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeSort::~QwTrackingTreeSort ()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* ======================================================================
 * checks for connected lines on the connectivity array
 * ====================================================================== */
int QwTrackingTreeSort::connectiv (
	char *ca,
	int *array,
	int *isvoid,
	char size,
	int idx)
{
  int ret = 0;
  idx *= size;
  for (int j = 0; j < size; j++) {
    if (array[j+idx] && (!ca || ca[j]) && isvoid[j] != true)
      ret++;
  }
  return ret;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeSort::connectarray (
	char *ca,
	int *array,
	int *isvoid,
	char size,
	int idx)
{
  memset (ca, 0, size);
  ca[idx] = 1;
  for (int i = 0; i < size; i++) {
    if (ca[i]) {
      for (int j = i+1; j < size; j++) {
        if (array[j+i*size] && isvoid[j] == false )
          ca[j] = 1;
      }
    }
  }
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingTreeSort::bestunconnected (
	char *ca,
	int *array,
	int *isvoid,
	double *chia,
	int size,
	int idx)
{
  int besti = idx, bestj = -1;
  double bestchi = chia[idx], bestdchi;

  for (int j = idx+1; j < size; j++) { /* search for the best single-track */
    if (! ca[j])
      continue;
    if (chia[j] < bestchi) {
      besti = j;
      bestchi = chia[j];
    }
  }

  bestdchi = (bestchi) * (2 + doubletrack);// this will allow a double track, with the 2nd best chi
  //to be up to (doubletrack)% larger than the best chi.

  for (int i = idx; i < size; i++) { /* is there a double track ? */
    if (! ca[i])
      continue;
    for (int j = i+1; j < size; j++) {
      if (! ca[j])
	continue;
      if (array[i*size+j]) // if it's two tracks sharing a bunch of wires, don't worry about it
	continue;
      if (chia[j] + chia[i] < bestdchi) { // but if there's two separate tracks, with good chi, then it's a double track
	besti = i;
	bestj = j;
	bestdchi = chia[j]+chia[i];
      }
    }
  }

  for (int i = idx; i < size; i++) {
    if (! ca[i])
      continue;
    if (i == besti || i == bestj || besti == -1) {
      isvoid[i] = good;//good
    }
    else if (isvoid[i] == false) {
      if ( (besti >= 0 && array[i*size+besti] ) ||
           (bestj >= 0 && array[i*size+bestj] ) ) {
        isvoid[i] = true;
      }
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeSort::globalconnectiv (
	char *ca,
	int *array,
	int *isvoid,
	int size,
	int idx)
{
  int i, max = 0, c;
  bool old;

  old = isvoid[idx];
  isvoid[idx] = false;

  for(i = 0; i < size; i++ ) {
    if( isvoid[i] != true ) {
      c = connectiv( ca, array, isvoid, size, i);
      if( c > max )
        max = c;
    }
  }
  isvoid[idx] = old;
  return max;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeSort::bestconnected (
	char *ca,
	int *array,
	int *isvoid,
	double *chia,
	int size,
	int idx)
{
  int besti = -1;
  double bestchi = 1e8, oworst = 0.0;
  double chi;

  for (int i = idx; i < size; i++) {
    if (! ca[i])
      continue;
    chi = chia[i];
    if (isvoid[i] != true) {

      if (oworst < chi)
	oworst = chi;
      continue;
    }
    if (globalconnectiv (ca, array, isvoid, size, i) > 2)
      continue;

    if (bestchi > chi) {
      besti = i;
      bestchi = chi;
    }
  }
  if (besti >= 0 && (3.0 > bestchi || oworst + 3.0 > bestchi)) {
    isvoid[besti] = good; // good
    return 1;
  }
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeSort::rcPTCommonWires (const QwPartialTrack *track1, const QwPartialTrack *track2)
{
  // Get lists of treelines in these tracks
  const std::vector<QwTreeLine*> tl1 = track1->GetListOfTreeLines();
  const std::vector<QwTreeLine*> tl2 = track2->GetListOfTreeLines();

  // Map direction to treeline (assumes one treeline per direction)
  std::map<EQwDirectionID,QwTreeLine*> tl1map, tl2map;
  for (size_t i = 0; i < tl1.size(); i++)
    tl1map[tl1[i]->GetDirection()] = tl1[i];
  for (size_t i = 0; i < tl2.size(); i++)
    tl2map[tl2[i]->GetDirection()] = tl2[i];

  // Count common wires for the partial track
  int common = 0;
  for (EQwDirectionID dir = kDirectionX; dir <= kDirectionV; dir++) {
    // Check whether entries exist for this direction
    if (tl1map.count(dir) == 0) continue;
    if (tl2map.count(dir) == 0) continue;
    // Count common wires for these treelines
    common += rcCommonWires (tl1map[dir], tl2map[dir]);
  }
  // Determine average assuming 3 directions
  common /= 3;
  return common;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* This function simply counts the number of common wires
shared between two treelines.  The only output is the integer
return value
*/
int QwTrackingTreeSort::rcCommonWires_r3 (const QwTreeLine *line1, const QwTreeLine *line2)
{
  //################
  // DECLARATIONS  #
  //################
  int common, total;

  //##################
  //DEFINE VARIABLES #
  //##################
  common = total = 0;
  std::vector<QwHit*> hits1 = line1->GetListOfHits();
  std::vector<QwHit*> hits2 = line2->GetListOfHits();

  //##############################################
  //Count the wires shared between the treelines #
  //##############################################
  int i1 = 0, i2 = 0;
  for ( ; i1 < line1->GetNumberOfHits() && i2 < line2->GetNumberOfHits() ; ) {
    if (hits1[i1]->GetElement() == hits2[i2]->GetElement()) {
      if (hits1[i1]->IsUsed() && hits2[i2]->IsUsed())
	common++;
      i1++;
      i2++;
      total++;
    }
    else if (hits1[i1]->GetElement() > hits2[i2]->GetElement()) {
      i2++;
      total++;
    }
    else if (hits1[i1]->GetElement() < hits2[i2]->GetElement()) {
      i1++;
      total++;
    }
  }
  total += line1->GetNumberOfHits() - i1 + line2->GetNumberOfHits() - i2;

  //Check which line has more hits used
  //  total = total1 > total2 ? total2 : total1;

  if (! total)
    return 0;

  // Get a ratio and multiply it to return an integer
  return (10000 * common / total + 50 ) / 100;
  // if 8 common out of 8, then this = 100
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*------------------------------------------------------------------------*//*!

 \brief Counts the number of common wires shared between two tree lines.

 The return value is the an integer score between 0 and 100, where 100 means
 that all wires are common.  One tree line as subset of the other will result
 in a score of 100.  The tree lines are not modified by this function.

*//*-------------------------------------------------------------------------*/
int QwTrackingTreeSort::rcCommonWires (
	const QwTreeLine *treeline1, ///< first tree line
	const QwTreeLine *treeline2) ///< second tree line
{
  // Get the lists of hits associated with the two tree lines
  const QwHit* const* hits1  = treeline1->fHits;
  const QwHit* const* hits2  = treeline2->fHits;

  // A two-bit pattern indicates on which treeline we should advance in the
  // next iteration of the search.  This assumes that the detectors are ordered.
  int fw = 3;
  // E.g. fw = (decimal) 3 = (binary) 11: this means that we should advance on
  // both treeline 1 and treeline 2.

  int common = 0; // number of common hits
  int total1 = 0, total2 = 0; // total number of hits on the tree lines

  // Infinite loop over the entries in both lists
  int i1 = -1, i2 = -1;
  for (;;) {

    // Advance on tree line 1
    if (fw & 1) {
      i1++;
      // Advance until we reach the end of the list or find a hit
      for ( ; i1 < DLAYERS*MAXHITPERLINE && hits1[i1]; i1++)
	if (hits1[i1]->IsUsed()) {
	  total1++;
	  break;
	}
    }

    // Advance on tree line 2
    if (fw & 2) {
      i2++;
      // Advance until we reach the end of the list or find a hit
      for ( ; i2 < DLAYERS*MAXHITPERLINE && hits2[i2]; i2++)
	if (hits2[i2]->IsUsed()) {
	  total2++;
	  break;
	}
    }

    // End condition: if we reach the end of the hits in either line
    // either because we reach the maximum, or because we read a null hit
    if (i1 == DLAYERS * MAXHITPERLINE || ! hits1[i1]
     || i2 == DLAYERS * MAXHITPERLINE || ! hits2[i2])
      break;

    // Now that we have a hit in tree line 1 and 2, determine the detector ID
    int id1 = hits1[i1]->GetDetectorInfo()->GetPlane();
    int id2 = hits2[i2]->GetDetectorInfo()->GetPlane();
    // If the ID in treeline 1 is lower, advance on treeline 1
    if (id1 < id2)
      fw = 1;
    // If the ID in treeline 2 is lower, advance on treeline 2
    else if (id1 > id2)
      fw = 2;
    // If both IDs are the same, check whether also the same wires were hit
    else {
      if (hits1[i1]->GetElement() == hits2[i2]->GetElement())
        common++; // This is a common hit! (disregarding L/R ambiguity)
      // and then advance on both treelines
      fw = 3;
    }

  } // end of infinite loop


  // Determine the smallest number of used hits for the two treelines
  int total = total1 > total2 ? total2 : total1;
  // and return 0 if there was a treeline without any used hits
  if (total == 0) return 0;

  // else return a value between 0 and 100
  return (10000 * common / total + 50 ) / 100;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*------------------------------------------------------------------------*//*!

 \brief The best (by chi^2) treelines are select

*//*-------------------------------------------------------------------------*/
int QwTrackingTreeSort::rcTreeConnSort (
	QwTreeLine *treelinelist,	///< list of tree lines
	EQwRegionID region)			///< region
{
  // Maximum allowed chi value (was 20000.0)
  double maxchi = 35000.0;


  /* Reduce the number of treelines
   * - by removing treelines with high chi values
   * - by reducing the maximum allowed chi^2 value if necessary
   */
  int index = 0;
  int iteration = 0;
  int nTooManyTreeLines = 0;
  do {
    double local_maxchi = 0.0;
    double local_minchi = maxchi;

    // Keep track of the number of iterations
    ++iteration;

    index = 0;
    // Loop over the list of treelines
    for (QwTreeLine* treeline = treelinelist;
         treeline; treeline = treeline->next) {

      // If we have been doing this for too long already, give up already
      if (iteration > 100 ) {
	if (fDebug) {
	  cout << *treeline;
	  cout << "... void because too many treelines already." << endl;
	}
	treeline->SetVoid();
	nTooManyTreeLines++;

      // Otherwise consider valid treelines
      } else if (treeline->IsValid()) {
	// Get weighted chi
	double chi = treeline->GetChiWeight();

	// Discard the treeline if chi is too large
	if (chi > maxchi) {
	  if (fDebug) {
	    cout << *treeline;
	    cout << "... void because chi^2 = " << chi << " above " << maxchi << endl;
	  }
	  treeline->SetVoid();

	// Otherwise consider this treeline
	} else {
	  // Keep track of smallest and largest chi value
	  if (chi > local_maxchi) {
	    local_maxchi = chi;
	  }
	  if (chi < local_minchi) {
	    local_minchi = chi;
	  }
	  index++;
	}
      }
    } // end of loop over the list of tree lines

    // Reduce search range to two thirds of range found
    maxchi = local_minchi + (local_maxchi - local_minchi) * 0.66;
    // until we have fewer track
  } while (index > 30 );
  // Number of treelines
  int nTreeLines = index;

  // Skip this event if we had no or too many tree lines.
  if (nTooManyTreeLines != 0) {
    if (fDebug) std::cout << "Skipping event, too many treelines." << std::endl;
    return 0;
  }
  if (nTreeLines == 0) {
    if (fDebug) std::cout << "Skipping event, no good treelines." << std::endl;
    return 0;
  }
  if (fDebug) cout << "Number of treelines with good chi: " << nTreeLines << endl;


  /* Now add the valid treelines to new lists */

  // Reserve memory for the lists of treelines
  char* connarr = (char*)   malloc (nTreeLines);
  int*   isvoid = (int*)    malloc (nTreeLines * sizeof(int));
  double*   chi = (double*) malloc (nTreeLines * sizeof(double));
  int*    array = (int*)    malloc (nTreeLines * nTreeLines * sizeof(int));
  QwTreeLine **tlarr = (QwTreeLine**) malloc (nTreeLines * sizeof(QwTreeLine*));

  // Loop over the treelines and store valid treelines in new list
  index = 0;
  for (QwTreeLine* treeline = treelinelist;
       treeline; treeline = treeline->next) {
    if (treeline->IsValid()) {
      tlarr[index]  = treeline;
      isvoid[index] = treeline->IsVoid();
      chi[index]    = treeline->GetChiWeight();
      index++;
    }
  } // end of loop over treelines


  /* Build the graph array:
   *  for every accepted tree line, store in a matrix which two tree lines
   *  shares at least one quarter of the hit wires with eachother.
   *
   * E.g. for three tree lines where tl1 and tl3 share enough hits:
   *  | 0 0 1 |
   *  | 0 0 0 |
   *  | 1 0 0 |
   *
   * The matrix is stored in serialized form.
   */
  if (region == kRegionID3) {
    for (int i = 0; i < nTreeLines; i++) {
      array[i * nTreeLines + i] = false; // diagonals are set to false
      for (int j = i+1; j < nTreeLines; j++) {
        int common = rcCommonWires_r3 (tlarr[i], tlarr[j]);
        array[i * nTreeLines + j] = array[j * nTreeLines + i] = (common > 25);
      }
    }
  } else { /* region == kRegionID2 */
    for (int i = 0; i < nTreeLines; i++) {
      array[i * nTreeLines + i] = false;
      for (int j = i+1; j < nTreeLines; j++) {
        int common = rcCommonWires (tlarr[i], tlarr[j]);
        array[i * nTreeLines + j] = array[j * nTreeLines + i] = (common > 100);
      }
    }
  }

  /* --------------------------------------------------------------------
  * check connectivity (?)
  * -------------------------------------------------------------------- */
  for (int i = 0; i < nTreeLines; ) {
    if (isvoid[i] == false ) {
      int bestconn = connectiv (0, array, isvoid, nTreeLines, i);
      if (bestconn > 0) {
	if (connectarray (connarr, array, isvoid, nTreeLines, i)) continue;
	bestunconnected (connarr, array, isvoid, chi, nTreeLines, i);
      } else {
	isvoid[i] = good; // good
      }
    } else
      i++;
  }
  for (int i = 0; i < nTreeLines; i++) {
    if (isvoid[i] == true) {
      if (connectiv (0, array, isvoid, nTreeLines, i)) {
	connectarray (connarr, array, isvoid, nTreeLines, i);
	bestconnected (connarr, array, isvoid, chi, nTreeLines, i);
      }
    }
  }
  for (int i = 0; i < nTreeLines; i++) {
    if (isvoid[i] != true) {
      tlarr[i]->SetValid();
    } else {
      if (fDebug) {
        cout << *tlarr[i];
        cout << "... void for some reason." << endl;
      }
      tlarr[i]->SetVoid();
    }
  }

  // Free malloc'ed arrays
  free(chi);
  free(connarr);
  free(array);
  free(isvoid);
  free(tlarr);

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeSort::rcPartConnSort (std::vector<QwPartialTrack*> parttracklist)
{
  char *connarr;
  int *array;
  QwPartialTrack **ptarr, *parttrack;
  int  *isvoid;
  double   *chia, chi, maxch = 2000.0, nmaxch, nminch;
  /* ------------------------------------------------------------------
   * find the number of used QwPartialTracks
   * ------------------------------------------------------------------ */

  int rep = 0;
  int idx = 0;
  do {        /* filter out high chi2 if needed */
    rep++;
    idx = 0;
    nmaxch = 0.0;
    nminch = maxch;
    for (size_t pt = 0; pt < parttracklist.size(); pt++) {
      parttrack = parttracklist[pt];
      if (parttrack->fIsVoid == false ) {
        chi = parttrack->GetChiWeight();
        if (chi > maxch) {
          if (fDebug) {
            cout << *parttrack;
            cout << "... void because chi^2 too high" << endl;
          }
          parttrack->fIsVoid = true;
        } else {
          if (chi > nmaxch) {
            nmaxch = chi;
          }
          if (chi < nminch) {
            nminch = chi;
          }
          idx++;
        }
      }
    }
    maxch = nminch + (nmaxch - nminch) * 0.66;
  } while (idx > 30 && rep < 10);


  int num = idx;

  if (! num)
    return 0;

  //the following mallocs replaced Qmallocs
  connarr = (char *)malloc( num );
  isvoid  = (int *)malloc( num * sizeof(int));
  chia    = (double *)malloc( num * sizeof(double));
  array   = (int*) malloc(num * num * sizeof(int));
  ptarr   = (QwPartialTrack **)malloc( sizeof(QwPartialTrack*)*num);

  if (! ptarr || ! array) {
    fprintf(stderr,"Cannot Allocate Sort Array for %d PartialTracks\n",num);
    abort();
  }

  /* ----------------------------------------------------------------------
   * find the used parttracks
   * ---------------------------------------------------------------------- */

  for (size_t pt = 0, idx = 0; pt < parttracklist.size(); pt++) {
    parttrack = parttracklist[pt];
    if (parttrack->fIsVoid == false) {
      ptarr[idx]  = parttrack;
      isvoid[idx] = parttrack->fIsVoid;
      chia[idx]   = parttrack->GetChiWeight();
      idx++;
    }
  }

  /* ----------------------------------------------------------------------
   * build the graph array
   * ---------------------------------------------------------------------- */

  for (int i = 0; i < num; i++) {
    array[i * num + i] = false;
    for (int j = i+1; j < num; j++) {
      array[i * num + j] = array[j * num + i] =
        (rcPTCommonWires(ptarr[i], ptarr[j]) > 25);
    }
  }

  /* --------------------------------------------------------------------
   * check connectivity
   * -------------------------------------------------------------------- */

  for (int i = 0; i < num; ) {
    if (isvoid[i] == false) {
      int bestconn =  connectiv( 0, array, isvoid, num, i);
      if (bestconn > 0) {
        if (connectarray(connarr, array, isvoid, num, i))
          continue;

        bestunconnected (connarr, array, isvoid, chia, num, i);

      } else
        isvoid[i] = good;//good
    } else
      i++;
  }

  for (int i = 0; i < num; i++) {
    if (isvoid[i] != true) {
      ptarr[i]->fIsVoid = false;
    } else
      ptarr[i]->fIsVoid = true;
  }

  // Free malloc'ed arrays
  free(chia);
  free(connarr);
  free(array);
  free(isvoid);
  free(ptarr);

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
