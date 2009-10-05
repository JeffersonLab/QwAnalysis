#include "QwPartialTrack.h"
ClassImp(QwPartialTrack);


#include "Det.h"
extern Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];

QwPartialTrack::QwPartialTrack()
{
  x = 0.0; y = 0.0; mx = 0.0; my = 0.0;
  isvoid = false; isused = false;
  next = 0;
  for (int i = 0; i < kNumDirections; i++)
    tline[i] = 0;
}

QwPartialTrack::~QwPartialTrack() { }

/**
 * Determine the chi^2 for a partial track, weighted by the number of hits
 *
 * @return Weighted chi^2
 */
double QwPartialTrack::GetChiWeight ()
{
  double weight;
  // Determine the weight if there enough hits
  if (numhits >= nummiss)
    // NOTE Added +1 to get this to work if numhits == nummiss
    weight = (double) (numhits + nummiss + 1)
                    / (numhits - nummiss + 1);
  else {
    std::cerr << "miss = " << nummiss << ", hit = " << numhits << std::endl;
    return 100.0; // This is bad...
  }
  return weight * weight * chi;
  // TODO Why is the weight squared here, but not in the weighted chi^2 for treelines?
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
        sumResiduals += treeline->CalculateAverageResidual();
      }
    } // end of loop over treelines
  } // end of loop over directions
  return sumResiduals / numTreeLines;
}



/**
 * Print some debugging information
 */
void QwPartialTrack::Print()
{
  if (!this) return;
  std::cout << *this << std::endl;
  if (next) next->Print();
}

/**
 * Print some debugging information for valid partial tracks
 */
void QwPartialTrack::PrintValid()
{
  if (!this) return;
  if (this->IsValid()) std::cout << *this << std::endl;
  if (next) next->PrintValid();
}

/**
 * Output stream operator overloading
 */
ostream& operator<< (ostream& stream, const QwPartialTrack& pt) {
  stream << "pt: ";
  stream << "(x,y) = (" << pt.x  << ", " << pt.y  << "), ";
  stream << "d/dz(x,y) = (" << pt.mx << ", " << pt.my << ")";
  if (pt.chi > 0.0) { // parttrack has been fitted
    stream << ", chi = " << pt.chi;
  }
  if (pt.isvoid) stream << " (void)";
  return stream;
};


/**
 * This method checks determines the intersection of the partial track
 * with the target.
 *
 * \note At this point the target defined at one z value only.  A more careful
 * determination of the primary vertex will be implemented using the general
 * QwVertex class, e.g. QwVertex* primary_vertex = QwVertex (partial_track, beam)
 *
 * @return zero
 */
int QwPartialTrack::DeterminePositionInTarget ()
{
  double primary[3];
  primary[2] = 0.0; // z position of target (crude)
  primary[0] = mx * primary[2] + x;
  primary[1] = my * primary[2] + y;
  std::cout << "Target vertex at : (" << primary[0] << "," << primary[1] << "," << primary[2] << ")" << std::endl;
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
  trig[0] = mx * trig[2] + x;
  trig[1] = my * trig[2] + y;
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
    std::cout << "Trigger scintillator hit at : (" << trig[0] << "," << trig[1] << "," << trig[2] << ")" << std::endl;
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

  // Get the Cherenkov detector
  Det* rd = rcDETRegion[package][kRegionIDCer][kDirectionY];

  // Get the point where the track intersects the detector planes
  cc[2] = rd->Zpos;
  cc[0] = mx * cc[2] + x;
  cc[1] = my * cc[2] + y;
  // Get the detector boundaries
  lim_cc[0][0] = rd->center[0] + rd->width[0]/2;
  lim_cc[0][1] = rd->center[0] - rd->width[0]/2;
  lim_cc[1][0] = rd->center[1] + rd->width[1]/2;
  lim_cc[1][1] = rd->center[1] - rd->width[1]/2;

  if (cc[0] < lim_cc[0][0]
   && cc[0] > lim_cc[0][1]
   && cc[1] < lim_cc[1][0]
   && cc[1] > lim_cc[1][1]) {
    cerenkovhit = 1;
    cerenkov[0] = cc[0];
    cerenkov[1] = cc[1];
    std::cout << "Cerenkov bar hit at : (" << cc[0] << "," << cc[1] << "," << cc[2] << ")" << std::endl;
  } else cerenkovhit = 0;

  return cerenkovhit;
}
