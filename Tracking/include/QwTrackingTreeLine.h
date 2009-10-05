/**
 * \class	QwTrackingTreeLine	QwTrackingTreeLine.h
 *
 * \brief	A container for track information
 *
 * \author	Wouter Deconinck <wdconinc@mit.edu>
 * \author	Jie Pan <jpan@jlab.org>
 * \date	Sun May 24 11:05:29 CDT 2009
 * \ingroup	QwTracking
 *
 * The QwTrackingTreeLine has a pointer to a set of hits.  It is passed to
 * various track fitting procedures and carries around the fit results.
 *
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

    //! Is this tree line void?
    bool IsVoid() { return isvoid; };
    //! Is this tree line valid?
    bool IsValid() { return ! isvoid; };
    //! Is this tree line not void?
    bool IsNotVoid() { return ! isvoid; };
    //! Is this tree line used?
    bool IsUsed() { return isused; };
    //! Is this tree line not used?
    bool IsNotUsed() { return ! isused; };

    //! Returns the weighted chi^2
    double GetChiWeight ();

    //! Returns the hit with the best
    QwHit* bestWireHit (double offset = 0.0);

    void Print();
    void PrintValid();

    friend ostream& operator<< (ostream& stream, const QwTrackingTreeLine& tl);

    //! Returns position at the first detector plane
    double GetPositionFirst (double binwidth) {
      return 0.5 * (a_beg + a_end) * binwidth;
    };

    //! Returns position at the last detector plane
    double GetPositionLast (double binwidth) {
      return 0.5 * (b_beg + b_end) * binwidth;
    };

    //! Returns resolution at the first detector plane
    double GetResolutionFirst (double binwidth) {
      return (a_beg - a_end) * binwidth;
    };

    //! Returns resolution at the last detector plane
    double GetResolutionLast (double binwidth) {
      return (b_beg - b_end) * binwidth;
    };

    //! Get the average residuals
    const double GetAverageResidual() const { return fAverageResidual; };
    //! Set the average residuals
    void SetAverageResidual(const double residual) { fAverageResidual = residual; };
    //! Calculate the average residuals
    const double CalculateAverageResidual();



    bool isvoid;			///< has been found void
    bool isused;			///< used (part of parttrack)

    double cx, mx, chi;			///< line parameters...
    double cov_cxmx[3];			///< errors in these

    int a_beg, a_end;			///< bin in first layer
    int b_beg, b_end;			///< bin in last layer

    int   numhits;			///< number of hits on this treeline
    int   nummiss;			///< number of planes without hits

    QwHit *hits[2*TLAYERS];		///< all hits that satisfy road requirement
    QwHit *usedhits[TLAYERS];		///< hits that correspond to optimal chi^2

    int   hasharray[2*TLAYERS];
    int   ID;				///< adamo ID

    int r3offset;			///< offset of demultiplexed group of 8
    int firstwire, lastwire;		///< first and last wire in group of 8

    QwTrackingTreeLine *next; //!	///< link to next list element

  private:

    double fAverageResidual;	// average residual over all used hits

  ClassDef(QwTrackingTreeLine,1);

}; // class QwTrackingTreeLine

#endif // QWTRACKINGTREELINE_H

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
