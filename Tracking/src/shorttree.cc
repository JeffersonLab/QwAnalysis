/*------------------------------------------------------------------------*//*!

 \file shortnode.cc
 \ingroup QwTracking

 \author	Wolfgang Wander <wwc@hermes.desy.de>
 \author	Burnham Stokes <bestokes@jlab.org>
 \author	Wouter Deconinck <wdconinc@mit.edu>

 \date		2009-09-04 18:06:23

 \brief Definition of a shorttree, the short version of a treenode

*//*-------------------------------------------------------------------------*/

#include "shortnode.h"
#include "shorttree.h"

namespace QwTracking {

shorttree::shorttree() { }

/**
 * Destructor deletes the memory occupied by the sons
 */
shorttree::~shorttree()
{
  for (int i = 0; i < 4; i++)
    if (son[i])
      delete son[i];
}

/**
 * Print some debugging information
 */
void shorttree::Print() {
  std::cout << *this << std::endl;
}

/**
 * Stream some info about the short tree
 * @param stream Stream as lhs of the operator
 * @param st Short tree as rhs of the operator
 * @return Stream as result of the operator
 */
std::ostream& operator<< (std::ostream& stream, const shorttree& st) {
  stream << "(" << st.minlevel << "," << "*" << ") ";
  stream << "bits = " << st.bits << ": ";
  for (int i = 0; i < TLAYERS; i++)
    stream << st.bit[i] << "," ;
  stream << "xref = " << st.xref;
  return stream;
}

} // namespace QwTracking
