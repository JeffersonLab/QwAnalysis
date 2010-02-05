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

// Qweak headers
#include "QwLog.h"
#include "nodenode.h"

namespace QwTracking {

int treenode::fCount = 0;
int treenode::fDebug = 0;

/**
 * Default constructor
 */
treenode::treenode()
{
  // Initialize pointers
  fNext = 0;
  for (int i = 0; i < 4; i++) fSon[i] = 0;

  // Count objects
  fCount++;
};

/**
 * Copy-constructor from object
 * @param node Original tree node
 */
treenode::treenode(treenode& node)
{
  // Copy the node
  *this = node;

  // Set the external reference link
  this->xref = -1L;

  // Initialize pointers
  fNext = 0;
  for (int i = 0; i < 4; i++) fSon[i] = 0;

  // Count objects
  fCount++;
};

/**
 * Copy-constructor from pointer
 * @param node Pointer to original tree node
 */
treenode::treenode(treenode* node)
{
  // Copy the node
  *this = *node;

  // Set the external reference link
  this->xref = -1L;

  // Initialize pointers
  fNext = 0;
  for (int i = 0; i < 4; i++) fSon[i] = 0;

  // Count objects
  fCount++;
};

treenode::~treenode()
{
  QwDebug << "Deleting treenode: " << this << QwLog::endl;

  // Delete the sons
  for (int i = 0; i < 4; i++)
    if (fSon[i]) delete fSon[i];

  // Count objects
  fCount--;
}

/**
 * Print the entire tree (descending all the way down)
 * \warning This will produce a lot of screen output!
 *
 * @param indent Indentation level (for recursive calls)
 */
void treenode::Print(int indent)
{
  // Print this node
  std::string indentation;
  for (int i = 0; i < indent; i++) indentation += " ";
  QwOut << indentation << this << ": " << *this << QwLog::endl;

  // Bail out if too deep (probably in a loop)
  if (indent > 50) {
    QwWarning << "Tree is too deep to print, probably a self-reference somewhere" << QwLog::endl;
    return;
  }

  // Descend to the sons of this node
  for (int i = 0; i < 4; i++) {
    nodenode* node = this->fSon[i];
    if (node) {
      treenode* tree = node->GetTree();
      if (tree)
        tree->Print(indent+1);
    } // if (node)

  } // loop over sons
}

/**
 * Stream summary info about the tree node
 * @param stream Stream as lhs of the operator
 * @param tn Tree node as rhs of the operator
 * @return Stream as result of the operator
 */
std::ostream& operator<< (std::ostream& stream, const treenode& tn)
{
  stream << "(" << tn.fMinLevel << "," << tn.fMaxLevel << ") ";
  stream << "bits = " << tn.bits << ": ";
  for (int i = 0; i < TLAYERS; i++)
    stream << tn.bit[i] << "," ;
  stream << "xref = " << tn.xref;
  return stream;
}

} // namespace QwTracking
