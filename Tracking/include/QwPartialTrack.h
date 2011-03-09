/*!
 * \file   QwPartialTrack.h
 * \brief  Definition of the partial track class
 *
 * \author Wouter Deconinck
 * \date   2009-12-11
 */

#ifndef QWPARTIALTRACK_H
#define QWPARTIALTRACK_H

#include <iostream>
#include <math.h>

// ROOT headers
#include "TObject.h"
#include "TClonesArray.h"
#include "TVector3.h"

// Qweak headers
#include "VQwTrackingElement.h"
#include "QwObjectCounter.h"
#include "QwTrackingTreeLine.h"
#include "QwDetectorInfo.h"
#include "QwBridge.h"

// Forward declarations
class QwVertex;

/**
 * \class QwPartialTrack
 * \ingroup QwTracking
 * \brief Contains the straight part of a track in one region only.
 *
 * A QwPartialTrack contains tracking information for one region only.
 * It is constructed by combining multiple tree lines together, and in turn
 * used to construct the complete track.
 */
class QwPartialTrack: public VQwTrackingElement, public QwObjectCounter<QwPartialTrack> {

  private:

    // Tree lines
    #define QWPARTIALTRACK_MAX_NUM_TREELINES 1000
    TClonesArray        *fQwTreeLines; //! ///< Array of QwTreeLines
    static TClonesArray *gQwTreeLines; //! ///< Static array of QwTreeLines

    //! Number of tree lines in this partial track
    Int_t fNQwTreeLines;
    //! List of tree lines in this partial track
    //std::vector<QwTrackingTreeLine*> fQwTreeLines;

  public: // methods

    /// \brief Default constructor
    QwPartialTrack();
    /// \brief Copy constructor
    QwPartialTrack(const QwPartialTrack* partialtrack);
    /// \brief Constructor with track position and direction
    QwPartialTrack(const TVector3 position, const TVector3 momentum);
    /// Destructor
    virtual ~QwPartialTrack() { };

  private:

    /// Initialization
    void Initialize();

  public:

    // Valid and used flags
    bool IsVoid() const { return fIsVoid; };
    bool IsValid() const { return ! fIsVoid; };
    bool IsGood() const { return fIsGood; };
    bool IsNotValid() const { return ! fIsVoid; };
    bool IsUsed() const { return fIsUsed; };
    bool IsNotUsed() const { return ! fIsUsed; };

    // Housekeeping methods for lists
    void Clear(Option_t *option = "");
    void Reset(Option_t *option = "");

    // Creating and adding tree lines
    void InitializeTreeLines();
    QwTrackingTreeLine* CreateNewTreeLine();
    void AddTreeLine(QwTrackingTreeLine* treeline);
    void ClearTreeLines(Option_t *option = "");
    void ResetTreeLines(Option_t *option = "");
    // Get the number of partial tracks
    Int_t GetNumberOfTreeLines() const { return fNQwTreeLines; };
    // Print the list of tree lines
    void PrintTreeLines(Option_t *option = "") const;

    // Get the weighted chi squared
    double GetChiWeight () const;

    void Print(const Option_t* options = 0) const;
    void PrintValid();
    friend ostream& operator<< (ostream& stream, const QwPartialTrack& pt);

    /// \brief Return the vertex at position z
    const TVector3 GetPosition(const double z) const;
    /// \brief Return the direction
    const TVector3 GetMomentumDirection() const;
    /// \brief Return the phi angle
    Double_t GetMomentumDirectionPhi() const {
      return GetMomentumDirection().Phi();
    };
    /// \brief Return the theta angle
    Double_t GetMomentumDirectionTheta() const {
      return GetMomentumDirection().Theta();
    };

    /// \brief Smear the position
    QwPartialTrack& SmearPosition(const double sigma_x, const double sigma_y);
    /// \brief Smear the theta angle
    QwPartialTrack& SmearAngleTheta(const double sigma);
    /// \brief Smear the phi angle
    QwPartialTrack& SmearAnglePhi(const double sigma);

    /// \brief Determine vertex in detector
    const QwVertex* DeterminePositionInDetector(const QwDetectorInfo& detector);
    /// \brief Determine vertex in the target
    int DeterminePositionInTarget ();
    /// \brief Determine intersection with trigger scintillators
    int DeterminePositionInTriggerScintillators (EQwDetectorPackage package);
    /// \brief Determine intersection with cerenkov bars
    int DeterminePositionInCerenkovBars (EQwDetectorPackage package);
    /// \brief Determine position in first horizontal drift chamber
    int DeterminePositionInHDC (EQwDetectorPackage package);

    // Average residuals
    double GetAverageResidual() const { return fAverageResidual; };
    void SetAverageResidual(const double residual) { fAverageResidual = residual; }
    double CalculateAverageResidual();
    void SetAverageResidual() { fAverageResidual = CalculateAverageResidual(); }

    // Tree lines
    QwTrackingTreeLine* GetTreeLine(const EQwDirectionID dir) {
      return tline[dir];
    }

  public: // members

    Double_t fOffsetX;		///< x coordinate (at MAGNET_CENTER)
    Double_t fOffsetY;		///< y coordinate (at MAGNET_CENTER)
    Double_t fSlopeX;		///< x slope
    Double_t fSlopeY;		///< y slope

    // Results of the fit to the hits
    Double_t fChi;		///< combined chi square
    double fCov[4][4];		///< covariance matrix

    QwTrackingTreeLine *tline[kNumDirections];	//!	///< tree line in u v and x
    double  clProb;		///< prob. that this cluster belongs to track
    double  pathlenoff;		///< pathlength offset
    double  pathlenslo;		///< pathlength slope
    QwBridge *bridge;	//!	///< magnetic field bridging
    bool fIsUsed;		///< used (part of a track)
    Int_t  nummiss;		///< missing hits
    Int_t  numhits;		///< used hits
    Bool_t fIsVoid;		///< marked as being void

    int triggerhit;		///< Did this track pass through the trigger?
    double trig[3];		///< x-y-z position at trigger face

    int cerenkovhit;		///< Did this track pass through the cerenkov bar?
    double cerenkov[3];		///< x-y-z position at Cerenkov bar face

    bool fIsGood;

    double pR2hit[3];           ///< x-y-z position at R2
    double uvR2hit[3];          ///< direction at R2

    double pR3hit[3];           ///< x-y-z position at R3
    double uvR3hit[3];          ///< direction at R3

    QwPartialTrack *next; //!	///< linked list (not saved)

  private:

    double fAverageResidual;	///< average residual over all used treelines

  ClassDef(QwPartialTrack,1);

}; // class QwPartialTrack

typedef VQwTrackingElementContainer<QwPartialTrack> QwPartialTrackContainer;

#endif // QWPARTIALTRACK_H
