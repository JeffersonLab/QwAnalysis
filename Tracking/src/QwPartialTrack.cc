#include "QwPartialTrack.h"
ClassImp(QwPartialTrack);

// ROOT headers
#include "TMath.h"
#include "TRandom.h"

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"

// Qweak headers (deprecated)
#include "Det.h"

// Initialize the static lists
TClonesArray* QwPartialTrack::gQwTreeLines = 0;

/**
 * Default constructor
 */
QwPartialTrack::QwPartialTrack()
{
  InitializeTreeLines();

  fOffsetX = 0.0; fOffsetY = 0.0;
  fSlopeX = 0.0;  fSlopeY = 0.0;
  fIsVoid = false; fIsUsed = false; fIsGood = false;
  next = 0;
  for (int i = 0; i < kNumDirections; i++)
    tline[i] = 0;
}

/**
 * Constructor with position and direction vectors
 * @param position Position of the partial track
 * @param direction Direction of the partial track
 */
QwPartialTrack::QwPartialTrack(const TVector3 position, const TVector3 direction)
{
  InitializeTreeLines();

  // Calculate slopes
  fSlopeX = direction.X() / direction.Z();
  fSlopeY = direction.Y() / direction.Z();

  // Calculate offset
  fOffsetX = position.X() - fSlopeX * position.Z();
  fOffsetY = position.Y() - fSlopeY * position.Z();
}


/**
 * Determine the chi^2 for a partial track, weighted by the number of hits
 *
 * @return Weighted chi^2
 */
double QwPartialTrack::GetChiWeight ()
{
  // Determine the weight if there enough hits
  if (numhits >= nummiss) {
    // NOTE Added +1 to get this to work if numhits == nummiss (wdc)
    double weight = (double) (numhits + nummiss + 1)
                           / (numhits - nummiss + 1);
    return weight * weight * fChi;
    // TODO Why is the weight squared here, but not in the weighted chi^2 for treelines?

  } else {

    QwDebug << "miss = " << nummiss << ", hit = " << numhits << QwLog::endl;
    return 100.0; // This is bad...
  }
}


/**
 * Calculate the average residual of this partial track over all treelines
 */
