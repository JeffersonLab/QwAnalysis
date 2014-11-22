#include "QwTrack.h"
#if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
ClassImp(QwTrack);
#endif

/**
 * Default constructor
 */
QwTrack::QwTrack()
{
  // Initialize
  Initialize();
}

/**
 * Constructor with front and back partial track
 * @param front Front partial track
 * @param back Back partial track
 */
QwTrack::QwTrack(const QwPartialTrack* front, const QwPartialTrack* back)
{
  // Initialize
  Initialize();

  // Null pointer
  if (front == 0 || back == 0) return;

  if (front->GetPackage() == back->GetPackage())
    SetPackage(front->GetPackage());

  // Copy tracks
  fFront = new QwPartialTrack(front);
  fBack  = new QwPartialTrack(back);

  // Add partial tracks
  AddPartialTrack(front);
  AddPartialTrack(back);
}

/**
 * Copy constructor by reference
 * @param that Original track
 */
QwTrack::QwTrack(const QwTrack& that)
: VQwTrackingElement(that)
{
  // Initialize
  Initialize();

  *this = that;

  // Copy tracks
  fFront = new QwPartialTrack(that.fFront);
  fBack  = new QwPartialTrack(that.fBack);
}

/**
 * Copy constructor from pointer
 * @param that Original track
 */
QwTrack::QwTrack(const QwTrack* that)
: VQwTrackingElement(*that)
{
  // Initialize
  Initialize();

  // Null pointer
  if (that == 0) return;

  *this = *that;

  // Copy tracks
  if (that->fFront) fFront = new QwPartialTrack(that->fFront);
  if (that->fBack)  fBack  = new QwPartialTrack(that->fBack);
}

/**
 * Virtual destructor
 */
QwTrack::~QwTrack()
{
  // Delete objects
  if (fFront) delete fFront;
  if (fBack)  delete fBack;

  ClearPartialTracks();
}

/**
 * Assignment operator
 * @param that Original track
 */
QwTrack& QwTrack::operator=(const QwTrack& that)
{
  if (this == &that) return *this;

  VQwTrackingElement::operator=(that);

  fPhi   = that.fPhi;
  fTheta = that.fTheta;
  fVertexZ = that.fVertexZ;
  fVertexR = that.fVertexR;

  fChi = that.fChi;
  fMomentum = that.fMomentum;

  fIterationsNewton = that.fIterationsNewton;
  fIterationsRungeKutta = that.fIterationsRungeKutta;

  fPositionDiff = that.fPositionDiff;
  fPositionXoff = that.fPositionXoff;
  fPositionYoff = that.fPositionYoff;
  fPositionRoff = that.fPositionRoff;
  fPositionPhioff = that.fPositionPhioff;
  fPositionThetaoff = that.fPositionThetaoff;

  fDirectionDiff = that.fDirectionDiff;
  fDirectionXoff = that.fDirectionXoff;
  fDirectionYoff = that.fDirectionYoff;
  fDirectionZoff = that.fDirectionZoff;
  fDirectionPhioff = that.fDirectionPhioff;
  fDirectionThetaoff = that.fDirectionThetaoff;

  fStartPosition = that.fStartPosition;
  fStartDirection = that.fStartDirection;

  fEndPositionGoal = that.fEndPositionGoal;
  fEndDirectionGoal = that.fEndDirectionGoal;

  fEndPositionActual = that.fEndPositionActual;
  fEndDirectionActual = that.fEndDirectionActual;

  fIterationsRK4 = that.fIterationsRK4;
  fEndPositionActualRK4 = that.fEndPositionActualRK4;
  fEndDirectionActualRK4 = that.fEndDirectionActualRK4;

  fIterationsRKF45 = that.fIterationsRKF45;
  fEndPositionActualRKF45 = that.fEndPositionActualRKF45;
  fEndDirectionActualRKF45 = that.fEndDirectionActualRKF45;

  // Copy partial tracks
  ClearPartialTracks();
  AddPartialTrackList(that.fQwPartialTracks);

 return *this;
}

/**
 * Initialization
 */
void QwTrack::Initialize()
{
  // Initialize the members;
  fChi = 0.0;
  fMomentum = 0.0;

  // Initialize all pointers
  fFront = 0;
  fBack = 0;
}



// Create a new QwPartialTrack
QwPartialTrack* QwTrack::CreateNewPartialTrack()
{
  QwPartialTrack* partialtrack = new QwPartialTrack();
  AddPartialTrack(partialtrack);
  return partialtrack;
}

// Add an existing QwPartialTrack
void QwTrack::AddPartialTrack(const QwPartialTrack* partialtrack)
{
  fQwPartialTracks.push_back(new QwPartialTrack(partialtrack));
  ++fNQwPartialTracks;
}

// Add a list of partial tracks
void QwTrack::AddPartialTrackList(const std::vector<QwPartialTrack*> &partialtracklist)
{
  for (std::vector<QwPartialTrack*>::const_iterator partialtrack = partialtracklist.begin();
       partialtrack != partialtracklist.end(); partialtrack++)
    AddPartialTrack(*partialtrack);
}

// Clear the local TClonesArray of partial tracks
void QwTrack::ClearPartialTracks(Option_t *option)
{
  for (size_t i = 0; i < fQwPartialTracks.size(); i++) {
    QwPartialTrack* tl = fQwPartialTracks.at(i);
    delete tl;
  }
  fQwPartialTracks.clear();
  fNQwPartialTracks = 0;
}

// Delete the static TClonesArray of partial tracks
void QwTrack::ResetPartialTracks(Option_t *option)
{
  ClearPartialTracks();
}

// Print the partial tracks
void QwTrack::PrintPartialTracks(Option_t *option) const
{
  for (std::vector<QwPartialTrack*>::const_iterator partialtrack = fQwPartialTracks.begin();
       partialtrack != fQwPartialTracks.end(); partialtrack++) {
    QwMessage  << **partialtrack << QwLog::endl;
  }
}


/**
 * Output stream operator overloading
 */
std::ostream& operator<< (std::ostream& stream, const QwTrack& t)
{
  stream << "track: ";
  if (t.GetRegion() != kRegionIDNull) {
    stream << "(" << t.GetRegion() << "/" << "?UD"[t.GetPackage()] << ") ";
    stream << "Start: " << t.fStartPosition << "/" << t.fStartDirection << std::endl;
    stream << "End (goal): " << t.fEndPositionGoal << "/" << t.fEndDirectionGoal << std::endl;
    stream << "End (actual): " << t.fEndPositionActual << "/" << t.fEndDirectionActual << std::endl;
  }
  return stream;
}
