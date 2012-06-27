/*! \file   QwRayTracer.cc
 *  \author Jie Pan <jpan@jlab.org>
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *  \date   Thu Nov 26 12:27:10 CST 2009
 *  \brief  Raytrace in magnetic field to bridging R2/R3 partial tracks.
 *
 *  \ingroup QwTracking
 *
 *   Firstly, unreasonable bridging candidates will be filtered out, others
 *   which can pass through the filter will be matched in a look-up table.
 *   If there is a match, the momentum will be determined by interpolation.
 *   In case there is no match in the look-up table, the bridging candidates
 *   will then be sent into a shooting routine for bridging and momentum
 *   determination. The Newton-Raphson method is used in the shooting routine,
 *   and the 4'th order Runge-Kutta method is used for integrating the equations
 *   of motion for electrons in the QTOR magnetic field.
 *
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// System headers
#include <cstdlib>

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwMagneticField.h"
#include "QwTrack.h"
#include "QwRayTracer.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Maximum number of iterations for Newton's method
#define MAX_ITERATIONS_NEWTON 10
// Maximum number of iterations for Runge-Kutta method
#define MAX_ITERATIONS_RUNGEKUTTA 5000

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwMagneticField* QwRayTracer::fBfield = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Constructor: set all member field to zero
 */
QwRayTracer::QwRayTracer(QwOptions& options)
: VQwBridgingMethod()
{
  fBdl = 0.0;
  fBdlx = 0.0;
  fBdly = 0.0;
  fBdlz = 0.0;

  // Process command line options
  ProcessOptions(options);

  // Load magnetic field
  LoadMagneticFieldMap(options);
}

/**
 * Destructor
 */
QwRayTracer::~QwRayTracer()
{
  // Delete the magnetic field
  delete fBfield;
}

/**
 * Load the magnetic field map
 * @param options Options object
 * @return True if the field map was successfully loaded
 */
bool QwRayTracer::LoadMagneticFieldMap(QwOptions& options)
{
  // If the field has already been loaded, return successfully
  if (fBfield) return true;

  // Otherwise reload the field map
  fBfield = new QwMagneticField(options);

  // Test magnetic field validity
  return fBfield->TestFieldMap();
}

/**
 * Define command line and config file options
 * @param options options object
 */
void QwRayTracer::DefineOptions(QwOptions& options)
{
  // Order of the Runge-Kutta method
  options.AddOptions("Momentum reconstruction")("QwRayTracer.order",
      po::value<int>(0)->default_value(5),
      "Runge-Kutta method order (higher than 4 implies adaptive step)");
  // Step size of Runge-Kutta method
  options.AddOptions("Momentum reconstruction")("QwRayTracer.step",
      po::value<float>(0)->default_value(1.0),
      "Runge-Kutta step size [cm]");
  // Step size of Newton's method in momentum
  options.AddOptions("Momentum reconstruction")("QwRayTracer.momentum_step",
      po::value<float>(0)->default_value(10.0),
      "Newton's method momentum step [MeV]");
  // Step size of Newton's method in position
  options.AddOptions("Momentum reconstruction")("QwRayTracer.position_resolution",
      po::value<float>(0)->default_value(1.0),
      "Newton's method position step [cm]");
}

/**
 * Process command line and config file options
 * @param options options object
 */
void QwRayTracer::ProcessOptions(QwOptions& options)
{
  fIntegrationOrder = options.GetValue<int>("QwRayTracer.order");
  fIntegrationStep = Qw::cm * options.GetValue<float>("QwRayTracer.step");
  fMomentumStep = Qw::MeV * options.GetValue<float>("QwRayTracer.momentum_step");
  fPositionResolution = Qw::cm * options.GetValue<float>("QwRayTracer.position_resolution");
}

/**
 * Bridge the front and back partial tracks using the ray-tracing technique
 * @param front Front partial track
 * @param back Back partial track
 * @return List of reconstructed tracks
 */
