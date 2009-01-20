#ifndef QWTRACKINGTREENODE_H
#define QWTRACKINGTREENODE_H

#include "nodenode.h"
#include "globals.h"

/*------------------------------------------------------------------------*//*!

 \namespace QwTracking

 \brief Contains tracking-related objects

    The namespace QwTracking contains the classes related to the pattern
    recognition algorithm and the tree search.  This includes the objects
    treenode, nodenode, shorttree, shortnode, and treeregion.

    \todo In the future, it would make sense to migrate all tracking-related
    objects to the namespace QwTracking, to avoid confusion with other objects
    with similar names in the analysis code (track, event, etc).

*//*-------------------------------------------------------------------------*/

namespace QwTracking {


// Forward declaration due to cyclic dependency
class nodenode;

/*------------------------------------------------------------------------*//*!

 \class treenode

 \brief A treenode contains the bits which make up a tree pattern.

    The treenode also has a pointer to its father and a pointer to its
    son nodenodes.  Each following generation of a treenode will have
    a higher bit resolution.

*//*------------------------------------------------------------------------*/

class treenode {

  public:

    treenode();
    ~treenode();

    void print();

    int maxlevel, minlevel;
    int bits;
    int bit[TLAYERS];
    int xref;
    int pattern_offset;

    nodenode *son[4];
    treenode *genlink;

};


} // QwTracking


#endif // QWTRACKINGTREENODE_H
