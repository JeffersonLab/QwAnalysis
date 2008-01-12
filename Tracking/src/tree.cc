/*! \file tree.cc --------------------------------------------------------------------------*\
 
 PROGRAM: QTR (Qweak Track Reconstruction)       AUTHOR: Burnham Stokes
                                                          bestokes@jlab.org
						ORIGINAL HRC AUTHOR
							Wolfgang Wander
							wwc@hermes.desy.de

  MODULE: tree.cc                               
 
 \brief The tree class contains the code for creating the treesearch 
          database.                                                       
 
 PURPOSE: This module contains the code for creating the treesearch 
          database.  The code first attempts to pull the database from
          disk.  But, if the required database is not found, the code
          will generate it from scratch and save a copy of it on disk.

 CONTENTS: (brief description for now)

 (01) consistent() - this function valids a possible treeline hit
                     pattern by seeing if the pattern is consistent
                     with a straight line trajectory through a 
                     detector with a slope less than or equal to
                     the HRCSET MaxSlope parameter.

 (02) existent()   - this function checks if a possible treeline hit
                     pattern is already included in the tree search
                     database.  If so, it returns a pointer to the pattern
                     within the database.  Otherwise, it returns 0.

 (03) nodeexists() - this function checks if a possible treeline hit
                     pattern is already known to a father.  If so, it
                     returns a pointer to the pattern within the tree
                     database.  Otherwise, it returns 0.

 (04) treedup()    - this function duplicates a treeline hit pattern.  It
                     returns a pointer to the copy of the treeline hit
                     pattern.

 (05) marklin()    - this function generates the treesearch database.  For
                     a given father, it generates the 2^(treelayers) 
                     possible son hit patterns.  Each son pattern is 
                     checked to see if it is consistent with a trajectory 
                     through the chamber.  If it is consistent, it is 
                     inserted into the treesearch database and then, by a 
                     recursive call to marklin(), its sons are generated.

 (06) treeout()    - a debugging function which displays the hit patterns
                     for an entry in the treesearch database.

 (07) _inittree()  - this function initializes the treesearch database and
                     then calls marklin() to generate the database.

 (08) _writetree() - a recursive function for pulling in the concise treesearch
                     search database.

 (09) writetree()  - this function calls _writetree() to write the long
                     version of the treesearch database to a disk file.  Later,
                     readtree() will read back this file to form the concise
                     treesearch database (so-called short tree) used by the 
                     treesearch algorithm.

 (10) freetree()   - this function clears the treesearch database and
                     frees up the memory that was used.

 (11) _readtree()  - a recursive function (called by readtree()) to read the
                     concise treesearch database (so-called short tree) from 
                     disk.  

 (12) readtree()   - this function calls _readtree() to read the concise
                     treesearch database from disk.  

 (13) inittree()   - this function creates the treesearch database for
                     one treeline.  It first attempts to read the 
                     database from a diskfile.  If this fails, it calls
                     _inittree() to generate the treesearch database.  

 (14) rcInitTree() - the main function of this module.  This function
                     calls inittree() to generate the tree database for
                     each of the treelines.    
                      
\*---------------------------------------------------------------------------*/

#include <iostream>
#include <math.h>


using namespace std;
#include "tree.h"
extern treeregion *rcTreeRegion[2][3][4][4];
extern Det *rcDETRegion[2][3][4];
extern Options opt;

EUppLow operator++(enum EUppLow &rs, int ) {
   return rs = (EUppLow)(rs + 1);
}
ERegion operator++(enum ERegion &rs, int ) {
   return rs = (ERegion)(rs + 1);
}
Etype operator++(enum Etype &rs, int ) {
   return rs = (Etype)(rs + 1);
}
Edir operator++(enum Edir &rs, int ) {
   return rs = (Edir)(rs + 1);
}
/*
Eorientation operator++(enum Eorientation &rs, int ) {
   return rs = (Eorientation)(rs + 1);

}*/

treenode::treenode(){


}


treenode::~treenode(){

}
void treenode::print(){
cout << "("<< minlevel << "," << maxlevel << ")" << endl;
cout << "bits = " << bits << endl;
int i=0;
while(i<TLAYERS){
	cout << bit[i] << "," ;
	i++;
}
cout << endl;
cout << "xref = " << xref << endl;
}

