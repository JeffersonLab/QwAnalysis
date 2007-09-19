/*--------------------------------------------------------------------------*\

  PROGRAM: HRC (Hermes Reconstruction Code)       AUTHOR: Wolfgang Wander
                                                          wwc@hermes.desy.de

						ALTERED FOR QWEAK BY:
							Burnham Stokes
							bestokes@jlab.org 

   MODULE: treedo.c                             COMMENTS: Brendan Fox
                                                          foxb@hermes.desy.de

  PURPOSE: This module contains the code for finding tracks in an event.
           The task of finding tracks is sub-divided into three steps:

           (1) for each set of planes with like-pitched wires, possible
               possible track candidates (called treelines) are located 
               by the treesearch algorithm.  For this search, the hit 
               information is used to generate a bit pattern for the hits
               seen in each tree-detector.  The bit patterns for all the
               chambers in one detector region are then processed by the
               the treesearch algorithm to identify all combinations of
               hits in the chambers which could correspond to straight
               tracks through the chambers with a slope below the HRCSET
               MaxSlope parameter.  Typically, these treelines contain 
               quite a few ghost tracks.  To cut down on these ghosts, 
               the treelines located by the searching are reduced by
	       a connectivity graph. This graph tries to minimize the number
	       of overlapping tracks (tracks including the same hits) based
	       on their chi^2 fit quality.

           (2) Once the sets of treelines (one set for the u, a second set
               for the v, and a third set for the x) have been located for
               a region of the detector, the treelines are matched together 
               to form partial tracks. For the case of VC reconstruction
	       no x treelines can be formed because of the lack of symmetry
	       in the front region (tilted x-planes). In this case only
	       u/v combinations are formed and the resulting hypotethical
	       x-track is searched for hits along its line.

           (3) And, finally, the partial tracks from the front and the back
               region are bridged (i.e. paired together) to form the final
               tracks.

 CONTENTS: (brief description for now) 

 (01) Bcheck() - 

 (02) rcLinkUsedTracks()

 (03) rcTreeDo() - this function manages the track-finding and track-fitting
                   in HRC.  For each detector region (front or back, top or 
                   bottom) and each HRC fit technique (standard, NOVC, and 
                   FITVC), this function builds the bit patterns for the hits 
                   seen in the detector and then invokes the treesearch 
                   algorithm to find the candidate treelines.  After the 
                   resulting treelines are filtered by the treesort and 
                   treecombine functions, the treelines from the different
                   plane with different wire-pitch are combined together to 
                   form the partial tracks seen in each region of the detector.
                   In the final stage of tracking, these partial tracks are
                   bridged together to reconstruct full tracks through the
                   detector.
		   In a second interation treesearch recalibrates the tracking
		   chamber hits with the now known momentum and recalculates
		   track parameters and momentum for all found tracks.

\*--------------------------------------------------------------------------*/


#include <iostream>
#include "treedo.h"

using namespace std;

extern EUppLow operator++(enum EUppLow &rs, int );
extern ERegion operator++(enum ERegion &rs, int );
extern Etype operator++(enum Etype &rs, int );
extern Edir operator++(enum Edir &rs, int );
/*extern Eorientation operator++(enum Eorientation &rs, int );*/

extern TreeLine  *trelin;
extern int trelinanz;
extern treeregion *rcTreeRegion[2][3][4][4];
extern Det *rcDETRegion[2][3][4];
extern Options opt;
int parttrackanz;
//________________________________________________________________________
/*---------------------------------------------------------------------------*\

   Bcheck() - this function compares the momentum look-up energy with the
              shooting method result.  This function is for debugging
              purposes only.

    inputs: (1) double E       - the momentum lookup energy
            (2) PartTrack *f   - pointer to the front partial track for the
                                 track
            (3) PartTrack *b   - pointer to the back partial track of the 
                                 track
            (4) double TVertex - transverse position of the vertex for the
                                 track 
            (5) double ZVertex - Z position of the vertex for the track

   outputs: an ASCII ntuple file.

\*---------------------------------------------------------------------------*/
double rcShootP( double Pest,PartTrack *front, PartTrack *back, double accuracy){
cerr << "This function is only a stub" << endl;
return -1000;
}
void treedo::BCheck( double E, PartTrack *f, PartTrack *b, double TVertex, double ZVertex)
{
  double Es = rcShootP(0.0,f,b,0.005);
  //extern physic phys_carlo;
  static FILE *test = 0;

  if( !test )
    test = fopen("magcheck","w");
  if( test && E > 0.0) {
    fprintf(test,"%f %f %f %f %f %f\n",
	    /*phys_carlo.E,*/E,Es,f->mx,f->my, TVertex, ZVertex);
  }
}
//________________________________________________________________________
/*---------------------------------------------------------------------------*\

  rcLinkUsedTracks() - this function links the track cross-linked list to
                       a single-linked list to ease the access for later
		       functions. The new link is done via the usenext
		       pointer.

    inputs: (1) Track *track - 
            (2) int upplow   - 

   returns: the head to the linked list.

\*---------------------------------------------------------------------------*/