const QwTrack* QwRayTracer::Bridge(
                        const QwPartialTrack* front,
                        const QwPartialTrack* back)
{
  // No track found yet
  QwTrack* track = 0;

  // Estimate initial momentum from
  Double_t momentum[2] = {0.0};
  momentum[0] = EstimateInitialMomentum(front->GetMomentumDirection());

  // Front track position and direction
  TVector3 start_position = front->GetPosition(-250 * Qw::cm);
  TVector3 start_direction = front->GetMomentumDirection();

  // Back track position and direction
  TVector3 end_position = back->GetPosition(250.0 * Qw::cm);
  TVector3 end_direction = back->GetMomentumDirection();

  TVector3 position = start_position;
  TVector3 direction =  start_direction;
  IntegrateRK(position, direction, momentum[0], end_position.Z(), fIntegrationOrder, fIntegrationStep);
  double positionRoff = position.Perp() - end_position.Perp();

  int mode=0;
  int iterations = 0;
  while (fabs(positionRoff) >= fPositionResolution
      && iterations < MAX_ITERATIONS_NEWTON) {
    ++iterations;

    Double_t r[2] = {0.0, 0.0};
    if(mode==0){

      // momentum - dp
      position = start_position;
      direction = start_direction;
      IntegrateRK(position, direction, momentum[0] - fMomentumStep, end_position.Z(), fIntegrationOrder, fIntegrationStep);
      r[0] = position.Perp();

      // momentum + dp
      position = start_position;
      direction = start_direction;
      IntegrateRK(position, direction, momentum[0] + fMomentumStep, end_position.Z(), fIntegrationOrder, fIntegrationStep);
      r[1] = position.Perp();
    }

    // Correction = f(momentum)
    if (r[0] != r[1]){
      if(r[1]>end_position.Perp() || r[0]<end_position.Perp()){
        momentum[1] = momentum[0] - fMomentumStep * (r[0] + r[1] - 2.0 * end_position.Perp()) / (r[1] - r[0]);
      } else {
        mode=1;
        if (positionRoff < 0)
          momentum[1] = momentum[0] - 0.001;
        else
          momentum[1] = momentum[0] + 0.001;
      }
    }

    // p1
    position = start_position;
    direction = start_direction;
    IntegrateRK(position, direction, momentum[1], end_position.Z(), fIntegrationOrder, fIntegrationStep);
    positionRoff = position.Perp() - end_position.Perp();

    momentum[0] = momentum[1];
  }

  if (iterations < MAX_ITERATIONS_NEWTON) {

    //QwMessage << "Converged after " << iterations << " iterations." << QwLog::endl;
    /*
    if (fMomentum < 0.980 * Qw::GeV || fMomentum > 1.165 * Qw::GeV) {
      QwMessage << "Out of momentum range: determined momentum by shooting: "
		<< fMomentum / Qw::GeV << " GeV" << std::endl;
      return -1;
    }

    if (fabs(fPositionPhiOff) > 10*Qw::deg) {
      QwMessage << "Out of position Phi-matching range: dPhi by shooting: "
		<< fPositionPhiOff / Qw::deg << " deg" << std::endl;
      return -1;
    }

    if (fabs(fDirectionPhiOff) > 10*Qw::deg) {
      QwMessage << "Out of direction phi-matching range: dphi by shooting: "
		<< fDirectionPhiOff / Qw::deg << " deg" << std::endl;
      return -1;
    }
    */

    // Create a new track
    track = new QwTrack(front,back);

    // Reconstructed momentum
    track->fMomentum = momentum[0] / Qw::GeV;

    // Runge-Kutta 4th order
    position = start_position;
    direction = start_direction;
    IntegrateRK(position, direction, momentum[0], end_position.Z(), 4, fIntegrationStep);
    track->fEndPositionRK4 = position;
    track->fEndDirectionRK4 = direction;
    // Runge-Kutta-Fehlberg
    position = start_position;
    direction = start_direction;
    IntegrateRK(position, direction, momentum[0], end_position.Z(), 5, fIntegrationStep);
    track->fEndPositionRKF45 = position;
    track->fEndDirectionRKF45 = direction;

    // Let front partial track determine the package and octant
    track->SetPackage(front->GetPackage());
    track->SetOctant(front->GetOctant());

    // Chi2 is sum of front and back partial tracks
    track->fChi = front->fChi + back->fChi;

    // Position differences at matching plane
    TVector3 position_diff = position - end_position;
    track->fPositionDiff = position_diff;
    track->fPositionXoff = position_diff.X();
    track->fPositionYoff = position_diff.Y();
    track->fPositionRoff     = position.Perp()  - end_position.Perp();
    track->fPositionPhioff   = position.Phi()   - end_position.Phi();
    track->fPositionThetaoff = position.Theta() - end_position.Theta();

    // Direction differences at matching plane
    TVector3 direction_diff = direction - end_direction;
    track->fDirectionDiff = direction_diff;
    track->fDirectionXoff = direction_diff.X();
    track->fDirectionYoff = direction_diff.Y();
    track->fDirectionZoff = direction_diff.Z();
    track->fDirectionPhioff   = direction.Phi()   - end_direction.Phi();
    track->fDirectionThetaoff = direction.Theta() - end_direction.Theta();

    track->fStartPosition = start_position;
    track->fStartDirection = start_direction;
    track->fEndPositionGoal = end_position;
    track->fEndDirectionGoal = end_direction;
    track->fEndPositionActual = position;
    track->fEndDirectionActual = direction;

    // Magnetic field integrals
    track->SetMagneticFieldIntegral(fBdl,fBdlx,fBdly,fBdlz);

  } else {
    QwMessage << "Can't converge after " << iterations << " iterations." << QwLog::endl;
  }

  return track;
}


