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
  double wp = 938.272013 * Qw::MeV;     // proton mass [MeV]
  double e0 = 1160.385 * Qw::MeV ;      // beam energy [MeV]
  // average value from the talk of David. A 
  // https://qweak.jlab.org/DocDB/0014/001429/001/Tracking_June2011.pdf
  double e_loss = 12.0  * Qw::MeV;    // target energy loss ~12 MeV

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

inline void VQwBridgingMethod::CalculateKinetics(const double vertex_z, double angle,const double momentum,double* results)
{

  Double_t cos_theta = 0.0;
  cos_theta = cos(angle);
  Double_t target_z_length_mm = 350.0; // Target Length 
  Double_t target_z_position_mm = -6500.0;
  Double_t target_z_space_mm[2] = {0.0};

  target_z_space_mm[0] = target_z_position_mm - 0.5*target_z_length_mm;
  target_z_space_mm[1] = target_z_position_mm + 0.5*target_z_length_mm;

  Double_t momentum_correction_MeV = 0.0;
  Double_t path_length_mm = 0.0;

  if (vertex_z < target_z_space_mm[1]) {
    path_length_mm = (target_z_space_mm[1] - vertex_z)/cos_theta; // path length in target after scattering
  }
  momentum_correction_MeV = 48.0*(path_length_mm/target_z_length_mm);
  // where the following assumption comes from,
  // Wednesday, June  8 09:43:05 EDT 2011, jhlee
  //
  //assume 48 MeV total energy loss through the full target length
    
  momentum_correction_MeV = 0.0; // temp. turn off the momentum correction.

  double Mp = 938.272013;    // Mass of the Proton in MeV

  double PP = momentum + momentum_correction_MeV;
  double P0 = Mp*PP/(Mp-PP*(1-cos_theta)); //pre-scattering energy
  double Q2 = 2.0*Mp*(P0-PP);

  results[0]=PP;
  results[1]=P0;
  results[2]=Q2;
    
  return;

}

#endif // VQWBRIDGINGMETHOD_H
