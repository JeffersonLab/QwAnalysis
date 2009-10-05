/**
 * \class	nodenode	nodenode.h
 *
 * \author	Wolfgang Wander <wwc@hermes.desy.de>
 * \author	Burnham Stokes <bestokes@jlab.org>
 * \author	Wouter Deconinck <wdconinc@mit.edu>
 *
 * \date	2009-09-04 18:06:23
 * \ingroup	QwTrackingAnl
 *
 * \brief A nodenode is used as a pointer which links treenodes to their siblings.
 *
 * Together with the treenode, any tree pattern can be
 * related to any of its family members.  This allows the
 * tree search algorithms to quickly move through the database
 * to identify matching patterns.
 *
 */

#ifndef QWTRACKINGNODENODE_H
#define QWTRACKINGNODENODE_H

#include "treenode.h"

namespace QwTracking {


// Forward declaration due to cyclic dependency
class treenode;

class nodenode {

  public:

    nodenode();
    ~nodenode();

    nodenode *next;	///< Pointer to the next node
    treenode *tree;	///< Pointer to the list of

}; // class nodenode

} // namespace QwTracking

#endif // QWTRACKINGNODENODE_H
