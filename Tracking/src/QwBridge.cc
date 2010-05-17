/*! \file   QwBridge.cc
 *
 *  \author J. Pan
 *  \date   Thu Nov 26 11:44:51 CST 2009
 *
 *  \brief  Bridging R2/R3 partial tracks.
 *
 *  \ingroup QwTracking
 *
 */

#include "QwBridge.h"
ClassImp(QwBridge);

QwBridge::QwBridge()
{
  // Initialize all pointers
  //fHits = 0;
};

QwBridge::~QwBridge()
{
  // Nothing
};

/**
 * Method to print vectors conveniently
 * @param stream Output stream
 * @param v Vector
 * @return Output stream
 */
inline ostream& operator<< (ostream& stream, const TVector3& v)
{
  stream << "(" << v.X() << "," << v.Y() << "," << v.Z() << ")";
  return stream;
}

/// Output stream operator
ostream& operator<< (ostream& stream, const QwBridge& b) {
  stream << "Start: " << b.fStartPosition << "/" << b.fStartDirection << std::endl;
  stream << "End (goal): " << b.fEndPositionGoal << "/" << b.fEndDirectionGoal << std::endl;
  stream << "End (actual): " << b.fEndPositionActual << "/" << b.fEndDirectionActual << std::endl;
  return stream;
}
