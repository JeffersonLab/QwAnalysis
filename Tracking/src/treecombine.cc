/*------------------------------------------------------------------------*//*!

 \class treecombine

 \brief Combines track segments and performs line fitting.

 \author Burnham Stokes
 \date   7/30/08

    Treecombine performs many of the tasks involved with matching hits to track
    segments and combining track segments into full tracks with lab coordinates.

*//*-------------------------------------------------------------------------*/

#include "treecombine.h"

// Standard C and C++ headers
#include <fstream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <cassert>
#include <cstring>
using namespace std;

// Qweak headers
#include "Hit.h"
#include "Det.h"
#include "options.h"

// Qweak tree object headers
#include "treeregion.h"
using namespace QwTracking;

// Other Qweak modules
#include "QwTrackingTreeSort.h"

#define PI 3.141592653589793
#define DBL_EPSILON 2.22045e-16


extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
extern treeregion *rcTreeRegion[kNumPackages][kNumRegions][kNumTypes][kNumDirections];
extern Options opt;

double UNorm( double *A, int n, int m);
double *M_Cholesky (double  *B, double *q, int n);
double *M_InvertPos (double *B, int n);
double *M_Invert (double *Ap, double *Bp, int n);
void RowMult (double a, double *A, double b, double *B, int n);
double *M_A_times_b (double *y,double *A, int n, int m, double *b);
double *M_Zero (double *A, int n);
double *M_Unit (double *A, int n);
void M_Print (double *A, int n);
void M_Print (double *A, double *B, int n);


treecombine::treecombine()
{
  debug = 0; // debug level
}

treecombine::~treecombine() { }


chi_hash::chi_hash()
{
  hits = 0;
}

//__________________________________________________________________
int treecombine::chi_hashval (int n, Hit **hit)
{
  int i;
  double hash = 389.0; // WTF IS THIS?!?
  for (i = 0; i < n; i++) {
    hash *= hit[i]->rResultPos;
  }
  i = (((((*(unsigned*)&hash))) & HASHMASK)^
       ((((*(unsigned*)&hash))>>22) & HASHMASK)) ;
  return i;
}
//__________________________________________________________________
void treecombine::chi_hashclear(void)
{
  memset( hasharr, 0, sizeof(hasharr));
}

