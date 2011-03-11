/*------------------------------------------------------------------------*//*!

 \file nodenode.h
 \ingroup QwTracking

 \author	Wolfgang Wander <wwc@hermes.desy.de>
 \author	Burnham Stokes <bestokes@jlab.org>
 \author	Wouter Deconinck <wdconinc@mit.edu>

 \date		2009-09-04 18:06:23

 \brief Definition of nodenode which links treenodes to their siblings

*//*-------------------------------------------------------------------------*/

#ifndef QWTRACKINGNODENODE_H
#define QWTRACKINGNODENODE_H

// Qweak headers
#include "QwLog.h"
#include "treenode.h"
#include "QwObjectCounter.h"

namespace QwTracking {

// Forward declaration due to cyclic dependency
class treenode;

/**
 * \class	nodenode
 *
 * \ingroup	QwTracking
 *
 * \brief A nodenode is used as a pointer which links treenodes to their siblings.
 *
 * Together with the treenode, any tree pattern can be
 * related to any of its family members.  This allows the
 * tree search algorithms to quickly move through the database
 * to identify matching patterns.
 *
 */
class nodenode: public QwObjectCounter<nodenode> {

  public:

    /// \brief Constructor with next and tree pointers
    nodenode(nodenode* next = 0, treenode* tree = 0);
    /// \brief Destructor
    ~nodenode();

  private:

    /// Pointer to the next node
    nodenode* fNext;
    /// Pointer to the next tree
    treenode* fTree;

  public:

    /// Set the tree
    void SetTree(treenode* tree) {
      fTree = tree;
      if (! tree) QwError << "Trying to assign null tree pointer" << QwLog::endl;
    };
    /// Get the tree
    treenode* GetTree() const { return fTree; };

    /// Set the next node
    void SetNext(nodenode* next) {
      if (next == this) {
        QwError << "Trying to link next to self" << QwLog::endl; return;
      }
      fNext = next;
    };
    /// Get the next node
    nodenode* GetNext() const { return fNext; };
    /// Get the next node (non-standard notation)
    nodenode* next() const { return fNext; };

  private:

    static int fDebug; /// Debug level

}; // class nodenode

} // namespace QwTracking

#endif // QWTRACKINGNODENODE_H
