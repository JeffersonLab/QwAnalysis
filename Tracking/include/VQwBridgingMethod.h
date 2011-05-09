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
#include "QwTrack.h"

// Forward declarations
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
    virtual ~VQwBridgingMethod() {
      ClearListOfTracks();
    };

    /// \brief Bridge from the front to back partial track (pure virtual)
    virtual int Bridge(const QwPartialTrack* front, const QwPartialTrack* back) = 0;

    /// Clear the list of tracks
    virtual void ClearListOfTracks() {
      for (size_t i = 0; i < fListOfTracks.size(); i++)
        delete fListOfTracks.at(i);
      fListOfTracks.clear();
    };
    /// Get the list of tracks that was found
    virtual std::vector<QwTrack*> GetListOfTracks() const { return fListOfTracks; };

  protected:

    /// List of tracks that were found in this bridging attempt
    std::vector<QwTrack*> fListOfTracks;


    /// Estimate the momentum based only on the direction
    virtual double EstimateInitialMomentum(const TVector3& direction) const;

     virtual double CalculateVertex(const TVector3& point,const double angle) const;

    /// Calculate Kinetics
    virtual void CalculateKinetics(const double vertex_z,const double angle,const double momentum,double* results);


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


// vertex might be wrong
inline double VQwBridgingMethod::CalculateVertex(const TVector3& point,const double angle) const
{
  double vertex_z=0.0;
  vertex_z=point.Z()-sqrt(point.X()*point.X()+point.Y()*point.Y())/tan(angle);
  return vertex_z;
}

inline void VQwBridgingMethod::CalculateKinetics(const double vertex_z, double angle,const double momentum,double* results){

 
    double length = 0.0;
    if(vertex_z<(-650.0+35.0/2))
        length = ((-650.0+35.0/2)-vertex_z)/cos(angle); //path length in target after scattering
    double momentum_correction = (length/35.0)*48.0;  //assume 48 MeV total energy loss through the full target length
    
    momentum_correction = 0.0; // 32.0 * Qw::MeV;  // assume 32 MeV with multi-scattering, etc.
    
    double PP = momentum + momentum_correction;

    double Mp = 938.272013;    // Mass of the Proton in MeV
    
    double P0 = Mp*PP/(Mp-PP*(1-cos(angle))); //pre-scattering energy
    double Q2 = 2.0*Mp*(P0-PP);

    results[0]=PP;
    results[1]=P0;
    results[2]=Q2;
    
    return;

}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


#endif // VQWBRIDGINGMETHOD_H
