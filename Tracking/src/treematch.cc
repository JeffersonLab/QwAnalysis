#include <iostream>
#include "treematch.h"
using namespace std;









//________________________________________________________________________
double rcPEval( double vz, double te, double ph, double bend){
cerr << "ERROR : THIS FUNCTION IS ONLY A STUB rcPEval " << endl;
return -1000;
}
//________________________________________________________________________
double rcZEval( double vz, double te, double ph, double mom, int idx){
cerr << "ERROR : THIS FUNCTION IS ONLY A STUB rcZEval " << endl;
return -1000;
}
//________________________________________________________________________
Hit *bestWireHit(TreeLine *walk){//returns the best measured wire hit
  Hit *besthit;
  double pos=9999,newpos;
  int ibest;
  for(int i =0;i<walk->numhits;i++){//get the best measured hit in the back
    newpos = walk->hits[i]->rPos;
    if(newpos < 0)newpos = -newpos;
    if(newpos < pos){
      pos = newpos;
      ibest = i;
    }
  }
return walk->hits[ibest];
}
//________________________________________________________________________

treematch::treematch(){

}
//________________________________________________________________________
treematch::~treematch(){

}
//________________________________________________________________________
TreeLine *treematch::MatchR3(TreeLine *front,TreeLine *back){
  //###############
  // DECLARATIONS #
  //###############
  TreeLine *combined,*fwalk,*bwalk;
  int fronthits,backhits;
  int numfront=0,numback=0;
  double x[2],y[2];
  Hit *fpos,*bpos;
  //#################
  // DO STUFF #
  //#################

  for(fwalk = front;fwalk;fwalk = fwalk->next){//loop over front track segments
    if(fwalk->isvoid == 0){//skip it if it's no good
      fpos = bestWireHit(fwalk);
      for(bwalk = back;bwalk;bwalk = bwalk->next){
    	bpos = bestWireHit(bwalk);
	
	x[0]=fpos->rPos;
	x[1]=bpos->rPos;
	y[0]=fpos->wire;
	y[1]=bpos->wire;

	cerr << x[0] << "," << y[0] << "," << x[1] << "," << y[1] << endl;

      }
    }
  }

  

  return combined;
}
//________________________________________________________________________
void treematch::TgTrackPar( PartTrack *front, PartTrack *back,double *theta, double *phi, double *bending, double *ZVertex )
{
  	*theta = atan( front->mx);
  	*phi   = atan( front->my);
  	if( bending && back )
    		*bending = atan( back->mx) - *theta;
  	*ZVertex =  - ( ( front->mx * front->x  + front->my * front->y )
		  	/( front->mx * front->mx + front->my * front->my));
}


//________________________________________________________________________
Track * treematch::TgPartMatch( PartTrack *front, PartTrack *back, Track *tracklist,
	     enum EUppLow upplow/*, enum Emethod method*/)
{
  double bestchi = 1e10, chi;
  double v1,v2, v3;
  Track *ret = 0, *newtrack = 0, *besttrack = 0, *trackwalk, *ytrackwalk;
  Bridge *bridge;
  //int m = method == meth_std ? 0 : 1;
  double theta, ZVertex, phi, bending, P;

  


  while( back) {
    /* --------- check for the cuts on magnetic bridging ------------ */ 
    if(   back->isvoid == false
	 //removed all the following cuts
	  /*&& (fabs(front->y-back->y))            < rcSET.rMagnMatchY[m]*3.0 
	  && (fabs(front->x-back->x ))           < rcSET.rMagnMatchX[m]*3.0
	  && ( (front->method != meth_std && ! bNoForceBridge) ||
	       (fabs(front->my - back->my ))     < rcSET.rMagnMatchYSl[m]*2.0 )
	  && (fabs( front->x+front->mx*
		    (target_center-magnet_center) )) < target_width 
	  && (fabs( front->y+front->my*
		    (target_center-magnet_center) )) < target_width*/ ) {

      newtrack = new Track;//QCnew(1,Track); /*  a new track */
      assert(newtrack);
      //TgInit( newtrack );
      
      /* ----- keep bridging information ----- */
      if( front->bridge )	/* do we have front brigding info (mcheck) */
	bridge = front->bridge;
      else if( back->bridge )	/* or back one (mckalman) ? */
	bridge = back->bridge;
      else
	bridge = new Bridge;//QCnew( 1, Bridge );
      assert( bridge );
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
  if( besttrack ) {
    int mtch = 1;
    besttrack->Used = true;	/* set the parttrack used flags */
    besttrack->front->Used = true;
    besttrack->back->Used = true;
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
    if( !newtrack->Used )
      continue;
    for( trackwalk = tracklist; trackwalk; trackwalk = trackwalk->next ) {
      for( ytrackwalk = trackwalk; ytrackwalk;
	   ytrackwalk = ytrackwalk->ynext ) {
	/*
	if( ytrackwalk->method != method )
	  continue;
	*/
	if(ytrackwalk->Used && ytrackwalk->back == newtrack->back ) {
	  if( ytrackwalk->chi > newtrack->chi ) {
	    ytrackwalk->Used = 0;
	  } else {
	    newtrack->Used = 0;
	  }
	}
      }
    }
  }
  
  return ret;
}


