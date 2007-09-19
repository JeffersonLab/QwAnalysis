#include <iostream>
#include "treecombine.h"
#include <math.h>
#include "mathstd.h"
#include "mathdefs.h"


using namespace std;

extern Det *rcDETRegion[2][3][4];
extern treeregion *rcTreeRegion[2][3][4][4];
extern int parttrackanz;

extern EUppLow operator++(enum EUppLow &rs, int );
extern ERegion operator++(enum ERegion &rs, int );
extern Etype operator++(enum Etype &rs, int );
extern Edir operator++(enum Edir &rs, int );
/*extern Eorientation operator++(enum Eorientation &rs, int );*/

matrix  M_Init (int Zeilen, int Spalten);
//__________________________________________________________________
treecombine::treecombine(){

}//__________________________________________________________________
chi_hash::chi_hash(){
  hits = 0;
}
//__________________________________________________________________

treecombine::~treecombine(){

}
//__________________________________________________________________
int treecombine::chi_hashval( int n, Hit **hit ){
  int i;
  double hash = 389.0;//WTF IS THIS?!?
  for( i = 0; i < n; i++ ) {
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
int treecombine::bestx(double *xresult,Hit *h,Hit **ha,int i){
	double pos,distance,bestx;
	double x = *xresult;

	pos = h->rPos1;
	distance = x - pos;
	if(distance < 0)distance = -distance;
	bestx = pos;
	//cerr << "d1= " << distance << endl;
	pos = -h->rPos1;
	distance = x - pos;
	if(distance < 0)distance = -distance;
	if(distance < bestx){
		bestx = pos;
	}
        //cerr << "d2= " << distance << endl;
	//cerr << "bestx= " << bestx << endl;
	ha[i]   = (Hit*)malloc( sizeof(Hit));
	assert( ha[i] );
	*ha[i]  = *h;
	ha[i]->next    = 0;
	ha[i]->nextdet = 0;
	ha[i]->used    = false;
	ha[i]->rResultPos = 
	  ha[i]->rPos    = bestx;

return 1;
}
//__________________________________________________________________

/* ------------------------------
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
int treecombine::bestx(double *xresult, double dist_cut, Hit *h, Hit **ha){
//##############
//DECLARATIONS #
//##############
cerr << "1";
  int good = 0;
  double x = *xresult;
  double pos;
  double adist, distance, odist;
  double minim = dist_cut;
  int  igood, j, doub;
  double breakcut = 0;

//###############################################
//Find good hits up to a max of MAXHITPERLINE.  #  
//Uses a hard maximum distance cut 'dist_cut'.  #
//If MAXHITPERLINE good hits are reached,       #
//replace good hits with better hits.           #
//###############################################
cerr << "2";
  if( h )
    breakcut = h->detec->WireSpacing + dist_cut;//wirespacing + bin resolution
  
cerr << "3";
  while( h ) {
    doub = 0;
    for( j = 0; j < 2; j++ ) {
	//cerr << "x=" << x << endl;
      pos        = j ? h->rPos2 : h->rPos1;
      distance   = x - pos;
      adist      = distance < 0 ? -distance : distance;
      if( adist < dist_cut) {//<- distance cut applied
	if( good < MAXHITPERLINE ) {
	  /* --- doublicate hits for calibration changes --- */
	//REMOVED A QMALLOC IN FOLLOWING LINE
	  ha [good]   = (Hit*)malloc( sizeof(Hit));
	  assert( ha[good] );
	  *ha[good]  = *h;
	  ha[good]->next    = 0;
	  ha[good]->nextdet = 0;
	  ha[good]->used    = false;
	  ha[good]->rResultPos = 
	    ha[good]->rPos    = pos;
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

void treecombine::mul_do(int i,int mul,int l,int *r,Hit   *hx[DLAYERS][MAXHITPERLINE],  Hit **ha){
  int j,s;
  
  if( i == 0 ) {
    for( j = 0; j < l; j++ ) {
      ha[j] = hx[j][0];
    }
  } else {
    for( j = 0; j < l; j++ ) {
      if( r[j] == 1 )
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

void treecombine::weight_lsq(double *slope, double *xshift, double cov[3],double *chi,Hit **hits, int n){
  cerr << "weight_lsq begin " << endl;
  static matrix A = 0, G, At, GA, AtGA;
  double  AtGy[2], y[DLAYERS], x[2];
  double r, s, sg, det, h;
  int i,j,k;

  if( !A ) {/* allocate matrices */
    A      = M_Init( DLAYERS, 2);
    G      = M_Init( DLAYERS, DLAYERS);
    At     = M_Init( 2, DLAYERS);
    GA     = M_Init( DLAYERS, 2);
    AtGA   = M_Init( 2, 2);
    for( i = 0; i < DLAYERS; i++ ) {
      At[0][i] = A[i][0] = -1.0;
    }
  }
  if( chi_hashfind(hits ,n, slope, xshift, cov, chi) )
    return;
  /* initialize the matrices and vectors */
  for( i = 0; i < n; i++ ) {
    A[i][1] = -(hits[i]->Zpos - magnet_center);
    y[i]     = -hits[i]->rResultPos;
    r = 1.0 / hits[i]->Resolution;
    G[i][i] = r*r;
  }
  /* calculate right hand side */
  for( i = 0; i < 2; i++ ) {
    s = 0;
    for( k = 0; k < n; k++ )
      s += A[k][i] * y[k] * G[k][k];
    AtGy[i] = s;
  }  
  /* and now the left hand side */
  for( i = 0; i < 2; i++ ) {
    for( k = 0; k < 2; k++ ) {
      s = 0;
      for( j = 0; j < n; j++ )
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
  for( i = 0; i < 2; i++ ) 
    x[i] = AtGA[i][0] * AtGy[0] + AtGA[i][1] * AtGy[1];
  
  *slope  = x[1];
  *xshift = x[0];
  cov[0]  = AtGA[0][0];
  cov[1]  = AtGA[0][1];
  cov[2]  = AtGA[1][1];

  /* sqrt( chi^2) */
  for( sg = s = 0.0, i = 0; i < n; i++ ) {
    r  = (*slope*(hits[i]->Zpos - magnet_center) + *xshift
	  - hits[i]->rResultPos);
    s  += G[i][i] * r * r;
    /* sg += G[i][i]; */
  }
  *chi   = sqrt( s/n );

  chi_hashinsert(hits,n, *slope, *xshift, cov, *chi);
  cerr << "weight_lsq end " << endl;
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

void treecombine::weight_lsq_r3(double *slope, double *xshift, double cov[3],double *chi,Hit **hits, int n,double z1, int offset,int tlayers){
  static matrix A = 0, G, At, GA, AtGA;
  double  AtGy[2], y[tlayers], x[2];
  double r, s, sg, det, h;
  int i,j,k;
  double resolution = 0.1;

  if( !A ) {/* allocate matrices */
    A      = M_Init( tlayers, 2);
    G      = M_Init( tlayers, tlayers);
    At     = M_Init( 2, tlayers);
    GA     = M_Init( tlayers, 2);
    AtGA   = M_Init( 2, 2);
    for( i = 0; i < tlayers; i++ ) {
      At[0][i] = A[i][0] = -1.0;
    }
  }
//cerr << "a";
  //if( chi_hashfind(hits ,n, slope, xshift, cov, chi) )
  //  return;
//cerr << "b";
  /* initialize the matrices and vectors */
  for( i = 0; i < n; i++ ) {
    A[i][1] = -(i+offset);
    //cerr << A[i][1] << ",";
    y[i]     = -hits[i]->rResultPos;
    r = 1.0 / hits[i]->Resolution;
    if(!(hits[i]->Resolution))r = 1.0 / resolution;// WARNING : this is a hack to make this fit work.  Hit resolutions needs to be set up.
    //cerr << r << ",";
    G[i][i] = r*r;
  }

  /* calculate right hand side */
  for( i = 0; i < 2; i++ ) {
    s = 0;
    for( k = 0; k < n; k++ )
      s += A[k][i] * y[k] * G[k][k];
    AtGy[i] = s;
  }  
  /* and now the left hand side */
  for( i = 0; i < 2; i++ ) {
    for( k = 0; k < 2; k++ ) {
      s = 0;
      for( j = 0; j < n; j++ )
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
  for( i = 0; i < 2; i++ ) 
    x[i] = AtGA[i][0] * AtGy[0] + AtGA[i][1] * AtGy[1];
  
  *slope  = x[1];
  *xshift = x[0];
  cov[0]  = AtGA[0][0];
  cov[1]  = AtGA[0][1];
  cov[2]  = AtGA[1][1];

  for( sg = s = 0.0, i = 0; i < n; i++ ) {
    r  = (*slope*(hits[i]->Zpos - z1) + *xshift
	  - hits[i]->rResultPos);
    s  += G[i][i] * r * r;
  }
  *chi   = sqrt( s/n );
  chi_hashinsert(hits,n, *slope, *xshift, cov, *chi);
}
//__________________________________________________________________
int treecombine::contains( double var, Hit **arr, int len) {
  int i;
  for( i = 0; i < len ; i++) {
    if( var == arr[i]->rResultPos )
      return 1;
  }
  return 0;
}
//__________________________________________________________________
/* ------------------------------
   searches for the closed hit to
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
  double t = 
    (( det->rRotSin * (det->center[0] - x) +
       det->rRotCos * (magnet_center - det->Zpos))) /
    ( slope_x * det->rRotSin - det->rRotCos);

  if( xval )
    *xval = x + slope_x * t;
  
  return t + magnet_center;
}
//__________________________________________________________________
int treecombine::inAcceptance( enum EUppLow up_low,
	      enum ERegion region,
	      double cx, double mx,
	      double cy, double my )
{
cerr << "This function needs to conform to Qweak acceptance" << endl;

  enum Edir dir;
  double z, x, y;
  Det *rd/*, *rds, *rde*/;
  for( dir = u_dir; dir <= x_dir; dir++) {
    for(rd = rcDETRegion[up_low][region-1][dir-u_dir];
	rd; rd = rd->nextsame ) {
      z = rd->Zpos;
      x = cx + (z-magnet_center)*mx;
      y = cy + (z-magnet_center)*my;
/*
      rde = rd->leftwing  ? rd->leftwing  : rd;
      rds = rd->rightwing ? rd->rightwing : rd;
*/
      if(/* rds->center[0] - 0.5*rds->width[0] > x ||
	  rde->center[0] + 0.5*rde->width[0] < x ||*/
	  rd ->center[1] - 0.5*rd ->width[1] > y ||
	  rd ->center[1] + 0.5*rd ->width[1] < y) {
	return 0;
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
int treecombine::TlCheckForX(double x1, double x2, double dx1, double dx2,double z1, double dz,TreeLine *treefill,enum EUppLow up_low,enum ERegion region,enum Etype type,enum Edir dir/*,enum Eorientation orient*/,int  dlayer, int tlayer,int  iteration,int  stay_tuned)
{
//################
// DECLARATIONS  #
//################
  Det *rd, *firstdet = 0, *lastdet;/* detector loop variable                 */
  double org_x;     		/* x at center of magnet                  */
  double org_mx;     		/* slope                                  */
  double org_dx;
  double org_dmx;		/* the same for the error                 */
  double thisX, thisZ;		/* X and Z at a given plane               */
  double startZ = 0.0, endZ = 0;
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
  //int vcmiss = 0;
  int besti = -1;               /* best permutation index                 */
  int ret   = 0;		/* return value (pessimistic)             */
  int first;
  //int missidx = fr_back + 2* (method == meth_std? 0 : 1);

//##################
//DEFINE VARIABLES #
//################## 
  org_mx  = (x2-x1)/dz;//calculate slope
  org_x   = x1 + org_mx * (magnet_center-z1);//calculate x at center of magnet
  org_dmx = (dx2-dx1)/dz; 
  org_dx  = dx1 + org_dmx * (magnet_center-z1);

  int rcSETiLevels0 = 4;
  cerr << "ERROR: static rcSETiLevels0 needs read in from Qset" << endl;
  res = rcTreeRegion[up_low][region-1][type][dir]->rWidth /(1<<(rcSETiLevels0-1));//bin 'resolution'

  org_mx = (x2-x1)/dz;
  //org_x = ?
  org_dmx = (dx2-dx1)/dz;
  //org_dmx = ?
  


//###################################
//LOOP OVER DETECTORS IN THE REGION #
//###################################
	// uses BESTX & SELECTX     #
	//###########################
  if(region == r3){
	
  }
  else{

  /* --- loop over the detectors of the region --- */
  first = 1;
  for(nhits = 0, rd = rcDETRegion[up_low][region-1][dir];
      rd; rd = rd->nextsame) {
    thisZ = rd->Zpos;
    thisX = org_mx * (thisZ - magnet_center) + org_x;
    cerr << thisX << "," << dz << endl;
    if( !firstdet ) {
      firstdet = rd;
      startZ = thisZ;
    }
    lastdet = rd;
    endZ = thisZ;
/*        
    visdir = dir;
    viswer = up_low;
    vispar = region;
*/
    double rcSETrMaxRoad          = 1.4;
    cerr << "ERROR : USING STUB VALUE FOR RCSET " << endl;


    /* --- search this road width for hits --- */
    //set 'resnow'
    resnow =  org_dmx * (thisZ - magnet_center) + org_dx;
    if( !iteration && !stay_tuned && (first || !rd->nextsame) && tlayer == dlayer)
      resnow -= res*(rcSETrMaxRoad-1.0);


    cerr << "a";
    //bestx finds the hits which occur closest to the path through the first and last detectors 
    //nMult is the number of good hits at each detector layer
    if( !iteration ){	        /* first track finding process */
      nMult[nhits] = bestx( &thisX, resnow,
			    rd->hitbydet, DetecHits[nhits]);
    }
    else			/* in iteration process (not used by TlTreeLineSort)*/
      nMult[nhits] = selectx( &thisX, resnow,
			      rd, treefill->hits, DetecHits[nhits]);
    cerr << "b";
    if( nMult[nhits] ) {	/* there are hits in this detector         */
      permutations *= nMult[nhits];
    } else {
      nhits --;			/* this detector did not fire              */
    }
    nhits++;	
  }
  }
//############################
//RETURN HITS FOUND IN BESTX #
//############################
  if( !iteration ) {		/* return the hits found in bestx()        */
    hitarray = treefill->hits;
    for( j = 0; j < nhits; j++ ) {
      for( i = 0; i < nMult[j]; i++ ) {
	*hitarray = DetecHits[j][i]; 
	hitarray ++;
      }
    }
    if( hitarray - treefill->hits < DLAYERS*MAXHITPERLINE + 1) 
      *hitarray = 0;		/* add a terminating 0 for later selectx()*/
  }

//#####################################################
//DETERMINE THE BEST SET OF HITS FOR THE TREELINE    #
//#####################################################
// uses : MUL_DO     #
//	  WEIGHT_LSQ #
//####################	  
  int rcSETiMissingTL0 = 2;
  if( nhits >= dlayer - rcSETiMissingTL0/*rcSET.iMissingTL[missidx]*/) {  /* allow missing hits */
    for( i = 0; i < permutations; i++ ) {
      //mul_do is used to create hit arrays for every possible hit combination with one hit per layer.
      mul_do(i,permutations,nhits,nMult,
	     DetecHits,UsedHits);
      
      chi = 0.0;
      weight_lsq( &mx, &cx, cov, &chi, UsedHits, nhits);

      stay_chi = 0.0;
      if( stay_tuned ) {
	dh = (cx+mx*(startZ-magnet_center)-
	      (org_x+org_mx*(startZ-magnet_center)));
	stay_chi += dh*dh;
	dh = (cx+mx*(endZ-magnet_center)-
	      (org_x+org_mx*(endZ-magnet_center)));
	stay_chi += dh*dh;
      }
      if( stay_chi + chi + dlayer-nhits < minweight ) {
	/* has this been the best track so far? */
	minweight = stay_chi + chi + dlayer-nhits;
	minchi = chi;
	mmx    = mx;
	memcpy( mcov, cov, sizeof cov );
	mcx    = cx;
	besti  = i;
      }
    }
    
    treefill->cx  = mcx;
    treefill->mx  = mmx;     /* return track parameters: x, slope, chi */
    treefill->chi = minchi;
    treefill->numhits    = nhits;
    //treefill->numvcmiss  = vcmiss;/* return the number of missing vc-hits */
    
    memcpy(treefill->cov_cxmx, mcov, sizeof mcov);
    
    if( besti != -1 ) {
      mul_do( besti, permutations,
	      nhits, nMult, DetecHits, UsedHits);
      for( j = 0 ; j < MAXHITPERLINE*DLAYERS && treefill->hits[j]; j ++ ) 
	treefill->hits[j]->used = false;
      for( j = 0; j < nhits; j++ ) {
	if( UsedHits[j] ) 
	  UsedHits[j]->used = true;
      }
    }
    ret = (besti != -1);
  }

//################
//SET PARAMATERS #
//################
  if( !ret ) {
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
be misleading due to the currently low resolution (8 bins@5/7/07) of 
the pattern database.  It should however, be able to distinguish the
right/left ambiguity for each wire.

OUTPUT : the best hits are added to treefill's hit list and treefill's
	line parameters are set.
--------------------------------*/
int treecombine::TlMatchHits(double x1,double x2,double z1, double dz,TreeLine *treefill,enum EUppLow up_low,enum ERegion region,enum Etype type,enum Edir dir,int bins,int tlayers){
//################
// DECLARATIONS  #
//################
  double thisX, thisZ;		/* X and Z for the pattern's wire hit */
  Hit *h;
  Det *rd;
  double dx,slope,intercept;
  Hit   *DetecHits[tlayers]; /* Hits at a detector         */
  Hit   **hitarray;		/* temporary     */

  double mx,cx,dh,chi,minchi=1e8;
  double minweight = 1e10;
  double mmx=0,mcx=0; /* least square fit parameters     */
  double cov[3], mcov[3];
  int ret=0;
  int nhits,j=0;
//##################
//DEFINE VARIABLES #
//##################
  dx = x2 - x1;
  slope = dx/dz;
  intercept = 0.5*((x2 + x1) + slope*(z1 + z1 + dz));


  intercept = x1 - slope*z1;
  //cerr << "(" << slope << "," << intercept << "," << z1 << "," << x1 << ")" << endl;
  nhits = treefill->lastwire - treefill->firstwire + 1;
  if(nhits < 0) nhits = - nhits;
cerr << "1";
//########################
//MATCH HITS TO TREELINE #
//########################
  //cerr << "matchhits" << endl;
//STEP 1 : Match left/right wire hits to the pattern hits
  rd = rcDETRegion[up_low][region-1][dir];
  if(treefill->r3offset >=100){//get the correct plane for this treeline
	rd = rd->nextsame;
  }
  for(int i=treefill->firstwire;i<=treefill->lastwire;i++){//loop over pattern positions
    for(h = rd->hitbydet;h;h = h->next){//loop over the hits
 	thisZ = treefill->r3offset+i;
	thisX = slope*thisZ + intercept;
	if(h->wire!=thisZ){
		continue;
	}
        //cerr << "Z=" << thisZ << endl;
        bestx( &thisX,h, DetecHits,j);
	j++;
	//        cerr << thisZ << "," << thisX << "," << z1 << "," << z1+dz << "," << intercept << "," << slope << 		"," << treefill->r3offset << "," << h->wire << endl;
	
    }
  }
cerr << "2";
if(j!=nhits) cerr << "WARNING NHITS != NGOODHITS " << nhits << "," << j << endl;
//############################
//RETURN HITS FOUND IN BESTX #
//############################
  hitarray = treefill->hits;
  for(int i = 0; i < j; i++ ) {
	DetecHits[i]->used = true;
	*hitarray = DetecHits[i]; 
	hitarray ++;
	//cerr << DetecHits[i]->rPos << ",";
  }
  cerr << endl;
  if( hitarray - treefill->hits < tlayers){ 
      *hitarray = 0;		/* add a terminating 0 for later selectx()*/
  }
cerr << "3";
//######################
//CALCULATE CHI SQUARE #
//######################
  chi = 0.0;
  weight_lsq_r3( &mx, &cx, cov, &chi,DetecHits, j,z1,treefill->r3offset,tlayers);
cerr << "4";
//################
//SET PARAMATERS #
//################
  treefill->cx  = cx;
  treefill->mx  = mx;     /* return track parameters: x, slope, chi */
  treefill->chi = chi;
  treefill->numhits    = nhits;   
  memcpy(treefill->cov_cxmx, mcov, sizeof mcov);
  if(nhits>=7){//CUT TRACKS MISSING MORE THAN 1 HIT
    ret =1;
  }

  if( !ret ) {
    treefill->isvoid  = true;
    treefill->nummiss = tlayers-nhits;
  } else {
    treefill->isvoid  = false;
    treefill->nummiss = tlayers-nhits;
  }
  return ret;
cerr << "5";
//return 0;
}

/* ------------------------------
   Marks TreeLines close to
   other Lines as being good
   / bad in respect to their
   chi^2
   ------------------------------ */
void treecombine::TlTreeLineSort(TreeLine *tl,enum EUppLow up_low,enum ERegion region,enum Etype type,enum Edir dir,unsigned long bins,int tlayer,int dlayer)
{
if(region == r3){
  double z1,z2,dx;
  double x1,x2;
  TreeLine *walk = tl;
  TreeLine *owalk;
  chi_hashclear();
  Det *rd;
  //double wirespacing;
  
  //rd = rcDETRegion[up_low][region-1][dir];
  //wirespacing = rd->WireSpacing;//<-by not looping over detectors, this assumes the wirespacing is the same for
				//each detector

  cerr << "dlayer = " << dlayer << endl;
  /* --------------------------------------------------
  calculate line parameters first
  -------------------------------------------------- */
	
  for( walk = tl; walk; walk = walk->next ) {//for each matching treeline
    if( ! walk->isvoid ) {
      if(dlayer==1){
		walk->r3offset +=282;//<-282 should be a read-in variable
      }
      z1 = (double)(walk->r3offset+walk->firstwire);//first z position
      z2 = (double)(walk->r3offset+walk->lastwire);//last z position
      dx = rcTreeRegion[up_low][region-1][type][dir]->rWidth;
      dx /= (double)bins;

	//cerr << "(" << walk->a_beg << "," << walk->a_end << "," << walk->b_beg << "," << walk->b_end << "," << bins << "," <<  dx << ")" << endl;

      

      x1 = (walk->a_beg - (double)bins/2)*dx + dx/2;
      x2 = (walk->b_end - (double)bins/2)*dx + dx/2;
      //cerr << "(" << x1 << "," << x2 << "," << z1 << "," << z2 << "," << walk->r3offset << ")" << endl;
      TlMatchHits(x1,x2,z1,z2-z1,walk,up_low,region,type,dir,bins,TLAYERS);

      cerr << "(" << walk->cx << "," << walk->mx << "," << walk->chi << "," << walk->numhits << ")" << endl;
    }
  }
  cerr << "a" << endl;
  /* --------------------------------------------------
     now sort again for identical treelines
     -------------------------------------------------- */
  for( walk = tl; walk; walk = walk->next ) {
    if( walk->isvoid == false ) {
      for( owalk = walk->next; owalk; owalk = owalk->next) {
        if(owalk->isvoid == false &&  walk->cx == owalk->cx
	   && walk->mx == owalk->mx ) {
	  owalk->isvoid = true;
        }
      }
    }
  }
  for(walk = tl; walk; walk = walk->next){
    cerr << "void = " << walk->isvoid << "," << walk->numhits << endl;
  }
  treesort ts;
  ts.rcTreeConnSort( tl,region);

  for(walk = tl; walk; walk = walk->next){
    cerr << "void = " << walk->isvoid << "," << walk->numhits << endl;
  }
}
else{


  double z1,z2,dx,dxh,dxb,dx_2;
  double x1,x2, dx1, dx2;
  TreeLine *walk = tl;
  TreeLine *owalk;
  chi_hashclear();
 
  z1  = rcTreeRegion[up_low][region-1][type][dir/*-u_dir*/]->rZPos[0];//first z position
  z2  = rcTreeRegion[up_low][region-1][type][dir/*-u_dir*/]->rZPos[tlayer-1];// last z position

  dx  = rcTreeRegion[up_low][region-1][type][dir/*-u_dir*/]->rWidth;//detector width
  dxh = 0.5*dx; //detector half-width
  dx /= (double)bins;//width of each bin
  dxb = dxh/(double)bins;//half-width of each bin

  
  /* --------------------------------------------------
     calculate line parameters first
     -------------------------------------------------- */
double rcSETrMaxRoad          = 1.4;
cerr << "ERROR : USING STUB VALUE FOR RCSET " << endl;
  for( walk = tl; walk; walk = walk->next ) {//for each matching treeline
    if( ! walk->isvoid ) {
      dx_2 =  dxh - dxb;
      x1 = 0.5*((walk->a_beg+walk->a_end) * dx) - dx_2;
      x2 = 0.5*((walk->b_beg+walk->b_end) * dx) - dx_2;
      dx1 = ((walk->a_end-walk->a_beg) * dx) + dx*rcSETrMaxRoad;
      dx2 = ((walk->b_end-walk->b_beg) * dx) + dx*rcSETrMaxRoad;
      TlCheckForX(x1, x2, dx1, dx2, z1, z2-z1, walk,up_low,region,type,dir, dlayer, tlayer, 0, 0);
    }
  }


  /* --------------------------------------------------
     now sort again for identical treelines
     -------------------------------------------------- */
  for( walk = tl; walk; walk = walk->next ) {
    if( walk->isvoid == false ) {
      for( owalk = walk->next; owalk; owalk = owalk->next) {
        if(owalk->isvoid == false &&  walk->cx == owalk->cx
	   && walk->mx == owalk->mx ) {
	  owalk->isvoid = true;
        }
      }
    }
  }
  treesort ts;
  ts.rcTreeConnSort( tl,region);
}

}
//__________________________________________________________________
int    rc_TrackFit( int No, Hit **Hit, vektor fit, matrix cov, double *chi,
		    double *addx, double *adde){
cerr << "This function is a stub, it needs to be written" << endl;
return -1000;
}

int treecombine::TcTreeLineCombine( TreeLine *wu, TreeLine *wv, TreeLine *wx,
		    PartTrack *pt, int tlayer )
{
  Hit *hits[DLAYERS*3], **hitarray, *h;
  int hitc, num, i, j;
  static matrix cov = 0;
  double fit[4], chi;
  enum Edir dir;
  extern int iEvent;

  if( !cov ) {
    cov = M_Init( 4,4);
  }

  for( hitc = 0, dir = u_dir; dir <= x_dir; dir++ ) {
    switch( dir ) {
    case u_dir: hitarray = wu->hits; break;
    case v_dir: hitarray = wv->hits; break;
    case x_dir: hitarray = wx->hits; break;
    default: hitarray = 0; break;
    }
      
    for( num = MAXHITPERLINE*DLAYERS; num--  && *hitarray; hitarray ++ ) {
      h = *hitarray;
      if( h->used == true && hitc < DLAYERS*3) 
	hits[hitc++] = h;
    }
  }

  if( rc_TrackFit( hitc, hits, fit, cov, &chi, 0,0)  == -1) {
    fprintf(stderr,"hrc: Event: %d - PartTrack Fit Failed\n", iEvent);
    return 0;
  }
  
  pt->x  = fit[0];
  pt->y  = fit[1];
  pt->mx = fit[2];
  pt->my = fit[3];
  pt->isvoid  = false;

  pt->chi = sqrt( chi);
  
  for( i = 0; i < 4; i++ )
    for( j = 0; j < 4; j++ ) 
      pt->Cov_Xv[i][j] = cov[i][j];
    
  pt->nummiss = wu->nummiss + wv->nummiss + wx->nummiss;
  pt->numhits = wu->numhits + wv->numhits + wx->numhits;
  pt->tline[0] = wu;
  pt->tline[1] = wv;
  pt->tline[2] = wx;
  return 1;
}  
//__________________________________________________________________
double uv2x(double u,double v){
cerr << "This function is just a stub" << endl;
return -1000;
}
double uv2y(double u,double v){
cerr << "This function is just a stub" << endl;
return -1000;
}
double xy2u(double x,double y){
cerr << "This function is just a stub" << endl;
return -1000;
}
double xy2v(double x,double y){
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
	parttrackanz : increased accordingly
	uvl : is used in TcTreeLineCombine
	PartTrack ret: a 3-D track
	

*/
PartTrack *treecombine::TlTreeCombine(TreeLine *uvl[2],long bins,enum EUppLow up_low,enum ERegion region,enum Etype type,int tlayer,int dlayer)
{
//################
// DECLARATIONS  #
//################
  cerr << "1";
  TreeLine *wu,*wv,*wx, *bwx, wrx;
  PartTrack *ret = 0, *ta;
  int in_acceptance;
  //int partmissidx = part + (method == meth_std? 0 : 1 )*2;

  double mu,xu,mv,xv, zx1, zx2, xx1, xx2;
  double x,y,mx,my,v1,v2,u1,u2, y1, y2;
  double x1, x2, d, maxd = 1e10, d1, d2;
  //double z;
  double rcSETrMaxXRoad         = 0.25;
  cerr << "ERROR : USING STUB VALUE FOR RCSET " << endl;
  chi_hashclear();
//###############
// DO STUFF     #
//###############

  //################
  // REGION 3      #
  //################
  if(region == r3 && type == d_drift){
	wu = uvl[u_dir];
	while(wu){
		if(wu->isvoid==true){
			wu = wu->next;
			continue;
		}
		//get wu's line parameters
		mu = wu->mx;//slope
		xu = wu->cx;//constant
		wv = uvl[v_dir];
		while(wv){
			if(wv->isvoid==true){
				wv = wv->next;
				continue;
			}
			//get wv's line parameters
			mv = wv->mx;//slope
      			xv = wv->cx;//constant

			//calculate positions
      			/* --- get u/v at the x detectors --- */
      			u1 = xu + (zx1 - magnet_center) * mu;
      			u2 = xu + (zx2 - magnet_center) * mu;
      			v1 = xv + (zx1 - magnet_center) * mv;
      			v2 = xv + (zx2 - magnet_center) * mv;
      
      			/* ------ for x -------- */
      			x1 = uv2x( u1, v1);
      			x2 = uv2x( u2, v2);

      			/* ------ for y -------- */
      			y1 = uv2y( u1, v1);
      			y2 = uv2y( u2, v2);
	
      			my = (y2-y1)/(zx2-zx1);
      			y  = 0.5*(y1+y2) + my * (magnet_center - 0.5*(zx1+zx2));



		}
	}
cerr << "2";
  return ret;
  }
  //#################
  // OTHER REGIONS  #
  //#################
  else{
  
  wu = uvl[u_dir /*- u_dir*/];	
  cerr << "3";
  zx1 = rcTreeRegion[up_low][region-1][type][x_dir/*-u_dir*/]->rZPos[0];
  cerr << "4";
  zx2 = rcTreeRegion[up_low][region-1][type][x_dir/*-u_dir*/]->rZPos[tlayer-1];
  cerr << "5";
  while( wu) {
    if( wu->isvoid == true ) {
      wu = wu->next;
      continue;
    }
    mu = wu->mx;
    xu = wu->cx;
    wv = uvl[v_dir/*-u_dir*/];
    while(wv) {
      if( wv->isvoid == true ) {
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
      x1 = uv2x( u1, v1);
      x2 = uv2x( u2, v2);

      /* ------ for y -------- */
      y1 = uv2y( u1, v1);
      y2 = uv2y( u2, v2);
	
      my = (y2-y1)/(zx2-zx1);
      y  = 0.5*(y1+y2) + my * (magnet_center - 0.5*(zx1+zx2));

      /* --- for the back region we now loop over the x treelines,
	 too. For the front region there are no x tree lines, yet.
	 they have to be calculated first --- */

      maxd = 1e10;
      if( rcTreeRegion[up_low][region-1][type][x_dir/*-u_dir*/]->searchable == true){
	wx = uvl[x_dir/*-u_dir*/];
	bwx = 0;

	while( wx ) {
	  if( wx->isvoid == true ) {
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
	
	//visdir = x_dir;	
	
	if( TlCheckForX(x1,x2, rcSETrMaxXRoad,rcSETrMaxXRoad, zx1,zx2-zx1,
			&wrx,up_low,region,type,x_dir/*,orient*/,dlayer,tlayer, 0,1)) {
	//replaced a Qmalloc below
	  wx = (TreeLine *)malloc( sizeof( TreeLine));
	  assert( wx );
	  *wx = wrx;

	  bwx = wx;
	  //wx->method = meth_std;
	  wx->next = uvl[x_dir/*-u_dir*/]; /* chain the link to the  */
	  uvl[x_dir/*-u_dir*/] = wx;       /* event treeline list */
	  maxd  = fabs( x1 - (wx->cx + wx->mx*(zx1-magnet_center)));
	  maxd += fabs( x2 - (wx->cx + wx->mx*(zx2-magnet_center)));
	} else
	  bwx = 0;
      }
      
      int rcSETiMissingVC0     = 6;
      int rcSETiMissingPT0     = 5;
      cerr << "ERROR : USING STUB VALUE FOR RCSET " << endl;
      if( maxd < 2 * rcSETrMaxXRoad
	  && bwx
	  && rcSETiMissingVC0/*rcSETiMissingVC[0]*/
	  >= bwx->numvcmiss + wu->numvcmiss + wv->numvcmiss
	  && rcSETiMissingPT0/*rcSET.iMissingPT[partmissidx]*/
	  >= bwx->nummiss + wu->nummiss + wv->nummiss) {
	in_acceptance = inAcceptance( up_low, region, bwx->cx, bwx->mx, y, my);
	if( in_acceptance ) {

	  wx = bwx;
	  x  = wx->cx;
	  mx = wx->mx;
	  
	  wx->Used = wu->Used = wv->Used = 1;
	  //visdir = x_dir;

	  //Statist[method].PartTracksGenerated[where][part] ++;
	  
	  /* ---- form a new PartTrack ---- */
	  //replaced a Qmalloc below
	  ta = (PartTrack *)malloc( sizeof(PartTrack));

	  parttrackanz ++;
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
void treecombine::ResidualWrite( Event *event)
{
  cerr << "This function may need a 'for loop' for orientation" << endl;

  enum EUppLow up_low;
  enum ERegion region;
  enum Edir dir;
  enum Etype type;
  //enum Eorientation orient;
  int allmiss, num;
  Track *tr;
  PartTrack *pt;
  TreeLine *tl;
  double x, y, v, mx, my, mv;
  Hit **hitarr, *hit;
  void mcHitCord( int, double* , double *);

  for( up_low = w_upper; up_low <= w_lower; up_low ++ ) {
    for( tr = event->usedtrack[up_low]; tr; tr=tr->next ) {
      //type = tr->type;
      for( region = r1; region <= r3; region ++ ) {
	/*
	if( region == f_front)
	  pt = tr->front;
	else
	  pt = tr->back;
	
	allmiss = ( pt->tline[0]->nummiss +
		    pt->tline[1]->nummiss +  pt->tline[2]->nummiss);
	*/
	for( dir = u_dir; dir <= x_dir; dir ++ ) {
	  tl = pt->tline[dir/*-u_dir*/];
	  hitarr = tl->hits;
	
	  for( num = MAXHITPERLINE*DLAYERS; num--&&*hitarr; hitarr ++ ) {
	    hit = *hitarr;
	    if( hit->used ) {
	      x = pt->mx*( hit->Zpos - magnet_center ) + pt->x;
	      y = pt->my*( hit->Zpos - magnet_center ) + pt->y;
	      //mcHitCord( hit->mcId, &mx, &my);
	      switch(dir) {
	      case u_dir: v = xy2u( x,y ); mv = xy2u(mx,my); break;
	      case v_dir: v = xy2v( x,y ); mv = xy2v(mx,my); break;
	      case x_dir: v = x; mv = mx; break;
	      default:    mv = v = 0;break;
	      }
	      printf("%d %d %d %d %d "
		     "%d "
		     "%f "
		     "%f %f %f "
		     "%f %f %f %f "
		     "%d %d %f %f %f "
		     "%f "
		     "XXptresXX\n",
		     hit->detec->ID, type, up_low, region,dir,
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
//__________________________________________________________________



