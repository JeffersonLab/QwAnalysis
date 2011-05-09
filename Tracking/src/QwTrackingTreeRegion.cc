/**
 * \file	QwTrackingTreeRegion.cc
 * \ingroup	QwTracking
 *
 * \author	Burnham Stokes <bestokes@jlab.org>
 * \author	Wouter Deconinck <wdconinc@jlab.org>
 * \date	2009-09-04 18:06:23
 *
 * \brief Implementation of the container for the pattern databases for each detector region.
 *
 * It is useful to separate the pattern database into multiple detector regions
 * to reduce the extent by which a set of hits are compared to a pattern.
 * For instance, there are two databases for the region 3 set of detectors.
 * One for the 'upper' and one for the 'lower' detector sets.  Track segments
 * in the area downstream of the QTOR magnet are approximated by straight lines.
 * Only the VDC is designed to measure track positions, so these two databases
 * designed to contain patterns which ressemble tracks passing through the VDC.
 *
 */

#include "QwTrackingTreeRegion.h"

// Qweak headers
#include "QwLog.h"
#include "shortnode.h"
#include "shorttree.h"

/**
 * Destructor
 */
QwTrackingTreeRegion::~QwTrackingTreeRegion()
{
  // Debug information
  QwDebug << "Deleting QwTrackingTreeRegion: " << this << QwLog::endl;

  // First, delete the nodes by going through the list of trees and following
  // implicitly the linked list of next pointers without branching out through
  // the tree pointers
  QwDebug << "Deleting linked lists of node objects..." << QwLog::endl;
  for (int i = 0; i < fNode.GetNumberOfTrees(); i++)
    for (int j = 0; j < 4; j++)
      if (fNode.GetTree(i)->son[j]) {
        delete fNode.GetTree(i)->son[j];
        fNode.GetTree(i)->son[j] = 0;
      }

  // Then delete the remaining flat list of trees
  QwDebug << "Deleting flat list of tree objects..." << QwLog::endl;
  delete[] fNode.GetTree();

  // Report memory statistics
  if (shortnode::GetObjectsAlive() > 0 || shorttree::GetObjectsAlive() > 0) {
    QwVerbose << "Memory occupied by tree objects (should be a single shortnode when all trees cleared):" << QwLog::endl;
    QwVerbose << "- allocated shortnode objects: " << shortnode::GetObjectsAlive() << QwLog::endl;
    QwVerbose << "- allocated shorttree objects: " << shorttree::GetObjectsAlive() << QwLog::endl;
  }
}


/**
 * Print the list of trees
 */
void QwTrackingTreeRegion::PrintTrees() const
{
  QwOut << "Trees:" << QwLog::endl;
  for (int i = 0; i < 1 /* fNode.GetNumberOfTrees() */; i++) {
    QwOut << "tree " << i << ": ";
    fNode.GetTree(i)->Print(true,1);
  }
}


/**
 * Print the list of nodes
 */
void QwTrackingTreeRegion::PrintNodes() const
{
  QwOut << "Nodes:" << QwLog::endl;
  for (int i = 0; i < fNode.GetNumberOfTrees(); i++) {
    QwOut << "tree " << i << ":" << QwLog::endl;
    fNode.GetTree(i)->Print(false,1);
    for (int j = 0; j < 4; j++) {
      shortnode* node = fNode.GetTree(i)->son[j];
      if (node) QwOut << " son " << j << ":" << QwLog::endl;
      while (node) {
        QwOut << "  tree ";
        node->GetTree(0)->Print(false,2);
        node = node->GetNext();
      } // loop over linked list of nodes
    } // loop over the four sons
  } // loop over list of trees
}
