#ifndef QWPARTIALTRACK_H
#define QWPARTIALTRACK_H

#include <iostream>
#include <math.h>

// ROOT headers
#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"

// Qweak headers
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

  private:

    // Tree lines
    #define QWPARTIALTRACK_MAX_NUM_TREELINES 1000
    Int_t fNQwTreeLines; ///< Number of QwTreeLines in the array
    TClonesArray        *fQwTreeLines; ///< Array of QwTreeLines
    static TClonesArray *gQwTreeLines; ///< Static array of QwTreeLines


  public: // methods

    QwPartialTrack();
    ~QwPartialTrack();

    // Valid and used flags
    bool IsVoid() { return isvoid; };
    bool IsValid() { return ! isvoid; };
    bool IsGood() { return isgood; };
    bool IsNotValid() { return ! isvoid; };
    bool IsUsed() { return isused; };
    bool IsNotUsed() { return ! isused; };

    // Housekeeping methods for lists
    void Clear(Option_t *option = "");
    void Reset(Option_t *option = "");

    // Creating and adding tree lines
    QwTrackingTreeLine* CreateNewTreeLine();
    void AddTreeLine(QwTrackingTreeLine* treeline);
    void ClearTreeLines(Option_t *option = "");
    void ResetTreeLines(Option_t *option = "");
    // Get the number of partial tracks
    Int_t GetNumberOfTreeLines() const { return fNQwTreeLines; };
    // Print the list of tree lines
    void PrintTreeLines();

    // Get the weighted chi squared
    double GetChiWeight ();

    void Print();
    void PrintValid();
    friend ostream& operator<< (ostream& stream, const QwPartialTrack& pt);

    /// \brief Return the vertex at position z
    TVector3 GetPosition(double z);
    /// \brief Return the direction at position z
    TVector3 GetDirection(double z = 0.0);
    /// \brief Return the phi angle at position z
    Double_t GetDirectionPhi(double z = 0.0) {
      return GetDirection(z).Phi();
    };
    /// \brief Return the theta angle at position z
    Double_t GetDirectionTheta(double z = 0.0) {
      return GetDirection(z).Theta();
    };

    /// \brief Determine vertex in the target
    int DeterminePositionInTarget ();
    /// \brief Determine intersection with trigger scintillators
    int DeterminePositionInTriggerScintillators (EQwDetectorPackage package);
    /// \brief Determine intersection with cerenkov bars
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

    std::vector <QwTrackingTreeLine> fQwTreeLines2;

    Double_t x;			///< x coordinate (at MAGNET_CENTER)
    Double_t y;			///< y coordinate (at MAGNET_CENTER)
    Double_t mx;		///< x slope
    Double_t my;		///< y slope
    Double_t chi;		///< combined chi square
    double Cov_Xv[4][4];	///< covariance matrix
    QwTrackingTreeLine *tline[kNumDirections];	//!	///< tree line in u v and x
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
