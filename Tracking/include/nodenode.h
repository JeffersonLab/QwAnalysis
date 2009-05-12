#ifndef QWTRACKINGNODENODE_H
#define QWTRACKINGNODENODE_H

#include "treenode.h"

namespace QwTracking {


// Forward declaration due to cyclic dependency
class treenode;

/*------------------------------------------------------------------------*//*!

 \class nodenode

 \brief A nodenode is used as a pointer which links treenodes to their siblings.

    Together with the treenode, any tree pattern can be
    related to any of its family members.  This allows the
    tree search algorithms to quickly move through the database
    to identify matching patterns.

*//*-------------------------------------------------------------------------*/

///
/// \ingroup QwTrackingAnl
class nodenode {

  public:

    nodenode();
    ~nodenode();


    nodenode *next;
    treenode *tree;

};


} // QwTracking


#endif // QWTRACKINGNODENODE_H
