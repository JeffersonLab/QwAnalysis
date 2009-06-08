//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

 \class QwTrackingTree

 \brief The QwTrackingTree class contains the code for creating the tree search database.

 \verbatim

 PROGRAM: QTR (Qweak Track Reconstruction)	AUTHOR: Burnham Stokes
							bestokes@jlab.org
						ORIGINAL HRC AUTHOR
							Wolfgang Wander
							wwc@hermes.desy.de

 MODULE: QwTrackingTree

 \endverbatim

 PURPOSE: This module contains the code for creating the tree search
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

 (14) [c'tor]      - the main function of this module.  This function
                     calls inittree() to generate the tree database for
                     each of the treelines.

*//*-------------------------------------------------------------------------*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwTrackingTree.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

const string QwTrackingTree::TREEDIR("tree");

//extern QwTrackingTreeRegion *rcTreeRegion[kNumPackages][kNumRegions][kNumTypes][kNumDirections];
//extern QwTrackingTreeRegion *rcTreeRegion[kNumPackages*kNumRegions*kNumTypes*kNumDirections];
extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
extern Options opt;

/*! Defines are relevant for the storage of the trees in files. */
#define OFFS1   2 /* Next Sons have to be linked to offset 1 nodelist */
#define SONEND  3 /* End of son-description */
#define REALSON 4 /* Son and grandson description follows */
#define REFSON  5 /* Son reference follows */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \note (wdconinc) Merged out all those little classes from tree.cc and tree.h
    to separate class def files in directory and namespace QwTracking.  Proper
    include should be done, as well as using namespace QwTracking.  */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingTree::printtree (treenode* tn)
{
  tn->print();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

 \class QwTrackingTree

 \fn QwTrackingTree()

 \brief Initializes the search tree.

    This function loops over each detector region, orientation, type,
    and wire direction to call inittree.  It also determines a file name
    for each case.

*//*-------------------------------------------------------------------------*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTree::QwTrackingTree ()
{
  // Set debug level
  debug = 1;

  if( debug )
      cout<<"###### Calling QwTrackingTree::QwTrackingTree ()"<<endl;

  tlayers = 8; // set tlayers == maxhits for now (for region 3)

  hshsiz = 511;

  // Initialize the QwTrackingTree structure
  father.genlink = 0;
  for (int i = 0; i < 4; i++) father.son[i] = 0;
  father.maxlevel = -1;
  father.minlevel = -1;
  father.bits = 1;
  for (int i = 0; i < TLAYERS; i++) father.bit[i] = 0;
  father.xref = -1;
  npat = 0;

  if( debug )
      cout<<"###### Leaving QwTrackingTree::QwTrackingTree ()"<<endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTree::~QwTrackingTree ()
{

if( debug )
      cout<<"###### Calling QwTrackingTree::~QwTrackingTree ()"<<endl;

if( debug )
      cout<<"###### Leaving QwTrackingTree::~QwTrackingTree ()"<<endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

 \fn consistent()

 \brief Determines whether the pattern is geometrically possible.

*//*-------------------------------------------------------------------------*/

int QwTrackingTree::consistent(
	treenode *tst,
	int level,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir)
{
  //###############
  // DECLARATIONS #
  //###############
  int i;
  int *b = tst->bit;	/* For faster access to pattern in tst->bit */
  double x0;		/* Bitnumber in the first tree-detector,
                           i.e. treelayer 0                         */
  double x3e, x3a;	/* Bitnumber in last / last checked layer   */
  double adda, adde;
  double dze, dza;
  double zv;
  Det *rd;



  //###########
  // REGION 2 #
  //###########
  if (type == kTypeDriftHDC && region == kRegionID2) {
    int templayers = 4;
    int tlaym1 = templayers - 1;
    double z[templayers];
    double binwidth, y0, dy;

    double xmin, xmax;	/// the acceptable limits at the current plane
    double xf;		/// the position of the left(min) edge of the last layer
    double mL, mR;	/// the slopes which set the bounds for acceptable hits.
    double off;		/// radial offset between upstream and downstream HDC chambers
    double xiL, xiR;	/// the left(min) and right(max) edges of the bin at the current plane

    /// find the z position of each tree-detector relative to the first tree-detector
    for (rd = rcDETRegion[package][region-1][dir], i = 0;
         rd && i < templayers;
         rd = rd->nextsame, i++) {   // Loop through each plane

      zv = rd->Zpos;                 // Get z position of the plane

      if (i) {                       // Compute the relative position to the upstream plane
        z[i] = zv - z[0];
	if (z[i] < z[0]) cerr << "ERROR: R2 PLANES OUT OF ORDER" << endl;
	/// the offset distance between the first and last planes of this wire direction
	if (i == templayers-1) dy = off = fabs((rd->center[1] - y0)*rd->rCos);
      } else {
        z[0] = zv;
        binwidth = rd->NumOfWires * rd->WireSpacing / (1<<level); /// the binwidth at this level
	y0 = fabs(rd->center[1]); /// the first plane's radial distance
      }
    }
    z[0] = 0.0; // set the first plane's z position to zero

    /*  initial setting for the line check using the first and
           the last tree-detectors                                    */
    dza = z[tlaym1];
    x0 = b[0];               /* Fetch the pattern for 1st tree-det.    */
    xf = b[tlaym1];          /* Fetch the pattern for last tree-det.   */

    /*  first check if a straight track through the bins in the
           first and the last tree-detectors fulfill the max angle
           condition set in Qoptions     */
    dy -= x0*binwidth; /// dy is decreased by a larger first layer bin
    dy += xf*binwidth; /// and increased by a larger last layer bin

    if (fabs (dy / dza) > opt.R2maxslope) {
      return 0;
    }

    if (b[0] == 1 && b[1] == 1 && b[2] == 0 && b[3] == 0 /*&& level == 5*/) cerr << "gotcha" << endl;
    /* check if all the bits are along a straight line by
       looping through each pair of outer tree-detectors and
       seeing if the bins on the enclosed tree-detectors are
       along a straight line                                      */
    x0 = b[0] * binwidth;
    xf = b[tlaym1] * binwidth + off;
    mL = mR = (xf - x0) / z[tlaym1]; /// get the initial bounding slopes
    i = 2; /// check if the bin in the 3rd layer is within the bounds
    xmin = mL * z[i] + x0;
    xmax = mR * z[i] + x0 + binwidth;
    xiL = b[i] * binwidth + off;///this layer is offset
    xiR = (b[i] + 1) * binwidth + off;
    if (xiL > xmax || xiR < xmin) {
      return 0; /// if the hit is out of bounds, cut the pattern
    }
    if (xiR > xmax) mL = (xiL - x0) / z[i]; /// this hit requires the boundaries to be narrowed
    if (xiL < xmin) mR = (xiR - x0) / z[i];
    i = 1; /// check if the bin in the 2nd layer is within the bounds
    xmin = mL * z[i] + x0;
    xmax = mR * z[i] + x0 + binwidth;
    xiL = b[i] * binwidth; /// this layer is not offset
    xiR = (b[i] + 1) * binwidth;
    if (xiL > xmax || xiR < xmin) {
      return 0; /// if the hit is out of bounds, cut the pattern
    }
    return 1; /// else, this is a good pattern


  //###########
  // REGION 3 #
  //###########
  } else if (type == kTypeDriftVDC && region == kRegionID3) {

    int templayers = 8;


    double xf = 0, zf;
    double z[templayers];
    double cellwidth = 1; // distance between wires
    //double cellwidth = 1.11125;
    //cerr << cellwidth << endl;
    int firstnonzero = 0;

    for (i = 0; i < templayers; i++) {
      if (b[i]) firstnonzero++;
      if (firstnonzero && ! b[i]) templayers = i + 1;
    }

    /* ----- find the z position of each tree-detector relative to
             the first tree-detector                                   ----- */

    z[0] = 0;
    for (i = 1; i < templayers; i++) {
      z[i] = z[i-1] + cellwidth;
    }

    /* Get the layer with the largest bit value (dependent on rules in marklin)*/
    for (i = 0; i < templayers; i++) {
      if (b[i] >= xf) {
	zf = i;
	xf = b[i];
      }
    }

    /* ----- initial setting for the line check using the first and
             the last tree-detectors                                   ----- */
    dze = dza = zf;
    x0 = b[0];                      /* Fetch the pattern for 1st tree-det.   */
    x3a = x3e = xf;          /* Fetch the pattern for last tree-det.         */
    x3e++;

    /* ----- first check if a straight track through the bins in the
             first and the last tree-detectors fulfill the max angle
             condition                                                 ----- */
    double m_min = -((double) tlayers - 1) / ((double) (1 << level) - 1);
    double m_max = -(4.0 - 1.0) / ((double) (1 << level) - 1);
    double m = -((double) zf) / ((double) (xf - x0));

    //cout << m_min << " " << m << " " << m_max << endl;
    if (m < m_min || m > m_max) return 0;

    /* ----- check if all the bits are along a straight line by
             looping through each pair of outer tree-detectors and
             seeing if the bins on the enclosed tree-detectors are
             along a straight line                                     ----- */

    for (i = (int) zf - 1; i > 0; i--) {     /* loop from the back
                                             tree-detectors forward */
      adda = (x0 - b[i]) * dza + (x3a - x0) * z[i];
      if (adda <= -dza || dza <= adda)
        return 0;
      adde = (x0 - b[i]) * dze + (x3e - x0 - 1) * z[i];
      if (adde <= -dze || dze <= adde)
        return 0;
      if (i > 1) {
        if (adda > 0) {
	  x3e = b[i] + 1;
	  dze = z[i];
        }
        if (adde < 0) {
	  x3a = b[i];
	  dza = z[i];
        }
      }
    }
    return 1;


  //###########
  // OR ELSE  #
  //###########
  } else {
    cerr << "Warning: no support for the creation of this search tree." << endl;
    return 0;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

treenode* QwTrackingTree::existent (treenode *tst, int hash)
{
  treenode *walk = generic[hash];
  while (walk) {
    if (! memcmp (tst->bit, walk->bit, tlayers * sizeof(tst->bit[0])))
      return walk;		/* found it! */
    walk = walk->genlink;	/* nope, so look at the next pattern */
  }
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

treenode* QwTrackingTree::nodeexists (nodenode* node, treenode* tr)
{
  while (node) {
    if (! memcmp(node->tree->bit, tr->bit, tlayers * sizeof(tr->bit[0])))
      return node->tree;		/* found it! */
    node = node->next;		/* nope, so look at the next son of this father */
  }
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

treenode* QwTrackingTree::treedup (treenode *todup)
{
  // TODO (wdc) Copy constructor for treenode
  treenode* ret = new treenode;	/* allocate the memory for the treenode	*/
  assert(ret);			/* cry if there was an error		*/

  *ret = *todup;		/* copy the treenode todup to
				   the new treenode			*/
  ret->xref = -1L;		/* set the external reference
				   link					*/
  return ret;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

 \fn marklin

 \brief Generates the treesearch pattern database.

    This function generates the treesearch database.  For
    a given father, it generates the 2^(treelayers)
    possible son hit patterns.  Each son pattern is
    checked to see if it is consistent with a trajectory
    through the chamber.  If it is consistent, it is
    inserted into the treesearch database and then, by a
    recursive call to marklin(), its sons are generated.
    marklin has different code for the different regions
    due to the significant differences between them.

*//*-------------------------------------------------------------------------*/

void QwTrackingTree::marklin (
	treenode* father,
	int level,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir)
{
  //###############
  // DECLARATIONS #
  //###############
  treenode son;
  treenode *sonptr;
  int i,j;
  int offs;
  int flip;
  int maxs;
  int insert_hitpattern;
  int hsh;

  if (level == maxlevel) return;
  son = *father;
  i = (1 << tlayers);	// (1 << x) is equal to 2^x
			/* Number of possible son patterns for this father */

  //###########
  // REGION 2 #
  //###########
  if (region == kRegionID2 && type == kTypeDriftHDC) {
    tlayers = 4; /// Four u, v, or x wire planes an electron can cross
    i = (1 << tlayers);
    while (i--) {    //loop through all possibilities
      offs = 1;
      maxs = 0;
      flip = 0;
      for (j = 0; j < tlayers; j++) {
	if (i & (1 << j)) {
	  son.bit[j] = (father->bit[j]<<1) + 1;
	} else {
	  son.bit[j] = (father->bit[j]<<1);
	}
	offs = (int) min (offs, son.bit[j]);
	maxs = (int) max (maxs, son.bit[j]);

      }

      son.bits = son.bit[tlayers-1] - son.bit[0];//width in bins between first and last planes

      /* check that the hits on the internal tree-detector planes
         are enclosed by the hits on the two outer tree-detector
         planes.  If not, the pattern cannot have a straight line
         put through it, so just stop with it.

         For example, a straight line cannot be put through the
         pattern 1 3 2 because the bin for the middle tree-
         detector (3) is not enclosed by the bins for the two
         outer detectors (1 and 2).                             */


      if (maxs - offs > abs(son.bits))
        continue;

      /* compute the offset of this hit pattern and, if non-zero,
         shift the pattern over by the offset                   */

      if (offs) {			/* If there is an offset, so         */
        for (j = 0; j < tlayers; j++)	/* shift all hits over this offset   */
	  son.bit[j]--;
      }
      /* see if the hit pattern is a flipped pattern and, if so,
         set the "pattern is flipped" flag and flip the pattern */


      if (son.bits < 0) {		  /* If hit pattern is flippable, then      */
        flip = 2;			  /* (1) set "pattern is flipped" flag, and */
        son.bits = -son.bits;	  /* (2) flip the hit pattern               */
//bits B
        for (j = 0; j < tlayers; j++)
	  son.bit[j] = son.bits-son.bit[j];
      }

      /* compute the width (in bins) of the hit pattern          */
      son.bits++;/* Remember: bins are numbered from 0 to
//bits C
      n, so we need to add 1 here to compute the width         */

      /* look at the other sons of this father to see if this
         particular son is already known to the father           */

      sonptr = nodeexists (father->son[offs+flip], &son);

      /*  compute the hash value of this particular son for use
         with the genlink hash search of the full treesearch
         database                                                  */
      hsh = (son.bit[tlayers-1] + son.bit[1]) % HSHSIZ;

      /*  initializes the "insert pattern" flag                     */
      insert_hitpattern = 1; /* right now, set this flag to insertt it  */

      /* if the hit pattern for this son was not located when
         the other sons of this father were searched, then look
         through the entire treesearch database for this
         particular hit pattern.  If the hit pattern is still not
         found, then consider inserting it into the treesearch
         database.  Otherwise, adjust the treenode with the like
         hit pattern so that its hit pattern is valid at this
         level of the treesearch division.
                                                                   */

      if (! sonptr && 0 == (sonptr = existent (&son, hsh))) {
        /* the pattern is completely unknown.  So, now check if
           it is consistent with a straight line trajectory
           through the tree-detectors whose slope is within the
           window set by the Qoptions parameter R2maxslope.             */

        if (consistent (&son, level+1, package, type, region, dir)) {
          /* the pattern is consistent, so now insert it into the
             treesearch database by:                              */
	  /*  1st: Create space for this new treenode             */
	  sonptr = treedup (&son);

	  /*  2nd: Since this treenode has no sons at the moment,
              zero the son pointers for this treenode             */
	  sonptr->son[0] = sonptr->son[1] =
	    sonptr->son[2] = sonptr->son[3] = 0;

	  /*  3rd: Since the hit pattern in this treenode is only
              known to be valid at this level of bin divsion,
              set the minimum and maximum valid level for this
              treenode to this level                              */

	  sonptr->maxlevel = sonptr->minlevel = level;

	  /*  4th: Update the genlink hash table so that this
              treenode will be examined during future searches
              of the entire treesearch database.                  */
	  sonptr->genlink = generic[hsh];      /* Append the pattern onto the    */
                                               /* end of genlink hash table.     */
	  generic[hsh] = sonptr;               /* update the genlink hash table. */

	  /*  5th: Call marklin() recursively to generate the sons of
              this tree node                                      */
	  npat++;
          /*
	  cout << "good" <<endl;
	  for (int k = 0; k < tlayers; k++) {
	    for (int l = 0; l < (2 << level); l++) {
	      if (son.bit[k] == l)
	        cout << "x ";
	      else
	        cout << "0 ";
	    }
	    cout << endl;
	  }
          */
	  marklin (sonptr, level+1, package, type, region, dir);

        } else {

	  /*  the pattern is not consistent with a line, so just
              reject it.                                               */
	  insert_hitpattern = 0; /* set "insert pattern" flag to not keep it */
          /*
	  cout << "inconsistent" << endl;
	  for (int k = 0; k < tlayers; k++) {
	    for (int l = 0; l < 2 << level; l++) {
	      if (son.bit[k] == l)
	        cout << "x ";
	      else
	        cout << "0 ";
	    }
	    cout << endl;
	  }
          */
	}
	/* the pattern was found when the entire database was
           searched above, so now the matching treenode for it
           needs to be updated so that it will be valid this level
           of bin division.                                         */

      } else if ((sonptr->minlevel > level && consistent(&son, level+1, package, type, region, dir))
	       || sonptr->maxlevel < level) {

	/*  1st: Update the levels of the found treenode to
            include this level                                  */
        sonptr->minlevel = (int) min (level, sonptr->minlevel);
	sonptr->maxlevel = (int) max (level, sonptr->maxlevel);

	/*  2nd: Update the levels of all the sons for this
            treenode
                                      */
          /*
	  cout << "good" <<endl;
	  for (int k = 0; k < tlayers; k++) {
	    for (int l = 0; l < 2 << level; l++) {
	      if (son.bit[k] == l)
	        cout << "x ";
	      else
	        cout << "0 ";
	    }
	    cout << endl;
	  }
          */
        npat++;
	marklin (sonptr, level+1, package, type, region, dir);
      }

	/* Since one of the recursive call to marklin()
           for building a son's generation could have already
           inserted the hit pattern for this son into the
           database, a final check is made to see if the hit
           pattern is already in the database before actually
           inserting the treenode into the database                 */
      if (insert_hitpattern  &&                        /* "insert pattern"
                                                          flag is set and    */
        ! nodeexists (father->son[offs+flip], &son)) { /* final check if hit
                                                          pattern is already
							  in the database    */

        nodenode *nodptr = new nodenode;		/* create a nodenode  */
	assert(nodptr);                                  /* cry if error     */
        nodptr->next           = father->son[offs+flip]; /* append it onto
                                                            the son list     */
	nodptr->tree           = sonptr;
	father->son[offs+flip] = nodptr;
      }
    }
  }

  //###########
  // REGION 3 #
  //###########
  else if (region == kRegionID3 && type == kTypeDriftVDC) {
    tlayers = 8;
    offs = 1;
    maxs = 0;
    flip = 0;
    int maxhits = 8; // max # of cells that can be hit in wanted tracks
    i = (1 << maxhits);
    while (i--) { // loop through all possibilities
      for (j = 0; j < maxhits; j++) { // this loop creates each possible pattern

	if(i & (1<<j)){
	  son.bit[j] = (father->bit[j]<<1)+1;
	}
	else{
	  son.bit[j] = father->bit[j]<<1;
	}
	offs = (int)min(offs,son.bit[j]);
	maxs = (int)max(maxs,son.bit[j]);
      }


      //Cut patterns in which there are hits that lie outside the road         between the first and last hits, i.e. :
      //  X 0      0 X
      //  0 X  or  X 0
      //  0 X      0 X
      //  X 0       0 X
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
        for( j = 0; j< maxhits; j++) /* shift all hits over this                   offset   */
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
      sonptr = nodeexists( father->son[offs+flip], &son);
      hsh = (son.bit[tlayers-1]+son.bit[1])%HSHSIZ;


      /*if(sonptr){for(j=0;j<maxhits;j++)cout << son.bit[j] << " " ;cout <<  "exists" << endl;}
      else{ for(j=0;j<maxhits;j++)cout << son.bit[j] << " " ;cout << endl;}
      cout << "hsh = " << son.bit[tlayers-1] << "," << son.bit[1] << "," << hshsiz << "," << hsh << endl;
      cout << "level : " << level << endl;
      cout << "bits : " << son.bits << endl;*/
      insert_hitpattern = 1;
      if( !sonptr&& 0 == (sonptr= existent( &son, hsh))){
        if( consistent( &son, level+1,package,type,region,dir)) {
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
          marklin( sonptr, level+1,package,type,region,dir);
        }
        else{/*
          cout << "inconsistent" << endl;
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
      else if( (sonptr->minlevel > level  && consistent( &son, level+1,package,type,region,dir) )||sonptr->maxlevel < level) {
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

        marklin( sonptr, level+1,package,type,region,dir);
      }

      if( insert_hitpattern  &&                          /* "insert pattern"
                                                          flag is set and    */
              !nodeexists( father->son[offs+flip], &son)) {

        nodenode* nodptr = new nodenode;		/* create a nodenode  */
        assert(nodptr);                                    /* cry if error       */
        nodptr->next             = father->son[offs+flip]; /* append it onto
                                                          the son list       */
        nodptr->tree             = sonptr;
        father->son[offs+flip] = nodptr;
        //cout << "father's son " << offs+flip << " set" << endl;
      }

    }


  //########
  // OTHER #
  //########
  } else {

    while (i--) {
      offs = 1;
      maxs = 0;
      flip = 0;

      for (j = 0; j < tlayers; j++) {
        //cout << "for("<< j << "," << tlayers << "," <<
        //father->bit[j] << "," << (1<<j) << "," << i << ")" << endl;


        if(i & (1<<j)){
          son.bit[j] = (father->bit[j]<<1)+1;
          }
        else{
          son.bit[j] = father->bit[j]<<1;
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
      sonptr= nodeexists(father->son[offs+flip],&son);
      hsh = (son.bit[tlayers-1]+son.bit[1])%hshsiz;
      cerr << "hsh = " << hsh << endl;
      insert_hitpattern = 1;

      if( !sonptr&& 0 == (sonptr= existent( &son, hsh))) {
        //cout << "Pattern is unknown" << endl;
        //cout << "-----------" << endl;
        //son.print();
        //cout << "-----------" << endl;
        if( consistent( &son, level+1,package,type,region,dir)) {
          //cout << "Adding treenode..." << endl;
          sonptr = treedup( &son);
          sonptr->son[0] = sonptr->son[1] =
          sonptr->son[2] = sonptr->son[3] = 0;
          sonptr->maxlevel =
          sonptr->minlevel = level;
          sonptr->genlink = generic[hsh];
          generic[hsh] = sonptr;
          marklin( sonptr, level+1,package,type,region,dir);
        }
        else
          insert_hitpattern = 0;
      }
      else if( (sonptr->minlevel > level  && consistent( &son, level+1,package,type,region,dir) )
      || sonptr->maxlevel < level) {
        sonptr->minlevel = (int)min(level,sonptr->minlevel);
        sonptr->maxlevel = (int)max(level,sonptr->maxlevel);
        marklin( sonptr, level+1,package,type,region,dir);
      }
      //cout << "insert_hitpattern = " << insert_hitpattern << endl;

      if( insert_hitpattern  &&                          /* "insert pattern"
                                                            flag is set and    */
          !nodeexists( father->son[offs+flip], &son)) {

        nodenode* nodptr = new nodenode;		/* create a nodenode  */
        assert(nodptr);                                    /* cry if error       */
        nodptr->next             = father->son[offs+flip]; /* append it onto
                                                            the son list       */
        nodptr->tree             = sonptr;
        father->son[offs+flip] = nodptr;
        //cout << "father's son " << offs+flip << " set" << endl;
      }
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingTree::treeout (treenode *tn, int level, int off)
// TODO (wdc) wtf? nested double loop over i???  this can't be right!
{
  nodenode *nd;
  int v;

  if (level == maxlevel)	/* the level of the treenode is deeper     */
    return;			/* than the depth of the database.         */

  for (int i = 0; i < tlayers; i++) {  /* loop over tree-detectors in hit pattern */
    v = tn->bit[i];		/* the "on" bin for a tree-detector        */
    if (off & 2)		/* is it reversed?                         */
      v = tn->bits - 1 - v;	/*   yes, then flip it                     */
    if (off & 1)		/* is it offset?                           */
      v++;			/*   yes, then apply the offset            */
    printf("%d%*s|%*s*%*s|\n", level, level, "",/* print the bin for this  */
      v, "", tn->bits - 1 - v, "");		/* tree-detector           */
    puts("");

    for (i = 0; i < 4; i++) {	/* now loop over the four types (normal,
                                   offset, flipped, flipped offset) of
                                   the sons below this treenode            */
      nd = tn->son[i];		/* nodenode for the son type               */
      while (nd) {		/* loop over nodenodes of this type        */
        treeout(nd->tree, level+1, i); /* display this nodenode's treenode */
        nd = nd->next;                 /* next son of this type bitte */
      }
    }
  } /* end of loop over tree-detectors */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingTree::freetree()
{
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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \fn QwTrackingTree::readtree
    @param filename	The filename of the file where the tree is stored
    @param levels	The number of levels
    @param tlayers	The number of layers
    @param rwidth	The distance between the wires/strips
    @param dontread	Variable to bypass reading stored trees

    @return		The search treeregion
 */

QwTrackingTreeRegion* QwTrackingTree::readtree (
	char *filename,
	int levels,
	int tlayers,
	double rwidth,
	int dontread)
{
	FILE *f = 0;

	shorttree *stb;
	QwTrackingTreeRegion *trr;

	double width;
	long num;

	xref = 0;
	if (!dontread) {
		/// Open the file for reading and complain if this fails
		f = fopen(filename, "rb");
		if (!f) {
			cerr << "Error: file not found!" << filename << endl;
			return 0;
		}

		/// If num and width cannot be read, then the file is invalid
		if (fread(&num, sizeof(num), 1L, f) < 1 ||
		    fread(&width, sizeof(width), 1L, f) < 1 ) {
			cerr << "Error: file appears invalid!" << filename << endl;
			fclose(f);
			return 0;
		}
		// cout << "Num = " << num << endl;
		// cout << "Width = " << width << endl;

		/// Allocate a shorttree array
		stb = new shorttree[num];
		assert(stb);

	} else {

		width = rwidth;
		stb = 0;
	}

	/// Allocate a QwTrackingTreeRegion object
	trr = new QwTrackingTreeRegion;
	maxref = num;
	/// ... and fill by recursively calling _readtree
	if (!dontread && _readtree (f, stb, 0, tlayers)) {
		free(stb);
		free(trr);
		fclose(f);
		stb = 0;
		return 0;
	}

	/// Close the file after reading in the tree
	if (!dontread) fclose(f);

	trr->searchable = (stb ? true : false);
	if (debug) cout << "Set searchable = " << trr->searchable << endl;

	trr->node.tree  = stb;
	trr->node.next  = 0;
	trr->rWidth     = width;

	return trr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! This function checks whether a pattern database requested by rcInittree
    already exists.  If not, it will call _inittree to create the database,
    write it out, then read it in again.  To force the generation of new databases,
    simply remove the old ones from the 'trees' directory.  If options are set
    to create a different level of pattern resolution, this function will
    automatically create the new databases.
 */

QwTrackingTreeRegion* QwTrackingTree::inittree (
	char *filename,
	int levels,
	int tlayer,
	double width,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir)
{
// TODO: This routine assumes that the directory 'trees' exists and doesn't create it itself. (wdconinc)
  QwTrackingTreeRegion *trr;
  treenode  *back;
  tlayers  = tlayer;
  maxlevel = levels+1;
  detwidth = width;
  if (tlayer == 0)
    return 0;

  // TODO: I disabled the next part to get past a segfault (wdconinc, 08-Dec-2008)
  // (wdc, 29-May-2009) no wonder it segfaults, no malloc/new yet on trr
//   cout << "trr->searchable = " << (trr? trr->searchable:-1) << endl;
//   if (region == kRegionID3 && (dir == kDirectionX || dir == kDirectionY)) {
//     /// region 3 does not have an x or y direction,
//     /// so tag them as unsearchable for the treedo function.
//     trr->searchable = 0;
//     return trr;
//   }

  /*! Try to read in an existing database */
  int dontread = 0;
  if (0 == (trr = readtree(filename, levels, tlayer, width, dontread)) ) {

    //cerr << package << " " << type << " " << region << endl;
    //cerr << "pattern generation forced" << endl;
    /// If reading in doesn't work, clean up any partial trees
    /// that might have been read in already
    if( trr ) { // TODO Replace this free() and flush() stuff.
      if( trr->node.tree )
        free(trr->node.tree);
      free(trr);
    }
    fflush(stdout);

    /// Generate a new tree database
    back = _inittree (tlayer, package, type, region, dir);

    if( !back ) {
      cerr << "QTR: Tree couldn't be built.\n";
      exit(1);
    }
    cout << " Generated.\n";
    fflush(stdout);

    /// Write the generated tree to disk for faster access later
    if( !writetree(filename, back, levels, tlayer, width)) {
      cerr << "QTR: Tree couldn't be written.\n";
      exit(1);
    }
    cout << " Cached.\n";
    fflush(stdout);

    /// Free the tree structure
    //freetree();
    /// and read it in again to get the shorter tree search format
    if( 0 == (trr = readtree(filename,levels,tlayer, width, 0))) {
      cerr << "QTR: New tree couldn't be read.\n";
      exit(1);
    }
    cout << " Done.\n";
  }
  return trr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! This function performs initilization before calling marklin,
    the iterative pattern generator.  the root treenode is created
    as the simplest pattern possible, and passed to marklin.
 */

treenode* QwTrackingTree::_inittree (
	int tlayer,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir)
{
  treenode *ret = treedup(&father); /// generate a copy of the father to start off this treesearch database
  memset (generic, 0, sizeof(generic)); /// clear genlink hash table
  marklin (ret, 0, package, type, region, dir);///call the recursive tree generator
  ret->genlink = generic[0];/// finally, add the father to the genlink hash table
  generic[0] = ret;
  cerr << "npat : " << npat << " " << region << " " << dir << endl;
  npat = 0;
  return ret;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! This function iteratively writes the patterns to the database.
 */

int QwTrackingTree::_writetree (treenode *tn, FILE *fp, int tlayers)
{
  nodenode* nd;
  //tn->print();///use this for debugging.
  if (tn->xref == -1) {/// pattern has never been written
    tn->xref = xref++;/// set its reference

    if (fputc (REALSON, fp) == EOF || /// and write all pattern data
        fwrite(&tn->minlevel, sizeof(int), 1L, fp) != 1 ||
        fwrite(&tn->bits,     sizeof(int), 1L, fp) != 1 ||
        fwrite(tn->bit,       sizeof(int), (size_t) tlayers, fp) != (unsigned int)tlayers )
      return -1;

    for (int i = 0; i < 4; i++) {
      nd = tn->son[i];
      //cout << "son" << endl;
      while (nd) {///write the sons of this treenode (and their sons, etc)
        if (_writetree(nd->tree, fp, tlayers))
          return -1;
        nd = nd->next;
        //cout << "bro" << endl;
      }
      if (fputc (SONEND, fp) == EOF) /// set the marker for end of sonlist
        return -1;
    }
  } else { /// else - only write a reference to
    if (fputc (REFSON, fp) == EOF  || /// a former written pattern
        fwrite (&tn->xref, sizeof(tn->xref), 1L, fp) != 1)
      return -1;
  }
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \brief This function initializes the output file.
    It then calls the iterative _writetree function to fill the output file.

    @return	The number of patterns written to the file
 */

long QwTrackingTree::writetree (
	char *filename,
	treenode *tn,
	int levels,
	int tlayers,
	double width)
{
	// Ensure that the tree directory is created correctly
	bfs::path treedirpath((std::string(getenv("QWANALYSIS")) + "/" + TREEDIR).c_str());
	if (! bfs::exists(treedirpath)) {
		bfs::create_directory(treedirpath);
		if (debug) cout << "[QwTrackingTree::writetree] Created tree directory." << endl;
	}
	if (! bfs::exists(treedirpath) || ! bfs::is_directory(treedirpath)) {
		cerr << "[QwTrackingTree::writetree] Error: could not create tree directory!" << endl;
		return 0;
	}

	// Open the output stream
	FILE *file = fopen(filename, "wb");
	xref = 0;
	if (!file) { /// error checking
		char *fwsn = strrchr(filename,'/'); /// try to write local file
		if (fwsn++) {
			file = fopen(fwsn, "wb+");
			if (file)
				strcpy(filename, fwsn);
		}
	}

	if (!file) {
		return 0;
	}

	/// Write 4 bytes to fill later with the number of different patterns
	if (fwrite(&xref,  sizeof(long),   1L, file) != 1 ||
	    fwrite(&width, sizeof(double), 1L, file) != 1 ||
	   _writetree(tn, file, tlayers)) { ///... and write whole tree
		fclose(file);
		return 0;
	}
	if (fputc(SONEND, file) == EOF) /// append marker for end of son list
		return -1;

	rewind(file);

	/// Now write the total numer of different patterns,
	if (fwrite(&xref, sizeof(long), 1L, file) != 1) {
		fclose(file);
		return 0;
	}
	/// close the file
	fclose(file);

	/// and return the number of different patterns
	return xref;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \brief This function reads in the tree pattern database from a file.

    It uses the shorttree and shortnode objects in order to reduce memory overhead.

    @param	file	The opened file
    @param	stb	A shorttree
    @param	fath	A list of father nodes
    @param	tlayers	The number of layers(?)

    @return	Returns negative on error, zero otherwise
 */

int QwTrackingTree::_readtree(FILE *file, shorttree *stb, shortnode **fath, int tlayers)
{
  int c, sonny;
  int Minlevel, Bits, Bit[TLAYERS];
  long ref;

  /// Go into an infinite loop while reading the file
  for(;;) {
	/// Read the next record type
	c = fgetc(file);

	/// \li If we encounter a real pattern, then ...
	if (c == REALSON) {

		if (xref >= maxref) {
			cerr << "QTR: readtree failed. error #5. rebuilding treefiles." << endl;
			return -1;
		}

		/// read in the node(?) information
		if (fread(&Minlevel, sizeof(int),         1L, file) != 1 ||
		    fread(&Bits,     sizeof(int),         1L, file) != 1 ||
		    fread(&Bit,      sizeof(int)*tlayers, 1L, file) != 1) {
			cerr << "QTR: readtree failed. error #1. rebuilding treefiles." << endl;
			return -1;
		}

		ref = xref;
		xref++;
		(stb+ref)->minlevel = Minlevel;
		(stb+ref)->bits = Bits;
		for (int i = 0; i < tlayers; i++) {
			(stb+ref)->bit[i] = Bit[i];
		}
		if (fath) { /// ... and append the patterns to the father's sons
			shortnode* node = new shortnode;
			assert(node);
			node -> tree = stb+ref;
			node -> next = *fath;
			(*fath)      = node;
			// node->tree->print(); /// use this for debugging purposes
		}

		memset (stb[ref].son, 0, sizeof(stb[ref].son)); /// has no sons yet

		/// Read in the sons of this node
		for(sonny = 0; sonny < 4; sonny++) {
			if (_readtree(file, stb, stb[ref].son + sonny, tlayers)) {
				cerr << "c";
				return -1;
			}
		}

	/// \li If we encounter a reference, then read in the reference
	} else if (c == REFSON) {

		if (fread (&ref, sizeof(ref), 1L, file) != 1) {
			cerr << "QTR: readtree failed. error #1. rebuilding treefiles." << endl;
			return -1;
		}
		if (ref >= xref || ref < 0) { /// some error checking
			cerr << "QTR: readtree failed. error #3. rebuilding treefiles." << endl;
			return -1;
		}
		if (!fath) { /// still some error checking
			cerr << "QTR: readtree failed. error #4. rebuilding treefiles." << endl;
			return -1;
		}
		shortnode* node = new shortnode; /// create a new node
		assert(node);
		node -> tree = stb+ref;	/// and append the alread read-in tree
		node -> next = *fath;	/// 'ref' to this node.
		(*fath)      = node;

	/// \li If we encounter an end token, then stop
	} else if( c == SONEND) {
		break;

	/// \li If we encounter something else, then fail
	} else {
		cerr << "QTR: readtree failed. error #2. rebuilding treefiles." << endl;
		return -1;
	}
  }

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....