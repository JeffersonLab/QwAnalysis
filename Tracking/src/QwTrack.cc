#include "QwTrack.h"
ClassImp(QwTrack)

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
}

/**
 * Copy constructor by reference
 * @param track Original track
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
 * @param track Original track
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

  if (fBridge) delete fBridge;
}

/**
 * Assignment operator
 */
QwTrack& QwTrack::operator=(const QwTrack& that)
{
  if (this == &that) return *this;

  VQwTrackingElement::operator=(that);

  fVertexZ = that.fVertexZ;
  fVertexR = that.fVertexR;
  fTheta = that.fTheta;
  fPhi   = that.fPhi;
  fScatteringAngle = that.fScatteringAngle;

  fXBj = that.fXBj;
  fY = that.fY;
  fQ2 = that.fQ2;
  fW2 = that.fW2;
  fNu = that.fNu;

  fChi = that.fChi;
  fMomentum = that.fMomentum;
   fTotalEnergy = that.fTotalEnergy;

  fPositionRoff = that.fPositionRoff;
  fPositionPhioff = that.fPositionPhioff;
  fDirectionThetaoff = that.fDirectionThetaoff;
  fDirectionPhioff = that.fDirectionPhioff;

  return *this;
}

/**
 * Initialization
 */
void QwTrack::Initialize()
{

  // Initialize the memebers;
  
  fQ2=0.0,fW2=0.0,fNu=0.0;
  fVertexZ=0.0,fVertexR=0.0;
  fTheta=0.0,fPhi=0.0;
  fScatteringAngle=0.0;
  fChi=0.0;
  fMomentum=0.0;
  fTotalEnergy=0.0;
  fXBj=0.0;
  fY=0.0;

  // Initialize all pointers
  fBridge = 0;
  fFront = 0;
  fBack = 0;
  beamvertex = 0;
  next = 0;
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
