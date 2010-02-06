/*------------------------------------------------------------------------*//*!

 \file shortnode.h
 \ingroup QwTracking

 \author	Wolfgang Wander <wwc@hermes.desy.de>
 \author	Burnham Stokes <bestokes@jlab.org>
 \author	Wouter Deconinck <wdconinc@mit.edu>

 \date		2009-09-04 18:06:23

 \brief Definition of a shorttree, the short version of a treenode

*//*-------------------------------------------------------------------------*/

#ifndef QWTRACKINGSHORTTREE_H
#define QWTRACKINGSHORTTREE_H

#include "globals.h"
#include "shortnode.h"

namespace QwTracking {

// Forward declaration due to cyclic dependency
class shortnode;

/**
 * \class	shorttree
 *
 * \ingroup	QwTracking
 *
 * \brief Similar to a treenode.
 *
 */
class shorttree {

  public:

    shorttree();
    ~shorttree();

    int minlevel;
    int bits;
    int bit[TLAYERS];
    int xref;

    shortnode *son[4];

    /// \brief Print some debugging information
    void Print();
    /// \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const shorttree& st);

}; // class shorttree

} // namespace QwTracking

#endif // QWTRACKINGSHORTTREE_H
