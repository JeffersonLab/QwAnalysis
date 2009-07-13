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

#include <cstdio>
#include <fstream>
#include <cassert>
#include <cstdlib>

#include "QwTrackingTreeMatch.h"
#include "QwTrackingTreeCombine.h"

using namespace std;

extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double rcPEval( double vz, double te, double ph, double bend){
  cerr << "Error: THIS FUNCTION IS ONLY A STUB rcPEval " << endl;
  return -1000;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double rcZEval( double vz, double te, double ph, double mom, int idx){
  cerr << "Error: THIS FUNCTION IS ONLY A STUB rcZEval " << endl;
  return -1000;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

//returns the best measured wire hit
QwHit* bestWireHit (QwTrackingTreeLine *walk, double bestpos = 0)
{
  double pos = 9999, newpos;
  int ibest;
  for (int i = 0; i < walk->numhits; i++) { //get the best measured hit in the back
    newpos = walk->hits[i]->rPos-bestpos;
    if (newpos < 0) newpos = -newpos;
    if (newpos < pos) {
      pos = newpos;
      ibest = i;
    }
  }
  return walk->hits[ibest];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeMatch::QwTrackingTreeMatch()
{
  debug = 0; // debug level
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
	QwTrackingTreeLine *front,
	QwTrackingTreeLine *back,
	EQwDetectorPackage package,
	EQwRegionID region,
	EQwDirectionID dir)
{
  if (region != kRegionID3) {
    cerr << "Error, this function is only for R3" << endl;
    return 0;
  }

  //###############
  // DECLARATIONS #
  //###############
  QwTrackingTreeLine *combined,*fwalk,*bwalk;
  double x[2],y[2],z[3],zp[2];
  QwHit *fpos, *bpos;
  double d,d2,d2u,d_uv;
  double pi = acos(-1.0), theta;
  double wirespacingf, wirespacingb, d_to_1st_wire_f, d_to_1st_wire_b;
  Det *rd;
  int i,j,k,l;
  double slope,intercept,fslope,bslope;
  int numflines=0,numblines=0;
  double fsloperes,bsloperes;
  double bestmatch;
  QwTrackingTreeLine  *lineptr;
  double mx,cx,cov[3],chi;
  double RotCos,RotSin,RotTan;
  int nhits,fhits,bhits;
  int matchfound = 0;
  QwTrackingTreeCombine *TreeCombine = new QwTrackingTreeCombine();
  QwHit *DetecHits[2*TLAYERS];

  ofstream gnu1,gnu2;
  if (dir == kDirectionU) {
    gnu1.open("gnu1.dat");
    gnu2.open("gnu2.dat");
  }

  //###################################
  // Get distance between planes, etc #
  //###################################
  rd = rcDETRegion[package][region][dir];
  theta = rd->Rot/360*2*pi;
  //get the u value for the first wire.
  d_to_1st_wire_f = rd->rSin * rd->PosOfFirstWire;
  // due to reverse order
  d_to_1st_wire_f = d_to_1st_wire_f - rd->NumOfWires * rd->WireSpacing;
  wirespacingf = rd->WireSpacing;
  fsloperes = rd->SlopeMatching;
  x[0]= rd->center[0];
  y[0]= rd->center[1];
  z[0]= rd->Zpos;
  zp[0] = z[0]-y[0]/tan(theta);

  rd = rd->nextsame;
  x[1]= rd->center[0];
  y[1]= rd->center[1];
  z[1]= rd->Zpos;
  zp[1] = z[1]-y[1]/tan(theta);

  d = (zp[1]-zp[0])*sin(theta); // distance between planes
  // cerr << "d = " << d << endl;
  wirespacingb = rd->WireSpacing;
  bsloperes = rd->SlopeMatching;
  // get the u value for the first wire.
  d_to_1st_wire_b = rd->rSin * rd->PosOfFirstWire;
  // due to reverse order
  d_to_1st_wire_b = d_to_1st_wire_b - rd->NumOfWires * rd->WireSpacing;


  if(dir == kDirectionV){//get the distance between the u and v planes
    RotCos = rcDETRegion[package][region][kDirectionU]->rRotCos;
    RotSin = rcDETRegion[package][region][kDirectionU]->rRotSin;
    RotTan = RotSin/RotCos;

    d_uv = (rcDETRegion[package][region][kDirectionV]->Zpos-rcDETRegion[package][region][kDirectionU]->Zpos);
    d_uv += (rcDETRegion[package][region][kDirectionU]->center[1]-rcDETRegion[package][region][kDirectionV]->center[1])/RotTan;
    d_uv *= RotSin;
//    d_uv = (rcDETRegion[package][region][kDirectionV]->Zpos-rcDETRegion[package][region][kDirectionU]->Zpos);
//    d_uv = d_uv/rcDETRegion[package][region][kDirectionU]->rRotCos;
  }
  //######################################
  // Get the radial offset of the planes #
  //######################################
  //warning : this assumes the planes are parallel.

  z[2] = z[0]+d*sin(theta);
  //d2 = (z[2]-z[1])/cos(theta);
  d2 = (y[1]-y[0])/sin(theta)+(zp[1]-zp[0])*cos(theta);
  d2u = d2*fabs(rd->rCos);

  //##########################
  //Revise the hit positions #
  //##########################
  if(dir == kDirectionU){
    for(int i =0;i<rd->NumOfWires;i++){
      gnu2 << "0 " << (i-141)*wirespacingf << " " << d << " " <<  (i-141)*wirespacingb +d2u << endl;
    }
  }
  for(fwalk = front;fwalk;fwalk = fwalk->next){
    numflines++;
    if(fwalk->isvoid == 0){
      for(i=0;i<fwalk->numhits;i++){
        fwalk->hits[i]->SetZPos((fwalk->hits[i]->GetElement() - 141) * wirespacingf);
        //fwalk->hits[i]->Zpos = (fwalk->hits[i]->GetElement() - 141) * wirespacingf;
        if(dir == kDirectionV)fwalk->hits[i]->rPos+= d_uv;
      }
    }
  }
  for(bwalk = back;bwalk;bwalk = bwalk->next){
    numblines++;
    if(bwalk->isvoid == 0){
      for(i=0;i<bwalk->numhits;i++){
        //bwalk->hits[i]->Zpos = (bwalk->hits[i]->GetElement() - 281 - 141) * wirespacingb + d2u;
        bwalk->hits[i]->SetZPos((bwalk->hits[i]->GetElement() - 281 - 141) * wirespacingb + d2u);
        bwalk->hits[i]->rPos = bwalk->hits[i]->rPos + d;
        if(dir == kDirectionV)bwalk->hits[i]->rPos+= d_uv;
      }
    }
  }


  int matches[numflines];
  int bmatches[numblines];
  double bestmatches[numblines];
  //###############################
  // Find matching track segments #
  //###############################
  for(i=0;i<numblines;i++)bestmatches[i]=99;
  i=0;
  for(fwalk = front;fwalk;fwalk = fwalk->next){//loop over front track segments
    matches[i]=-1;
    if(fwalk->isvoid == 0){//skip it if it's no good
      if(dir == kDirectionU){
        for(k=0;k<fwalk->numhits;k++){
          //gnu1 << fwalk->hits[k]->Zpos << " " << fwalk->hits[k]->rPos << endl;
          gnu1 << fwalk->hits[k]->GetZPos() << " " << fwalk->hits[k]->rPos << endl;
        }
      }
      fpos = bestWireHit(fwalk);
      j=0;
      bestmatch=99;
      for(bwalk = back;bwalk;bwalk = bwalk->next){
        j++;
	if(bwalk->isvoid !=0)continue;
        if(dir == kDirectionU){
          for(l=0;l<bwalk->numhits;l++){
           gnu1 << bwalk->hits[l]->GetZPos() << " " << bwalk->hits[l]->rPos << endl;//gnu1 << bwalk->hits[l]->Zpos << " " << bwalk->hits[l]->rPos << endl;
          }
        }
    	bpos = bestWireHit(bwalk,d);

	y[0]=fpos->GetZPos(); //y[0]=fpos->Zpos;
	y[1]=bpos->GetZPos(); // y[1]=bpos->Zpos;
	x[0]=fpos->rPos;
	x[1]=bpos->rPos;

        slope = (y[1]-y[0])/(x[1]-x[0]);
        intercept = y[1]-slope*x[1];

	fslope = wirespacingf/fwalk->mx;
	bslope = wirespacingb/bwalk->mx;
        if(fabs(fslope-slope)<=fsloperes && fabs(bslope-slope)<=bsloperes
	   && fabs(fslope-slope)+fabs(bslope-slope)<bestmatch){//if it's a good match
          if(bestmatches[j-1]!=99){// if the back segment has been matched already
		bestmatch = fabs(fslope-slope)+fabs(bslope-slope);
                if(bestmatch > bestmatches[j-1])continue;//check if it's better
                else matches[bmatches[j-1]]=-1;//if so, remove the bad match
          }
          //cerr << "match found " << i << " " << j-1 << endl;
	  matches[i]=j-1;//set the match on both match arrays
          bmatches[j-1]=i;
          bestmatch = bestmatches[j-1] = fabs(fslope-slope)+fabs(bslope-slope);
	}
	//cerr << x[0] << "," << y[0] << "," << x[1] << "," << y[1] << endl;

	//cerr << "slope = " << fslope << "," << slope << "," << bslope << endl;
        //cerr << "line = " << slope << "*x + " << intercept << endl;
        //cerr << "fline = " << wirespacingf/fwalk->mx << "*x + " << -wirespacingf*(fwalk->cx)/fwalk->mx - wirespacingf*141<< endl;
	//cerr << "mx = " << fwalk->mx << endl;
	//cerr << "cx = " << fwalk->cx << endl;

      }
    }
    i++;
  }
  //################################
  // Create the combined treelines #
  //################################
  lineptr = new QwTrackingTreeLine;
  assert(lineptr);

  for(fwalk = front,i=0;fwalk;fwalk = fwalk->next,i++){
    for(bwalk = back,j=0;bwalk;bwalk = bwalk->next,j++){
      if(matches[i]==j){//if this front segment was matched to this back segment
        //set the hits
        matchfound = 1;
	fhits = fwalk->numhits;
        bhits = bwalk->numhits;
        for(k=0;k<fhits;k++){
	  DetecHits[k]=fwalk->hits[k];
          lineptr->hits[k]=fwalk->hits[k];
	}
	for(k=0;k<bhits;k++){
	  DetecHits[k+fhits]=bwalk->hits[k];
	  lineptr->hits[k+fhits]=bwalk->hits[k];
        }
        nhits = fhits + bhits;
        for(k=0;k<nhits;k++){
	 // cerr << DetecHits[k]->Zpos << " " << DetecHits[k]->rPos << endl;
	}
        //fit a line to the hits
        TreeCombine->weight_lsq_r3 (&mx, &cx, cov, &chi, DetecHits, nhits, 0, -1, 2*TLAYERS);
        lineptr->mx = mx;
        lineptr->cx = cx;
        lineptr->chi = chi;
        lineptr->numhits = nhits;
        lineptr->nummiss = 2*TLAYERS - nhits;
        lineptr->isvoid = false;

        //cerr << "line = " << 1/mx << "x + (" << -cx/mx << ")" << endl;
	//string the tracks together
        lineptr->next = combined;
        combined = lineptr;
      }
    }
  }
  gnu1.close();
  gnu2.close();
  if (!matchfound) {
    return 0;
  }
  combined->next = 0;
  //cerr << "Matching Done" << endl;
  //cerr << combined->isvoid << endl;
  return combined;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingTreeMatch::TgTrackPar (
	QwPartialTrack *front,	//!- front partial track
	QwPartialTrack *back,	//!- back partial track
	double *theta,		//!- determined polar angle
	double *phi,		//!- determined azimuthal angle
	double *bending,	//!- bending in polar angle
	double *ZVertex)	//!- determined z vertex
{
  *theta = atan(front->mx);
  *phi   = atan(front->my);
  if (bending && back)
    *bending = atan(back->mx) - *theta;
  *ZVertex = - ( (front->mx * front->x  + front->my * front->y)
	       / (front->mx * front->mx + front->my * front->my));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrack* QwTrackingTreeMatch::TgPartMatch (
	QwPartialTrack *front,		//!- front partial track
	QwPartialTrack *back,		//!- back partial track
	QwTrack *tracklist,		//!- list of tracks
	EQwDetectorPackage package	//!- package identifier
	/*enum Emethod method*/)
{
  double bestchi = 1e10, chi;
  double v1, v2, v3;
  QwTrack *ret = 0, *newtrack = 0, *besttrack = 0, *trackwalk, *ytrackwalk;
  Bridge *bridge;
  //int m = method == meth_std ? 0 : 1;
  double theta, ZVertex, phi, bending, P;


  while (back) {
    /* --------- check for the cuts on magnetic bridging ------------ */
    if (back->isvoid == false
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
	bridge = new Bridge;//QCnew( 1, Bridge );
      assert (bridge);
      TgTrackPar( front, back, &theta, &phi, &bending, &ZVertex);

      ZVertex *= 0.01;
      if( ZVertex < DZA )
	ZVertex = DZA;
      if( ZVertex > DZA + DZW )
	ZVertex = DZA+DZW;
      P = rcPEval( ZVertex, theta, phi, bending);
      if( P > 0.0 ) {
	bridge->Momentum = P;
	if( bending < 0 )
	  P = -P;
	bridge->xOff  = rcZEval( ZVertex, theta, phi, P, 0);
	bridge->yOff  = rcZEval( ZVertex, theta, phi, P, 1);
	bridge->ySOff = rcZEval( ZVertex, theta, phi, P, 2);
      } else
	bridge->Momentum = 0.0;
      bridge->ySlopeMatch = back->my - front->my;
      bridge->xMatch      = v2 = back->x  - front->x  + bridge->xOff;
      bridge->yMatch      = v1 = back->y  - front->y  + bridge->yOff;
      bridge->ySMatch     = v3 = back->my - front->my + bridge->ySOff;


	double rcSET_rMagnMatchYSl0 = 0.03;//INSERTED FROM HRCSET.C FUNCTION
	double rcSET_rMagnMatchY0 = 1.0;//THESE ARE BOGUS VALUES TO MAKE THE PROG WORK
	double rcSET_rMagnMatchX0 = 3.0;
	cerr << "ERROR : INVALID MAGNETIC FIELD VALUES USED " << endl;

      if( fabs(v3) > rcSET_rMagnMatchYSl0/*rcSET.rMagnMatchYSl[m] && front->method == meth_std*/)
	v3 = 1e12;		/* reject */

      if( fabs( v2) > rcSET_rMagnMatchY0 ||
	  fabs( v1) > rcSET_rMagnMatchX0 )
	v1 = v2 = 1e12;


      newtrack->bridge = bridge;
      //newtrack->method = method;
      newtrack->front  = front;
      newtrack->back = back;
/*NOT SURE WHAT THIS BPHOTOSEARCH IS FOR, BUT IT SEEMS USELESS AT THIS POINT
      if( bPhotoSearch )
	newtrack->Used = true;
      else
	newtrack->Used = false;
*/

      /* ------ a weighted measure for the quality of bridging ------ */

      chi = v1 * v1 + v2 * v2 + v3 * v3;
      newtrack->chi  = sqrt(chi + front->chi*front->chi + back->chi*back->chi);
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
    besttrack->front->isused = true;
    besttrack->back->isused = true;
	double rcSET_rXSlopeSep = 0.01;//INSERTED FROM HRCSET
        cerr << "Error : bogus value used" << endl;
    for( trackwalk = ret; trackwalk ; trackwalk = trackwalk->next ) {
      if( fabs( newtrack->back->mx - besttrack->back->mx ) > rcSET_rXSlopeSep )
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
	  if (ytrackwalk->chi > newtrack->chi) {
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