int QwRayTracer::IntegrateRK(TVector3& r, TVector3& v, const double p, const double z, const int order, const double h)
{
  // Tolerance per step
  const double epsilon = 0.0000001;

  if (order == 2) {
    // Butcher tableau RK2
    const int s = 2;
    const double alpha = 0.5; // 0.5, 1.0, 1.5
    // Define A
    TMatrixD A(s,s);
    A[1][0] = alpha;
    // Define b
    TMatrixD b(1,s);
    b[0][0] = 1.0 - 1.0 / (2.0 * alpha);
    b[0][1] = 1.0 / (2.0 * alpha);

    return IntegrateRK(A, b, r, v, p, z, h, epsilon);
  }

  if (order == 4) {
    // Butcher tableau RK4
    const int s = 4;
    // Define A
    TMatrixD A(s,s);
    A[1][0] = A[2][1] = 1.0 / 2.0;
    A[3][2] = 1.0;
    // Define b
    TMatrixD b(1,s);
    b[0][0] = b[0][3] = 1.0 / 6.0;
    b[0][1] = b[0][2] = 1.0 / 3.0;

    return IntegrateRK(A, b, r, v, p, z, h, epsilon);
  }

  if (order == 5) {
    // Butcher tableau RKF5
    const int s = 6;
    // Define A
    TMatrixD A(s,s);
    A[1][0] = 1.0 / 4.0;
    A[2][0] = 3.0 / 32.0;
    A[2][1] = 9.0 / 32.0;
    A[3][0] = 1932.0 / 2197.0;
    A[3][1] = -7200.0 / 2197.0;
    A[3][2] = 7296.0 / 2197.0;
    A[4][0] = 439.0 / 216.0;
    A[4][1] = -8.0;
    A[4][2] = 3680.0 / 513.0;
    A[4][3] = -845.0 / 4104.0;
    A[5][0] = -8.0 / 27.0;
    A[5][1] = 2.0;
    A[5][2] = -3544.0 / 2565.0;
    A[5][3] = 1859.0 / 4104.0;
    A[5][4] = -11.0 / 40.0;
    // Define b
    TMatrixD b(2,s); b.Zero();
    b[0][0] = 16.0 / 135.0;
    b[0][2] = 6656.0 / 12825.0;
    b[0][3] = 28561.0 / 56430.0;
    b[0][4] = -9.0 / 50.0;
    b[0][5] = 2.0 / 55.0;
    b[1][0] = 25.0 / 216.0;
    b[1][2] = 1408.0 / 2565.0;
    b[1][3] = 2197.0 / 4104.0;
    b[1][4] = -1.0 / 5.0;

    return IntegrateRK(A, b, r, v, p, z, h, epsilon);
  }

  return false;
}

