#ifndef QWTRACKINGSHORTNODE_H
#define QWTRACKINGSHORTNODE_H

#include "shorttree.h"

namespace QwTracking {


// Forward declaration due to cyclic dependency
class shorttree;

/*------------------------------------------------------------------------*//*!

 \class shortnode

 \brief Similar to a nodenode.

*//*-------------------------------------------------------------------------*/

///
/// \ingroup QwTrackingAnl
class shortnode {

  public:

    shortnode();
    ~shortnode();

    void print();

    shortnode *next;
    shorttree *tree;

  private:

    int debug;

};


} // QwTracking


#endif // QWTRACKINGSHORTNODE_H
