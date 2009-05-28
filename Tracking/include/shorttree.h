#ifndef QWTRACKINGSHORTTREE_H
#define QWTRACKINGSHORTTREE_H

#include "shortnode.h"
#include "globals.h"

namespace QwTracking {


// Forward declaration due to cyclic dependency
class shortnode;

/*------------------------------------------------------------------------*//*!

    \class shorttree

    \brief Similar to a treenode.

*//*-------------------------------------------------------------------------*/

///
/// \ingroup QwTrackingAnl
class shorttree {

  public:

    shorttree();
    ~shorttree();

    void print();

    int minlevel;
    int bits;
    int bit[TLAYERS];
    int xref;

    shortnode *son[4];

};


} // QwTracking


#endif // QWTRACKINGSHORTTREE_H