/**
 * Integrate using the Runge-Kutta algorithm
 *
 *  RK integration for trajectory and field integral.
 *  beta = qc/E = -0.2998 / E[GeV] [coul.(m/s)/j]
 *              = -0.2998 / E[MeV] [coul.(mm/s)/j]
 *
 *  The coupled differential equations are :
 *        dx/ds = vx
 *        dy/ds = vy
 *        dz/ds = vz
 *        dvx/ds = beta * (vy*bz - vz*by)
 *        dvy/ds = beta * (vz*bx - vx*bz)
 *        dvz/ds = beta * (vx*by - vy*bx)
 *  where ds is the displacement along the trajectory (= h, the integration step)
 *
 *  If the endpoint is at upstream and startpoint is at downstream,
 *  the electron will swim backward
 *
 * @param A Runge-Kutta matrix
 * @param b Weight vector (first row is lowest order)
 * @param r Initial position (reference to final position)
 * @param v Initial momentum direction (reference to final direction)
 * @param p Initial momentum magnitude
 * @param z Final position
 * @param step Step size
 * @param epsilon Allowed truncation error per step
 * @return Number of iterations
 */
int QwRayTracer::IntegrateRK(
    const TMatrixD& A,
    const TMatrixD& b,
    TVector3& r,
    TVector3& v,
    const double p,
    const double z,
    const double step,
    const double epsilon)
{
  // Order
  const int s = A.GetNrows();

  // Adaptive step if q > 1
  const int q = b.GetNrows();
  const double h_min = 0.1 * Qw::cm;
  const double h_max = 10.0 * Qw::cm;
  double h = step;

  // Determine c
  TVectorD c(s);
  for (int i = 0; i < s; i++)
    for (int j = 0; j < s; j++)
      c[i] += A[i][j];

  // Beta factor
  const double beta = - Qw::c * Qw::e / p;

  // Initial conditions
  TVector3 r_new[2], r_old[2];
  TVector3 v_new[2], v_old[2];
  r_old[0] = r_old[1] = r;
  v_old[0] = v_old[1] = v;

  // Loop
  int iterations = 0;
  bool last_iteration = false;
  while (! last_iteration && iterations < MAX_ITERATIONS_RUNGEKUTTA) {
    iterations++;

    // Last step to end up at z
    if (fabs(r_old[0].Z() - z) < h) {
      h = z - r_old[0].Z();
      last_iteration = true;
    }

    // Determine k
    TVector3 k_r[s];
    TVector3 k_v[s];
    TVector3 point, B;
    for (int i = 0; i < s; i++) {
      r_new[0] = r_old[0];
      v_new[0] = v_old[0];
      for (int j = 0; j < i; j++) {
        r_new[0] += A[i][j] * k_r[j];
        v_new[0] += A[i][j] * k_v[j];
      }
      fBfield->GetCartesianFieldValue(r_new[0], B);

      k_r[i] = h * v_new[0];
      k_v[i] = beta * k_r[i].Cross(B);
    }

    // New values
    for (int p = 0; p < q; p++) {
      r_new[p] = r_old[p];
      v_new[p] = v_old[p];
      for (int i = 0; i < s; i++) {
        r_new[p] += b[p][i] * k_r[i];
        v_new[p] += b[p][i] * k_v[i];
      }

    }

    // Adaptive step
    if (q == 2) {
      TVector3 r_diff = r_new[0] - r_new[1];
      h = pow(epsilon * h / (2 * r_diff.Mag()), 0.25);
      if (h < h_min) h = h_min;
      if (h > h_max) h = h_max;
    }

    // Take lowest order, that's the one for which the error is minimized
    r_old[0] = r_old[1] = r_new[0];
    v_old[0] = v_old[1] = v_new[0];

  } // end of loop

  // Return arguments
  r = r_new[0];
  v = v_new[0];

  if (iterations == MAX_ITERATIONS_RUNGEKUTTA) return -1;
  else return iterations;
}
