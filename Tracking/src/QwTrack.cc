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

  ClearPartialTracks();
}

/**
 * Assignment operator
 * @param track Original track
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
  fQ2 = that.fQ2;
  fW2 = that.fW2;
  fNu = that.fNu;
  fY = that.fY;

  fChi = that.fChi;
  fMomentum = that.fMomentum;
   fTotalEnergy = that.fTotalEnergy;

  fPositionRoff = that.fPositionRoff;
  fPositionPhioff = that.fPositionPhioff;
  fDirectionThetaoff = that.fDirectionThetaoff;
  fDirectionPhioff = that.fDirectionPhioff;

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
  // Initialize the memebers;
  
  fQ2=0.0,fW2=0.0,fNu=0.0;
  fVertexZ=0.0,fVertexR=0.0;
  fTheta=0.0,fPhi=0.0;
  fScatteringAngle = 0.0;
  fChi = 0.0;
  fMomentum = 0.0;
  fTotalEnergy = 0.0;
  fXBj = 0.0;
  fY = 0.0;

  // Initialize all pointers
  fBridge = 0;
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

// Add a linked list of QwPartialTrack's
void QwTrack::AddPartialTrackList(const QwPartialTrack* partialtracklist)
{
  for (const QwPartialTrack *partialtrack = partialtracklist;
         partialtrack; partialtrack = partialtrack->next){
    if (partialtrack->IsValid()){
       AddPartialTrack(partialtrack);
    }
  }
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
    QwPartialTrack* tl = (*partialtrack)->next;
    while (tl) {
      QwMessage  << *tl << QwLog::endl;
      tl = tl->next;
    }
  }
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
