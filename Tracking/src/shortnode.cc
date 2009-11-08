/**
 * \file	shortnode.cc
 *
 * \author	Wolfgang Wander <wwc@hermes.desy.de>
 * \author	Burnham Stokes <bestokes@jlab.org>
 * \author	Wouter Deconinck <wdconinc@mit.edu>
 *
 * \date	2009-09-04 18:06:23
 * \ingroup	QwTrackingAnl
 *
 */

#include "shortnode.h"
#include "shorttree.h"

namespace QwTracking {

shortnode::shortnode()
{
  debug = 0; // debug level
}

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
