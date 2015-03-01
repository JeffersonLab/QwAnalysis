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
    void AddPartialTrackList(const std::vector<QwPartialTrack*> &partialtracklist);
    void ClearPartialTracks(Option_t *option = "");
    void ResetPartialTracks(Option_t *option = "");
    ///@}
    // Get the number of partial tracks
    Int_t GetNumberOfPartialTracks() const { return fNQwPartialTracks; };
    //! \brief Get the specified partial track
    const QwPartialTrack* GetPartialTrack(const int pt) const { return fQwPartialTracks.at(pt); };
 
    // Print the list of partial tracks
    void PrintPartialTracks(Option_t *option = "") const;


    /// Set magnetic field integral
    void SetMagneticFieldIntegral(double bdl, double bdlx, double bdly, double bdlz) {
      fBdl = bdl;
      fBdlXYZ = TVector3(bdlx,bdly,bdlz);
    }
    /// Get magnetic field integral
    const TVector3& GetMagneticFieldIntegral() const {
      return fBdlXYZ;
    }


    /// Output stream operator for tracks
    friend std::ostream& operator<< (std::ostream& stream, const QwTrack& t);

  public:

    //@{
    /// Quantities determined from front partial track
    double fPhi;                ///< Azimuthal angle phi of track at primary vertex
    double fTheta;              ///< Polar angle theta of track at primary vertex
    double fVertexZ;            ///< Primary vertex position in longitudinal direction
    double fVertexR;	        ///< Primary vertex position in transverse direction
    //@}

    double fChi;                ///< Combined chi, i.e. the sum of the chi for front and back partial track
    double fMomentum;           ///< Spectrometer momentum

    int fIterationsNewton;      ///< Number of iterations in Newton's method
    int fIterationsRungeKutta;  ///< Number of iterations in Runge-Kutta method

    //@{
    /// Matching of front and back track position and direction at matching plane
    TVector3 fPositionDiff;     ///< Difference in position at matching plane
    double fPositionXoff;       ///< Difference in X position at matching plane
    double fPositionYoff;       ///< Difference in Y position at matching plane
    double fPositionRoff;       ///< Difference in radial position at matching plane
    double fPositionPhioff;     ///< Difference in azimuthal angle phi at matching plane
    double fPositionThetaoff;   ///< Difference in polar angle theta at matching plane
    TVector3 fDirectionDiff;    ///< Difference in momentum at matching plane
    double fDirectionXoff;      ///< Difference in X momentum at matching plane
    double fDirectionYoff;      ///< Difference in Y momentum at matching plane
    double fDirectionZoff;      ///< Difference in Z momentum at matching plane
    double fDirectionPhioff;    ///< Difference in momentum azimuthal angle at matching plane
    double fDirectionThetaoff;  ///< Difference in momentum polar angle at matching plane
    //@}

    //@{
    /// Position and direction before and after swimming
    TVector3 fStartPosition;            ///< Start position of front track
    TVector3 fStartDirection;           ///< Start direction of front track

    TVector3 fEndPositionGoal;          ///< Goal position of back track
    TVector3 fEndDirectionGoal;         ///< Goal direction of back track

    TVector3 fEndPositionActual;        ///< Actual position of track at back plane
    TVector3 fEndDirectionActual;       ///< Actual direction of track at back plane

    int fIterationsRK4;                 ///< Number of iterations using Runge-Kutta 4th order
    TVector3 fEndPositionActualRK4;     ///< Actual position of track at back plane using Runge-Kutta 4th order
    TVector3 fEndDirectionActualRK4;    ///< Actual direction of track at back plane using Runge-Kutta 4th order

    int fIterationsRKF45;               ///< Number of iterations using Runge-Kutta-Fehlberg
    TVector3 fEndPositionActualRKF45;   ///< Actual position of track at back plane using Runge-Kutta-Fehlberg
    TVector3 fEndDirectionActualRKF45;  ///< Actual direction of track at back plane using Runge-Kutta-Fehlberg
    //@}

    //@{
    /// Magnetic field integrals
    double fBdl;        ///< Magnetic field integral along track
    TVector3 fBdlXYZ;   ///< Magnetic field integral along track
    //@}

    QwPartialTrack *fFront;     ///< Front partial track
    QwPartialTrack *fBack;      ///< Back partial track

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(QwTrack,3);
  #endif

}; // class QwTrack


/**
 * Method to print vectors conveniently
 * @param stream Output stream
 * @param v Vector
 * @return Output stream
 */
inline std::ostream& operator<< (std::ostream& stream, const TVector3& v)
{
  stream << "(" << v.X() << "," << v.Y() << "," << v.Z() << ")";
  return stream;
}


#endif // QWTRACK_H
