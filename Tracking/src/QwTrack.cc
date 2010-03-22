#include "QwTrack.h"
ClassImp(QwTrack);

QwTrack::QwTrack() { }

QwTrack::~QwTrack() { }

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