shorttree::shorttree(){

}

shorttree::~shorttree(){

}
void shorttree::print(){
	//cout << "("<< minlevel << "," << maxlevel << ")" << endl;
	cout << "bits = " << bits << endl;
	int i=0;
	while(i<TLAYERS){
		cout << bit[i] << "," ;
		i++;
	}
	cout << endl;
	//cout << "xref = " << xref << endl;
}

nodenode::nodenode(){

}

nodenode::~nodenode(){

}

shortnode::shortnode(){

}

shortnode::~shortnode(){

}

treeregion::treeregion(){

}

treeregion::~treeregion(){

}

//____________________________________________________________
tree::tree(){
	tlayers = 8;//set tlayers == maxhits for now (for region 3)
	hshsiz = 511;
	father.genlink = 0;
	for(int i=0;i<4;i++){father.son[i]=0;}
	father.maxlevel = -1;
	father.minlevel = -1;
	father.bits = 1;
	for(int i=0;i<TLAYERS;i++){father.bit[i]=0;}
	father.xref = -1;
	npat=0;

	#define OFFS1   2 /* Next Sons have to be linked to offset 1 nodelist */
	#define SONEND  3 /* End of son-description */
	#define REALSON 4 /* Son and grandson description follows */
	#define REFSON  5 /* Son reference follows */
  
}
//____________________________________________________________
tree::~tree(){

}
//____________________________________________________________
void tree::rcInitTree(){
	/* This function should be set up to generate a treeregion
	for each detector region.  The original code is set up to
	do this - DONE*/
	/* This function now generates a treeregion for each 
	detector for each orientation and plane direction. It needs
	checks for the GEM according to plane directions. */
 	enum EUppLow up_low;
	enum ERegion region;
	enum Etype type;
	enum Edir direction;
	char filename[256];
	for(up_low = w_upper; up_low <= w_lower;up_low++){
		if(up_low == w_lower) continue; //skip lower for now
	   	for(region = r1; region<=r3;region++){
			if(region < r3) continue;//skip r1 and r2 for now
			for(type = d_drift; type <=d_cerenkov;type++){
				if(type!=d_drift&&region==r3)continue;//only VDC in r3
				for(direction = null_dir; direction<= y_dir;direction++){
					if(direction!=u_dir && direction!=v_dir)continue;
						
					sprintf( filename,"trees/tree%d-%d-%c-%c-%c-%c.tre",
		     				tlayers,opt.levels[up_low][region-1][type],
		     				"ul"[up_low],
		     				"123"[region-1],
		     				"dgtc"[type],
		     				"nuvxy"[direction]);
					rcTreeRegion[up_low][region-1][type][direction] =
	      						inittree(filename,opt.levels[up_low][region-1][type],
							tlayers,rcDETRegion[up_low][region-1][direction]->width[2],type,region);
						
//cout << "(" << up_low << "," << region << "," << type << "," << direction << "," << orient << ")\n" ;
					//}
				}
			}
		}
	}
}
//____________________________________________________________

