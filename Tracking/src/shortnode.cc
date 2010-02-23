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

// Qweak headers
#include "QwLog.h"

// Qweak tree headers
#include "shorttree.h"

namespace QwTracking {

int shortnode::fCount = 0;
int shortnode::fDebug = 0;

/**
 * Constructor sets the debug level to zero
 */
shortnode::shortnode()
{
  // Initialize pointers
  next = 0;
  tree = 0;
  // No tree pointed at yet
  fNTrees = 0;

  // Count objects
  fCount++;
}

/**
 * Destructor deletes the memory occupied by the next nodes and daughter tree
 */
shortnode::~shortnode()
{
  // Delete the next node in the linked list (recursion)
  if (next) delete next;

  // Count objects
  fCount--;
}

/**
 * Print some debugging information
 */
void shortnode::Print(int indent)
{
  // Print this node
  std::string indentation;
  for (int i = 0; i < indent; i++) indentation += " ";
  QwOut << this << ": " << *this << QwLog::endl;

  // Print next node
  if (next) {
    QwOut << indentation << "next: ";
    next->Print(indent+1);
  }

  // Print tree
  if (tree) {
    QwOut << indentation << "tree: ";
    tree->Print(indent+1);
  }
}

/**
 * Stream some info about the short node
 * @param stream Stream as lhs of the operator
 * @param sn Short node as rhs of the operator
 * @return Stream as result of the operator
 */
std::ostream& operator<< (std::ostream& stream, const shortnode& sn)
{
  stream << *(sn.tree);
  return stream;
}

} // namespace QwTracking
