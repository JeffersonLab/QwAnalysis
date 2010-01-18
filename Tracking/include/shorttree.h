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

// Qweak headers
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

    /// \brief Default constructor
    shorttree();
    /// \brief Destructor
    ~shorttree();

    int fMinLevel;
    int bits;
    int bit[TLAYERS];
    int xref;

    shortnode* son[4];

    /// \brief Print some debugging information
    void Print(int indent = 0);
    /// \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const shorttree& st);

    /// Get number of objects
    static const int GetCount() { return fCount; };

  private:

    static int fCount; /// Object counter

    static int fDebug; /// Debug level

}; // class shorttree

} // namespace QwTracking

#endif // QWTRACKINGSHORTTREE_H
