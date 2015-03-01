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
#include "QwGeometry.h"
#include "VQwTrackingElement.h"

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

    /// \brief Default constructor
    QwTrackingTree(unsigned int numlayers);
    /// \brief Destructor
    virtual ~QwTrackingTree();

    /// Set the debug level
    void SetDebugLevel (const int debuglevel) { fDebug = debuglevel; };
    /// Set the maximum allowed slope
    void SetMaxSlope (const double maxslope) { fMaxSlope = maxslope; };

    /// Print the full tree and hash table
    void Print(const Option_t* options = 0) const { PrintTree(); PrintHashTable(); };
    /// \brief Print the full tree
    void PrintTree() const;
    /// \brief Print the hash table
    void PrintHashTable() const;

    void SetGeometry(const QwGeometry& geometry) { fGeometry = geometry; };

    int consistent (
	treenode *testnode,
	int level,
	QwDetectorInfo* detector);
    treenode* existent (treenode *node, int hash);
    treenode* nodeexists (nodenode *nd, treenode *tr);

    /// \brief Recursively generate the treesearch pattern database
    void marklin (
	treenode *father,
	int level,
	QwDetectorInfo* detector);

    long writetree (
	const string& filename,
	treenode *tn,
	int levels,
	int tlayers,
	double width);

    QwTrackingTreeRegion* readtree (
	const string& filename,
	int levels,
	int tlayers,
	double rwidth,
	bool skipreading);

    QwTrackingTreeRegion* inittree (
	const string& filename,
	int levels,
	int tlayer,
	double width,
	QwDetectorInfo* detector,
	bool regenerate);

  private:

    /// Name of the tree directory (in $QWSCRATCH), as static member field
    static const std::string fgTreeDir;

    /// Detector (or set of detectors) represented by this search tree
    QwGeometry fGeometry;

    int fDebug;		///< Debug level

    // Number of rows in the bit pattern (VDC wires or HDC planes)
    unsigned int fNumLayers;	///< Number of detector planes
    unsigned int& fNumPlanes;	///< Number of planes in the region 2 HDCs
    unsigned int& fNumWires;	///< Number of wires in a region 3 VDC group

    unsigned int fHashSize;	///< Length of the hash table (as member field)

    double fMaxSlope;	///< Maximum allowed slope for tracks in this detector
    int fNumPatterns;	///< Number of valid patterns in the tree

    /// Reference
    int fRef;
    /// Maximum number of references in the cached tree file
    int fMaxRef;

    int fMaxLevel;	/*!< maximum level of the bin division within
			     the treesearch database,
			     i.e.: resolution = width / (2^maxlevel) */

    /// Father node: the main entry point to the tree
    treenode* fFather;

    /// Hash table: the list of all nodes in the tree, organized in linked
    /// lists and sorted by hash value; an array of pointers to treenodes.
    //treenode* fHashTable[HSHSIZ];
    treenode** fHashTable;

  private:

    /// \brief Recursive method for pulling in the concise treesearch search database
    int _writetree (treenode *tn, FILE *fp, int32_t tlayers);
    /// \brief Recursive method to read the concise treesearch database from disk
    int _readtree  (FILE *file, shorttree *stb, shortnode **father, int32_t tlayers);
    /// \brief Recursive method to initialize and generate the treesearch database
    treenode* _inittree (
	int32_t tlayer,
	QwDetectorInfo* detector);

}; // class QwTrackingTree

#endif // QWTRACKINGTREE_H
