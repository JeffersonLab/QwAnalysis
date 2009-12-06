/*------------------------------------------------------------------------*//*!

 \file treenode.h
 \ingroup QwTracking

 \author	Wolfgang Wander <wwc@hermes.desy.de>
 \author	Burnham Stokes <bestokes@jlab.org>
 \author	Wouter Deconinck <wdconinc@mit.edu>

 \date		2009-09-04 18:06:23

 \brief Definition of a treenode which contains the bits that make up a tree pattern

*//*-------------------------------------------------------------------------*/

#ifndef QWTRACKINGTREENODE_H
#define QWTRACKINGTREENODE_H

#include <iostream>

#include "globals.h"

/**
 * \namespace QwTracking
 *
 * \brief Contains tracking-related objects
 *
 * The namespace QwTracking contains the classes related to the pattern
 * recognition algorithm and the tree search.  This includes the objects
 * treenode, nodenode, shorttree, shortnode, and treeregion.
 *
 */
namespace QwTracking {


// Forward declaration due to cyclic dependency
class nodenode;

/**
 * \class	treenode
 *
 * \ingroup	QwTracking
 *
 * \brief A treenode contains the bits that make up a tree pattern.
 *
 * The treenode also has a pointer to its father and a pointer to its
 * son nodenodes.  Each following generation of a treenode will have
 * a higher bit resolution.
 *
 */
class treenode {

  public:

    treenode();
    ~treenode();

    int maxlevel, minlevel;
    int bits;
    int bit[TLAYERS];
    int xref;
    int pattern_offset;

    nodenode *son[4];		///< Each tree has 4 son nodes
    treenode *genlink;		///< Link back to the generating tree

    /// \brief Print some debugging information
    void Print();
    /// \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const treenode& tn);

}; // class treenode

} // namespace QwTracking

#endif // QWTRACKINGTREENODE_H
