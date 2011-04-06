#include "QwPartialTrack.h"
ClassImp(QwPartialTrack)

// ROOT headers
#include "TMath.h"
#include "TRandom.h"

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwVertex.h"

/**
 * Default constructor
 */
QwPartialTrack::QwPartialTrack()
{
  // Initialize
  Initialize();
}

/**
 * Constructor with position and direction vectors
 * @param position Position of the partial track
 * @param direction Direction of the partial track
 */
QwPartialTrack::QwPartialTrack(const TVector3& position, const TVector3& direction)
{
  // Initialize
  Initialize();

  // Calculate slopes
  fSlopeX = direction.X() / direction.Z();
  fSlopeY = direction.Y() / direction.Z();

  // Calculate offset
  fOffsetX = position.X() - fSlopeX * position.Z();
  fOffsetY = position.Y() - fSlopeY * position.Z();
}

/**
 * Copy constructor by reference
 */
QwPartialTrack::QwPartialTrack(const QwPartialTrack& that)
: VQwTrackingElement(that)
{
  // Initialize
  Initialize();

  // Assignment
  *this = that;
}

/**
 * Copy constructor with pointer
 */
QwPartialTrack::QwPartialTrack(const QwPartialTrack* that)
: VQwTrackingElement(*that)
{
  // Initialize
  Initialize();

  // Null pointer          
  if (that == 0) return;

  // Assignment
  *this = *that;
}

/**
 * Destructor
 */
QwPartialTrack::~QwPartialTrack()
{
  // nothing  
}

/**
 * Assignment operator
 */
QwPartialTrack& QwPartialTrack::operator=(const QwPartialTrack& that)
{
  if (this == &that) return *this;

  VQwTrackingElement::operator=(that);
  
  fAlone = that.fAlone;
  
  fOffsetX = that.fOffsetX;
  fOffsetY = that.fOffsetY;
  fSlopeX = that.fSlopeX;
  fSlopeY = that.fSlopeY;

  fChi = that.fChi;
  fAverageResidual=that.fAverageResidual;

  for (size_t i = 0; i < 4; i++)
    for (size_t j = 0; j < 4; j++)
      fCov[i][j] = that.fCov[i][j];

  for (size_t i = 0; i < kNumDirections; i++)
    fTreeLine[i] = that.fTreeLine[i];

  fIsUsed = that.fIsUsed;
  fIsVoid = that.fIsVoid;
  fIsGood = that.fIsGood;

  fNumMiss = that.fNumMiss;
  fNumHits = that.fNumHits;

  triggerhit = that.triggerhit;
  for (size_t i = 0; i < 3; i++)
    trig[i] = that.trig[i];

  cerenkovhit = that.cerenkovhit;
  for (size_t i = 0; i < 3; i++)
    cerenkov[i] = that.cerenkov[i];

  for (size_t i = 0; i < 3; i++) {
    pR2hit[i] = that.pR2hit[i];
    uvR2hit[i] = that.uvR2hit[i];
    pR3hit[i] = that.pR3hit[i];
    uvR3hit[i] = that.uvR3hit[i];
  }

  // Copy tree lines
  ClearTreeLines();
  AddTreeLineList(that.fQwTreeLines);

  return *this;
}


/**
 * Perform object initialization
 */
void QwPartialTrack::Initialize()
{
  // Initialize the member fields
  fOffsetX = 0.0; fOffsetY = 0.0;
  fSlopeX = 0.0;  fSlopeY = 0.0;
  fIsVoid = false; fIsUsed = false; fIsGood = false;

  // Initialize pointers
  next = 0;
  for (int i = 0; i < kNumDirections; i++)
    fTreeLine[i] = 0;

  //Only 2 Plane 0 treelines
  fAlone=0;
}


/**
 * Determine the chi^2 for a partial track, weighted by the number of hits
 *
 * @return Weighted chi^2
 */
double QwPartialTrack::GetChiWeight () const
{
  // Determine the weight if there are enough hits
  if (fNumHits >= fNumMiss) {
    // NOTE Added +1 to get this to work if fNumHits == fNumMiss (wdc)
    double weight = (double) (fNumHits + fNumMiss + 1)
                           / (fNumHits - fNumMiss + 1);
    return weight * weight * fChi;
    // TODO Why is the weight squared here, but not in the weighted chi^2 for treelines?

  } else {

    QwDebug << "miss = " << fNumMiss << ", hit = " << fNumHits << QwLog::endl;
    return 100.0; // This is bad...
  }
}


/**
 * Calculate the average residual of this partial track over all treelines
 */
double QwPartialTrack::CalculateAverageResidual()
{
  int numTreeLines = 0;
  double sumResiduals = 0.0;
  for (EQwDirectionID dir = kDirectionX; dir <= kDirectionV; dir++) {
    for (QwTrackingTreeLine* treeline = fTreeLine[dir]; treeline;
         treeline = treeline->next) {
      if (treeline->IsUsed()) {
        numTreeLines++;
        sumResiduals += treeline->GetAverageResidual();
      }
    } // end of loop over treelines
  } // end of loop over directions
  fAverageResidual = sumResiduals / numTreeLines;
  return fAverageResidual;
}


