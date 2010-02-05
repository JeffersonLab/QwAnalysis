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
using std::cout;
using std::cerr;
using std::endl;
using std::string;

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

// Definitions
#define HSHSIZ 511	///< Length of the hash table (as header define)

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

    /// \brief Default constructor (TODO should take some option info)
    QwTrackingTree();
    /// \brief Destructor
    ~QwTrackingTree();

    /// Set the debug level
    void SetDebugLevel (const int debuglevel) { fDebug = debuglevel; };
    /// Set the maximum allowed slope
    void SetMaxSlope (const double maxslope) { fMaxSlope = maxslope; };

    /// Print the full tree and hash table
    void Print() const { PrintTree(); PrintHashTable(); };
    /// \brief Print the full tree
    void PrintTree() const;
    /// \brief Print the hash table
    void PrintHashTable() const;

    int consistent (
	treenode *tst,
	int level,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir);
    treenode* existent (treenode *tst, int hash);
    treenode* nodeexists (nodenode *nd, treenode *tr);
    void marklin (
	treenode *node,
	int level,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir);

    long writetree (
	string filename,
	treenode *tn,
	int levels,
	int tlayers,
	double width);

    QwTrackingTreeRegion* readtree (
	string filename,
	int levels,
	int tlayers,
	double rwidth,
	bool regenerate);

    QwTrackingTreeRegion* inittree (
	string filename,
	int levels,
	int tlayer,
	double width,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir);

  private:

    /// Name of the tree directory (in $QWSCRATCH), as static member field
    static const std::string fgTreeDir;

    int fDebug;		///< Debug level

    int fHashSize;	///< Length of the hash table (as member field)

    double fMaxSlope;	///< Maximum allowed slope for tracks in this detector
    int fNPatterns;	///< Number of valid patterns in the tree
    int fNLayers;	///< Number of detector planes

    int xref;
    int maxref;

    int fMaxLevel;	/*!< maximum level of the bin division within
			     the treesearch database,
			     i.e.: resolution = width / (2^maxlevel) */

    /// Father node: the main entry point to the tree
    treenode* fFather;

    /// Hash table: the list of all nodes in the tree, organized in linked
    /// lists and sorted by hash value
    treenode* fHashTable[HSHSIZ];

    /// \brief Recursive method for pulling in the concise treesearch search database
    int _writetree (treenode *tn, FILE *fp, int32_t tlayers);
    /// \brief Recursive method to read the concise treesearch database from disk
    int _readtree  (FILE *f, shorttree *stb, shortnode **father, int32_t tlayers);
    /// \brief Recursive method to initialize and generate the treesearch database
    treenode* _inittree (
	int32_t tlayer,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir);

}; // class QwTrackingTree

#endif // QWTRACKINGTREE_H
