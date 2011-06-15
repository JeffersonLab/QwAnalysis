/*------------------------------------------------------------------------*//*!

 \file shortnode.h
 \ingroup QwTracking

 \author	Wolfgang Wander <wwc@hermes.desy.de>
 \author	Burnham Stokes <bestokes@jlab.org>
 \author	Wouter Deconinck <wdconinc@mit.edu>

 \date		2009-09-04 18:06:23

 \brief Definition of a shortnode, the short version of a nodenode

*//*-------------------------------------------------------------------------*/

#ifndef QWTRACKINGSHORTNODE_H
#define QWTRACKINGSHORTNODE_H

// System headers
#include <iostream>

// Qweak headers
#include "QwLog.h"
#include "QwObjectCounter.h"

namespace QwTracking {

// Forward declaration due to cyclic dependency
class shorttree;

/**
 * \class	shortnode
 * \ingroup	QwTracking
 *
 * \brief Similar to a nodenode.
 *
 */
class shortnode: public QwObjectCounter<shortnode> {

  public:

    /// \brief Default constructor
    shortnode();
    /// \brief Destructor
    ~shortnode();

  private:

    /// Link to the next node
    shortnode* fNext;

    /// Pointer to the tree with the information, this can be either a single
    /// tree object or an entire array of trees
    shorttree* fTree;

    /// Number of trees pointed at by the pointer above
    int fNTrees;

  public:

    /// Set the next node
    void SetNext(shortnode* next) {
      if (next == this) {
        QwError << "Trying to link next to self" << QwLog::endl; return;
      }
      fNext = next;
    };
    /// Get the next node
    shortnode* GetNext() const { return fNext; };
    /// Get the next node (non-standard notation)
    shortnode* next() const { return fNext; };

    /// Set the tree
    void SetTree(shorttree* tree, int ntrees = 1) { fTree = tree; fNTrees = ntrees; };
    /// \brief Get the tree
    shorttree* GetTree(int i = 0) const;

    /// Set the number of trees (if allocated as an array)
    void SetNumberOfTrees(int ntrees) { fNTrees = ntrees; };
    /// Get the number of trees
    int GetNumberOfTrees() const { return fNTrees; };

  private:

    static int fDebug; ///< Debug level

  public:

    /// \brief Print some debugging information
    void Print(bool recursive = false, int indent = 0);
    /// \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const shortnode& sn);

}; // class shortnode

} // namespace QwTracking

#endif // QWTRACKINGSHORTNODE_H
