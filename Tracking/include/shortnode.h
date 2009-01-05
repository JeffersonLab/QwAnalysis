//
// C++ Interface: shortnode
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QWTRACKINGSHORTNODE_H
#define QWTRACKINGSHORTNODE_H

#include <shorttree.h>

namespace QwTracking {

class shorttree;

/**
    @author Wouter Deconinck <wdconinc@mit.edu>

    \class shortnode
    \brief Similar to a nodenode.
*/
class shortnode {
  public:
    shortnode();
    ~shortnode();

    void print();

    shortnode *next;
    shorttree *tree;
};

} // QwTracking

#endif
