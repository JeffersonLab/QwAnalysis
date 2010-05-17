#include "QwTrack.h"
ClassImp(QwTrack);

QwTrack::QwTrack()
{
  // Initialize all pointers
  fBridge = 0;
  next = 0;
  ynext = 0;
  usednext = 0;
  front = 0;
  back = 0;
  beamvertex = 0;
}

QwTrack::~QwTrack()
{
  // Nothing
}

/**
 * Output stream operator overloading
 */
ostream& operator<< (ostream& stream, const QwTrack& t)
{
  stream << "track: ";
  if (t.GetRegion() != kRegionIDNull)
    stream << "(" << t.GetRegion() << "/" << "?UD"[t.GetPackage()] << ") ";
  return stream;
};