//__________________________________________________________________
void treecombine::chi_hashinsert(Hit **hits, int n, double slope, double xshift, double cov[3],double chi)
{
  int val = chi_hashval( n, hits), i;
//Got rid of a Qmalloc in the following line

    chi_hash *new_chi_hash;
    new_chi_hash = (chi_hash*)malloc(sizeof(chi_hash));
  //chi_hash *new = (chi_hash*)malloc( sizeof(chi_hash));
  if( new_chi_hash ) {
    new_chi_hash->next = hasharr[val];
    hasharr[val] = new_chi_hash;
    new_chi_hash->x  = xshift;
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
//__________________________________________________________________
int treecombine::chi_hashfind( Hit **hits, int n, double *slope, double *xshift, double cov[3],double *chi)
{
  int val = chi_hashval( n, hits), i;
  chi_hash *new_chi_hash;
  new_chi_hash = new chi_hash;
//  new_chi_hash = (chi_hash*)malloc(sizeof(chi_hash));
  for( new_chi_hash = hasharr[val]; new_chi_hash; new_chi_hash = new_chi_hash->next) {
    if(!new_chi_hash->hits)break;
    if( new_chi_hash->hits == n ) {
      for( i = 0; i < n; i++ ){
	if( new_chi_hash->hit[i] != hits[i]->rResultPos ){
	  break;
	}
      }
      if( i == n ) {		/* HIT! */
	*xshift = new_chi_hash->x;
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
//__________________________________________________________________

/*-------------------------------
   BESTX : This is a much simplified
   version of bestx.  It simply
   matches the left/right wire hits
   in r3 to the pattern being used.
--------------------------------*/
int treecombine::bestx (double *xresult, Hit *h, Hit *ha)
{
  double pos, distance, bestx, resolution;
  double x = *xresult;
  pos = h->rPos1;
  resolution = h->Resolution;
  distance = x - pos;
  if (distance < 0) distance = -distance;
  bestx = pos;
  //cerr << "d1= " << distance << endl;
  pos = -h->rPos1;
  distance = x - pos;
  if (distance < 0) distance = -distance;
  if (distance < bestx) {
    bestx = pos;
  }
  *ha = *h;
  ha->next    = 0;
  ha->nextdet = 0;
  ha->used    = false;
  ha->rResultPos =
        ha->rPos = bestx;
  ha->Resolution = resolution;

  return 1;
}
//__________________________________________________________________

/* ------------------------------
   .bestx
   BESTX : searches for the closed hit to
   to *xresult and writes it
   to *xresult. In addition the
   hitchain is searched for hits
   inbetween *xresult - dist_cut
   and *xresult + dist_cut. All
   corresponding hits that
   are found are stored in *ha

   Size of the arrays must be
   MAXHITPERLINE
   ------------------------------ */
int treecombine::bestx (
	double *xresult,
	double dist_cut,
	Hit *h,
	Hit **ha,
	double Dx)
{
//##############
//DECLARATIONS #
//##############
  int good = 0;
  double x = *xresult;
  double pos;
  double adist, distance, odist;
  double minim = dist_cut;
  int  igood, j, doub;
  double breakcut = 0;
  double wirespace = h->detec->WireSpacing; //assumes the same wirespacing for each R2 plane


//###############################################
//Find good hits up to a max of MAXHITPERLINE.  #
//Uses a hard maximum distance cut 'dist_cut'.  #
//If MAXHITPERLINE good hits are reached,       #
//replace good hits with better hits.           #
//###############################################
  if( h )
    breakcut = h->detec->WireSpacing + dist_cut;//wirespacing + bin resolution

  while( h ) {
    doub = 0;
    for( j = 0; j < 2; j++ ) {
      pos        = j ? (h->wire+1)*wirespace + h->rPos1 + Dx: (h->wire+1)*wirespace - h->rPos1 + Dx;
      distance   = x - pos;
      adist      = fabs(distance);
      if( adist < dist_cut) {//<- distance cut applied
	if( good < MAXHITPERLINE ) {
	  /* --- doublicate hits for calibration changes --- */
	  ha [good]   = (Hit*)malloc( sizeof(Hit));
	  assert( ha[good] );
	  *ha[good]  = *h;
	  ha[good]->next    = 0;
	  ha[good]->nextdet = 0;
	  ha[good]->used    = false;
	  ha[good]->rResultPos =
	    ha[good]->rPos    = pos;
          ha[good]->Zpos = h->detec->Zpos;
	  if( adist < minim ) {
	    *xresult = pos;
	    minim = adist;
	  }
	  good++;
	} else {		/* try to save it if better than others */
	  for( igood = 0; igood < good; igood ++ ) {
	    odist = x - ha[igood]->rPos;
	    if( odist < 0 )
	      odist = -odist;
	    if( adist < odist ) {
	      *ha[igood]   = *h;
	      ha[igood]->next    = 0;
	      ha[igood]->nextdet = 0;
	      ha[igood]->used    = false;
	      ha[igood]->rResultPos =
		ha[igood]->rPos    = pos;
	      break;
	    }
	  }
	}
      }
      if( pos == h->rPos2 )
	break;
    }
    if( distance < -breakcut)
      break;
    h = h->nextdet;
  }
  return good;
}
//__________________________________________________________________



/* --------------------------------------------------------------------------
 * creates permutations of hits
 * i   : a number from 0 to mul-1 (number of permutation)
 * mul : the number of permutations
 * l   : the length of r[l], bx[l][MAXHITPERLINE], xa[l]
 * r   : array of multiplicity in detectors
 * hx  : hits filled: ha[l][r[]]
 * ha  : array to write permutated hits in
 * ------------------------------------------------------------------------- */

void treecombine::mul_do (int i, int mul, int l, int *r, Hit *hx[DLAYERS][MAXHITPERLINE], Hit **ha)
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
//__________________________________________________________________
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

void treecombine::weight_lsq (
	double *slope,
	double *xshift,
	double cov[3],
	double *chi,
	Hit **hits,
	int n,
	int tlayers)
{
  double r, s, det, h;
  double A[tlayers][2],G[tlayers][tlayers],AtGA[2][2];
  double AtGy[2], y[tlayers], x[2];

  for (int i = 0; i < tlayers; i++)
    A[i][0] = -1.0;

  //##########
  //SET HITS #
  //##########

  for (int i = 0; i < n; i++) {
    A[i][1] = -(hits[i]->Zpos);
    y[i]     = -hits[i]->rResultPos;
    r = 1.0 / hits[i]->Resolution;
    G[i][i] = r*r;
  }
  /* calculate right hand side */
  for (int i = 0; i < 2; i++) {
    s = 0;
    for (int k = 0; k < n; k++)
      s += A[k][i] * y[k] * G[k][k];
    AtGy[i] = s;
  }
  /* and now the left hand side */
  for (int i = 0; i < 2; i++) {
    for (int k = 0; k < 2; k++) {
      s = 0;
      for (int j = 0; j < n; j++)
	s += G[j][j]*A[j][i]*A[j][k];
      AtGA[i][k] = s;
    }
  }
  /* calculate covariance */
  det = ( AtGA[0][0] * AtGA[1][1] - AtGA[1][0] * AtGA[0][1]);
  h          = AtGA[0][0];
  AtGA[0][0] = AtGA[1][1] / det;
  AtGA[1][1] = h / det;
  AtGA[0][1] /= -det;
  AtGA[1][0] /= -det;
  /* solve equation */
  for (int i = 0; i < 2; i++)
    x[i] = AtGA[i][0] * AtGy[0] + AtGA[i][1] * AtGy[1];

  *slope  = x[1];
  *xshift = x[0];

  cov[0]  = AtGA[0][0];
  cov[1]  = AtGA[0][1];
  cov[2]  = AtGA[1][1];

  /* sqrt( chi^2) */
  for (int i = 0, s = 0.0; i < n; i++) {
    r  = (*slope*(hits[i]->Zpos - magnet_center) + *xshift
	  - hits[i]->rResultPos);
    s  += G[i][i] * r * r;
  }
  *chi   = sqrt( s/n );
  chi_hashinsert(hits,n, *slope, *xshift, cov, *chi);
}
//__________________________________________________________________
/* ----------------------------------------------------------------------
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

void treecombine::weight_lsq_r3(double *slope, double *xshift, double cov[3],double *chi,Hit **hits, int n,double z1, int offset,int tlayers)
{
  double A[tlayers][2],G[tlayers][tlayers],AtGA[2][2];
  double  AtGy[2], y[tlayers], x[2];
  double r, s, det, h;
  double resolution = 0.1;
  /* initialize the matrices and vectors */
  for (int i = 0; i < tlayers; i++)
    A[i][0] = -1.0;

  //###########
  // Set Hits #
  //###########
  for (int i = 0; i < n; i++) {
    if (offset == -1) {
      A[i][1] = -hits[i]->Zpos;//used by matchR3
      y[i] = -hits[i]->rPos;
    } else {
      A[i][1] = -(i+offset);//used by Tl MatchHits
      y[i] = -hits[i]->rResultPos;
    }
    r = 1.0 / hits[i]->Resolution;
    if (!(hits[i]->Resolution)) r = 1.0 / resolution;// WARNING : this is a hack to make this fit work.  Hit resolutions needs to be set up.
    G[i][i] = r * r;
  }
  /* calculate right hand side */
  for (int i = 0; i < 2; i++) {
    s = 0;
    for(int k = 0; k < n; k++)
      s += A[k][i] * y[k] * G[k][k];
    AtGy[i] = s;
  }
  /* and now the left hand side */
  for(int i = 0; i < 2; i++) {
    for(int k = 0; k < 2; k++) {
      s = 0;
      for(int j = 0; j < n; j++)
	s += G[j][j]*A[j][i]*A[j][k];
      AtGA[i][k] = s;
    }
  }
  /* calculate covariance */
  det = ( AtGA[0][0] * AtGA[1][1] - AtGA[1][0] * AtGA[0][1]);
  h          = AtGA[0][0];
  AtGA[0][0] = AtGA[1][1] / det;
  AtGA[1][1] = h / det;
  AtGA[0][1] /= -det;
  AtGA[1][0] /= -det;
  /* solve equation */
  for (int i = 0; i < 2; i++)
    x[i] = AtGA[i][0] * AtGy[0] + AtGA[i][1] * AtGy[1];
  *slope  = x[1];
  *xshift = x[0];
  cov[0]  = AtGA[0][0];
  cov[1]  = AtGA[0][1];
  cov[2]  = AtGA[1][1];
  if (offset == -1) {
    for (int i = 0, s = 0.0; i < n; i++) {
      r  = (*slope*(hits[i]->Zpos - z1) + *xshift
	    - hits[i]->rResultPos);
      s  += G[i][i] * r * r;
    }
  } else {
    for (int i = 0, s = 0.0; i < n; i++) {
      r = *slope*(i+offset) + *xshift - hits[i]->rResultPos;
      s += G[i][i] * r * r;
    }
  }

  *chi   = sqrt( s/n );
  //chi_hashinsert(hits,n, *slope, *xshift, cov, *chi);
}
//__________________________________________________________________
int treecombine::contains( double var, Hit **arr, int len)
{
  for (int i = 0; i < len ; i++) {
    if (var == arr[i]->rResultPos)
      return 1;
  }
  return 0;
}
//__________________________________________________________________
/* ------------------------------
   searches for the closest hit to
   to *xresult and writes it
   to *xresult. In addition the
   hitarray is searched for hits
   in the detector detect and
   inbetween *xresult - dist_cut
   and *xresult + dist_cut. All
   the possible hits are stored
   in *ha

   Size of the arrays must be
   MAXHITPERLINE
   ------------------------------ */


int treecombine::selectx(double *xresult,double dist_cut,Det *detec, Hit *hitarray[], Hit **ha)
{
  int good = 0;
  double x = *xresult;
  double pos;
  double distance;
  double minim = dist_cut;
  Hit *h;
  int num;


  for( num = MAXHITPERLINE*DLAYERS; num--  && *hitarray; hitarray ++ ) {
    h = *hitarray;
//comented out the rightwing/leftwing stuff from the following line
    if( !h->detec ||
	(h->detec != detec/* && h->detec != detec->leftwing &&
	 h->detec != detec->rightwing */)) {
      continue;
    }

    pos        = h->rResultPos;

    if( !contains( pos, ha, good ) ) {
      distance   = x - pos;
      ha [good]   = h;
      if( fabs(distance) < minim ) {
	*xresult = pos;
	minim = fabs(distance);
      }
      good++;
      if( good == MAXHITPERLINE)
	break;
    }
  }
  return good;
}
//__________________________________________________________________
/* ----------------------------------------------------------------------
 * returns the z position of a crossing point of a detector
 * and a line with track parameters x and slope_x
 */
double treecombine::detZPosition( Det *det, double x, double slope_x, double *xval )
{
  cerr << "This function is just a stub" << endl;
  return -9999;
}
//__________________________________________________________________

/*------------------------------------------------------------------------*//*!

 This method checks whether a track with the given parameters is in the
 geometric acceptance of all defined detector planes in the specified
 region and package.  Geometric acceptance is calculated from the centers
 and widths in the geometry definition.

 Returns one if in the acceptance (or always for region 2), returns zero
 if not in the acceptance of at least one detector plane.

*//*-------------------------------------------------------------------------*/
int treecombine::inAcceptance (
	EQwDetectorPackage package,
	EQwRegionID region,
	double cx, double mx,
	double cy, double my )
{

  EQwDirectionID dir;
  double z, x, y,z1;
  Det *rd/*, *rds, *rde*/;
  if (region == kRegionID2) {
    return 1; //don't check R2 acceptance yet
  } else {
    for (dir = kDirectionX; dir <= kDirectionV; dir++) {
      for (rd = rcDETRegion[package][region-1][dir], z1 = rd->Zpos;
	   rd; rd = rd->nextsame) {
        z = rd->Zpos;
        x = cx + (z-z1) * mx;
        y = cy + (z-z1) * my;

        if (rd->center[0] - 0.5*rd->width[0] > x ||
	    rd->center[0] + 0.5*rd->width[0] < x ||
	    rd->center[1] - 0.5*rd->width[1] > y ||
	    rd->center[1] + 0.5*rd->width[1] < y) {
	  return 0;
        }
      }
    }
  }
  return 1;
}
//__________________________________________________________________
/* -------------------------------------------------------------------------
 * TLCHECKFORX :
 * checks for x hits on a line given by values in (RENAMED VARIABLE) and (RENAMED VARIABLE)
 * dlayer: number of detector layers to search in
 * ------------------------------------------------------------------------- */
int treecombine::TlCheckForX (
	double x1,
	double x2,
	double dx1,
	double dx2,
	double Dx,
	double z1,
	double dz,
	QwTrackingTreeLine *treefill,
	EQwDetectorPackage package,
	EQwRegionID region,
	EQwDetectorType type,
	EQwDirectionID dir,
	int dlayer,
	int tlayer,
	int iteration,
	int stay_tuned)
{
  //################
  // DECLARATIONS  #
  //################
  Det *rd, *firstdet = 0, *lastdet;/* detector loop variable                 */
  double org_mx;     		/* slope                                  */
  double org_dmx;		/* the same for the error                 */
  double thisX, thisZ;		/* X and Z at a given plane               */
  double startZ = 0.0, endZ = 0;
  if(DLAYERS<4)cerr << "ERROR : DLAYERS MUST BE >=4 FOR R2" << endl;
  Hit   *DetecHits[DLAYERS][MAXHITPERLINE]; /* Hits at a detector         */
  Hit   *UsedHits[DLAYERS];	            /* Hits for this line         */
  Hit   **hitarray;		/* temporary                              */
  double res,resnow;	        /* resultion of a plane                   */
  double mx,cx,dh,chi,minchi=1e8;
  double minweight = 1e10;
  double mmx=0,mcx=0; /* least square fit parameters     */
  double cov[3], mcov[3];
  double stay_chi;
  int i,j;
  int nMult[DLAYERS];		/* # of hits at a detector                */
  int nhits;			/* number of hits per line                */
  int permutations = 1;		/* multiplicity for permutations          */
  int besti = -1;               /* best permutation index                 */
  int ret   = 0;		/* return value (pessimistic)             */
  int first;
  double MaxRoad = opt.R2maxroad;
  int plane;

  //##################
  //DEFINE VARIABLES #
  //##################
  org_mx  = (x2-x1)/dz;//pattern slope
  org_dmx = (dx2-dx1)/dz; //pattern resolution slope
  res = rcTreeRegion[package][region-1][type][dir]->rWidth /(1<<(opt.levels[package][region-1][type]-1));//bin 'resolution'

  //###################################
  //LOOP OVER DETECTORS IN THE REGION #
  //###################################
  // uses BESTX & SELECTX     #
  //###########################
  /* --- loop over the detectors of the region --- */
  first = 1;
  for (nhits = 0, plane =0, rd = rcDETRegion[package][region-1][dir];
      rd; rd = rd->nextsame, plane++) {
    thisZ = rd->Zpos;
    thisX = org_mx * (thisZ - z1) + x1;

    if (! firstdet) {
      firstdet = rd;
      startZ = thisZ;
    }
    lastdet = rd;
    endZ = thisZ;

    /* --- search this road width for hits --- */
    //set 'resnow'
    resnow =  org_dmx * (thisZ - z1) + dx1;
    if (! iteration && ! stay_tuned && (first || ! rd->nextsame) && tlayer == dlayer)
      resnow -= res * (MaxRoad - 1.0);
    //.bestx finds the hits which occur closest to the path through the first and last detectors
    //nMult is the number of good hits at each detector layer
    if (! iteration) {          /* first track finding process */
      if (plane < 2)
        nMult[nhits] = bestx (&thisX, resnow, rd->hitbydet, DetecHits[nhits]);
      else
        nMult[nhits] = bestx (&thisX, resnow, rd->hitbydet, DetecHits[nhits], Dx);
    }
    else			/* in iteration process (not used by TlTreeLineSort)*/
      nMult[nhits] = selectx (&thisX, resnow, rd, treefill->hits, DetecHits[nhits]);

    if (nMult[nhits]) {         /* there are hits in this detector         */
      permutations *= nMult[nhits];
    } else {
      nhits--;			/* this detector did not fire              */
    }
    nhits++;
  }

  //############################
  //RETURN HITS FOUND IN BESTX #
  //############################
  if (! iteration) {		/* return the hits found in bestx()        */
    hitarray = treefill->hits;
    for (j = 0; j < nhits; j++ ) {
      for (i = 0; i < nMult[j]; i++ ) {
	*hitarray = DetecHits[j][i];
	hitarray ++;
      }
    }
    if (hitarray - treefill->hits < DLAYERS*MAXHITPERLINE + 1)
      *hitarray = 0;		/* add a terminating 0 for later selectx()*/
  }

  //#####################################################
  //DETERMINE THE BEST SET OF HITS FOR THE TREELINE    #
  //#####################################################
  // uses : MUL_DO     #
  //        WEIGHT_LSQ #
  //####################
  int rcSETiMissingTL0 = 2;
  if (nhits >= dlayer - rcSETiMissingTL0/*rcSET.iMissingTL[missidx]*/) {  /* allow missing hits */
    for (i = 0; i < permutations; i++) {
      //mul_do is used to create hit arrays for every possible hit combination with one hit per layer.
      mul_do (i, permutations, nhits, nMult, DetecHits, UsedHits);

      chi = 0.0;

      weight_lsq (&mx, &cx, cov, &chi, UsedHits, nhits, tlayer);

      stay_chi = 0.0;
      if (stay_tuned) {
	dh = (cx + mx * (startZ - magnet_center) -
	     (x1 + org_mx * (startZ - z1)));
	stay_chi += dh*dh;
	dh = (cx + mx * (endZ - magnet_center) -
	     (x1 + org_mx * (endZ - z1)));
	stay_chi += dh * dh;
      }
      if (stay_chi + chi + dlayer-nhits < minweight) {
	/* has this been the best track so far? */
	minweight = stay_chi + chi + dlayer - nhits;
	minchi = chi;
	mmx    = mx;
	memcpy (mcov, cov, sizeof cov);
	mcx    = cx;
	besti  = i;
      }
    }
    //for (j = 0; j < nhits; j++) {
    //  cerr << dir << ',' << UsedHits[j]->Zpos << ',' << UsedHits[j]->rResultPos << endl;
    //}
    //cerr << "line = " << mmx << ',' << mcx << ',' << chi << endl;
    treefill->cx  = mcx;
    treefill->mx  = mmx;     /* return track parameters: x, slope, chi */
    treefill->chi = minchi;
    treefill->numhits = nhits;

    memcpy (treefill->cov_cxmx, mcov, sizeof mcov);

    if (besti != -1) {
      mul_do (besti, permutations,
	      nhits, nMult, DetecHits, UsedHits);
      for (j = 0; j < MAXHITPERLINE*DLAYERS && treefill->hits[j]; j++)
	treefill->hits[j]->used = false;
      for (j = 0; j < nhits; j++) {
	if (UsedHits[j])
	  UsedHits[j]->used = true;
      }
    }
    ret = (besti != -1);
  }

  //################
  //SET PARAMATERS #
  //################
  if (! ret) {
    treefill->isvoid  = true;
    treefill->nummiss = dlayer;
  } else {
    treefill->isvoid  = false;
    treefill->nummiss = dlayer - nhits;
  }
  return ret;
}
//__________________________________________________________________

/* -------------------------------
This function replaces TlCheckForX for region 3.  It matches one of
the two wire hits to the hit in the pattern.  It then calculates a
chi_square for the matching of the hits to the pattern.  This may
be misleading due to the currently low resolution (8 bins) of
the pattern database.  It should however, be able to distinguish the
right/left ambiguity for each wire.

OUTPUT : the best hits are added to treefill's hit list and treefill's
	line parameters are set.
--------------------------------*/
int treecombine::TlMatchHits (
	double x1, double x2,	//!- x coordinates
	double z1,		//!- z coordinate
	double dz,		//!- distance in z coordinate
	QwTrackingTreeLine *treefill,	//!- determined treeline
	EQwDetectorPackage package,	//!- package identifier
	EQwRegionID region,	//!- region identifier
	EQwDetectorType type,		//!- detector type identifier
	EQwDirectionID dir,	//!- wire direction identifier
	int tlayers)		//!- number of tree layers
{
  //################
  // DECLARATIONS  #
  //################
  double thisX, thisZ; /* X and Z for the pattern's wire hit */
  Hit *h;
  Det *rd;
  double dx, slope, intercept;
  Hit  DetecHits[tlayers]; /* Hits at a detector */
  Hit *DHits = DetecHits;

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
  intercept = 0.5*((x2 + x1) + slope*(z1 + z1 + dz));
  intercept = x1 - slope*z1;
  //cerr << "(" << slope << "," << intercept << "," << z1 << "," << x1 << ")" << endl;
  nhits = treefill->lastwire - treefill->firstwire + 1;
  if (nhits < 0) nhits = - nhits;

  //########################
  //MATCH HITS TO TREELINE #
  //########################
  //cerr << "matchhits" << endl;

  //STEP 1 : Match left/right wire hits to the pattern hits
  rd = rcDETRegion[package][region-1][dir];
  if (treefill->r3offset >= 281){//get the correct plane for this treeline
	rd = rd->nextsame;
  }
  // Loop over pattern positions
  for (int i = treefill->firstwire; i <= treefill->lastwire; i++, nmaxhits++) {
    // Loop over the hits (what? again?)
    for (h = rd->hitbydet; h; h = h->next) {
      thisZ = treefill->r3offset + i;
      if (h->wire != thisZ)
        continue;

      thisX = slope * thisZ + intercept;
      //cerr << "Z=" << thisZ << endl;
      bestx (&thisX, h, DHits);
      DHits++;
      j++;
    }
  }

  if (j != nhits) cerr << "WARNING NHITS != NGOODHITS " << nhits << "," << j << endl;

  //############################
  //RETURN HITS FOUND IN BESTX #
  //############################
  for (int i = 0; i < j; i++) {
    DetecHits[i].used = true;
    treefill->thehits[i] = DetecHits[i];
    treefill->hits[i] = &treefill->thehits[i];
  }

  //######################
  //CALCULATE CHI SQUARE #
  //######################
  chi = 0.0;
  weight_lsq_r3( &mx, &cx, cov, &chi,treefill->hits, j,z1,treefill->r3offset,tlayers);
  //################
  //SET PARAMATERS #
  //################
  treefill->cx  = cx;
  treefill->mx  = mx;     /* return track parameters: x, slope, chi */
  treefill->chi = chi;
  treefill->numhits = nhits;
  memcpy(treefill->cov_cxmx, cov, sizeof cov);
/*  if(nhits>=7){//CUT TRACKS MISSING MORE THAN 1 HIT
    ret =1;
  }*/
  ret = 1;  //need to set up a check that the missing hits is not greater than 1.
  if (! ret) {
    treefill->isvoid  = true;
    treefill->nummiss = nmaxhits-nhits;
  } else {
    treefill->isvoid  = false;
    treefill->nummiss = nmaxhits-nhits;
  }
  return ret;
}

/* ------------------------------
   Marks TreeLines close to
   other Lines as being good
   / bad in respect to their
   chi^2
   ------------------------------ */
void treecombine::TlTreeLineSort (
	QwTrackingTreeLine *tl,
	EQwDetectorPackage package,
	EQwRegionID region,
	EQwDetectorType type,
	EQwDirectionID dir,
	unsigned long bins,
	int tlayer,
	int dlayer)
{
  double z1, z2;
  double x1, x2, dx;

  QwTrackingTreeLine *walk = tl;
  QwTrackingTreeLine *owalk;
  chi_hashclear();

  /* Region 3 */
  if (region == kRegionID3) {

    /* --------------------------------------------------
       calculate line parameters first
       -------------------------------------------------- */
    for (walk = tl; walk; walk = walk->next) { // for each matching treeline
      if (! walk->isvoid) {
        if (dlayer == 1) {
	  walk->r3offset += rcDETRegion[package][region-1][dir]->NumOfWires;
        }
        z1 = (double) (walk->r3offset + walk->firstwire); // first z position
        z2 = (double) (walk->r3offset + walk->lastwire);  // last z position
        dx = rcTreeRegion[package][region-1][type][dir]->rWidth;
        dx /= (double) bins;

        x1 = (walk->a_beg - (double) bins/2) * dx + dx/2;
        x2 = (walk->b_end - (double) bins/2) * dx + dx/2;
        TlMatchHits (x1, x2, z1, z2-z1, walk, package, region, type, dir, TLAYERS);
      }
    }


  /* Region 2 */
  } else if (region == kRegionID2) {

    Det *rd;
    double dxh, dxb, dx_2;
    double dx1, dx2, Dx;

    int i;
    for (rd = rcDETRegion[package][region-1][dir], i = 0;
         rd && i < tlayer; rd = rd->nextsame, i++) {
      if (i == 0) {
        z1 = rd->Zpos; // first z position
        Dx = -rd->center[1]; // radial position of DC
      }
      if (i == tlayer-1) {
        z2 = rd->Zpos; // last z position
        Dx += rd->center[1]; // difference in radial positions of two DCs.
        Dx *= fabs(rd->rCos);
      }
    }

    dx  = rcTreeRegion[package][region-1][type][dir]->rWidth; // detector width
    dxh = 0.5 * dx; // detector half-width
    dx /= (double) bins; // width of each bin
    dxb = dxh / (double) bins; // half-width of each bin

    /* --------------------------------------------------
       calculate line parameters first
       -------------------------------------------------- */
    double MaxRoad = opt.R2maxroad;///how large to make the 'road' along a track in which hits will be considered.

    for (walk = tl; walk; walk = walk->next) { // for each matching treeline
      if (! walk->isvoid) {
        dx_2 =  dxh - dxb;
        x1 = 0.5*((walk->a_beg + walk->a_end) * dx);
        x2 = 0.5*((walk->b_beg + walk->b_end) * dx) + Dx;
//cerr << "(x1,x2,z1,z2) = (" << x1 << ',' << x2 << ',' << z1 << ',' << z2 << ")" << endl;
        dx1 = ((walk->a_end - walk->a_beg) * dx) + dx * MaxRoad;
        dx2 = ((walk->b_end - walk->b_beg) * dx) + dx * MaxRoad;
//cerr << "Dx : " << Dx << ',' << dir << endl;
        TlCheckForX(x1, x2, dx1, dx2, Dx, z1, z2-z1, walk, package, region, type, dir, tlayer, tlayer, 0, 0);
      }
    }


  /* Other regions */
  } else
    cerr << "Warning: treecombine not implemented for region " << region << endl;

  /* End of region-specific parts */


  /* --------------------------------------------------
     now sort again for identical treelines
     -------------------------------------------------- */
  for (walk = tl; walk; walk = walk->next) {
    if (walk->isvoid == false) {
      for (owalk = walk->next; owalk; owalk = owalk->next) {
        if (owalk->isvoid == false
	 && walk->cx == owalk->cx
	 && walk->mx == owalk->mx) {
	  owalk->isvoid = true;
        }
      }
    }
  }

  /* Sort tracks */
  QwTrackingTreeSort ts;
  ts.rcTreeConnSort (tl, region);

}

/*!--------------------------------------------------------------------------*\

 r2_TrackFit()

    This function fits a set of Hits in the HDC to a 3-D track.  This
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
int r2_TrackFit (int Num, Hit **Hit, double *fit, double *cov, double *chi)
{
  //###############
  // Declarations #
  //###############
  double A[4][4];	//metric matrix A
  double B[4];		//constants vector
  double cff;		//resolution coefficient
  double r[4];		//factors of elements of the metric matrix A
  double uvx;		//u,v,or x coordinate of the track at a location in z
  double rCos[4],rSin[4];	//the rotation angles for the u,v,x coordinates.
  double x0[4];			//the translational offsets for the u,v,x axes.
  Uv2xy uv2xy;			//u,v to x,y projection class
  double bx[4],mx[4];		//track fit parameters
  EQwDirectionID Dir;		//wire direction enumerator
  Det *rd = rcDETRegion[0][1][3];	//pointer to this detector

  //##################
  // Initializations #
  //##################

  //set the angles for our reference frame
  rCos[1] = uv2xy.R2_xy[0][0];//cos theta u
  rCos[2] = uv2xy.R2_xy[1][0];//cos theta v
  rSin[1] = uv2xy.R2_xy[0][1];//sin theta u
  rSin[2] = uv2xy.R2_xy[1][1];//sin theta v
  rCos[3] = 1;//cos theta x
  rSin[3] = 0;//sin theta x

  //set the offsets for the u,v,x axes
  x0[1] = fabs(uv2xy.R2_offset[0])*rCos[1]+uv2xy.R2_wirespacing;
  x0[2] = fabs(uv2xy.R2_offset[1])*rCos[2]+uv2xy.R2_wirespacing;
  x0[3] = 0;

  //initialize the matrices
  for (int i = 0; i < 4; i++) {
    B[i] = 0;
    for (int j = 0; j < 4; j++)
      A[i][j] = 0;
  }

  // Calculate the metric matrix
  for (int i = 0; i < Num; i++) {
    cff  = 1.0 / Hit[i]->Resolution;
    cff *= cff;
    r[0] = rCos[Hit[i]->detec->dir];
    r[1] = rCos[Hit[i]->detec->dir] * (Hit[i]->Zpos);
    r[2] = rSin[Hit[i]->detec->dir];
    r[3] = rSin[Hit[i]->detec->dir] * (Hit[i]->Zpos);
    for (int k = 0; k < 4; k++) {
      B[k] += cff*r[k]*(Hit[i]->rPos - x0[Hit[i]->detec->dir]);
      for (int j = 0; j < 4; j++)
	A[k][j] += cff * r[k] * r[j];
    }
  }


  //##################
  // Perform the fit #
  //##################

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
  bx[1] = (fit[0]+fabs(uv2xy.R2_offset[0]))*rCos[1] + fit[2]*rSin[1] + uv2xy.R2_wirespacing;
  bx[2] = (fit[0]+fabs(uv2xy.R2_offset[1]))*rCos[2] + fit[2]*rSin[2] + uv2xy.R2_wirespacing;
  bx[3] = fit[0];
  mx[1] = fit[1]*rCos[1]+fit[3]*rSin[1];
  mx[2] = fit[1]*rCos[2]+fit[3]*rSin[2];
  mx[3] = fit[1];

  // Calculate chi^2
  *chi = 0;
  for (int i = 0; i < Num; i++) {
    Dir = Hit[i]->detec->dir;
    cff  = 1.0 / Hit[i]->Resolution;
    cff *= cff;
    uvx  = bx[Dir] + mx[Dir] * (Hit[i]->Zpos);
    *chi += (uvx-Hit[i]->rPos) * (uvx-Hit[i]->rPos) * cff;
  }
  *chi = *chi / (Num - 4);

  // Translate to the lab frame
  fit[0] += rd->center[1];

  return 0;
}
//__________________________________________________________________




int treecombine::r3_TrackFit2( int Num, Hit **Hit, double *fit, double *cov, double *chi)
{
  //###############
  // Declarations #
  //###############
  double B[4];
  double cff;
  double r[4];
  double uvx;
  double rCos[3],rSin[3];
  Uv2xy uv2xy;
  double bx[4],mx[4];
  EQwDirectionID Dir;
  //Det *rd;

  double AA[4][4];
  double *AAp = &AA[0][0];

  for (int i = 0; i < Num; i++) {
    Hit[i]->rPos1 = Hit[i]->rPos;
    Hit[i]->rPos = Hit[i]->Zpos;
    Hit[i]->Zpos = Hit[i]->rPos1;
    //cerr << Hit[i]->Zpos << ',' << Hit[i]->rPos << ',' << Hit[i]->detec->dir << endl;
  }

  //set the angles for our frame
  rCos[1] = -uv2xy.R3_xy[0][0];
  rCos[2] = -uv2xy.R3_xy[1][0];
  rSin[1] = -uv2xy.R3_xy[0][1];
  rSin[2] = -uv2xy.R3_xy[1][1];

  for (int i = 0; i < 4; i++) {	/* reset the matrices to 0 */
    B[i] = 0;
    for(int j = 0; j < 4; j++)
      AA[i][j] = 0;
  }

  //##############################
  // Calculate the metric matrix #
  //##############################
  for (int i = 0; i < Num; i++ ) {
    //cerr << i << " " << Hit[i]->Zpos << " " << Hit[i]->rPos << " ";
    cff  = 1.0/Hit[i]->Resolution;
    cff *= cff;
    r[0] = rCos[Hit[i]->detec->dir];
    r[1] = rCos[Hit[i]->detec->dir]*(Hit[i]->Zpos);
    r[2] = rSin[Hit[i]->detec->dir];
    r[3] = rSin[Hit[i]->detec->dir]*(Hit[i]->Zpos);

    for (int k = 0; k < 4; k++) {
      B[k] += cff * r[k] * (Hit[i]->rPos) * (-1);
      for (int j = 0; j < 4; j++)
	AA[k][j] += cff*r[k]*r[j];
    }
  }

  if (debug) cout << "[treecombine::r3_TrackFit2] Before inversion: AA =" << endl;
  if (debug) M_Print(AAp, cov, 4);

  M_Invert(AAp, cov, 4);

  if (!cov) {
    cerr << "[treecombine::r3_TrackFit2] Inversion failed" << endl;
    return -1;
  }

  if (debug) cout << "[treecombine::r3_TrackFit2] After inversion: AA =" << endl;
  if (debug) M_Print(AAp, cov, 4);

  /* calculate the fit */

  M_A_times_b (fit, cov, 4, 4, B); // fit = matrix cov * vector B
  // cerr << "3" << endl;

  //calculate the line parameters in u,v directions
  bx[1] = uv2xy.xy2u(fit[0],fit[2],Hit[0]->detec->region);
  bx[2] = uv2xy.xy2v(fit[0],fit[2],Hit[0]->detec->region);
  mx[1] = uv2xy.xy2u(fit[1],fit[3],Hit[0]->detec->region);
  mx[2] = uv2xy.xy2v(fit[1],fit[3],Hit[0]->detec->region);

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
    cff  = 1.0 / Hit[i]->Resolution;
    cff *= cff;
    uvx  = bx[Dir] + mx[Dir] * (Hit[i]->Zpos);
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
//__________________________________________________________________




int treecombine::r3_TrackFit( int Num, Hit **hit, double *fit, double *cov, double *chi,double uv2xy[2][2])
{
  //###############
  // Declarations #
  //###############

  Hit xyz[Num];
  double wcov[3],wchi,mx,bx,my,by;
  Hit *chihits[Num];
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
    xyz[i].rPos1=hit[i]->rPos1 * uv2xy[0][0] + hit[i]->rPos2 * uv2xy[0][1];//x
    xyz[i].rPos=xyz[i].rPos1;
    xyz[i].rPos2=hit[i]->rPos1 * uv2xy[1][0] + hit[i]->rPos2 * uv2xy[1][1];//y
    xyz[i].Zpos=hit[i]->rPos;//z

    cerr << "Hit coordinates :" << i << "(" << xyz[i].rPos1 << "," << xyz[i].rPos2 << "," << xyz[i].Zpos << ")" << endl;
    xyz[i].Resolution = 0;
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
  P1[2] = xyz[0].Zpos;
  P1[0] = mx * P1[2] + bx;
  P1[1] = my * P1[2] + by;
  P2[2] = xyz[Num-1].Zpos;
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
//__________________________________________________________________



int treecombine::TcTreeLineCombine2(QwTrackingTreeLine *wu, QwTrackingTreeLine *wv, PartTrack *pt, int tlayer)
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

  Hit **hitarray, *h;
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
  Hit *hits[wu->numhits + wv->numhits];
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
    if (debug) {
      cout << "[treecombine::TcTreeLineCombine2] #tlayer = " << tlayer << endl;
      cout << "[treecombine::TcTreeLineCombine2] #hits   = " << num << endl;
    }

    // Loop over all hits
    for (int i = 0; i < num && *hitarray; i++, hitarray++) {
      h = *hitarray;
      ntotal++;
      if (h->used != 0) {
	hits[hitc] = h;
	hitc++;
      }
    }
  }

  // Perform the fit.
  if (r3_TrackFit2(hitc, hits, fit, covp, &chi)  == -1) {
    fprintf(stderr,"hrc: PartTrack Fit Failed\n");
    return 0;
  }
  cout << "Ntotal = " << ntotal << endl;
  //#########################
  // Record the fit results #
  //#########################
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
  pt->tline[0] = wu;
  pt->tline[1] = wv;


  return 1;
}
//__________________________________________________________________



int treecombine::TcTreeLineCombine(QwTrackingTreeLine *wu,QwTrackingTreeLine *wv,PartTrack *pt, int tlayer )
{
  //###############
  // Declarations #
  //###############
  Hit *hits[tlayer*2], **hitarray, *h;
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
cerr << "a" << endl;
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
        h->rPos1 = h->Zpos;//u

        //z is rPos
      } else {
        if (!uv2xy[0][1]) {
          uv2xy[0][1]=fabs(h->detec->rCos);
	  uv2xy[1][1]=fabs(h->detec->rSin);
	}
        h->rPos1 = m * h->rPos + b;//u
        h->rPos2 = h->Zpos;//v
        //cerr << h->rPos << " " << h->Zpos << endl;
        //z is rPos
      }
      //now each hit has a u,v,z coordinate.
      gnu3 << h->rPos << " " << h->rPos1 << " " << h->rPos2 << endl;
      //string all the hits together
      if(h->used !=0){
        hits[hitc++] = h;
      }
    }
  }
  gnu3.close();
  //########################
  // Perform the track fit #
  //########################
/*  if( rc_TrackFit( hitc, hits, fitp, covp, &chi, 0,0)  == -1) {
    fprintf(stderr,"hrc: PartTrack Fit Failed\n");
    return 0;
  }*/
  if(r3_TrackFit(hitc,hits,fitp,covp,&chi,uv2xy) == -1){
    fprintf(stderr,"hrc: PartTrack Fit Failed\n");
    return 0;
  }
  //#########################
  // Record the fit results #
  //#########################
  pt->x  = fit[0];
  pt->y  = fit[1];
  pt->mx = fit[2];
  pt->my = fit[3];
  pt->isvoid  = false;

  pt->chi = sqrt( chi);
  for (int i = 0; i < 4; i++ )
    for (int j = 0; j < 4; j++ )
      pt->Cov_Xv[i][j] = cov[i][j];

  pt->numhits = wu->numhits + wv->numhits;
  pt->tline[0] = wu;
  pt->tline[1] = wv;

return 1;
}
//__________________________________________________________________
int treecombine::TcTreeLineCombine (
	QwTrackingTreeLine *wu,
	QwTrackingTreeLine *wv,
	QwTrackingTreeLine *wx,
	PartTrack *pt,
	int tlayer)
{
  Hit *hits[DLAYERS*3], **hitarray, *h;
  int hitc, num;
  double cov[4][4];
  double fit[4], chi;

  for (int i = 0; i < 4; i++) fit[i] = 0;

  EQwDirectionID dir;
  double *covp = &cov[0][0];
  int ntotal=0;
  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      cov[i][j] = 0;

  // Put all the hits into one array.
  for (hitc = 0, dir = kDirectionU; dir <= kDirectionX; dir++) {
    switch (dir) {
      case kDirectionU: hitarray = wu->hits; break;
      case kDirectionV: hitarray = wv->hits; break;
      case kDirectionX: hitarray = wx->hits; break;
      default: hitarray = 0; break;
    }

    for (num = MAXHITPERLINE*DLAYERS; num--  && *hitarray; hitarray ++ ) {
      h = *hitarray;
      ntotal++;
      if( h->used != 0 && hitc < DLAYERS*3)
	hits[hitc++] = h;
    }
  }

  // Perform the fit.
  if (r2_TrackFit( hitc, hits, fit, covp, &chi)  == -1) {
    fprintf(stderr,"hrc: PartTrack Fit Failed\n");
    return 0;
  }
  //cerr << "5" << endl;

  //Put the fit parameters into the particle track using the lab frame now
  cout << "Ntotal = " << ntotal << endl;
  pt->x  = fit[2];
  pt->y  = fit[0];
  pt->mx = fit[3];
  pt->my = fit[1];
  pt->isvoid  = false;

  pt->chi = sqrt( chi);

  for (int i = 0; i < 4; i++)
    for (int j = 0; j < 4; j++)
      pt->Cov_Xv[i][j] = cov[i][j];

  pt->nummiss = wu->nummiss + wv->nummiss + wx->nummiss;
  pt->numhits = wu->numhits + wv->numhits + wx->numhits;
  pt->tline[0] = wu;
  pt->tline[1] = wv;
  pt->tline[2] = wx;
  //cerr << "6" << endl;
  return 1;
}
//__________________________________________________________________
double uv2x (double u, double v, double wirecos)
{
  return (u-v)*fabs(wirecos);
}
double uv2y (double u, double v, double wiresin)
{
  return (u+v)*fabs(wiresin);
}
double xy2u (double x, double y)
{
  cerr << "This function is just a stub" << endl;
  return -1000;
}
double xy2v (double x, double y)
{
  cerr << "This function is just a stub" << endl;
  return -1000;
}
//__________________________________________________________________




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
	PartTrack ret: a 3-D track


