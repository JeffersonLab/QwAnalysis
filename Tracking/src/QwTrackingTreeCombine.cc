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

// Qweak headers
#include "QwHit.h"
#include "Det.h"
#include "options.h"
#include "matrix.h"

// Qweak tree object headers
#include "QwTrackingTreeRegion.h"
using namespace QwTracking;

// Other Qweak modules
#include "QwTrackingTreeSort.h"

#define PI 3.141592653589793

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
extern Options opt;

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

/*-------------------------------
   BESTX : This is a much simplified
   version of bestx.  It simply
   matches the left/right wire hits
   in r3 to the pattern being used.
--------------------------------*/
int QwTrackingTreeCombine::bestx (
	double *xresult,	//!- x position of the track in this plane
	QwHit *h,		//!- hit to investigate
	QwHit *ha)		//!- hit with the selected position
{
  double position, distance, bestx;
  double x = *xresult;

  // Detector resolution (not actually used)
  //double resolution = h->Resolution;
  double resolution = h->GetSpatialResolution();

  // First option of left/right ambiguity
  // position = h->rPos1;
  position = h->GetDriftDistance();
  distance = x - position;
  if (distance < 0) distance = -distance;
  bestx = position;
  //cerr << "d1= " << distance << endl;

  // Second option of left/right ambiguity
  // position = -h->rPos1;
  position = -h->GetDriftDistance();
  distance = x - position;
  if (distance < 0) distance = -distance;
  // is this better?
  if (distance < bestx) {
    bestx = position;
  }

  // Write the best hit (actually just uses the old structure)
  *ha = *h;
  ha->next    = 0;
  ha->nextdet = 0;
  ha->isused    = false;
  ha->rResultPos =
        ha->rPos = bestx;
  ha->SetSpatialResolution(resolution);//  ha->Resolution = resolution;

  return 1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/* ------------------------------

   BESTX : searches for the closest hit to
   to *xresult and writes it
   to *xresult. In addition the
   hitchain is searched for hits
   inbetween *xresult - resolution
   and *xresult + resolution. All
   corresponding hits that
   are found are stored in *ha

   Size of the arrays must be
   MAXHITPERLINE

   ------------------------------ */
int QwTrackingTreeCombine::bestx (
	double *xresult,	//!- x position of the track in this plane
	double resolution,	//!- resolution
	QwHitContainer *sublist,//!- hit list
	QwHit* hitlist,		//!- hit list (redundant? can be used as argument?)
	QwHit **ha,		//!- returned hit list
	double Dx)		//!- (default: Dx = 0)
{
  //###############
  // DECLARATIONS #
  //###############
  int ngood = 0; // number of good hits that have been found already
  double x = *xresult; // x is the x coordinate of the track in the plane
  double position;
  double adistance, distance, odist;
  double minimum = resolution; // will keep track of minimum distance
  double breakcut = 0;
  double wirespace = sublist->begin()->GetDetectorInfo()->GetWireSpacing();
  // This assumes the same wirespacing for each R2 plane, and first hit defined



  //#################################################
  // Find good hits up to a max of MAXHITPERLINE.   #
  // Uses a hard maximum distance cut 'resolution'. #
  // If MAXHITPERLINE good hits are reached,        #
  // replace good hits with better hits.            #
  //#################################################
  if (hitlist)
    breakcut = wirespace + resolution; // wirespacing + bin resolution

  // Loop over the hit list
  for (QwHitContainer::iterator hit = sublist->begin(); hit != sublist->end(); hit++) {
    // Two options for the left/right ambiguity
    for (int j = 0; j < 2; j++) {
      position   = j ? (hit->GetElement() + 1) * wirespace + hit->GetDriftDistance() + Dx
		     : (hit->GetElement() + 1) * wirespace - hit->GetDriftDistance() + Dx;
      distance   = x - position;
      adistance  = fabs(distance);

      // Distance cut: only consider this hit if it is not too far away.
      if (adistance < resolution) {
        // Save only the MAXHITPERLINE best hits
	if (ngood < MAXHITPERLINE ) {
	  /* --- duplicate hits for calibration changes --- */
	  ha[ngood] = new QwHit;
	  // Copy the hit, but reset the pointers
	  *ha[ngood] = *hit;
	  ha[ngood]->next    = 0;
	  ha[ngood]->nextdet = 0;
	  ha[ngood]->isused    = false;
	  // Store this position
	  ha[ngood]->rResultPos = ha[ngood]->rPos = position;

          // ha[ngood]->Zpos = hit->detec->Zpos;
          // ha[ngood]->SetZPos(hit->GetZPos());
	  if (adistance < minimum) {
	    *xresult = position;
	    minimum = adistance;
	  }
	  ngood++;


	// If we have already the maximum allowed number of hits,
	// then try to save this hit if it is better than the others
	} else {
	  // Loop over the hits that we already saved
	  for (int i = 0; i < ngood; i++) {
	    distance = x - ha[i]->rPos;
	    if (odist < 0)
	      odist = -odist;
	    if (adistance < odist) {
	      *ha[i]   = *hit;
	      ha[i]->next    = 0;
	      ha[i]->nextdet = 0;
	      ha[i]->isused    = false;
	      ha[i]->rResultPos =
	      ha[i]->rPos    = position;
	      break;
	    }
	  }
	}
      } // end of distance cut

      if (position == hit->rPos2)
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
    A(i,1) = -(hits[i]->GetDetectorInfo()->fZPos);
    y(i)   = - hits[i]->rResultPos;
    double r = hits[i]->GetDetectorInfo()->fSpatialResolution;
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
  double A[tlayers][2], G[tlayers][tlayers], AtGA[2][2];
  double AtGy[2], y[tlayers], x[2];
  double r, det, h;
  double resolution = 0.1;

  /* Initialize the matrices and vectors */
  for (int i = 0; i < tlayers; i++)
    A[i][0] = -1.0;

  //###########
  // Set Hits #
  //###########
  for (int i = 0; i < n; i++) {
    if (offset == -1) {
      // A[i][1] = -hits[i]->Zpos; //used by matchR3
      A[i][1] = -hits[i]->GetZPos(); //used by matchR3
      y[i]    = -hits[i]->rPos;
    } else {
      A[i][1] = -(i+offset); //used by Tl MatchHits
      y[i]    = -hits[i]->rResultPos;
    }
    r = 1.0 / hits[i]->GetSpatialResolution(); // r = 1.0 / hits[i]->Resolution;
    if (! (hits[i]->GetSpatialResolution())) r = 1.0 / resolution; // if (! (hits[i]->Resolution)) r = 1.0 / resolution;
    // WARNING : this is a hack to make this fit work.  Hit resolutions needs to be set up.
    G[i][i] = r * r;
  }

  /* Calculate right hand side: -A^T G y  */
  for (int i = 0; i < 2; i++) {
    double s = 0;
    for (int k = 0; k < n; k++)
      s += (A[k][i]) * G[k][k] * y[k];
    AtGy[i] = s;
  }

  /* Calculate the left hand side: A^T * G * A  */
  for (int i = 0; i < 2; i++) {
    for (int k = 0; k < 2; k++) {
      double s = 0;
      for (int j = 0; j < n; j++)
        s += (A[j][i]) * G[j][j] * A[j][k];
      AtGA[i][k] = s;
    }
  }

  /* Calculate inverse of A^T * G * A */
  det = (AtGA[0][0] * AtGA[1][1] - AtGA[1][0] * AtGA[0][1]);
  cout << "det = " << det << endl;
  h          = AtGA[0][0];
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
      r  = *slope * (hits[i]->GetZPos() - z1) + *xshift - hits[i]->rResultPos;
      double dbg_tmp = G[i][i];
      s  += G[i][i] * r * r;
    }
  } else {
    for (int i = 0; i < n; i++) {
      r = *slope * (i + offset) + *xshift - hits[i]->rResultPos;
      double dbg_tmp = G[i][i];
      s += G[i][i] * r * r;
    }
  }

  *chi = sqrt (s/n);
  //chi_hashinsert(hits, n, *slope, *xshift, cov, *chi);
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
	Det *detec,
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
//comented out the rightwing/leftwing stuff from the following line
    if( !h->detec ||
	(h->detec != detec/* && h->detec != detec->leftwing &&
	 h->detec != detec->rightwing */)) {
      continue;
    }

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

/* ----------------------------------------------------------------------
 * returns the z position of a crossing point of a detector
 * and a line with track parameters x and slope_x
 */
double QwTrackingTreeCombine::detZPosition (
	Det *det,
	double x,
	double slope_x,
	double *xval)
{
  // TODO This should be method of something, not in QwTrackingTreeCombine...
  cerr << "This function is just a stub" << endl;
  return -9999;
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

  EQwDirectionID dir;
  double z, x, y, z1;
  Det *rd;

  /* Region 2 */
  if (region == kRegionID2) {

    // Don't check R2 acceptance yet
    return true;


  /* Region 3 */
  } else {

    // Loop over all direction in this region
    for (dir = kDirectionU; dir <= kDirectionV; dir++) {

      // Loop over all detector planes
      // TODO (wdc) This is unsafe if rd is initialized to 0x0.
      for (rd = rcDETRegion[package][region][dir], z1 = rd->Zpos;
	   rd; rd = rd->nextsame) {
	z = rd->Zpos;
	x = cx + (z - z1) * mx;
	y = cy + (z - z1) * my;

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

/* -------------------------------------------------------------------------
 * TLCHECKFORX :
 * checks for x hits on a line given by values in (RENAMED VARIABLE) and (RENAMED VARIABLE)
 * dlayer: number of detector layers to search in
 * ------------------------------------------------------------------------- */
bool QwTrackingTreeCombine::TlCheckForX (
	double x1,  double x2,	//!- position at first and last planes
	double dx1, double dx2,	//!- uncertainty at first and last planes
	double Dx,		//!- offset between first and last planes
	double z1,		//!- reference z coordinate
	double Dz,		//!- distance between first and last planes
	QwTrackingTreeLine *treeline,	//!- treeline to operate on
	QwHitContainer *hitlist,	//!- hits list
	EQwDetectorPackage package,
	EQwRegionID region,
	EQwDetectorType type,
	EQwDirectionID dir,
	int dlayer,
	int tlayer,
	int iteration,
	int stay_tuned,
	double width)
{
  //################
  // DECLARATIONS  #
  //################
  Det *firstdet = 0, *lastdet;/* detector loop variable                 */
  double thisX, thisZ;		/* X and Z at a given plane               */
  double startZ = 0.0, endZ = 0;

  if (DLAYERS < 4) cerr << "Error: DLAYERS must be >= 4 for region 2!" << endl;


  double resnow;	        /* resultion of a plane                   */
  double dh;
  double minweight = 1e10;
  double stay_chi;
  bool ret   = false;		/* return value (pessimistic)             */
  double MaxRoad = opt.R2maxroad; // TODO global opt

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
  double orig_mx  =  (x2 - x1)  / Dz;	// track slope
  double orig_dmx = (dx2 - dx1) / Dz;	// track resolution slope
  // (the resolution changes linearly between first and last detector plane)

  // bin 'resolution'
  double resolution = width / (1 << (opt.levels[package][region][type]-1));

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
  for (Det* rd = rcDETRegion[package][region][dir];
       rd; rd = rd->nextsame, plane++) {

    // Get sublist of hits in this detector
    QwHitContainer *sublist = hitlist->GetSubList_Plane(region, package, rd->plane);

    // Coordinates of the track at this detector plane
    thisZ = rd->Zpos;
    thisX = orig_mx * (thisZ - z1) + x1;

    // Store the z coordinates of first and last detector planes
    if (! firstdet) {
      firstdet = rd;
      startZ = thisZ;
    }
    lastdet = rd;
    endZ = thisZ;

    // Skip inactive planes
    if (rd->IsInactive()) {
      delete sublist;
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
      resnow -= resolution * (MaxRoad - 1.0);

    // bestx finds the hits which occur closest to the path through
    // the first and last detectors:
    // nHitsInPlane is the number of good hits at each detector layer
    if (! iteration) {          /* first track finding process */
      if (plane < 2)
        // thisX is the calculated x position of the track at the detector plane
        // (thisX is changed by the routine)
        // resnow is the resolution at this position
        // rd->hitbydet is the hit list
        // goodHits is the returned list of hits
        nHitsInPlane[nPlanesWithHits] =
          bestx (&thisX, resnow, sublist, rd->hitbydet, goodHits[nPlanesWithHits], 0.0);
      else // for the second set of planes we need to add a detector offset
        nHitsInPlane[nPlanesWithHits] =
          bestx (&thisX, resnow, sublist, rd->hitbydet, goodHits[nPlanesWithHits], Dx);

    } else			/* in iteration process (not used by TlTreeLineSort)*/
      nHitsInPlane[nPlanesWithHits] = selectx (&thisX, resnow, rd, treeline->hits, goodHits[nPlanesWithHits]);

    /* If there are hits in this detector plane */
    if (nHitsInPlane[nPlanesWithHits]) {
      nPermutations *= nHitsInPlane[nPlanesWithHits];
      nPlanesWithHits++;
    }

    // Delete the sublist
    delete sublist;
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
    if (hitarray - treeline->hits < DLAYERS*MAXHITPERLINE + 1)
      *hitarray = 0;		/* add a terminating 0 for later selectx()*/
  }

  //#####################################################
  //DETERMINE THE BEST SET OF HITS FOR THE TREELINE    #
  //#####################################################
  // uses : MUL_DO     #
  //        WEIGHT_LSQ #
  //####################
  int rcSETiMissingTL0 = 2; // allow some planes without hits
  if (nPlanesWithHits >= dlayer - rcSETiMissingTL0) {

    // Loop over all possible permutations
    int best_permutation = -1;
    double best_mx = 0.0, best_cx = 0.0, best_chi = 1e10;
    double best_cov[3] = {0.0, 0.0, 0.0};
    for (int permutation = 0; permutation < nPermutations; permutation++) {

      // Select a permutations from goodHits[][] and store in usedHits[]
      mul_do (permutation, nPermutations, nPlanesWithHits, nHitsInPlane, goodHits, usedHits);
      // (returns usedHits)

      // Determine chi^2 of this set of hits
      double mx, cx; // fitted slope and offset
      double chi = 0.0; // chi^2 of the fit
      double cov[3] = {0.0, 0.0, 0.0}; // covariance matrix
      weight_lsq (&mx, &cx, cov, &chi, usedHits, nPlanesWithHits, tlayer);
      // (returns mx, cx, cov, chi)

      // ?
      stay_chi = 0.0;
      if (stay_tuned) {
	dh = (cx + mx * (startZ - MAGNET_CENTER) -
	     (x1 + orig_mx * (startZ - z1)));
	stay_chi += dh*dh;
	dh = (cx + mx * (endZ - MAGNET_CENTER) -
	     (x1 + orig_mx * (endZ - z1)));
	stay_chi += dh * dh;
      }
      if (stay_chi + chi + dlayer-nPlanesWithHits < minweight) {
	/* has this been the best track so far? */
	minweight = stay_chi + chi + dlayer - nPlanesWithHits;
	best_chi   = chi;
	best_mx    = mx;
	best_cx    = cx;
	best_permutation  = permutation;
	memcpy (best_cov, cov, sizeof cov);
      }

    } // end of loop over all possible permutations

    //for (j = 0; j < nPlanesWithHits; j++) {
    //  cerr << dir << ',' << usedHits[j]->Zpos << ',' << usedHits[j]->rResultPos << endl;
    //}
    //cerr << "line = " << mmx << ',' << mcx << ',' << chi << endl;
    treeline->cx  = best_cx;
    treeline->mx  = best_mx;
    treeline->chi = best_chi;
    treeline->numhits = nPlanesWithHits;
    memcpy (treeline->cov_cxmx, best_cov, sizeof best_cov);

    // Select the best permutation (if it was found)
    if (best_permutation != -1) {
      mul_do (best_permutation, nPermutations,
	      nPlanesWithHits, nHitsInPlane, goodHits, usedHits);

      // Reset the used flags
      for (int i = 0; i < MAXHITPERLINE * DLAYERS && treeline->hits[i]; i++)
	treeline->hits[i]->isused = false;

      // Set the used flag on all hits that are used in this treeline
      for (int plane = 0; plane < nPlanesWithHits; plane++) {
	if (usedHits[plane])
	  usedHits[plane]->isused = true;
      }
    }

    // Check whether we found an optimal track
    if (best_permutation == -1)
      ret = false;	// acceptable hit assignment NOT found
    else
      ret = true;	// acceptable hit assignment found

  } // end of if for required number of planes with hits

  // Store the number of planes without hits; if no acceptable hit assignment
  // was found, set the treeline to void.
  if (ret) {
    treeline->isvoid  = false;
    treeline->nummiss = dlayer - nPlanesWithHits;
  } else {
    if (fDebug) cout << "Treeline: voided because no best hit assignment" << endl;
    treeline->isvoid  = true;
    treeline->nummiss = dlayer;
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
	double x1,
	double x2,		//!- x coordinates
	double z1,		//!- z coordinate
	double dz,		//!- distance in z coordinate
	QwTrackingTreeLine *treeline,	//!- determined treeline
	EQwDetectorPackage package,	//!- package identifier
	EQwRegionID region,	//!- region identifier
	EQwDetectorType type,	//!- detector type identifier
	EQwDirectionID dir,	//!- wire direction identifier
	int tlayers)		//!- number of tree layers
{
  //################
  // DECLARATIONS  #
  //################
  double thisX, thisZ; /* X and Z for the pattern's wire hit */

  Det *rd;
  double dx, slope, intercept;
  QwHit  DetecHits[tlayers]; /* Hits at a detector */
  QwHit *DHits = DetecHits;

  double mx = 0, cx = 0, chi;
  double cov[3];
  int ret = 0;
  int nhits, j = 0;
  int nmaxhits = 0;

  //##################
  //DEFINE VARIABLES #
  //##################
  dx = x2 - x1;
  slope = dx / dz;
  // TOOD Which one is correct here? opposite sign
  intercept = 0.5 * ((x2 + x1) + slope * (z1 + z1 + dz));
  intercept = x1 - slope * z1;
  cout << "(" << slope << "," << intercept << "," << z1 << "," << x1 << ")" << endl;
  nhits = treeline->lastwire - treeline->firstwire + 1;
  if (nhits < 0) nhits = - nhits;

  //########################
  //MATCH HITS TO TREELINE #
  //########################
  //cerr << "matchhits" << endl;

  //STEP 1 : Match left/right wire hits to the pattern hits
  rd = rcDETRegion[package][region][dir];
  if (treeline->r3offset >= 281) { // get the correct plane for this treeline
    rd = rd->nextsame;
  }
  // Loop over pattern positions
  for (int i = treeline->firstwire; i <= treeline->lastwire; i++, nmaxhits++) {
    // Loop over the hits
      for (QwHit *hit = rd->hitbydet; hit; hit = hit->next) {
      thisZ = treeline->r3offset + i;
      if (hit->GetElement() != thisZ)
        continue;

      thisX = slope * thisZ + intercept;
      //cerr << "Z = " << thisZ << endl;
      bestx (&thisX, hit, DHits);
      DHits++;
      j++;
    }
  }

  if (j != nhits) cerr << "WARNING NHITS != NGOODHITS " << nhits << "," << j << endl;

  //############################
  //RETURN HITS FOUND IN BESTX #
  //############################
  for (int i = 0; i < j; i++) {
    DetecHits[i].isused = true;
    treeline->thehits[i] = DetecHits[i];
    treeline->hits[i] = &treeline->thehits[i];
  }

  //######################
  //CALCULATE CHI SQUARE #
  //######################
  chi = 0.0;
  weight_lsq_r3 (&mx, &cx, cov, &chi, treeline->hits, j, z1, treeline->r3offset, tlayers);

  //################
  //SET PARAMATERS #
  //################
  treeline->cx  = cx;
  treeline->mx  = mx;     /* return track parameters: x, slope, chi */
  treeline->chi = chi;
  treeline->numhits = nhits;
  memcpy(treeline->cov_cxmx, cov, sizeof cov);
/*  if(nhits>=7){//CUT TRACKS MISSING MORE THAN 1 HIT
    ret =1;
  }*/
  ret = 1;  //need to set up a check that the missing hits is not greater than 1.
  if (! ret) {
    treeline->isvoid  = true;
    treeline->nummiss = nmaxhits - nhits;
  } else {
    treeline->isvoid  = false;
    treeline->nummiss = nmaxhits - nhits;
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
	EQwDetectorType type,
	EQwDirectionID dir,
	unsigned long bins,
	int tlayer,
	int dlayer,
        double width)
{
  double z1, z2, Dz;
  double r1, r2, Dr;
  double x1, x2, Dx, dx;

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
                             treeline && !treeline->isvoid;
                             treeline = treeline->next) {

      if (dlayer == 1) {
	treeline->r3offset += rcDETRegion[package][region][dir]->NumOfWires;
      }
      z1 = (double) (treeline->r3offset + treeline->firstwire); // first z position
      z2 = (double) (treeline->r3offset + treeline->lastwire);  // last z position
      dx = width / (double) bins;

      x1 = (treeline->a_beg - (double) bins/2) * dx + dx/2;
      x2 = (treeline->b_end - (double) bins/2) * dx + dx/2;
      TlMatchHits (
        x1, x2, z1, z2 - z1,
        treeline,
        package, region, type, dir,
        TLAYERS);

    }



  /* Region 2 */
  } else if (region == kRegionID2) {

    double dxh, dxb;
    double dx1, dx2;

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
      if (i == tlayer-1) {
        r2 = rd->center[1];	// r position
        z2 = rd->Zpos;		// z position
        // We are looping over detectors with the same direction
        Dr = (r2 - r1);		// difference in r position
        Dz = (z2 - z1);		// difference in z position
        Dx = Dr * fabs(rd->rCos); // difference in x position
      }
    }
    if (fDebug) cout << "position difference first/last: " << Dx << " cm" << endl;

    // Determine bin widths
    dx  = width;		// detector width
    dxh = 0.5 * dx;		// detector half-width
    dx /= (double) bins;	// width of each bin
    dxb = dxh / (double) bins;	// half-width of each bin

    /* --------------------------------------------------
       calculate line parameters first
       -------------------------------------------------- */

    // How large to make the 'road' along a track in which hits will be considered.
    double MaxRoad = opt.R2maxroad; // TODO global opt

    // Loop over all valid tree lines
    for (QwTrackingTreeLine *treeline = treelinelist; treeline;
                             treeline = treeline->next) {
      if (treeline->isvoid) continue; // no tree lines should be void yet

      // Calculate the track from the average of the bins that were hit
      x1 = 0.5 * ((treeline->a_beg + treeline->a_end) * dx);
      x2 = 0.5 * ((treeline->b_beg + treeline->b_end) * dx) + Dx;
      // Calculate the uncertainty on the track from the differences
      // and add a road width in units of bin widths
      dx1 = ((treeline->a_end - treeline->a_beg) * dx) + dx * MaxRoad;
      dx2 = ((treeline->b_end - treeline->b_beg) * dx) + dx * MaxRoad;
      // Debug output
      if (fDebug) {
        cout << "(x1,x2,z1,z2) = (" << x1 << ", " << x2 << ", " << z1 << ", " << z2 << "); ";
        cout << "(dx1,dx2) = (" << dx1 << ", " << dx2 << ")" << endl;
      }
      // Check this tree line for hits and calculate chi^2
      TlCheckForX (
        x1, x2, dx1, dx2, Dx, z1, Dz,
        treeline, hitlist,
        package, region, type, dir,
        tlayer, tlayer, 0, 0, width);
    }

  /* Other regions */
  } else {
    cerr << "Warning: QwTrackingTreeCombine not implemented for region " << region << endl;
  }

  /* End of region-specific parts */

  /* Now search for identical tree lines in the list */
  for (QwTrackingTreeLine *treeline1 = treelinelist; treeline1;
                           treeline1 = treeline1->next) {
    if (treeline1->isvoid == false) {
      for (QwTrackingTreeLine *treeline2 = treeline1->next; treeline2;
                               treeline2 = treeline2->next) {
        if (treeline2->isvoid == false
	 && treeline1->cx == treeline2->cx
	 && treeline1->mx == treeline2->mx) {
	  treeline2->isvoid = true;
	  if (fDebug) cout << "Treeline void because it already exists" << endl;
        }
      }
    }
  }
  if (fDebug) treelinelist->Print();

  /* Sort tracks */
  QwTrackingTreeSort* treesort = new QwTrackingTreeSort(); treesort->SetDebugLevel(fDebug);
  treesort->rcTreeConnSort (treelinelist, region);
  if (treesort) delete treesort;
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

int r2_TrackFit (int Num, QwHit **Hit, double *fit, double *cov, double *chi)
{
  using namespace boost::numeric::ublas;

  //###############
  // Declarations #
  //###############
  double A[4][4];	//metric matrix A
  double B[4];		//constants vector
  double cff;		//resolution coefficient
  double r[4];		//factors of elements of the metric matrix A
  double uvx;		//u,v,or x coordinate of the track at a location in z
  double rCos[kNumDirections],rSin[kNumDirections];	//the rotation angles for the u,v,x coordinates.
  double x0[kNumDirections];			//the translational offsets for the u,v,x axes.
  double bx[kNumDirections],mx[kNumDirections];	//track fit parameters

  Det *rd = rcDETRegion[kPackageUp][kRegionID2][kDirectionX];	//pointer to this detector

  //##################
  // Initializations #
  //##################

  Uv2xy uv2xy(kRegionID2);

  // Set the angles for our reference frame
  rCos[kDirectionX] = 1;//cos theta x
  rSin[kDirectionX] = 0;//sin theta x
  rCos[kDirectionU] = uv2xy.R2_xy[0][0];//cos theta u
  rSin[kDirectionU] = uv2xy.R2_xy[0][1];//sin theta u
  rCos[kDirectionV] = uv2xy.R2_xy[1][0];//cos theta v
  rSin[kDirectionV] = uv2xy.R2_xy[1][1];//sin theta v

  //set the offsets for the u,v,x axes
  x0[kDirectionX] = 0;
  x0[kDirectionU] = fabs(uv2xy.R2_offset[0]) * rCos[1] + uv2xy.R2_wirespacing;
  x0[kDirectionV] = fabs(uv2xy.R2_offset[1]) * rCos[2] + uv2xy.R2_wirespacing;

  //initialize the matrices
  for (int i = 0; i < 4; i++) {
    B[i] = 0;
    for (int j = 0; j < 4; j++)
      A[i][j] = 0;
  }

  // Calculate the metric matrix
  for (int i = 0; i < Num; i++) {
    cff  = 1.0 / Hit[i]->GetSpatialResolution();
    cff *= cff;
    r[0] = rCos[Hit[i]->GetDetectorInfo()->fDirection];
    r[1] = rCos[Hit[i]->GetDetectorInfo()->fDirection] * (Hit[i]->GetZPosition());
    r[2] = rSin[Hit[i]->GetDetectorInfo()->fDirection];
    r[3] = rSin[Hit[i]->GetDetectorInfo()->fDirection] * (Hit[i]->GetZPosition());
    for (int k = 0; k < 4; k++) {
      B[k] += cff*r[k]*(Hit[i]->rPos - x0[Hit[i]->GetDetectorInfo()->fDirection]);
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
  double *Ap = &A[0][0];
  M_Invert(Ap, cov, 4);

  // Check that inversion was successful
  if (!cov) {
    cerr << "Inversion failed" << endl;
    return -1;
  }

  // Calculate the fit
  M_A_times_b (fit, cov, 4, 4, B);// fit = matrix cov * vector B

  //calculate the line parameters in u,v,x directions
  bx[kDirectionX] = fit[0];
  bx[kDirectionU] = (fit[0]+fabs(uv2xy.R2_offset[0]))*rCos[1] + fit[2]*rSin[1] + uv2xy.R2_wirespacing;
  bx[kDirectionV] = (fit[0]+fabs(uv2xy.R2_offset[1]))*rCos[2] + fit[2]*rSin[2] + uv2xy.R2_wirespacing;
  mx[kDirectionX] = fit[1];
  mx[kDirectionU] = fit[1]*rCos[1]+fit[3]*rSin[1];
  mx[kDirectionV] = fit[1]*rCos[2]+fit[3]*rSin[2];

  // Calculate chi^2
  *chi = 0;
  for (int i = 0; i < Num; i++) {
    EQwDirectionID dir = Hit[i]->GetDetectorInfo()->fDirection;
    cff  = 1.0 / Hit[i]->GetSpatialResolution(); // cff  = 1.0 / Hit[i]->Resolution;
    cff *= cff;
    uvx  = bx[dir] + mx[dir] * (Hit[i]->GetZPosition());
    *chi += (uvx - Hit[i]->rPos) * (uvx - Hit[i]->rPos) * cff;
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
  double bx[kNumDirections],mx[kNumDirections];
  EQwDirectionID Dir;
  //Det *rd;

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

  Uv2xy uv2xy(kRegionID3);

  // Set the angles for our frame
  rCos[kDirectionU] = -uv2xy.R3_xy[0][0];
  rCos[kDirectionV] = -uv2xy.R3_xy[1][0];
  rSin[kDirectionU] = -uv2xy.R3_xy[0][1];
  rSin[kDirectionV] = -uv2xy.R3_xy[1][1];

  /* Initialize */
  for (int i = 0; i < 4; i++) {
    B[i] = 0;
    for(int j = 0; j < 4; j++)
      AA[i][j] = 0;
  }

  //##############################
  // Calculate the metric matrix #
  //##############################
  for (int i = 0; i < Num; i++ ) {
    //cerr << i << " " << Hit[i]->Zpos << " " << Hit[i]->rPos << " ";
    cff  = 1.0 / Hit[i]->GetSpatialResolution(); // cff  = 1.0/Hit[i]->Resolution;
    cff *= cff;
    r[0] = rCos[Hit[i]->detec->dir];
    r[1] = rCos[Hit[i]->detec->dir]*(Hit[i]->GetZPos()); //r[1] = rCos[Hit[i]->detec->dir]*(Hit[i]->Zpos);
    r[2] = rSin[Hit[i]->detec->dir];
    r[3] = rSin[Hit[i]->detec->dir]*(Hit[i]->GetZPos()); //r[3] = rSin[Hit[i]->detec->dir]*(Hit[i]->Zpos);

    for (int k = 0; k < 4; k++) {
      B[k] += cff * r[k] * (Hit[i]->rPos) * (-1);
      for (int j = 0; j < 4; j++)
	AA[k][j] += cff*r[k]*r[j];
    }
  }

  if (fDebug) cout << "[QwTrackingTreeCombine::r3_TrackFit2] Before inversion: AA =" << endl;
  if (fDebug) M_Print(AAp, cov, 4);

  M_Invert(AAp, cov, 4);

  if (!cov) {
    cerr << "[QwTrackingTreeCombine::r3_TrackFit2] Inversion failed" << endl;
    return -1;
  }

  if (fDebug) cout << "[QwTrackingTreeCombine::r3_TrackFit2] After inversion: AA =" << endl;
  if (fDebug) M_Print(AAp, cov, 4);

  /* calculate the fit */

  M_A_times_b (fit, cov, 4, 4, B); // fit = matrix cov * vector B
  // cerr << "3" << endl;

  //calculate the line parameters in u,v directions
  bx[kDirectionU] = uv2xy.xy2u(fit[0],fit[2]);
  bx[kDirectionV] = uv2xy.xy2v(fit[0],fit[2]);
  mx[kDirectionU] = uv2xy.xy2u(fit[1],fit[3]);
  mx[kDirectionV] = uv2xy.xy2v(fit[1],fit[3]);

  //cerr << uv2xy.xy2v(fit[0],fit[2],Hit[0]->detec->region) << endl;

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
    Dir = Hit[i]->detec->dir;
    cff  = 1.0 / Hit[i]->GetSpatialResolution(); // cff  = 1.0 / Hit[i]->Resolution;
    cff *= cff;
    uvx  = bx[Dir] + mx[Dir] * (Hit[i]->GetZPos()); // uvx  = bx[Dir] + mx[Dir] * (Hit[i]->Zpos);
    //cerr << uvx << ',' << Hit[i]->rPos << endl;
    *chi += (uvx-Hit[i]->rPos)* (uvx-Hit[i]->rPos)*cff;
  }
  *chi = *chi / (Num - 4);

  //##########################
  // Rotate to the lab frame #
  //##########################

  double P1[3],P2[3],Pp1[3],Pp2[3];
  double ztrans,ytrans,xtrans,costheta,sintheta;
  //get some detector information
  if (Hit[0]->detec->dir == kDirectionU) {
    costheta = Hit[0]->detec->rRotSin;
    sintheta = Hit[0]->detec->rRotCos;
    xtrans = Hit[0]->detec->center[0];
    ytrans = Hit[0]->detec->center[1];
    ztrans = Hit[0]->detec->Zpos;

  } else {
    cerr << "Error : first hit is not in 1st u-plane" << endl;
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
  Pp1[1] = P1[1]*costheta - P1[2]*sintheta;
  Pp1[2] = P1[1]*sintheta + P1[2]*costheta;
  Pp2[0] = P2[0];
  Pp2[1] = P2[1]*costheta - P2[2]*sintheta;
  Pp2[2] = P2[1]*sintheta + P2[2]*costheta;
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
  fit[1] = (Pp2[0]-Pp1[0]) / (Pp2[2]-Pp1[2]);
  fit[3] = (Pp2[1]-Pp1[1]) / (Pp2[2]-Pp1[2]);
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
  double P1[3],P2[3],Pp1[3],Pp2[3];
  double ztrans,ytrans,xtrans,costheta,sintheta;

  // get some detector information
  if (hit[0]->detec->dir == kDirectionU) {
    costheta = hit[0]->detec->rRotCos;
    sintheta = hit[0]->detec->rRotSin;
    xtrans = hit[0]->detec->center[0];
    ytrans = hit[0]->detec->center[1];
    ztrans = hit[0]->detec->Zpos;

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
    xyz[i].SetZPos(hit[i]->rPos);//z

//    cerr << "Hit coordinates :" << i << "(" << xyz[i].rPos1 << "," << xyz[i].rPos2 << "," << xyz[i].Zpos << ")" << endl;
    cerr << "Hit coordinates :" << i << "(" << xyz[i].GetDriftDistance() << "," << xyz[i].rPos2 << "," << xyz[i].GetZPos() << ")" << endl;
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
  P1[2] = xyz[0].GetZPos(); //P1[2] = xyz[0].Zpos;
  P1[0] = mx * P1[2] + bx;
  P1[1] = my * P1[2] + by;
  P2[2] = xyz[Num-1].GetZPos(); // P2[2] = xyz[Num-1].Zpos;
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
  QwHit *hits[wu->numhits + wv->numhits];
  for (hitc = 0, dir = kDirectionU; dir <= kDirectionV; dir++) {

    switch (dir) {
      case kDirectionU:
        hitarray = wu->hits;
        num = wu->numhits;
        break;
      case kDirectionV:
        hitarray = wv->hits;
        num = wv->numhits;
        break;
      default:
        hitarray = 0;
        num = 0;
        break;
    }

    // Array bounds
    if (fDebug) {
      cout << "[QwTrackingTreeCombine::TcTreeLineCombine2] #tlayer = " << tlayer << endl;
      cout << "[QwTrackingTreeCombine::TcTreeLineCombine2] #hits   = " << num << endl;
    }

    // Loop over all hits
    for (int i = 0; i < num && *hitarray; i++, hitarray++) {
      h = *hitarray;
      ntotal++;
      if (h->isused != 0) {
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
  if (fDebug) cout << "Ntotal = " << ntotal << endl;

  //#########################
  // Record the fit results #
  //#########################

  QwPartialTrack* pt = new QwPartialTrack();

  pt->x  = fit[0];
  pt->y  = fit[2];
  pt->mx = fit[1];
  pt->my = fit[3];
  pt->isvoid  = false;

  pt->chi = sqrt(chi);
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      pt->Cov_Xv[i][j] = cov[i][j];

  pt->numhits = wu->numhits + wv->numhits;
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
      num = wu->numhits;
      m = 1/wv->mx;
      b = -wv->cx/wv->mx;
    } else {
      hitarray = wv->hits;
      num = wv->numhits;
      m = 1/wu->mx;
      b = -wu->cx/wu->mx;
    }
    cerr << "line = " << m << "x+" << b << endl;
    for (int i = 0; i < num; i++, hitarray++) {
      h = *hitarray;
      if (dir == kDirectionU) {
        if (!uv2xy[0][0]) {
          uv2xy[0][0] = -fabs(h->detec->rCos);
	  uv2xy[1][0] =  fabs(h->detec->rSin);
	}
        h->rPos2 = m * h->rPos + b;//v
        // h->rPos1 = h->Zpos;//u
        h->SetDriftDistance(h->GetZPos());//u

        //z is rPos
      } else {
        if (!uv2xy[0][1]) {
          uv2xy[0][1]=fabs(h->detec->rCos);
	  uv2xy[1][1]=fabs(h->detec->rSin);
	}
        // h->rPos1 = m * h->rPos + b;//u
        h->SetDriftDistance(m * h->rPos + b);//u
        h->rPos2 = h->GetZPos();//v  // h->rPos2 = h->Zpos;//v
        //cerr << h->rPos << " " << h->Zpos << endl;
        //z is rPos
      }
      //now each hit has a u,v,z coordinate.
      // gnu3 << h->rPos << " " << h->rPos1 << " " << h->rPos2 << endl;
      gnu3 << h->rPos << " " << h->GetDriftDistance() << " " << h->rPos2 << endl;
      //string all the hits together
      if(h->isused !=0){
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

  pt->x  = fit[0];
  pt->y  = fit[1];
  pt->mx = fit[2];
  pt->my = fit[3];
  pt->isvoid  = false;

  pt->chi = sqrt(chi);

  for (int i = 0; i < 4; i++ )
    for (int j = 0; j < 4; j++ )
      pt->Cov_Xv[i][j] = cov[i][j];

  pt->numhits = wu->numhits + wv->numhits;
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

  QwHit **hitarray, *h;
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
      if (h->isused != 0 && hitc < DLAYERS*3)
	hits[hitc++] = h;
    }
  }

  // Perform the fit.
  double chi = 0;
  if (r2_TrackFit(hitc, hits, fit, covp, &chi)  == -1) {
    cerr << "QwPartialTrack Fit Failed" << endl;
    return 0;
  }

  // Put the fit parameters into the particle track using the lab frame now

  if (fDebug) cout << "Ntotal = " << ntotal << endl;

  QwPartialTrack* pt = new QwPartialTrack();

  pt->x  = fit[2];
  pt->y  = fit[0];
  pt->mx = fit[3];
  pt->my = fit[1];
  pt->isvoid  = false;

  pt->chi = sqrt(chi);

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      pt->Cov_Xv[i][j] = cov[i][j];

  pt->nummiss = wu->nummiss + wv->nummiss + wx->nummiss;
  pt->numhits = wu->numhits + wv->numhits + wx->numhits;

  // Store tree lines
  pt->tline[kDirectionX] = wx;
  pt->tline[kDirectionU] = wu;
  pt->tline[kDirectionV] = wv;

  return pt;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double uv2x (double u, double v, double wirecos)
{
  return (u-v)*fabs(wirecos);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double uv2y (double u, double v, double wiresin)
{
  return (u+v)*fabs(wiresin);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double xy2u (double x, double y)
{
  cerr << "This function is just a stub" << endl;
  return -1000;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double xy2v (double x, double y)
{
  cerr << "This function is just a stub" << endl;
  return -1000;
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
	long bins,
	EQwDetectorPackage package,
	EQwRegionID region,
	EQwDetectorType type,
	int tlayer,
	int dlayer,
	QwTrackingTreeRegion **myTreeRegion)
{
  if (fDebug) cout << "[QwTrackingTreeCombine::TlTreeCombine]" << endl;

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

  int in_acceptance;

  double zx1, zx2;
  double d;

  chi_hashclear();

  int i;

  //################
  // REGION 3 VDC  #
  //################
  if (region == kRegionID3 && type == kTypeDriftVDC) {

    Det *rdu = rcDETRegion[package][region][kDirectionU];
    Det *rdv = rcDETRegion[package][region][kDirectionV];
    double z1 = rdu->Zpos;
    double z2 = rdv->Zpos;
    double rot = rdv->Rot; // rotation of planes about the lab x axis
    double uv_dz = (z2 - z1) / sin(rot); // distance between u and v planes
    double wirecos = rdv->rCos;
    double wiresin = rdv->rSin;

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
    if (fDebug) std::cout << "TreeLines in U:" << std::endl;
    if (fDebug) uvl[kDirectionU]->Print();
    if (fDebug) std::cout << "TreeLines in V:" << std::endl;
    if (fDebug) uvl[kDirectionV]->Print();

    // Get the u track
    QwTrackingTreeLine *wu = uvl[kDirectionU];
    while (wu) {
      if (wu->isvoid) { // skip this treeline if it was no good
	wu = wu->next;
	continue;
      }
      // Get wu's line parameters
      double mu = wu->mx; // slope
      double xu = wu->cx; // constant

      // Get the v track
      QwTrackingTreeLine *wv = uvl[kDirectionV];
      while (wv) {
        if (wv->isvoid) {
	  wv = wv->next;
	  continue;
        }
	// Get wv's line parameters
	double mv = wv->mx; // slope
	double xv = wv->cx; // constant


	QwPartialTrack *pt = TcTreeLineCombine2 (wu, wv, tlayer);

	if (pt) {
	    pt->next   = pt_next;
	    pt->bridge = 0;
	    pt->pathlenoff = 0;
	    pt->pathlenslo = 0;
	    pt_next = pt;

	    parttracklist.push_back(pt);

	    // Check whether this track went through the trigger and/or
	    // the Cerenkov bar.
	    checkR3(pt, package);

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
  if (region == kRegionID2 && type == kTypeDriftHDC) {

    double MaxXRoad = opt.R2maxXroad; // TODO global opt
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
    if (fDebug) std::cout << "TreeLines in X:" << std::endl;
    if (fDebug) uvl[kDirectionX]->Print();
    if (fDebug) std::cout << "TreeLines in U:" << std::endl;
    if (fDebug) uvl[kDirectionU]->Print();
    if (fDebug) std::cout << "TreeLines in V:" << std::endl;
    if (fDebug) uvl[kDirectionV]->Print();

    // Get the u track
    QwTrackingTreeLine *wu = uvl[kDirectionU];
    while (wu && nPartialTracks < MAXIMUM_PARTIAL_TRACKS) {
      if (wu->isvoid) { // skip this treeline if it was no good
	wu = wu->next;
	continue;
      }
      // Get wu's line parameters
      double mu = wu->mx; // slope
      double xu = wu->cx; // constant

      // Get the v track
      QwTrackingTreeLine *wv = uvl[kDirectionV];
      while (wv) {
        if (wv->isvoid) {
	  wv = wv->next;
	  continue;
        }
	// Get wv's line parameters
	double mv = wv->mx; // slope
	double xv = wv->cx; // constant

        // Determine u,v at the x detectors
        double u1 = xu + zx1 * mu;
        double u2 = xu + zx2 * mu;
        double v1 = xv + zx1 * mv;
        double v2 = xv + zx2 * mv;

        // Transform u,v to x,y
        Uv2xy uv2xy(region);
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
        double distance, distance1, distance2, minimum = 1e10;
	while (wx) {
	  if (wx->isvoid) {
	    wx = wx->next;
	    continue;
	  }
	  // Get wx's line parameters
	  double mx = wx->mx; // slope
	  double xx = wx->cx; // constant

	  // Coordinates for this treeline in x at intersection
	  double xx1 = xx + mx * zx1;
          double xx2 = xx + mx * zx2;
	  // Difference with x coordinates from u,v treeline track
          distance1 = fabs(x1 - xx1);
          distance2 = fabs(x2 - xx2);
	  distance = distance1 + distance2;

	  // Keep track of best x treeline
          if (distance < minimum) {
	    best_wx = wx;
	    minimum = distance;
	  }
	  wx = wx->next;
	}

	if (best_wx)
	  in_acceptance = InAcceptance(package, region, best_wx->cx, best_wx->mx, y1, my);
	else
	  cerr << "not in acceptance" << endl;

	// Store found partial track (or null)
	QwPartialTrack *pt = TcTreeLineCombine(wu, wv, best_wx, tlayer);

        if (minimum < MaxXRoad && best_wx && in_acceptance) {
	  if (pt) {

	    nPartialTracks++;

	    best_wx->isused = wv->isused = wu->isused = true;

            pt->next = pt_next;
	    pt->bridge = 0;
	    pt->pathlenoff = 0;
	    pt->pathlenslo = 0;
	    pt_next = pt;

	    parttracklist.push_back(pt);

	  }
	} else {
          if (fDebug) cout << "not close enough " << minimum << ',' << MaxXRoad << ',' << in_acceptance << endl;
        }
        wv = wv->next;
      }
      wu = wu->next;
    }

    if (nPartialTracks >= MAXIMUM_PARTIAL_TRACKS)
      std::cout << "Wow, that's a lot of partial tracks!" << std::endl;

    return pt_next;


  //#################
  // OTHER REGIONS  #
  //#################

  } else {

    cerr << "[QwTrackingTreeCombine::TlTreeCombine] Error: ";
    cerr << "Region " << region << " and type " << type << "not supported!";
    cerr << endl;

    // The legacy code which used to be here was removed after r280. (wdc)

  }

  return pt_next;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingTreeCombine::ResidualWrite (QwEvent* event)
{
  cerr << "This function may need a 'for loop' for orientation" << endl;

  EQwDetectorPackage package;
  EQwRegionID region;
  EQwDirectionID dir;
  EQwDetectorType type;
  int allmiss, num;
  QwTrack *tr;
  QwPartialTrack *pt;
  QwTrackingTreeLine *tl;
  double x, y, v, mx, my, mv;
  QwHit **hitarr, *hit;
  void mcHitCord( int, double* , double *);

  for (package = kPackageUp; package <= kPackageDown; package++) {

    for (tr = event->track[package]; tr; tr = tr->next) {
      //type = tr->type;
      for (region = kRegionID1; region <= kRegionID3; region++) {
	/*
	if( region == f_front)
	  pt = tr->front;
	else
	  pt = tr->back;

	allmiss = (pt->tline[kDirectionX]->nummiss +
		   pt->tline[kDirectionU]->nummiss +
		   pt->tline[kDirectionV]->nummiss);
	*/
	for (dir = kDirectionX; dir <= kDirectionV; dir++) {
	  // TODO pt is undefined here
	  tl = pt->tline[dir];
	  hitarr = tl->hits;

	  for (num = MAXHITPERLINE*DLAYERS; num--&&*hitarr; hitarr++) {
	    hit = *hitarr;
	    if (hit->isused) {
	      // x = pt->mx*( hit->Zpos - MAGNET_CENTER ) + pt->x;
	      // y = pt->my*( hit->Zpos - MAGNET_CENTER ) + pt->y;
	      x = pt->mx*( hit->GetZPos() - MAGNET_CENTER ) + pt->x;
	      y = pt->my*( hit->GetZPos() - MAGNET_CENTER ) + pt->y;

	      //mcHitCord( hit->mcId, &mx, &my);
	      switch (dir) {
	        case kDirectionU: v = xy2u( x,y ); mv = xy2u(mx,my); break;
	        case kDirectionV: v = xy2v( x,y ); mv = xy2v(mx,my); break;
	        case kDirectionX: v = x; mv = mx; break;
	        default:   mv = v = 0; break;
	      }
	      printf("%d %d %d %d %d "
		     "%d "
		     "%f "
		     "%f %f %f "
		     "%f %f %f %f "
		     "%d %d %f %f %f "
		     "%f "
		     "XXptresXX\n",
		     hit->detec->ID, type, package, region,dir,
		     hit->GetElement(),
		     // 0.5*(hit->rPos1+hit->rPos2),
		     0.5*(hit->GetDriftDistance()+hit->rPos2),
		     hit->rPos, v, hit->rResultPos,
		     tl->chi, pt->chi,x, y,
		     tl->nummiss, allmiss, mx, my, mv,
		     tr->P);
	    }
	  }
	}
      }
    }
  }
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*------------------------------------------------------------------------*//*!

 This method checks whether the region 3 partial track has an intersection
 with the trigger scintillators or the Cerenkov bars in a detector package.

 The trigger scintillators are assumed to be the only (or at least the first)
 detectors in region 3 that are oriented in direction X.

 The cerenkov bars are assumed to be the only (or at least the first)
 detectors in region 3 that are oriented in direction Y.

 Geometric acceptance is calculated from the centers and widths in the
 geometry definition.  Tracks are drawn straight from region 3.

*//*-------------------------------------------------------------------------*/
int QwTrackingTreeCombine::checkR3 (QwPartialTrack *pt, EQwDetectorPackage package)
{
  double trig[3], cc[3];
  double lim_trig[2][2], lim_cc[2][2];

  // Get the trig scintillator
  Det *rd = rcDETRegion[package][kRegionIDTrig][kDirectionX];

  //get the point where the track intersects the detector planes
  trig[2] = rd->Zpos;
  trig[0] = pt->mx * trig[2] + pt->x;
  trig[1] = pt->my * trig[2] + pt->y;
  //get the detector boundaries
  lim_trig[0][0] = rd->center[0] + rd->width[0]/2;
  lim_trig[0][1] = rd->center[0] - rd->width[0]/2;
  lim_trig[1][0] = rd->center[1] + rd->width[1]/2;
  lim_trig[1][1] = rd->center[1] - rd->width[1]/2;

  if (trig[0] < lim_trig[0][0]
   && trig[0] > lim_trig[0][1]
   && trig[1] < lim_trig[1][0]
   && trig[1] > lim_trig[1][1]) {
	pt->triggerhit = 1;
	pt->trig[0]    = trig[0];
	pt->trig[1]    = trig[1];
	cout << "Trigger scintillator hit at : (" << trig[0] << "," << trig[1] << "," << trig[2] << ")" << endl;
  } else pt->triggerhit = 0;

  // Get the Cherenkov detector
  rd = rcDETRegion[package][kRegionIDCer][kDirectionY];
  cc[2] = rd->Zpos;
  cc[0] = pt->mx * cc[2] + pt->x;
  cc[1] = pt->my * cc[2] + pt->y;
  lim_cc[0][0] = rd->center[0] + rd->width[0]/2;
  lim_cc[0][1] = rd->center[0] - rd->width[0]/2;
  lim_cc[1][0] = rd->center[1] + rd->width[1]/2;
  lim_cc[1][1] = rd->center[1] - rd->width[1]/2;

  if (cc[0] < lim_cc[0][0]
   && cc[0] > lim_cc[0][1]
   && cc[1] < lim_cc[1][0]
   && cc[1] > lim_cc[1][1]) {
	pt->cerenkovhit = 1;
	pt->cerenkov[0] = cc[0];
	pt->cerenkov[1] = cc[1];
	cout << "Cerenkov bar hit at : (" << cc[0] << "," << cc[1] << "," << cc[2] << ")" << endl;
  } else pt->cerenkovhit = 0;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
