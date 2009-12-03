#ifndef QWPARTIALTRACK_H
#define QWPARTIALTRACK_H

#include <iostream>
#include <math.h>

#include "TObject.h"

#include "globals.h"
#include "QwTrackingTreeLine.h"
#include "QwBridge.h"


/*------------------------------------------------------------------------*//*!

 \brief Contains the straight part of a track in one region only.

 A QwPartialTrack contains tracking information for one region only.
 It is constructed by combining multiple tree lines together, and in turn
 used to construct the complete track.

 \ingroup QwTrackingAnl

*//*-------------------------------------------------------------------------*/
class QwPartialTrack: public TObject {

  public: // methods

    QwPartialTrack();
    ~QwPartialTrack();


    bool IsVoid() { return isvoid; };
    bool IsValid() { return ! isvoid; };
    bool IsGood() { return isgood; };
    bool IsNotValid() { return ! isvoid; };
    bool IsUsed() { return isused; };
    bool IsNotUsed() { return ! isused; };

    double GetChiWeight ();

    void Print();
    void PrintValid();
    friend ostream& operator<< (ostream& stream, const QwPartialTrack& pt);


    /// Determine vertex in the target
    int DeterminePositionInTarget ();
    /// Determine intersection with trigger scintillators
    int DeterminePositionInTriggerScintillators (EQwDetectorPackage package);
    /// Determine intersection with cerenkov bars
    int DeterminePositionInCerenkovBars (EQwDetectorPackage package);

    int DetermineHitInHDC (EQwDetectorPackage package);

    // Average residuals
    const double GetAverageResidual() const { return fAverageResidual; };
    void SetAverageResidual(const double residual) { fAverageResidual = residual; };
    const double CalculateAverageResidual();
    void SetAverageResidual() { fAverageResidual = CalculateAverageResidual(); };

    // Tree lines
    QwTrackingTreeLine* GetTreeLine(const EQwDirectionID dir) {
      return tline[dir];
    };

  public: // members

    Double_t x;			///< x coordinate (at MAGNET_CENTER)
    Double_t y;			///< y coordinate (at MAGNET_CENTER)
    Double_t mx;		///< x slope
    Double_t my;		///< y slope
    Double_t chi;		///< combined chi square
    double Cov_Xv[4][4];	///< covariance matrix
    QwTrackingTreeLine *tline[kNumDirections];		///< tree line in u v and x
    double  clProb;		///< prob. that this cluster belongs to track
    double  pathlenoff;		///< pathlength offset
    double  pathlenslo;		///< pathlength slope
    QwBridge *bridge;		///< magnetic field bridging
    bool isused;		///< used (part of a track)
    Int_t  nummiss;		///< missing hits
    Int_t  numhits;		///< used hits
    Bool_t isvoid;		///< marked as being void

    int triggerhit;		///< Did this track pass through the trigger?
    double trig[3];		///< x-y-z position at trigger face

    int cerenkovhit;		///< Did this track pass through the cerenkov bar?
    double cerenkov[3];		///< x-y-z position at Cerenkov bar face

    bool isgood;

    double pR2hit[3];           ///< x-y-z position at R2
    double uvR2hit[3];          ///< direction at R2

    double pR3hit[3];           ///< x-y-z position at R3
    double uvR3hit[3];          ///< direction at R3

    QwPartialTrack *next; //!	///< linked list (not saved)

  private:

    double fAverageResidual;	///< average residual over all used treelines

  ClassDef(QwPartialTrack,1);

}; // class QwPartialTrack

#endif // QWPARTIALTRACK_H