const double QwPartialTrack::CalculateAverageResidual()
{
  int numTreeLines = 0;
  double sumResiduals = 0.0;
  for (EQwDirectionID dir = kDirectionX; dir <= kDirectionV; dir++) {
    for (QwTrackingTreeLine* treeline = tline[dir]; treeline;
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
  ClearTreeLines();
};

// Delete the static TClonesArrays
void QwPartialTrack::Reset(Option_t *option)
{
  ResetTreeLines();
};

/**
 * Initialize the list of tree lines
 */
void QwPartialTrack::InitializeTreeLines()
{
  // Create the static TClonesArray for the tree lines if not existing yet
  if (! gQwTreeLines)
    gQwTreeLines = new TClonesArray("QwTrackingTreeLine", QWPARTIALTRACK_MAX_NUM_TREELINES);
  // Set local TClonesArray to static TClonesArray and zero hits
  fQwTreeLines = gQwTreeLines;
  fNQwTreeLines = 0;
}

/**
 * Create a new QwTreeLine
 */
QwTrackingTreeLine* QwPartialTrack::CreateNewTreeLine()
{
  TClonesArray &treelines = *fQwTreeLines;
  QwTrackingTreeLine *treeline = new (treelines[fNQwTreeLines++]) QwTrackingTreeLine();
  return treeline;
};

// Add an existing QwTreeLine
void QwPartialTrack::AddTreeLine(QwTrackingTreeLine* treeline)
{
  QwTrackingTreeLine* newtreeline = CreateNewTreeLine();
  *newtreeline = *treeline;
  fQwTreeLines2.push_back(treeline);
};

// Clear the local TClonesArray of tree lines
void QwPartialTrack::ClearTreeLines(Option_t *option)
{
  fQwTreeLines->Clear(option); // Clear the local TClonesArray
  fNQwTreeLines = 0; // No tree lines in local TClonesArray
};

// Delete the static TClonesArray of tree lines
void QwPartialTrack::ResetTreeLines(Option_t *option)
{
  delete gQwTreeLines;
  gQwTreeLines = 0;
}

// Print the tree lines
void QwPartialTrack::PrintTreeLines()
{
  TIterator* iterator = fQwTreeLines->MakeIterator();
  QwTrackingTreeLine* treeline = 0;
  while ((treeline = (QwTrackingTreeLine*) iterator->Next()))
    QwVerbose << *treeline << QwLog::endl;
}


/**
 * Print some debugging information
 */
void QwPartialTrack::Print()
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
    stream << "(" << pt.GetRegion() << "/" << "?UD"[pt.GetPackage()] << ") ";
  stream << "(x,y) = (" << pt.fOffsetX << ", " << pt.fOffsetY << "), ";
  stream << "d/dz(x,y) = (" << pt.fSlopeX << ", " << pt.fSlopeY << ")";
  if (pt.fChi > 0.0) { // parttrack has been fitted
    stream << ", chi = " << pt.fChi;
  }
  if (pt.IsVoid()) stream << " (void)";
  return stream;
};


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
 * with the target.
 *
 * \note At this point the target defined at one z value only.  A more careful
 * determination of the primary vertex will be implemented using the general
 * QwVertex class, e.g. QwVertex* primary_vertex = QwVertex (partial_track, beam)
 */
int QwPartialTrack::DeterminePositionInTarget ()
{
  TVector3 primary = GetPosition(0.0);
  QwVerbose << "Target vertex at : (" << primary.X() << "," << primary.Y() << "," << primary.Z() << ")" << QwLog::endl;
  return 0;
}


/**
 * This method checks whether the region 3 partial track has an intersection
 * with the trigger scintillators in a detector package.
 *
 * The trigger scintillators are assumed to be the only (or at least the first)
 * detectors in region 3 that are oriented in direction X.
 *
 * Geometric acceptance is calculated from the centers and widths in the
 * geometry definition.  Tracks are drawn straight from region 3.
 *
 * @return 1 if a hit in the trigger scintillators was found
 */
int QwPartialTrack::DeterminePositionInTriggerScintillators (EQwDetectorPackage package)
{
  double trig[3];
  double lim_trig[2][2];

  // Get the trig scintillator
  Det* rd = rcDETRegion[package][kRegionIDTrig][kDirectionX];

  // Get the point where the track intersects the detector planes
  trig[2] = rd->Zpos;
  trig[0] = fSlopeX * trig[2] + fOffsetX;
  trig[1] = fSlopeY * trig[2] + fOffsetY;
  // Get the detector boundaries
  lim_trig[0][0] = rd->center[0] + rd->width[0]/2;
  lim_trig[0][1] = rd->center[0] - rd->width[0]/2;
  lim_trig[1][0] = rd->center[1] + rd->width[1]/2;
  lim_trig[1][1] = rd->center[1] - rd->width[1]/2;

  if (trig[0] < lim_trig[0][0]
   && trig[0] > lim_trig[0][1]
   && trig[1] < lim_trig[1][0]
   && trig[1] > lim_trig[1][1]) {
    triggerhit = 1;
    trig[0]    = trig[0];
    trig[1]    = trig[1];
    QwVerbose << "Trigger scintillator hit at : (" << trig[0] << "," << trig[1] << "," << trig[2] << ")" << QwLog::endl;
  } else triggerhit = 0;

  return triggerhit;
}

/**
 * This method checks whether the region 3 partial track has an intersection
 * with the trigger scintillators in a detector package.
 *
 * The cerenkov bars are assumed to be the only (or at least the first)
 * detectors in region 3 that are oriented in direction Y.
 *
 * Geometric acceptance is calculated from the centers and widths in the
 * geometry definition.  Tracks are drawn straight from region 3.
 *
 * @return 1 if a hit in the cerenkov bars was found
 */
int QwPartialTrack::DeterminePositionInCerenkovBars (EQwDetectorPackage package)
{
  double cc[3];
  double lim_cc[2][2];

  //QwVerbose<<"r3: x, y, mx, my: "<<x<<", "<<y<<", "<<mx<<", "<<my<<QwLog::endl;

  // Get the Cherenkov detector
  Det* rd = rcDETRegion[package][kRegionIDCer][kDirectionY];

  // Get the point where the track intersects the detector planes
  cc[2] = rd->Zpos;
  cc[0] = fSlopeX * cc[2] + fOffsetX;
  cc[1] = fSlopeY * cc[2] + fOffsetY;
  // Get the detector boundaries
  lim_cc[0][0] = rd->center[0] + rd->width[0]/2;
  lim_cc[0][1] = rd->center[0] - rd->width[0]/2;
  lim_cc[1][0] = rd->center[1] + rd->width[1]/2;
  lim_cc[1][1] = rd->center[1] - rd->width[1]/2;

  if (cc[0] < lim_cc[0][0]
   && cc[0] > lim_cc[0][1]
   && cc[1] < lim_cc[1][0]
   && cc[1] > lim_cc[1][1]) {

    fIsGood = true;
    cerenkovhit = 1;

    cerenkov[0] = cc[0];
    cerenkov[1] = cc[1];
    cerenkov[2] = cc[2];

    pR3hit[0] = cerenkov[0];
    pR3hit[1] = cerenkov[1];
    pR3hit[2] = cerenkov[2];

    double kz = sqrt(fSlopeX * fSlopeX + fSlopeY * fSlopeY + 1);
    uvR3hit[0] = fSlopeX / kz;
    uvR3hit[1] = fSlopeY / kz;
    uvR3hit[2] = 1 / kz;

    QwVerbose << "Cerenkov bar hit at : (" << cc[0] << "," << cc[1] << "," << cc[2] << ")   "
              << "direction ("<<uvR3hit[0]<<","<<uvR3hit[1]<<","<<uvR3hit[2] << QwLog::endl;
  } else {
    cerenkovhit = 0;
    fIsGood = false;
  }

  return cerenkovhit;
}

int QwPartialTrack::DeterminePositionInHDC (EQwDetectorPackage package)
{
  double lim_hdc[2][2];

  //QwVerbose<<"r2: x, y, mx, my: "<<x<<", "<<y<<", "<<mx<<", "<<my<<QwLog::endl;

  // Get the HDC detector
  Det* rd = rcDETRegion[package][kRegionID2][kDirectionX];
  // Get the point where the track intersects the detector planes
  TVector3 hdc_front = GetPosition(rd->Zpos);

  // Get the HDC detector
  rd = rcDETRegion[package][kRegionID2][kDirectionV];
  // Get the point where the track intersects the detector planes
  TVector3 hdc_back = GetPosition(rd->Zpos);

  // Get the detector boundaries
  lim_hdc[0][0] = rd->center[0] + rd->width[0]/2;
  lim_hdc[0][1] = rd->center[0] - rd->width[0]/2;
  lim_hdc[1][0] = rd->center[1] + rd->width[1]/2;
  lim_hdc[1][1] = rd->center[1] - rd->width[1]/2;

  if (hdc_front.X() < lim_hdc[0][0]
   && hdc_front.X() > lim_hdc[0][1]
   && hdc_front.Y() < lim_hdc[1][0]
   && hdc_front.Y() > lim_hdc[1][1]
   && hdc_back.X() < lim_hdc[0][0]
   && hdc_back.X() > lim_hdc[0][1]
   && hdc_back.Y() < lim_hdc[1][0]
   && hdc_back.Y() > lim_hdc[1][1]) {

    fIsGood = true;

    pR2hit[0] = hdc_back.X();
    pR2hit[1] = hdc_back.Y();
    pR2hit[2] = hdc_back.Z();

    QwVerbose << "HDC front hit at : ("
              << hdc_front.X() << "," << hdc_front.Y() << "," << hdc_front.Z() << ")" << QwLog::endl;
    QwVerbose << "HDC back  hit at : ("
              << hdc_back.X() << "," << hdc_back.Y() << "," << hdc_back.Z() << ")" << QwLog::endl;

    TVector3 partial_track = GetMomentumDirection();
    QwVerbose << "Partial track direction vector: ("
              << partial_track.X() << "," << partial_track.Y() << "," << partial_track.Z() << ")" << QwLog::endl;

    QwVerbose << "Partial track direction angle: "
              << "theta = " << GetMomentumDirectionTheta() * Qw::deg << " deg,"
              << "phi = " << GetMomentumDirectionPhi() * Qw::deg << " deg" << QwLog::endl;


  } else {
    fIsGood = false;
  }

  return 0;
}
