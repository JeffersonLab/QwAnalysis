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

    \file QwTrackingTreeLine.h

    \author Wouter Deconinck <wdconinc@mit.edu>
    \author jpan <jpan@jlab.org>

    $Date: Sun May 24 11:05:29 CDT 2009 $

    \brief A container for track information

    The QwTrackingTreeLine has a pointer to a set of hits.
    It is passed to various track fitting procedures and
    carries around the fit results.
    This track is used for found tracks.
 */
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef QWTRACKINGTREELINE_H
#define QWTRACKINGTREELINE_H

#include "TObject.h"

#include "globals.h"
#include "QwHit.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

///
/// \ingroup QwTrackingAnl

class QwTrackingTreeLine: public TObject {

  public:

    QwTrackingTreeLine(int _a_beg = 0, int _a_end = 0 , int _b_beg = 0, int _b_end = 0);
     ~QwTrackingTreeLine();

    bool IsVoid() { return isvoid; };
    bool IsNotVoid() { return ! isvoid; };
    bool IsUsed() { return isused; };
    bool IsNotUsed() { return ! isused; };

    void Print();

    friend ostream& operator<< (ostream& stream, const QwTrackingTreeLine& tl);

    double GetPositionFirst (double binwidth) {
      return 0.5 * (a_beg + a_end) * binwidth;
    }; //! Returns position at the first detector plane

    double GetPositionLast (double binwidth) {
      return 0.5 * (b_beg + b_end) * binwidth;
    }; //! Returns position at the last detector plane

    double GetResolutionFirst (double binwidth) {
      return (a_beg - a_end) * binwidth;
    }; //! Returns resolution at the first detector plane

    double GetResolutionLast (double binwidth) {
      return (b_beg - b_end) * binwidth;
    }; //! Returns resolution at the last detector plane

    // Get/set/calculate the average residuals
    const double GetAverageResidual() const { return fAverageResidual; };
    void SetAverageResidual(const double residual) { fAverageResidual = residual; };
    const double CalculateAverageResidual();



    bool isvoid;		/*!< has been found void */
    bool isused;		/*!< used (part of parttrack) */

    double cx, mx, chi;		/*!< line parameters... */
    double cov_cxmx[3];		/*!< errors in these */

    int a_beg, a_end;		/*!< bin in first layer */
    int b_beg, b_end;		/*!< bin in last layer */

    int   numhits;		/*!< number of hits on this treeline */
    int   nummiss;		/*!< number of planes without hits */

    QwHit *hits[2*TLAYERS];	/*!< hitarray */
    QwHit thehits[2*TLAYERS];

    int   hasharray[2*TLAYERS];
    int   ID;			/*!< adamo ID */
    int   r3offset,firstwire,lastwire;

    QwTrackingTreeLine *next;	/*!< link to next list element */

  private:

    double fAverageResidual;	/*!< average residual over all used hits */

}; // class QwTrackingTreeLine

#endif //Q WTRACKINGTREELINE_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
