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
  // Step size of Runge-Kutta method for fixed steps (order 4 or less)
  options.AddOptions("Momentum reconstruction")("QwRayTracer.step",
      po::value<float>(0)->default_value(1.0),
      "Runge-Kutta fixed step size (for order 4 or less) [cm]");
  // Minimum step size of Runge-Kutta method for adaptive step
  options.AddOptions("Momentum reconstruction")("QwRayTracer.min_step",
      po::value<float>(0)->default_value(0.1),
      "Runge-Kutta minimum step size (for adaptive step) [cm]");
  // Maximum step size of Runge-Kutta method for adaptive step
  options.AddOptions("Momentum reconstruction")("QwRayTracer.max_step",
      po::value<float>(0)->default_value(10.0),
      "Runge-Kutta maximum step size (for adaptive step) [cm]");
  // Maximum step size of Runge-Kutta method for adaptive step
  options.AddOptions("Momentum reconstruction")("QwRayTracer.tolerance",
      po::value<float>(0)->default_value(1e-10),
      "Runge-Kutta adaptive step maximum allowable truncation error");
  // Step size of Newton's method in momentum
  options.AddOptions("Momentum reconstruction")("QwRayTracer.momentum_step",
      po::value<float>(0)->default_value(30.0),
      "Newton's method momentum step [MeV]");

  // Starting momentum of Newton's method
  // Note: Start iteration from a higher momentum limit (1.250 GeV)
  //       as suggested by David Armstrong, to avoid possible bias
  options.AddOptions("Momentum reconstruction")("QwRayTracer.initial_momemtum",
      po::value<float>(0)->default_value(1.25),
      "Newton's method position step [GeV]");

  // Starting position for magnetic field swimming
  options.AddOptions("Momentum reconstruction")("QwRayTracer.start_position",
      po::value<float>(0)->default_value(-250.0),
      "Magnetic field swimming start position [cm]");
  // Ending position for magnetic field swimming
  options.AddOptions("Momentum reconstruction")("QwRayTracer.end_position",
      po::value<float>(0)->default_value(+577.0),
      "Magnetic field swimming end position [cm]");

  // Newton's method optimization variable
  options.AddOptions("Momentum reconstruction")("QwRayTracer.optim_variable",
      po::value<int>(0)->default_value(0),
      "Newton's method optimization variable: 0 for position.Perp(), 1 for direction.Theta()");
  // Newton's method optimization cut-off resolution
  options.AddOptions("Momentum reconstruction")("QwRayTracer.optim_resolution",
      po::value<float>(0)->default_value(0.3),
      "Newton's method optimization cut-off resolution [cm for position, rad for angles]");
}

/**
 * Process command line and config file options
 * @param options options object
 */
