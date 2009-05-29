//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef TREE_H
#define TREE_H

// Standard C and C++ headers
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>

// Boost filesystem headers
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
namespace bfs = boost::filesystem;

// Qweak tree object headers
#include "treenode.h"
#include "nodenode.h"
#include "shortnode.h"
#include "shorttree.h"
#include "treeregion.h"

#include "QwTypes.h"
#include "options.h"
#include "Det.h"
#include "tracking.h"
#include "globals.h"

using namespace std;
using namespace QwTracking;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/*------------------------------------------------------------------------*//*!

 \class tree

 \brief Creates and manages the treesearch pattern database.

    The pattern database is used to determine whether subsets
    of hits resemble track segments.

 \ingroup QwTrackingAnl

*//*-------------------------------------------------------------------------*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class tree {

  public:

    // objects
    int maxlevel;	/*!< maximum level of the bin division within
			     the treesearch database,
			     i.e.: resolution = (detwidth) / (2^maxlevel) */
    int tlayers;	//!< number of detector planes
    int hshsiz;
    int npat;
    treenode* generic[HSHSIZ];	//!- flat list of all nodes in the tree
    double zList[TLAYERS];
    double detwidth;
    double rcSET_rMaxSlope;//this needs to be defined by rcSET.rMaxSlope
    treenode father;
    int xref;
    int maxref;

    static const string TREEDIR;

    // Constructor (TODO should take some option info)
    tree();
    ~tree();

    void SetDebugLevel(int debuglevel) { debug = debuglevel; };

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

    treeregion* readtree (
	char *filename,
	int levels,
	int tlayers,
	double rwidth,
	int dontread);

    treeregion* inittree (
	char *filename,
	int levels,
	int tlayer,
	double width,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir);

  private:

    int debug; // debug level

    int _writetree (treenode *tn, FILE *fp, int tlayers);
    int _readtree  (FILE *f, shorttree *stb, shortnode **father, int tlayers);

    treenode* _inittree (
	int tlayer,
	EQwDetectorPackage package,
	EQwDetectorType type,
	EQwRegionID region,
	EQwDirectionID dir);

};

#endif // TREE_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....