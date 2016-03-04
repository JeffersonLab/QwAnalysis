//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

 \class QwTrackingTreeCombine

 \brief  Combines track segments and performs line fitting.

 \author Burnham Stokes
 \date   7/30/08

    Treecombine performs many of the tasks involved with matching hits to track
    segments and combining track segments into full tracks with lab coordinates.

 *//*-------------------------------------------------------------------------*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwTrackingTreeCombine.h"

// Standard C and C++ headers
#include <fstream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <sys/time.h>
#include <algorithm>

// Qweak headers
#include "QwLog.h"
#include "QwOptions.h"
#include "QwHitContainer.h"
#include "QwDetectorInfo.h"
#include "QwTrackingTree.h"
#include "QwTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwEvent.h"

// Qweak headers
#include "matrix.h"
#include "uv2xy.h"

// Qweak tree object headers
#include "QwTrackingTreeRegion.h"

// Other Qweak modules
#include "QwTrackingTreeSort.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeCombine::QwTrackingTreeCombine()
: fMaxRoad(0.0),fMaxXRoad(0.0)
{
  fDebug = 0; // Debug level

  // Get maximum number of HDC planes
  fMaxMissedPlanes = gQwOptions.GetValue<int>("QwTracking.R2.MaxMissedPlanes");
  fMaxMissedWires = gQwOptions.GetValue<int>("QwTracking.R3.MaxMissedWires");

  // Attempt to drop hit with worst residual
  fDropWorstHit = gQwOptions.GetValue<bool>("QwTracking.R2.DropWorstHit");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeCombine::~QwTrackingTreeCombine ()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Determines whether the left or right drift distance assignment is closer to
 * the supplied coordinate for hits in region 3.
 *
 * @param track_position Position of the track through this detector plane
 * @param hit Hit for which we want to resolve the left/right ambiguity
 * @return Copy of the hit with the selected position
 */
QwHit* QwTrackingTreeCombine::SelectLeftRightHit (
    double track_position,
    QwHit* hit)
{
  // Declarations
  double position, distance, best_position,best_distance;

  // First option of left/right ambiguity
  position = + hit->GetDriftDistance();
  distance = fabs ( track_position - position );
  // This is best position for now.
  best_position = position;
  best_distance = distance;

  // Second option of left/right ambiguity
  position = - hit->GetDriftDistance();
  distance = fabs ( track_position - position );
  // Is this a better position?
  if ( distance < best_distance )
    best_position = position;

  // Write the best hit (actually just uses the old structure)
  QwHit* besthit = new QwHit ( *hit );
  besthit->fDriftPosition = best_position;
  return besthit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Determines whether the left or right drift distance assignment is closer to
 * the supplied track coordinate for hits in region 2.
 *
 * @param xresult Position of the track crossing through this plane
 * @param resolution Resolution in this detector plane
 * @param hitlist Detected hit list (left/right ambiguity not resolved yet)
 * @param ha Returned hit list (left/right ambiguity is now resolved)
 * @param Dx Detector offset in the wire plane direction (default value is zero)
 * @return Number of good hits
 */
int QwTrackingTreeCombine::SelectLeftRightHit (
    double *xresult,
    double resolution,
    QwHitContainer *hitlist,
    QwHit **ha,
    double Dx )
{
  //###############
  // DECLARATIONS #
  //###############
  int ngood = 0; // number of good hits that have been found already
  double track_position = *xresult; // x is the x coordinate of the track in the plane
  double odist = 0.0;
  double minimum = resolution; // will keep track of minimum distance
  double wirespacing = hitlist->begin()->GetDetectorInfo()->GetElementSpacing();

  // Loop over the hit list
  QwHitContainer::iterator end=hitlist->end();
  for ( QwHitContainer::iterator hit = hitlist->begin();
      hit != end; ++hit )
  {
    if(hit->GetHitNumber()!=0 || hit->GetDriftDistance()<0) continue;
    // Consider the two options due to the left/right ambiguity
    for ( int j = 0; j < 2; ++j )
    {
      double hit_position =
          j ? ( hit->GetElement()-0.5) * wirespacing +
              hit->GetDriftDistance() + Dx
              : (hit->GetElement()-0.5)* wirespacing-hit->GetDriftDistance()+Dx;
      double signed_distance = track_position - hit_position;
      double distance = fabs ( signed_distance );

      // Only consider this hit if it is not too far away from the track position,
      // i.e. within the drift distance resolution
      if ( distance < resolution )
      {
        // Save only a maximum number of hits per track crossing through the plane
        if ( ngood < MAXHITPERLINE )
        {

          // Duplicate this hit for calibration changes
          ha[ngood] = new QwHit;
          // Copy the hit, but reset the pointers
          *ha[ngood] = *hit;
          // Store this position
          ha[ngood]->fDriftPosition = hit_position;

          if ( distance < minimum )
          {
            //*xresult = hit_position;
            minimum = distance;
          }
          ++ngood;


          // If we have already the maximum allowed number of hits,
          // then save this hit only if it is better than the others
        }
        else
        {
          // Loop over the hits that we already saved
          for ( int i = 0; i < ngood; ++i )
          {
            distance = track_position - ha[i]->GetDriftPosition();
            if ( odist < 0 )
              odist = -odist;
            if ( distance < odist )
            {
              *ha[i] = *hit;
              ha[i]->fDriftPosition = hit_position;
              break;
            }
          }
        }
      } // end of distance cut

    } // end of left/right ambiguity

  } // end of loop over hit list
  return ngood;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* --------------------------------------------------------------------------
 * creates permutations of hits
 * i   : a number from 0 to mul-1 (number of permutation)
 * mul : the number of permutations
 * l   : the length of r[l], bx[l][MAXHITPERLINE], xa[l]
 * r   : array of multiplicity in detectors
 * hx  : hits filled: ha[l][r[]]
 * ha  : array to write permutated hits in
 * ------------------------------------------------------------------------- */
void QwTrackingTreeCombine::SelectPermutationOfHits (
    int i,
    int mul,
    int l,
    int *r,
    QwHit *hx[DLAYERS][MAXHITPERLINE],
    QwHit **ha )
{
  int s;

  if ( i == 0 )
  {
    for ( int j = 0; j < l; j++ )
    {
      ha[j] = hx[j][0];
    }
  }
  else
  {
    for ( int j = 0; j < l; j++ )
    {
      if ( r[j] == 1 )
        s = 0;
      else
      {
        s = i % r[j];
        i /= r[j];
      }
      ha[j] = hx[j][s];
    }
  }
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* ----------------------------------------------------------------------
 * weight_lsq
 * we solve the linear equation with
 *
 * x = (x_offset, x_slope)
 * y = (hit positions)
 * A = - ( (1, zpos1), (1, zpos2), ... )
 * C = cov(hitpositions)
 * G = C^-1
 *
 * (A^T * G * A) x = -A^T G y
 *
 * to do a least square fit weighted with the hit resolutions and
 * get the covariance matrix for position and slope.
 */
// TODO Why is this similar to weight_lsq_r3?  can't this be one function?
void QwTrackingTreeCombine::r2_TreelineFit (
    double& slope,	//!< (returns) slope of fitted track
    double& offset,	//!< (returns) offset of fitted track
    double  cov[3],	//!< (returns) covariance matrix of slope and offset
    double& chi,	//!< (returns) chi^2 of the fit
    QwHit **hits,	//!< list of hits in every plane
    int n )		//!< number of planes with hits
{
  // Declaration of matrices
  double A[n][2], G[n][n], AtGA[2][2];
  double AtGy[2], y[n], x[2];
  // Initialize the matrices and vectors
  for ( int i = 0; i < n; ++i )
    A[i][0] = -1.0;

  // Set the hit values
  for ( int i = 0; i < n; ++i )
  {
    A[i][1] = - hits[i]->GetDetectorInfo()->GetZPosition();
    y[i]    = - hits[i]->GetDriftPosition();
    double resolution = hits[i]->GetDetectorInfo()->GetSpatialResolution();
    G[i][i] = 1.0 / ( resolution * resolution );
  }

  // Calculate right hand side: A^T G y
  for ( int k = 0; k < 2; ++k )
  {
    double sum = 0.0;
    for ( int i = 0; i < n; i++ )
      sum += ( A[i][k] ) * G[i][i] * y[i];
    AtGy[k] = sum;
  }

  // Calculate the left hand side: A^T * G * A
  for ( int j = 0; j < 2; ++j )
  {
    for ( int k = 0; k < 2; ++k )
    {
      double sum = 0.0;
      for ( int i = 0; i < n; ++i )
        sum += ( A[i][j] ) * G[i][i] * A[i][k];
      AtGA[j][k] = sum;
    }
  }

  // Calculate inverse of A^T * G * A
  double det = ( AtGA[0][0] * AtGA[1][1] - AtGA[1][0] * AtGA[0][1] );
  double tmp = AtGA[0][0];
  AtGA[0][0] = AtGA[1][1] / det;
  AtGA[1][1] = tmp / det;
  AtGA[0][1] /= -det;
  AtGA[1][0] /= -det;

  // Solve equation: x = (A^T * G * A)^-1 * (A^T * G * y)
  for ( int k = 0; k < 2; ++k )
    x[k] = AtGA[k][0] * AtGy[0] + AtGA[k][1] * AtGy[1];
  slope  = x[1];
  offset = x[0];
  // Calculate covariance
  cov[0]  = AtGA[0][0];
  cov[1]  = AtGA[0][1];
  cov[2]  = AtGA[1][1];

  // Calculate chi^2
  double sum = 0.0;
  double firstpass=0.0;
  for ( int i = 0; i < n; ++i )
  {

    hits[i]->SetTreeLinePosition(slope * hits[i]->GetDetectorInfo()->GetZPosition() + offset);
    hits[i]->CalculateTreeLineResidual();
    double residual = hits[i]->GetTreeLineResidual();
    firstpass+=fabs(residual);
    sum += G[i][i] * residual * residual;
  }

  // Normalize chi^2
  chi = sqrt ( sum / (n-2) );

  firstpass/=n;

  //the following procedure is used to optimize the treeline by trying to drop one bad hit to see if a significant
  //improvement of average residual happened. If yes, save it to replace the old one. Now, this optimization is temporarily
  // turned off.
  double bad_=0.05;
  // if all planes got hit or average residual is bigger than 400 microns, then try to minimize the average residual by
  // dropping one hit from the line
  if(n==5 && firstpass>bad_){

    //std::cerr << "happend!" << std::endl;
    for(int drop=0;drop<4;++drop){
      for ( int k = 0; k < 2; ++k )
      {
        double sum = 0.0;
        for ( int i = 0; i < n; ++i ){
          if(i!=drop)
            sum += ( A[i][k] ) * G[i][i] * y[i];
        }
        AtGy[k] = sum;
      }

      // Calculate the left hand side: A^T * G * A
      for ( int j = 0; j < 2; ++j )
      {
        for ( int k = 0; k < 2; ++k )
        {
          double sum = 0.0;
          for ( int i = 0; i < n; i++ ){
            if(i!=drop)
              sum += ( A[i][j] ) * G[i][i] * A[i][k];
          }
          AtGA[j][k] = sum;
        }
      }

      // Calculate inverse of A^T * G * A
      double det = ( AtGA[0][0] * AtGA[1][1] - AtGA[1][0] * AtGA[0][1] );
      double tmp = AtGA[0][0];
      AtGA[0][0] = AtGA[1][1] / det;
      AtGA[1][1] = tmp / det;
      AtGA[0][1] /= -det;
      AtGA[1][0] /= -det;

      // Solve equation: x = (A^T * G * A)^-1 * (A^T * G * y)
      for ( int k = 0; k < 2; ++k )
        x[k] = AtGA[k][0] * AtGy[0] + AtGA[k][1] * AtGy[1];
      slope  = x[1];
      offset = x[0];
      // Calculate covariance
      cov[0]  = AtGA[0][0];
      cov[1]  = AtGA[0][1];
      cov[2]  = AtGA[1][1];

      // Calculate chi^2
      double sum=0.0;

      for ( int i = 0; i < n; ++i )
      {
        if(i!=drop){
          hits[i]->SetTreeLinePosition(slope * hits[i]->GetDetectorInfo()->GetZPosition() + offset);
          hits[i]->CalculateTreeLineResidual();
          double residual=hits[i]->GetTreeLineResidual();
          sum += G[i][i] * residual * residual;
        }
      }

      double chi_second=sqrt(sum/(n-3));
      if(sqrt(chi_second<chi/10)){
        chi=chi_second;
        hits[drop]->SetUsed(true);
        break;
      }
    } // the end of the drop loop
  } // if statement is over
  // NOTE I have no idea what the next line does, but it also works without this.
  // It is particularly strange that the corresponding chi_hashfind is never
  // used: what's the use of writing to a hash list if you never use it?
  // (wdconinc, July 14, 2009)
  //chi_hashinsert(hits, n, *slope, *offset, cov, *chi);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* ----------------------------------------------------------------------
 * We solve the linear equation with
 *
 *   x = (x_offset, x_slope)
 *   y = (hit positions)
 *   A = - ( (1, zpos1), (1, zpos2), ... )
 *   C = cov(hitpositions)
 *   G = C^-1
 *
 *   (A^T * G * A) x = -A^T G y
 *
 * to do a least square fit weighted with the hit resolutions,
 * and get the covariance matrix for position and slope.
 */

void QwTrackingTreeCombine::r3_TreelineFit (
    double& slope,
    double& offset,
    double  cov[3],
    double& chi,
    QwHit **hits,
    int n,
    double z1,
    int wire_offset )
{
  // Return if n is zero, no hits
  if (n == 0) return;

  // Declaration of matrices
  double A[n][2], G[n][n], AtGA[2][2];
  double AtGy[2], y[n], x[2];

  // Initialize the matrices and vectors
  for ( int i = 0; i < n; i++ )
    A[i][0] = -1.0;
  //###########
  // Set Hits #
  //###########
  for ( int i = 0; i < n; i++ )
  {
    if ( wire_offset == -1 )
    {
      A[i][1] = -hits[i]->GetWirePosition(); //used by matchR3 for plane 0
      y[i]    = -hits[i]->GetDriftPosition();
    }
    else
    {
      //A[i][1] = - ( i + wire_offset ); //used by Tl MatchHits
      A[i][1]= -hits[i]->GetElement();
      y[i]    = -hits[i]->GetDriftPosition();
    }
    double resolution = hits[i]->GetDetectorInfo()->GetSpatialResolution();
    G[i][i] = 1.0 / ( resolution * resolution );
  }

  // Calculate right hand side: -A^T G y
  for ( int k = 0; k < 2; k++ )
  {
    double sum = 0.0;
    for ( int i = 0; i < n; i++ )
      sum += ( A[i][k] ) * G[i][i] * y[i];
    AtGy[k] = sum;
  }
  // Calculate the left hand side: A^T * G * A
  for ( int j = 0; j < 2; j++ )
  {
    for ( int k = 0; k < 2; k++ )
    {
      double sum = 0.0;
      for ( int i = 0; i < n; i++ )
        sum += ( A[i][j] ) * G[i][i] * A[i][k];
      AtGA[j][k] = sum;
    }
  }

  // Calculate inverse of A^T * G * A
  double det = ( AtGA[0][0] * AtGA[1][1] - AtGA[1][0] * AtGA[0][1] );
  double tmp = AtGA[0][0];
  AtGA[0][0] = AtGA[1][1] / det;
  AtGA[1][1] = tmp / det;
  AtGA[0][1] /= -det;
  AtGA[1][0] /= -det;

  // Solve equation: x = (A^T * G * A)^-1 * (A^T * G * y)
  for ( int k = 0; k < 2; k++ )
    x[k] = AtGA[k][0] * AtGy[0] + AtGA[k][1] * AtGy[1];
  slope  = x[1];
  offset = x[0];
  // Calculate covariance
  cov[0]  = AtGA[0][0];
  cov[1]  = AtGA[0][1];
  cov[2]  = AtGA[1][1];

  // Calculate chi^2
  double sum = 0.0;
  if ( wire_offset == -1 )
  {
    for ( int i = 0; i < n; i++ )
    {
      hits[i]->SetTreeLinePosition ( slope * ( hits[i]->GetWirePosition() - z1 ) + offset );
      hits[i]->CalculateTreeLineResidual();
      double residual = hits[i]->GetTreeLineResidual();
      sum += G[i][i] * residual * residual;
    }
  }
  else
  {
    for ( int i = 0; i < n; i++ )
    {
      // 			hits[i]->SetWirePosition ( i + wire_offset ); // TODO element spacing
      //hits[i]->SetTrackPosition ( slope * ( i + wire_offset ) + offset );
      hits[i]->SetWirePosition (hits[i]->GetElement());
      hits[i]->SetTreeLinePosition ( slope * hits[i]->GetElement() + offset );
      hits[i]->CalculateTreeLineResidual();
      double residual = hits[i]->GetTreeLineResidual();
      sum += G[i][i] * residual * residual;
    }
  }
  // Normalize chi^2
  chi = sqrt ( sum / ( n-2 ) );
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeCombine::contains ( double var, QwHit **arr, int len )
{
  // TODO (wdc) This is unsafe due to double comparisons.
  // In light of calibration effects, this could fail.
  cerr << "[QwTrackingTreeCombine::contains] Warning: double == double is unsafe." << endl;
  for ( int i = 0; i < len ; i++ )
  {
    if ( var == arr[i]->GetDriftPosition() )
      return 1;
  }
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* ------------------------------
   searches for the closest hit to
   to *xresult and writes it
   to *xresult. In addition the
   hitarray is searched for hits
   in the detector detect and
   inbetween *xresult - resolution
   and *xresult + resolution. All
   the possible hits are stored
   in *ha

   Size of the arrays must be
   MAXHITPERLINE
   ------------------------------ */

int QwTrackingTreeCombine::selectx (
    double *xresult,
    double resolution,
    QwHit** hitarray,
    QwHit** ha )
{
  int good = 0;
  double x = *xresult;
  double minimum = resolution;


  for (int num = MAXHITPERLINE * DLAYERS; num-- && *hitarray; hitarray++ )
  {
    QwHit* h = *hitarray;

    // There used to be a functionality with the detector info here.  Removed (wdc)
    //     if( !h->GetDetectorInfo() ||
    // 	(h->GetDetectorInfo()->GetID() != detec->ID)) {
    //       continue;
    //     }

    double position = h->GetDriftPosition();

    if ( ! contains ( position, ha, good ) )
    {
      double distance   = x - position;
      ha [good]   = h;
      if ( fabs ( distance ) < minimum )
      {
        *xresult = position;
        minimum = fabs ( distance );
      }
      good++;
      if ( good == MAXHITPERLINE )
        break;
    }
  }
  return good;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*------------------------------------------------------------------------*//*!

 This method checks whether a track with the given parameters is in the
 geometric acceptance of all defined detector planes in the specified
 region and package.  Geometric acceptance is calculated from the centers
 and widths in the geometry definition.

 Returns one if in the acceptance (or always for region 2), returns zero
 if not in the acceptance of at least one detector plane.

 *//*-------------------------------------------------------------------------*/
bool QwTrackingTreeCombine::InAcceptance (
    EQwDetectorPackage package,
    EQwRegionID region,
    double cx, double mx,
    double cy, double my )
{
  switch (region) {
    /* Region 2 */
    case kRegionID2: {
      return true;
      break;
    }

    /* Region 3 */
    case kRegionID3: {

      // TODO this is of course wrong for tilted planes... hence this
      // function is not used anywhere (and probably shouldn't exist).

      // Loop over all direction in this region
      for ( EQwDirectionID dir = kDirectionU; dir <= kDirectionV; dir++ ) {
        // Loop over all detector planes
        double z1 = 0.0;
        QwGeometry vdc(fGeometry.in(package).in(region).in(dir));
        for (size_t i = 0; i < vdc.size(); i++) {
          QwDetectorInfo* det = vdc.at(i);
          double z = det->GetZPosition();
          double x = cx + (z - z1) * mx;
          double y = cy + (z - z1) * my;
          if (! det->InAcceptance(x,y)) return false;
        }
      }
      break;
    }

    /* Others */
    default: {
      return true;
      break;
    }
  }

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*------------------------------------------------------------------------*//*!

 In TlCheckForX we search for the possible hit assignments for the treeline.
 While looping over the detector planes, we look at all registered hits and
 store 'good enough' hits in the goodHits array (indexed by plane number and
 hit number).  Both left and right hits can be stored in goodHits, and a
 maximum of MAXHITPERLINE hits per detector plane is enforced.

 Since only one hit per detector plane can be associated with the treeline,
 we determine the chi^2 for all possible hit permutations.  The hit combination
 with the best chi^2 is stored in the treeline structure.

 @param x1 position at first plane
 @param x2 position at last plane
 @param dx1 uncertainty at first plane
 @param dx2 uncertainty at last plane
 @param Dx offset between first and last planes
 @param z1 reference z coordinate
 @param Dz distance between first and last planes
 @param treeline treeline to operate on
 @param hitlist hit list
 @param dlayer
 @param tlayer
 @param iteration
 @param stay_tuned
 @param width
 *//*-------------------------------------------------------------------------*/
bool QwTrackingTreeCombine::TlCheckForX (
    double x1,  double x2,
    double dx1, double dx2,
    double Dx,
    double z1,
    double Dz,
    QwTreeLine *treeline,
    QwHitContainer *hitlist,
    int dlayer,
    int tlayer,
    int iteration,
    int stay_tuned,
    double width )
{
  //################
  // DECLARATIONS  #
  //################
  double startZ = 0.0;
  double endZ = 0.0;
  std::vector<int> patterns_copy;

  if ( DLAYERS < 4 ) cerr << "Error: DLAYERS must be >= 4 for region 2!" << endl;


  //	double minweight = 1e10;
  bool ret   = false;		/* return value (pessimistic)             */


  // Hit lists (TODO should be QwHitContainer?)
  QwHit *goodHits[DLAYERS][MAXHITPERLINE]; /* all hits in each detector plane */
  QwHit *usedHits[DLAYERS];		   /* one hit per detector plane */

  // Initialize to null
  for ( int i = 0; i < DLAYERS; ++i )
  {
    for ( int j = 0; j < MAXHITPERLINE; ++j )
    {
      goodHits[i][j] = 0;
    }
    patterns_copy.push_back(treeline->fMatchingPattern.at(i));
    usedHits[i] = 0;
  }

  //##################
  //DEFINE VARIABLES #
  //##################
  double orig_slope = (x2 - x1)   / Dz;	// track slope
  double orig_dmx   = (dx2 - dx1) / Dz;	// track resolution slope
  // (the resolution changes linearly between first and last detector plane)

  // Bin 'resolution'
  // TODO This should be retrieved from the QwHitPattern stored inside the tree line
  int levels = 0;
  switch ( treeline->GetRegion() )
  {
    case kRegionID2: levels = gQwOptions.GetValue<int> ( "QwTracking.R2.levels" ); break;
    case kRegionID3: levels = gQwOptions.GetValue<int> ( "QwTracking.R3.levels" ); break;
    default: break;
  }
  double resolution = width / ( 1 << ( levels - 1 ) );

  //####################################
  // LOOP OVER DETECTORS IN THE REGION #
  //####################################
  // uses BESTX & SELECTX     #
  //###########################

  // Loop over the detector planes of this package/region/direction
  int nPlanesWithHits = 0;	/* number of detector plane with good hits */
  int nHitsInPlane[DLAYERS];	/* number of good hits in a detector plane */

  int nPermutations = 1;	/* number of permutations of hit assignments */

  EQwRegionID region = treeline->GetRegion();
  EQwDetectorPackage package = treeline->GetPackage();
  EQwDirectionID dir = treeline->GetDirection();

  QwGeometry planes(fGeometry.in(package).in(region).in(dir));
  QwDetectorInfo* front_plane = 0;
  for (size_t plane = 0; plane < planes.size(); ++plane) {

    // Get subhitlist of hits in this detector
    QwHitContainer *hitsInPlane = hitlist->GetSubList_Plane(region, package, planes.at(plane)->GetPlane());

    // Coordinates of the track at this detector plane
    double thisZ = planes.at(plane)->GetZPosition();
    double thisX = orig_slope * (thisZ - z1) + x1;

    // Store the z coordinates of first detector plane
    if (! front_plane) {
      front_plane = planes.at(plane);
      startZ = thisZ;
    }
    // Store the z coordinates of last detector plane
    endZ = thisZ;

    // Skip inactive planes
    if (planes.at(plane)->IsInactive()) {
      delete hitsInPlane;
      continue;
    }

    // Skip empty planes
    if (hitsInPlane->size() == 0) {
      delete hitsInPlane;
      continue;
    }

    /* --- search this road width for hits --- */

    // resolution at this detector plane
    double resnow = orig_dmx * (thisZ - z1) + dx1;
    // unless we override this resolution
    // NOTE: what is this all about?
    if (! iteration
        && ! stay_tuned
        && plane+1 == planes.size() /* last plane in this set */
        && tlayer == dlayer)
      resnow -= resolution * (fMaxRoad - 1.0);

    // SelectLeftRightHit finds the hits which occur closest to the path
    // through the first and last detectors:
    // nHitsInPlane is the number of good hits at each detector layer
    if (! iteration) { /* first track finding process */


      double DX_pass=0.0;
      DX_pass = plane<2 ? 0.0:Dx;

      nHitsInPlane[nPlanesWithHits] =
          SelectLeftRightHit(&thisX, resnow, hitsInPlane, goodHits[nPlanesWithHits], DX_pass);
    }

    else { /* in iteration process (not used by TlTreeLineSort)*/
      nHitsInPlane[nPlanesWithHits] =
          selectx(&thisX, resnow, treeline->fHits, goodHits[nPlanesWithHits]);
    }



    /* If there are hits in this detector plane */
    if ( nHitsInPlane[nPlanesWithHits] ) {
      nPermutations *= nHitsInPlane[nPlanesWithHits];
      ++nPlanesWithHits;
    }

    // Delete the subhitlist
    delete hitsInPlane;
  }

  // Now the hits that have been found in the road are copied to treeline->hits
  // This is done by using the temporary pointer hitarray
  if ( ! iteration ) // return the hits found in SelectLeftRightHit()
  {
    QwHit **hitarray = treeline->fHits;
    for ( int planeWithHits = 0; planeWithHits < nPlanesWithHits; ++planeWithHits )
    {
      for ( int hitInPlane = 0; hitInPlane < nHitsInPlane[planeWithHits]; ++hitInPlane )
      {
        *hitarray = goodHits[planeWithHits][hitInPlane];
        //                                 copy hit to new list
        hitarray++;
      }
    }

    // Check number of hits that have been written and add terminating null
    if ( hitarray - treeline->fHits < DLAYERS*MAXHITPERLINE + 1 )
      *hitarray = 0;		/* add a terminating 0 for later selectx()*/
  }


  //#####################################################
  // DETERMINE THE BEST SET OF HITS FOR THE TREELINE    #
  //#####################################################
  // uses : MUL_DO     #
  //        WEIGHT_LSQ #
  //####################

  // check the validity of goodhits


  if ( nPlanesWithHits >= dlayer - fMaxMissedPlanes )
  {

    // Loop over all possible permutations
    int best_permutation = -1;
    double best_slope = 0.0, best_offset = 0.0, best_chi = 1e10;
    double best_cov[3] = {0.0, 0.0, 0.0};
    double best_trackpos[4]={0.0};
    for ( int permutation = 0; permutation < nPermutations; ++permutation )
    {
      // Select a permutations from goodHits[][] and store in usedHits[]
      SelectPermutationOfHits ( permutation, nPermutations, nPlanesWithHits, nHitsInPlane, goodHits, usedHits );
      // (returns usedHits)

      // Determine chi^2 of this set of hits
      double slope, offset; // fitted slope and offset
      double chi = 0.0; // chi^2 of the fit
      double cov[3] = {0.0, 0.0, 0.0}; // covariance matrix

      r2_TreelineFit ( slope, offset, cov, chi, usedHits, nPlanesWithHits );
      // (returns slope, offset, cov, chi)



      double stay_chi = 0.0;
      if ( stay_tuned )
      {
        double dh = ( offset +  slope * ( startZ - MAGNET_CENTER ) -
            ( x1 + orig_slope * ( startZ - z1 ) ) );
        stay_chi += dh * dh;
        dh = ( offset +  slope * ( endZ - MAGNET_CENTER ) -
            ( x1 + orig_slope * ( endZ - z1 ) ) );
        stay_chi += dh * dh;
      }
      /*
			if ( stay_chi + chi + dlayer - nPlanesWithHits < minweight )
			{
			  // has this been the best track so far? 
				minweight   = stay_chi + chi + dlayer - nPlanesWithHits;
				best_chi    = chi;
				best_slope  = slope;
				best_offset = offset;
				best_permutation  = permutation;
                                for(int plane=0;plane<nPlanesWithHits;plane++){
                                best_trackpos[plane]=usedHits[plane]->GetTrackPosition();
                         }
       */

      if ( chi < best_chi )
      {
        for(int plane=0;plane<nPlanesWithHits;++plane)
          best_trackpos[plane]=0.0;
        best_chi    = chi;
        best_slope  = slope;
        best_offset = offset;
        best_permutation  = permutation;
        for(int plane=0;plane<nPlanesWithHits;++plane){
          if(!usedHits[plane]->IsUsed())
            best_trackpos[plane]=usedHits[plane]->GetTreeLinePosition();
        }

        memcpy ( best_cov, cov, sizeof cov );
      }

      // if the hit is marked as used, we need to reset the used flags as unsed because
      // this hit will not be used due to the optimization
      for(int plane=0;plane<nPlanesWithHits;++plane)
        if(usedHits[plane]->IsUsed())
          usedHits[plane]->SetUsed(false);
    }// end of loop over all possible permutations



    treeline->fOffset  = best_offset;
    treeline->fSlope  = best_slope;
    treeline->fChi = best_chi;
    treeline->fNumHits = nPlanesWithHits;
    treeline->SetCov ( best_cov );


    // Select the best permutation (if it was found)
    if ( best_permutation != -1 )
    {
      SelectPermutationOfHits ( best_permutation, nPermutations,
          nPlanesWithHits, nHitsInPlane, goodHits, usedHits );

      for(int plane=0;plane<nPlanesWithHits;++plane){
        usedHits[plane]->SetTreeLinePosition(best_trackpos[plane]);
        usedHits[plane]->CalculateTreeLineResidual();
      }

      // Reset the used flags
      for ( int i = 0; i < MAXHITPERLINE * DLAYERS && treeline->fHits[i]; ++i )
        treeline->fHits[i]->SetUsed ( false );

      // Set the used flag on all hits that are used in this treeline
      for ( int plane = 0; plane < nPlanesWithHits; ++plane )
      {
        if(best_trackpos[plane]!=0.0){
          treeline->AddHit(usedHits[plane]);
          if ( usedHits[plane] )
            usedHits[plane]->SetUsed ( true );
        }
      }

      // Store the final set of hits for output
      for ( int plane = 0; plane < nPlanesWithHits; ++plane )
      {
        if ( usedHits[plane] ) treeline->AddHit ( usedHits[plane] );
      }
    }

    // Set the detector info pointer
    if (treeline->fHits[0])
      treeline->SetDetectorInfo ( treeline->fHits[0]->GetDetectorInfo() );

    // Check whether we found an optimal track
    if ( best_permutation == -1 )
      ret = false;	// acceptable hit assignment NOT found
    else
      ret = true;	// acceptable hit assignment found

  } // end of if for required number of planes with hits

  // Store the number of planes without hits; if no acceptable hit assignment
  // was found, set the treeline to void.
  if ( ret )
  {
    treeline->SetValid();
    treeline->fNumMiss = dlayer - nPlanesWithHits;
  }
  else
  {
    if ( fDebug )
    {
      QwDebug << *treeline << "... void because no best hit assignment." << QwLog::endl;
    }
    treeline->SetVoid();
    treeline->fNumMiss = dlayer;
  }

  // Return whether an acceptable hit assignment was found
  return ret;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* -------------------------------
This function replaces TlCheckForX for region 3.  It matches one of
the two wire hits to the hit in the pattern.  It then calculates a
chi_square for the matching of the hits to the pattern.  This may
be misleading due to the currently low resolution (8 bins) of
the pattern database.  It should however, be able to distinguish the
right/left ambiguity for each wire.

OUTPUT : the best hits are added to treeline's hit list and treeline's
	line parameters are set.
--------------------------------*/
int QwTrackingTreeCombine::TlMatchHits (
    double x1,	///< x coordinates at first wire (i.e. distances)
    double x2,	///< x coordinates at last wire (i.e. distances)
    double z1,	///< z coordinates of first wire (i.e. wire number
    double z2,	///< z coordinates of last wire (i.e. wire number)
    QwTreeLine *treeline,	///< determined treeline
    QwHitContainer *hitlist,	///< hit list
    int tlayers )		///< number of tree layers
{
  // This is how region 2 handles this:
  //
  //  // Hit lists
  //  QwHit *goodHits[tlayers][MAXHITPERLINE]; /* all hits in each wire */
  //  QwHit *usedHits[tlayers];		   /* one hit per wire */
  //  // Initialize to null
  //  for (int i = 0; i < DLAYERS; i++) {
  //    for (int j = 0; j < MAXHITPERLINE; j++) {
  //      goodHits[i][j] = 0;
  //    }
  //    usedHits[i] = 0;
  //  }

  // Calculate the geometrical parameters of this treeline
  // In this function:
  //   x = signed drift distance to the hit wire
  //   z = wire number 'coordinate' (starting at wire 0)
  double dx = x2 - x1;
  double dz = z2 - z1;
  double track_slope = dx / dz;
  // Distance to wire at (theoretical) wire 0
  double intercept = x1 - track_slope * z1;
  // Number of wires that can possibly be hit for this treeline
  int nWires = abs ( treeline->fR3LastWire - treeline->fR3FirstWire + 1 );


  // Loop over the hits with wires in this tree line
  int nHits = 0;


  int tl_first=treeline->fR3Offset+treeline->fR3FirstWire;
  int tl_last=treeline->fR3Offset+treeline->fR3LastWire;


  for ( QwHitContainer::iterator hit = hitlist->begin();
      hit != hitlist->end() && nHits < tlayers; hit++ )
  {
    // 		int begin=0,end=0;
    // 		double track_resolution=hit->GetDetectorInfo()->GetTrackResolution();
    // 		double halfwidth=1.52;
    int wire=hit->GetElement();

    if ( wire < tl_first || wire > tl_last){
      continue;
    }
    if(hit->IsUsed()==false)
      continue;
    // 		if ( wire < (tl_first - fMaxMissedWires+2)
    // 		|| wire > (tl_last + fMaxMissedWires-2)
    // 		        || hit->GetHitNumber() != 0 )
    // 			continue;
    //
    // 		if(wire < tl_first){
    // 		 begin=(halfwidth-hit->GetDriftDistance()-track_resolution)/halfwidth*4;
    // 		 if(begin > treeline->a_beg) continue;
    // 		}
    // 		else if( wire > tl_last){
    // 		 end=(halfwidth+hit->GetDriftDistance()+track_resolution)/halfwidth*4;
    // 		 if(end < treeline->b_end) continue;
    // 		}

    // Calculate the wire number and associated z coordinate
    double thisZ = ( double ) hit->GetElement();
    // Calculate the track position at this wire
    double thisX = track_slope * thisZ + intercept;

    // Determine the best hit assignment for this position
    QwHit* goodhit = SelectLeftRightHit ( thisX, & ( *hit ));

    //############################
    //RETURN HITS FOUND IN BESTX #
    //############################

    goodhit->SetUsed ( true );
    treeline->fHits[nHits] = new QwHit(goodhit);
    treeline->AddHit (goodhit);

    // Delete the hit from SelectLeftRightHit again so as to not leak memory
    delete goodhit;

    nHits++;
  }

  // Warn when the number of wires between first and last is different from the
  // number of hits found (missing wires or wires wit multiple hits)
  //    if ( nHits != nWires )
  //            QwWarning << "Expected " << nWires << " consecutive wires to be hit, " << "but found " << nHits << " hits in plane" << treeline->GetPlane() <<  QwLog::endl;

  // Return if no hits were found
  //if (nHits == 0) return 0;

  //######################
  //CALCULATE CHI SQUARE #
  //######################
  double chi = 0.0;
  double slope = 0.0, offset = 0.0;
  double cov[3] = {0.0, 0.0, 0.0};
  r3_TreelineFit ( slope, offset, cov, chi, treeline->fHits, nHits, z1, treeline->fR3Offset );
  //    (returns slope, offset, cov, chi)

  //################
  //SET PARAMATERS #
  //################

  treeline->fOffset  = offset;
  treeline->fSlope   = slope;     /* return track parameters: offset, slope, chi */
  treeline->fChi     = chi;
  treeline->fNumHits = nHits;
  treeline->SetCov ( cov );

  // Set the detector info pointer
  treeline->SetDetectorInfo ( treeline->fHits[0]->GetDetectorInfo() );

  int ret = 1;  //need to set up a check that the missing hits is not greater than 1.
  if ( ! ret )
  {
    treeline->SetVoid();
    treeline->fNumMiss = nWires - nHits;
  }
  else
  {
    QwDebug << "Treeline: voided because no best hit assignment" << QwLog::endl;
    QwDebug << "...well, actually this hasn't been implemented properly yet." << QwLog::endl;
    treeline->SetValid();
    treeline->fNumMiss = nWires - nHits;
  }
  return ret;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* ------------------------------
   Marks TreeLines close to
   other Lines as being good
   or bad in respect to their
   chi^2
   ------------------------------ */
void QwTrackingTreeCombine::TlTreeLineSort (
    QwTreeLine *treelinelist,
    QwHitContainer *hitlist,
    EQwDetectorPackage package,
    EQwRegionID region,
    EQwDirectionID dir,
    unsigned long bins,
    int tlayer,
    int dlayer,
    double width )
{
  switch (region) {
    /* Region 3 */
    case kRegionID3: {

      /* TODO In this section we should replace the treeline list with an
		   std::list<QwTreeLine>. */

      /* --------------------------------------------------
		   Calculate line parameters first
		   -------------------------------------------------- */

      // For each valid tree line
      for ( QwTreeLine *treeline = treelinelist;
          treeline && treeline->IsValid();
          treeline = treeline->next )
      {
        // First wire position
        double z1 = ( double ) ( treeline->fR3Offset + treeline->fR3FirstWire );
        // Last wire position
        double z2 = ( double ) ( treeline->fR3Offset + treeline->fR3LastWire );

        double d1 = 0, d2 = 0;
        int oct=0;
        for (QwHitContainer::iterator hit = hitlist->begin(); hit != hitlist->end(); hit++) {
          int wire = hit->GetElement();
          int row = wire-treeline->fR3Offset;
          if(oct==0 && hit->GetDetectorInfo()->GetPackage() == package){
            oct=hit->GetDetectorInfo()->GetOctant();
          }
          if (row < 0 || row > 7) continue;
          double distance = hit->GetDriftDistance();
          double track_resolution=hit->GetDetectorInfo()->GetTrackResolution();
          std::pair<double,double> range = treeline->CalculateDistance(row,width,bins,track_resolution);
          if (distance >= range.first && distance <= range.second) {
            hit->SetUsed(true);
          }
          if (wire == z1 && hit->IsUsed())
            d1 = distance;
          else if (wire == z2 && hit->IsUsed())
            d2 = distance;
        }

        d1 *= -1;


        // Bin width (bins in the direction of chamber thickness)
        // 			double dx = width / ( double ) bins;
        // Chamber thickness coordinates at first and last wire

        // 			double x1 = ( treeline->a_beg - ( double ) bins/2 ) * dx + dx/2;
        // 			double x2 = ( treeline->b_end - ( double ) bins/2 ) * dx + dx/2;

        treeline->SetOctant(oct);
        // Match the hits with the correct treeline
        TlMatchHits (
            d1, d2, z1, z2,
            treeline, hitlist,
            MAX_LAYERS );
      }
      break;
    }

    /* Region 2 */
    case kRegionID2: {

      // Get the front and back HDC plane to determine offset
      QwGeometry hdc(fGeometry.in(package).in(region).in(dir));
      QwDetectorInfo* front = hdc.front();
      QwDetectorInfo* back = hdc.back();
      //	    double r1 = front->GetYPosition();
      double z1 = front->GetZPosition();
      //	    double r2 = back->GetYPosition();
      double z2 = back->GetZPosition();
      //double rcos = back->GetElementAngleCos();
      assert(front->GetOctant()==back->GetOctant());

      // We are looping over detectors with the same direction
      //	    double Dr = (r2 - r1);           // difference in r position
      double Dz = (z2 - z1);           // difference in z position
      //double Dx = Dr * fabs (rcos);    // difference in x position
      double Dx=back->GetPlaneOffset()-front->GetPlaneOffset();
      //std::cout << "position difference first/last: " << Dx << " cm" << std::endl;
      QwDebug << "position difference first/last: " << Dx << " cm" << QwLog::endl;

      // Determine bin widths
      double dx = width;			// detector width
      //double dxh = 0.5 * dx;		// detector half-width
      dx /= ( double ) bins;		// width of each bin
      //double dxb = dxh / (double) bins;	// half-width of each bin

      /* --------------------------------------------------
                calculate line parameters first
            -------------------------------------------------- */

      // Loop over all valid tree lines
      for ( QwTreeLine *treeline = treelinelist;
          treeline; treeline = treeline->next )
      {
        if ( treeline->IsVoid() )
        {
          // No tree lines should be void yet
          QwWarning << "Warning: No tree lines should have been voided yet!" << QwLog::endl;
          continue;
        }

        treeline->SetOctant(front->GetOctant());
        // Debug output
        QwDebug << *treeline << QwLog::endl;

        // Calculate the track from the average of the bins that were hit

        double x1 = treeline->GetPositionFirst ( dx );
        double x2 = treeline->GetPositionLast ( dx ) + Dx;
        // Calculate the uncertainty on the track from the differences
        // and add a road width in units of bin widths

        double dx1 = treeline->GetResolutionFirst ( dx ) + dx * fMaxRoad;
        double dx2 = treeline->GetResolutionLast ( dx )  + dx * fMaxRoad;

        // Debug output
        QwDebug << "(x1,x2,z1,z2) = (" << x1 << ", " << x2 << ", " << z1 << ", " << z2 << "); " << QwLog::endl;
        QwDebug << "(dx1,dx2) = (" << dx1 << ", " << dx2 << ")" << QwLog::endl;

        // Check this tree line for hits and calculate chi^2
        TlCheckForX (
            x1, x2, dx1, dx2, Dx, z1, Dz,
            treeline, hitlist,
            tlayer, tlayer, 0, 0, width );

        // Debug output
        QwDebug << "Done processing " << *treeline << QwLog::endl;
      }
      break;
    }
    /* Other regions */
    default:
      QwWarning << "Warning: TreeCombine not implemented for region " << region << QwLog::endl;
  }
  /* End of region-specific parts */


  // Calculate the average residual
  for (QwTreeLine *treeline = treelinelist; treeline;
      treeline = treeline->next) {
    if (treeline->IsValid()) {
      treeline->CalculateAverageResidual();
    }
  }

  // Now search for identical tree lines in the list
  QwDebug << "Searching for identical treelines..." << QwLog::endl;
  for (QwTreeLine *treeline1 = treelinelist; treeline1;
      treeline1 = treeline1->next) {
    if (treeline1->IsValid()) {
      for (QwTreeLine *treeline2 = treeline1->next; treeline2;
          treeline2 = treeline2->next) {
        if (treeline2->IsValid()
            && treeline1->fOffset == treeline2->fOffset
            && treeline1->fSlope  == treeline2->fSlope) {
          QwDebug << *treeline2 << "... ";
          QwDebug << "void because it already exists." << QwLog::endl;
          treeline2->SetVoid();
        } // end of second treeline valid and equal to first treeline
      } // end of second loop over treelines
    } // end of first treeline valid
  } // end of first loop over treelines
  // Print the list of tree lines
  if (fDebug) {
    cout << "List of treelines:" << endl;
    if (treelinelist) treelinelist->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*!--------------------------------------------------------------------------*\
 Minuit2 Set-up
   The following is the code that the minuit2 minimizer calls in 
   r2_PartialTrackFit to optimize the fit parameters.  It applies
   rotation parameters defined in the geofile (qweak_HDC.<runrange>.geo) 
   found in the QwAnalysis/Tracking/src/ directory.  These rotation 
   parameters (defined by plane) allow the individual planes in Region 2
   to be rotated around the center of each chamber. **this eventually needs
   to be corrected to have both chambers in a package rotate around the same 
   point**      
 *//*-------------------------------------------------------------------------*/

class MyFCN : public ROOT::Minuit2::FCNBase { 

	public: 

		MyFCN(std::vector<QwHit*> fhits) : hits(fhits)  {}

		double operator() (const std::vector<double> & fit) const {
			double value = 0;
			double chi2 = 0;
			double resolution = hits[0]->GetDetectorInfo()->GetSpatialResolution();
			double normalization = 1/(resolution*resolution);

	
			// Calculate the metric matrix
			double dx_[12] = { 0.0 };
			double l[3] = { 0.0 };
			double h[3] = { 0.0 };
			for (int i = 0; i < hits.size(); ++i)
				dx_[hits[i]->GetPlane() - 1] = hits[i]->GetDetectorInfo()->GetPlaneOffset();

			for (int i = 0; i < 3; ++i)
			{
				h[i] = dx_[6 + i] == 0 ? dx_[9 + i] : dx_[6 + i];
				l[i] = dx_[i] == 0 ? dx_[3 + i] : dx_[i];
				dx_[i] = dx_[3 + i] = 0;
				dx_[6 + i] = dx_[9 + i] = h[i] - l[i];
			}

			for (int i=0; i<hits.size();i++)
			{

				double cosx = hits[i]->GetDetectorInfo()->GetDetectorRollCos();
				double sinx = hits[i]->GetDetectorInfo()->GetDetectorRollSin();
				double cosy = hits[i]->GetDetectorInfo()->GetDetectorPitchCos();
				double siny = hits[i]->GetDetectorInfo()->GetDetectorPitchSin();
				double rcos = hits[i]->GetDetectorInfo()->GetElementAngleCos();
				double rsin = hits[i]->GetDetectorInfo()->GetElementAngleSin();

				double dx = dx_[hits[i]->GetPlane() - 1];

				double wire_off = -0.5 * hits[i]->GetDetectorInfo()->GetElementSpacing()
				    + hits[i]->GetDetectorInfo()->GetElementOffset();
				double hit_pos = hits[i]->GetDriftPosition() + wire_off - dx;

				// Get end points of the hit wire
				double x0 = hit_pos/rsin;
				double y0 = hit_pos/rcos;
		
				double zRot = 1e6;	
				if(hits[i]->GetPackage()==1) zRot = -315.9965; 
				if(hits[i]->GetPackage()==2) zRot = -315.33;

				double z0 = hits[i]->GetDetectorInfo()->GetZPosition();
				double zD = z0 - zRot;
	
				double xR_1 = x0;
				double yR_1 = 0.0;
				double zR_1 = z0;
				double xR_2 = 0.0;
				double yR_2 = y0;
				double zR_2 = z0;
	
				xR_1 = cosy*x0 +zD*cosx*siny;
				yR_1 = zD*sinx;
				zR_1 = zD*cosx*cosy + zRot - siny*x0;
				
				xR_2 = zD*cosx*siny - siny*sinx*y0;
				yR_2 = cosx*y0 + zD*sinx;
				zR_2 = zD*cosx*cosy + zRot - sinx*cosy*y0;

				double xc = hits[i]->GetDetectorInfo()->GetXPosition();
				double yc = hits[i]->GetDetectorInfo()->GetYPosition();
				
				double z = (-(fit[1]-xc-zD*cosx*siny)*cosx*siny - 
					     (fit[3]-yc-zD*sinx)*sinx*(cosy*cosy-siny*siny) + 
					     (zRot + zD*cosx*cosy)*cosy*cosx)  / 
					     (fit[0]*cosx*siny + fit[2]*sinx*(cosy*cosy-siny*siny) + cosy*cosx);	
				
				double x = fit[1] + fit[0] * z;
				double y = fit[3] + fit[2] * z;
			
	
				x -= xc;
				y -= yc;
			
				double diffx = xR_2 - xR_1;
				double diffy = yR_2 - yR_1;
				double diffz = zR_2 - zR_1;

				double numx = -y*diffz + z*diffy + yR_1*zR_2 - yR_2*zR_1;
				double numy =  x*diffz - z*diffx - xR_1*zR_2 + xR_2*zR_1;
				double numz = -x*diffy + y*diffx + xR_1*yR_2 - xR_2*yR_1;
				
				double num = sqrt(numx*numx + numy*numy + numz*numz);

				double demx = xR_2 - xR_1;
				double demy = yR_2 - yR_1;
				double demz = zR_2 - zR_1;
				double dem = sqrt(demx*demx + demy*demy + demz*demz);
				
				double residual = num / dem;

				chi2 += normalization * residual * residual;
			}
			chi2 /= (hits.size() - 4);
			return chi2;


		} 


		double Up() const { return 1.; }



	private: 

		std::vector<QwHit*> hits;
};


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*!--------------------------------------------------------------------------*\

 r2_TrackFit()

    This function fits a set of hits in the HDC to a 3-D track.  This
    task is complicated by the unorthogonality of the u,v, and x wire
    planes.  To obtain the track, the metric matrix (defined below) is
    calculated using the projections of each hit into the lab
    coordinate system.  The matrix is inverted in order to solve the
    system of four equations for the four unknown track parameters
    (bx,mx,by,my).  Once the fit is calculated, the chisquare value
    is determined so that this track may be compared to other track
    candidates in order to select the best fit.

  metric matrix :  The metric matrix is defined by the chi-squared
		equation for unorthogonal coordinate systems.  In order to
		solve for the four track parameters, chi-square is
		differentiated with respect to the parameters, and set to
		zero.  The metric matrix is then derived from the four
		equations using the standard linear algebra technique for
		solving systems of equations.

 *//*-------------------------------------------------------------------------*/

int QwTrackingTreeCombine::r2_PartialTrackFit (
    const int num_hits,
    QwHit **hits,
    double *fit,
    double *cov,
    double &chi2,
    double* signedresidual,
    bool drop_worst_hit)
{
  //##################
  // Initializations #
  //##################

  // Initialize the matrices
  double A[4][4];
  double *Ap = &A[0][0];
  double B[4];
  for (int i = 0; i < 4; ++i)
  {
    B[i] = 0.0;
    for (int j = 0; j < 4; ++j)
      A[i][j] = 0.0;
  }

  // Find first hit on a X wire
  int hitx = 0; // will be number of X hits
  for (hitx = 0; hitx < num_hits; ++hitx)
    if (hits[hitx]->GetDetectorInfo()->GetElementDirection() == kDirectionX)
      break;
  // Find first hit on a U wire
  int hitu = 0; // will be number of U hits
  for (hitu = 0; hitu < num_hits; ++hitu)
    if (hits[hitu]->GetDetectorInfo()->GetElementDirection() == kDirectionU)
      break;
  // Find first hit on a V wire
  int hitv = 0; // will be number of V hits
  for (hitv = 0; hitv < num_hits; ++hitv)
    if (hits[hitv]->GetDetectorInfo()->GetElementDirection() == kDirectionV)
      break;

  // Did we have any direction without any hits? i.e. first hit is identical to number of hits
  if (hitx == num_hits || hitu == num_hits || hitv == num_hits) {
    QwWarning << "No hit on X plane found.  What I gotta do?" << QwLog::endl;
    return -1;
  }

  // Calculate the metric matrix
  double dx_[12] = { 0.0 };
  double l[3] = { 0.0 };
  double h[3] = { 0.0 };
  for (int i = 0; i < num_hits; ++i)
    dx_[hits[i]->GetPlane() - 1] = hits[i]->GetDetectorInfo()->GetPlaneOffset();

  //dx_ is used to save the Dx for every detector
  for (int i = 0; i < 3; ++i)
  {
    h[i] = dx_[6 + i] == 0 ? dx_[9 + i] : dx_[6 + i];
    l[i] = dx_[i] == 0 ? dx_[3 + i] : dx_[i];
    dx_[i] = dx_[3 + i] = 0;
    dx_[6 + i] = dx_[9 + i] = h[i] - l[i];
  }

  for (int i = 0; i < num_hits; ++i)
  {
    // Normalization = 1/sigma^2
    double resolution = hits[i]->GetDetectorInfo()->GetSpatialResolution();
    double normalization = 1.0 / (resolution * resolution);

    // Get cos and sin of the wire angles of this plane
    double rcos = hits[i]->GetDetectorInfo()->GetElementAngleCos();
    double rsin = hits[i]->GetDetectorInfo()->GetElementAngleSin();

    // Determine the offset of the center of this plane
    double center_x = hits[i]->GetDetectorInfo()->GetXPosition();
    double center_y = hits[i]->GetDetectorInfo()->GetYPosition();
    double center_offset = rcos * center_y + rsin * center_x;

    // TODO
    double dx = dx_[hits[i]->GetPlane() - 1];

    double wire_off = -0.5 * hits[i]->GetDetectorInfo()->GetElementSpacing()
        + hits[i]->GetDetectorInfo()->GetElementOffset();
    double hit_pos = hits[i]->GetDriftPosition() + wire_off - dx;

    double r[4];
    r[0] = rsin;
    r[1] = rsin * (hits[i]->GetDetectorInfo()->GetZPosition());
    r[2] = rcos;
    r[3] = rcos * (hits[i]->GetDetectorInfo()->GetZPosition());
    for (int k = 0; k < 4; ++k)
    {
      B[k] += normalization * r[k] * (hit_pos + center_offset);
      for (int j = 0; j < 4; ++j)
        A[k][j] += normalization * r[k] * r[j];
    }
  }

  // Invert the metric matrix
  M_Invert(Ap, cov, 4);

  // Check that inversion was successful
  if (!cov)
  {
    QwWarning << "Inversion failed" << QwLog::endl;
    return -1;
  }

  // Calculate the fit
  M_A_times_b(fit, cov, 4, 4, B); // fit = matrix cov * vector B

  // Calculate chi2^2,rewrite
  chi2 = 0.0;
  std::pair<int, double> worst_case(0, -0.01);
  //Call the Minuit2 code to minimize chi2 
  TFitterMinuit * minuit = new TFitterMinuit();
  minuit->SetPrintLevel(fDebug-1);
  std::vector<QwHit*> test;

  for (int i = 0; i < num_hits; ++i)
  {
     test.push_back(hits[i]); 
  
  }

  MyFCN *fcn = new MyFCN(test);
  minuit->SetMinuitFCN(fcn);

  minuit->SetParameter(0,"M",fit[1],1,0,0);
  minuit->SetParameter(1,"XOff",fit[0],100,0,0);
  
  minuit->SetParameter(2,"N",fit[3],1,0,0);
  minuit->SetParameter(3,"YOff",fit[2],100,0,0);
       minuit->CreateMinimizer();
        int iret = minuit->Minimize();


  std::vector<double> test2;
  test2.push_back(minuit->GetParameter(0));
  test2.push_back(minuit->GetParameter(1));
  test2.push_back(minuit->GetParameter(2));
  test2.push_back(minuit->GetParameter(3));
  
  fit[0] = minuit->GetParameter(1);
  fit[1] = minuit->GetParameter(0);
  fit[2] = minuit->GetParameter(3);
  fit[3] = minuit->GetParameter(2);
 

  for (int i = 0; i < num_hits; ++i)
  {
    // Normalization = 1/sigma^2
    double resolution = hits[i]->GetDetectorInfo()->GetSpatialResolution();
    double normalization = 1.0 / (resolution * resolution);

    // Get cos and sin of the wire angles of this plane
    double rcos = hits[i]->GetDetectorInfo()->GetElementAngleCos();
    double rsin = hits[i]->GetDetectorInfo()->GetElementAngleSin();

    // Absolute positions of the fitted position at this plane
    double x = fit[0] + fit[1] * hits[i]->GetDetectorInfo()->GetZPosition();
    double y = fit[2] + fit[3] * hits[i]->GetDetectorInfo()->GetZPosition();

    // Relative positions of the fitted position at this plane
    x -= hits[i]->GetDetectorInfo()->GetXPosition();
    y -= hits[i]->GetDetectorInfo()->GetYPosition();

    double dx = dx_[hits[i]->GetPlane() - 1];

    double wire_off = -0.5 * hits[i]->GetDetectorInfo()->GetElementSpacing()
        + hits[i]->GetDetectorInfo()->GetElementOffset();
    double hit_pos = hits[i]->GetDriftPosition() + wire_off - dx;
    double residual = y * rcos + x * rsin - hit_pos;

    // Update residuals
    hits[i]->SetPartialTrackPosition(dx - wire_off + y * rcos + x * rsin);
    hits[i]->CalculatePartialTrackResidual();
    QwDebug << "hit " << i << ": " << hits[i]->GetPartialTrackPosition() << QwLog::endl;

    signedresidual[hits[i]->GetPlane() - 1] = residual;
    residual = fabs(residual);
    if (residual > worst_case.second)
    {
      worst_case.first = i;
      worst_case.second = residual;
    }

    chi2 += normalization * residual * residual;
  }

 // Divide by degrees of freedom (number of hits minus two offsets, two slopes)
 // chi2 /= (num_hits - 4);

  //this is the minuit calculated chi2
  chi2 = fcn->operator()(test2);
 
  // Return if we are done
  if (drop_worst_hit == false) {
  delete minuit;


   return 0;
  }

  double best_chi2 = chi2;

  /*
   * try to delete the worst point from the set of hit points, and repeat the
   * procedure one more time. Now it is temporarily turned off
   */
  double temp_residual[12];

  for (int i = 1; i < 12; ++i)
    temp_residual[i] = -10;



  int drop = worst_case.first;
  double fit_drop[4] = { 0.0 };

  // Initialize the matrices
  for (int i = 0; i < 4; ++i)
  {
    B[i] = 0.0;
    for (int j = 0; j < 4; ++j)
      A[i][j] = 0.0;
  }

  // Adjust the matrices A and B by ignoring the worst hit
  for (int i = 0; i < num_hits; ++i)
  {
    // Skip the dropped hit
    if (i == drop) continue;

    // Normalization = 1/sigma^2
    double resolution = hits[i]->GetDetectorInfo()->GetSpatialResolution();
    double normalization = 1.0 / (resolution * resolution);

    // Get cos and sin of the wire angles of this plane
    double rcos = hits[i]->GetDetectorInfo()->GetElementAngleCos();
    double rsin = hits[i]->GetDetectorInfo()->GetElementAngleSin();

    // Determine the offset of the center of this plane
    double center_x = hits[i]->GetDetectorInfo()->GetXPosition();
    double center_y = hits[i]->GetDetectorInfo()->GetYPosition();
    double center_offset = rcos * center_y + rsin * center_x;

    // TODO
    double dx = dx_[hits[i]->GetPlane() - 1];

    double wire_off = -0.5 * hits[i]->GetDetectorInfo()->GetElementSpacing()
        + hits[i]->GetDetectorInfo()->GetElementOffset();
    double hit_pos = hits[i]->GetDriftPosition() + wire_off - dx;

    double r[4];
    r[0] = rsin;
    r[1] = rsin * (hits[i]->GetDetectorInfo()->GetZPosition());
    r[2] = rcos;
    r[3] = rcos * (hits[i]->GetDetectorInfo()->GetZPosition());
    for (int k = 0; k < 4; k++)
    {
      B[k] += normalization * r[k] * (hit_pos + center_offset);
      for (int j = 0; j < 4; j++)
        A[k][j] += normalization * r[k] * r[j];
    }
  }     // end of the for loop for num_hits

  M_Invert(Ap, cov, 4);

  // Check that inversion was successful
  if (!cov)
  {
    cerr << "Inversion failed" << endl;
    return -1;
  }

  // Calculate the fit
  M_A_times_b(fit_drop, cov, 4, 4, B); // fit = matrix cov * vector B

  // Calculate chi2^2, rewrite
  double chi2_test = 0.0;
  for (int i = 0; i < num_hits; i++)
  {
    // Skip the dropped hit
    if (i == drop) continue;


    // Normalization = 1/sigma^2
    double resolution = hits[i]->GetDetectorInfo()->GetSpatialResolution();
    double normalization = 1.0 / (resolution * resolution);

    // Get cos and sin of the wire angles of this plane
    double rcos = hits[i]->GetDetectorInfo()->GetElementAngleCos();
    double rsin = hits[i]->GetDetectorInfo()->GetElementAngleSin();

    // Absolute positions of the fitted position at this plane
    double x = fit_drop[0] + fit_drop[1] * hits[i]->GetDetectorInfo()->GetZPosition();
    double y = fit_drop[2] + fit_drop[3] * hits[i]->GetDetectorInfo()->GetZPosition();

    // Relative positions of the fitted position at this plane
    x -= hits[i]->GetDetectorInfo()->GetXPosition();
    y -= hits[i]->GetDetectorInfo()->GetYPosition();

    double dx = dx_[hits[i]->GetPlane() - 1];

    double wire_off = -0.5 * hits[i]->GetDetectorInfo()->GetElementSpacing()
        + hits[i]->GetDetectorInfo()->GetElementOffset();
    double hit_pos = hits[i]->GetDriftPosition() + wire_off - dx;
    double residual = y * rcos + x * rsin - hit_pos;

    // Update residuals
    hits[i]->SetPartialTrackPosition(dx - wire_off + y * rcos + x * rsin);
    hits[i]->CalculatePartialTrackResidual();
    QwDebug << "hit " << i << ": " << hits[i]->GetPartialTrackPosition() << QwLog::endl;

    temp_residual[hits[i]->GetPlane() - 1] = residual;
    chi2_test += normalization * residual * residual;

  }

  // Divide by degrees of freedom (number of hits minus one dropped hit, two offsets, two slopes)
  chi2_test /= (num_hits - 5);


  // If new chi^2 is less than 80% of the original chi^2, use the improved version
  if (chi2_test < 0.8 * best_chi2)
  {
    best_chi2 = chi2_test;
    for (int k = 0; k < 4; ++k)
      fit[k] = fit_drop[k];
    for (int k = 0; k < 12; ++k)
      signedresidual[k] = temp_residual[k];
  }

  chi2 = best_chi2;
  delete minuit;


  return 0;
}


// NOTE:this version is using information directly from the treeline in plane0 to do the reconstruction
QwPartialTrack* QwTrackingTreeCombine::r3_PartialTrackFit (
    const QwTreeLine* wu,
    const QwTreeLine* wv)
{
  // Initialize variables
  double fit[4];
  double cov[4][4];
  for (size_t i = 0; i < 4; i++) {
    fit[i] = 0;
    for (size_t j = 0; j < 4; j++)
      cov[i][j] = 0.0;
  }

  // NOTE: first get angle information to transfer xy to uv
  double angle = wu->GetHit(0)->GetDetectorInfo()->GetElementAngle();
  Uv2xy uv2xy(angle, 2*Qw::pi - angle);
  //
  double rCos[kNumDirections],rSin[kNumDirections];
  rCos[kDirectionU] = uv2xy.fXY[0][0];
  rCos[kDirectionV] = uv2xy.fXY[1][0];
  rSin[kDirectionU] = uv2xy.fXY[0][1];
  rSin[kDirectionV] = uv2xy.fXY[1][1];

  // NOTE: the parameter contains the uoffset, uslope,voffset,vslope from treeline 0
  // TASK: transfer partial track in u/v to xy(local coordination)
  double perp;
  // NOTE: contains some magic number from geometry file
  // Hard coded values to be updated
  double uvshift = 2.54;

  if (wu->GetHit(0)->GetDetectorInfo()->GetPackage() == kPackage1)
    perp = 39.3679;
  else
    perp = 39.2835;

  double ufront = -wu->fOffset / wu->fSlope;
  double uback  =  ufront + perp / wu->fSlope;

  double vfront = -wv->fOffset / wv->fSlope;
  double vback  =  vfront + perp / wv->fSlope;

  double Pufront[3],Puback[3],Pvfront[3],Pvback[3];
  // NOTE: plane needs four pars to describe ax+by+cz+d=0, here we assume b=1;
  double uplane[4],vplane[4];
  Pufront[0] = ufront*rCos[kDirectionU];
  Pufront[1] = ufront*rSin[kDirectionU];
  Pufront[2] = 0;
  Puback[0] = uback*rCos[kDirectionU];
  Puback[1] = uback*rSin[kDirectionU];
  Puback[2] = perp+Pufront[2];
  uplane[0] = -fabs(rCos[kDirectionU]/rSin[kDirectionU]);
  uplane[1] = 1;
  uplane[3] = -(Pufront[1]+uplane[0]*Pufront[0]);
  uplane[2] = -(uplane[0]*Puback[0]+uplane[1]*Puback[1]+uplane[3])/Puback[2];

  Pvfront[0] = vfront*rCos[kDirectionV];
  Pvfront[1] = vfront*rSin[kDirectionV];
  Pvfront[2] = -uvshift;
  Pvback[0] = vback*rCos[kDirectionV];
  Pvback[1] = vback*rSin[kDirectionV];
  Pvback[2] = perp+Pvfront[2];
  vplane[0] = fabs(rCos[kDirectionV]/rSin[kDirectionV]);
  vplane[1] = 1;
  vplane[2] = (-(vplane[0]*Pvfront[0]+vplane[1]*Pvfront[1])+(vplane[0]*Pvback[0]+vplane[1]*Pvback[1]))/(Pvfront[2]-Pvback[2]);
  vplane[3] = -(vplane[0]*Pvfront[0]+vplane[1]*Pvfront[1])-vplane[2]*Pvfront[2];


  // TASK 2:hook two planes together to get partial track in local xy;
  double P[3],P_dir[3];

  P_dir[0]=uplane[1]*vplane[2]-uplane[2]*vplane[1];
  P_dir[1]=uplane[2]*vplane[0]-uplane[0]*vplane[2];
  P_dir[2]=uplane[0]*vplane[1]-uplane[1]*vplane[0];


  P[0]=-(uplane[3]-vplane[3])/(uplane[0]-vplane[0]);
  P[1]=-(uplane[0]*P[0]+uplane[3]);
  P[2]=0;


  fit[0]=P[0];
  fit[1]=P_dir[0]/P_dir[2];
  fit[2]=P[1];
  fit[3]=P_dir[1]/P_dir[2];

  // TASK 3: rotate xy local to the global coordination(partially because of y axis)
  double P1[3],P2[3],Pp1[3],Pp2[3];
    double ztrans,ytrans,xtrans,costheta,sintheta,cosphi,sinphi;
  //get some detector information
  // NOTE: since the first plane is in v direction and hits[0] is in u, so here 1 should be changed to 2
    
  //if ( wu->GetHit(0)->GetDetectorInfo()->GetPlane() == 2 )
    
  QwVerbose << "TODO (wdc) needs checking" << QwLog::endl;
  costheta = wu->GetHit(0)->GetDetectorInfo()->GetDetectorPitchCos();
  sintheta = wu->GetHit(0)->GetDetectorInfo()->GetDetectorPitchSin();
  cosphi = wu->GetHit(0)->GetDetectorInfo()->GetDetectorRollCos();
  sinphi = wu->GetHit(0)->GetDetectorInfo()->GetDetectorRollSin();
    
  /// xtrans,ytrans,ztrans are first plane's information
  // Due to different handedness, package1's first plane is u, 
  // package2's first plane is v
  if(wu->GetHit(0)->GetDetectorInfo()->GetPackage()==1)
  {
    if( wu->GetHit(0)->GetDetectorInfo()->GetDirection()== kDirectionU )
    {
      xtrans = wu->GetHit(0)->GetDetectorInfo()->GetXPosition();
      ytrans = wu->GetHit(0)->GetDetectorInfo()->GetYPosition();
      ztrans = wu->GetHit(0)->GetDetectorInfo()->GetZPosition();
    }
    else
    { 
      QwWarning << "Error : first hit is not in 1st plane" << QwLog::endl;
      return 0;
    }
  }
  else
  {
    if( wv->GetHit(0)->GetDetectorInfo()->GetDirection()== kDirectionV )
    {
      xtrans = wv->GetHit(0)->GetDetectorInfo()->GetXPosition();
      ytrans = wv->GetHit(0)->GetDetectorInfo()->GetYPosition();
      ztrans = wv->GetHit(0)->GetDetectorInfo()->GetZPosition();
    }
    else
    {
      QwWarning << "Error : first hit is not in 1st plane" << QwLog::endl;
      return 0;
    }
  }
    
  //std::cout<<"(xtrans,ytrans,ztrans) = ("<<xtrans<<", "<<ytrans<<", "<<ztrans<<")"<<std::endl;
    

  P1[2] = 69.9342699;
  P1[0] = fit[1] * P1[2] + fit[0];
  P1[1] = fit[3] * P1[2] + fit[2];
  P2[2] = 92.33705405;
  P2[0] = fit[1] * P2[2] + fit[0];
  P2[1] = fit[3] * P2[2] + fit[2];

  // rotate the points into the lab orientation
  // translate the points into the lab frame

  Pp1[0] = (ytrans + P1[0] * costheta + P1[2] * sintheta) * cosphi + (xtrans + P1[1]) * sinphi;
  Pp1[1] = -1*(ytrans + P1[0] * costheta + P1[2] * sintheta) * sinphi + (xtrans + P1[1]) * cosphi;
  Pp1[2] = ztrans - P1[0] * sintheta + P1[2] * costheta;

  Pp2[0] = (ytrans + P2[0] * costheta + P2[2] * sintheta) * cosphi + (xtrans + P2[1]) * sinphi;
  Pp2[1] = -1*(ytrans + P2[0] * costheta + P2[2] * sintheta) * sinphi + (xtrans + P2[1]) * cosphi;
  Pp2[2] = ztrans - P2[0] * sintheta + P2[2] * costheta;

  // Calculate the new line
  fit[1] = ( Pp2[0] - Pp1[0] ) / ( Pp2[2] - Pp1[2] );
  fit[3] = ( Pp2[1] - Pp1[1] ) / ( Pp2[2] - Pp1[2] );
  fit[0] = Pp2[0] - fit[1] * Pp2[2];
  fit[2] = Pp2[1] - fit[3] * Pp2[2];


  // Create partial track
  QwPartialTrack* pt = new QwPartialTrack();

  // NOTE Why here x and y are swapped because of different coordinate systems
  // first assume it's in the octant 3, then call rotate method in pt class to rotate it to the right position
  pt->fOffsetX = -fit[2]; /// \todo why the minus sign here?
  pt->fOffsetY =  fit[0];
  pt->fSlopeX  = -fit[3]; /// \todo why the minus sign here?
  pt->fSlopeY  =  fit[1];
  // Debug output
  QwDebug << "Reconstructed partial track in x,y (local)" << QwLog::endl;
  QwDebug << "x = " << pt->fOffsetX << " + z * " << pt->fSlopeX << QwLog::endl;
  QwDebug << "y = " << pt->fOffsetY << " + z * " << pt->fSlopeY << QwLog::endl;

  // Add number of hits
  pt->fNumHits = wu->fNumHits + wv->fNumHits;

  // Store copy of treeline
  pt->AddTreeLine(wu);
  pt->AddTreeLine(wv);

  pt->fIsVoid  = false;

  // chi
  pt->fChi = sqrt(wu->fChi*wu->fChi + wv->fChi*wv->fChi); /// \todo chi2(pt) =?= sum chi2(tl) for R3
  // Covariance matrix
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      pt->fCov[i][j] = cov[i][j];

  // Return partial track
  return pt;
}


/*!--------------------------------------------------------------------------*\

 \brief The

    This function fits a set of hits in the HDC to a 3-D track.  This
    task is complicated by the unorthogonality of the u,v, and x wire
    planes.  To obtain the track, the metric matrix (defined below) is
    calculated using the projections of each hit into the lab
    coordinate system.  The matrix is inverted in order to solve the
    system of four equations for the four unknown track parameters
    (bx,mx,by,my).  Once the fit is calculated, the chisquare value
    is determined so that this track may be compared to other track
    candidates in order to select the best fit.

  metric matrix :  The metric matrix is defined by the chi-squared
		equation for unorthogonal coordinate systems.  In order to
		solve for the four track parameters, chi-square is
		differentiated with respect to the parameters, and set to
		zero.  The metric matrix is then derived from the four
		equations using the standard linear algebra technique for
		solving systems of equations.

 *//*-------------------------------------------------------------------------*/

QwPartialTrack* QwTrackingTreeCombine::TcTreeLineCombine (
    QwTreeLine *wu,
    QwTreeLine *wv,
    QwTreeLine *wx,
    int tlayer,
    bool drop_worst_hit)
{
  QwHit *hits[3 * DLAYERS];
  for (int i = 0; i < 3 * DLAYERS; i++)
    hits[i] = 0;

  QwHit **hitarray = 0;
  QwHit *h = 0;

  // Initialize fit results and covariance matrix
  double fit[4];
  double cov[4][4];
  double *covp = &cov[0][0];
  for (int i = 0; i < 4; ++i)
  {
    fit[i] = 0;
    for (int j = 0; j < 4; ++j)
      cov[i][j] = 0;
  }

  // Initialize residuals for each plane
  double residual[12];
  for (int i = 0; i < 12; ++i)
    residual[i] = -10;


  // Put all the hits into one array
  int hitc = 0;
  for (EQwDirectionID dir = kDirectionX; dir <= kDirectionV; dir++)
  {
    switch (dir)
    {
      case kDirectionU:
        hitarray = wu->fHits;
        break;
      case kDirectionV:
        hitarray = wv->fHits;
        break;
      case kDirectionX:
        hitarray = wx->fHits;
        break;
      default:
        hitarray = 0;
        break;
    }
    if (!hitarray)
      continue;

    for (int num = MAXHITPERLINE * DLAYERS; num-- && *hitarray; ++hitarray)
    {
      h = *hitarray;
      if (h->IsUsed() != 0 && hitc < DLAYERS * 3)
      {
        //if(h->GetDirection()!=kDirectionX)
        hits[hitc++] = h;
        //else if(h->GetPlane()==1 || h->GetPlane()==7 || h->GetPlane()==4 || h->GetPlane()==10)
        //    hits[hitc++] = h;
      }
    }
  }

  // Perform the fit
  double chi = 0.0;
  if (r2_PartialTrackFit(hitc, hits, fit, covp, chi, residual, drop_worst_hit) == -1)
  {
    QwWarning << "QwPartialTrack Fit Failed" << QwLog::endl;
    return 0;
  }

  // Create new partial track
  QwPartialTrack* pt = new QwPartialTrack();

  // Store the track offset and slope in the correct coordinate frame
  pt->fOffsetX = -fit[0];
  pt->fOffsetY = fit[2];
  pt->fSlopeX  = -fit[1];
  pt->fSlopeY  = fit[3];

  pt->fIsVoid  = false;

  pt->fChi = sqrt(chi);

  // TODO FIXME
  pt->SetAverageResidual(wx->GetAverageResidual()+wu->GetAverageResidual()+wv->GetAverageResidual());

  // Store covariance matrix
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      pt->fCov[i][j] = cov[i][j];

  // Store residuals for each plane
  for (int i = 0; i < 12; ++i)
    pt->fSignedResidual[i] = residual[i];

  pt->fNumMiss = wu->fNumMiss + wv->fNumMiss + wx->fNumMiss;
  pt->fNumHits = wu->fNumHits + wv->fNumHits + wx->fNumHits;

  // Store tree lines info
  pt->AddTreeLine(wx);
  pt->AddTreeLine(wu);
  pt->AddTreeLine(wv);

  pt->TResidual[kDirectionX] = wx->GetAverageResidual();
  pt->TResidual[kDirectionU] = wu->GetAverageResidual();
  pt->TResidual[kDirectionV] = wv->GetAverageResidual();

  // Return the partial track
  return pt;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* ------------------------------
   Combines u v and x to tracks
   in space.
   ------------------------------ */
/*INPUT:
	treelines : u and v direction
	bins : number of bins in one layer

  OUTPUT:
	(old) uvl gets new treelines for the front x direction
	uvl : is used in TcTreeLineCombine
	QwPartialTrack ret: a 3-D track


 */
std::vector<QwPartialTrack*> QwTrackingTreeCombine::TlTreeCombine (
    const std::vector<QwTreeLine*>& treelines_x,
    const std::vector<QwTreeLine*>& treelines_u,
    const std::vector<QwTreeLine*>& treelines_v,
    EQwDetectorPackage package,
    EQwRegionID region,
    int tlayer,
    int dlayer)
{
  QwDebug << "[QwTrackingTreeCombine::TlTreeCombine]" << QwLog::endl;

  // List of partial tracks to return
  std::vector<QwPartialTrack*> parttracklist;

  //################
  // DECLARATIONS  #
  //################
  int nPartialTracks = 0;

  const int MAXIMUM_PARTIAL_TRACKS = 50;

  //#################################
  // Get the x, u, and v tree lines #
  //#################################

  switch (region) {

    //################
    // REGION 3 VDC  #
    //################
    case kRegionID3:
    {

      //#########################
      // Get the u and v tracks #
      //#########################

      // For 2 plane 0  counters only counters
      int nump0 = 0;
      int count = 0;
      // Count number of U treelines
      for (size_t u = 0; u < treelines_u.size(); u++) {
        // Get treeline
        QwTreeLine* wu = treelines_u[u];

        // Debug
        if (fDebug) {
          QwOut << "wu: " << *wu << QwLog::endl;
          for (int hit = 0; hit < wu->GetNumberOfHits(); hit++)
            QwOut << *(wu->GetHit(hit)) << QwLog::endl;
        }

        // Skip all treelines in a single plane (not matched between chambers)
        if (wu->GetPlane() > 0) continue;

        // Skip this treeline if it was no good
        if (wu->IsVoid()) continue;

        count++;
      }
      if (count < 2) {
        count = 0;
        // Count number of V treelines
        for (size_t v = 0; v < treelines_v.size(); v++) {
          // Get treeline
          QwTreeLine* wv = treelines_v[v];

          // Debug
          if (fDebug) {
            QwOut << "wv: " << *wv << QwLog::endl;
            for (int hit = 0; hit < wv->GetNumberOfHits(); hit++)
              QwOut << *(wv->GetHit(hit)) << QwLog::endl;
          }

          // Skip all treelines in a single plane (not matched between chambers)
          if (wv->GetPlane() > 0) continue;

          // Skip this treeline if it was no good
          if (wv->IsVoid()) continue;

          count++;
        }
      }
      if (count < 2) nump0 = 1;


      // Get the U treeline
      for (size_t u = 0; u < treelines_u.size(); u++) {
        // Get treeline
        QwTreeLine* wu = treelines_u[u];

        // Skip all treelines in a single plane (not matched between chambers)
        if (wu->GetPlane() > 0) continue;

        // Skip this treeline if it was no good
        if (wu->IsVoid()) continue;

        // Get the V treeline
        for (size_t v = 0; v < treelines_v.size(); v++) {
          // Get treeline
          QwTreeLine* wv = treelines_v[v];

          // Skip all treelines in a single plane (not matched between chambers)
          if (wv->GetPlane() > 0) continue;

          // Skip this treeline if it was no good
          if (wv->IsVoid()) continue;

          // Combine the U and V treeline into a partial track
          QwPartialTrack *pt = r3_PartialTrackFit(wu, wv);

          // If a partial track was found
          if (pt) {
            // Set geometry identification
            pt->SetRegion(region);
            pt->SetPackage(package);
            pt->SetOctant(wu->GetOctant());
            pt->RotateCoordinates();
	    if(gQwOptions.GetValue<bool>("QwTracking.R3.RotatorTilt"))
            	pt->RotateRotator(wu->GetHit(0)->GetDetectorInfo());

            // Set 2 plane 0 treelines only
            pt->SetAlone(nump0);

            // Add partial track to list to return
            parttracklist.push_back(pt);
          }

        } // end of loop over V treelines
      } // end of loop over U treelines

      break;
    }

    //################
    // REGION 2 HDC  #
    //################
    case kRegionID2:
    {

      //############################
      // Get the u, v and x tracks #
      //############################

      // Find the partial track with the lowest chi^2
      double best_chi = 10000;
      QwPartialTrack* best_pt = 0;
      QwTreeLine* best_tl[3] = {0};

      //  while (nPartialTracks < MAXIMUM_PARTIAL_TRACKS)

      // Get the U treeline
      for (size_t u = 0; u < treelines_u.size(); u++) {
        // Get treeline
        QwTreeLine* wu = treelines_u[u];

        // Debug
        if (fDebug) {
          QwOut << "wu: " << *wu << QwLog::endl;
          for (int hit = 0; hit < wu->GetNumberOfHits(); hit++)
            QwOut << *(wu->GetHit(hit)) << QwLog::endl;
        }

        // Skip this treeline if it was no good
        if (wu->IsVoid()) continue;

        // Get the V treeline
        for (size_t v = 0; v < treelines_v.size(); v++) {
          // Get treeline
          QwTreeLine* wv = treelines_v[v];

          // Debug
          if (fDebug) {
            QwOut << "wv: " << *wv << QwLog::endl;
            for (int hit = 0; hit < wv->GetNumberOfHits(); hit++)
              QwOut << *(wv->GetHit(hit)) << QwLog::endl;
          }

          // Skip this treeline if it was no good
          if (wv->IsVoid()) continue;

          // Get the V treeline
          for (size_t x = 0; x < treelines_x.size(); x++) {
            // Get treeline
            QwTreeLine* wx = treelines_x[x];

            // Debug
            if (fDebug) {
              QwOut << "wx: " << *wx << QwLog::endl;
              for (int hit = 0; hit < wx->GetNumberOfHits(); hit++)
                QwOut << *(wx->GetHit(hit)) << QwLog::endl;
            }

            // Skip this treeline if it was no good
            if (wx->IsVoid()) continue;

            // Combine the U, V and X treeline into a partial track
            QwPartialTrack *pt = TcTreeLineCombine(wu, wv, wx, tlayer, fDropWorstHit);

            // If a partial track was found
            if (pt) {
              // Find partial track with best chi
              if (pt->fChi < best_chi) {
                // Update best partial track
                if (best_pt) {
                  delete best_pt;
                  best_pt = 0;
                }
                best_pt = pt;
                // Update best chi
                best_chi = best_pt->fChi;

                // Set geometry identification
                best_pt->SetRegion(region);
                best_pt->SetPackage(package);
                best_pt->SetOctant(wv->GetOctant());
                best_pt->RotateCoordinates();

                best_tl[0] = wx;
                best_tl[1] = wu;
                best_tl[2] = wv;

              } else {
                // Delete the partial track
                delete pt;
              }
            }

          } // end of loop over X treelines

        } // end of loop over V treelines

      } // end of loop over U treelines

      // If there was a best partial track
      if (best_pt) {
        ++nPartialTracks;

        best_tl[0]->SetUsed();
        best_tl[1]->SetUsed();
        best_tl[2]->SetUsed();

        // Add partial track to list to return
        parttracklist.push_back(best_pt);
      }


      if (nPartialTracks >= MAXIMUM_PARTIAL_TRACKS)
        QwWarning << "Wow, that's a lot of partial tracks!" << QwLog::endl;


      break;
    }


    //#################
    // OTHER REGIONS  #
    //#################
    default:
    {
      QwError << "[QwTrackingTreeCombine::TlTreeCombine] Error: "
          << "Region " << region << " not supported!" << QwLog::endl;
      break;
    }

  } // end of switch on region


  // Calculate the average residual
  for (size_t pt = 0; pt < parttracklist.size(); pt++) {
    QwPartialTrack* parttrack = parttracklist[pt];
    if (parttrack->IsValid())
      parttrack->CalculateAverageResidual();
  }


  return parttracklist;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
