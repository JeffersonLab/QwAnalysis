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
#include "QwParameterFile.h"

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
  VQwBridgingMethod()
  : fBeamEnergy(0.0) {
    // Load beam properties
    LoadBeamProperty("beam_property.map");
  };
  /// Destructor
  virtual ~VQwBridgingMethod() {
    ClearListOfTracks();
  };

  /// Clear the list of tracks
  virtual void ClearListOfTracks() {
    for (size_t i = 0; i < fListOfTracks.size(); i++)
      delete fListOfTracks.at(i);
    fListOfTracks.clear();
  };
  /// Get the list of tracks that was found
  virtual std::vector<QwTrack*> GetListOfTracks() const { return fListOfTracks; };


  /// \brief Load the beam properties from a map file
  void LoadBeamProperty(const TString& map);


  /// \brief Bridge from the front to back partial track (pure virtual)
  virtual const QwTrack* Bridge(const QwPartialTrack* front, const QwPartialTrack* back) = 0;


 protected:

  /// List of tracks that were found in this bridging attempt
  std::vector<QwTrack*> fListOfTracks;

  Double_t fBeamEnergy; ///< Nominal beam energy

  /// Estimate the momentum based only on the direction
  virtual double EstimateInitialMomentum(const TVector3& direction) const;
  virtual double EstimateInitialMomentum(const double vertex_z,double angle,const double energy) const;
  virtual double CalculateVertex(const TVector3& point, const double angle) const;

  /// Calculate Kinetics
  virtual void CalculateKinematics(const double vertex_z,const double angle,const double momentum,double* results);


}; // class VQwBridgingMethod


/**
 * Load the beam properties from a map file.
 * @param map Name of map file
 */
inline void VQwBridgingMethod::LoadBeamProperty(const TString& map)
{
  QwParameterFile mapstr(map.Data());
  while (mapstr.ReadNextLine())
  {
    mapstr.TrimComment();       // Remove everything after a comment character.
    mapstr.TrimWhitespace();    // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())  continue;

    TString varname, varvalue;
    if (mapstr.HasVariablePair("=",varname,varvalue)) {
      //  This is a declaration line.  Decode it.
      varname.ToLower();
      if (varname == "energy") {
        fBeamEnergy = atof(varvalue.Data()) * Qw::MeV;
      }
    }
  }
}


/**
 * Estimate the momentum based only on the direction
 *
 * The scattering angle theta of the track is used to calculate the momentum
 * from elastic scattering.
 *
 * @param direction Momentum direction (not necessarily normalized)
 * @return Initial momentum
 */
inline double VQwBridgingMethod::EstimateInitialMomentum(
    const double vertex_z,
    const double angle,
    const double energy) const
{
  double wp = 938.272013*Qw::MeV;
  // double e_loss=38.0*Qw::MeV;
  double e_loss = 50*Qw::MeV;
  double e0 = energy;
  
  double target_z_length = 34.35; // Target Length (cm)
  double target_z_position = -652.67; // Target center position (cm) in Z
  double target_z_space[2] = {0.0};

  target_z_space[0] = target_z_position - 0.5*target_z_length;
  target_z_space[1] = target_z_position + 0.5*target_z_length;
  double pre_loss = 0.0;
  double depth = vertex_z - target_z_space[0];

  if(vertex_z < target_z_space[0])
    pre_loss = 0.05 * Qw::MeV;  
  else if(vertex_z >= target_z_space[0] && vertex_z <= target_z_space[1])
    //pre_loss *= (vertex_z - target_z_space[0])/target_z_length; // linear approximation
    pre_loss = 0.05 + depth*0.6618 - 0.003462*depth*depth; // quadratic fit approximation
  else
    pre_loss = (18.7 + 3.9)*Qw::MeV;  // averaged total Eloss, downstream, including Al windows
  
  // Kinematics for elastic e+p scattering
  //return e0 / (1.0 + e0 / wp * (1.0 - cth)) - e_loss;
  e0 -= pre_loss;
  double cth = cos(angle);
  return e0 / (1.0 + e0/wp*(1-cth)) - e_loss;
}