Track * treedo::rcLinkUsedTracks( Track *track, int upplow )
{
  Track *ret = 0, *usedwalk = 0;
  Track *trackwalk, *ytrack;

  /* loop over all found tracks */
  for(trackwalk = track; trackwalk; trackwalk = trackwalk->next ) {
    /* and the possible y-tracks */
    for( ytrack = trackwalk; ytrack; ytrack = ytrack->ynext ) {
      //Statist[ytrack->method].TracksGenerated[upplow] ++;
      if(!ytrack->Used)
	continue;
      //Statist[ytrack->method].TracksUsed[upplow] ++;
      if( !ret )		/* return the first used track */
	ret = usedwalk = ytrack;
      else {
	usedwalk->usednext = ytrack; /* and link them together */
	usedwalk = ytrack;
      }
    }
  }
  return ret;
}
//________________________________________________________________________
/*---------------------------------------------------------------------------*\

  rcTreeDo() - this function is the main tracking function. It
               performes tracking in projections (u/v/x) to form treelines,
	       it combines projection tracks to tracks in space and bridges
	       tracks in space before and after the magnet to form recon-
	       structed particle tracks. The function calculates the momentum
	       of the tracks and afterwards reiterates the track parameters
	       using the now know track position and momentum for iterating
	       the hit positions in the tracking chambers.

    inputs: (1) int iEventNo - the event number

   outputs: (1) Event *rcTreeDo() - pointer to the structure with all
                                    of the reconstruction information for
                                    this event.

\*---------------------------------------------------------------------------*/
Event * treedo::rcTreeDo(int iEventNo){
cerr << "Initialize" << endl;
  enum EUppLow upplow;        /* Loop counter for TOP and BOTTOM detectors  */
//  enum Emethod method;        /* Loop counter over reconstruction method    */
//  enum EFrontBack frback;     /* Loop counter over FRONT or BACK regions    */
  enum Edir dir;              /* Loop counter over wire pitches (U,V,X)     */
  enum ERegion region;
  enum Etype type;
 /* enum Eorientation orient;*/

  int i,k;
  int dlayer = 0;	      /* number of detector planes in the search    */
  double A[3][2];	      /* conversion between xy and uv */
  Event *event;               /* area for storing the reconstruction info   */                     
  PartTrack *area = 0;
  Det *rd/*, *rnd*/;              /* pointers for moving through the linked 
                                 lists of detector id and hit information   */
  TreeLine *trelin1,*trelin2;  

  treesearch TreeSearch;
  treecombine TreeCombine;
  treesort TreeSort;
  treematch TreeMatch;

  /*int charge;
  int found_front = 0;
  double ZVertex, bending, theta, phi, ESpec;
  int outbounds = 0;*/
  int levelmax;
  static int init = 0;
  static char *channel[TLAYERS];
  static int  *hashchannel[TLAYERS];
  int rcSETiLevels0 = 4;
  const int numWiresr3 = 282;
  static char *channelr3[numWiresr3];
  static int  *hashchannelr3[numWiresr3];

  	event = (Event *)calloc(1,sizeof(Event));
  	assert(event);
  	// Initialize stuff if first event
  	if( !init ) {
    		init++;                            /* don't initialize again           */
    		levelmax = rcSETiLevels0;       /* determine the bin-division depth
                                          of the tree-search               */
    		for( i = 0; i < TLAYERS; i++ ) {   /* reserve space for the bit patterns */
      			channel[i]     = (char*)malloc( 1UL << levelmax );
      			hashchannel[i] = (int*)malloc( (sizeof(int)*(1UL << (levelmax-1) )));
      			assert( channel[i] && hashchannel[i] );
    		}
		for( i = 0; i < numWiresr3; i++){ /* reserve space for region 3 bit patterns */
			channelr3[i]     = (char*)malloc( 1UL << (opt.levels[0][2][0]));
			hashchannelr3[i] = (int*)malloc((sizeof(int)*(1UL << (opt.levels[0][2][0]-1))));
			assert(channelr3[i] && hashchannelr3[i]);
		}
		
  	}



	if(!init){
		//do stuff
		init++;
	}
	//loop through all detectors/regions/wire directions
	for(upplow = w_upper;upplow <=w_upper/*w_lower*/;upplow++){
	for(region = r3/*r1*/;region<=r3;region++){
	for(type=d_drift;type<=d_drift/*d_cerenkov*/;type++){
	parttrackanz = 0;/* no partial tracks found yet */
	for(dir=u_dir;dir<=u_dir/*y_dir*/;dir++){


/* ---- 1st: check that the direction is tree-searchable        ---- */

		if(rcTreeRegion[upplow][region-1][type][dir]->searchable == false){
			(event->treeline[upplow][region-1][type][dir])=0;
			printf("%i %i %i %i\n",upplow,region,type,dir);
			continue;//'searchable' is set by tree.cc
		}

/* ---- 2nd: do some variable initialization for the found treeline 
             linked list                                              ---- */

	  	trelin = 0;    /* clear pointer to linked list of treelines */
	  	trelinanz = 0; /* clear the "treelines found" counter       */

/* ---- 3rd: create the bit patterns for the hits                     ---- */ 


//_______________________________________
		if(region == r3){
          		dlayer = 0;    /* set "number of detectors" to zero            */
			int decrease;
			trelin1 = 0;trelin2 = 0;
			for(k=0,rd = rcDETRegion[upplow][region-1][dir],decrease = 0;
	      			rd; rd = rd->nextsame,decrease+=282,k++ ) { /* over the like-pitched planes in a region    */
				trelin = 0;//clear the linked list for the 2nd layer
				Hit *H,*Hnext;
	      			A[dir][0] = rd->rCos; /* cos(angle of wire pitch) */
	      			A[dir][1] = rd->rSin; /* sin(angle of wire pitch) */
				H = rd->hitbydet;
				cerr << "Set Pattern Hits" << endl;
				for( i = 0; i < numWiresr3; i++){
	      				memset(channelr3[i],0,1UL<<(opt.levels[0][2][0]));
	      				memset(hashchannelr3[i], 0, sizeof(int)*
		     				(1UL<<(opt.levels[0][2][0]-1)));
				}
				while(H && H->wire < 282+decrease){
					TreeSearch.TsSetPoint(rcTreeRegion[upplow][region-1][type][dir]->rWidth,H,channelr3[H->wire-decrease],hashchannelr3[H->wire-decrease],1U<<(opt.levels[upplow][region-1][type]-1));
					//for(int i=0;i<8;i++)cerr << hashchannelr3[H->wire-decrease][i] << " ";cerr << endl;
					//cerr << H->wire-decrease << ":";
					if(opt.showEventPattern){
						cerr << "w" << H->wire << ":";
						for(int i=0;i<(1UL<<(opt.levels[upplow][region-1][type]))-1;i++){
							
							cerr << "<" ;
							cerr << channelr3[H->wire-decrease][i] ;
							cerr << ">";
						}
						cerr << endl;
					}
					Hnext = H->nextdet;
					H = Hnext;
				}
				if(1)cerr << "Search for Matching Patterns " <<  endl;

				TreeSearch.TsSearch(&(rcTreeRegion[upplow][region-1][type][dir]->node),
		   			channelr3, hashchannelr3, opt.levels[upplow][region-1][type],numWiresr3);
				
				
				if(1)cerr << "Sort Patterns " <<  endl;
	  			if( rcTreeRegion[upplow][region-1][type][dir] ){//<- This if statement may be done wrong
					if(k==0){
	    					TreeCombine.TlTreeLineSort(trelin,upplow,region,type,dir,
							1UL<<(opt.levels[upplow][region-1][type]-1),0,dlayer);
						trelin1 = trelin;
					}
					else if(k==1){
						TreeCombine.TlTreeLineSort(trelin,upplow,region,type,dir,
							1UL<<(opt.levels[upplow][region-1][type]-1),0,dlayer);
						trelin2 = trelin;
					}

				}
				dlayer++;
			
			}
			if(1)cerr << "Match Region 3 Segments" << endl;
			
			trelin = TreeMatch.MatchR3(trelin1,
					trelin2);
	  		tlayers = TLAYERS;     /* remember the number of tree-detector */
	  		tlaym1  = tlayers - 1; /* remember tlayers-1 for convenience   */
		}		
		else{
			cerr << "Error, no code for this type of detector here\n";	
			return event;
		}



//_______________________________________
/*


	  	tlayer = 0;    /* set "number of tree-detectors" to zero       *
          	dlayer = 0;    /* set "number of detectors" to zero            *
		for( i= 0;i<numWiresr3;i++){//this is redundant				
	      		memset(channelr3[i],0,1UL<<(opt.levels[upplow][region-1][type]));
	      		memset(hashchannelr3[i], 0, sizeof(int)*(1UL<<(opt.levels[upplow][region-1][type]-1)));
		}

		for(rd = rcDETRegion[upplow][region-1][dir];
	      		rd; rd = rd->nextsame ) { /* over the like-pitched planes in a region    *
			if(rd->type == d_drift && region == r3){
				Hit *H,*Hnext;
	      			A[dir][0] = rd->rCos; /* cos(angle of wire pitch) *
	      			A[dir][1] = rd->rSin; /* sin(angle of wire pitch) *
				H = rd->hitbydet;
				while(H){
					TreeSearch.TsSetPoint(rcTreeRegion[upplow][region-1][type][dir]->rWidth,H,channelr3[H->wire],hashchannelr3[H->wire],1U<<(opt.levels[upplow][region-1][type]-1));
					//for(int i=0;i<8;i++)cerr << hashchannelr3[H->wire][i] << " ";cerr << endl;
					//cerr << H->wire << ":";
					//for(int i=0;i<15;i++)cerr << "<" << channelr3[H->wire][i] << ">";cerr << endl;
					
					Hnext = H->nextdet;
					H = Hnext;
				
				}
			}
			
			else{
				cerr << "Error, no code for this type of detector here\n";	
				return event;
			}
			tlayer++;
			dlayer++;
		}/* end of loop over the planes in the region *
		//cerr << "tlayer = " << tlayer << endl;
	  	tlayers = tlayer;     /* remember the number of tree-detector *
	  	tlaym1  = tlayer - 1; /* remember tlayers-1 for convenience   *
________________________________________________*/
/* ---- 4th: now the hit patterns for the planes with wires of a 
             the same pitch have been constructed for a region of
             the detector, so apply the treesearch algorithm to
             these hit patterns to find the possible candidates for
             tracks (i.e. the treelines).  The found treelines are
             stored in a linked list and the pointer to this list
             is put into rcTreeRegion.                                ---- */



		if(region == r3){
			//for(int ii=0;ii<10;ii++){
			//for(int i=0;i<15;i++)cerr << "<" << channelr3[ii][i] << ">";cerr << endl;}
			//cerr << "---" << endl;
	  		//TreeSearch.TsSearch(&(rcTreeRegion[upplow][region-1][type][dir]->node),
		   	//	channelr3, hashchannelr3, opt.levels[upplow][region-1][type],numWiresr3);
		}
		else{
			//TreeSearch.TsSearch(&(rcTreeRegion[upplow][region-1][type][dir]->node),
		   	//	channel, hashchannel, opt.levels[upplow][region-1][type],0);

		}

	  	if( trelinanz > TREELINABORT ) {          /* Too many treelines */
	    						/* so drop the event  */
	    		return 0;
	  	}

/* ---- 5th: sort and search for ghosts in the  resulting lines       ---- */
cerr << "E" << endl;
		/*
	  	if( rcTreeRegion[upplow][region-1][type][dir] ){//<- This if statement may be done wrong
	    		TreeCombine.TlTreeLineSort(trelin,upplow,region,type,dir,
				1UL<<(opt.levels[upplow][region-1][type]-1),tlayer,dlayer);
		}
	  	event->treeline[upplow][region-1][type][dir] = trelin;
		*/
	}/* end of loop over the three wire-pitch directions */
cerr << "F" << endl;
cerr << "event : " << event->treeline[upplow][region-1][type][u_dir]->numhits << endl;
/* ---- TASK 2: Combine the treelines into partial tracks             ---- */
	if( rcTreeRegion[upplow][region-1][type]  && tlayers) {//<- This if statement may be done wrong
	  area = TreeCombine.TlTreeCombine(event->treeline[upplow][region-1][type], 
			       1L<<(opt.levels[upplow][region-1][type]-1),
			       upplow,
			       region,
			       type,
			       tlayers,
			       dlayer);
	}
	
cerr << "G" << endl;
	if( parttrackanz > 600 ) { /* Adamo can't stand that */
	  cerr << "Too many partial tracks " << endl;
	  return 0;
	}
/* ---- TASK 3: Sort out the Partial Tracks                          ---- */
	TreeSort.rcPartConnSort(area);
/* ---- TASK 4: Hook up the partial track info to the event info     ---- */
cerr << "H" << endl;           
	event->parttrack[upplow][region][type] = area;
	}}/* end of loop over the detectors */
    	 /* end of loop over the regions */

    	/* ==============================
     	* Correlate front and back
     	* tracks from x, y and y' infor-
     	* mation
     	* ============================== */

	cerr << "This is a stub for the part of the function that performs the bridging of the partial tracks" << endl;


	}
cerr << "I" << endl;
  	return event;
}
//________________________________________________________________________
