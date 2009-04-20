#ifndef TREE_H
#define TREE_H

// Qweak tree object headers
#include "treenode.h"
#include "nodenode.h"
#include "shortnode.h"
#include "shorttree.h"
#include "treeregion.h"
using namespace QwTracking;

#include "QwTypes.h"
#include "enum.h"
#include "options.h"
#include "Hit.h"
#include "Det.h"
#include "tracking.h"

#include "globals.h"


/*------------------------------------------------------------------------*//*!

 \class tree

 \brief Creates and manages the treesearch pattern database.

    The pattern database is used to determine whether subsets
    of hits resemble track segments.

*//*-------------------------------------------------------------------------*/

class tree {

  public:

    // objects
    int maxlevel;		/*!< maximum level of the bin division w/in
				     the treesearch database, i.e.:
				     resolution = (detwidth) / (2^maxlevel) */
    int tlayers;
    int hshsiz;
    int npat;
    treenode *generic[HSHSIZ];
    double zList[TLAYERS];
    double detwidth;
    double rcSET_rMaxSlope;//this needs to be defined by rcSET.rMaxSlope
    treenode father;
    int xref;
    int maxref;
    int debug;


    // functions
    tree();
    ~tree();

    void rcInitTree();
    int consistent(treenode *tst, int level,enum EPackage package, enum Etype type, EQwRegionID region, EQwDirectionID dir);
    treenode * existent(treenode *tst, int hash);
    treenode *nodeexists(nodenode *nd, treenode *tr);
    treenode * treedup(treenode *todup);
    void marklin(treenode *Father,int level,enum EPackage package, enum Etype type, EQwRegionID region,EQwDirectionID dir);

    void treeout(treenode *tn, int level, int off);
    void printtree(treenode *tn);
    void freetree();
    treeregion * readtree(char *filename, int levels, int tlayers, double rwidth, int dontread);
    long writetree(char *fn, treenode *tn, int levels, int tlayers, double width);
    treeregion * inittree(char *filename, int levels, int tlayer, double width,enum EPackage package, enum Etype type, enum EQwRegionID region,EQwDirectionID dir);
    treenode * _inittree(int tlayer,enum EPackage package, enum Etype type,enum EQwRegionID region, EQwDirectionID dir);//should be moved to private once inittree() is written

  private:

    int _writetree(treenode *tn, FILE *fp, int tlayers);
    int _readtree(FILE *f, shorttree *stb, shortnode **fath, int tlayers);

};


#endif // TREE_H
