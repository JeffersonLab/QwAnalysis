//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
// C++ Interface: QwTrackingTreeLine
//
// Description:
//
//
// Author: Wouter Deconinck <wdconinc@mit.edu>, (C) 2008
//
//         Jie Pan <jpan@jlab.org>, Sat May 23 23:07:17 CDT 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/*! \class QwTrackingTreeLine

    \file QwTrackingTreeLine.cc

    \author Wouter Deconinck <wdconinc@mit.edu>
    \author jpan <jpan@jlab.org>

    \date Sun May 24 11:05:29 CDT 2009

    \brief A container for track information

    The QwTrackingTreeLine has a pointer to a set of hits.
    It is passed to various track fitting procedures and
    carries around the fit results.
    This track is used for found tracks.
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwTrackingTreeLine.h"

#include <cmath>

ClassImp(QwTrackingTreeLine);

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrackingTreeLine::QwTrackingTreeLine(int _a_beg, int _a_end, int _b_beg, int _b_end)
{
  isvoid = false;	// treeline is not void yet
  isused = false;	// treeline is not part of a partial track yet

  next = 0;		// no next element yet in linked-list

  cx = mx = chi = 0.0;	//

  for (int i = 0; i < 3; i++)
    cov_cxmx[i] = 0.0;

  a_beg = _a_beg;
  a_end = _a_end;
  b_beg = _b_beg;
  b_end = _b_end;

  numhits = 0;
  nummiss = 0;

  for (int i = 0; i < 2 * TLAYERS; i++) {
    hits[i] = 0;	/*!< hitarray */
    hasharray[i] = 0;
  }

  ID = 0;
  r3offset = firstwire = lastwire = 0;


}



/**
 * Delete the tree line and the lists of hits depending on it
 */
QwTrackingTreeLine::~QwTrackingTreeLine()
{
  // Delete the hits in this treeline
  for (int i = 0; i < 2*TLAYERS; i++) {
    if (hits[i]) delete hits[i];
  }
}


/**
 * Determine the chi^2 for a tree line, weighted by the number of hits
 * @param treeline Tree line
 * @return Weighted chi^2
 */
double QwTrackingTreeLine::GetChiWeight ()
{
  double weight;
  // NOTE Added +1 to get this to work if numhits == nummiss (region 2 cosmics)
  if (numhits >= nummiss)
    weight = (double) (numhits + nummiss + 1)
                    / (numhits - nummiss + 1);
  else {
    std::cerr << "miss = " << nummiss << ", hit = " << numhits << std::endl;
    return 100000.0; // This is bad...
  }
  return weight * chi;
}

/**
 * Determine the hit with the smallest drift distance, i.e. a first order
 * estimate of the crossing of the track with the central wire plane.
 * @param offset Optional offset to the position
 * @return Hit with smallest drift distance
 */
QwHit* QwTrackingTreeLine::bestWireHit (double offset)
{
  double best_position = 9999.9;
  int best_hit = 0;
  // Get the best measured hit in the back
  for (int hit = 0; hit < numhits; hit++) {
    double position = fabs(hits[hit]->GetDriftDistance() - offset);
    if (position < best_position) {
      best_position = position;
      best_hit = hit;
    }
  }
  return hits[best_hit];
}


/*!--------------------------------------------------------------------------*\

 \brief Calculate average residual of this partial track over all treelines

*//*-------------------------------------------------------------------------*/
const double QwTrackingTreeLine::CalculateAverageResidual()
{
  int numHits = 0;
  double sumResiduals = 0.0;
  for (int layer = 0; layer < 2 * TLAYERS; layer++) {
    for (QwHit* hit = hits[layer]; hit; hit = hit->next) {
      if (hit->fIsUsed) {
        numHits++;
        sumResiduals += hit->GetDriftDistance();
      }
    } // end of loop over hits (only one of them is used)
  } // end of loop over layers
  return sumResiduals / numHits;
}



/**
 * Print the tree line in a linked list
 */
void QwTrackingTreeLine::Print() {
  if (!this) return;
  std::cout << *this << std::endl;
  next->Print();
}


/**
 * Print the valid tree lines in a linked list
 */
void QwTrackingTreeLine::PrintValid() {
  if (!this) return;
  if (this->IsValid()) std::cout << *this << std::endl;
  next->PrintValid();
}


/**
 * Stream some info about the tree line
 * @param stream Stream as lhs of the operator
 * @param tl Tree line as rhs of the operator
 * @return Stream as result of the operator
 */
ostream& operator<< (ostream& stream, const QwTrackingTreeLine& tl) {
  stream << "tl: ";
  stream << tl.a_beg << ", " << tl.a_end << " -- ";
  stream << tl.b_beg << ", " << tl.b_end;
  if (tl.chi > 0.0) { // treeline has been fitted
    stream << "; cx = " << tl.cx;
    stream << ", mx = " << tl.mx;
    stream << ", chi = " << tl.chi;
    stream << ", hits:";
    for (int hit = 0; hit < tl.numhits; hit++)
      stream << " " << tl.usedhits[hit]->GetPlane() << "." << tl.usedhits[hit]->GetElement();
  }
  if (tl.isvoid) stream << " (void)";
  return stream;
};
