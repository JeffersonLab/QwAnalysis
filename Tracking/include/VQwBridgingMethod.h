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
  VQwBridgingMethod() { };
  /// Destructor
  virtual ~VQwBridgingMethod() { };


  /// \brief Bridge from the front to back partial track (pure virtual)
  virtual const QwTrack* Bridge(const QwPartialTrack* front, const QwPartialTrack* back) = 0;


 protected:

  /// Estimate the momentum based only on the direction
  virtual double EstimateInitialMomentum(const TVector3& direction) const;

}; // class VQwBridgingMethod


/**
 * Estimate the momentum for a first attempt at bridging.
 *
 * This takes into account only the direction of the front partial track and assumes
 * an elastic electron-proton reaction without any energy loss.  Any effects of the
 * energy loss should be small enough that they can be recovered from the bridging
 * algorithm in little time, and this avoids any bias of LH2 over Al events (E_loss).
 *
 * @param direction Direction of front partial track
 * @return Initial momentum
 */
inline double VQwBridgingMethod::EstimateInitialMomentum(const TVector3& direction) const
{
  double Mp = Qw::Mp;           // proton mass
  double e0 = 1.165 * Qw::GeV;  // beam energy
  // \todo Get the beam energy from the event (?)

  // Kinematics for elastic e+p scattering
  double cth = direction.CosTheta();
  return e0 / (1.0 + e0 / Mp * (1 - cth));
}

#endif // VQWBRIDGINGMETHOD_H
