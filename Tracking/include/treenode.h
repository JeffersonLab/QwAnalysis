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

// System headers
#include <iostream>

// Qweak headers
#include "QwLog.h"
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

    /// \brief Default constructor
    treenode();
    /// \brief Copy-constructor from object
    treenode(treenode& node);
    /// \brief Copy-constructor from pointer
    treenode(treenode* node);
    /// \brief Destructor
    ~treenode();

    int fMaxLevel, fMinLevel;
    int bits;
    int bit[TLAYERS];
    int xref;
    int pattern_offset;

    unsigned int fSize;

    /// Set the next node
    void SetNext(treenode* next) {
      if (next == this) {
        QwError << "Trying to link next to self" << QwLog::endl; return;
      }
      fNext = next;
    };
    /// Get the next node
    treenode* GetNext() const { return fNext; };
    /// Get the next node (non-standard notation)
    treenode* next() const { return fNext; };

    /// Get size of the bit array
    const unsigned int size() const { return fSize; };

    /// \brief Print some debugging information
    void Print(int indent = 0);
    /// \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const treenode& tn);

    /// Get number of objects
    static const int GetCount() { return fCount; };

  public:

    ///< Each tree has four son nodes
    nodenode* fSon[4];

  private:

    ///< Link to the next tree node
    treenode* fNext;

    static int fCount; /// Object counter
    static int fDebug; /// Debug level

}; // class treenode

} // namespace QwTracking

#endif // QWTRACKINGTREENODE_H
