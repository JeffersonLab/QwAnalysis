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
#include "QwTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"

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
 * (0, delta_para, delta_perp) assuming no lateral displacement.  The difference in the
 * u coordinate between the center of the chambers is then given by delta_u.
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
QwTreeLine *QwTrackingTreeMatch::MatchRegion3 (
	const QwTreeLine* frontlist,
	const QwTreeLine* backlist)
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
  QwTreeLine* treelinelist = 0;

  // Initialize the tree combine object (TODO can this be avoided?)
  QwTrackingTreeCombine *TreeCombine = new QwTrackingTreeCombine();


  /// Set up the geometry of the two wire planes: distances between them,
  /// relative orientation, etc.

  // Get detector identification
  const QwDetectorInfo* frontdetector = frontlist->GetDetectorInfo();
  const QwDetectorInfo* backdetector  = backlist->GetDetectorInfo();

  // Rotation of the detector planes in the xz plane around the y axis
  double cos_theta = frontdetector->GetDetectorPitchCos();
  double sin_theta = frontdetector->GetDetectorPitchSin();

  // Differences in position between the front and back detector planes
  // NOTE: these positions are still in the wrong coordinate system
  double delta_x = backdetector->GetXPosition() - frontdetector->GetXPosition();
  double delta_y = backdetector->GetYPosition() - frontdetector->GetYPosition();
  double delta_z = backdetector->GetZPosition() - frontdetector->GetZPosition();

  // Distance between the chamber centers perpendicular to the wire planes
  double delta_perp =   delta_z * cos_theta + delta_y * sin_theta;
	
  // Distance between the chamber centers parallel to the wire planes
  // 0.5 is tangent of wire angle, now includes x offset as well
  double delta_para = - delta_z * sin_theta + delta_y * cos_theta + 0.5 * delta_x;

  // Parallel distance between the chamber centers in u or v coordinates
  // NOTE: fabs because cos < 0 for v planes in one octant !@#$%
  double delta_u = delta_para * fabs(frontdetector->GetElementAngleCos());

  // NOTE:pkg1 and pkg2 need different solution
 
  // For the good tree lines in the front and back VDC planes, we first need
  // to set the 'z' coordinate in the wire direction.  The 'z' position for
  // VDC planes is the coordinate in the wire plane.  By definition, the
  // middle wire (141) has a 'z' position of zero.

  // Loop over the tree lines in the front VDC plane to set the wire position.
  int numflines = 0;
  for (const QwTreeLine* frontline = frontlist; frontline;
       frontline = frontline->next, numflines++) {
    // Skip the the void tree lines
    if (frontline->IsVoid()) continue;
    // Loop over all hits of the valid tree lines
    for (int hit = 0; hit < frontline->fNumHits; hit++) {
      int element = frontline->fHits[hit]->GetElement();
      double wire = frontdetector->GetElementCoordinate(element);
      frontline->fHits[hit]->SetWirePosition(wire);
      if (fDebug) QwMessage << "wire " << element << ": "
                  << frontline->fHits[hit]->GetWirePosition()  << " "
                  << frontline->fHits[hit]->GetDriftPosition() << QwLog::endl;
    }
  }
  // Loop over the tree lines in the back VDC plane to set the wire position
  int numblines = 0;
  for (const QwTreeLine* backline = backlist; backline;
       backline = backline->next, numblines++) {
    // Skip the the void tree lines
    if (backline->IsVoid()) continue;
    // Loop over all hits of the valid tree lines
    for (int hit = 0; hit < backline->fNumHits; hit++) {
      int element = backline->fHits[hit]->GetElement();
      double wire = backdetector->GetElementCoordinate(element);
      backline->fHits[hit]->SetWirePosition(wire);
      if (fDebug) QwMessage << "wire " << element << ": "
                  << backline->fHits[hit]->GetWirePosition() + delta_u << " "
                  << backline->fHits[hit]->GetDriftPosition() + delta_perp << QwLog::endl;
    }
  }


  //###############################
  // Find matching track segments #
  //###############################
  int fmatches[numflines]; // matches indexed by front tree lines
  int bmatches[numblines]; // matches indexed by back tree lines
  int reverse[numflines*numblines];  // to specify if ifront and ifback combination should flip the drift distance sign


  // Initialize the array of best matches for all back plane tree lines
  double bestmatches[numblines];
  for (int i = 0; i < numblines; i++) bestmatches[i] = 99;

  // Loop over the tree lines in the front VDC plane
  int ifront = 0;
  for (const QwTreeLine* frontline = frontlist; frontline;
       frontline = frontline->next, ifront++) {

    // Initialization of fmatches
    fmatches[ifront] = -1;

    // Skip void tree lines
    if (frontline->IsVoid()) continue;

    // Get the hit with smallest drift distance
    QwHit* fronthit = frontline->GetBestWireHit();

    // No match found yet
    double bestmatch = 99;

    // Loop over the tree lines in the back VDC plane
    int iback = 0;
    for (const QwTreeLine* backline = backlist; backline;
         backline = backline->next, iback++) {

      // Skip void tree lines
      if (backline->IsVoid()) continue;

      // Get the hit with smallest drift distance around the center of the
      // second wire plane.
      QwHit* backhit = backline->GetBestWireHit();

      // Get the positions of the best wire hit
      double u[2], perp[2];
      u[0] = fronthit->GetWirePosition(); // distance parallel to plane
      u[1] = backhit->GetWirePosition();  // i.e. wire direction
      perp[0] = fronthit->GetDriftPosition(); // distance perpendicular to plane
      perp[1] = backhit->GetDriftPosition();  // i.e. drift distance

      // Slope between the front and back plane central hits
      // NOTE: this is the slope wrt the normal to the plane

      double slope=0;
      // NOTE: pkg1 needs to minus delta_u while pkg2 needs to add delta_u due to the location of the first wire
      if(backline->GetPackage()==1) 
      slope = (-1*delta_u + u[1] - u[0]) / (delta_perp + perp[1] - perp[0]);
      else
      slope = (delta_u + u[1] - u[0]) / (delta_perp + perp[1] - perp[0]);

      // Wire spacing and slope matching parameters for front and back planes
      double wirespacing_f = frontdetector->GetElementSpacing();
      double wirespacing_b = backdetector->GetElementSpacing();
      double sloperes_f = frontdetector->GetSlopeMatching();
      double sloperes_b = backdetector->GetSlopeMatching();


      // Slope of the front and back tree line wrt the normal to the plane
      // NOTE: the slopes of the tree line are wrt the plane
      double fslope = wirespacing_f / frontline->fSlope;
      double bslope = wirespacing_b / backline->fSlope;

      // NOTE:Do a preliminary slope check to align two treelines into one
      if (fabs(fslope - slope) <= sloperes_f
       && fabs(bslope - slope) <= sloperes_b
       && fabs(fslope - slope) + fabs(bslope - slope) < bestmatch){
	// if ok, do nothing at all
	reverse[ifront*numblines+iback]=1;
	}
	else{
	// NOTE: if not, we need to flip the sign to see if it works
	fslope*=-1;
	bslope*=-1;
	reverse[ifront*numblines+iback]=-1;
	}
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
  for (const QwTreeLine* frontline = frontlist; frontline;
       frontline = frontline->next, ifront++) {

    // Loop over the tree lines in the back VDC plane
    int iback = 0;
    for (const QwTreeLine* backline = backlist; backline;
         backline = backline->next, iback++) {

      // If this front segment was matched to this back segment
      if (fmatches[ifront] == iback) {

        // Create a new tree line
        QwTreeLine* treeline = new QwTreeLine;
        // Set the detector identification
        treeline->SetRegion(frontline->GetRegion());
        treeline->SetPackage(frontline->GetPackage());
        treeline->SetDirection(frontline->GetDirection());
	treeline->SetOctant(frontline->GetOctant());

        QwHit *DetecHits[2*MAX_LAYERS];
        for (int i = 0; i < 2*MAX_LAYERS; i++) DetecHits[i] = 0;

        // NOTE:Set the hits for front VDC, the reverse will determine how we should align the treelines
        int fronthits = frontline->fNumHits;
        for (int hit = 0; hit < fronthits; hit++) {
          treeline->AddHit(frontline->fHits[hit]);
          treeline->fHits[hit] = treeline->GetListOfHits().back();
          DetecHits[hit] = treeline->GetListOfHits().back();
          DetecHits[hit]->fDriftPosition *= reverse[ifront*numblines+iback];
        }
        // Set the hits for back VDC
        int backhits = backline->fNumHits;
        for (int hit = 0; hit < backhits; hit++) {
          treeline->AddHit(backline->fHits[hit]);
          treeline->fHits[hit+fronthits] = treeline->GetListOfHits().back();
          DetecHits[hit+fronthits] = treeline->GetListOfHits().back();

          if (backline->GetPackage() == 1)
            DetecHits[hit+fronthits]->fWirePosition -= delta_u;
          else
            DetecHits[hit+fronthits]->fWirePosition += delta_u;

          DetecHits[hit+fronthits]->fDriftPosition *= reverse[ifront*numblines+iback];
          DetecHits[hit+fronthits]->fDriftPosition += delta_perp;
        }
        int nhits = fronthits + backhits;

// 	std::cout << "wire position: " << std::endl;
// 	for(int i=0;i<nhits;i++)
// 		std::cout << DetecHits[i]->fWirePosition << "," << std::endl;
// 
// 	std::cout << "distance: " << std::endl;
// 	for(int i=0;i<nhits;i++)
// 		std::cout << DetecHits[i]->fDriftPosition << "," << std::endl;

        // Fit a line to the hits
        double slope, offset, chi, cov[3];
        TreeCombine->r3_TreelineFit (slope, offset, cov, chi, DetecHits, nhits, 0, -1);

        // Store the determined offset, slope, and chi^2 into the tree line
        treeline->SetOffset(offset);
        treeline->SetSlope(slope);
        treeline->SetChi(chi);

        treeline->CalculateAverageResidual();

        treeline->fNumHits = nhits;
        treeline->fNumMiss = 2 * MAX_LAYERS - nhits;

        // Set the tree line valid
        treeline->SetValid();

        // TODO remove this along with fHits, this is just so the destructor of treeline is happy
        for (int hit = 0; hit < fronthits + backhits; hit++) {
          treeline->fHits[hit] = new QwHit(treeline->fHits[hit]);
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
