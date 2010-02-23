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
class shortnode {

  public:

    /// \brief Default constructor
    shortnode();
    /// \brief Destructor
    ~shortnode();

  public:

    /// Link to the next node
    shortnode* next;
    /// Pointer to the tree with the information, this can be either a single
    /// tree object or an entire array of trees
    shorttree* tree;

  private:

    /// Number of trees pointed at by the pointer above
    int fNTrees;

  public:

    /// Set the next node
    void SetNext(shortnode* _next) { next = _next; };
    /// Set the tree
    void SetTree(shorttree* _tree, int ntrees = 1) { tree = _tree; fNTrees = ntrees; };
    /// Get the (or a) tree
    shorttree* GetTree(int i = 0) const { return (fNTrees > 1 ? tree : tree); };
    /// Set the number of trees (if allocated as an array)
    void SetNumberOfTrees(int ntrees) { fNTrees = ntrees; };
    /// Get the number of trees
    const int GetNumberOfTrees() const { return fNTrees; };

    /// \brief Print some debugging information
    void Print(int indent = 0);
    /// \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const shortnode& sn);

    /// Get number of objects
    static const int GetCount() { return fCount; };

  private:

    static int fCount; /// Object counter
    static int fDebug; ///< Debug level

}; // class shortnode

} // namespace QwTracking

#endif // QWTRACKINGSHORTNODE_H
