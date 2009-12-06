/*------------------------------------------------------------------------*//*!

 \file treenode.cc
 \ingroup QwTracking

 \author	Wolfgang Wander <wwc@hermes.desy.de>
 \author	Burnham Stokes <bestokes@jlab.org>
 \author	Wouter Deconinck <wdconinc@mit.edu>

 \date		2009-09-04 18:06:23

 \brief Definition of a treenode which contains the bits that make up a tree pattern

*//*-------------------------------------------------------------------------*/

#include "treenode.h"

namespace QwTracking {

treenode::treenode() { }

treenode::~treenode() { }

/**
 * Print some debugging information
 */
void treenode::Print() {
  std::cout << *this << std::endl;
}

/**
 * Stream some info about the tree node
 * @param stream Stream as lhs of the operator
 * @param tn Tree node as rhs of the operator
 * @return Stream as result of the operator
 */
std::ostream& operator<< (std::ostream& stream, const treenode& tn) {
  stream << "(" << tn.minlevel << "," << tn.maxlevel << ") ";
  stream << "bits = " << tn.bits << ": ";
  for (int i = 0; i < TLAYERS; i++)
    stream << tn.bit[i] << "," ;
  stream << "xref = " << tn.xref;
  return stream;
}

} // namespace QwTracking