// Clear the local TClonesArrays
void QwPartialTrack::Clear(Option_t *option)
{
  ClearTreeLines(option);
}

// Delete the static TClonesArrays
void QwPartialTrack::Reset(Option_t *option)
{
  ResetTreeLines(option);
}

// Create a new QwTreeLine
QwTrackingTreeLine* QwPartialTrack::CreateNewTreeLine()
{
  QwTrackingTreeLine* treeline = new QwTrackingTreeLine();
  AddTreeLine(treeline);
  return treeline;
}

// Add an existing QwTreeLine
void QwPartialTrack::AddTreeLine(QwTrackingTreeLine* treeline)
{
  fQwTreeLines.push_back(new QwTrackingTreeLine(treeline));
  fNQwTreeLines++;
}

// Add a linked list of QwTreeLine's
void QwPartialTrack::AddTreeLineList(QwTrackingTreeLine* treelinelist)
{
  for (QwTrackingTreeLine *treeline = treelinelist;
         treeline; treeline = treeline->next){
    if (treeline->IsValid()){
       AddTreeLine(treeline);
    }
  }
}

// Add a list of tree lines
void QwPartialTrack::AddTreeLineList(const std::vector<QwTrackingTreeLine*> &treelinelist)
{ 
  for (std::vector<QwTrackingTreeLine*>::const_iterator treeline = treelinelist.begin();
       treeline != treelinelist.end(); treeline++)
    AddTreeLine(*treeline);
}

// Clear the local TClonesArray of tree lines
void QwPartialTrack::ClearTreeLines(Option_t *option)
{
  for (size_t i = 0; i < fQwTreeLines.size(); i++) {
    QwTrackingTreeLine* tl = fQwTreeLines.at(i);
    delete tl;
  }
  fQwTreeLines.clear();
  fNQwTreeLines = 0;
}

// Delete the static TClonesArray of tree lines
void QwPartialTrack::ResetTreeLines(Option_t *option)
{
  ClearTreeLines();
}

// Print the tree lines
void QwPartialTrack::PrintTreeLines(Option_t *option) const
{
  for (std::vector<QwTrackingTreeLine*>::const_iterator treeline = fQwTreeLines.begin();
       treeline != fQwTreeLines.end(); treeline++) {
    std::cout << **treeline << std::endl;
    QwTrackingTreeLine* tl = (*treeline)->next;
    while (tl) {
      std::cout << *tl << std::endl;
      tl = tl->next;
    }
  }
}


/**
 * Print some debugging information
 */
void QwPartialTrack::Print(const Option_t* options) const
{
  if (!this) return;
  QwVerbose << *this << QwLog::endl;
  if (next) next->Print();
}

/**
 * Print some debugging information for valid partial tracks
 */
void QwPartialTrack::PrintValid()
{
  if (!this) return;
  if (this->IsValid()) QwVerbose << *this << QwLog::endl;
  if (next) next->PrintValid();
}

/**
 * Output stream operator overloading
 */
ostream& operator<< (ostream& stream, const QwPartialTrack& pt)
{
  stream << "pt: ";
  if (pt.GetRegion() != kRegionIDNull)
    stream << "(" << pt.GetRegion() << "/" << "?UD"[pt.GetPackage()] << "); ";
  stream << "x,y(z=0) = (" << pt.fOffsetX/Qw::cm << " cm, " << pt.fOffsetY/Qw::cm << " cm), ";
  stream << "d(x,y)/dz = (" << pt.fSlopeX << ", " << pt.fSlopeY << ")";

  if (pt.fChi > 0.0) { // parttrack has been fitted
    stream << ", chi = " << pt.fChi;
  }
  if (pt.fAverageResidual > 0.0) {
    stream << ", res = " << pt.fAverageResidual;
  }
  if (pt.IsVoid()) stream << " (void)";
  return stream;
}


/**
 * Determines the position of the track at the given z position
 */
const TVector3 QwPartialTrack::GetPosition(const double z) const
{
  TVector3 position;
  position.SetX(fOffsetX + fSlopeX * z);
  position.SetY(fOffsetY + fSlopeY * z);
  position.SetZ(z);
  return position;
}

/**
 * Determines the direction of the track at the given z position
 */
const TVector3 QwPartialTrack::GetMomentumDirection() const
{
  TVector3 direction;
  double kz = sqrt(fSlopeX * fSlopeX + fSlopeY * fSlopeY + 1);
  direction.SetX(fSlopeX / kz);
  direction.SetY(fSlopeY / kz);
  direction.SetZ(1 / kz);
  return direction;
}

/**
 * Smear the position of the partial track
 * @param sigma_x Standard deviation in x
 * @param sigma_y Standard deviation in y
 */