void QwRayTracer::ProcessOptions(QwOptions& options)
{
  fIntegrationOrder = options.GetValue<int>("QwRayTracer.order");
  fIntegrationStep = Qw::cm * options.GetValue<float>("QwRayTracer.step");
  fIntegrationMinimumStep = Qw::cm * options.GetValue<float>("QwRayTracer.min_step");
  fIntegrationMaximumStep = Qw::cm * options.GetValue<float>("QwRayTracer.max_step");
  fIntegrationTolerance = options.GetValue<float>("QwRayTracer.tolerance");

  fMomentumStep = Qw::MeV * options.GetValue<float>("QwRayTracer.momentum_step");
  fInitialMomentum = Qw::GeV * options.GetValue<float>("QwRayTracer.initial_momemtum");
  fStartPosition = Qw::cm * options.GetValue<float>("QwRayTracer.start_position");
  fEndPosition = Qw::cm * options.GetValue<float>("QwRayTracer.end_position");

  fOptimizationVariable = options.GetValue<int>("QwRayTracer.optim_variable");
  fOptimizationResolution = options.GetValue<float>("QwRayTracer.optim_resolution");
  switch (fOptimizationVariable) {
    case 0: fOptimizationResolution *= Qw::cm;
    case 1: fOptimizationResolution *= Qw::rad;
    default: fOptimizationResolution *= Qw::cm;
  }
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
  Double_t momentum = EstimateInitialMomentum(front->GetMomentumDirection());

  // Start iteration from a smaller momentum than initial guess,
  // to avoid the gap in the momentum spectrum
  // if(momentum > 0.50 * Qw::GeV)
  //     momentum = momentum - 0.30 * Qw::GeV;
  // else
  //     momentum = 0.20 * Qw::GeV;
  //
  momentum = fInitialMomentum;

  // Front track position and direction
  TVector3 start_position = front->GetPosition(fStartPosition);
  TVector3 start_direction = front->GetMomentumDirection();

  // Back track position and direction
  TVector3 end_position = back->GetPosition(fEndPosition);
  TVector3 end_direction = back->GetMomentumDirection();

  // Position and direction after swimming from front track position and direction
  TVector3 position = start_position;
  TVector3 direction =  start_direction;
  IntegrateRK(position, direction, momentum, end_position.Z(), fIntegrationOrder, fIntegrationStep);

  // Difference to optimize on
  double difference = GetOptimizationVariable(position,direction) - GetOptimizationVariable(end_position,end_direction);

  // Count the number of iterations in the Newton's method and Runge-Kutta method
  int iterations_newton = 0;
  int iterations_rungekutta = 0;

  // Loop until we get blow the resolution required (or give up)
  while (fabs(difference) >= fOptimizationResolution
      && iterations_newton < MAX_ITERATIONS_NEWTON) {

    // Increment number of iterations
    iterations_newton++;

    // Evaluation optimization variable on both side of current momentum
    Double_t r[2] = {0.0, 0.0};

    // momentum - dp
    position = start_position;
    direction = start_direction;
    IntegrateRK(position, direction, momentum - fMomentumStep, end_position.Z(), fIntegrationOrder, fIntegrationStep);
    r[0] = GetOptimizationVariable(position,direction);

    // momentum + dp
    position = start_position;
    direction = start_direction;
    IntegrateRK(position, direction, momentum + fMomentumStep, end_position.Z(), fIntegrationOrder, fIntegrationStep);
    r[1] = GetOptimizationVariable(position,direction);

    // Correction = f(momentum)
    if (r[0] != r[1]) {
      momentum = momentum - fMomentumStep * (r[0] + r[1] - 2.0 * GetOptimizationVariable(end_position,end_direction)) / (r[1] - r[0]);
    }

    // Update momentum
    position = start_position;
    direction = start_direction;
    iterations_rungekutta = IntegrateRK(position, direction, momentum, end_position.Z(), fIntegrationOrder, fIntegrationStep);
    // update the difference
    difference = GetOptimizationVariable(position,direction) - GetOptimizationVariable(end_position,end_direction);

  }


  if (iterations_newton < MAX_ITERATIONS_NEWTON) {

    QwVerbose << "Converged after " << iterations_newton << " iterations." << QwLog::endl;

    /* The following restrictions can now be achieved through bridging filters

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
    track->fMomentum = momentum;
    track->fIterationsNewton = iterations_newton;
    track->fIterationsRungeKutta = iterations_rungekutta;

    // Runge-Kutta 4th order
    position = start_position;
    direction = start_direction;
    track->fIterationsRK4 = IntegrateRK(position, direction, momentum, end_position.Z(), 4, fIntegrationStep);
    track->fEndPositionActualRK4 = position;
    track->fEndDirectionActualRK4 = direction;

    // Runge-Kutta-Fehlberg 4th-5th order
    position = start_position;
    direction = start_direction;
    track->fIterationsRKF45 = IntegrateRK(position, direction, momentum, end_position.Z(), 5, fIntegrationStep);
    track->fEndPositionActualRKF45 = position;
    track->fEndDirectionActualRKF45 = direction;

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
    QwMessage << "Couldn't converge after " << iterations_newton << " iterations." << QwLog::endl;
  }

  return track;
}


int QwRayTracer::IntegrateRK(TVector3& r, TVector3& v, const double p, const double z, const int order, const double h)
{
  // Tolerance per step
  const double tolerance = fIntegrationTolerance;

  if (order == 2) {
    // Butcher tableau RK2
    // Ref: https://en.wikipedia.org/wiki/Runge-Kutta_methods
    const int s = 2;
    const int q = 1;
    const double alpha = 0.5; // 0.5, 1.0, 1.5
    // Define A
    TMatrixD A(s,s); A.Zero();
    A[1][0] = alpha;
    // Define b
    TMatrixD b(q,s); b.Zero();
    b[0][0] = 1.0 - 1.0 / (2.0 * alpha);
    b[0][1] = 1.0 / (2.0 * alpha);

    return IntegrateRK(A, b, r, v, p, z, h, tolerance);
  }

  if (order == 4) {
    // Butcher tableau RK4
    // Ref: https://en.wikipedia.org/wiki/Runge-Kutta_methods
    const int s = 4;
    const int q = 1;
    // Define A
    TMatrixD A(s,s); A.Zero();
    A[1][0] = A[2][1] = 1.0 / 2.0;
    A[3][2] = 1.0;
    // Define b
    TMatrixD b(q,s); b.Zero();
    b[0][0] = b[0][3] = 1.0 / 6.0;
    b[0][1] = b[0][2] = 1.0 / 3.0;

    return IntegrateRK(A, b, r, v, p, z, h, tolerance);
  }

  if (order == 5) {
    // Butcher tableau RKF4(5)
    // Ref: https://en.wikipedia.org/wiki/Runge-Kutta-Fehlberg_method
    const int s = 6;
    const int q = 2;
    // Define A
    TMatrixD A(s,s); A.Zero();
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
    TMatrixD b(q,s); b.Zero();
    b[0][0] = 16.0 / 135.0;
    b[0][2] = 6656.0 / 12825.0;
    b[0][3] = 28561.0 / 56430.0;
    b[0][4] = -9.0 / 50.0;
    b[0][5] = 2.0 / 55.0;
    b[1][0] = 25.0 / 216.0;
    b[1][2] = 1408.0 / 2565.0;
    b[1][3] = 2197.0 / 4104.0;
    b[1][4] = -1.0 / 5.0;

    return IntegrateRK(A, b, r, v, p, z, h, tolerance);
  }

  // Return zero iterations if no matching Runge-Kutta algorithm
  return 0;
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
 *  Ref: https://en.wikipedia.org/wiki/Runge-Kutta_methods#Explicit_Runge.E2.80.93Kutta_methods
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
    const double tolerance)
{
  // Order
  const int s = A.GetNrows();

  // Adaptive step if q > 1
  const int q = b.GetNrows();
  const double h_min = fIntegrationMinimumStep;
  const double h_max = fIntegrationMaximumStep;
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
      TVector3 r_diff = (r_old[0].Z() - z) / v_old[0].Z() * v_old[0];
      h = r_diff.Mag();
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
    // Ref: http://mathfaculty.fullerton.edu/mathews//n2003/rungekuttafehlberg/RungeKuttaFehlbergProof.pdf
    if (q == 2) {
      TVector3 r_diff = r_new[0] - r_new[1];
      h *= pow(tolerance * h / (2 * r_diff.Mag()), 0.25);
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
