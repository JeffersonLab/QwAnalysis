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
#include "globals.h"
#include "QwLog.h"

// Qweak tree headers
#include "shortnode.h"

namespace QwTracking {

// Default size
unsigned int shorttree::fDefaultSize = MAX_LAYERS;

// Reset debug level
int shorttree::fDebug = 0;

/**
 * Default constructor, initializes the son pointer to null
 * @param size Size of the bit pattern
 */
shorttree::shorttree(unsigned int size)
{
  // Set the size
  fSize = size;
  fBit = new int[fSize];
  for (unsigned int i = 0; i < fSize; i++) fBit[i] = 0;

  // Initialize pointers
  for (int i = 0; i < 4; i++)
    son[i] = 0;
}

/**
 * Destructor deletes the memory occupied by the sons
 */
shorttree::~shorttree()
{
  // Delete the bit pattern
  delete[] fBit;
}

/**
 * Print some debugging information
 */
void shorttree::Print(bool recursive, int indent)
{
  // Print this node
  std::string indentation;
  for (int i = 0; i < indent; i++) indentation += " ";
  QwOut << this << ": " << *this << QwLog::endl;
  // Bit pattern
  for (unsigned int i = 0; i < fSize; i++) {
    QwOut << indentation;
    for (int j = 0; j < (1 << (fMinLevel + 1)); j++) {
      if (j == fBit[i]) QwOut << "|";
      else QwOut << ".";
    }
    QwOut << QwLog::endl;
  }

  // Descend to the sons of this node
  for (int i = 0; recursive && i < 4; i++) {
    shortnode* node = this->son[i];
    if (node) {
      QwOut << indentation << "son " <<  i << ": ";
      node->Print(recursive,indent+1);
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
  for (unsigned int i = 0; i < st.fSize; i++)
    stream << st.fBit[i] << ",";
  stream << " width = " << st.fWidth;
  return stream;
}

} // namespace QwTracking
