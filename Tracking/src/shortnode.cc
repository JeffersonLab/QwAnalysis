/*------------------------------------------------------------------------*//*!

 \file shortnode.cc
 \ingroup QwTracking

 \author	Wolfgang Wander <wwc@hermes.desy.de>
 \author	Burnham Stokes <bestokes@jlab.org>
 \author	Wouter Deconinck <wdconinc@mit.edu>

 \date		2009-09-04 18:06:23

 \brief Definition of a shortnode, the short version of a nodenode

*//*-------------------------------------------------------------------------*/

#include "shortnode.h"
#include "shorttree.h"

namespace QwTracking {

/**
 * Constructor sets the debug level to zero
 */
shortnode::shortnode()
{
  debug = 0; // debug level
}

/**
 * Destructor deletes the memory occupied by the next nodes and daughter tree
 */
shortnode::~shortnode()
{
  if (next) delete next;
  if (tree) delete tree;
}

/**
 * Print some debugging information
 */
void shortnode::Print() {
  std::cout << *this << std::endl;
}

/**
 * Stream some info about the short node
 * @param stream Stream as lhs of the operator
 * @param sn Short node as rhs of the operator
 * @return Stream as result of the operator
 */
std::ostream& operator<< (std::ostream& stream, const shortnode& sn) {
  stream << *(sn.tree);
  return stream;
}

} // namespace QwTracking
