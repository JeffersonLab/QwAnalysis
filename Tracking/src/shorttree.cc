/*------------------------------------------------------------------------*//*!

 \file shortnode.cc
 \ingroup QwTracking

 \author	Wolfgang Wander <wwc@hermes.desy.de>
 \author	Burnham Stokes <bestokes@jlab.org>
 \author	Wouter Deconinck <wdconinc@mit.edu>

 \date		2009-09-04 18:06:23

 \brief Definition of a shorttree, the short version of a treenode

*//*-------------------------------------------------------------------------*/

#include "shorttree.h"

// Qweak headers
#include "QwLog.h"

// Qweak tree headers
#include "shortnode.h"

namespace QwTracking {

int shorttree::fCount = 0;
int shorttree::fDebug = 0;

/**
 * Default constructor, initializes the son pointer to null
 */
shorttree::shorttree()
{
  for (int i = 0; i < 4; i++)
    son[i] = 0;

  // Count objects
  fCount++;
}

/**
 * Destructor deletes the memory occupied by the sons
 */
shorttree::~shorttree()
{
  // Count objects
  fCount--;
}

/**
 * Print some debugging information
 */
void shorttree::Print(int indent)
{
  // Print this node
  std::string indentation;
  for (int i = 0; i < indent; i++) indentation += " ";
  QwOut << this << ": " << *this << QwLog::endl;

  // Descend to the sons of this node
  for (int i = 0; i < 4; i++) {
    shortnode* node = this->son[i];
    if (node) {
      QwOut << indentation << "son " <<  i << ": ";
      node->Print(indent+1);
    }
  }
}

/**
 * Stream some info about the short tree
 * @param stream Stream as lhs of the operator
 * @param st Short tree as rhs of the operator
 * @return Stream as result of the operator
 */
std::ostream& operator<< (std::ostream& stream, const shorttree& st)
{
  stream << "(" << st.fMinLevel << "," << "*" << ") ";
  stream << "bits = " << st.bits << ": ";
  for (int i = 0; i < TLAYERS; i++)
    stream << st.bit[i] << "," ;
  stream << "xref = " << st.xref;
  return stream;
}

} // namespace QwTracking