QwPartialTrack& QwPartialTrack::SmearPosition(double sigma_x, double sigma_y)
{
  fOffsetX = gRandom->Gaus(fOffsetX, sigma_x);
  fOffsetY = gRandom->Gaus(fOffsetY, sigma_y);
  return *this;
}

/**
 * Smear the theta angle of the partial track
 * @param sigma Standard deviation in theta
 */
QwPartialTrack& QwPartialTrack::SmearAngleTheta(double sigma)
{
  double theta = GetMomentumDirectionTheta();
  double phi = GetMomentumDirectionPhi();
  theta = gRandom->Gaus(theta, sigma);
  fSlopeX = sin(theta) * cos(phi);
  fSlopeY = sin(theta) * sin(phi);
  return *this;
}

/**
 * Smear the phi angle of the partial track
 * @param sigma Standard deviation in phi
 */
QwPartialTrack& QwPartialTrack::SmearAnglePhi(double sigma)
{
  double theta = GetMomentumDirectionTheta();
  double phi = GetMomentumDirectionPhi();
  phi = gRandom->Gaus(phi, sigma);
  fSlopeX = sin(theta) * cos(phi);
  fSlopeY = sin(theta) * sin(phi);
  return *this;
}


/**
 * This method checks determines the intersection of the partial track
 * with a planar detector.
 */
const QwVertex* QwPartialTrack::DeterminePositionInDetector(const QwDetectorInfo* detector)
{
  QwVertex* vertex = 0;
  if (detector->GetDetectorRotation() != 0) {
    // Get intersection with z-plane
    double z = detector->GetZPosition();
    TVector3 position = GetPosition(z);
    // Check acceptance with active width
    if (fabs(position.X() - detector->GetPosition().X()) < detector->GetActiveWidthX()/2
     && fabs(position.Y() - detector->GetPosition().Y()) < detector->GetActiveWidthY()/2) {
      vertex = new QwVertex(position);
    } else {
      QwMessage << "No support for intersections of partial tracks "
          << "with tilted detectors yet..." << QwLog::endl;
    }
  }
  return vertex;
}


/**
 * This method checks determines the intersection of the partial track
 * with the target.
 *
 * \note At this point the target defined at one z value only.  A more careful
 * determination of the primary vertex will be implemented using the general
 * QwVertex class, e.g. QwVertex* primary_vertex = QwVertex (partial_track, beam)
 */
const QwVertex* QwPartialTrack::DeterminePositionInTarget (const QwGeometry& geometry)
{
  // TODO target should be put in in the geometry file
  TVector3 primary = GetPosition(0.0);
  QwVerbose << "Target vertex at : (" << primary.X() << "," << primary.Y() << "," << primary.Z() << ")" << QwLog::endl;
  return 0;
}

/**
 * This method checks whether the region 3 partial track has an intersection
 * with the trigger scintillators in a detector package.
 *
 * The trigger scintillators are assumed to be the only (or at least the first)
 * detectors in region 3.
 */
const QwVertex* QwPartialTrack::DeterminePositionInTriggerScintillators (const QwGeometry& geometry)
{
  // Get the trigger scintillator detectors in this package
  QwGeometry trigscint(geometry.in(kRegionIDTrig).in(GetPackage()));

  // Find the first hit
  for (size_t i = 0; i < trigscint.size(); i++) {
    const QwVertex* vertex = DeterminePositionInDetector(trigscint.at(i));
    if (vertex) return vertex;
  }
  return 0;
}

/**
 * This method checks whether the region 3 partial track has an intersection
 * with the trigger scintillators in a detector package.
 *
 * The cerenkov bars are assumed to be the only (or at least the first)
 * detectors in region 3.
 */
const QwVertex* QwPartialTrack::DeterminePositionInCerenkovBars (const QwGeometry& geometry)
{
  // Get the Cherenkov detectors in this package
  QwGeometry cerenkov(geometry.in(kRegionIDCer).in(GetPackage()));

  // Find the first hit
  for (size_t i = 0; i < cerenkov.size(); i++) {
    const QwVertex* vertex = DeterminePositionInDetector(cerenkov.at(i));
    if (vertex) return vertex;
  }
  return 0;
}

/**
 * This method checks whether the region 2 partial track has an intersection
 * with the horizontal drift chambers in a detector package.
 *
 * The drift chambers are assumed to be the only (or at least the first)
 * detectors in region 2.
 */
const QwVertex* QwPartialTrack::DeterminePositionInHDC (const QwGeometry& geometry)
{
  // Get the HDC detectors in this package
  QwGeometry hdc(geometry.in(kRegionID2).in(GetPackage()));
  // Find the first hit
  for (size_t i = 0; i < hdc.size(); i++) {
    const QwVertex* vertex = DeterminePositionInDetector(hdc.at(i));
    if (vertex) return vertex;
  }
  return 0;
}
