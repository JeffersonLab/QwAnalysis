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

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeSort::QwTrackingTreeSort (void)
{
  debug = 1;
  if( debug )
      cout<<"###### Calling QwTrackingTreeSort::QwTrackingTreeSort ()"<<endl;

  doubletrack = 0.2;
  good = 2;

  if( debug )
      cout<<"###### Leaving QwTrackingTreeSort::QwTrackingTreeSort ()"<<endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeSort::~QwTrackingTreeSort (void)
{
  if( debug )
      cout<<"###### Calling QwTrackingTreeSort::~QwTrackingTreeSort ()"<<endl;

  if( debug )
      cout<<"###### Leaving QwTrackingTreeSort::~QwTrackingTreeSort ()"<<endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* ======================================================================
 * checks for connected lines on the connectivity array
 * ====================================================================== */

int QwTrackingTreeSort::connectiv( char *ca, int *array, int *isvoid, char size, int idx )
{
  	int ret, j;
  	idx *= size;
  	for( ret = j = 0; j < size; j++ ) {
    		if( array[j+idx] && (!ca || ca[j]) && isvoid[j] != true)
      			ret ++;
  	}
  	return ret;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double QwTrackingTreeSort::chiweight (QwTrackingTreeLine *tl)
{
  double fac;
  if (tl->numhits > tl->nummiss)
    fac = (double) (tl->numhits + tl->nummiss)
                 / (tl->numhits - tl->nummiss);
  else {
    cerr << "miss = " << tl->nummiss << ", hit = " << tl->numhits << endl;
    return 100000.0; // This is bad...
  }
  return fac * tl->chi;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double QwTrackingTreeSort::ptchiweight (PartTrack *pt)
{
  double fac;
  if (pt->numhits > pt->nummiss)
    fac = (double) (pt->numhits + pt->nummiss)
                 / (pt->numhits - pt->nummiss);
  else {
    cerr << "miss = " << pt->nummiss << ", hit = " << pt->numhits << endl;
    return 100.0; // This is bad...
  }
  return fac * fac * pt->chi; // Why is 'fac' squared here, but not in chiweight?
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeSort::connectarray (char *ca, int *array, int *isvoid, char size, int idx )
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

int QwTrackingTreeSort::globalconnectiv (char *ca, int *array, int *isvoid, int size, int idx)
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

int QwTrackingTreeSort::bestconnected (char *ca, int *array, int *isvoid, double *chia,
	       int size, int idx)
{
  int i, besti = -1;
  double bestchi = 1e8, oworst = 0.0;
  double chi;

  for(i = idx; i<size; i++ ) {
    if( !ca[i] )
      continue;
    chi = chia[i];
    if( isvoid[i] != true) {

      if( oworst < chi )
	oworst = chi;
      continue;
    }
    if( globalconnectiv( ca, array, isvoid, size, i) > 2 )
      continue;

    if( bestchi > chi ) {
      besti = i;
      bestchi = chi;
    }
  }
  if( besti >= 0 && (3.0 > bestchi || oworst + 3.0 > bestchi )) {
    isvoid[i] = good;//good
    return 1;
  }
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeSort::rcPTCommonWires (PartTrack *track1, PartTrack *track2)
{
  int common = 0;
  for (int i = 0; i < 3; i++)
    common += rcCommonWires (track1->tline[i], track2->tline[i]);
  common /= 3;
  return common;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* This function simply counts the number of common wires
shared between two treelines.  The only output is the integer
return value
*/
int QwTrackingTreeSort::rcCommonWires_r3 (QwTrackingTreeLine *line1, QwTrackingTreeLine *line2)
{
  //################
  // DECLARATIONS  #
  //################
  int total1, total2, common, total;
  Hit **hits1, **hits2;

  //##################
  //DEFINE VARIABLES #
  //##################
  common = total1 = total2 = total = 0;
  hits1  = line1->hits;
  hits2  = line2->hits;

  //##############################################
  //Count the wires shared between the treelines #
  //##############################################
  int i1 = 0, i2 = 0;
  for ( ; i1 < line1->numhits && i2 < line2->numhits ; ) {
    if (hits1[i1]->wire == hits2[i2]->wire) {
      if (hits1[i1]->used && hits2[i2]->used)
	common++;
      i1++;
      i2++;
      total++;
    }
    else if (hits1[i1]->wire > hits2[i2]->wire) {
      i2++;
      total++;
    }
    else if (hits1[i1]->wire < hits2[i2]->wire) {
      i1++;
      total++;
    }
  }
  total += line1->numhits - i1 + line2->numhits - i2;
/*
  for( ;; ) {
    // A
    if( fw & 1 ) {//Set i1 equal to the index of the next hit used in line1
      i1++;
      for( ; i1 < TLAYERS && hits1[i1]; i1++)
	if( hits1[i1]->used ) {
	  total1++;
	  break;
	}
    }
    // B
    if( fw & 2 ) {//Set i2 equal to the index of the next hit used in line2
      i2++;
      for( ; i2 < TLAYERS && hits2[i2]; i2++)
	if( hits2[i2]->used ) {
	  total2++;
	  break;
	}
    }
    // ---
    if( i1 == TLAYERS || ! hits1[i1] ||
	i2 == TLAYERS || ! hits2[i2] )
      break;//break if we reach the end of the hits in either line
    //-----------------------------------------------------------
    //The following lines separate hits in different detectors
    did1 = hits1[i1]->detec->ID;//this line was altered
    did2 = hits2[i2]->detec->ID;//this line was altered

    cout << "dids = (" << did1 << "," << did2 << ")" << endl;
    if( did1 < did2 )
      fw = 1;// do only A in the next iteration
    else if( did1 > did2 )
      fw = 2;// do only B in the next iteration
    else {
      if( hits1[i1]->wire == hits2[i2]->wire )
	common ++;
      fw = 3;// do both A and B next iteration
    }
    //-----------------------------------------------------------
  }
*/
//Check which line has more hits used
//  total = total1 > total2 ? total2 : total1;

  if (! total)
    return 0;

  // Get a ratio and mulitply it to return an integer
  return (10000 * common / total + 50 ) / 100;
  // if 8 common out of 8, then this = 100
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* This function simply counts the number of common wires
shared between two treelines.  The only output is the integer
return value
*/
int QwTrackingTreeSort::rcCommonWires (QwTrackingTreeLine *line1, QwTrackingTreeLine *line2 )
{
  //cerr << "ERROR : This function needs editing before use" << endl;

  //################
  // DECLARATIONS  #
  //################
  int total1, total2, common, total;
  Hit **hits1, **hits2;
  int did1, did2;
  int  i1, i2, fw = 3;

  //##################
  //DEFINE VARIABLES #
  //##################
  common = total1 = total2 = 0;
  i1 = i2 = -1;
  hits1  = line1->hits;
  hits2  = line2->hits;

  //##################
  //DO STUFF #
  //##################
  for (;;) {
    if (fw & 1) { /* Set i1 equal to the index of the next hit used in line1 */
      i1++;
      for ( ; i1 < DLAYERS*MAXHITPERLINE && hits1[i1]; i1++)
	if (hits1[i1]->used) {
	  total1++;
	  break;
	}
    }
    if (fw & 2) { /* Set i2 equal to the index of the next hit used in line2 */
      i2++;
      for ( ; i2 < DLAYERS*MAXHITPERLINE && hits2[i2]; i2++)
	if (hits2[i2]->used) {
	  total2++;
	  break;
	}
    }
    if (i1 == DLAYERS*MAXHITPERLINE || ! hits1[i1] ||
	i2 == DLAYERS*MAXHITPERLINE || ! hits2[i2])
      break; /* break if we reach the end of the hits in either line */
    //-----------------------------------------------------------
    //The following lines separate hits in different detectors
    did1 = hits1[i1]->detec->ID;
    did2 = hits2[i2]->detec->ID;

    if (did1 < did2)
      fw = 1;
    else if (did1 > did2)
      fw = 2;
    else {
      if (hits1[i1]->wire == hits2[i2]->wire)
	common++;
      fw = 3;
    }
    //-----------------------------------------------------------
  }

  //##################
  //DO STUFF #
  //##################
  total = total1 > total2 ? total2 : total1;

  if (! total)
    return 0;

  return (10000 * common / total + 50 ) / 100;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeSort::rcTreeConnSort (QwTrackingTreeLine *head, EQwRegionID region)
{
  //################
  // DECLARATIONS  #
  //################
  char* connarr;
  int* array;
  QwTrackingTreeLine **tlarr, *walk;
  int num, idx, bestconn, common;
  int iteration = 0;
  int num_tl = 0;
  int  *isvoid;
  double   *chia, chi, nmaxch, nminch;
  double maxchi = 35000.0; // maximum allowed chi^2 (was 20000.0)

  //DBG = DEBUG & D_GRAPH;

/* ----------------------------------------------------------------------
 * find the number of used treelines
 * ---------------------------------------------------------------------- */

  do {				/* filter out high chi2 if needed */
    nmaxch = 0.0;
    iteration++;

    nminch = maxchi;
    for (idx = 0, walk = head; walk; walk = walk->next) {
      if (iteration > 100 ) {	/* skip the event */
	walk->isvoid = true;
	num_tl++;
      }
      else if (walk->isvoid == false) {
	chi = chiweight (walk);
	if (chi > maxchi) {
	  cerr << "[QwTrackingTreeSort::rcTreeConnSort] chi^2 too high: " << chi << " > " << maxchi << endl;
	  walk->isvoid = true;
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
    maxchi = nminch + (nmaxch - nminch) * 0.66;
  } while (idx > 30 ); // 30?!? should probably reduce this

  num = idx;
  if (debug) cout << "Number of treelines with reasonable chi^2: " << num << endl;
  if (num_tl) {
    std::cout << "Skipping event because of 0 good treelines." << std::endl;
    return 0;
  }
  if (! num) {
    std::cout << "Skipping event because of 0 good treelines." << std::endl;
    return 0;
  }

  // the following mallocs replaced Qmallocs
  connarr = (char*) malloc (num);
  isvoid  = (int*) malloc (num * sizeof(int));
  chia    = (double*) malloc (num * sizeof(double));
  array   = (int*) malloc (num*num*sizeof(int));
  tlarr   = (QwTrackingTreeLine**) malloc (sizeof(QwTrackingTreeLine*)*num);

  assert (array && tlarr);

  /* ----------------------------------------------------------------------
  * find the used treelines
  * ---------------------------------------------------------------------- */

  for (idx = 0, walk = head; walk; walk = walk->next) {
    if (walk->isvoid == false) {
      tlarr[idx]  = walk;
      isvoid[idx] = walk->isvoid;
      chia[idx]   = chiweight(walk);
      idx++;
    }
  }

  /* ----------------------------------------------------------------------
  * build the graph array
  * ---------------------------------------------------------------------- */
  if (region == kRegionID3) {
    for (int i = 0; i < num; i++) {
      array[i*num+i] = 0;
      for (int j = i+1; j < num; j++) {
	common = rcCommonWires_r3 (tlarr[i], tlarr[j]);
	array[i*num+j] = array[j*num+i] = (common > 25); // this is true if
	// one of the two lines shares at least 1/4 of its wires with the other line
      }
    }
  } else {
    for (int i = 0; i < num; i++) {
      array[i*num+i] = 0;
      for (int j = i+1; j < num; j++) {
        array[i*num+j] = array[j*num+i] = (rcCommonWires (tlarr[i], tlarr[j]) > 25);// 25?!?
      }
    }
  }

  /* --------------------------------------------------------------------
  * check connectivity
  * -------------------------------------------------------------------- */
  for (int i = 0; i < num; ) {
    if (isvoid[i] == false ) {
      bestconn =  connectiv( 0, array, isvoid, num, i);
      if (bestconn > 0) {
	if (connectarray (connarr, array, isvoid, num, i)) continue;
	bestunconnected (connarr, array, isvoid, chia, num, i);
      } else {
	isvoid[i] = good; // good
      }
    } else
      i++;
  }
  for (int i = 0; i < num; i++) {
    if (isvoid[i] == true) {
      if (connectiv (0, array, isvoid, num, i)) {
	connectarray (connarr, array, isvoid, num, i);
	bestconnected (connarr, array, isvoid, chia, num, i);
      }
    }
  }
  for (int i = 0; i < num; i++) {
    if (isvoid[i] != true) {
      tlarr[i]->isvoid = false;
    } else {
      tlarr[i]->isvoid = true;
    }
  }
  /* do not free Qalloc'ed things!!! */
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeSort::rcPartConnSort (PartTrack *head)
{
  char *connarr;
  int *array;
  PartTrack **ptarr, *walk;
  int num, idx, i, j, bestconn;
  int  *isvoid;
  double   *chia, chi, maxch = 200.0, nmaxch, nminch;
  /* ------------------------------------------------------------------
   * find the number of used PartTracks
   * ------------------------------------------------------------------ */

  //DBG = DEBUG & D_GRAPHP;

  do {        /* filter out high chi2 if needed */
    nmaxch = 0.0;
    nminch = maxch;
    for (idx = 0, walk = head; walk; walk = walk->next) {
      if (walk->isvoid == false ) {
        chi = ptchiweight(walk);
        if (chi > maxch) {
          walk->isvoid = true;
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
  } while( idx > 30 );


  num = idx;

  if (! num)
    return 0;

  //the following mallocs replaced Qmallocs
  connarr = (char *)malloc( num );
  isvoid  = (int *)malloc( num * sizeof(int));
  chia    = (double *)malloc( num * sizeof(double));
  array   = (int *)malloc( num*num);
  ptarr   = (PartTrack **)malloc( sizeof(PartTrack*)*num);

  if (! ptarr || ! array) {
    fprintf(stderr,"Cannot Allocate Sort Array for %d PartialTracks\n",num);
    abort();
  }
  /*
  if( DE && (DEBUG & D_GRAPHP)) {
    printf("----------SORT PT %c %c\n",
    "FB"[vispar],"UL"[viswer]);
  }
  */

  /* ----------------------------------------------------------------------
   * find the used parttracks
   * ---------------------------------------------------------------------- */

  for (idx = 0, walk = head; walk; walk = walk->next) {
    if (walk->isvoid == false) {
      ptarr[idx]  = walk;
      isvoid[idx] = walk->isvoid;
      chia[idx]   = ptchiweight(walk);
      idx++;
    }
  }

  /* ----------------------------------------------------------------------
   * build the graph array
   * ---------------------------------------------------------------------- */

  for (i = 0; i < num; i++) {
    array[i*num+i] = 0;

    for (j = i+1; j < num; j++) {
      array[i*num+j] = array[j*num+i] =
        (rcPTCommonWires(ptarr[i], ptarr[j]) > 25);
    }
  }
  /*
  if( DE && (DEBUG & D_GRAPHP)) {
    for( i = 0; i < num; i++ ) {
      if( isvoid[i] != true )
        printf("pthave (%d) %f,%f %f,%f - %f %d (%d)\n", i,
        ptarr[i]->x,
        ptarr[i]->y,
        ptarr[i]->mx,
        ptarr[i]->my,
        ptchiweight(ptarr[i]),
        ptarr[i]->nummiss,
        connectiv( 0, array, isvoid, num, i));
    }
  }
  */
  /* --------------------------------------------------------------------
   * check connectivity
   * -------------------------------------------------------------------- */

  for (i = 0; i < num; ) {
    if (isvoid[i] == false) {
      bestconn =  connectiv( 0, array, isvoid, num, i);
      if (bestconn > 0) {
        if (connectarray(connarr, array, isvoid, num, i))
          continue;
        /*
        if( DE && (DEBUG & D_GRAPHP)) {
          printf(" %d-connections:", i);
          for( j = 0; j < num; j++ ) {
            if( connarr[j])
              printf("%d ",j);
          }
          puts("");
        }
        */
        bestunconnected (connarr, array, isvoid, chia, num, i);
        /*
        if( DE && (DEBUG & D_GRAPHP))
          puts("");
        */
      } else
        isvoid[i] = good;//good
    } else
      i++;
  }
  /*
  if( DE && (DEBUG & D_GRAPHP)) {
    for( i = 0; i < num; i++ ) {
      if( isvoid[i] != true )
  printf("ptkeep (%d) %f,%f %f,%f - %f %d (%d)\n", i,
         ptarr[i]->x,
         ptarr[i]->y,
         ptarr[i]->mx,
         ptarr[i]->my,
         ptchiweight(ptarr[i]),
         ptarr[i]->nummiss,
         connectiv( 0, array, isvoid, num, i));
    }
  }
  */
  for (i = 0; i < num; i++) {
    if (isvoid[i] != true) {
      //Statist[method].PartTracksUsed[where][part] ++;
      ptarr[i]->isvoid = false;
    } else
      ptarr[i]->isvoid = true;
  }

  /* do not free Qalloc'ed things!!! */
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
