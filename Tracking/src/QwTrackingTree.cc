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
                     the fMaxSlope parameter.

 (02) existent()   - this function checks if a possible treeline hit
                     pattern is already included in the tree search
                     database.  If so, it returns a pointer to the pattern
                     within the database.  Otherwise, it returns 0.

 (03) nodeexists() - this function checks if a possible treeline hit
                     pattern is already known to a father.  If so, it
                     returns a pointer to the pattern within the tree
                     database.  Otherwise, it returns 0.

 (05) marklin()    - this function generates the treesearch database.  For
                     a given father, it generates the 2^(treelayers)
                     possible son hit patterns.  Each son pattern is
                     checked to see if it is consistent with a trajectory
                     through the chamber.  If it is consistent, it is
                     inserted into the treesearch database and then, by a
                     recursive call to marklin(), its sons are generated.

 (07) _inittree()  - this function initializes the treesearch database and
                     then calls marklin() to generate the database.

 (08) _writetree() - a recursive function for pulling in the concise treesearch
                     search database.

 (09) writetree()  - this function calls _writetree() to write the long
                     version of the treesearch database to a disk file.  Later,
                     readtree() will read back this file to form the concise
                     treesearch database (so-called short tree) used by the
                     treesearch algorithm.

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

#include "QwTrackingTree.h"

// Qweak headers
#include "QwLog.h"
#include "QwTypes.h"
#include "QwTrackingTreeRegion.h"

// Deprecated Qweak headers
#include "Det.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

const std::string QwTrackingTree::fgTreeDir("tree");

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

/**
 * Print the full tree
 */
