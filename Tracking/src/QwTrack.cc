#include "QwTrack.h"
ClassImp(QwTrack);

/**
 * Default constructor
 */
QwTrack::QwTrack()
{
  // Initialize
  Initialize();
}

/**
 * Copy constructor by reference
 * @param track Original track
 */
QwTrack::QwTrack(const QwTrack& track)
{
  // Initialize
  Initialize();

  // Copy tracks
  front = new QwPartialTrack(track.front);
  back  = new QwPartialTrack(track.back);
}

/**
 * Virtual destructor
 */
QwTrack::~QwTrack()
{
  // Delete objects
  //if (front) delete front;  //jpan: not sure why the program crash here, temporarily comment it out
  //if (back)  delete back;
}

/**
 * Initialization
 */
void QwTrack::Initialize()
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
