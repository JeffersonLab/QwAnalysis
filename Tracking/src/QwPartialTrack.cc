#include "QwPartialTrack.h"

/*!--------------------------------------------------------------------------*\

 \brief Calculate average residual of this partial track over all treelines

*//*-------------------------------------------------------------------------*/
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



ostream& operator<< (ostream& stream, const QwPartialTrack& pt) {
  stream << "pt: ";
  stream << "(x,y) = (" << pt.x  << ", " << pt.y  << "), ";
  stream << "d/dz(x,y) = (" << pt.mx << ", " << pt.my << ") ";
  if (pt.isvoid) stream << " (void)";
  if (pt.next) stream << " -> " << pt.next;
  return stream;
};

