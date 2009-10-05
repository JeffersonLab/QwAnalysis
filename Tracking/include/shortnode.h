/**
 * \class	shortnode	shortnode.h
 *
 * \author	Wolfgang Wander <wwc@hermes.desy.de>
 * \author	Burnham Stokes <bestokes@jlab.org>
 * \author	Wouter Deconinck <wdconinc@mit.edu>
 *
 * \date	2009-09-04 18:06:23
 * \ingroup	QwTrackingAnl
 *
 * \brief Similar to a nodenode.
 *
 */

#ifndef QWTRACKINGSHORTNODE_H
#define QWTRACKINGSHORTNODE_H

#include <iostream>

namespace QwTracking {

// Forward declaration due to cyclic dependency
class shorttree;

class shortnode {

  public:

    shortnode();
    ~shortnode();

    shortnode *next;
    shorttree *tree;

    /// \brief Print some debugging information
    void Print();
    /// \brief Output stream operator
    friend std::ostream& operator<< (std::ostream& stream, const shortnode& sn);

  private:

    int debug;	///< Debug level

}; // class shortnode

} // namespace QwTracking

#endif // QWTRACKINGSHORTNODE_H
