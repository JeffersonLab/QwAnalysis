//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

 \class QwTrackingTreeCombine

 \brief Combines track segments and performs line fitting.

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

// Boost uBLAS (linear algebra) headers
//    These are included by the inclusion of "matrix.h" below; pmk, 2009oct07.
// #include <boost/numeric/ublas/vector.hpp>
// #include <boost/numeric/ublas/matrix.hpp>

// Qweak headers
#include "QwLog.h"
#include "QwOptions.h"
#include "QwHit.h"
#include "QwDetectorInfo.h"
#include "QwTrackingTree.h"
#include "QwTrackingTreeLine.h"
#include "QwPartialTrack.h"
#include "QwTrack.h"
#include "QwEvent.h"

// Qweak headers
#include "Det.h"
#include "matrix.h"
#include "uv2xy.h"

// Qweak tree object headers
#include "QwTrackingTreeRegion.h"

// Other Qweak modules
#include "QwTrackingTreeSort.h"

#define PI 3.141592653589793

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeCombine::QwTrackingTreeCombine()
{
  fDebug = 0; // Debug level
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeCombine::~QwTrackingTreeCombine ()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeCombine::chi_hashval (int n, QwHit **hit)
{
  double hash = 389.0; // WTF IS THIS?!?
  for (int i = 0; i < n; i++) {
    hash *= hit[i]->rResultPos;
  }
  return ((  (*(unsigned*) & hash)        & HASHMASK) ^
          ( ((*(unsigned*) & hash) >> 22) & HASHMASK) ) ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*! Clear the hash array */
void QwTrackingTreeCombine::chi_hashclear()
{
  memset (hasharr, 0, sizeof(hasharr));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingTreeCombine::chi_hashinsert (
	QwHit **hits,
	int n,
	double slope,
	double xshift,
	double cov[3],
	double chi)
{
  int val = chi_hashval (n, hits), i;

  chi_hash *new_chi_hash = new chi_hash;

  if (new_chi_hash) {
    new_chi_hash->next = hasharr[val];
    hasharr[val] = new_chi_hash;
    new_chi_hash->cx = xshift;
    new_chi_hash->mx = slope;
    new_chi_hash->cov[0] = cov[0];
    new_chi_hash->cov[1] = cov[1];
    new_chi_hash->cov[2] = cov[2];
    new_chi_hash->chi    = chi;
    new_chi_hash->hits   = n;
    for( i = 0; i < n; i++ ) {
      new_chi_hash->hit[i] = hits[i]->rResultPos;
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeCombine::chi_hashfind (
	QwHit **hits,
	int n,
	double *slope,
	double *xshift,
	double cov[3],
	double *chi)
{
  int val = chi_hashval (n, hits);

  chi_hash *new_chi_hash = new chi_hash;

  for (new_chi_hash = hasharr[val]; new_chi_hash;
       new_chi_hash = new_chi_hash->next) {
    if (! new_chi_hash->hits) break;
    if (new_chi_hash->hits == n) {
      int i;
      for (i = 0; i < n; i++) {
	if (new_chi_hash->hit[i] != hits[i]->rResultPos) {
	  break;
	}
      }
      if (i == n) {		/* HIT! */
	*xshift = new_chi_hash->cx;
	*slope  = new_chi_hash->mx;
	cov[0]  = new_chi_hash->cov[0];
	cov[1]  = new_chi_hash->cov[1];
	cov[2]  = new_chi_hash->cov[2];
	*chi    = new_chi_hash->chi;
	return 1;
      }
    }
  }
  return 0;
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
QwHit* QwTrackingTreeCombine::bestx (
	double track_position,
	QwHit* hit)
{
  // Declarations
  double position, distance, best_position;

  // First option of left/right ambiguity
  position = + hit->GetDriftDistance();
  distance = fabs(track_position - position);
  // This is best position for now.
  best_position = position;

  // Second option of left/right ambiguity
  position = - hit->GetDriftDistance();
  distance = fabs(track_position - position);
  // Is this a better position?
  if (distance < best_position)
    best_position = position;

  // Write the best hit (actually just uses the old structure)
  QwHit* besthit = new QwHit(*hit);
  besthit->next = 0; // only this entry in linked list
  besthit->rResultPos =
    besthit->rPos = best_position;

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
int QwTrackingTreeCombine::bestx (
	double *xresult,
	double resolution,
	QwHitContainer *hitlist,
	QwHit **ha,
	double Dx)
{
  //###############
  // DECLARATIONS #
  //###############
  int ngood = 0; // number of good hits that have been found already
  double track_position = *xresult; // x is the x coordinate of the track in the plane
  double odist = 0.0;
  double minimum = resolution; // will keep track of minimum distance
  double breakcut = 0;
  double wirespacing = hitlist->begin()->GetDetectorInfo()->GetElementSpacing();
  // This assumes the same wirespacing for each R2 plane, and first hit defined


  if (hitlist->size() == 0) // ?
    breakcut = wirespacing + resolution; // wirespacing + bin resolution

  // Loop over the hit list
  for (QwHitContainer::iterator hit = hitlist->begin();
       hit != hitlist->end(); hit++) {

    // Consider the two options due to the left/right ambiguity
    for (int j = 0; j < 2; j++) {
      double hit_position =
        j ? (hit->GetElement() + 1) * wirespacing + hit->GetDriftDistance() + Dx
          : (hit->GetElement() + 1) * wirespacing - hit->GetDriftDistance() + Dx;
      double signed_distance = track_position - hit_position;
      double distance = fabs(signed_distance);

      // Only consider this hit if it is not too far away from the track position,
      // i.e. within the drift distance resolution
      if (distance < resolution) {

        // Save only a maximum number of hits per track crossing through the plane
        if (ngood < MAXHITPERLINE) {

          // Duplicate this hit for calibration changes
          ha[ngood] = new QwHit;
          // Copy the hit, but reset the pointers
          *ha[ngood] = *hit;
          ha[ngood]->next    = 0;
          // Store this position
          ha[ngood]->rResultPos = ha[ngood]->rPos = hit_position;

          // ha[ngood]->SetZPos(hit->GetZPos());
          if (distance < minimum) {
            *xresult = hit_position;
            minimum = distance;
          }
          ngood++;


        // If we have already the maximum allowed number of hits,
        // then save this hit only if it is better than the others
        } else {
          // Loop over the hits that we already saved
          for (int i = 0; i < ngood; i++) {
            distance = track_position - ha[i]->GetPosition();
            if (odist < 0)
              odist = -odist;
            if (distance < odist) {
              *ha[i]   = *hit;
              ha[i]->next    = 0;
              ha[i]->rResultPos =
                ha[i]->rPos = hit_position;
              break;
            }
          }
        }
      } // end of distance cut

      if (hit_position == hit->rPos2)
        break;

    } // end of left/right ambiguity

    // ? don't know what this cuts out
    //if (distance < -breakcut)
    //  break;

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
void QwTrackingTreeCombine::mul_do (
	int i,
	int mul,
	int l,
	int *r,
	QwHit *hx[DLAYERS][MAXHITPERLINE],
	QwHit **ha)
{
  int s;

  if (i == 0) {
    for (int j = 0; j < l; j++) {
      ha[j] = hx[j][0];
    }
  } else {
    for (int j = 0; j < l; j++) {
      if (r[j] == 1)
	s = 0;
      else {
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
void QwTrackingTreeCombine::weight_lsq (
	double *slope,	//!< (returns) slope of fitted track
	double *offset,	//!< (returns) offset of fitted track
	double cov[3],	//!< (returns) covariance matrix of slope and offset
	double *chi,	//!< (returns) chi^2 of the fit
	QwHit **hits,	//!< list of hits in every plane
	int n,		//!< number of planes with hits
	int tlayers)	//!< number of detector planes
{
  // (wdc) Changing this to boost::numeric::ublas was not a very smart idea.
  // It is a lot slower than what was there before...  Need to revert this at
  // some point (TODO)
  using namespace boost::numeric::ublas;
  using boost::numeric::ublas::vector;
  using boost::numeric::ublas::matrix;

  vector<double> AtGy(2), y(tlayers), x(2);
  matrix<double> A(tlayers,2), At(tlayers,2), G(tlayers,tlayers);
  matrix<double> AtGA(2,2), AtG(2, tlayers);

  /* Initialize */
  x.assign(zero_vector<double>(2));
  y.assign(zero_vector<double>(tlayers));
  A.assign(zero_matrix<double>(tlayers,2));
  G.assign(zero_matrix<double>(tlayers,tlayers));
  for (int i = 0; i < tlayers; i++)
    A(i,0) = -1.0;

  /* Set the hit values */
  for (int i = 0; i < n; i++) {
    A(i,1) = -(hits[i]->GetDetectorInfo()->GetZPosition());
    y(i)   = - hits[i]->rResultPos;
    double r = hits[i]->GetDetectorInfo()->GetSpatialResolution();
    G(i,i) = 1 / (r * r);
  }

  /* Calculate right and left hand side */
  At = trans(A);
  /* right hand side: A^T * G * y  */
  AtGy = prod (prod (At, G), y);
  /* left hand side: A^T * G * A  */
  AtG = prod (At, G);
  AtGA = prod (AtG, A);
  // (this has to happen in two steps, see uBLAS docs)

  /* Calculate inverse of A^T * G * A */
  double det = (AtGA(0,0) * AtGA(1,1) - AtGA(1,0) * AtGA(0,1));
  double tmp = AtGA(0,0);
  AtGA(0,0) = -AtGA(1,1);
  AtGA(1,1) = -tmp;
  AtGA /= -det;

  /* Solve equation: (A^T * G * A)^-1 * (A^T * G * y) */
  x = prod (AtGA, AtGy);

  *slope  = x(1);
  *offset = x(0);

  cov[0]  = AtGA(0,0);
  cov[1]  = AtGA(0,1);
  cov[2]  = AtGA(1,1);


  /* sqrt (chi^2) */
  double sum = 0.0;
  for (int i = 0; i < n; i++) {

//     r  = (*slope * (hits[i]->Zpos - MAGNET_CENTER) + *offset
// 	  - hits[i]->rResultPos);
    double r  = (*slope * (hits[i]->GetZPosition()) + *offset
		- hits[i]->rResultPos);

    sum += G(i,i) * r * r;
  }
  *chi   = sqrt (sum / n);


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

void QwTrackingTreeCombine::weight_lsq_r3 (
	double *slope,
	double *xshift,
	double cov[3],
	double *chi,
	QwHit **hits,
	int n,
	double z1,
	int offset,
	int tlayers)
{
  // Safety check
  if (n > tlayers) {
    QwWarning << "Warning: arrays WILL overflow!" << QwLog::endl;
  }

  double A[tlayers][2], G[tlayers][tlayers], AtGA[2][2];
  double AtGy[2], y[tlayers], x[2];

  /* Initialize the matrices and vectors */
  for (int i = 0; i < n; i++)
    A[i][0] = -1.0;

  //###########
  // Set Hits #
  //###########
  for (int i = 0; i < n; i++) {
    if (offset == -1) {
      // A[i][1] = -hits[i]->Zpos; //used by matchR3
      A[i][1] = -hits[i]->GetZPosition(); //used by matchR3
      y[i]    = -hits[i]->rPos;
    } else {
      A[i][1] = -(i+offset); //used by Tl MatchHits
      y[i]    = -hits[i]->rResultPos;
    }
    double r = 1.0 / hits[i]->GetSpatialResolution();
    G[i][i] = r * r;
  }

  /* Calculate right hand side: -A^T G y  */
  for (int i = 0; i < 2; i++) {
    double s = 0.0;
    for (int k = 0; k < n; k++)
      s += (A[k][i]) * G[k][k] * y[k];
    AtGy[i] = s;
  }

  /* Calculate the left hand side: A^T * G * A  */
  for (int i = 0; i < 2; i++) {
    for (int k = 0; k < 2; k++) {
      double s = 0.0;
      for (int j = 0; j < n; j++)
        s += (A[j][i]) * G[j][j] * A[j][k];
      AtGA[i][k] = s;
    }
  }

  /* Calculate inverse of A^T * G * A */
  double det = (AtGA[0][0] * AtGA[1][1] - AtGA[1][0] * AtGA[0][1]);
  double h   = AtGA[0][0];
  AtGA[0][0] = AtGA[1][1] / det;
  AtGA[1][1] = h / det;
  AtGA[0][1] /= -det;
  AtGA[1][0] /= -det;

  /* Solve equation: x = (A^T * G * A)^-1 * (A^T * G * y) */
  for (int i = 0; i < 2; i++)
    x[i] = AtGA[i][0] * AtGy[0] + AtGA[i][1] * AtGy[1];
  *slope  = x[1];
  *xshift = x[0];
  /* Calculate covariance */
  cov[0]  = AtGA[0][0];
  cov[1]  = AtGA[0][1];
  cov[2]  = AtGA[1][1];

  double s = 0.0;
  if (offset == -1) {
    for (int i = 0; i < n; i++) {
      // r  = *slope * (hits[i]->Zpos - z1) + *xshift - hits[i]->rResultPos;
      double r  = *slope * (hits[i]->GetZPosition() - z1) + *xshift - hits[i]->rResultPos;
      s  += G[i][i] * r * r;
    }
  } else {
    for (int i = 0; i < n; i++) {
      double r = *slope * (i + offset) + *xshift - hits[i]->rResultPos;
      s += G[i][i] * r * r;
    }
  }

  *chi = sqrt (s/n);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeCombine::contains (double var, QwHit **arr, int len)
{
  // TODO (wdc) This is unsafe due to double comparisons.
  // In light of calibration effects, this could fail.
  cerr << "[QwTrackingTreeCombine::contains] Warning: double == double is unsafe." << endl;
  for (int i = 0; i < len ; i++) {
    if (var == arr[i]->rResultPos)
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
	QwHit *hitarray[],
	QwHit **ha)
{
  int good = 0;
  double x = *xresult;
  double position;
  double distance;
  double minimum = resolution;
  QwHit *h;
  int num;


  for (num = MAXHITPERLINE * DLAYERS; num-- && *hitarray; hitarray++) {
    h = *hitarray;

// There used to be a functionality with the detector info here.  Removed (wdc)
//     if( !h->GetDetectorInfo() ||
// 	(h->GetDetectorInfo()->GetID() != detec->ID)) {
//       continue;
//     }

    position = h->rResultPos;

    if (! contains (position, ha, good)) {
      distance   = x - position;
      ha [good]   = h;
      if (fabs(distance) < minimum) {
	*xresult = position;
	minimum = fabs(distance);
      }
      good++;
      if (good == MAXHITPERLINE)
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
	double cy, double my)
{
  /* Region 2 */
  if (region == kRegionID2) {

    // Don't check R2 acceptance yet
    return true;


  /* Region 3 */
  } else {

    // Loop over all direction in this region
    for (EQwDirectionID dir = kDirectionU; dir <= kDirectionV; dir++) {

      // Loop over all detector planes
      // TODO (wdc) This is unsafe if rd is initialized to 0x0.
      Det *rd;
      double z1 = 0.0;
      for (rd = rcDETRegion[package][region][dir], z1 = rd->Zpos;
	   rd; rd = rd->nextsame) {
	double z = rd->Zpos;
	double x = cx + (z - z1) * mx;
	double y = cy + (z - z1) * my;

	// Is this intersection in the detector acceptance?
	if (rd->center[0] - 0.5*rd->width[0] > x
	 || rd->center[0] + 0.5*rd->width[0] < x
	 || rd->center[1] - 0.5*rd->width[1] > y
	 || rd->center[1] + 0.5*rd->width[1] < y) {
	  return false;
	}
      }
    }
  }
  return 1;
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

*//*-------------------------------------------------------------------------*/
bool QwTrackingTreeCombine::TlCheckForX (
	double x1,  double x2,	///< position at first and last planes
	double dx1, double dx2,	///< uncertainty at first and last planes
	double Dx,		///< offset between first and last planes
	double z1,		///< reference z coordinate
	double Dz,		///< distance between first and last planes
	QwTrackingTreeLine *treeline,	///< treeline to operate on
	QwHitContainer *hitlist,	///< hits list
	int dlayer,
	int tlayer,
	int iteration,
	int stay_tuned,
	double width)
{
  //################
  // DECLARATIONS  #
  //################
  Det *firstdet = 0, *lastdet;	/* detector loop variable                 */
  double thisX, thisZ;		/* X and Z at a given plane               */
  double startZ = 0.0, endZ = 0;

  if (DLAYERS < 4) cerr << "Error: DLAYERS must be >= 4 for region 2!" << endl;


  double resnow;	        /* resultion of a plane                   */
  double dh;
  double minweight = 1e10;
  double stay_chi;
  bool ret   = false;		/* return value (pessimistic)             */


  // Hit lists (TODO should be QwHitContainer?)
  QwHit *goodHits[DLAYERS][MAXHITPERLINE]; /* all hits in each detector plane */
  QwHit *usedHits[DLAYERS];		   /* one hit per detector plane */
  // Initialize to null
  for (int i = 0; i < DLAYERS; i++) {
    for (int j = 0; j < MAXHITPERLINE; j++) {
      goodHits[i][j] = 0;
    }
    usedHits[i] = 0;
  }

  //##################
  //DEFINE VARIABLES #
  //##################
  double orig_slope =  (x2 - x1)  / Dz;	// track slope
  double orig_dmx   = (dx2 - dx1) / Dz;	// track resolution slope
  // (the resolution changes linearly between first and last detector plane)

  // Bin 'resolution'
  // TODO This should be retrieved from the QwHitPattern stored inside the tree line
  int levels = 0;
  switch (treeline->GetRegion()) {
    case kRegionID2: levels = gQwOptions.GetValue<int>("QwTracking.R2.levels"); break;
    case kRegionID3: levels = gQwOptions.GetValue<int>("QwTracking.R2.levels"); break;
    default: break;
  }
  double resolution = width / (1 << (levels - 1));

  //###################################
  //LOOP OVER DETECTORS IN THE REGION #
  //###################################
  // uses BESTX & SELECTX     #
  //###########################

  // Loop over the detector planes of this package/region/direction
  int nPlanesWithHits = 0;	/* number of detector plane with good hits */
  int nHitsInPlane[DLAYERS];	/* number of good hits in a detector plane */
  int nPermutations = 1;	/* number of permutations of hit assignments */
  int plane = 0; // plane number

  EQwRegionID region = treeline->GetRegion();
  EQwDetectorPackage package = treeline->GetPackage();
  EQwDirectionID dir = treeline->GetDirection();

  for (Det* rd = rcDETRegion[package][region][dir];
       rd; rd = rd->nextsame, plane++) {

    // Get subhitlist of hits in this detector
    QwHitContainer *subhitlist = hitlist->GetSubList_Plane(region, package, rd->plane);

    // Coordinates of the track at this detector plane
    thisZ = rd->Zpos;
    thisX = orig_slope * (thisZ - z1) + x1;

    // Store the z coordinates of first and last detector planes
    if (! firstdet) {
      firstdet = rd;
      startZ = thisZ;
    }
    lastdet = rd;
    endZ = thisZ;

    // Skip inactive planes
    if (rd->IsInactive()) {
      delete subhitlist;
      continue;
    }

    // Skip empty planes
    if (subhitlist->size() == 0) {
      delete subhitlist;
      continue;
    }

    /* --- search this road width for hits --- */

    // resolution at this detector plane
    resnow = orig_dmx * (thisZ - z1) + dx1;
    // unless we override this resolution
    if (! iteration
     && ! stay_tuned
     && ! rd->nextsame
     && tlayer == dlayer)
      resnow -= resolution * (fMaxRoad - 1.0);

    // bestx finds the hits which occur closest to the path through
    // the first and last detectors:
    // nHitsInPlane is the number of good hits at each detector layer
    if (! iteration) {          /* first track finding process */
      if (plane < 2)
        // thisX is the calculated x position of the track at the detector plane
        // (thisX is changed by the routine)
        // resnow is the resolution at this position
        // subhitlist is the hit list
        // goodHits is the returned list of hits
        nHitsInPlane[nPlanesWithHits] =
          bestx (&thisX, resnow, subhitlist, goodHits[nPlanesWithHits], 0.0);
      else // for the second set of planes we need to add the detector offset
        nHitsInPlane[nPlanesWithHits] =
          bestx (&thisX, resnow, subhitlist, goodHits[nPlanesWithHits], Dx);

    } else			/* in iteration process (not used by TlTreeLineSort)*/
      nHitsInPlane[nPlanesWithHits] = selectx (&thisX, resnow,/* rd,*/ treeline->hits, goodHits[nPlanesWithHits]);

    /* If there are hits in this detector plane */
    if (nHitsInPlane[nPlanesWithHits]) {
      nPermutations *= nHitsInPlane[nPlanesWithHits];
      nPlanesWithHits++;
    }

    // Delete the subhitlist
    delete subhitlist;
  }

  // Now the hits that have been found in the road are copied to treeline->hits
  // This is done by using the temporary pointer hitarray
  if (! iteration) { // return the hits found in bestx()
    QwHit **hitarray = treeline->hits;
    for (int planeWithHits = 0; planeWithHits < nPlanesWithHits; planeWithHits++ ) {
      for (int hitInPlane = 0; hitInPlane < nHitsInPlane[planeWithHits]; hitInPlane++ ) {
	*hitarray = goodHits[planeWithHits][hitInPlane]; // copy hit to new list
	hitarray++;
      }
    }
    // Check number of hits that have been written and add terminating null
    if (hitarray - treeline->hits < DLAYERS*MAXHITPERLINE + 1)
      *hitarray = 0;		/* add a terminating 0 for later selectx()*/
  }

  //#####################################################
  //DETERMINE THE BEST SET OF HITS FOR THE TREELINE    #
  //#####################################################
  // uses : MUL_DO     #
  //        WEIGHT_LSQ #
  //####################
  int rcSETiMissingTL0 = 1; // allow some planes without hits
  if (nPlanesWithHits >= dlayer - rcSETiMissingTL0) {

    // Loop over all possible permutations
    int best_permutation = -1;
    double best_slope = 0.0, best_offset = 0.0, best_chi = 1e10;
    double best_cov[3] = {0.0, 0.0, 0.0};
    for (int permutation = 0; permutation < nPermutations; permutation++) {

      // Select a permutations from goodHits[][] and store in usedHits[]
      mul_do (permutation, nPermutations, nPlanesWithHits, nHitsInPlane, goodHits, usedHits);
      // (returns usedHits)

      // Determine chi^2 of this set of hits
      double slope, offset; // fitted slope and offset
      double chi = 0.0; // chi^2 of the fit
      double cov[3] = {0.0, 0.0, 0.0}; // covariance matrix

      timeval start, finish;
      gettimeofday(&start, 0);

      weight_lsq (&slope, &offset, cov, &chi, usedHits, nPlanesWithHits, tlayer);
      // (returns slope, offset, cov, chi)

      gettimeofday(&finish, 0);

      // ?
      stay_chi = 0.0;
      if (stay_tuned) {
	dh = (offset +  slope * (startZ - MAGNET_CENTER) -
	     (x1 + orig_slope * (startZ - z1)));
	stay_chi += dh*dh;
	dh = (offset +  slope * (endZ - MAGNET_CENTER) -
	     (x1 + orig_slope * (endZ - z1)));
	stay_chi += dh * dh;
      }
      if (stay_chi + chi + dlayer - nPlanesWithHits < minweight) {
	/* has this been the best track so far? */
	minweight   = stay_chi + chi + dlayer - nPlanesWithHits;
	best_chi    = chi;
	best_slope  = slope;
	best_offset = offset;
	best_permutation  = permutation;
	memcpy (best_cov, cov, sizeof cov);
      }

    } // end of loop over all possible permutations

    treeline->fOffset  = best_offset;
    treeline->fSlope  = best_slope;
    treeline->fChi = best_chi;
    treeline->fNumHits = nPlanesWithHits;
    treeline->SetCov(best_cov);

    // Select the best permutation (if it was found)
    if (best_permutation != -1) {
      mul_do (best_permutation, nPermutations,
	      nPlanesWithHits, nHitsInPlane, goodHits, usedHits);

      // Reset the used flags
      for (int i = 0; i < MAXHITPERLINE * DLAYERS && treeline->hits[i]; i++)
	treeline->hits[i]->fIsUsed = false;

      // Set the used flag on all hits that are used in this treeline
      for (int plane = 0; plane < nPlanesWithHits; plane++) {
	treeline->usedhits[plane] = usedHits[plane];
	if (usedHits[plane])
	  usedHits[plane]->fIsUsed = true;
      }

      // Store the final set of hits for output
      for (int plane = 0; plane < nPlanesWithHits; plane++) {
        if (usedHits[plane]) treeline->AddHit(usedHits[plane]);
      }
    }

    // Set the detector info pointer
    treeline->SetDetectorInfo(treeline->hits[0]->GetDetectorInfo());

    // Check whether we found an optimal track
    if (best_permutation == -1)
      ret = false;	// acceptable hit assignment NOT found
    else
      ret = true;	// acceptable hit assignment found

  } // end of if for required number of planes with hits

  // Store the number of planes without hits; if no acceptable hit assignment
  // was found, set the treeline to void.
  if (ret) {
    treeline->SetValid();
    treeline->fNumMiss = dlayer - nPlanesWithHits;
  } else {
    if (fDebug) {
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
	double x1, double x2,	///< x coordinates at first/last wire (i.e. distances)
	double z1, double z2,	///< z coordinates of first/last wire (i.e. wire number)
	QwTrackingTreeLine *treeline,	///< determined treeline
	QwHitContainer *hitlist,	///< hit list
	int tlayers)		///< number of tree layers
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

  // Declare list of hits and initialize to null
  //  QwHit* goodHits2[tlayers][MAXHITPERLINE];
  QwHit* goodHits[tlayers];
  for (int i = 0; i < tlayers; i++) goodHits[i] = 0;

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
  int nWires = abs(treeline->lastwire - treeline->firstwire + 1);


  // Loop over the pattern positions
  int nHits = 0;
  for (int wire = treeline->firstwire;
           wire <= treeline->lastwire;
           wire++) {

    // Calculate the wire number and associated z coordinate
    int thiswire = treeline->r3offset + wire;
    double thisZ = (double) thiswire;

    // Loop over the hit list to find hits in this wire
    for (QwHitContainer::iterator hit = hitlist->begin();
         hit != hitlist->end() && nHits < tlayers; hit++) {
      // Skip if this is a different wire
      if (hit->GetElement() != thiswire) continue;

      // Calculate the track position at this wire
      double thisX = track_slope * thisZ + intercept;

      // Determine the best hit assignment for this position
      goodHits[nHits] = bestx (thisX, &(*hit));
      nHits++;
    }
  }

  if (nHits != nWires) QwWarning << "WARNING NHITS != NGOODHITS " << nWires << "," << nHits << QwLog::endl;

  //############################
  //RETURN HITS FOUND IN BESTX #
  //############################
  for (int hit = 0; hit < nHits; hit++) {
    goodHits[hit]->fIsUsed = true;
    treeline->usedhits[hit] = goodHits[hit];
    treeline->hits[hit] = treeline->usedhits[hit];
  }

  //######################
  //CALCULATE CHI SQUARE #
  //######################
  double chi = 0.0;
  double slope = 0.0, offset = 0.0;
  double cov[3] = {0.0, 0.0, 0.0};
  weight_lsq_r3 (&slope, &offset, cov, &chi, treeline->hits, nHits, z1, treeline->r3offset, tlayers);

  //################
  //SET PARAMATERS #
  //################

  treeline->fOffset  = offset;
  treeline->fSlope   = slope;     /* return track parameters: offset, slope, chi */
  treeline->fChi     = chi;
  treeline->fNumHits = nHits;
  memcpy(treeline->fCov, cov, sizeof cov);

  // Set the detector info pointer
  treeline->SetDetectorInfo(treeline->hits[0]->GetDetectorInfo());

  int ret = 1;  //need to set up a check that the missing hits is not greater than 1.
  if (! ret) {
    treeline->SetVoid();
    treeline->fNumMiss = nWires - nHits;
  } else {
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
	QwTrackingTreeLine *treelinelist,
	QwHitContainer *hitlist,
	EQwDetectorPackage package,
	EQwRegionID region,
	EQwDirectionID dir,
	unsigned long bins,
	int tlayer,
	int dlayer,
        double width)
{
  chi_hashclear();

  /* Region 3 */
  if (region == kRegionID3) {

    /* TODO In this section we should replace the treeline list with an
       std::list<QwTreeLine>.  Also rcDETRegion->NumOfWires is not really
       needed and can be passed as an argument, similar rcTreeRegion->rWidth. */

    /* --------------------------------------------------
       Calculate line parameters first
       -------------------------------------------------- */

    // For each valid tree line
    for (QwTrackingTreeLine *treeline = treelinelist;
                             treeline && treeline->IsValid();
                             treeline = treeline->next) {

      // First wire position
      double z1 = (double) (treeline->r3offset + treeline->firstwire);
      // Last wire position
      double z2 = (double) (treeline->r3offset + treeline->lastwire);

      // Bin width (bins in the direction of chamber thickness)
      double dx = width / (double) bins;
      // Chamber thickness coordinates at first and last wire
      double x1 = (treeline->a_beg - (double) bins/2) * dx + dx/2;
      double x2 = (treeline->b_end - (double) bins/2) * dx + dx/2;

      // Match the hits with the correct treeline
      TlMatchHits (
        x1, x2, z1, z2,
        treeline, hitlist,
        TLAYERS);
    }



  /* Region 2 */
  } else if (region == kRegionID2) {

    double z1, z2, Dz;
    double r1, r2, Dr;
    double Dx;

    Dx = r1 = Dz = z2 = z1 = 0.0;

    // Determine position differences between first and last detector
    // TODO (wdc) This assumes that tlayer is correct!
    int i = 0;
    for (Det* rd = rcDETRegion[package][region][dir];
              rd && i < tlayer; rd = rd->nextsame, i++) {

      // Store first detector plane's coordinates
      if (i == 0) {
        r1 = rd->center[1];	// r position
        z1 = rd->Zpos;		// z position
      }

      // Store last detector plane's coordinates
      if (i == tlayer - 1) {
        r2 = rd->center[1];	// r position
        z2 = rd->Zpos;		// z position
        // We are looping over detectors with the same direction
        Dr = (r2 - r1);		// difference in r position
        Dz = (z2 - z1);		// difference in z position
        Dx = Dr * fabs(rd->rCos); // difference in x position
      }
    }
    QwDebug << "position difference first/last: " << Dx << " cm" << QwLog::endl;

    // Determine bin widths
    double dx = width;			// detector width
    //double dxh = 0.5 * dx;		// detector half-width
    dx /= (double) bins;		// width of each bin
    //double dxb = dxh / (double) bins;	// half-width of each bin

    /* --------------------------------------------------
       calculate line parameters first
       -------------------------------------------------- */

    // Loop over all valid tree lines
    for (QwTrackingTreeLine *treeline = treelinelist;
           treeline; treeline = treeline->next) {

      if (treeline->IsVoid()) {
        // No tree lines should be void yet
        QwWarning << "Warning: No tree lines should have been voided yet!" << QwLog::endl;
        continue;
      }

      // Debug output
      QwDebug << *treeline << QwLog::endl;

      // Calculate the track from the average of the bins that were hit
      double x1 = treeline->GetPositionFirst(dx);
      double x2 = treeline->GetPositionLast(dx) + Dx;
      // Calculate the uncertainty on the track from the differences
      // and add a road width in units of bin widths
      double dx1 = treeline->GetResolutionFirst(dx) + dx * fMaxRoad;
      double dx2 = treeline->GetResolutionLast(dx)  + dx * fMaxRoad;

      // Debug output
      QwDebug << "(x1,x2,z1,z2) = (" << x1 << ", " << x2 << ", " << z1 << ", " << z2 << "); " << QwLog::endl;
      QwDebug << "(dx1,dx2) = (" << dx1 << ", " << dx2 << ")" << QwLog::endl;

      // Check this tree line for hits and calculate chi^2
      TlCheckForX (
        x1, x2, dx1, dx2, Dx, z1, Dz,
        treeline, hitlist,
        tlayer, tlayer, 0, 0, width);

      // Debug output
      QwDebug << "Done processing " << *treeline << QwLog::endl;
    }

  /* Other regions */
  } else {
    QwWarning << "Warning: TreeCombine not implemented for region " << region << QwLog::endl;
  }

  /* End of region-specific parts */

  /* Now search for identical tree lines in the list */
  QwDebug << "Searching for identical treelines..." << QwLog::endl;
  for (QwTrackingTreeLine *treeline1 = treelinelist; treeline1;
                           treeline1 = treeline1->next) {
    if (treeline1->IsValid()) {
      for (QwTrackingTreeLine *treeline2 = treeline1->next; treeline2;
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
  if (fDebug) {
    cout << "List of treelines:" << endl;
    treelinelist->Print();
  }
}

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
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeCombine::r2_TrackFit (
	int Num,
	QwHit **Hit,
	double *fit,
	double *cov,
	double *chi)
{
  using namespace boost::numeric::ublas;

  //###############
  // Declarations #
  //###############
  double cff;		//resolution coefficient
  double r[4];		//factors of elements of the metric matrix A
  double uvx;		//u,v,or x coordinate of the track at a location in z
  double rCos[kNumDirections],rSin[kNumDirections];	//the rotation angles for the u,v,x coordinates.
  double x0[kNumDirections];			//the translational offsets for the u,v,x axes.
  double offset[kNumDirections],slope[kNumDirections];	//track fit parameters

  Det *rd = rcDETRegion[kPackageUp][kRegionID2][kDirectionX];	//pointer to this detector

  // Initialize the matrices
  double A[4][4];
  double *Ap = &A[0][0];
  double B[4];
  for (int i = 0; i < 4; i++) {
    B[i] = 0.0;
    for (int j = 0; j < 4; j++)
      A[i][j] = 0.0;
  }

  //##################
  // Initializations #
  //##################

  // Find first hit on a u wire and a v wire
  int hitu = 0;
  for (hitu = 0; hitu < Num; hitu++)
    if (Hit[hitu]->GetDetectorInfo()->GetElementDirection() == kDirectionU)
      break;
  int hitv = 0;
  for (hitv = 0; hitv < Num; hitv++)
    if (Hit[hitv]->GetDetectorInfo()->GetElementDirection() == kDirectionV)
      break;

  // Transformation from [u,v] to [x,y]
  double angle = Hit[hitu]->GetDetectorInfo()->GetElementAngle();
  double offsetu = Hit[hitu]->GetDetectorInfo()->GetElementOffset();
  double offsetv = Hit[hitv]->GetDetectorInfo()->GetElementOffset();
  double spacing = Hit[hitu]->GetDetectorInfo()->GetElementSpacing();
  if (angle < 90.0) angle = 180.0 - angle; // angle for U is smaller than 90 deg
  Uv2xy uv2xy(90.0 + angle, 90.0 + 180.0 - angle);
  uv2xy.SetOffset(offsetu, offsetv);
  uv2xy.SetWireSpacing(spacing);

  // Set the angles for our reference frame
  rCos[kDirectionX] = 1; // cos theta x
  rSin[kDirectionX] = 0; // sin theta x
  rCos[kDirectionU] = uv2xy.fXY[0][0]; // cos theta u
  rSin[kDirectionU] = uv2xy.fXY[0][1]; // sin theta u
  rCos[kDirectionV] = uv2xy.fXY[1][0]; // cos theta v
  rSin[kDirectionV] = uv2xy.fXY[1][1]; // sin theta v

  // Set the offsets for the u,v,x axes (why fabs?)
  x0[kDirectionX] = 0;
  x0[kDirectionU] = fabs(uv2xy.fOffset[0]) * uv2xy.fXY[0][0] + uv2xy.fWireSpacing;
  x0[kDirectionV] = fabs(uv2xy.fOffset[1]) * uv2xy.fXY[1][0] + uv2xy.fWireSpacing;

  // Calculate the metric matrix
  for (int i = 0; i < Num; i++) {
    cff  = 1.0 / Hit[i]->GetSpatialResolution();
    cff *= cff;
    EQwDirectionID dir = Hit[i]->GetDetectorInfo()->fDirection;
    r[0] = rCos[dir];
    r[1] = rCos[dir] * (Hit[i]->GetDetectorInfo()->GetZPosition());
    r[2] = rSin[dir];
    r[3] = rSin[dir] * (Hit[i]->GetDetectorInfo()->GetZPosition());
    for (int k = 0; k < 4; k++) {
      B[k] += cff * r[k] * (Hit[i]->GetDriftDistance() - x0[dir]);
      for (int j = 0; j < 4; j++)
        A[k][j] += cff * r[k] * r[j];
    }
  }


  //##################
  // Perform the fit #
  //##################

// As much as I would like to use standard libraries, this takes about 50 times
// longer than the fast routines!
//
//   matrix<double> mA(4,4);
//   for (int j = 0; j < 4; j++)
//     for (int k = 0; k < 4; k++)
//       mA(j,k) = A[k][j];
//
//   // Invert the metric matrix
//   matrix<double> mAinv = invert(mA);

  // Invert the metric matrix
  M_Invert(Ap, cov, 4);

  // Check that inversion was successful
  if (!cov) {
    cerr << "Inversion failed" << endl;
    return -1;
  }

  // Calculate the fit
  M_A_times_b (fit, cov, 4, 4, B); // fit = matrix cov * vector B

  // Transform the offsets from XY to XUV coordinates
  offset[kDirectionX] = fit[0];
  offset[kDirectionU] = uv2xy.xy2u(fit[0], fit[2]);
  offset[kDirectionU] = (fit[0] + fabs(uv2xy.fOffset[0])) * uv2xy.fXY[0][0]
                      + fit[2] * uv2xy.fXY[0][1] + uv2xy.fWireSpacing;
  offset[kDirectionV] = uv2xy.xy2v(fit[0], fit[2]);
  offset[kDirectionV] = (fit[0] + fabs(uv2xy.fOffset[1])) * uv2xy.fXY[1][0]
                  + fit[2] * uv2xy.fXY[1][1] + uv2xy.fWireSpacing;

  // Transform the slopes from XY to XUV coordinates
  slope[kDirectionX] = fit[1];
  slope[kDirectionU] = uv2xy.xy2mu(fit[1], fit[3]);
  slope[kDirectionV] = uv2xy.xy2mv(fit[1], fit[3]);

  // Calculate chi^2
  *chi = 0.0;
  for (int i = 0; i < Num; i++) {
    EQwDirectionID dir = Hit[i]->GetDetectorInfo()->fDirection;
    cff  = 1.0 / Hit[i]->GetSpatialResolution(); // cff  = 1.0 / Hit[i]->Resolution;
    cff *= cff;
    uvx  = offset[dir] + slope[dir] * (Hit[i]->GetDetectorInfo()->GetZPosition());
    *chi += (uvx - Hit[i]->GetDriftDistance()) * (uvx - Hit[i]->GetDriftDistance()) * cff;
  }
  *chi = *chi / (Num - 4);

  // Translate to the lab frame
  fit[0] += rd->center[1];

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeCombine::r3_TrackFit2( int Num, QwHit **Hit, double *fit, double *cov, double *chi)
{
  // Boost uBLAS linear algebra library for matrix inversion
  using namespace boost::numeric::ublas;
  using boost::numeric::ublas::vector;
  using boost::numeric::ublas::matrix;

  //###############
  // Declarations #
  //###############
  double B[4];
  double cff;
  double r[4];
  double uvx;
  double rCos[kNumDirections],rSin[kNumDirections];
  double offset[kNumDirections],slope[kNumDirections];

  double AA[4][4];
  double *AAp = &AA[0][0];

  for (int i = 0; i < Num; i++) {
//    Hit[i]->rPos1 = Hit[i]->rPos;
//    Hit[i]->rPos = Hit[i]->Zpos;
//    Hit[i]->Zpos = Hit[i]->rPos1;
    Hit[i]->SetDriftDistance(Hit[i]->rPos);
    Hit[i]->rPos = Hit[i]->GetZPosition();
    Hit[i]->SetZPosition(Hit[i]->GetDriftDistance());
    //cerr << Hit[i]->Zpos << ',' << Hit[i]->rPos << ',' << Hit[i]->detec->dir << endl;
  }

  // Create the transformation helper object
  double angle = Hit[0]->GetDetectorInfo()->GetElementAngle();
  // The angle for U is smaller than 90 deg, the other one is the complement
  if (angle > 90.0) angle = 180.0 - angle;
  Uv2xy uv2xy (angle, 180.0 - angle);

  // Set the angles for our frame
  rCos[kDirectionU] = -uv2xy.fXY[0][0];
  rCos[kDirectionV] = -uv2xy.fXY[1][0];
  rSin[kDirectionU] = -uv2xy.fXY[0][1];
  rSin[kDirectionV] = -uv2xy.fXY[1][1];

  /* Initialize */
  for (int i = 0; i < 4; i++) {
    B[i] = 0.0;
    for(int j = 0; j < 4; j++)
      AA[i][j] = 0.0;
  }

  //##############################
  // Calculate the metric matrix #
  //##############################
  for (int i = 0; i < Num; i++) {
    cff  = 1.0 / Hit[i]->GetSpatialResolution();
    cff *= cff;
    EQwDirectionID direction = Hit[i]->GetDetectorInfo()->fDirection;
    r[0] = rCos[direction];
    r[1] = rCos[direction] * (Hit[i]->GetZPosition());
    r[2] = rSin[direction];
    r[3] = rSin[direction] * (Hit[i]->GetZPosition());

    for (int k = 0; k < 4; k++) {
      B[k] += cff * r[k] * (Hit[i]->rPos) * (-1);
      for (int j = 0; j < 4; j++)
        AA[k][j] += cff * r[k] * r[j];
    }
  }

  QwDebug << "[QwTrackingTreeCombine::r3_TrackFit2] Before inversion: AA =" << QwLog::endl;
  if (fDebug) M_Print(AAp, cov, 4);

  M_Invert(AAp, cov, 4);

  if (!cov) {
    QwWarning << "[QwTrackingTreeCombine::r3_TrackFit2] Inversion failed" << QwLog::endl;
    return -1;
  }

  QwDebug << "[QwTrackingTreeCombine::r3_TrackFit2] After inversion: AA =" << QwLog::endl;
  if (fDebug) M_Print(AAp, cov, 4);

  /* calculate the fit */

  M_A_times_b (fit, cov, 4, 4, B); // fit = matrix cov * vector B
  // cerr << "3" << endl;

  //calculate the line parameters in u,v directions
  offset[kDirectionU] = uv2xy.xy2u (fit[0], fit[2]);
  offset[kDirectionV] = uv2xy.xy2v (fit[0], fit[2]);
  slope[kDirectionU]  = uv2xy.xy2u (fit[1], fit[3]);
  slope[kDirectionV]  = uv2xy.xy2v (fit[1], fit[3]);

  //ERROR : Somehow I've missed a negative sign somewhere and need to correct for it :
  fit[0] = -fit[0];
  fit[1] = -fit[1];

  //cerr << " u = " << bx[1] << " + z*" << mx[1] << endl;
  //cerr << " v = " << bx[2] << " + z*" << mx[2] << endl;

  //cerr << " y = " << fit[2] << " + z*" << fit[3] << endl;
  //cerr << " x = " << fit[0] << " + z*" << fit[1] << endl;

  /* calculate chi^2 */
  *chi = 0;
  for (int i = 0; i < Num; i++) {
    EQwDirectionID direction = Hit[i]->GetDetectorInfo()->fDirection;
    cff  = 1.0 / Hit[i]->GetSpatialResolution();
    cff *= cff;
    uvx  = offset[direction] + slope[direction] * (Hit[i]->GetZPosition());
    *chi += (uvx - Hit[i]->rPos) * (uvx - Hit[i]->rPos) * cff;
  }
  *chi = *chi / (Num - 4);

  //##########################
  // Rotate to the lab frame #
  //##########################

  double P1[3],P2[3],Pp1[3],Pp2[3];
  double ztrans,ytrans,xtrans,costheta,sintheta;

  //get some detector information
  if (Hit[0]->GetDetectorInfo()->fDirection == kDirectionU) {

    costheta = Hit[0]->GetDetectorInfo()->GetSinDetectorRotation();
    sintheta = Hit[0]->GetDetectorInfo()->GetCosDetectorRotation();

    xtrans = Hit[0]->GetDetectorInfo()->GetXPosition();
    ytrans = Hit[0]->GetDetectorInfo()->GetYPosition();
    ztrans = Hit[0]->GetDetectorInfo()->GetZPosition();

  } else {
    QwWarning << "Error : first hit is not in 1st u-plane" << QwLog::endl;
    return -1;
  }

  //cerr << costheta << ',' << sintheta << endl;
  //cerr << xtrans << ',' << ytrans << ',' << ztrans << endl;
  // get two points on the line
  P1[2] = 69.9342699;
  P1[0] = fit[1] * P1[2] + fit[0];
  P1[1] = fit[3] * P1[2] + fit[2];
  P2[2] = 92.33705405;
  P2[0] = fit[1] * P2[2] + fit[0];
  P2[1] = fit[3] * P2[2] + fit[2];
  //cerr << "(" << P1[0] << ',' << P1[1] << ',' << P1[2] << ')' << endl;
  //cerr << "(" << P2[0] << ',' << P2[1] << ',' << P2[2] << ')' << endl;

  // rotate the points into the lab orientation
  Pp1[0] = P1[0];
  Pp1[1] = P1[1] * costheta - P1[2] * sintheta;
  Pp1[2] = P1[1] * sintheta + P1[2] * costheta;
  Pp2[0] = P2[0];
  Pp2[1] = P2[1] * costheta - P2[2] * sintheta;
  Pp2[2] = P2[1] * sintheta + P2[2] * costheta;
  //cerr << "(" << Pp1[0] << ',' << Pp1[1] << ',' << Pp1[2] << ')' << endl;
  //cerr << "(" << Pp2[0] << ',' << Pp2[1] << ',' << Pp2[2] << ')' << endl;

  // translate the points into the lab frame
  Pp1[0] += xtrans;
  Pp1[1] += ytrans;
  Pp1[2] += ztrans;
  Pp2[0] += xtrans;
  Pp2[1] += ytrans;
  Pp2[2] += ztrans;

  // calculate the new line
  fit[1] = (Pp2[0] - Pp1[0]) / (Pp2[2] - Pp1[2]);
  fit[3] = (Pp2[1] - Pp1[1]) / (Pp2[2] - Pp1[2]);
  fit[0] = Pp2[0] - fit[1] * Pp2[2];
  fit[2] = Pp2[1] - fit[3] * Pp2[2];

  //cerr << "(" << Pp1[0] << ',' << Pp1[1] << ',' << Pp1[2] << ')' << endl;
  //cerr << "(" << Pp2[0] << ',' << Pp2[1] << ',' << Pp2[2] << ')' << endl;

  // and we're done :)
  //cerr << " y = " << fit[2] << " + z*" << fit[3] << endl;
  //cerr << " x = " << fit[0] << " + z*" << fit[1] << endl;
  //cout << fit[0] << ' ' << fit[1] << ' ' << fit[2] << ' ' << fit[3] << endl;

  //double zz1 = 540.0;
  //double zz2 = 569.38;
  //cerr << "(" << (fit[1] * zz1 + fit[0]) << "," << fit[3] * zz1 + fit[2] << "," << zz1 << ")" << endl;
  //cerr << "(" << (fit[1] * zz2 + fit[0]) << "," << fit[3] * zz2 + fit[2] << "," << zz2 << ")" << endl;

  return 1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrackingTreeCombine::r3_TrackFit( int Num, QwHit **hit, double *fit, double *cov, double *chi,double uv2xy[2][2])
{
  //###############
  // Declarations #
  //###############

  QwHit xyz[Num];
  double wcov[3],wchi,mx,bx,my,by;
  QwHit *chihits[Num]; // Hit *chihits[Num];
  double P1[3],P2[3];
  double Pp1[3] = {0.0};
  double Pp2[3] = {0.0};
  double ztrans,ytrans,xtrans,costheta,sintheta;



  // get some detector information
  if (hit[0]->GetDetectorInfo()->fDirection == kDirectionU) {
    costheta = hit[0]->GetDetectorInfo()->GetCosDetectorRotation();
    sintheta = hit[0]->GetDetectorInfo()->GetSinDetectorRotation();
    xtrans = hit[0]->GetDetectorInfo()->GetXPosition();
    ytrans = hit[0]->GetDetectorInfo()->GetYPosition();
    ztrans = hit[0]->GetDetectorInfo()->GetZPosition();

  } else {
    cerr << "Error : first hit is not in 1st u-plane" << endl;
    return -1;
  }
  //#################################################
  // Calculate the x,y coordinates in the VDC frame #
  //#################################################
  for (int i = 0; i < Num; i++) {
//    xyz[i].rPos1=hit[i]->rPos1 * uv2xy[0][0] + hit[i]->rPos2 * uv2xy[0][1];//x
//    xyz[i].rPos=xyz[i].rPos1;
//    xyz[i].rPos2=hit[i]->rPos1 * uv2xy[1][0] + hit[i]->rPos2 * uv2xy[1][1];//y
//    xyz[i].Zpos=hit[i]->rPos;//z
    xyz[i].SetDriftDistance(hit[i]->GetDriftDistance() * uv2xy[0][0] + hit[i]->rPos2 * uv2xy[0][1]);//x
    xyz[i].rPos=xyz[i].GetDriftDistance();
    xyz[i].rPos2=hit[i]->GetDriftDistance() * uv2xy[1][0] + hit[i]->rPos2 * uv2xy[1][1];//y
    xyz[i].SetZPosition(hit[i]->rPos);//z

//    cerr << "Hit coordinates :" << i << "(" << xyz[i].rPos1 << "," << xyz[i].rPos2 << "," << xyz[i].Zpos << ")" << endl;
    cerr << "Hit coordinates :" << i << "(" << xyz[i].GetDriftDistance() << "," << xyz[i].rPos2 << "," << xyz[i].GetZPosition() << ")" << endl;
    // xyz[i].Resolution = 0;
    xyz[i].SetSpatialResolution(0);
  }
  //####################
  // Calculate the fit #
  //####################

  for (int i = 0; i < Num; i++) chihits[i] = &xyz[i];
  weight_lsq_r3(&mx,&bx,wcov,&wchi,chihits,Num,0,-1,Num);
  cerr << "x = " << mx << "z+"<<bx << endl;
  for (int i = 0; i < Num; i++) xyz[i].rPos = xyz[i].rPos2;
  weight_lsq_r3(&my,&by,wcov,&wchi,chihits,Num,0,-1,Num);
  cerr << "y = " << my << "z+"<<by << endl;

  //#################
  // Calculate chi2 #
  //#################
  // Note : without resolutions, chi2 is meaningless

  //##########################
  // Rotate to the lab frame #
  //##########################



  // get two points on the line
  P1[2] = xyz[0].GetZPosition(); //P1[2] = xyz[0].Zpos;
  P1[0] = mx * P1[2] + bx;
  P1[1] = my * P1[2] + by;
  P2[2] = xyz[Num-1].GetZPosition(); // P2[2] = xyz[Num-1].Zpos;
  P2[0] = mx * P2[2] + bx;
  P2[1] = my * P2[2] + by;

  // rotate the points into the lab orientation
  Pp1[1] = P1[1]*costheta - P1[2]*sintheta;
  Pp1[2] = P1[1]*sintheta + P1[2]*costheta;
  Pp2[1] = P2[1]*costheta - P2[2]*sintheta;
  Pp2[2] = P2[1]*sintheta + P2[2]*costheta;

  // translate the points into the lab frame
  Pp1[0] += xtrans;
  Pp1[1] += ytrans;
  Pp1[2] += ztrans;
  Pp2[0] += xtrans;
  Pp2[1] += ytrans;
  Pp2[2] += ztrans;

  // calculate the new line
  mx = (Pp2[0]-Pp1[0]) / (Pp2[2]-Pp1[2]);
  my = (Pp2[1]-Pp1[1]) / (Pp2[2]-Pp1[2]);
  bx = Pp2[0] - mx * Pp2[2];
  by = Pp2[1] - my * Pp2[2];

  // and we're done :)
  fit[0] = bx;
  fit[1] = by;
  fit[2] = mx;
  fit[3] = my;

  cerr << "x = " << mx << "z+"<<bx << endl;
  cerr << "y = " << my << "z+"<<by << endl;
  double zz1 = 	540.0;
  double zz2 =  569.38;
  cerr << "(" << (mx * zz1 + bx) << "," << my * zz1 + by << "," << zz1 << ")" << endl;
  cerr << "(" << (mx * zz2 + bx) << "," << my * zz2 + by << "," << zz2 << ")" << endl;
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwPartialTrack* QwTrackingTreeCombine::TcTreeLineCombine2(
	QwTrackingTreeLine *wu,
	QwTrackingTreeLine *wv,
	int tlayer)
{
  //###############
  // Declarations #
  //###############
  /// \note The number of hits should be passed correctly.  The next line used
  ///       to be 2 * tlayer (for no obvious reason) and was ad-hoc changed to
  ///       twice that.
  /// \todo Need to revisit whether tlayer needs to be passed and used as the
  ///       length of the array hits.
  //Hit *hits[4 * tlayer];

  QwHit **hitarray, *h;
  int hitc, num;

  // covariance matrix, fit, and chi-squared
  double fit[4];
  double  cov[4][4];
  double *covp = &cov[0][0];
  double chi;

  EQwDirectionID dir;
  int ntotal = 0;

  // Initialize covariance matrix and fit
  for (int i = 0; i < 4; i++) {
    fit[i] = 0;
    for (int j = 0; j < 4; j++)
      cov[i][j] = 0;
  }

  // Put all the hits into the array hits, with lenght the total number of hits
  // in the u and v directions.  Not all of the entries will be filled (because
  // not all hits h will be h->used), so it is an upper limit.
  QwHit *hits[wu->fNumHits + wv->fNumHits];
  for (hitc = 0, dir = kDirectionU; dir <= kDirectionV; dir++) {

    switch (dir) {
      case kDirectionU:
        hitarray = wu->hits;
        num = wu->fNumHits;
        break;
      case kDirectionV:
        hitarray = wv->hits;
        num = wv->fNumHits;
        break;
      default:
        hitarray = 0;
        num = 0;
        break;
    }

    // Array bounds
    QwDebug << "[QwTrackingTreeCombine::TcTreeLineCombine2] #tlayer = " << tlayer << QwLog::endl;
    QwDebug << "[QwTrackingTreeCombine::TcTreeLineCombine2] #hits   = " << num << QwLog::endl;

    // Loop over all hits
    for (int i = 0; i < num && *hitarray; i++, hitarray++) {
      h = *hitarray;
      ntotal++;
      if (h->fIsUsed != 0) {
	hits[hitc] = h;
	hitc++;
      }
    }
  }

  // Perform the fit.
  if (r3_TrackFit2(hitc, hits, fit, covp, &chi)  == -1) {
    fprintf(stderr,"hrc: QwPartialTrack Fit Failed\n");
    return 0;
  }
  QwDebug << "Ntotal = " << ntotal << QwLog::endl;

  //#########################
  // Record the fit results #
  //#########################

  QwPartialTrack* pt = new QwPartialTrack();

  pt->fOffsetX = fit[0];
  pt->fOffsetY = fit[2];
  pt->fSlopeX  = fit[1];
  pt->fSlopeY  = fit[3];
  pt->fIsVoid  = false;

  pt->fChi = sqrt(chi);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      pt->fCov[i][j] = cov[i][j];

  pt->numhits = wu->fNumHits + wv->fNumHits;
  pt->tline[kDirectionU] = wu;
  pt->tline[kDirectionV] = wv;

  return pt;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwPartialTrack* QwTrackingTreeCombine::TcTreeLineCombine(
	QwTrackingTreeLine *wu,
	QwTrackingTreeLine *wv,
	int tlayer)
{
  //###############
  // Declarations #
  //###############
  QwHit *hits[tlayer*2], **hitarray, *h;
  int hitc,num;
  static double cov[4][4];
  double *covp = &cov[0][0];
  double fit[4],chi;
  double *fitp = &fit[0];
  EQwDirectionID dir;
  double m,b;
  double uv2xy[2][2];//2 by 2 projection matrix

  ofstream gnu3;

  gnu3.open("gnu3.dat");

  //initialize cov,uv2xy
  for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) cov[i][j] = 0;
  for (int i = 0; i < 2; i++) for (int j = 0; j < 2; j++) uv2xy[i][j] = 0;

 //####################################
  //Get the v/u-coordinate for each hit
  //on the u/v plane in the VDC frame
  //Note : need to implement some kind
  //of resolution values at this point
  //in the code.
  //####################################
  for (hitc = 0, dir = kDirectionU; dir <= kDirectionV; dir++) {
    if (dir == kDirectionU) {
      hitarray = wu->hits;
      num = wu->fNumHits;
      m = 1.0 / wv->fSlope;
      b = -wv->fOffset / wv->fSlope;
    } else {
      hitarray = wv->hits;
      num = wv->fNumHits;
      m = 1.0/ wu->fSlope;
      b = -wu->fOffset / wu->fSlope;
    }
    cerr << "line = " << m << "x+" << b << endl;
    for (int i = 0; i < num; i++, hitarray++) {
      h = *hitarray;
      if (dir == kDirectionU) {
        if (!uv2xy[0][0]) {
          uv2xy[0][0] = -fabs(h->GetDetectorInfo()->GetElementAngleCos());
          uv2xy[1][0] =  fabs(h->GetDetectorInfo()->GetElementAngleSin());
	}
        h->rPos2 = m * h->rPos + b;//v
        // h->rPos1 = h->Zpos;//u
        h->SetDriftDistance(h->GetZPosition());//u

        //z is rPos
      } else {
        if (!uv2xy[0][1]) {
          uv2xy[0][1] = fabs(h->GetDetectorInfo()->GetElementAngleCos());
          uv2xy[1][1] = fabs(h->GetDetectorInfo()->GetElementAngleSin());
	}
        // h->rPos1 = m * h->rPos + b;//u
        h->SetDriftDistance(m * h->GetDriftDistance() + b);//u
        h->rPos2 = h->GetZPosition();//v  // h->rPos2 = h->Zpos;//v
        //cerr << h->rPos << " " << h->Zpos << endl;
        //z is rPos
      }
      //now each hit has a u,v,z coordinate.
      // gnu3 << h->rPos << " " << h->rPos1 << " " << h->rPos2 << endl;
      gnu3 << h->rPos << " " << h->GetDriftDistance() << " " << h->rPos2 << endl;
      //string all the hits together
      if(h->fIsUsed !=0){
        hits[hitc++] = h;
      }
    }
  }
  gnu3.close();
  //########################
  // Perform the track fit #
  //########################
/*  if( rc_TrackFit( hitc, hits, fitp, covp, &chi, 0,0)  == -1) {
    fprintf(stderr,"hrc: QwPartialTrack Fit Failed\n");
    return 0;
  }*/
  if (r3_TrackFit(hitc,hits,fitp,covp,&chi,uv2xy) == -1) {
    fprintf(stderr,"hrc: QwPartialTrack Fit Failed\n");
    return 0;
  }

  //#########################
  // Record the fit results #
  //#########################
  QwPartialTrack* pt = new QwPartialTrack();

  pt->fOffsetX = fit[0];
  pt->fOffsetY = fit[1];
  pt->fSlopeX  = fit[2];
  pt->fSlopeY  = fit[3];
  pt->fIsVoid  = false;
  pt->fChi = sqrt(chi);

  for (int i = 0; i < 4; i++ )
    for (int j = 0; j < 4; j++ )
      pt->fCov[i][j] = cov[i][j];

  pt->numhits = wu->fNumHits + wv->fNumHits;
  pt->tline[kDirectionU] = wu;
  pt->tline[kDirectionV] = wv;

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
	QwTrackingTreeLine *wu,
	QwTrackingTreeLine *wv,
	QwTrackingTreeLine *wx,
	int tlayer)
{
  QwHit *hits[3 * DLAYERS];
  for (int i = 0; i < 3 * DLAYERS; i++)
    hits[i] = 0;

  QwHit **hitarray = 0;
  QwHit *h = 0;
  int hitc, num;


  /* Initialize */
  double fit[4];
  double cov[4][4];
  double *covp = &cov[0][0];
  int ntotal = 0;
  for (int i = 0; i < 4; i++) {
    fit[i] = 0;
    for (int j = 0; j < 4; j++)
      cov[i][j] = 0;
  }

  // Put all the hits into one array.
  hitc = 0;
  for (EQwDirectionID dir = kDirectionX; dir <= kDirectionV; dir++) {
    switch (dir) {
      case kDirectionU: hitarray = wu->hits; break;
      case kDirectionV: hitarray = wv->hits; break;
      case kDirectionX: hitarray = wx->hits; break;
      default: hitarray = 0; break;
    }
    if (! hitarray) continue;

    for (num = MAXHITPERLINE * DLAYERS; num-- && *hitarray; hitarray++) {
      h = *hitarray;
      ntotal++;
      if (h->fIsUsed != 0 && hitc < DLAYERS*3)
	hits[hitc++] = h;
    }
  }

  // Perform the fit.
  double chi = 0.0;
  if (r2_TrackFit(hitc, hits, fit, covp, &chi)  == -1) {
    cerr << "QwPartialTrack Fit Failed" << endl;
    return 0;
  }

  // Put the fit parameters into the particle track using the lab frame now

  QwDebug << "Ntotal = " << ntotal << QwLog::endl;

  QwPartialTrack* pt = new QwPartialTrack();

  pt->fOffsetX = fit[2];
  pt->fOffsetY = fit[0];
  pt->fSlopeX  = fit[3];
  pt->fSlopeY  = fit[1];
  pt->fIsVoid  = false;
  pt->fChi = sqrt(chi);

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      pt->fCov[i][j] = cov[i][j];

  pt->nummiss = wu->fNumMiss + wv->fNumMiss + wx->fNumMiss;
  pt->numhits = wu->fNumHits + wv->fNumHits + wx->fNumHits;

  // Store tree lines
  pt->tline[kDirectionX] = wx;
  pt->tline[kDirectionU] = wu;
  pt->tline[kDirectionV] = wv;

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
QwPartialTrack* QwTrackingTreeCombine::TlTreeCombine (
	QwTrackingTreeLine *uvl[kNumDirections],
	EQwDetectorPackage package,
	EQwRegionID region,
	int tlayer,
	int dlayer,
	QwTrackingTreeRegion **myTreeRegion)
{
  QwDebug << "[QwTrackingTreeCombine::TlTreeCombine]" << QwLog::endl;

  //################
  // DECLARATIONS  #
  //################
  QwPartialTrack* pt_next = 0;
  int nPartialTracks = 0;

  const int MAXIMUM_PARTIAL_TRACKS = 50;

  // TODO This should return a std::vector of QwPartialTrack pointers
  // This is already filled, but not returned yet, because whatever is
  // calling this is not ready for it yet.
  std::vector<QwPartialTrack*> parttracklist;

  int in_acceptance = 0;

  double zx1, zx2;
  zx1 = zx2 = 0.0;
  double d;

  chi_hashclear();

  int i;

  //################
  // REGION 3 VDC  #
  //################
  if (region == kRegionID3) {

    Det *rdu = rcDETRegion[package][region][kDirectionU];
    //  Det *rdv = rcDETRegion[package][region][kDirectionV];
    //  double z1 = rdu->Zpos;
    //  double z2 = rdv->Zpos;
    //  double rot = rdv->Rot; // rotation of planes about the lab x axis
    //  double uv_dz = (z2 - z1) / sin(rot); // distance between u and v planes
    //  double wirecos = rdv->rCos;
    //  double wiresin = rdv->rSin;

    //###################################
    // Get distance between planes, etc #
    //###################################
    double x_[2], y_[2];

    x_[0] = rdu->center[0];
    y_[0] = rdu->center[1];

    x_[1] = rdu->nextsame->center[0];
    y_[1] = rdu->nextsame->center[1];

    // Distance between first u and last u planes
    // (wdc) Shouldn't this be between first u and last v plane (?)
    d = sqrt(pow(x_[1] - x_[0], 2) + pow(y_[1] - y_[0], 2));


    //#########################
    // Get the u and v tracks #
    //#########################

    // Print the u and v tree line lists
    if (fDebug) {
      std::cout << "TreeLines in U:" << std::endl;
      uvl[kDirectionU]->Print();
      std::cout << "TreeLines in V:" << std::endl;
      uvl[kDirectionV]->Print();
    }

    // Get the u track
    QwTrackingTreeLine *wu = uvl[kDirectionU];
    while (wu) {
      if (wu->IsVoid()) { // skip this treeline if it was no good
        wu = wu->next;
        continue;
      }
      // Get wu's line parameters
      //    double mu = wu->mx; // slope
      //    double xu = wu->cx; // constant

      // Get the v track
      QwTrackingTreeLine *wv = uvl[kDirectionV];
      while (wv) {
        if (wv->IsVoid()) {
          wv = wv->next;
          continue;
        }
        // Get wv's line parameters
        //        double mv = wv->mx; // slope
        //        double xv = wv->cx; // constant


        QwPartialTrack *pt = TcTreeLineCombine2 (wu, wv, tlayer);

        if (pt) {
            pt->next   = pt_next;
            pt->bridge = 0;
            pt->pathlenoff = 0;
            pt->pathlenslo = 0;
            pt_next = pt;

            parttracklist.push_back(pt);

            // Check whether this track went through the trigger and/or
            // the cerenkov bar.
            pt->DeterminePositionInTriggerScintillators(package);
            pt->DeterminePositionInCerenkovBars(package);
        }

        // Next v treeline
        wv = wv->next;
      }

      // Next u treeline
      wu = wu->next;
    }

    // Return list of partial tracks
    return pt_next;
  }

  //################
  // REGION 2 HDC  #
  //################
  if (region == kRegionID2) {

    Det *rd;
    for (rd = rcDETRegion[package][region][kDirectionX], i = 0;
         rd && i < tlayer; rd = rd->nextsame, i++) {
      if (i == 0) {
        zx1 = rd->Zpos; // first x-plane z position
      }
      if (i == tlayer - 1) {
        zx2 = rd->Zpos; // last x-plane z position
      }
    }

    //############################
    // Get the u, v and x tracks #
    //############################
    // Print the u, v and x tree line lists
    if (fDebug) std::cout << "Valid treelines in X:" << std::endl;
    if (fDebug) uvl[kDirectionX]->PrintValid();
    if (fDebug) std::cout << "Valid treelines in U:" << std::endl;
    if (fDebug) uvl[kDirectionU]->PrintValid();
    if (fDebug) std::cout << "Valid treelines in V:" << std::endl;
    if (fDebug) uvl[kDirectionV]->PrintValid();

    // Get the u track
    QwTrackingTreeLine *wu = uvl[kDirectionU];
    while (wu && nPartialTracks < MAXIMUM_PARTIAL_TRACKS) {
      if (wu->IsVoid()) { // skip this treeline if it was no good
	wu = wu->next;
	continue;
      }
      // Get wu's line parameters
      double slope_u = wu->fSlope; // slope
      double offset_u = wu->fOffset; // constant

      // Get the v track
      QwTrackingTreeLine *wv = uvl[kDirectionV];
      while (wv) {
        if (wv->IsVoid()) {
	  wv = wv->next;
	  continue;
        }
	// Get wv's line parameters
	double slope_v = wv->fSlope; // slope
	double offset_v = wv->fOffset; // constant

        // Determine u,v at the x detectors
        double u1 = offset_u + zx1 * slope_u;
        double u2 = offset_u + zx2 * slope_u;
        double v1 = offset_v + zx1 * slope_v;
        double v2 = offset_v + zx2 * slope_v;

        // Transformation from [u,v] to [x,y]
        double angle = wu->GetDetectorInfo()->GetElementAngle();
        double offsetu = wu->GetDetectorInfo()->GetElementOffset();
        double offsetv = wv->GetDetectorInfo()->GetElementOffset();
        double spacing = wu->GetDetectorInfo()->GetElementSpacing();
        Uv2xy uv2xy(90.0 + angle, 90.0 + 180.0 - angle);
        uv2xy.SetOffset(offsetu, offsetv);
        uv2xy.SetWireSpacing(spacing);

        // for x
        double x1 = uv2xy.uv2x(u1, v1);
        double x2 = uv2xy.uv2x(u2, v2);
        // for y
        double y1 = uv2xy.uv2y(u1, v1);
        double y2 = uv2xy.uv2y(u2, v2);
        // slope in y
        double my = (y2 - y1) / (zx1 - zx2);

	// Loop over the x tracks
	// (TODO no x hit will never give a partial track!)
	QwTrackingTreeLine *wx = uvl[kDirectionX];
        QwTrackingTreeLine *best_wx = 0; // start with null, no solution guaranteed
        double minimum = 1.0e10;
	while (wx) {
	  if (wx->IsVoid()) {
	    wx = wx->next;
	    continue;
	  }
	  // Get wx's line parameters
	  double slope_x = wx->fSlope; // slope
	  double offset_x = wx->fOffset; // constant

	  // Coordinates for this treeline in x at intersection
	  double xx1 = offset_x + slope_x * zx1;
          double xx2 = offset_x + slope_x * zx2;
	  // Difference with x coordinates from u,v treeline track
          double distance1 = fabs(x1 - xx1);
          double distance2 = fabs(x2 - xx2);
	  double distance = distance1 + distance2;

	  // Keep track of best x treeline
          if (distance < minimum) {
	    best_wx = wx;
	    minimum = distance;
	  }
	  wx = wx->next;
	}

	if (best_wx)
	  in_acceptance = InAcceptance(package, region, best_wx->fOffset, best_wx->fSlope, y1, my);
	else
	  QwDebug << "not in acceptance" << QwLog::endl;

	// Store found partial track (or null)
	QwPartialTrack *pt = TcTreeLineCombine(wu, wv, best_wx, tlayer);

        if (minimum < fMaxXRoad && best_wx && in_acceptance) {
	  if (pt) {

	    nPartialTracks++;

	    best_wx->SetUsed();
	    wv->SetUsed();
	    wu->SetUsed();

            pt->next = pt_next;
	    pt->bridge = 0;
	    pt->pathlenoff = 0;
	    pt->pathlenslo = 0;
	    pt_next = pt;

	    parttracklist.push_back(pt);

            pt->DeterminePositionInTarget();
            pt->DetermineHitInHDC(package);
	  }
	} else {
          delete pt;
          QwDebug << "not close enough " << minimum << ',' << fMaxXRoad << ',' << in_acceptance << QwLog::endl;
        }
        wv = wv->next;
      }
      wu = wu->next;
    }

    if (nPartialTracks >= MAXIMUM_PARTIAL_TRACKS)
      QwWarning << "Wow, that's a lot of partial tracks!" << QwLog::endl;

    return pt_next;


  //#################
  // OTHER REGIONS  #
  //#################

  } else {

    QwError << "[QwTrackingTreeCombine::TlTreeCombine] Error: "
            << "Region " << region << " not supported!" << QwLog::endl;

  }

  return pt_next;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
