#include "QwTrack.h"
ClassImp(QwTrack)

/**
 * Default constructor
 */
QwTrack::QwTrack()
{
  // Initialize
  Initialize();

  QwMessage << "QwTrack created " << this << QwLog::endl;
}

/**
 * Constructor with front and back partial track
 */
QwTrack::QwTrack(const QwPartialTrack* front, const QwPartialTrack* back)
{
  // Initialize
  Initialize();

  // Copy tracks
  front = new QwPartialTrack(front);
  back  = new QwPartialTrack(back);

  QwMessage << "QwTrack created " << this << QwLog::endl;
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

  QwMessage << "QwTrack created " << this << QwLog::endl;
}

/**
 * Virtual destructor
 */
QwTrack::~QwTrack()
{
  // Delete objects
  if (front) delete front;
  if (back)  delete back;

  QwMessage << "QwTrack deleted " << this << QwLog::endl;
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
}
