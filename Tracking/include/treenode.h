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
#include "QwObjectCounter.h"

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
 * This is the basic element of the tree search database when it is
 * constructed.  Each valid track has a corresponding tree node.  The
 * minimum and maximum levels of bin division are stored in the treenode.
 *
 * The array fBit contains the rank of the bit that is active for this track.
 * For example, a track that goes through (simplified) element 2 in layer 1,
 * element 4 in layer 2, element 5 in layer 3, and element 7 in layer 4 will
 * have the following fBit array: [2,4,6,7].  This example track will have a
 * width of 7 - 2 + 1 = 6 (such that the track [2,2,2,2] has a width of 1).
 *
 * The treenode also has a pointer to its father and a pointer to its
 * son nodenodes.  Each following generation of a treenode will have
 * a higher bit resolution.
 */
class treenode: public QwObjectCounter<treenode> {

  public:

    /// \brief Default constructor
    treenode(unsigned int size);
    /// \brief Copy-constructor from object
    treenode(treenode& node);
    /// \brief Copy-constructor from pointer
    treenode(treenode* node);
    /// \brief Destructor
    ~treenode();

  public:

    /// Minimum level at which this node is valid
    int fMinLevel;
    /// Maximum level at which this node is valid
    int fMaxLevel;

    /// Hit pattern, one bin specified per detector layer
    int* fBit;

    /// Width in bins of the hit pattern
    int fWidth;

    /// Reference of this node when writing to file
    int fRef;

  public: // TODO Too many low-level calls to hide this behind a getter

    /// Each tree has four son nodes
    nodenode* fSon[4];

  private:

    /// Link to the next tree node
    treenode* fNext;

  public:

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

  private:

    unsigned int fSize;

  public:

    /// Get size of the bit array
    unsigned int size() const { return fSize; };

  private:

    static int fDebug; ///< Debug level

  public:

    /// \brief Print some debugging information
    void Print(bool recursive = false, int indent = 0);
    /// \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const treenode& tn);

}; // class treenode

} // namespace QwTracking

#endif // QWTRACKINGTREENODE_H
