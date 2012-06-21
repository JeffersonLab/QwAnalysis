/*!
 * \file   QwTrack.h
 * \brief  Definition of the track class
 *
 * \author Wouter Deconinck
 * \date   2009-12-11
 */

#ifndef QWTRACK_H
#define QWTRACK_H

// Qweak headers
#include "VQwTrackingElement.h"
#include "QwPartialTrack.h"
#include "QwBridge.h"
#include "QwVertex.h"

/**
 * \class QwTrack
 * \ingroup QwTracking
 * \brief Contains the complete track as a concatenation of partial tracks
 *
 * A QwTrack contains the complete description of the track as a concatenation
 * of multiple partial tracks.  Associated with the track is the kinematical
 * information, for use in the final Q^2 determination.
 *
 * Several vectors of QwPartialTracks are stored in the QwTrack object.  This
 * allows for combining different QwPartialTracks with each other, and selecting
 * the optimal fit.
 */
class QwTrack: public VQwTrackingElement, public QwObjectCounter<QwTrack> {

  private:

    //! Number of partial tracks in this track
    Int_t fNQwPartialTracks;
    //! List of partial tracks in this track
    std::vector<QwPartialTrack*> fQwPartialTracks;

  public:

    /// Default constructor
    QwTrack();
    /// Constructor with front and back partial track
    QwTrack(const QwPartialTrack* front, const QwPartialTrack* back);
    /// Copy constructor by reference
    QwTrack(const QwTrack& that);
    /// Copy constructor from pointer
    QwTrack(const QwTrack* that);
    /// Virtual destructor
    virtual ~QwTrack();

    /// Assignment operator
    QwTrack& operator=(const QwTrack& that);

    /// Initialization
    void Initialize();

    ///@{
    /// Creating and adding partial tracks
    QwPartialTrack* CreateNewPartialTrack();
    void AddPartialTrack(const QwPartialTrack* partialtrack);
    void AddPartialTrackList(const QwPartialTrack* partialtracklist);
    void AddPartialTrackList(const std::vector<QwPartialTrack*> &partialtracklist);
    void ClearPartialTracks(Option_t *option = "");
    void ResetPartialTracks(Option_t *option = "");
    ///@}
    // Get the number of partial tracks
    Int_t GetNumberOfPartialTracks() const { return fNQwPartialTracks; };
    // Print the list of partial tracks
    void PrintPartialTracks(Option_t *option = "") const;

    /// Output stream operator for tracks
    friend ostream& operator<< (ostream& stream, const QwTrack& t);

  public:

    //@{
    /// Quantities determined from front partial track
    double fPhi;                ///< Azimuthal angle phi of track at primary vertex
    double fTheta;              ///< Polar angle theta of track at primary vertex
    double fVertexZ;            ///< Primary vertex position in longitudinal direction
    double fVertexR;            ///< Primary vertex position in transverse direction
    //@}

    double fChi;                ///< Combined chi square
    double fMomentum;           ///< Spectrometer momentum
    double fTotalEnergy;        ///< Total energy of the track
    double fScatteringAngle;    ///< Scattering angle

    double fPositionRoff,fPositionPhioff;    
    double fDirectionThetaoff,fDirectionPhioff;

    double fXBj;                ///< Bjorken-x scaling variable \f$ x = Q^2 / 2 M \nu \f$
    double fQ2;                 ///< Four-momentum transfer squared \f$ Q^2 = - q \cdot q \f$
    double fW2;                 ///< Invariant mass squared of the recoiling target system
    double fNu;                 ///< Energy loss of the electron \f$ \nu = E - E' = q \cdot p / M \f$
    double fY;                  ///< Fractional energy loss \f$ y = \nu / E \f$

    QwBridge *fBridge;          ///< Magnet front-back matching information

    QwPartialTrack *fFront;     ///< Front partial track
    QwPartialTrack *fBack;      ///< Back partial track

  ClassDef(QwTrack,2);

}; // class QwTrack

#endif // QWTRACK_H
