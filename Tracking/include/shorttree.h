//
// C++ Interface: shorttree
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef QWTRACKINGSHORTTREE_H
#define QWTRACKINGSHORTTREE_H

#include <shortnode.h>

#define TLAYERS 8

namespace QwTracking {

class shortnode;

/**
    @author Wouter Deconinck <wdconinc@mit.edu>

    \class shorttree
    \brief Similar to a treenode.
 */
class shorttree{
  public:
    shorttree();
    ~shorttree();

    void print();

    int maxlevel;
    int minlevel;
    int bits;
    int xref;
    int bit[TLAYERS];
    shortnode *son[4];
};

} // QwTracking

#endif
