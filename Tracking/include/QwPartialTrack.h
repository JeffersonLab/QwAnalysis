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
#include "TClonesArray.h"
#include "TVector3.h"

// Qweak headers
#include "VQwTrackingElement.h"
#include "QwObjectCounter.h"
#include "QwTreeLine.h"
#include "QwDetectorInfo.h"
#include "QwGeometry.h"

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

    //! Number of tree lines in this partial track
    Int_t fNQwTreeLines;
    //! List of tree lines in this partial track
    std::vector<QwTreeLine*> fQwTreeLines;

  public: // methods

    /// \brief Default constructor
    QwPartialTrack();
    /// \brief Copy constructor by reference
    QwPartialTrack(const QwPartialTrack& that);
    /// \brief Copy constructor from pointer
    QwPartialTrack(const QwPartialTrack* that);
    /// \brief Constructor with track position and direction
    QwPartialTrack(const TVector3& position, const TVector3& momentum);
    /// Destructor
    virtual ~QwPartialTrack();

    /// Assignment operator
    QwPartialTrack& operator=(const QwPartialTrack& that);

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

    //! \name Creating, adding, and getting tree lines
    // @{
    //! Create a new tree line
    QwTreeLine* CreateNewTreeLine();
    //! Add an existing tree line as a copy
    void AddTreeLine(const QwTreeLine* treeline);
    //! \brief Add a linked list of existing tree lines as a copy
    void AddTreeLineList(const QwTreeLine* treelinelist);
    //! \brief Add a standard vector of existing tree lines as a copy
    void AddTreeLineList(const std::vector<QwTreeLine*> &treelinelist);
    //! \brief Clear the list of tree lines
    void ClearTreeLines(Option_t *option = "");
    //! \brief Reset the list of tree lines
    void ResetTreeLines(Option_t *option = "");
    //! \brief Get the number of tree lines
    Int_t GetNumberOfTreeLines() const { return fNQwTreeLines; };
    //! \brief Get the list of tree lines
    const std::vector<QwTreeLine*>& GetListOfTreeLines() const { return fQwTreeLines; };
    //! \brief Get the specified tree line
    const QwTreeLine* GetTreeLine(const int tl) const { return fQwTreeLines.at(tl); };
    //! \brief Print the list of tree lines
    void PrintTreeLines(Option_t *option = "") const;
    // @}

    // Get the weighted chi squared
    double GetChiWeight () const;

    void Print(const Option_t* options = 0) const;
    void PrintValid();
    friend std::ostream& operator<< (std::ostream& stream, const QwPartialTrack& pt);

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


    /// \brief Get the vertex position
    Double_t GetVertexZ() const {
      return
         -(fSlopeX * fOffsetX + fSlopeY * fOffsetY) /
          (fSlopeX * fSlopeX  + fSlopeY * fSlopeY);
    }

    /// \brief Smear the position
    QwPartialTrack& SmearPosition(const double sigma_x, const double sigma_y);
    /// \brief Smear the theta angle
    QwPartialTrack& SmearAngleTheta(const double sigma);
    /// \brief Smear the phi angle
    QwPartialTrack& SmearAnglePhi(const double sigma);

    /// \brief Determine vertex in detector
    const QwVertex* DeterminePositionInDetector(const QwDetectorInfo* geometry);
    /// \brief Determine vertex in the target
    const QwVertex* DeterminePositionInTarget (const QwGeometry& geometry);
    /// \brief Determine intersection with trigger scintillators
    const QwVertex* DeterminePositionInTriggerScintillators (const QwGeometry& geometry);
    /// \brief Determine intersection with cerenkov bars
    const QwVertex* DeterminePositionInCerenkovBars (const QwGeometry& geometry);
    /// \brief Determine position in first horizontal drift chamber
    const QwVertex* DeterminePositionInHDC (const QwGeometry& geometry);

    // Average residuals
    double GetAverageResidual() const { return fAverageResidual; };
    void SetAverageResidual(const double residual) { fAverageResidual = residual; }
    double CalculateAverageResidual();
    void SetAverageResidual() { fAverageResidual = CalculateAverageResidual(); }

    // Only 2 Treelines in Plane 0
    void SetAlone(const int alone) { fAlone = alone; }
    //! Only 2 Treelines in Plane 0
    int GetAlone() const { return fAlone; }
   
    //! Rotate coordinates to right octant
    void RotateCoordinates();

    //Rotate coordinates to account for any rotator pitch, yaw, or roll
    void RotateRotator(const QwDetectorInfo* geometry);

  public: // members

    Double_t fOffsetX;		///< x coordinate (at MAGNET_CENTER)
    Double_t fOffsetY;		///< y coordinate (at MAGNET_CENTER)
    Double_t fSlopeX;		///< x slope
    Double_t fSlopeY;		///< y slope

    // Results of the fit to the hits
    Double_t fChi;		///< combined chi square
    Double_t fCov[4][4];	///< covariance matrix

    // record the slope and offset from each treeline,modified 4/26/11
    Double_t TResidual[kNumDirections];                 // record the residual in u v and x treelines(designed for R2)
    Double_t fSignedResidual[12];                       // record the signed residual in every plane(track point - hit point) (designed for R2)

    Bool_t fIsUsed;		///< used (part of a track)
    Bool_t fIsVoid;		///< marked as being void
    Bool_t fIsGood;

    Int_t  fNumMiss;		///< missing hits
    Int_t  fNumHits;		///< used hits

    double pR2hit[3];           ///< x-y-z position at R2
    double uvR2hit[3];          ///< direction at R2

    double pR3hit[3];           ///< x-y-z position at R3
    double uvR3hit[3];          ///< direction at R3

    int fAlone;                /// number of Plane 0 Treelines
    

  private:

    double fAverageResidual;	///< average residual over all used treelines

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwPartialTrack,1);
  #endif

}; // class QwPartialTrack

typedef VQwTrackingElementContainer<QwPartialTrack> QwPartialTrackContainer;

#endif // QWPARTIALTRACK_H
