/**
 *  \file   QwTrackingTreeMatch.cc
 *  \brief  Module that matches track segments for pairs of wire planes
 *
 *  \author Burnham Stocks <bestokes@jlab.org>
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *  \author Jie Pan <jpan@jlab.org>
 *
 *  \date   Thu May 28 22:01:11 CDT 2009
 */

#include "QwTrackingTreeMatch.h"

// System C and C++ headers
#include <cstdio>
#include <fstream>
#include <cassert>
#include <cstdlib>

// Qweak headers
#include "QwLog.h"
#include "QwDetectorInfo.h"
#include "QwTrackingTreeCombine.h"

// Qweak tracking headers
#include "QwTrackingTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"

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

/**
 * Match the tree lines in two like-pitched planes in region 3.
 *
 * In this function the list of tree lines in the front VDC plane is combined
 * with the list of tree lines in the back VDC plane.  The resulting list of
 * combined tree lines is returned as a linked-list.  The criterion for keeping
 * a combined tree line is that the following slopes are within their slope
 * matching resolutions:
 * - the slope of the tree line through the front plane,
 * - the slope between the central hits in the front and back planes,
 * - the slope of the tree line through the back plane.
 *
 * The reference frame for the matching is defined with the center of the first
 * wire plane at the origin.  The center of the second wire plane is then at
 * (0, d_para, d_perp) assuming no lateral displacement.  The difference in the
 * u coordinate between the center of the chambers is then given by u_para.
 *
 * The line slopes are calculated in a different reference frame: the distance
 * between the wires (in the plane) is represented by z, the perpendicular
 * distance from the wire is represented by x.
 *
 * \todo This function requires the wire planes to be parallel.
 *
 * \image html QwTrackingTreeMatch-1.jpg
 * \image html QwTrackingTreeMatch-2.jpg
 * \image html QwTrackingTreeMatch-3.jpg
 * \image html QwTrackingTreeMatch-4.jpg
 * \image html QwTrackingTreeMatch-5.jpg
 *
 * @param frontlist List of tree lines in the front plane
 * @param backlist List of tree lines in the back plane
 * @return List of tree lines after matching
 */
