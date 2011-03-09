/*!
 * \file   VQwBridgingMethod.h
 * \brief  Definition of the bridging method interface
 *
 * \author Wouter Deconinck
 * \date   2010-01-23
 */

#ifndef VQWBRIDGINGMETHOD_H
#define VQWBRIDGINGMETHOD_H

// System headers
#include <iostream>

// ROOT headers
#include "TVector3.h"

// Qweak headers
#include "QwUnits.h"

// Forward declarations
class QwTrack;
class QwPartialTrack;

/**
 *  \class VQwBridgingMethod
 *  \ingroup QwTracking
 *  \brief Interface to the various bridging methods
 */
class VQwBridgingMethod {

  public:

    /// Default constructor
    VQwBridgingMethod() { };
    /// Destructor
    virtual ~VQwBridgingMethod() { };

    /// \brief Bridge from the front to back partial track (pure virtual)
    virtual int Bridge(const QwPartialTrack* front, const QwPartialTrack* back) = 0;

    /// Clear the list of tracks
    virtual void ClearListOfTracks() { fListOfTracks.clear(); };
    /// Get the list of tracks that was found
    virtual std::vector<QwTrack*> GetListOfTracks() const { return fListOfTracks; };

  protected:

    /// List of tracks that were found in this bridging attempt
    std::vector<QwTrack*> fListOfTracks;


    /// Estimate the momentum based only on the direction
    virtual double EstimateInitialMomentum(const TVector3& direction) const;

}; // class VQwBridgingMethod


/**
 * Estimate the momentum based only on the direction
 *
 * The scattering angle theta of the track is used to calculate the momentum
 * from elastic scattering.  This function assumes a fixed beam energy of
 * 1.165 GeV.
 *
 * @param direction Momentum direction (not necessarily normalized)
 * @return Initial momentum
 */
inline double VQwBridgingMethod::EstimateInitialMomentum(const TVector3& direction) const
{
    double cth = direction.CosTheta(); // cos(theta) = uz/r, where ux,uy,uz form a unit vector
    double wp = 0.93828 * Qw::GeV;     // proton mass [GeV]
    double e0 = 1.165 * Qw::GeV;       // beam energy [GeV]
    double e_loss = 12.0 * Qw::MeV;    // target energy loss ~12 MeV

    // Kinematics for elastic e+p scattering
    return e0 / (1.0 + e0 / wp * (1.0 - cth)) - e_loss;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


#endif // VQWBRIDGINGMETHOD_H