*/
PartTrack *treecombine::TlTreeCombine (
	QwTrackingTreeLine *uvl[kNumDirections],
	long bins,
	EQwDetectorPackage package,
	EQwRegionID region,
	EQwDetectorType type,
	int tlayer,
	int dlayer)
{
  if (debug) cout << "[treecombine::TlTreeCombine]" << endl;

  //################
  // DECLARATIONS  #
  //################
  QwTrackingTreeLine *wu, *wv, *wx, *bwx, wrx;
  PartTrack *ret = 0, *ta;
  int in_acceptance;
  Uv2xy uv2xy;

  double mu, xu, mv, xv, mx, xx, zx1, zx2, xx1, xx2, z1, z2;
  double x, y, my, v1, v2, u1, u2, y1, y2;
  double x1, x2, d, maxd = 1e10, d1, d2;

  chi_hashclear();

  double uv_dz;// distance between u and v planes
  double Rot; // rotation of planes about the lab x-axis
  Det *rd;
  double wirecos, wiresin, x_[2], y_[2];

  int i;

  //###############
  // DO STUFF     #
  //###############

  //################
  // REGION 3 VDC  #
  //################
  if (region == kRegionID3 && type == kTypeDriftVDC) {

    rd = rcDETRegion[package][region-1][kDirectionU];
    z1 = rd->Zpos;
    rd = rcDETRegion[package][region-1][kDirectionV];
    z2 = rd->Zpos;
    Rot = rd->Rot;
    uv_dz = (z2-z1)/sin(Rot);
    wirecos = rd->rCos;
    wiresin = rd->rSin;

    //###################################
    // Get distance between planes, etc #
    //###################################
    rd = rcDETRegion[package][region-1][kDirectionU];
    x_[0] = rd->center[0];
    y_[0] = rd->center[1];

    rd = rcDETRegion[package][region-1][kDirectionU]->nextsame;
    x_[1] = rd->center[0];
    y_[1] = rd->center[1];

    // distance between first u and last v planes
    d = sqrt(pow(x_[1] - x_[0], 2) + pow(y_[1] - y_[0], 2));


    //#########################
    // Get the u and v tracks #
    //#########################
    // get the u track
    wu = uvl[kDirectionU];
    while (wu) {
      if (wu->isvoid != 0) { // skip this treeline if it was no good
	wu = wu->next;
	continue;
      }
      // get wu's line parameters
      mu = wu->mx; // slope
      xu = wu->cx; // constant

      // get the v track
      wv = uvl[kDirectionV];
      while (wv) {
        if (wv->isvoid != 0) {
	  wv = wv->next;
	  continue;
        }
	// get wv's line parameters
	mv = wv->mx; // slope
      	xv = wv->cx; // constant

	PartTrack *ta = new PartTrack();

	if (TcTreeLineCombine2(wu, wv, ta, tlayer)) {
	    ta->ID = 0;
	    ta->isvoid  = false;
	    //ta->trdcol  = 0;
	    ta->next   = ret;
	    //ta->method = method;
	    ta->bridge = 0;
	    //ta->cluster  = 0;
	    ta->Used     = 0;
	    ta->pathlenoff = 0;
	    ta->pathlenslo = 0;
            ta->next = ret; // string together the
	    ret = ta;       // good tracks
	}

	// Check whether this track went through the trigger and/or
	// the Cerenkov bar.
	checkR3(ta,package);
        wv = wv->next;
      }
      wu = wu->next;
    }
  return ret;
  }

  //################
  // REGION 2 HDC  #
  //################
  if (region == kRegionID2 && type == kTypeDriftHDC) {

    double MaxXRoad = opt.R2maxXroad;
    for (rd = rcDETRegion[package][region-1][kDirectionX], i = 0;
         rd && i < tlayer; rd = rd->nextsame, i++) {
      if (i == 0) {
        zx1 = rd->Zpos; /// first x-plane z position
      }
      if (i == tlayer - 1) {
        zx2 = rd->Zpos; /// last x-plane z position
      }
    }
    //#########################
    // Get the u,v and x tracks #
    //#########################

    // get the u track
    wu = uvl[kDirectionU];
    while (wu) {
      if (wu->isvoid != 0) { // skip this treeline if it was no good
	wu = wu->next;
	continue;
      }
      // get wu's line parameters
      mu = wu->mx; // slope
      xu = wu->cx; // constant

      // get the v track
      wv = uvl[kDirectionV];
      while (wv) {
        if (wv->isvoid != 0) {
	  wv = wv->next;
	  continue;
        }
	// get wv's line parameters
	mv = wv->mx; // slope
	xv = wv->cx; // constant

        // get u/v at the x detectors
        u1 = xu + zx1 * mu;
        u2 = xu + zx2 * mu;
        v1 = xv + zx1 * mv;
        v2 = xv + zx2 * mv;

        // for x
        x1 = uv2xy.uv2x(u1, v1, region);
        x2 = uv2xy.uv2x(u2, v2, region);

        // for y
        y1 = uv2xy.uv2y(u1, v1, region);
        y2 = uv2xy.uv2y(u2, v2, region);

	my = (y2 - y1) / (zx1 - zx2);


	// get the x track
	wx = uvl[kDirectionX];
        bwx = 0;
        maxd = 1e10;
	while (wx) {
	  if (wx->isvoid != 0) {
	    wx = wx->next;
	    continue;
	  }
	  // get wx's line parameters
	  mx = wx->mx; // slope
	  xx = wx->cx; // constant

	  xx1 = xx + mx * zx1;
          xx2 = xx + mx * zx2;
//cerr << "wx : " << mx << "," << xx << endl;
          d1 = fabs(x1 - xx1);
          d2 = fabs(x2 - xx2);
	  d = d1 + d2;

          if (d < maxd) {
	    bwx = wx;
	    maxd = d;
	  }
	  wx = wx->next;
	}


//        cerr << "x : " << bwx->cx << ',' << bwx->mx << endl;
//        cerr << "u : " << wu->cx << ',' << wu->mx << endl;
//        cerr << "v : " << wv->cx << ',' << wv->mx << endl;

//	cerr << "P1(xuv,xx1,u,v,zx,y) = (" << x1 << ',' << xx1 << ',' << u1 << ',' << v1  << ',' << zx1 << ',' << y1 << ")" << endl;
//	cerr << "P2(xuv,xx2,u,v,zx,y) = (" << x2 << ',' << xx2 << ',' << u2 << ',' << v2 <<  ',' << zx2 << ',' << y2 << ")" << endl;
//	cerr << "(mx,bx,mu,bu,mv,bv) = (" << mx << ',' << xx << ',' << mu << ',' << xu << ',' << mv << ',' << xv << ")" << endl;
	PartTrack *ta = new PartTrack();


//cerr << "2" << endl;
	if (bwx) in_acceptance = inAcceptance(package, region, bwx->cx, bwx->mx, y1, my);
	else cerr << "not in acceptance" << endl;
        if (maxd < MaxXRoad && bwx && in_acceptance) {
	  if (TcTreeLineCombine(wu, wv, bwx, ta, tlayer)) {
//cerr << "7" << endl;
	    bwx->Used = wv->Used = wu->Used = 1;
	    ta->ID = 0;
	    ta->isvoid = false;
	    ta->next   = ret;
	    ta->bridge = 0;
	    ta->Used   = 0;
	    ta->pathlenoff = 0;
	    ta->pathlenslo = 0;
            ta->next = ret; // string together the
	    ret = ta;       // good tracks
//cerr << "8";
	  }
	} else {
          cerr << "not close enough " << maxd << ',' << MaxXRoad << ',' << in_acceptance << endl;
        }
//cerr << "3" << endl;
        wv = wv->next;
      }
      wu = wu->next;
    }
//cerr << "9" << endl;
    return ret;

  //#################
  // OTHER REGIONS  #
  //#################
  // (All that follows is useful legacy junk, which should not be reached
  //  in the case of QwTracking because everything is handled above)
  } else {
  cerr << "[treecombine::TlTreeCombine] Error: you shouldn't be here!" << endl;
  double rcSETrMaxXRoad         = 0.25;
  cerr << "Error : Using stub value" << endl;
  wu = uvl[kDirectionU];
  zx1 = rcTreeRegion[package][region-1][type][kDirectionX]->rZPos[0];
  zx2 = rcTreeRegion[package][region-1][type][kDirectionX]->rZPos[tlayer-1];
  while( wu) {
    if( wu->isvoid != 0 ) {
      wu = wu->next;
      continue;
    }
    mu = wu->mx;
    xu = wu->cx;
    wv = uvl[kDirectionV];
    while(wv) {
      if( wv->isvoid != 0 ) {
	wv = wv->next;
	continue;
      }

      mv = wv->mx;
      xv = wv->cx;
      /* --- get u/v at the x detectors --- */
      u1 = xu + (zx1 - magnet_center) * mu;
      u2 = xu + (zx2 - magnet_center) * mu;
      v1 = xv + (zx1 - magnet_center) * mv;
      v2 = xv + (zx2 - magnet_center) * mv;

      /* ------ for x -------- */
      x1 = uv2x( u1, v1,1);
      x2 = uv2x( u2, v2,1);

      /* ------ for y -------- */
      y1 = uv2y( u1, v1,1);
      y2 = uv2y( u2, v2,1);

      my = (y2-y1)/(zx2-zx1);
      y  = 0.5*(y1+y2) + my * (magnet_center - 0.5*(zx1+zx2));

      /* --- for the back region we now loop over the x treelines,
	 too. For the front region there are no x tree lines, yet.
	 they have to be calculated first --- */

      maxd = 1e10;
      if( rcTreeRegion[package][region-1][type][kDirectionX]->searchable != 0){
	wx = uvl[kDirectionX];
	bwx = 0;

	while( wx ) {
	  if( wx->isvoid != 0 ) {
	    wx = wx->next;
	    continue;
	  }
	  mx = wx->mx;
	  x  = wx->cx;
	  xx1 = x + (zx1 - magnet_center) * mx;
	  xx2 = x + (zx2 - magnet_center) * mx;
	  d1 = x1-xx1;
	  if( d1 < 0 ) d1 = -d1;
	  d2 = x2-xx2;
	  if( d2 < 0 ) d2 = -d2;
	  d = d1+d2;
	  if( d < maxd ) {
	    bwx = wx;
	    maxd = d;
	  }
	  wx = wx->next;
	}
      } else {
	/* for the front region we have to find the x treelines now --- */

	//visdir = kDirectionX;

	if( TlCheckForX(x1,x2,-99, rcSETrMaxXRoad,rcSETrMaxXRoad, zx1,zx2-zx1,
			&wrx,package,region,type,kDirectionX,dlayer,tlayer, 0,1)) {
	//replaced a Qmalloc below
	  wx = (QwTrackingTreeLine *)malloc( sizeof( QwTrackingTreeLine));
	  assert( wx );
	  *wx = wrx;

	  bwx = wx;
	  //wx->method = meth_std;
	  wx->next = uvl[kDirectionX]; /* chain the link to the  */
	  uvl[kDirectionX] = wx;       /* event treeline list */
	  maxd  = fabs( x1 - (wx->cx + wx->mx*(zx1-magnet_center)));
	  maxd += fabs( x2 - (wx->cx + wx->mx*(zx2-magnet_center)));
	} else
	  bwx = 0;
      }

      int rcSETiMissingVC0     = 6;
      int rcSETiMissingPT0     = 5;
      cerr << "ERROR: USING STUB VALUE FOR RCSET " << endl;
      if (maxd < 2 * rcSETrMaxXRoad
	  && bwx
	  && rcSETiMissingVC0/*rcSETiMissingVC[0]*/
	  >= bwx->numvcmiss + wu->numvcmiss + wv->numvcmiss
	  && rcSETiMissingPT0/*rcSET.iMissingPT[partmissidx]*/
	  >= bwx->nummiss + wu->nummiss + wv->nummiss) {
	in_acceptance = inAcceptance (package, region, bwx->cx, bwx->mx, y, my);
	if (in_acceptance) {

	  wx = bwx;
	  x  = wx->cx;
	  mx = wx->mx;

	  wx->Used = wu->Used = wv->Used = 1;
	  //visdir = kDirectionX;

	  //Statist[method].PartTracksGenerated[where][part] ++;

	  /* ---- form a new PartTrack ---- */
	  //replaced a Qmalloc below
	  ta = (PartTrack *)malloc( sizeof(PartTrack));


	  assert(ta);

	  if( TcTreeLineCombine( wu, wv, wx, ta, tlayer) ) {
	    ta->ID = 0;
	    ta->isvoid  = false;
	    //ta->trdcol  = 0;
	    ta->next   = ret;
	    //ta->method = method;
	    ta->bridge = 0;
	    //ta->cluster  = 0;
	    ta->Used     = 0;
	    ta->pathlenoff = 0;
	    ta->pathlenslo = 0;
	    ret = ta;
	  }
	}
      }
      wv = wv->next;
    }
    wu = wu->next;
  }
  return ret;
  }
}
//__________________________________________________________________



