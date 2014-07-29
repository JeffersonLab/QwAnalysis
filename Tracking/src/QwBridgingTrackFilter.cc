// Corresonding header
#include "QwBridgingTrackFilter.h"

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwPartialTrack.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwBridgingTrackFilter::QwBridgingTrackFilter()
{
  // Set default filter criteria
  // - polar angle, should probably be split in front and back
  fMinTheta = 1.0 * Qw::deg;
  fMaxTheta = 13.0 * Qw::deg;
  // - difference in polar angle between front and back
  fMinDiffTheta = 5.0 * Qw::deg;
  fMaxDiffTheta = 45.0 * Qw::deg;
  // - azimuthal angle
  fMinPhi = 0.0 * Qw::deg;
  fMaxPhi = 360.0 * Qw::deg;
  // - difference in azimuthal angle between front and back
  fMinDiffPhi = - 20.0 * Qw::deg;
  fMaxDiffPhi =   20.0 * Qw::deg;
  // - longitudinal vertex position in target
  fMinVertexZ = -672.0 * Qw::cm;
  fMaxVertexZ = -628.0 * Qw::cm;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Filter combinations of front and back partial tracks before bridging
 * @param front Front partial track
 * @param back Back partial track
 * @return Failure code (zero when accepted)
 */
QwBridgingTrackFilter::EStatus
QwBridgingTrackFilter::Filter(const QwPartialTrack* front,
                              const QwPartialTrack* back) const
{
    // NOTE The angle limits will be determined from MC simulation results

    // Scattering angle limit
    double theta = front->GetMomentumDirectionTheta();
    if ((theta < fMinTheta) || (theta > fMaxTheta)) {
        QwDebug << "QwBridgingTrackFilter: theta = " << theta/Qw::deg << " deg, "
                << "allowed range [" << fMinTheta/Qw::deg << ","
                                     << fMaxTheta/Qw::deg << "] deg" << QwLog::endl;
        return kFailThetaFront;
    }

    // Bending angle limits in the magnetic field
    double dtheta = (back->GetMomentumDirectionTheta() - front->GetMomentumDirectionTheta());
    if ((dtheta < fMinDiffTheta) || (dtheta > fMaxDiffTheta)) {
      // QwMessage << "QwBridgingTrackFilter: dtheta = " << dtheta/Qw::deg << " deg, "
      //            << "allowed range [" << fMinDiffTheta/Qw::deg << ","
      //                                 << fMaxDiffTheta/Qw::deg << "] deg" << QwLog::endl;
        return kFailDiffTheta;
    }
    double dphi = (back->GetMomentumDirectionPhi() - front->GetMomentumDirectionPhi());
    if ((dphi < fMinDiffPhi) || (dphi > fMaxDiffPhi)) {
      //  QwMessage << "QwBridgingTrackFilter: dphi = " << dphi/Qw::deg << " deg, "
      //            << "allowed range [" << fMinDiffPhi/Qw::deg << ","
      //                                       << fMaxDiffPhi/Qw::deg << "] deg" << QwLog::endl;
        return kFailDiffPhi;
    }

    // Scattering vertex limits and position phi limits (QTOR keep-out zone)
    TVector3 start_position = front->GetPosition(-330.685 * Qw::cm);
    TVector3 start_direction = front->GetMomentumDirection();
    // front track position and angles at z = -250 cm plane
    //double r = fabs(start_position.Z() - (-250.0 * Qw::cm)) / start_direction.Z(); // unused
    //double x = start_position.X() + r * start_direction.X(); // unused
    //double y = start_position.Y() + r * start_direction.Y(); // unused

    //    double position_r = sqrt(x*x + y*y);
    double position_phi = start_position.Phi();
    //double direction_theta = direction_theta = front->GetMomentumDirectionTheta(); // unused

    //double vertex_z = -250.0 * Qw::cm - position_r / tan(acos(start_direction.Z()));
    double vertex_z=-(front->fSlopeX*front->fOffsetX + front->fSlopeY*front->fOffsetY)/(front->fSlopeX*front->fSlopeX+front->fSlopeY*front->fSlopeY);
    if (vertex_z < fMinVertexZ || vertex_z > fMaxVertexZ) {
      //   QwMessage << "QwBridgingTrackFilter: vertex z = " << vertex_z/Qw::cm << " cm, "
      //            << "allowed range [" << fMinVertexZ/Qw::cm << ","
      //                                 << fMaxVertexZ/Qw::cm << "] cm" << QwLog::endl;
        return kFailVertexZ;
    }

    // scattering angle phi and the position_phi at Region 2 should have very small difference
    // this will post a limit on the phi difference
    double direction_phi = 0.0;
    direction_phi = front->GetMomentumDirectionPhi();
    double delta_front_phi = fabs(direction_phi - position_phi);
    if (delta_front_phi> 5.0*Qw::deg) {
      //   QwMessage << "QwBridgingTrackFilter: delta_front_phi = " << delta_front_phi/Qw::deg << " deg, "
      //            << "allowed range [-5, 5] cm" << QwLog::endl;
        return kFailVertexZ;
    }

    // TODO: add in QTOR keep-out zones

    return kPass;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
