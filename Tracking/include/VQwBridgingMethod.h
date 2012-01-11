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
  double e0 = 1157.5 * Qw::MeV ;      // beam energy [MeV]
  // average value from the talk of David. A 
  // https://qweak.jlab.org/DocDB/0014/001429/001/Tracking_June2011.pdf
  double e_loss = 38.0  * Qw::MeV;    // target energy loss ~12 MeV
  double pre_loss = 13.0 * Qw::MeV;
  // Kinematics for elastic e+p scattering
  //return e0 / (1.0 + e0 / wp * (1.0 - cth)) - e_loss;
  e0-=pre_loss;
  return e0/(1.0+e0/wp*(1-cth))-e_loss;
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
  Double_t target_z_length= 35.0; // Target Length in cm
  Double_t target_z_position= -652.0;
  Double_t target_z_space[2] = {0.0};

  target_z_space[0] = target_z_position - 0.5*target_z_length;
  target_z_space[1] = target_z_position + 0.5*target_z_length;

  Double_t momentum_correction_MeV = 0.0;
  Double_t path_length_mm = 0.0;

  
  //if (vertex_z < target_z_space_mm[1]) {
  //  path_length_mm = (target_z_space_mm[1] - vertex_z)/cos_theta; // path length in target after scattering
  //}
  //momentum_correction_MeV = 48.0*(path_length_mm/target_z_length_mm)*Qw::MeV;
  // where the following assumption comes from,
  // Wednesday, June  8 09:43:05 EDT 2011, jhlee
  //
  //assume 48 MeV total energy loss through the full target length
    
  momentum_correction_MeV = 24.0*Qw::MeV; // temp. turn off the momentum correction.

  
  Double_t Mp = 938.272013*Qw::MeV;    // Mass of the Proton in MeV
  
  Double_t pre_loss = 22.0*Qw::MeV;
  if(vertex_z < target_z_space[0])
    pre_loss = 0.0 * Qw::MeV;
  else if(vertex_z < target_z_space[1])
    pre_loss *= (vertex_z - target_z_space[0])/target_z_length;

    
  //double PP = momentum + momentum_correction_MeV;
  //double P0 = Mp*PP/(Mp-PP*(1-cos_theta)); //pre-scattering energy
  //double Q2 = 2.0*Mp*(P0-PP);

  //double P0=1160.385;    // for run I
  Double_t P0=1157.5*Qw::MeV;
  P0-=pre_loss;
  Double_t PP=Mp*P0/(Mp+P0*(1-cos_theta));
  Double_t Q2=2.0*P0*PP*(1-cos_theta);
  results[0]=PP;
  results[1]=P0;
  results[2]=Q2;
    
  return;

}

#endif // VQWBRIDGINGMETHOD_H