void QwTrackingTree::PrintTree() const
{
  // The first entry of the hash table is the root of the tree.  The node fFather
  // is only the root node that is copied before the tree is constructed, so it
  // stays empty.
  QwOut << "Tree:" << QwLog::endl;
  fHashTable[0]->Print();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrackingTree::PrintHashTable() const
{
  QwOut << "Hash table:" << QwLog::endl;
  for (int i = 0; i < fHashSize; i++) {
    QwOut << "hash " << i << ":" << QwLog::endl;
    treenode* node = fHashTable[i];
    while (node) {
      QwOut << node << ": " << *node << QwLog::endl;
      node = node->GetNext();
    }
  }
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

QwTrackingTree::QwTrackingTree () : fNumPlanes(fNumLayers),fNumWires(fNumLayers)
{
  fDebug = 0; // debug level

  fNumLayers = 8; // set fNumLayers == fNumWires = 8 for now (for region 3)
  // fNumPlanes = fNumLayers is number of region 2 HDC planes
  // fNumWires  = fNumLayers is number of region 3 VDC wires per group

  // Until variable hash table sizes are implemented, the local hash size should
  // be equal to the value defined in the header.
  fHashSize = HSHSIZ;
  fHashTable = new treenode*[fHashSize];

  // Initialize the QwTrackingTree structure
  fFather = new treenode();
  fFather->fMaxLevel = -1;
  fFather->fMinLevel = -1;
  fFather->bits = 1;
  for (unsigned int i = 0; i < fFather->size(); i++) fFather->bit[i] = 0;
  fFather->xref = -1;

  // Reset the number of patterns generated
  fNumPatterns = 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTree::~QwTrackingTree ()
{
  // Debug information
  QwDebug << "Deleting QwTrackingTree: " << this << QwLog::endl;

  // Delete all top links in the hash table
  QwDebug << "Deleting fHashTable..." << QwLog::endl;
  for (int i = 0; i < fHashSize; i++) {
    treenode* node = fHashTable[i];
    while (node) {
      treenode* node_next = node->GetNext();
      delete node;
      node = node_next;
    }
  }
  // Delete hash table itself
  delete[] fHashTable;

  // Delete father node
  QwDebug << "Deleting fFather..." << QwLog::endl;
  delete fFather;

  // Report memory statistics
  if (treenode::GetCount() > 0 || nodenode::GetCount() > 0) {
    QwMessage << "Memory occupied by tree objects (should be close to zero when all trees cleared):" << QwLog::endl;
    QwMessage << "- allocated treenode objects: " << treenode::GetCount() << QwLog::endl;
    QwMessage << "- allocated nodenode objects: " << nodenode::GetCount() << QwLog::endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * \brief Determines whether the pattern is geometrically possible
 *
 *  ...
 *
 * @param tst
 * @param level
 * @param package
 * @param type
 * @param region
 * @param dir
 * @return
 */
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

    y0 = binwidth = dy = off = 0.0;

    /// find the z position of each tree-detector relative to the first tree-detector
    for (rd = rcDETRegion[package][region][dir], i = 0;
         rd && i < templayers;
         rd = rd->nextsame, i++) {   // Loop through each plane

      zv = rd->Zpos;                 // Get z position of the plane

      if (i) {                       // Compute the relative position to the upstream plane
        z[i] = zv - z[0];
	if (z[i] < z[0]) {
	  QwError << "Region 2 planes are out of order" << QwLog::endl;
	  exit(1);
	}
	/// the offset distance between the first and last planes of this wire direction
	if (i == templayers-1) dy = off = fabs((rd->center[1] - y0)*rd->rCos);
      } else {
        z[0] = zv;
        binwidth = rd->NumOfWires * rd->WireSpacing / (1 << level); /// the binwidth at this level
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
           condition set in QwOptions     */
    dy -= x0 * binwidth; /// dy is decreased by a larger first layer bin
    dy += xf * binwidth; /// and increased by a larger last layer bin

    if (fabs (dy / dza) > fMaxSlope) {
      return 0;
    }

    if (b[0] == 1 && b[1] == 1 && b[2] == 0 && b[3] == 0 /*&& level == 5*/)
      cout << "gotcha" << endl;
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


    double xf = 0, zf = 0.0;
    double z[templayers];
    double cellwidth = 1; // distance between wires
    //double cellwidth = 1.11125;
    //cout << cellwidth << endl;
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
    double m_min = -((double) fNumLayers - 1) / ((double) (1 << level) - 1);
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
    QwWarning << "Warning: no support for the creation of this search tree." << QwLog::endl;
    return 0;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * \brief Search for a node in the search tree
 *
 *  Starting with the hash value, the node is searched in the search tree.
 *  When the bits are identical to an entry in the search tree, that entry
 *  is returned.  Otherwise, when no match is found, null is returned.
 *
 * @param node Node to search for
 * @param hash Hash value of the node
 * @return Node in the tree, or null if not found
 */
treenode* QwTrackingTree::existent (treenode *node, int hash)
{
  treenode *walk = fHashTable[hash];
  while (walk) {
    if (! memcmp (node->bit, walk->bit, fNumLayers * sizeof(node->bit[0])))
      return walk;          /* found it! */
    walk = walk->GetNext(); /* nope, so look at the next pattern */
  }
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

treenode* QwTrackingTree::nodeexists (nodenode* node, treenode* tr)
{
  while (node) {
    if (! node->GetTree()) {
      QwError << "Floor gone from under my feet!" << QwLog::endl;
      return 0;
    }
    if (! memcmp(node->GetTree()->bit, tr->bit, fNumLayers * sizeof(tr->bit[0])))
      return node->GetTree();		/* found it! */
    node = node->GetNext();		/* nope, so look at the next son of this father */
  }
  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * \brief Generate the treesearch pattern database.
 *
 *  This function generates the treesearch database.  For
 *  a given father, it generates the 2^(treelayers)
 *  possible son hit patterns.  Each son pattern is
 *  checked to see if it is consistent with a trajectory
 *  through the chamber.  If it is consistent, it is
 *  inserted into the treesearch database and then, by a
 *  recursive call to marklin(), its sons are generated.
 *  marklin has different code for the different regions
 *  due to the significant differences between them.
 *
 * @param father Father node
 * @param level Level of precision
 * @param package Detector package
 * @param type Detector type
 * @param region Detector region
 * @param dir Detector direction
 */
void QwTrackingTree::marklin (
	treenode* father,
	int level,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir)
{
  // Local copy of the father node
  treenode son = *father;

  // If we have reached the maximum depth of the tree, do nothing.
  if (level == fMaxLevel) return;

  //###########
  // REGION 2 #
  //###########
  if (region == kRegionID2 && type == kTypeDriftHDC) {

    // There are four u, v, or x wire planes.
    fNumPlanes = 4;

    // Loop through all possibilities:
    // in binary this will run over 1111, 1110, 1101,..., 0010, 0001, 0000,
    // where each bit position is a plane (here with four planes).  The last
    // entry (0000) is of course skipped.
    int plane_combination = (1 << fNumPlanes);
    while (plane_combination--) {
      int offs = 1;
      int maxs = 0;
      int flip = 0;
      for (unsigned int plane = 0; plane < fNumPlanes; plane++) {
        // If this plane is active in this combination
	if (plane_combination & (1 << plane)) {
	  son.bit[plane] = (father->bit[plane] << 1) + 1;
	} else {
	  son.bit[plane] = (father->bit[plane] << 1);
	}
	// Keep track of minimum and maximum bin at any plane
	offs = (int) std::min (offs, son.bit[plane]);
	maxs = (int) std::max (maxs, son.bit[plane]);
      }

      // Width in bins between first and last planes
      son.bits = son.bit[fNumPlanes-1] - son.bit[0];

      /* Check that the hits on the internal tree-detector planes
         are enclosed by the hits on the two outer tree-detector
         planes.  If not, the pattern cannot have a straight line
         put through it, so just stop with it.

         For example, a straight line cannot be put through the
         pattern 1 3 2 because the bin for the middle tree-
         detector (3) is not enclosed by the bins for the two
         outer detectors (1 and 2).                             */

      if (maxs - offs > abs(son.bits))
        continue;

      /* Compute the offset of this hit pattern and, if non-zero,
         shift the pattern over by the offset                   */

      if (offs) { // non-zero offset, but the offset can only at most be one
        for (unsigned int plane = 0; plane < fNumPlanes; plane++)
          son.bit[plane]--; // ... so decrement is sufficient
      }

      /* See if the hit pattern is a flipped pattern and, if so,
         set the "pattern is flipped" flag and flip the pattern */

      if (son.bits < 0) {	/* If hit pattern is flippable, then      */
        flip = 2;		/* (1) set "pattern is flipped" flag, and */
        son.bits = -son.bits;	/* (2) flip the hit pattern               */
        for (unsigned int plane = 0; plane < fNumPlanes; plane++)
          son.bit[plane] = son.bits - son.bit[plane];
      }

      /* Compute the width (in bins) of the hit pattern          */
      son.bits++;
      /* Remember: bins are numbered from 0 to
         n, so we need to add 1 here to compute the width        */

      /* Look at the other sons of this father to see if this
         particular son is already known to the father           */
      treenode* sonptr = nodeexists (father->fSon[offs+flip], &son);

      /* Compute the hash value of this particular son for use
         with the hash search of the full treesearch database    */
      int hash = (son.bit[fNumPlanes-1] + son.bit[1]) % fHashSize;

      /*  initializes the "insert pattern" flag                     */
      int insert_hitpattern = 1; /* right now, set this flag to insert it  */

      /* if the hit pattern for this son was not located when
         the other sons of this father were searched, then look
         through the entire treesearch database for this
         particular hit pattern.  If the hit pattern is still not
         found, then consider inserting it into the treesearch
         database.  Otherwise, adjust the treenode with the like
         hit pattern so that its hit pattern is valid at this
         level of the treesearch division.
                                                                   */
      if (! sonptr && 0 == (sonptr = existent (&son, hash))) {


        /* the pattern is completely unknown.  So, now check if
           it is consistent with a straight line trajectory
           through the tree-detectors whose slope is within the
           window set by the QwOptions parameter fMaxSlope.             */

        if (consistent (&son, level+1, package, type, region, dir)) {
          /* the pattern is consistent, so now insert it into the
             treesearch database by:                              */
	  /*  1st: Create space for this new treenode             */
	  sonptr = new treenode(son);

	  /*  2nd: Since this treenode has no sons at the moment,
              zero the son pointers for this treenode             */
	  sonptr->fSon[0] = sonptr->fSon[1] =
	    sonptr->fSon[2] = sonptr->fSon[3] = 0;

	  /*  3rd: Since the hit pattern in this treenode is only
              known to be valid at this level of bin divsion,
              set the minimum and maximum valid level for this
              treenode to this level                              */

	  sonptr->fMaxLevel = sonptr->fMinLevel = level;

	  /*  4th: Update the hash table so that this
              treenode will be examined during future searches
              of the entire treesearch database.                  */
	  sonptr->SetNext(fHashTable[hash]);  /* Append the pattern onto the */
                                              /* end of the hash table.      */
	  fHashTable[hash] = sonptr;          /* update the hash table.      */

	  fNumPatterns++;

	  /*  5th: Call marklin() recursively to generate the sons of
              this tree node                                      */
	  marklin (sonptr, level+1, package, type, region, dir);

        } else {

	  /*  the pattern is not consistent with a line, so just
              reject it.                                               */
	  insert_hitpattern = 0; /* set "insert pattern" flag to not keep it */
          /*
	  cout << "inconsistent" << endl;
	  for (int k = 0; k < fNumPlanes; k++) {
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

      } else if ((sonptr->fMinLevel > level && consistent(&son, level+1, package, type, region, dir))
	       || sonptr->fMaxLevel < level) {

	/*  1st: Update the levels of the found treenode to
            include this level                                  */
        sonptr->fMinLevel = (int) std::min (level, sonptr->fMinLevel);
	sonptr->fMaxLevel = (int) std::max (level, sonptr->fMaxLevel);

        fNumPatterns++;

	/*  2nd: Update the levels of all the sons for this
            treenode                                      */
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
        ! nodeexists (father->fSon[offs+flip], &son)) { /* final check if hit
                                                          pattern is already
							  in the database    */

        nodenode* nodptr = new nodenode();		/* create a nodenode  */
        nodptr->SetNext(father->fSon[offs+flip]); /* append it to the son list */
	nodptr->SetTree(sonptr);
	father->fSon[offs+flip] = nodptr;
      }

    } // end of loop over bins

  } // end of region 2

  //###########
  // REGION 3 #
  //###########
  else if (region == kRegionID3 && type == kTypeDriftVDC) {

    // There are 8 wires in each demultiplexed VDC group
    fNumWires = 8;

    int offs = 1;
    int maxs = 0;
    int flip = 0;

    int wire_combination = (1 << fNumWires);
    while (wire_combination--) { // loop through all possibilities
      for (unsigned int wire = 0; wire < fNumWires; wire++) { // this loop creates each possible pattern

	if (wire_combination & (1 << wire)) {
	  son.bit[wire] = (father->bit[wire] << 1) + 1;
	} else {
	  son.bit[wire] = (father->bit[wire] << 1);
	}
	offs = (int) std::min (offs,son.bit[wire]);
	maxs = (int) std::max (maxs,son.bit[wire]);
      }


      // Cut patterns in which there are hits that lie outside the road
      // between the first and last hits, i.e. :
      //   X 0      0 X
      //   0 X  or  X 0
      //   0 X      0 X
      //   X 0      0 X
      son.bits = son.bit[fNumWires-1] - son.bit[0];

      int cutback = 0;
      int cutflag = 0;
      for (unsigned int wire = 1; wire < fNumWires; wire++) {
        if (son.bit[wire] < son.bit[wire-1]) { // if the bin decreases
          if (son.bit[wire]) // and it's nonzero cut it
            cutflag++;
          if (! son.bit[wire]) { // but if it's zero, make sure it stays zero
            cutback++;
            if (cutback == 1) son.bits = son.bit[wire-1] - son.bit[0];
          }
        }
        if (son.bit[wire] && cutback)
          cutflag++;
      }
      if (cutflag) {
        /*
        cout << "Cut :" ;
        for (unsigned int wire = 0; wire < fNumWires; wire++)
          cout << son.bit[wire] << " " ;
        cout << endl;*/
        continue;
      }
      if (offs) { /* If there is an offset, so         */
        /*cout << "Offset :" ;
        for (unsigned int wire = 0; wire < fNumWires; wire++)
          cout << son.bit[wire] << " " ;
        cout << endl;
        */
        for (unsigned int wire = 0; wire < fNumWires; wire++) /* shift all hits over this                   offset   */
          son.bit[wire]--;
      }
      if (son.bits < 0) {
        /*cout << "Flip :" ;
        for (unsigned int wire = 0; wire < fNumWires; wire++)
          cout << son.bit[wire] << " " ;
        cout << endl;
    */
        flip = 2;
        son.bits = -son.bits;
        for (unsigned int wire = 0; wire < fNumWires; wire++)
          son.bit[wire] = son.bits - son.bit[wire];
      }
      son.bits++;

      treenode* sonptr = nodeexists(father->fSon[offs+flip], &son);

      int hash = (son.bit[fNumWires-1] + son.bit[1]) % fHashSize;


      /*if(sonptr){for(j=0;j<fNumWires;j++)cout << son.bit[j] << " " ;cout <<  "exists" << endl;}
      else{ for(j=0;j<fNumWires;j++)cout << son.bit[j] << " " ;cout << endl;}
      cout << "hash = " << son.bit[fNumWires-1] << "," << son.bit[1] << "," << hshsiz << "," << hash << endl;
      cout << "level : " << level << endl;
      cout << "bits : " << son.bits << endl;*/
      int insert_hitpattern = 1;
      if (! sonptr&& 0 == (sonptr = existent(&son, hash))) {
        if (consistent (&son, level+1, package, type, region, dir)) {
          //cout << "Adding treenode..." << endl;

          sonptr = new treenode(son);
          sonptr->fSon[0] = sonptr->fSon[1] =
            sonptr->fSon[2] = sonptr->fSon[3] = 0;
          sonptr->fMaxLevel =
            sonptr->fMinLevel = level;

          sonptr->SetNext(fHashTable[hash]);
          fHashTable[hash] = sonptr;

          fNumPatterns++;

          marklin (sonptr, level+1, package, type, region, dir);

        } else { /*

          cout << "inconsistent" << endl;
          for(int k=0;k<fNumWires;k++){
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
      else if ( (sonptr->fMinLevel > level
                 && consistent(&son, level+1, package, type, region, dir) )
              || sonptr->fMaxLevel < level) {
        sonptr->fMinLevel = (int) std::min (level,sonptr->fMinLevel);
        sonptr->fMaxLevel = (int) std::max (level,sonptr->fMaxLevel);

        marklin (sonptr, level+1, package, type, region, dir);
      }

      if (insert_hitpattern  /* "insert pattern" flag is set and    */
        && ! nodeexists (father->fSon[offs+flip], &son)) {

        nodenode* nodptr = new nodenode();		 /* create a nodenode  */
        nodptr->SetNext(father->fSon[offs+flip]); /* append it to the son list */
        nodptr->SetTree(sonptr);
        father->fSon[offs+flip] = nodptr;
      }
    }

  } // end of region 3

  //########
  // OTHER #
  //########
  else {

    QwError << "What are you doing here?!?  Call the software expert now!" << QwLog::endl;

    int layer_combination = (1 << fNumLayers);
    while (layer_combination--) {
      int offs = 1;
      int maxs = 0;
      int flip = 0;

      for (unsigned int layer = 0; layer < fNumLayers; layer++) {
        //cout << "for("<< layer << "," << fNumLayers << "," <<
        //father->bit[layer] << "," << (1<<layer) << "," << layer_combination << ")" << endl;


        if (layer_combination & (1 << layer)) {
          son.bit[layer] = (father->bit[layer] << 1) + 1;
        } else {
          son.bit[layer] = (father->bit[layer] << 1);
        }
        offs = (int) std::min (offs, son.bit[layer]);
        maxs = (int) std::max (maxs, son.bit[layer]);
      }
      son.bits = son.bit[fNumLayers-1] - son.bit[0];
      //cout << "(" << maxs << "," << offs << "," << son.bits << ")" << endl;
      if (maxs - offs > abs(son.bits)) {
        //cout << "yes" << endl;
        continue;
      }

      if (offs)
        for (unsigned int layer = 0; layer < fNumLayers; layer++)
          son.bit[layer]--;

      if (son.bits < 0) {
        flip = 2;
        son.bits = -son.bits;
        for (unsigned int layer = 0; layer < fNumLayers; layer++)
          son.bit[layer] = son.bits - son.bit[layer];
      }
      son.bits++;

      treenode* sonptr = nodeexists(father->fSon[offs+flip], &son);

      int hash = (son.bit[fNumLayers-1] + son.bit[1]) % fHashSize;
      //cout << "hash = " << hash << endl;
      int insert_hitpattern = 1;

      if (! sonptr && 0 == (sonptr = existent(&son, hash))) {
        //cout << "Pattern is unknown" << endl;
        //cout << "-----------" << endl;
        //son.print();
        //cout << "-----------" << endl;
        if (consistent(&son, level+1, package, type, region, dir)) {
          //cout << "Adding treenode..." << endl;
          sonptr = new treenode(son);
          sonptr->fSon[0] = sonptr->fSon[1] =
            sonptr->fSon[2] = sonptr->fSon[3] = 0;
          sonptr->fMaxLevel =
            sonptr->fMinLevel = level;

          sonptr->SetNext(fHashTable[hash]);
          fHashTable[hash] = sonptr;

          marklin (sonptr, level+1, package, type, region, dir);
        }
        else
          insert_hitpattern = 0;
      }
      else if ( (sonptr->fMinLevel > level  && consistent( &son, level+1,package,type,region,dir) )
      || sonptr->fMaxLevel < level) {
        sonptr->fMinLevel = (int) std::min (level,sonptr->fMinLevel);
        sonptr->fMaxLevel = (int) std::max (level,sonptr->fMaxLevel);
        marklin (sonptr, level+1, package, type, region, dir);
      }

      if (insert_hitpattern &&                          /* "insert pattern"
                                                            flag is set and    */
          ! nodeexists (father->fSon[offs+flip], &son)) {

        nodenode* nodptr = new nodenode();		/* create a nodenode  */
        nodptr->SetNext(father->fSon[offs+flip]); /* append it to the son list */
        nodptr->SetTree(sonptr);
        father->fSon[offs+flip] = nodptr;
      }
    }

  } // end of other region

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \fn QwTrackingTree::readtree
    @param filename	The filename of the file where the tree is stored
    @param levels	The number of levels
    @param tlayers	The number of layers
    @param rwidth	The distance between the wires/strips
    @param skipreading	Flag to skip the reading of cached trees

    @return		The search treeregion
 */

QwTrackingTreeRegion* QwTrackingTree::readtree (
	string filename,
	int levels,
	int tlayers,
	double rwidth,
	bool skipreading)
{
  FILE *file                = 0;
  shorttree *stb            = 0;
  QwTrackingTreeRegion *trr = 0;

  Double_t width = 0.0;
  int32_t  num   =   0;

  // Check whether the tree directory exists
  bfs::path scratchdir(std::string(getenv("QWSCRATCH")));
  bfs::path treepath(scratchdir / bfs::path(fgTreeDir));
  bfs::path fullfilename;

  if (! bfs::exists(treepath) || ! bfs::is_directory(treepath)) {
    QwWarning << "Could not find tree directory." << QwLog::endl
              << "tree directory = " << treepath.string() << QwLog::endl;
    treepath = bfs::path(".");
    QwMessage << "Falling back to local directory for tree files..." << QwLog::endl
              << "tree directory = " << treepath.string() << QwLog::endl;
  }
  // Construct full path and file name
  fullfilename = treepath / bfs::path(filename);


  xref = 0;

  // Skip the reading of the trees
  if (skipreading) return 0;

  /// Open the file for reading and complain if this fails
  file = fopen(fullfilename.string().c_str(), "rb");
  if (! file) {
    QwWarning << "Tree file not found.  Rebuilding..." << QwLog::endl
              << "tree file = " << fullfilename.string() << QwLog::endl;
    return 0;
  }

  /// If num and width cannot be read, then the file is invalid
  if (fread(&num,   sizeof(num),   1L, file) < 1 ||
      fread(&width, sizeof(width), 1L, file) < 1 ) {
    QwWarning << "Tree file appears invalid.  Rebuilding..." << QwLog::endl
              << "tree file = " << fullfilename.string() << QwLog::endl;
    fclose(file);
    return 0;
  }

  /// Allocate a shorttree array
  stb = new shorttree[num];

  /// Allocate a QwTrackingTreeRegion object
  trr = new QwTrackingTreeRegion();
  maxref = num;
  /// ... and fill by recursively calling _readtree
  if (_readtree (file, stb, 0, tlayers)) {
    // Errors occurred while reading the tree
    delete [] stb; stb = 0;
    delete trr; trr = 0;
    QwWarning << "Tree file appears invalid.  Rebuilding..." << QwLog::endl
              << "tree file = " << fullfilename.string() << QwLog::endl;
    fclose(file);
    return 0;
  }

  /// Close the file after reading in the tree
  if (file) fclose(file);

  trr->SetSearchable(stb ? true : false);
  QwDebug << "Set searchable = " << trr->IsSearchable() << QwLog::endl;

  shortnode* node = trr->GetNode();
  node->SetTree(stb, num);
  node->SetNext(0);
  trr->SetWidth(width);

  return trr;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! This function checks whether a pattern database requested by inittree
    already exists.  If not, it will call _inittree to create the database,
    write it out, then read it in again.  To force the generation of new databases,
    simply remove the old ones from the 'trees' directory.  If options are set
    to create a different level of pattern resolution, this function will
    automatically create the new databases.
 */

QwTrackingTreeRegion* QwTrackingTree::inittree (
	string filename,
	int levels,
	int tlayer,
	double width,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir)
{
// TODO: This routine assumes that the directory 'trees' exists and doesn't create it itself. (wdconinc)
  QwTrackingTreeRegion *trr = 0;
  fNumLayers  = tlayer;
  fMaxLevel = levels + 1;

  if (tlayer == 0) return 0;

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
  QwMessage << "Attempting to read tree from " << filename << QwLog::endl;
  bool regenerate = true; // flag to force regeneration every time
  trr = readtree(filename, levels, tlayer, width, regenerate);
  if (trr == 0) {

    /// Generate a new tree database
    treenode* back = _inittree (tlayer, package, type, region, dir);
    if (! back) {
      QwError << "Search tree could not be built." << QwLog::endl;
      exit(1);
    }
    QwMessage << " Generated " << fNumPatterns << " patterns" << QwLog::endl;

    /// Write the generated tree to disk for faster access later
    if (! writetree(filename, back, levels, tlayer, width)) {
      QwError << "Search tree could not be written." << QwLog::endl;
      exit(1);
    }
    QwMessage << " Cached in " << filename << QwLog::endl;

    // TODO Here we need to delete something, I think (wdc)

    /// Read it in again to get the shorter tree search format
    trr = readtree(filename, levels, tlayer, width, 0);
    if (trr == 0) {
      QwError << "New tree could not be read." << QwLog::endl;
      exit(1);
    }
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
  /// Generate a copy of the father node to start off this tree search database
  treenode *node = new treenode(fFather);

  /// Clear the hash table
  fNumPatterns = 0;
  // TODO Replace with delete/new if constructor is not sufficient
  memset (fHashTable, 0, sizeof(fHashTable));

  /// Call the recursive tree generator
  marklin (node, 0, package, type, region, dir);

  /// Finally, add the father node to the hash table
  node->SetNext(fHashTable[0]);
  fHashTable[0] = node;

  // Print the number of patterns
  QwDebug << "Generated fNumPatterns = " << fNumPatterns << " patterns" << QwLog::endl;

  // Return the node
  return node;
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
        fwrite(&tn->fMinLevel, sizeof(int), 1L, fp) != 1 ||
        fwrite(&tn->bits,     sizeof(int), 1L, fp) != 1 ||
        fwrite(tn->bit,       sizeof(int), (size_t) tlayers, fp) != (unsigned int)tlayers )
      return -1;

    for (int i = 0; i < 4; i++) {
      nd = tn->fSon[i];
      while (nd) {///write the sons of this treenode (and their sons, etc)
        if (_writetree(nd->GetTree(), fp, tlayers))
          return -1;
        nd = nd->next();
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
	string filename,
	treenode *tn,
	int levels,
	int tlayers,
	double width)
{
	// Ensure that the tree directory is created correctly
	bfs::path scratchdir(std::string(getenv("QWSCRATCH")));
	if (! bfs::exists(scratchdir)) {
		QwError << "QWSCRATCH directory does not exist!" << QwLog::endl
		        << "QWSCRATCH = " << scratchdir.string() << QwLog::endl;
		exit(1);
	}
	bfs::path treepath(scratchdir / bfs::path(fgTreeDir));
	if (! bfs::exists(treepath)) {
		bfs::create_directory(treepath);
		QwDebug << "Created tree directory." << QwLog::endl;
		QwDebug << "tree directory = " << treepath.string() << QwLog::endl;
	}
	if (! bfs::exists(treepath) || ! bfs::is_directory(treepath)) {
		QwWarning << "Could not create tree directory!" << QwLog::endl
		          << "tree directory = " << treepath.string() << QwLog::endl;
		treepath = bfs::path(".");
		QwWarning << "Falling back to local directory for tree files..." << QwLog::endl
		          << "tree directory = " << treepath.string() << QwLog::endl;
	}
	// Construct full path and file name
	bfs::path fullfilename = treepath / bfs::path(filename);

	// Open the output stream
	FILE *file = fopen(fullfilename.string().c_str(), "wb");
	xref = 0;
	if (!file) {
		QwWarning << "Could not open tree file.  Rebuilding..." << QwLog::endl
		          << "tree file = " << filename << QwLog::endl;
		return 0;
	}

	/// Write 4 bytes to fill later with the number of different patterns
	if (fwrite(&xref,  sizeof(int32_t), 1L, file) != 1 ||
	    fwrite(&width, sizeof(double),  1L, file) != 1 ||
	   _writetree(tn, file, tlayers)) { ///... and write whole tree
		fclose(file);
		return 0;
	}
	if (fputc(SONEND, file) == EOF) /// append marker for end of son list
		return -1;

	rewind(file);

	/// Now write the total numer of different patterns,
	if (fwrite(&xref, sizeof(int32_t), 1L, file) != 1) {
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
    @param	father	A list of father nodes
    @param	tlayers	The number of layers(?)

    @return	Returns negative on error, zero otherwise
 */

int QwTrackingTree::_readtree(
	FILE *file,
	shorttree *stb,
	shortnode **father,
	int tlayers)
{
  /// Go into an infinite loop while reading the file
  for (;;) {

    /// Read the next record type
    int32_t type = fgetc(file);


    /// \li If we encounter a real pattern, then ...
    if (type == REALSON) {

      if (xref >= maxref) {
        QwWarning << "QTR: readtree failed. Rebuilding treefiles." << QwLog::endl
                  << "More nodes in file than announced in header!" << QwLog::endl;
        return -1;
      }

      /// read in the node(?) information
      int32_t minlevel = 0;
      int32_t bits = 0;
      int32_t bit[tlayers];
      for (int i = 0; i < tlayers; i++) bit[i] = 0; // initialize to zero
      if (fread(&minlevel, sizeof(int32_t),           1L, file) != 1 ||
          fread(&bits,     sizeof(int32_t),           1L, file) != 1 ||
          fread(&bit,      sizeof(int32_t) * tlayers, 1L, file) != 1) {
        QwWarning << "QTR: readtree failed. Rebuilding treefiles." << QwLog::endl
                  << "Header could not be read" << QwLog::endl;
        return -1;
      }

      int32_t ref = xref;
      xref++;
      (stb+ref)->fMinLevel = minlevel;
      (stb+ref)->bits = bits;
      for (int i = 0; i < tlayers; i++)
        (stb+ref)->bit[i] = bit[i];

      if (father) { /// ... and append the patterns to the father's sons
        shortnode* node = new shortnode();
        node->SetTree(stb + ref);
        node->SetNext(*father);
        (*father) = node;
      }

      /// Read in the sons of this node
      for (int32_t son = 0; son < 4; son++) {
        if (_readtree(file, stb, stb[ref].son + son, tlayers)) {
          QwWarning << "An error... don't know what happened..." << QwLog::endl;
          return -1;
        }
      }


    /// \li If we encounter a reference, then read in the reference
    } else if (type == REFSON) {

      int32_t ref;
      if (fread (&ref, sizeof(ref), 1L, file) != 1) {
        QwWarning << "QTR: readtree failed. Rebuilding treefiles." << QwLog::endl
                  << "Could not read reference field." << QwLog::endl;
        return -1;
      }
      if (ref >= xref || ref < 0) { /// some error checking
        QwWarning << "QTR: readtree failed. Rebuilding treefiles." << QwLog::endl
                  << "Reference field contains nonsense." << QwLog::endl;
        return -1;
      }
      if (!father) { /// still some error checking
        QwWarning << "QTR: readtree failed. Rebuilding treefiles." << QwLog::endl
                  << "Father node invalid." << QwLog::endl;
        return -1;
      }
      shortnode* node = new shortnode(); /// create a new node
      node->SetTree(stb + ref);	/// and append the alread read-in tree
      node->SetNext(*father);	/// 'ref' to this node.
      (*father) = node;


    /// \li If we encounter an end token, then stop
    } else if (type == SONEND) {
      break;


    /// \li If we encounter something else, then fail
    } else {
      QwError << "Reading cached tree failed, rebuilding tree files." << QwLog::endl;
      return -1;
    }
  }

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
