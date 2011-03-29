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
#include "shortnode.h"
#include "QwObjectCounter.h"

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
 * A shorttree is basically the same as a treenode.  It contains the minimum
 * level of bin division where the bit pattern is valid, the bit pattern in
 * the array fBit, and the width of the pattern.
 *
 * The difference with the treenode is that there is only a link fSon to the
 * son nodes, no link to the next shorttree.  This is because a shorttree is
 * read from file and reconstructing that linkage information is expensive,
 * and not necessary.
 */
class shorttree: public QwObjectCounter<shorttree> {

  public:

    /// \brief Set the default size for short trees
    static void SetDefaultSize(unsigned int size) {
      fDefaultSize = size;
    }

  public:

    /// \brief Default constructor
    shorttree(unsigned int size = fDefaultSize);
    /// \brief Destructor
    ~shorttree();

    /// Minimum level at which this node is valid
    int fMinLevel;

    /// Hit pattern, one bin specified per detector layer
    int* fBit;

    /// Width in bins of the hit pattern
    int fWidth;

  public: // TODO Too many low-level calls to hide this behind a getter

    /// Each tree has four son nodes
    shortnode* son[4];

  private:

    unsigned int fSize;

  public:

    /// Get size of the bit array
    unsigned int size() const { return fSize; };

  private:

    static unsigned int fDefaultSize; ///< Default size

    static int fDebug; ///< Debug level

  public:

    /// \brief Print some debugging information
    void Print(bool recursive = false, int indent = 0);
    /// \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const shorttree& st);

}; // class shorttree

} // namespace QwTracking

#endif // QWTRACKINGSHORTTREE_H
