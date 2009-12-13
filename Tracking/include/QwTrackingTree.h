/*!
 * \file   QwTrackingTree.h
 * \brief  Definition of the track search tree
 *
 * \author Wouter Deconinck
 * \date   2009-12-11
 */

#ifndef QWTRACKINGTREE_H
#define QWTRACKINGTREE_H

// Standard C and C++ headers
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>
#include <inttypes.h>
using std::cout; using std::cerr; using std::endl;

// Boost filesystem headers
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
namespace bfs = boost::filesystem;

// Qweak tree object headers
#include "treenode.h"
#include "nodenode.h"
#include "shortnode.h"
#include "shorttree.h"
using QwTracking::treenode; using QwTracking::nodenode;
using QwTracking::shortnode; using QwTracking::shorttree;

// Qweak headers
#include "QwTypes.h"
#include "VQwTrackingElement.h"
#include "globals.h"

// Forward declarations
class QwTrackingTreeRegion;

/**
 * \class QwTrackingTree
 * \ingroup QwTracking
 * \brief Creates and manages the treesearch pattern database.
 *
 * The pattern database is used to determine whether subsets
 * of hits resemble track segments.
 */
class QwTrackingTree: public VQwTrackingElement {

  public:

    // objects
    int maxlevel;	/*!< maximum level of the bin division within
			     the treesearch database,
			     i.e.: resolution = (detwidth) / (2^maxlevel) */
    int tlayers;	///< number of detector planes
    int hshsiz;
    int npat;
    treenode* generic[HSHSIZ];	///< flat list of all nodes in the tree
    double zList[TLAYERS];
    double detwidth;
    treenode father;
    int xref;
    int maxref;

    static const std::string TREEDIR;

    // Constructor/Destructor (TODO should take some option info)
    QwTrackingTree();
    ~QwTrackingTree();

    /// Set the debug level
    void SetDebugLevel (const int debuglevel) { fDebug = debuglevel; };
    /// Set the maximum allowed slope
    void SetMaxSlope (const double maxslope) { fMaxSlope = maxslope; };

    int consistent (
	treenode *tst,
	int level,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir);
    treenode* existent (treenode *tst, int hash);
    treenode* nodeexists (nodenode *nd, treenode *tr);
    treenode* treedup (treenode *todup);
    void marklin (
	treenode *Father,
	int level,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir);

    void treeout (treenode *tn, int level, int off);
    void printtree (treenode *tn);
    void freetree ();

    long writetree (
	char *fn,
	treenode *tn,
	int levels,
	int tlayers,
	double width);

    QwTrackingTreeRegion* readtree (
	char *filename,
	int levels,
	int tlayers,
	double rwidth,
	int dontread);

    QwTrackingTreeRegion* inittree (
	char *filename,
	int levels,
	int tlayer,
	double width,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir);

  private:

    int fDebug;		///< Debug level

    double fMaxSlope;	///< Maximum allowed slope for tracks in this detector

    /// Recursive method for pulling in the concise treesearch search database
    int _writetree (treenode *tn, FILE *fp, int32_t tlayers);
    /// Recursive method to read the concise treesearch database from disk
    int _readtree  (FILE *f, shorttree *stb, shortnode **father, int32_t tlayers);
    /// Recursive method to initialize and generate the treesearch database
    treenode* _inittree (
	int32_t tlayer,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir);

}; // class QwTrackingTree

#endif // QWTRACKINGTREE_H