int tree::consistent(treenode *tst, int level,enum Etype type,enum ERegion region){
  if(type == d_drift && region == r3){
  
  int templayers = 8;
  int i;
  int *b = tst->bit;		/* For faster access to pattern in tst->bit */



  double x0;			/* Bitnumber in the first tree-detector,
                                   i.e. treelayer 0                         */ 
  double x3e, x3a;		/* Bitnumber in last / last checked layer   */
  double adda, adde;	        
  double dze, dza;

  double xf=0,zf;

  double z[templayers];
  double cellwidth = 1.11125;//distance between wires

  int cutback=0;
  int firstnonzero=0;

  for(i=0;i<templayers;i++){
     if(b[i])firstnonzero++;
     if(firstnonzero && !b[i])templayers=i+1;
  }

  /* ----- find the z position of each tree-detector relative to  
           the first tree-detector                                   ----- */
  //for one HDC layer, the tree-detectors are all in a row.  This
  //will have to be altered when the 2nd layer is added
  z[0]=0;
  for( i = 1; i < templayers; i++) {
	z[i]=z[i-1]+cellwidth;
  }

  /* Get the layer with the largest bit value (dependent on rules in marklin)*/
  for( i = 0; i < templayers; i++){
	if(b[i]>=xf){
		zf=i;
		xf=b[i];
	}
  }
  /* ----- initial setting for the line check using the first and
           the last tree-detectors                                   ----- */
  dze = dza = zf;
  x0 = b[0];                      /* Fetch the pattern for 1st tree-det.    */
  x3a = x3e = xf;          /* Fetch the pattern for last tree-det.   */
  x3e++;


  
  

 
  /* ----- first check if a straight track through the bins in the 
           first and the last tree-detectors fulfill the max angle 
           condition 		                                   ----- */  
  double m_min = -((double)tlayers-1)/((double)(1<<level)-1);
  double m_max = -(4.0-1.0)/((double)(1<<level)-1);
  double m = -((double)zf)/((double)(xf-x0));

  //cout << m_min << " " << m << " " << m_max << endl;
  if(m < m_min || m > m_max)return 0;

  /* ----- check if all the bits are along a straight line by 
           looping through each pair of outer tree-detectors and 
           seeing if the bins on the enclosed tree-detectors are 
           along a straight line                                     ----- */

  for( i = zf - 1; i > 0; i--) {     /* loop from the back 
                                             tree-detectors forward */  
    adda = (x0-b[i])*dza+(x3a-x0)*z[i];   
    if( adda <= -dza || dza <= adda)
      return 0;
    adde = (x0-b[i])*dze+(x3e-x0-1)*z[i];
    if( adde <= -dze || dze <= adde)
      return 0;
    if( i > 1) {
      if( adda > 0) {
	x3e = b[i]+1;
	dze = z[i];
      } 
      if( adde < 0) {
	x3a = b[i];
	dza = z[i];
      }
    }
  }
  return 1;
  }
  else{
	cerr << "This function is only a stub" << endl;
	return 0;
  }

}
/*
int tree::consistent(treenode *tst, int level,enum Etype type,enum ERegion region){
	cerr << type << "," << region << "," << level << endl;




	cerr << "This function is only a stub" << endl;
//stub function for consistent until the rest of tree is implemented
	return 1;
}
*/
//____________________________________________________________
treenode * tree::existent(treenode *tst, int hash){
	treenode *walk = generic[hash];   
	while( walk) { 
		if(!memcmp( tst->bit,walk->bit,tlayers*sizeof(tst->bit[0]))){
      			return walk;                  /* found it! */
		}
    		walk = walk->genlink;           /* nope, so look at the next pattern */
  	}
  	return 0;
}
//____________________________________________________________
treenode * tree::nodeexists(nodenode *nd, treenode *tr){
	while( nd) {
		if( !memcmp(nd->tree->bit,tr->bit, tlayers * sizeof(tr->bit[0])))
      			return nd->tree;  /* found it! */
    		nd = nd->next;      /* nope, so look at the next son of this father */
  	}
	return 0;
}
//____________________________________________________________
treenode * tree::treedup(treenode *todup){
	treenode *ret;

	ret = (treenode*)malloc(sizeof(treenode)); /* allocate the memory for 
                                                the copy treenode          */
	assert(ret);                               /* cry if there was an error  */
	*ret = *todup;                             /* copy the treenode todup to 
                                                the new treenode           */
	ret->xref = -1L;                           /* set the external reference
                                                link                       */
	return ret;

}
//____________________________________________________________
void tree::marklin(treenode *Father,int level,enum Etype type,enum ERegion region){
	treenode son;
	treenode *sonptr;
	nodenode *nodptr;
	int i,j;
	int offs;
	int flip;
	int maxs;
	int insert_hitpattern;
	int hsh;
	if(level == maxlevel){
		return;
	}
	son = *Father;
	
	i= (1<<tlayers);	// (1<<x) is equal to 2^x 
				/* Number of possible son patterns for this 
                              this Father*/
	
	if(region==r3 && type == d_drift){
		offs=1;
		maxs=0;
		flip=0;
		int shift;//cell # of first cell
		int maxhits = 8;//max # of cells that can be hit in wanted tracks
		i = (1<<maxhits);
		
		while(i--){//loop through all possibilities
	
			for(j=0;j<maxhits;j++){//this loop creates each possible pattern	
	
				if(i & (1<<j)){	
					son.bit[j] = (Father->bit[j]<<1)+1;
				}
				else{	
					son.bit[j] = Father->bit[j]<<1;
				}
			
				offs = (int)min(offs,son.bit[j]);
			
				maxs = (int)max(maxs,son.bit[j]);
			}

			
			//Cut patterns in which there are hits that lie outside the road 				between the first and last hits, i.e. :
			//  X 0      0 X  
			//  0 X  or  X 0
			//  0 X      0 X
			//  X 0	     0 X
			son.bits = son.bit[maxhits-1] - son.bit[0];
			//cout << "-------------------------" << endl;
			int cutback =0;
			int cutflag =0;
			for(j=1;j<maxhits;j++){
				if(son.bit[j]<son.bit[j-1]){//if the bin decreases
					if(son.bit[j])//and it's nonzero cut it
						cutflag++;
					if(!son.bit[j]){//but if it's zero, make sure it stays zero
						cutback++;
						if(cutback==1)son.bits = son.bit[j-1] - son.bit[0];
					}
				}
				if(son.bit[j] && cutback)
					cutflag++;

			}
			if(cutflag){
				/*
				cout << "Cut :" ;
				for(j=0;j<maxhits;j++)
					cout << son.bit[j] << " " ;
				cout << endl;*/
				continue;
			}

			if( offs){/* If there is an offset, so         */
				/*cout << "Offset :" ;
				for(j=0;j<maxhits;j++)
					cout << son.bit[j] << " " ;
				cout << endl;
				*/
      				for( j = 0; j< maxhits; j++) /* shift all hits over this 									offset   */
					son.bit[j] --;
			}
			if(son.bits < 0){
				/*cout << "Flip :" ;
				for(j=0;j<maxhits;j++)
					cout << son.bit[j] << " " ;
				cout << endl;
		*/
				flip =2 ;
				son.bits = -son.bits;
				for(j=0;j<maxhits;j++)
					son.bit[j] = son.bits-son.bit[j];
			}
    			son.bits++;
			sonptr = nodeexists( Father->son[offs+flip], &son);
			hsh = (son.bit[tlayers-1]+son.bit[1])%HSHSIZ;


			/*if(sonptr){for(j=0;j<maxhits;j++)cout << son.bit[j] << " " ;cout <<  "exists" << endl;}
			else{ for(j=0;j<maxhits;j++)cout << son.bit[j] << " " ;cout << endl;}
			cout << "hsh = " << son.bit[tlayers-1] << "," << son.bit[1] << "," << hshsiz << "," << hsh << endl;
			cout << "level : " << level << endl;
			cout << "bits : " << son.bits << endl;*/
			insert_hitpattern = 1;
			if( !sonptr&& 0 == (sonptr= existent( &son, hsh))){
				if( consistent( &son, level+1,type,region)) {
					//cout << "Adding treenode..." << endl;
					
					sonptr = treedup( &son);
					sonptr->son[0] = sonptr->son[1] = 
	  				sonptr->son[2] = sonptr->son[3] = 0; 
					sonptr->maxlevel = 
	  				sonptr->minlevel = level;
					sonptr->genlink = generic[hsh];      
			       		generic[hsh] = sonptr;
					npat++;
					/*cout << "good" <<endl;
					for(int k=0;k<tlayers;k++){
						for(int l=0;l< 2<<(level);l++){
							if(son.bit[k]==l)cout << "x ";
							else cout << "0 ";
						}
						cout << endl;
					}
					*/
					marklin( sonptr, level+1,type,region);
				}
				else{
					/*cout << "inconsistent" << endl;
					for(int k=0;k<tlayers;k++){
						for(int l=0;l< 2<<(level);l++){
							if(son.bit[k]==l)cout << "x ";
							else cout << "0 ";
						}
						cout << endl;
					}
					*/
					insert_hitpattern = 0;
				}
			}
			else if( (sonptr->minlevel > level  && consistent( &son, level+1,type,region) )||sonptr->maxlevel < level) {
				sonptr->minlevel = (int)min(level,sonptr->minlevel);
	      			sonptr->maxlevel = (int)max(level,sonptr->maxlevel);
				/*for(int k=0;k<tlayers;k++){
					for(int l=0;l< 2<<(level);l++){
						if(son.bit[k]==l)cout << "x ";
						else cout << "0 ";
					}
					cout << endl;
				}
				*/

				marklin( sonptr, level+1,type,region);
			}
			
			if( insert_hitpattern  &&                          /* "insert pattern"
                                                          flag is set and    */
        			!nodeexists( Father->son[offs+flip], &son)) {
				nodptr = (nodenode*)malloc( sizeof( nodenode));    /* create a nodenode  */
				assert(nodptr);                                    /* cry if error       */
      				nodptr->next             = Father->son[offs+flip]; /* append it onto     
                                                          the son list       */
				nodptr->tree             = sonptr;
				Father->son[offs+flip] = nodptr;
				//cout << "Father's son " << offs+flip << " set" << endl;
			}

		}

		






	}
	else{
	while( i--){
		offs =1;
		maxs =0;
		flip =0;

		for(j=0;j<tlayers;j++){
			//cout << "for("<< j << "," << tlayers << "," <<
			//Father->bit[j] << "," << (1<<j) << "," << i << ")" << endl;
	

			if(i & (1<<j)){
				son.bit[j] = (Father->bit[j]<<1)+1;
				}
			else{
				son.bit[j] = Father->bit[j]<<1;
			}
			offs = (int)min(offs,son.bit[j]);
			maxs = (int)max(maxs,son.bit[j]);
		}
		son.bits = son.bit[tlayers-1] - son.bit[0];
		//cout << "(" << maxs << "," << offs << "," << son.bits << ")" << endl;
		if(maxs-offs > abs(son.bits)){
			//cout << "yes" << endl;
			continue;
		}
		
		if(offs)
			for(j=0;j<tlayers;j++)
				son.bit[j]--;
		
		if(son.bits < 0){
			flip =2 ;
			son.bits = -son.bits;
			for(j=0;j<tlayers;j++)
				son.bit[j] = son.bits-son.bit[j];
		}
		son.bits++;
		sonptr= nodeexists(Father->son[offs+flip],&son);
		hsh = (son.bit[tlayers-1]+son.bit[1])%hshsiz;
		cerr << "hsh = " << hsh << endl;
		insert_hitpattern = 1;
		
		if( !sonptr&& 0 == (sonptr= existent( &son, hsh))) {
			//cout << "Pattern is unknown" << endl;
			//cout << "-----------" << endl;
			//son.print();
			//cout << "-----------" << endl;
			if( consistent( &son, level+1,type,region)) {
				//cout << "Adding treenode..." << endl;
				sonptr = treedup( &son);
				sonptr->son[0] = sonptr->son[1] = 
	  			sonptr->son[2] = sonptr->son[3] = 0; 
				sonptr->maxlevel = 
	  			sonptr->minlevel = level;
				sonptr->genlink = generic[hsh];      
			       	generic[hsh] = sonptr;
				marklin( sonptr, level+1,type,region);
			}
			else
				insert_hitpattern = 0;
		}	
		else if( (sonptr->minlevel > level  && consistent( &son, level+1,type,region) )
	      ||	sonptr->maxlevel < level) {
			sonptr->minlevel = (int)min(level,sonptr->minlevel);
      			sonptr->maxlevel = (int)max(level,sonptr->maxlevel);
			marklin( sonptr, level+1,type,region);
		}
		//cout << "insert_hitpattern = " << insert_hitpattern << endl;

		if( insert_hitpattern  &&                          /* "insert pattern"
                                                          flag is set and    */
        !nodeexists( Father->son[offs+flip], &son)) {
			nodptr = (nodenode*)malloc( sizeof( nodenode));    /* create a nodenode  */
			assert(nodptr);                                    /* cry if error       */
      			nodptr->next             = Father->son[offs+flip]; /* append it onto     
                                                          the son list       */
			nodptr->tree             = sonptr;
			Father->son[offs+flip] = nodptr;
			//cout << "Father's son " << offs+flip << " set" << endl;
		}
	}
	}
}
//____________________________________________________________