QwTrackingTreeLine *QwTrackingTreeMatch::MatchRegion3 (
	QwTrackingTreeLine* frontlist,
	QwTrackingTreeLine* backlist)
{
  // Check the region of the tree lines (only region 3 is allowed)
  if (frontlist->GetRegion() != kRegionID3
    || backlist->GetRegion() != kRegionID3) {
    QwError << "[TreeMatch::MatchR3] Tree line matching is only valid for region 3!"
            << QwLog::endl;
    return 0;
  }

  // Check whether the front and back planes are consistent
  if (frontlist->GetRegion()    != backlist->GetRegion()
   || frontlist->GetPackage()   != backlist->GetPackage()
   || frontlist->GetDirection() != backlist->GetDirection()) {
    QwError << "[TreeMatch::MatchR3] The front and back planes are not consistent!"
            << QwLog::endl;
    return 0;
  }

  // Check whether the planes are different
  if (frontlist->GetPlane() == backlist->GetPlane()) {
    QwError << "[TreeMatch::MatchR3] The front and back planes are identical!"
            << QwLog::endl;
    return 0;
  }


  // Initialize the list of combined tree lines to null
  QwTrackingTreeLine* treelinelist = 0;

  // Initialize the tree combine object (TODO can this be avoided?)
  QwTrackingTreeCombine *TreeCombine = new QwTrackingTreeCombine();


  /// Set up the geometry of the two wire planes: distances between them,
  /// relative orientation, etc.

  // Get detector identification (TODO not assigned correctly yet)
  //QwDetectorInfo* frontdetector = frontlist->GetDetectorInfo();
  //QwDetectorInfo* backdetector  = backlist->GetDetectorInfo();
  // TODO We currently get the detector info from the first tree line hit
  QwDetectorInfo* frontdetector = frontlist->GetHit(0)->GetDetectorInfo();
  QwDetectorInfo* backdetector  = backlist->GetHit(0)->GetDetectorInfo();

  // Rotation of the detector planes around the x axis
  double cos_theta = frontdetector->GetDetectorRotationCos();
  double sin_theta = frontdetector->GetDetectorRotationSin();

  // Get the u value for the first wire in the front plane
  double d_to_1st_wire_f = frontdetector->GetElementAngleSin() * frontdetector->GetElementOffset();
  // ... due to reverse order
  d_to_1st_wire_f -= frontdetector->GetNumberOfElements() * frontdetector->GetElementSpacing();

  // Get the u value for the first wire in the back plane
  double d_to_1st_wire_b = backdetector->GetElementAngleSin() * backdetector->GetElementOffset();
  // ... due to reverse order
  d_to_1st_wire_b -= backdetector->GetNumberOfElements() * backdetector->GetElementSpacing();

  // Wire spacing and slope matching parameters for front and back planes
  double wirespacing_f = frontdetector->GetElementSpacing();
  double wirespacing_b = backdetector->GetElementSpacing();
  double sloperes_f = frontdetector->GetSlopeMatching();
  double sloperes_b = backdetector->GetSlopeMatching();

  // Differences in position between the front and back detector planes
  double delta_x = backdetector->GetXPosition() - frontdetector->GetXPosition();
  double delta_y = backdetector->GetYPosition() - frontdetector->GetYPosition();
  double delta_z = backdetector->GetZPosition() - frontdetector->GetZPosition();

  // Distance between the chamber centers perpendicular to the wire planes
  double d_perp = delta_z * sin_theta - delta_y * cos_theta;
  // Distance between the chamber centers parallel to the wire planes
  double d_para = delta_z * cos_theta + delta_y * sin_theta;
  // Parallel distance between the chamber centers in u or v coordinates
  double u_para = d_para * fabs(frontdetector->GetElementAngleCos());

  // TODO A difference in x coordinate between the two chambers is ignored.
  // This might become relevant if misalignment needs to be included.  The
  // helper class uv2xy could be used here.
  if (delta_x > 0.1)
    QwWarning << "[TreeMatch::MatchR3] Horizontal shifts between VDC planes are ignored"
              << QwLog::endl;


  // For the good tree lines in the front and back VDC planes, we first need
  // to set the 'z' coordinate in the wire direction.  The 'z' position for
  // VDC planes is the coordinate in the wire plane.  By definition, the
  // middle wire (141) has a 'z' position of zero.

  // Loop over the tree lines in the front VDC plane to set the 'z' position.
  int numflines = 0;
  for (QwTrackingTreeLine* frontline = frontlist; frontline;
       frontline = frontline->next, numflines++) {
    // Skip the the void tree lines
    if (frontline->IsVoid()) continue;
    // Loop over all hits of the valid tree lines
    for (int hit = 0; hit < frontline->fNumHits; hit++) {
      double zpos = (frontline->hits[hit]->GetElement() - 141) * wirespacing_f;
      frontline->hits[hit]->SetZPosition(zpos);
    }
  }
  // Loop over the tree lines in the back VDC plane to set the 'z' position
  int numblines = 0;
  for (QwTrackingTreeLine* backline = backlist; backline;
       backline = backline->next, numblines++) {
    // Skip the the void tree lines
    if (backline->IsVoid()) continue;
    // Loop over all hits of the valid tree lines
    for (int hit = 0; hit < backline->fNumHits; hit++) {
      double zpos = (backline->hits[hit]->GetElement() - 141) * wirespacing_b;
      backline->hits[hit]->SetZPosition(zpos);
    }
  }


  //###############################
  // Find matching track segments #
  //###############################
  int fmatches[numflines]; // matches indexed by front tree lines
  int bmatches[numblines]; // matches indexed by back tree lines

  // Initialize the array of best matches for all back plane tree lines
  double bestmatches[numblines];
  for (int i = 0; i < numblines; i++) bestmatches[i] = 99;

  // Loop over the tree lines in the front VDC plane
  int ifront = 0;
  for (QwTrackingTreeLine* frontline = frontlist; frontline;
       frontline = frontline->next, ifront++) {

    // Skip void tree lines
    if (frontline->IsVoid()) continue; // Skip it if it's no good

    // Get the hit with smallest drift distance
    QwHit* fpos = frontline->GetBestWireHit();

    // No match found yet
    fmatches[ifront] = -1;
    double bestmatch = 99;

    // Loop over the tree lines in the back VDC plane
    int iback = 0;
    for (QwTrackingTreeLine* backline = backlist; backline;
         backline = backline->next, iback++) {

      // Skip void tree lines
      if (backline->IsVoid()) continue;

      // Get the hit with smallest drift distance around the center of the
      // second wire plane.
      QwHit* bpos = backline->GetBestWireHit();

      // Get the positions of the best wire hit
      double x[2], y[2];
      y[0] = fpos->GetZPosition(); // Z position (i.e. wire direction)
      y[1] = bpos->GetZPosition();
      x[0] = fpos->GetPosition(); // X position (i.e. drift distance)
      x[1] = bpos->GetPosition();

      // Slope between the front and back plane central hits
      double slope = (u_para + y[1] - y[0]) / (d_perp + x[1] - x[0]);

      // Slope of the front and back tree line
      double fslope = wirespacing_f / frontline->fSlope;
      double bslope = wirespacing_b / backline->fSlope;

      // Check whether this is a good match
      if (fabs(fslope - slope) <= sloperes_f
       && fabs(bslope - slope) <= sloperes_b
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

  // Loop over the tree lines in the front VDC plane
  ifront = 0;
  for (QwTrackingTreeLine* frontline = frontlist; frontline;
       frontline = frontline->next, ifront++) {

    // Loop over the tree lines in the back VDC plane
    int iback = 0;
    for (QwTrackingTreeLine* backline = backlist; backline;
         backline = backline->next, iback++) {

      // If this front segment was matched to this back segment
      if (fmatches[ifront] == iback) {

        // Create a new tree line
        QwTrackingTreeLine* treeline = new QwTrackingTreeLine;
        // Set the detector identification
        treeline->SetRegion(frontline->GetRegion());
        treeline->SetPackage(frontline->GetPackage());
        treeline->SetDirection(frontline->GetDirection());

        QwHit *DetecHits[2*TLAYERS];
        for (int i = 0; i < 2*TLAYERS; i++) DetecHits[i] = 0;

        // Set the hits for front VDC
        int fronthits = frontline->fNumHits;
        for (int hit = 0; hit < fronthits; hit++) {
          treeline->hits[hit] = new QwHit(frontline->hits[hit]);
          DetecHits[hit] = treeline->hits[hit];
        }
        // Set the hits for back VDC
        int backhits = backline->fNumHits;
        for (int hit = 0; hit < backhits; hit++) {
          treeline->hits[hit+fronthits] = new QwHit(backline->hits[hit]);
          DetecHits[hit+fronthits] = treeline->hits[hit+fronthits];
          DetecHits[hit+fronthits]->fZPosition += u_para;
          DetecHits[hit+fronthits]->fPosition  += d_perp;
        }
        int nhits = fronthits + backhits;

        // Debug output
        if (fDebug) {
          for (int hit = 0; hit < nhits; hit++) {
            std::cout << DetecHits[hit]->GetZPosition() << " " << DetecHits[hit]->GetPosition() << endl;
          }
        }

        // Fit a line to the hits
        double slope, offset, chi, cov[3];
        TreeCombine->weight_lsq_r3 (&slope, &offset, cov, &chi, DetecHits, nhits, 0, -1, 2*TLAYERS);

        // Store the determined offset, slope, and chi^2 into the tree line
        treeline->SetOffset(offset);
        treeline->SetSlope(slope);
        treeline->SetChi(chi);

        treeline->fNumHits = nhits;
        treeline->fNumMiss = 2 * TLAYERS - nhits;

        // Set the tree line valid
        treeline->SetValid();

        // Store the final set of hits into this tree line
        for (int hit = 0; hit < fronthits; hit++) {
          treeline->usedhits[hit] = DetecHits[hit];
          treeline->AddHit(DetecHits[hit]);
        }
        for (int hit = fronthits; hit < fronthits + backhits; hit++) {
          treeline->usedhits[hit] = DetecHits[hit];
          treeline->AddHit(DetecHits[hit]);
        }

        treeline->next = treelinelist;
        treelinelist = treeline;
      }
    }
  }

  // Delete the tree combining object
  delete TreeCombine;

  // Return the list of matched tree lines (if no tree lines found, this is null)
  return treelinelist;
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
