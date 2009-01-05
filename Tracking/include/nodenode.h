//
// C++ Interface: nodenode
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QWTRACKINGNODENODE_H
#define QWTRACKINGNODENODE_H

#include <treenode.h>

namespace QwTracking {

class treenode;

/**
    @author Wouter Deconinck <wdconinc@mit.edu>

    \class nodenode
    \brief A nodenode is used as a pointer which links treenodes to their siblings.

    Together with the treenode, any tree pattern can be
    related to any of its family members.  This allows the
    tree search algorithms to quickly move through the database
    to identify matching patterns.
 */
class nodenode {
  public:
    nodenode();
    ~nodenode();

    nodenode *next;
    treenode *tree;
};

} // QwTracking

#endif
