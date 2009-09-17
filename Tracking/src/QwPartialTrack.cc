#include "QwPartialTrack.h"

ClassImp(QwPartialTrack)

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
 * @param pt Partial track
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



void QwPartialTrack::Print() {
  if (!this) return;
  std::cout << *this << std::endl;
  if (next) next->Print();
}

void QwPartialTrack::PrintValid() {
  if (!this) return;
  if (this->IsValid()) std::cout << *this << std::endl;
  if (next) next->PrintValid();
}


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