void treecombine::ResidualWrite (Event* event)
{
  cerr << "This function may need a 'for loop' for orientation" << endl;

  EQwDetectorPackage package;
  EQwRegionID region;
  EQwDirectionID dir;
  EQwDetectorType type;
  //enum Eorientation orient;
  int allmiss, num;
  Track *tr;
  PartTrack *pt;
  QwTrackingTreeLine *tl;
  double x, y, v, mx, my, mv;
  Hit **hitarr, *hit;
  void mcHitCord( int, double* , double *);

  for (package = kPackageUp; package <= kPackageDown; package++) {

    for (tr = event->usedtrack[package]; tr; tr = tr->next) {
      //type = tr->type;
      for (region = kRegionID1; region <= kRegionID3; region++) {
	/*
	if( region == f_front)
	  pt = tr->front;
	else
	  pt = tr->back;

	allmiss = ( pt->tline[0]->nummiss +
		    pt->tline[1]->nummiss +  pt->tline[2]->nummiss);
	*/
	for (dir = kDirectionU; dir <= kDirectionX; dir++) {
	  // TODO pt is undefined here
	  tl = pt->tline[dir];
	  hitarr = tl->hits;

	  for( num = MAXHITPERLINE*DLAYERS; num--&&*hitarr; hitarr ++ ) {
	    hit = *hitarr;
	    if( hit->used ) {
	      x = pt->mx*( hit->Zpos - magnet_center ) + pt->x;
	      y = pt->my*( hit->Zpos - magnet_center ) + pt->y;
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
		     hit->wire,
		     0.5*(hit->rPos1+hit->rPos2),
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
//__________________________________________________________________
/*
matrix  M_Init (int Zeilen, int Spalten)
{

  matrix ret;
  int n;

  n = Zeilen;
  ret = (matrix) calloc (1, 0x10 + Zeilen * (sizeof (vektor) +
					     Spalten * sizeof (double)));
  if(!ret)
	exit(0);
  while (n--)
    *(ret + n) = (vektor) (((char *) ret) +
			   (( (Zeilen * sizeof (vektor)) & ~0xf ) +0x10) +
			   n * Spalten * sizeof (double));

  return ret;
}
*/
//__________________________________________________________________



// Least upper bound standard
double UNorm (double *A, int n, int m)
{
  int counter;
  double help, Max = 0.0;
  double *B = A;
  while (n--) {
    counter = m;
    while (counter--) {
      B = A + n + counter;
      if (Max < (help = fabs (*(B))))
	Max = help;
      }
  }
  return Max;
}
//__________________________________________________________________



/*------------------------------------------------------------------------*//*!

 \fn double *M_Cholesky (double *B, double *q, int n)

 \brief Calculates the Cholesky decomposition of the positive-definite matrix B

*//*-------------------------------------------------------------------------*/
double *M_Cholesky (double *B, double *q, int n)
{
  int i, j, k;
  double sum, min;
  double *C = B;
  double A[n][n];
  double p[n],*pp;

  for(i=0;i<n;i++){
    for(j=0;j<n;j++){
      A[i][j]=*C;
      C++;
    }
  }

  min = UNorm (B, n, n)  * n * DBL_EPSILON;
  for( i = 0; i < n; i++ ) {
    for( j = i; j < n; j++ ) {
      sum = A[i][j];
      for( k = i-1; k >= 0; k-- )
	sum -= A[i][k] * A[j][k];
      if( i == j ) {
	if( sum < min ) {
	  return 0;
	}
	p[i] = sqrt(sum);
      } else
	A[j][i] = sum/p[i];
    }
  }
  C = B;
  pp=q;
  for(i=0;i<n;i++){
    *p = p[i];
    pp++;
    for(j=0;j<n;j++){
      *C = A[i][j];
      C++;
    }
  }
  C = B;
  pp = q;
  return B;
}
//__________________________________________________________________



/*------------------------------------------------------------------------*//*!

 \fn void RowMult(double a, double *A, double b, double *B, int n)

 \brief Calculates a*A + b*B, where both A and B are rows of length n

*//*-------------------------------------------------------------------------*/
void RowMult (double a, double *A, double b, double *B, int n)
{
  for (int i = 0; i < n; i++)
    A[i] = a*A[i] + b*B[i];

  return;
}
//__________________________________________________________________



/*------------------------------------------------------------------------*//*!

 \fn void M_Print (double *A, int n)

 \brief Print matrix A of dimension [n,n] to cout

*//*-------------------------------------------------------------------------*/
void M_Print (double *A, int n)
{
  // Print matrix A
  for (int i = 0; i < n; i++) {
    cout << "[";
    for (int j = 0; j < n; j++) {
      cout << A[i*n+j] << ' ';
    }
    cout << ']' << endl;
  }
  cout << endl;

  return;
}



/*------------------------------------------------------------------------*//*!

 \fn void M_Print (double *A, double *B, int n)

 \brief Print matrices A and B of dimension [n,n] to cout

*//*-------------------------------------------------------------------------*/
void M_Print (double *A, double *B, int n)
{
  // Print matrices A and B
  for (int i = 0; i < n; i++) {
    cout << "[";
    for (int j = 0; j < n; j++) {
      cout << A[i*n+j] << ' ';
    }
    cout << '|' ;
    for (int j = 0; j < n; j++) {
      cout << B[i*n+j] << ' ';
    }
    cout << ']' << endl;
  }
  cout << endl;

  return;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_Unit (double *A, int n)

 \brief Creates unit matrix A

*//*-------------------------------------------------------------------------*/
double *M_Unit (double *A, int n)
{
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      A[i*n+j] = 0;
    }
    A[i*n+i] = 1;
  }

  return A;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_Zero (double *A, int n)

 \brief Creates zero matrix A

*//*-------------------------------------------------------------------------*/
double *M_Zero (double *A, int n)
{
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      A[i*n+j] = 0;
    }
  }

  return A;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_Invert (double *Ap, double *Bp, int n)

 \brief Matrix inversion of Ap will be stored in Bp

*//*-------------------------------------------------------------------------*/
double *M_Invert (double *Ap, double *Bp, int n)
{
  double *row1, *row2;
  double a, b;

  // Initialize B to unit matrix (wdc: assumed n = 4 here)
  M_Unit (Bp, n);

  // Print matrices Ap and Bp
  //M_Print (Ap, Bp, n);

  // Calculate inverse matrix using row-reduce method
  // (wdc: TODO  faster algs could be useful later)
  if(n == 4) { // this will not be generalized for nxn matrices.

    // get A10 to An0 to be zero.
    for (int i = 1; i < n; i++) {
      a = -(Ap[0]);
      b = Ap[i*n];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[0]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[0]);
      RowMult(a, row1, b, row2, 4);
    }

    // get A21 and A31 to be zero
    for (int i = 2; i < n; i++) {
      a = -(Ap[5]);
      b = Ap[i*n+1];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[4]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[4]);
      RowMult(a, row1, b, row2, 4);
    }

    // get A32 to be zero
    a = -(Ap[10]);
    b = Ap[14];
    row1 = &(Ap[12]);
    row2 = &(Ap[8]);
    RowMult(a, row1, b, row2, 4);
    row1 = &(Bp[12]);
    row2 = &(Bp[8]);
    RowMult(a, row1, b, row2, 4);

    // Now the matrix is upper right triangular.

    // row reduce the 4th row
    a = 1/(Ap[15]);
    row1 = &(Ap[12]);
    RowMult(a, row1, 0, row2, 4);
    row1 = &(Bp[12]);
    RowMult(a, row1, 0, row2, 4);

    // get A03 to A23 to be zero
    for (int i = 0; i < n-1; i++) {
      a = -(Ap[15]);
      b = Ap[i*n+3];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[12]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[12]);
      RowMult(a, row1, b, row2, 4);
    }

    // row reduce the 3rd row
    a = 1/(Ap[10]);
    row1 = &(Ap[8]);
    RowMult(a, row1, 0, row2, 4);
    row1 = &(Bp[8]);
    RowMult(a, row1, 0, row2, 4);

    // get A02 and A12 to be zero
    for (int i = 0; i < 2; i++) {
      a = -(Ap[10]);
      b = Ap[i*n+2];
      row1 = &(Ap[i*n]);
      row2 = &(Ap[8]);
      RowMult(a, row1, b, row2, 4);
      row1 = &(Bp[i*n]);
      row2 = &(Bp[8]);
      RowMult(a, row1, b, row2, 4);
    }

    // row reduce the 2nd row
    a = 1/(Ap[5]);
    row1 = &(Ap[4]);
    RowMult(a, row1, 0, row2, n);
    row1 = &(Bp[4]);
    RowMult(a, row1, 0, row2, n);

    // get A01 to be zero
    a = -(Ap[5]);
    b = Ap[1];
    row1 = &(Ap[0]);
    row2 = &(Ap[4]);
    RowMult(a, row1, b, row2, n);
    row1 = &(Bp[0]);
    row2 = &(Bp[4]);
    RowMult(a, row1, b, row2, n);

    // row reduce 1st row
    a = 1/(Ap[0]);
    row1 = &(Ap[0]);
    RowMult(a, row1, 0, row2, n);
    row1 = &(Bp[0]);
    RowMult(a, row1, 0, row2, n);
  }

  // Print matrices Ap and Bp
  //M_Print (Ap, Bp, n);

  return Bp;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_InvertPos (double *B, int n)

 \brief Calculates a*A + b*B, where both A and B are rows of length n

*//*-------------------------------------------------------------------------*/
double *M_InvertPos (double *B, int n)
{
  double sum;
  double p[n];
  double q[n];
  double inv[n][n];
  double *pp = NULL;
  double A[n][n],*C;

  // First we find the cholesky decomposition of B
  if (M_Cholesky(B, pp, n)) {

    C = B;
    for (int i = 0; i < n; i++) {
      p[i] = *q;
      for (int j = 0; j < n; j++ ) {
        A[i][j] = *C;
        C++;
      }
    }

    for (int i = 0; i < n; i++) {
      A[i][i] = 1.0 / p[i];
      for (int j = i+1; j < n; j++) {
	sum = 0;
	for (int k = i; k < j; k++)
	  sum -= A[j][k] * A[k][i];
	A[j][i] = sum / p[j];
      }
    }

    for (int i = 0; i < n; i++) {
      for (int j = i; j < n; j++) {
	sum = 0;
	for (int k = j; k < n; k++)
	  sum += A[k][i] * A[k][j];
	inv[i][j] = inv[j][i] = sum;
      }
    }

    C = B;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < n; j++) {
	*C = inv[i][j];
        C++;
      }
    }

  } else {
    B = 0;
    cerr << "Cholesky decomposition failed!" << endl;
  }

  C = B;

  return B;
}



/*------------------------------------------------------------------------*//*!

 \fn double *M_A_times_b (double *y, double *A, int n, int m, double *b)

 \brief Calculates y[n] = A[n,m] * b[m], with dimensions indicated

*//*-------------------------------------------------------------------------*/
double *M_A_times_b (double *y, double *A, int n, int m, double *b)
{
  for (int i = 0; i < n; i++) {
    y[i] = 0;
    for (int j = 0; j < m; j++) {
      y[i] += A[i*n+j] * b[j];
    }
  }
  return y;
}
//__________________________________________________________________


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
int treecombine::checkR3 (PartTrack *pt, EQwDetectorPackage package)
{
  double trig[3], cc[3];
  double lim_trig[2][2], lim_cc[2][2];

  // Get the trig scintillator: the only detector with X direction in region 3
  // TODO (wdc) This is so unsafe, it would never pass JLab scrutiny!
  Det *rd = rcDETRegion[package][kRegionID3-1][kDirectionX];

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

  // Get the Cherenkov detector: the only detector with Y direction in region 3
  // TODO (wdc) This is so unsafe, it would never pass JLab scrutiny!
  rd = rcDETRegion[package][kRegionID3-1][kDirectionY];
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