void tree::treeout(treenode *tn, int level, int off){
	nodenode *nd;
  	int i,v;

  	if( level == maxlevel)         /* the level of the treenode is deeper     */
    		return;                      /* than the depth of the database.         */

  	for(i = 0; i < tlayers; i++) { /* loop over tree-detectors in hit pattern */ 
    		v = tn->bit[i];              /* the "on" bin for a tree-detector        */
    		if( off & 2)                 /* is it reversed?                         */
      			v = tn->bits-1-v;          /* yes, then flip it                       */
    		if( off & 1)                 /* is it offset?                           */
      			v++;                       /* yes, then apply the offset              */
    		printf("%d%*s|%*s*%*s|\n",level,level,"", /* print the bin for this     */
	   		v, "", tn->bits-1-v,"");           /* tree-detector              */
    		puts("");

    		for(i = 0; i < 4; i++) {     /* now loop over the four types (normal, 
                                    offset, flipped, flipped offset) of 
                                    the sons below this treenode            */
      			nd = tn->son[i];           /* nodenode for the son type               */
      			while( nd) {               /* loop over nodenodes of this type        */
				treeout( nd->tree, level+1, i); /* display this nodenode's treenode */
				nd = nd->next;           /* next son of this type bitte */
      			} 
    		} 
  	} /* of loop over tree-detectors */
}
//____________________________________________________________
void tree::freetree(){
	treenode *tn,*ltn; 
  	nodenode *nd,*lnd;
  	int i,j;

  	for( i = 0; i < HSHSIZ; i++) { /* loop over the entries in the genlink 
                                    hash table                            */

    		tn = generic[i];		 /* fetch the first treenode for this 
				    hash entry                            */
    		while(tn) {                  /* loop over the treenodes for this hash
                                    table entry                           */

      			for( j = 0; j < 4; j++) {  /* loop over the four types of sons
                                    connected to this treenode            */

				nd = tn->son[j];         /* fetch the first nodenode of this type */
				while(nd) {              /* loop over all nodenodes of this type  */
	  				nd = (lnd = nd)->next; /* lnd = pointer to treenode attached  
                                          to this nodenode                */
	                         /*  nd = next nodenode attached to this 
                                          nodenode                        */
	  				free(lnd);             /* free the memory for the treenode      */
				} /* end loop over nodenodes */

      			}
      			tn = (ltn = tn)->genlink;  /* ltn = this treenode                   */
                                 /*  tn = genlink treenode attached to
				          this treenode                   */
      			free(ltn);                 /* free the memory for this treenode     */

    		} /* end loop over genlink treenodes for a hash table entry */
	} /* end loop over the genlink hash table entries */

  	return;
}
//____________________________________________________________
treeregion * tree::readtree(char *filename, int levels, int tlayers, double rwidth, int dontread){
  	FILE *f = 0;
  	shorttree *stb;
  	treeregion *trr;
  	double width;
  	long num;

  	xref = 0;
  	if( !dontread) {
    		f = fopen( filename, "rb");
    		if( !f)
      			return 0;

    		if( fread(&num, sizeof(long), 1L, f) < 1 ||
		fread(&width, sizeof( width), 1L, f) < 1 ) {
      			fclose(f) ;
      			return 0;
    		}
		//cerr << "Num = " << num << endl << "width = " << width << endl;
    		stb = (shorttree *)malloc( num * sizeof(shorttree));
    		assert(stb);
  	} else {
    		width = rwidth;
    		stb = 0;
  	}
  	trr = (treeregion *)malloc( sizeof( treeregion));
  	maxref = num;
  	if( !dontread && _readtree( f, stb, 0, tlayers)) {
		
    		free(stb);
    		free(trr);
    		fclose(f);
    		stb = 0;
    		return 0;
  	}
  	if( !dontread )
    		fclose(f);
  	trr->searchable = stb ? true : false;
	//cerr << "searchable = " << trr->searchable << endl;
  	trr->node.tree  = stb;
  	trr->node.next  = 0;
  	trr->rWidth     = width;
	
  	return trr;
}
//____________________________________________________________
treeregion * tree::inittree(char *filename, int levels, int tlayer, double width,enum Etype type,enum ERegion region){
  	treeregion *trr;
  	treenode  *back;
  	tlayers  = tlayer;
  	maxlevel = levels+1;
  	detwidth = width;

  	if( tlayer == 0 )
    		return 0;
  	
  	/* ---------------- first: try to read it in ... */
  	if(0 == (trr = readtree(filename,levels,tlayer, width,0))|| trr->rWidth != width/*||1*/){ //FORCING PATTERN GENERATION) {
		//cerr << "pattern generation forced" << endl;
    		/* ------- if this doesnt work, generate own database -------*/
    		if( trr ) {
      			if( trr->node.tree )
				free(trr->node.tree);
      			free(trr);
    		}
    		fflush(stdout);
    		back = _inittree( tlayer,type,region);

    		if( !back) {
      			fprintf(stderr,"hrc: Tree couldn't be built.\n");
      			exit(1);
    		}
    		printf(" Generated.\n");
    		fflush(stdout);
    		/* ------- and write it to disk for later fast access ----- */
      
    		if( !writetree(filename, back, levels, tlayer, width)) {
      			fprintf(stderr,"hrc: Tree couldn't be written.\n");
      			perror(filename);
      			exit(1);
    		}
    		printf(" Cached.\n");
    		fflush(stdout);
    		freetree(); /* Free the tree structuere -*/
    		/*- and read it in again to get the short search format -*/
    		if( 0 == (trr = readtree(filename,levels,tlayer, width, 0))) {
      			fprintf(stderr,"hrc: New-Tree couldn't be read.\n");
      			perror(filename);
      			exit(1);
    		}
    		printf(" Done.\n");
  	}
  	return trr;


	
}
//____________________________________________________________
treenode * tree::_inittree(int tlayer,enum Etype type,enum ERegion region){
	treenode *ret = treedup(&father); /* generate a copy of the father to
                                        start off this treesearch database */
  	
  	memset(generic,0,sizeof(generic)); /* clear genlink hash table           */
  	marklin(ret, 0,type,region);		     /* call the recursive tree generator  */
  	ret->genlink = generic[0];	     /* finally, add the father to the 
                                                 genlink hash table        */
  	generic[0] = ret;
	cerr << "npat : " << npat<< endl;
  	return ret; /* return the generated treesearch database to caller */
}
//____________________________________________________________
int tree::_writetree(treenode *tn, FILE *fp, int tlayers){
  	int i;
  	nodenode *nd;
	//tn->print();
	if(tn->maxlevel<tn->minlevel)cerr << "("<<tn->minlevel<<","<<tn->maxlevel<<")"<<endl;
  	if( tn->xref == -1) {         /* pattern has never been written */
    		tn->xref = xref++;		/* set its reference */
		
    		if( fputc( REALSON, fp) == EOF || /* and write all pattern data */
       		fwrite(&tn->minlevel, sizeof( int), 1L, fp) != 1 ||
       		fwrite(&tn->bits, sizeof( int), 1L, fp) != 1 ||
       		fwrite(tn->bit,sizeof( int), (size_t)tlayers, fp) != (unsigned int)tlayers )
      			return -1;
    	for( i = 0; i < 4; i++) {
      		nd = tn->son[i];
      		while( nd) {
			if( _writetree(nd->tree,fp,tlayers))
	  			return -1;
			nd = nd->next;
      		}
      		if( fputc( SONEND, fp) == EOF) /* set the marker for end of sonlist */
			return -1;
    		}
  	} else {			/* else - only write a reference to */
    		if( fputc( REFSON, fp) == EOF  || /* a former written pattern */
       		fwrite( &tn->xref, sizeof(tn->xref), 1L, fp) != 1)
      			return -1;
  	}
  	return 0;
}
//____________________________________________________________
long tree::writetree(char *fn, treenode *tn, int levels, int tlayers, double width){
	
	//cout << "write tree called" << endl;
  	FILE *fp = 0;
  	//extern bool bWriteGlobal;
  
  	//if( bWriteGlobal ) <- ALLOWS FOR COMMAND LINE OPTION OF MAIN.C TO CHOOSE
	// WHETHER TO WRITE OR NOT
    	fp = fopen(fn, "wb");	/* open an output stream */
	
  	xref = 0;
  	if( !fp){		/* error checking */
		//cout << "debug4c" << endl;
    		char *fwsn = strrchr(fn,'/'); /* try to write local file */
    		if( fwsn++ ) {
			//cout << "debug4d" << endl;
      			fp = fopen( fwsn, "wb+");
      			if( fp )
			strcpy( fn, fwsn);
    		}
  	}
  	if( !fp){
		//cout << "debug4b" << endl;
    		return 0;
	}
  	/* --------------------- write 4 bytes to fill later */
  	if( fwrite( &xref, sizeof(long), 1L, fp) != 1 ||
     	fwrite( &width, sizeof( double), 1L, fp) != 1 ||
     	_writetree( tn, fp, tlayers) ) { /* ... and write whole tree*/
    		fclose(fp);
    		return 0;
  	}
  	if( fputc( SONEND, fp) == EOF) /* append marker for end of son list */
    		return -1;
  
  	rewind(fp);			/* now write the total numer of different */
  	if( fwrite( &xref, sizeof(long), 1L, fp) != 1) { /* patternes */
    		fclose(fp);
		//cout << "debug4" << endl;
    		return 0;
  	}
  	fclose(fp);			/* everything worked fine */
	//cout << "write finished" << endl;
  	return xref;
}
//____________________________________________________________