inline double VQwBridgingMethod::EstimateInitialMomentum(const TVector3& direction) const
{
  double cth = direction.CosTheta(); // cos(theta) = uz/r, where ux,uy,uz form a unit vector
  double wp = 938.272013 * Qw::MeV;     // proton mass [MeV]
  double e0 = 1157.5 * Qw::MeV ;      // beam energy [MeV]
  // average value from the talk of David. A 
  // https://qweak.jlab.org/DocDB/0014/001429/001/Tracking_June2011.pdf
  double e_loss = 38.0  * Qw::MeV;    // target energy loss ~12 MeV
  double pre_loss = 10.04 * Qw::MeV;
  // Kinematics for elastic e+p scattering
  //return e0 / (1.0 + e0 / wp * (1.0 - cth)) - e_loss;
  e0-=pre_loss;
  return e0/(1.0+e0/wp*(1-cth))-e_loss;
}


// vertex might be wrong
inline double VQwBridgingMethod::CalculateVertex(const TVector3& point, const double angle) const
{
  double vertex_z=0.0;
  vertex_z=point.Z()-sqrt(point.X()*point.X()+point.Y()*point.Y())/tan(angle);
  return vertex_z;
}

inline void VQwBridgingMethod::CalculateKinematics(const double vertex_z, double angle, const double energy,double* results)
{
  Double_t cos_theta = 0.0;
  cos_theta = cos(angle);
  Double_t target_z_length= 34.35; // Target Length (cm)
  Double_t target_z_position= -652.67; // Target center position (cm) in Z
  Double_t target_z_space[2] = {0.0};

  target_z_space[0] = target_z_position - 0.5*target_z_length;
  target_z_space[1] = target_z_position + 0.5*target_z_length;

  //Double_t momentum_correction_MeV = 0.0;
  // Double_t path_length_mm = 0.0;

  
  //if (vertex_z < target_z_space_mm[1]) {
  //  path_length_mm = (target_z_space_mm[1] - vertex_z)/cos_theta; // path length in target after scattering
  //}
  //momentum_correction_MeV = 48.0*(path_length_mm/target_z_length_mm)*Qw::MeV;
  // where the following assumption comes from,
  // Wednesday, June  8 09:43:05 EDT 2011, jhlee
  //
  //assume 48 MeV total energy loss through the full target length
    
  //momentum_correction_MeV = 24.0*Qw::MeV; // temp. turn off the momentum correction.

  
  Double_t Mp = 938.272013*Qw::MeV;    // Mass of the Proton in MeV
  
  Double_t pre_loss = 20.08*Qw::MeV;
  Double_t depth = vertex_z - target_z_space[0];

  if(vertex_z < target_z_space[0])
    pre_loss = 0.05 * Qw::MeV;  
  else if(vertex_z >= target_z_space[0] && vertex_z <= target_z_space[1])
    //pre_loss *= (vertex_z - target_z_space[0])/target_z_length; // linear approximation
    pre_loss = 0.05 + depth*0.6618 - 0.003462*depth*depth; // quadratic fit approximation
  else
    pre_loss = (18.7 + 3.9)*Qw::MeV;  // averaged total Eloss, downstream, including Al windows

  // pre_loss = 10.0*Qw::MeV; // uncomment this line if using constant method, averaged whole target Eloss
   
  //double PP = momentum + momentum_correction_MeV;
  //double P0 = Mp*PP/(Mp-PP*(1-cos_theta)); //pre-scattering energy
  //double Q2 = 2.0*Mp*(P0-PP);

  //double P0=1160.385;    // for run I
  //Double_t P0=1157.5*Qw::MeV;
  Double_t P0=energy*Qw::MeV;
  P0-=pre_loss;
  Double_t PP = Mp*P0/(Mp+P0*(1-cos_theta));
  Double_t Q2 = 2.0*P0*PP*(1-cos_theta);
  results[0] = PP;
  results[1] = P0;
  results[2] = Q2;
}

#endif // VQWBRIDGINGMETHOD_H
