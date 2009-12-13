//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
// C++ Interface: QwTrackingTreeMatch
//
// Description:
//
//
// Author: Burnham Stocks <bestokes@jlab.org>
// Original HRC Author: wolfgang Wander <wwc@hermes.desy.de>
//
// Modified by: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//              Jie Pan <jpan@jlab.org>, Thu May 28 22:01:11 CDT 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \class QwTrackingTreeMatch

    \file QwTrackingTreeMatch.cc

    $date: Thu May 28 22:01:11 CDT 2009 $

    \brief This module matches track segments for individual wire planes.
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwTrackingTreeMatch.h"

// System C and C++ headers
#include <cstdio>
#include <fstream>
#include <cassert>
#include <cstdlib>

// Qweak headers
#include "QwTrackingTreeCombine.h"

#include "Det.h"

extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double rcPEval( double vz, double te, double ph, double bend){
  std::cerr << "Error: THIS FUNCTION IS ONLY A STUB rcPEval " << std::endl;
  return -1000;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double rcZEval( double vz, double te, double ph, double mom, int idx){
  std::cerr << "Error: THIS FUNCTION IS ONLY A STUB rcZEval " << std::endl;
  return -1000;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeMatch::QwTrackingTreeMatch()
{
  fDebug = 0; // debug level
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeMatch::~QwTrackingTreeMatch()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* VDC reference frame : The center of the upstream u or v wire plane is placed
   at the origin.  The u or v wire plane is in the y-z plane.  The center of the
   downstream u or v plane is at (d,d2,0).  The line slopes are calculated with
   a different reference frame: distance between wires representing dy and
   distance from wire representing dx.
*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// This function requires the wire planes to be parallel
QwTrackingTreeLine *QwTrackingTreeMatch::MatchR3 (
	QwTrackingTreeLine *frontlist,
	QwTrackingTreeLine *backlist,
	EQwDetectorPackage package,
	EQwRegionID region,
	EQwDirectionID dir)
{
  if (region != kRegionID3) {
    std::cerr << "Error, this function is only for R3" << std::endl;
    return 0;
  }

  //###############
  // DECLARATIONS #
  //###############
  QwTrackingTreeLine *combined = NULL;
  double x[2],y[2],z[3],zp[2];
  double d, d2, d2u, d_uv = 0.0;
  double pi = acos(-1.0), theta;
  Det *rd;
  double bestmatch;
  int matchfound = 0;
  QwTrackingTreeCombine *TreeCombine = new QwTrackingTreeCombine();
  QwHit *DetecHits[2*TLAYERS];

  //###################################
  // Get distance between planes, etc #
  //###################################
  rd = rcDETRegion[package][region][dir];
  theta = rd->Rot/360*2*pi;

  // Get the u value for the first wire.
  double d_to_1st_wire_f = rd->rSin * rd->PosOfFirstWire;
  // ... due to reverse order
  d_to_1st_wire_f = d_to_1st_wire_f - rd->NumOfWires * rd->WireSpacing;

  double wirespacingf = rd->WireSpacing;
  double fsloperes = rd->SlopeMatching;
  x[0]= rd->center[0];
  y[0]= rd->center[1];
  z[0]= rd->Zpos;
  zp[0] = z[0] - y[0] / tan(theta);

  rd = rd->nextsame;
  x[1]= rd->center[0];
  y[1]= rd->center[1];
  z[1]= rd->Zpos;
  zp[1] = z[1] - y[1] / tan(theta);

  d = (zp[1] - zp[0]) * sin(theta); // distance between the planes
  // cerr << "d = " << d << endl;
  double wirespacingb = rd->WireSpacing;
  double bsloperes = rd->SlopeMatching;

  // Get the u value for the first wire.
  double d_to_1st_wire_b = rd->rSin * rd->PosOfFirstWire;
  // ... due to reverse order
  d_to_1st_wire_b = d_to_1st_wire_b - rd->NumOfWires * rd->WireSpacing;


  // Get the distance between the u and v planes
  if (dir == kDirectionV) {
    double RotCos = rcDETRegion[package][region][kDirectionU]->rRotCos;
    double RotSin = rcDETRegion[package][region][kDirectionU]->rRotSin;
    double RotTan = RotSin / RotCos;

    d_uv  = rcDETRegion[package][region][kDirectionV]->Zpos
          - rcDETRegion[package][region][kDirectionU]->Zpos;
    d_uv += ( rcDETRegion[package][region][kDirectionU]->center[1]
            - rcDETRegion[package][region][kDirectionV]->center[1] ) / RotTan;
    d_uv *= RotSin;
  }

  //######################################
  // Get the radial offset of the planes #
  //######################################
  // warning : this assumes the planes are parallel.

  z[2] = z[0] + d * sin(theta);
  //d2 = (z[2] - z[1]) / cos(theta);
  d2 = (y[1] - y[0]) / sin(theta) + (zp[1] - zp[0]) * cos(theta);
  d2u = d2 * fabs(rd->rCos);

  //##########################
  //Revise the hit positions #
  //##########################

  // Loop over the tree lines in the front VDC to set the 'z' position
  int numflines = 0;
  for (QwTrackingTreeLine* frontline = frontlist; frontline;
       frontline = frontline->next, numflines++) {
    if (frontline->IsVoid()) continue;
    for (int hit = 0; hit < frontline->numhits; hit++) {
      frontline->hits[hit]->SetZPosition((frontline->hits[hit]->GetElement() - 141) * wirespacingf);
      if (dir == kDirectionV) frontline->hits[hit]->rPos += d_uv;
    }
  }

  // Loop over the tree lines in the back VDC to set the 'z' position
  int numblines = 0;
  for (QwTrackingTreeLine* backline = backlist; backline;
       backline = backline->next, numblines++) {
    if (backline->IsVoid()) continue;
    for (int hit = 0; hit < backline->numhits; hit++) {
      backline->hits[hit]->SetZPosition((backline->hits[hit]->GetElement() - 141) * wirespacingb + d2u);
      backline->hits[hit]->rPos = backline->hits[hit]->rPos + d;
      if (dir == kDirectionV) backline->hits[hit]->rPos += d_uv;
    }
  }


  //###############################
  // Find matching track segments #
  //###############################
  int fmatches[numflines]; // matches indexed by front tree lines
  int bmatches[numblines]; // matches indexed by back tree lines
  double bestmatches[numblines];

  for (int i = 0; i < numblines; i++) bestmatches[i] = 99;

  // Loop over the tree lines in the front VDC
  int ifront = 0;
  for (QwTrackingTreeLine* frontline = frontlist; frontline;
       frontline = frontline->next, ifront++) {

    fmatches[ifront] = -1;
    if (frontline->IsVoid()) continue; // Skip it if it's no good

    QwHit* fpos = frontline->bestWireHit();
    bestmatch = 99;

    // Loop over the tree lines in the back VDC
    int iback = 0;
    for (QwTrackingTreeLine* backline = backlist; backline;
         backline = backline->next, iback++) {

      if (backline->IsVoid()) continue;
      QwHit* bpos = backline->bestWireHit(d);

      y[0] = fpos->GetZPosition(); // Z position (i.e. wire direction)
      y[1] = bpos->GetZPosition();
      x[0] = fpos->rPos; // X position (i.e. drift distance)
      x[1] = bpos->rPos;

      double slope = (y[1] - y[0]) / (x[1] - x[0]);
      // NOTE unused:
      //double intercept = y[1] - slope * x[1];

      double fslope = wirespacingf / frontline->fSlope;
      double bslope = wirespacingb / backline->fSlope;

      // TODO Fudge fudge fudge
      slope *= 2.0;

      // Check whether this is a good match
      if (fabs(fslope - slope) <= fsloperes
       && fabs(bslope - slope) <= bsloperes
       && fabs(fslope - slope) + fabs(bslope - slope) < bestmatch) {

        // If the back segment has been matched already
        if (bestmatches[iback] != 99) {
          // Check if it's better than what we had already
          bestmatch = fabs(fslope - slope) + fabs(bslope - slope);
          if (bestmatch > bestmatches[iback]) continue;
          else fmatches[bmatches[iback]] = -1;
        }

        // Set the match on both match arrays
        fmatches[ifront] = iback;
        bmatches[iback] = ifront;
        bestmatch = bestmatches[iback] = fabs(fslope - slope) + fabs(bslope - slope);
      } // end of if good match

    } // end of loop over back VDC tree lines

  } // end of loop over front VDC tree lines

  //################################
  // Create the combined treelines #
  //################################
  QwTrackingTreeLine* lineptr = new QwTrackingTreeLine;
  assert(lineptr);

  // Loop over the tree lines in the front VDC
  ifront = 0;
  for (QwTrackingTreeLine* frontline = frontlist; frontline;
       frontline = frontline->next, ifront++) {

    // Loop over the tree lines in the back VDC
    int iback = 0;
    for (QwTrackingTreeLine* backline = backlist; backline;
         backline = backline->next, iback++) {

      // If this front segment was matched to this back segment
      if (fmatches[ifront] == iback) {

        // Set the hits for front VDC
        int fronthits = frontline->numhits;
        for (int hit = 0; hit < fronthits; hit++) {
          DetecHits[hit] = frontline->hits[hit];
          lineptr->hits[hit] = frontline->hits[hit];
        }
        // Set the hits for back VDC
        int backhits = backline->numhits;
        for (int hit = 0; hit < backhits; hit++) {
          DetecHits[hit+fronthits] = backline->hits[hit];
          lineptr->hits[hit+fronthits] = backline->hits[hit];
        }
        int nhits = fronthits + backhits;
        // Debug output
        if (fDebug) {
          for (int hit = 0; hit < nhits; hit++) {
            std::cout << DetecHits[hit]->GetZPosition() << " " << DetecHits[hit]->GetDriftDistance() << endl;
          }
        }

        // Fit a line to the hits
        double slope, offset, chi, cov[3];
        TreeCombine->weight_lsq_r3 (&slope, &offset, cov, &chi, DetecHits, nhits, 0, -1, 2*TLAYERS);
        lineptr->fSlope = slope;
        lineptr->fOffset = offset;
        lineptr->fChi = chi;
        lineptr->numhits = nhits;
        lineptr->nummiss = 2*TLAYERS - nhits;
        lineptr->SetValid();

        for (int hit = 0; hit < fronthits; hit++)
          lineptr->usedhits[hit] = DetecHits[hit];
        for (int hit = fronthits; hit < fronthits + backhits; hit++)
          lineptr->usedhits[hit] = DetecHits[hit];

        lineptr->next = combined;
        combined = lineptr;
        matchfound = 1;
      }
    }
  }

  if (!matchfound) {
    return 0;
  }

  return combined;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingTreeMatch::TgTrackPar (
	QwPartialTrack *front,	///< front partial track
	QwPartialTrack *back,	///< back partial track
	double *theta,		///< determined polar angle
	double *phi,		///< determined azimuthal angle
	double *bending,	///< bending in polar angle
	double *ZVertex)	///< determined z vertex
{
  *theta = atan(front->fSlopeX);
  *phi   = atan(front->fSlopeY);
  if (bending && back)
    *bending = atan(back->fSlopeX) - *theta;
  *ZVertex = - ( (front->fSlopeX * front->fOffsetX + front->fSlopeY * front->fOffsetY)
	       / (front->fSlopeX * front->fSlopeX  + front->fSlopeY * front->fSlopeY));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrack* QwTrackingTreeMatch::TgPartMatch (
	QwPartialTrack *front,		///< front partial track
	QwPartialTrack *back,		///< back partial track
	QwTrack *tracklist,		///< list of tracks
	EQwDetectorPackage package	///< package identifier
	/*enum Emethod method*/)
{
  double bestchi = 1e10, chi;
  double v1, v2, v3;
  QwTrack *ret = 0, *newtrack = 0, *besttrack = 0, *trackwalk, *ytrackwalk;
  QwBridge *bridge;
  //int m = method == meth_std ? 0 : 1;
  double theta, ZVertex, phi, bending, P;


  while (back) {
    /* --------- check for the cuts on magnetic bridging ------------ */
    if (back->IsVoid() == false
	 //removed all the following cuts
	  /*&& (fabs(front->y-back->y))            < rcSET.rMagnMatchY[m]*3.0
	  && (fabs(front->x-back->x ))           < rcSET.rMagnMatchX[m]*3.0
	  && ( (front->method != meth_std && ! bNoForceBridge) ||
	       (fabs(front->my - back->my ))     < rcSET.rMagnMatchYSl[m]*2.0 )
	  && (fabs( front->x+front->mx*
		    (target_center-magnet_center) )) < target_width
	  && (fabs( front->y+front->my*
		    (target_center-magnet_center) )) < target_width*/ ) {

      newtrack = new QwTrack; /*  a new track */
      assert(newtrack);
      //TgInit( newtrack );

      /* ----- keep bridging information ----- */
      if (front->bridge)	/* do we have front brigding info (mcheck) */
	bridge = front->bridge;
      else if (back->bridge)	/* or back one (mckalman) ? */
	bridge = back->bridge;
      else
	bridge = new QwBridge;
      assert (bridge);
      TgTrackPar( front, back, &theta, &phi, &bending, &ZVertex);

      ZVertex *= 0.01;
      if( ZVertex < DZA )
	ZVertex = DZA;
      if( ZVertex > DZA + DZW )
	ZVertex = DZA+DZW;
      P = rcPEval( ZVertex, theta, phi, bending);
      if( P > 0.0 ) {
	bridge->fMomentum = P;
	if( bending < 0 )
	  P = -P;
	bridge->xOff  = rcZEval( ZVertex, theta, phi, P, 0);
	bridge->yOff  = rcZEval( ZVertex, theta, phi, P, 1);
	bridge->ySOff = rcZEval( ZVertex, theta, phi, P, 2);
      } else
	bridge->fMomentum = 0.0;
      bridge->ySlopeMatch = back->fSlopeY - front->fSlopeY;
      bridge->xMatch      = v2 = back->fOffsetX - front->fOffsetX + bridge->xOff;
      bridge->yMatch      = v1 = back->fOffsetY - front->fOffsetY + bridge->yOff;
      bridge->ySMatch     = v3 = back->fSlopeY  - front->fSlopeY  + bridge->ySOff;


	double rcSET_rMagnMatchYSl0 = 0.03;//INSERTED FROM HRCSET.C FUNCTION
	double rcSET_rMagnMatchY0 = 1.0;//THESE ARE BOGUS VALUES TO MAKE THE PROG WORK
	double rcSET_rMagnMatchX0 = 3.0;
	std::cerr << "ERROR : INVALID MAGNETIC FIELD VALUES USED " << std::endl;

      if( fabs(v3) > rcSET_rMagnMatchYSl0/*rcSET.rMagnMatchYSl[m] && front->method == meth_std*/)
	v3 = 1e12;		/* reject */

      if( fabs( v2) > rcSET_rMagnMatchY0 ||
	  fabs( v1) > rcSET_rMagnMatchX0 )
	v1 = v2 = 1e12;


      newtrack->bridge = bridge;
      //newtrack->method = method;
      newtrack->front  = front;
      newtrack->back = back;

      /* ------ a weighted measure for the quality of bridging ------ */

      chi = v1 * v1 + v2 * v2 + v3 * v3;
      newtrack->fChi  = sqrt(chi + front->fChi * front->fChi + back->fChi * back->fChi);
      if(bestchi > chi) {
	besttrack = newtrack;
	bestchi = chi;
      }
      newtrack->ynext = ret;
      ret = newtrack;
    }
    back = back->next;
  }

  /* --- for the best track ... */
  if (besttrack) {
    int mtch = 1;
    besttrack->isused = true;	/* set the parttrack used flags */
    besttrack->front->fIsUsed = true;
    besttrack->back->fIsUsed = true;
	double rcSET_rXSlopeSep = 0.01;//INSERTED FROM HRCSET
        std::cerr << "Error : bogus value used" << std::endl;
    for( trackwalk = ret; trackwalk ; trackwalk = trackwalk->next ) {
      if( fabs( newtrack->back->fSlopeX - besttrack->back->fSlopeX ) > rcSET_rXSlopeSep )
        mtch ++;
    }
    besttrack->yTracks = mtch;
/*
    if( bDebugMatch )
      printf("%f %f %f %f %f %f XXmatXX\n",
	     besttrack->front->x, besttrack->back->x, besttrack->bridge->xOff,
	     besttrack->front->y, besttrack->back->y, besttrack->bridge->yOff);
*/
  }
  /* --- check for found back parttracks in other tracks
     --- we only keep the best match --- */
  for( newtrack = ret; newtrack; newtrack = newtrack->ynext ) {
    if( !newtrack->isused )
      continue;
    for( trackwalk = tracklist; trackwalk; trackwalk = trackwalk->next ) {
      for( ytrackwalk = trackwalk; ytrackwalk;
	   ytrackwalk = ytrackwalk->ynext ) {
	/*
	if( ytrackwalk->method != method )
	  continue;
	*/
	if(ytrackwalk->isused && ytrackwalk->back == newtrack->back ) {
	  if (ytrackwalk->fChi > newtrack->fChi) {
	    ytrackwalk->isused = 0;
	  } else {
	    newtrack->isused = 0;
	  }
	}
      }
    }
  }

  return ret;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
