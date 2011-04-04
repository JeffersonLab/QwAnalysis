/*------------------------------------------------------------------------*//*!

 \file nodenode.cc
 \ingroup QwTracking

 \author	Wolfgang Wander <wwc@hermes.desy.de>
 \author	Burnham Stokes <bestokes@jlab.org>
 \author	Wouter Deconinck <wdconinc@mit.edu>

 \date		2009-09-04 18:06:23

 \brief Definition of nodenode which links treenodes to their siblings

*//*-------------------------------------------------------------------------*/

#include "nodenode.h"

// Qweak headers
#include "QwLog.h"

namespace QwTracking {

int nodenode::fDebug = 0;


/**
 * Constructor with next and tree pointers
 * @param next Pointer to the next node in the linked list
 * @param tree Pointer to the next tree for this node
 */
nodenode::nodenode(nodenode* next, treenode* tree)
{
  // Initialize pointers
  fNext = next;
  fTree = tree;
}


/**
 * Destructor
 */
nodenode::~nodenode()
{
  // Delete the pointer to the next node
  if (fNext) delete fNext;
}

} // namespace QwTracking