int tree::_readtree(FILE *f, shorttree *stb, shortnode **fath, int tlayers){
int c, sonny;
int Minlevel,Bits,Bit[TLAYERS];
long ref;

shortnode *node;
for(;;) {
  	c = fgetc( f);
	if( c == REALSON) {		/* in case of a real pattern read it 						in */
      		if( xref >= maxref ) {
			fputs("hrc: readtree failed. error #5. rebuilding treefiles.",stderr);
			return -1;
      		}
		if(fread(&Minlevel,sizeof(int),1L,f)!=1||
		fread(&Bits,sizeof(int),1L,f)!=1||
		fread(&Bit,sizeof(int)*tlayers,1L,f)!=1){
			fputs("hrc: readtree failed. error #1. rebuilding treefiles.",stderr);
			return -1;
		}
		
		ref = xref;
		xref++;
		(stb+ref)->minlevel = Minlevel;
		(stb+ref)->bits = Bits;
		for(int i=0;i<tlayers;i++){
			(stb+ref)->bit[i] = Bit[i];
		}
		/*
      		if( fread( stb+(ref = xref++), sizeof(int)*(2+tlayers), 1L, f) != 1) {
			fputs("hrc: readtree failed. error #1. rebuilding treefiles.",stderr);
			return -1;
      		}
		*/
		//(stb+ref)->print();
      		if( fath) {	/* and append this patterns to the fathers 					sons */
			node = (shortnode *)malloc( sizeof( shortnode));
			assert(node);
			node -> tree = stb+ref;
			node -> next = *fath;
			(*fath)      = node;
			//node->tree->print();
      		}
   
      		memset( stb[ref].son, 0, sizeof(stb[ref].son)); /* has no sons yet */
      		/* if( level < maxlevel -1) {  but try to read them in */
		
      		for(sonny = 0; sonny < 4; sonny++) {
			if( _readtree( f, stb, 
		      	stb[ref].son+sonny, tlayers)){
				cerr << "c";
	  			return -1;
			}
      		}
      /* } */
    	} else if( c == REFSON) {	/* in case of a reference - read in the ref */
      		if( fread( &ref, sizeof(ref), 1L, f) != 1) {
			fputs("hrc: readtree failed. error #1. rebuilding treefiles.",stderr);
			return -1;
      		}
      		if( ref >= xref || ref < 0) { /* some error checking */
			fputs("hrc: readtree failed. error #3. rebuilding treefiles.",stderr);
			return -1;
      		}
      		if( !fath) {		/* still some error checking */
			fputs("hrc: readtree failed. error #4. rebuilding treefiles.",stderr);
			return -1;
      		}
      		node = (shortnode *)malloc( sizeof( shortnode)); /* create a new node */
      		assert(node);
      		node -> tree = stb+ref;	/* and append the alread read-in tree */
      		node -> next = *fath;	/* 'ref' to this node. */
      		(*fath)      = node;
    	} else if( c == SONEND)	/* sonend means end reading */
      		break;
    	else {			/* this function constists of 50% err check */
      		fputs("hrc: readtree failed. error #2. rebuilding treefiles.",stderr);
      		return -1;
    	}
}
return 0;
}
//____________________________________________________________
void tree::printtree(treenode *tn){
	tn->print();

}
//____________________________________________________________
TreeLine::TreeLine(){}
TreeLine::~TreeLine(){}

