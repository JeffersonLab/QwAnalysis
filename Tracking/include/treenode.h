//
// C++ Interface: treenode
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QWTRACKINGTREENODE_H
#define QWTRACKINGTREENODE_H

#include <nodenode.h>

// TODO TLAYERS should be moved to ONE location,
// and preferably even gotten rid of...
#define TLAYERS 8

namespace QwTracking {

class nodenode;

/**
    @author Wouter Deconinck <wdconinc@mit.edu>

    \class treenode
    \brief A treenode contains the bits which make up a tree pattern.

    The treenode also has a pointer to its father and a pointer to its
    son nodenodes.  Each following generation of a treenode will have
    a higher bit resolution.
 */

class treenode {
  public:
    treenode();
    ~treenode();

    void print();

    treenode *genlink;
    nodenode *son[4];

    int maxlevel,minlevel;
    int bits;
    int bit[TLAYERS];
    int xref;
    int pattern_offset;
};

}

#endif
