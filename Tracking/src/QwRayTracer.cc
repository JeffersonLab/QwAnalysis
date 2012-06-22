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
  momentum[0] = 0;

  // Front track position and direction
  TVector3 start_position  = front->GetPosition(-250 * Qw::cm);
  TVector3 start_direction = front->GetMomentumDirection();

  // Back track position and direction
  TVector3 end_position  = back->GetPosition(250.0 * Qw::cm);
  TVector3 end_direction = back->GetMomentumDirection();


  double positionRoff = end_position.Perp();

  TVector3 position = start_position;
  TVector3 direction =  start_direction;
  IntegrateRK4(position, direction, momentum[0], end_position.Z(), fIntegrationStep);
  positionRoff = position.Perp() - end_position.Perp();

  int mode = 0;
  int iterations = 0;
  while (fabs(positionRoff) >= fPositionResolution
      && iterations < MAX_ITERATIONS_NEWTON) {
    ++iterations;

    Double_t r[2] = {0.0, 0.0};
    if(mode==0){
      Double_t x[2] = {0.0, 0.0};
      Double_t y[2] = {0.0, 0.0};

      // momentum - dp
      position = start_position;
      direction = start_direction;
      IntegrateRK4(position, direction, momentum[0] - fMomentumStep, end_position.Z(), fIntegrationStep);
      x[0] = position.X();
      y[0] = position.Y();

      // momentum + dp
      position = start_position;
      direction = start_direction;
      IntegrateRK4(position, direction, momentum[0] + fMomentumStep, end_position.Z(), fIntegrationStep);
      x[1] = position.X();
      y[1] = position.Y();

      // Calculate difference
      r[0] = sqrt(x[0]*x[0] + y[0]*y[0]);
      r[1] = sqrt(x[1]*x[1] + y[1]*y[1]);
    }

    // Correction = f(momentum)
    if (r[0] != r[1]){
      if (r[1] > end_position.Perp() || r[0] < end_position.Perp()) {
        momentum[1] = momentum[0] - fMomentumStep * (r[0] + r[1] - 2.0 * end_position.Perp()) / (r[1] - r[0]);
      } else {
        mode = 1;
        if (positionRoff < 0)
          momentum[1] = momentum[0] - 0.001;
        else
          momentum[1] = momentum[0] + 0.001;
      }
    }

    // p1
    position = start_position;
    direction = start_direction;
    IntegrateRK4(position, direction, momentum[1], end_position.Z(), fIntegrationStep);

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
    track->fMomentum = momentum[0];

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


/**
 * Integrate using the Runge-Kutta 4th order algorithm
 *
 *  RK4 integration for trajectory and field integral.
 *  beta = qc/e = -0.2998 / E[GeV] [coul.(m/s)/j]
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
 * @param r0 Initial position (reference to final position)
 * @param uv0 Initial momentum direction (reference to final direction)
 * @param p0 Initial momentum magnitude
 * @param z_end Final position
 * @param step Step size
 * @return True if the integration was successful
 */
bool QwRayTracer::IntegrateRK4(TVector3& r0, TVector3& uv0, const double p0, double z_end, double step)
{
  r0[0] /= Qw::m;
  r0[1] /= Qw::m;
  r0[2] /= Qw::m;
  z_end /= Qw::m;
  step  /= Qw::m;

  // Local variables
  double xx[2],yy[2],zz[2];
  double uvx[2],uvy[2],uvz[2];
  double x1,y1,z1;
  double vx,vy,vz,vx1,vy1,vz1;
  double dx1,dy1,dz1;
  double dx2,dy2,dz2;
  double dx3,dy3,dz3;
  double dx4,dy4,dz4;
  double dvx1,dvy1,dvz1;
  double dvx2,dvy2,dvz2;
  double dvx3,dvy3,dvz3;
  double dvx4,dvy4,dvz4;

  double point1[3];

  // Position vector and references to components
  double point[3];
  double &x = point[0];
  double &y = point[1];
  double &z = point[2];

  // Magnetic field and references to components
  double bfield[3];
  double &bx = bfield[0];
  double &by = bfield[1];
  double &bz = bfield[2];

  // Momentum
  double beta = -0.2998 / p0;

  // Field integral
  fBdl = 0.0;
  fBdlx = 0.0;
  fBdly = 0.0;
  fBdlz = 0.0;

  xx[0] = r0[0];
  yy[0] = r0[1];
  zz[0] = r0[2];

  // Reverse coordinates for backward swimming
  if (r0.Z() > z_end) {
    xx[0] = -xx[0];
    zz[0] = -zz[0];
    uvx[0] =  uv0[0];
    uvy[0] = -uv0[1];
    uvz[0] =  uv0[2];
    z_end = -z_end;
  } else { // forward swimming
    uvx[0] = uv0[0];
    uvy[0] = uv0[1];
    uvz[0] = uv0[2];
  }

  // Integration loop
  int iterations = 0;
  while (fabs(zz[0] - z_end) >= step && iterations < MAX_ITERATIONS_RUNGEKUTTA) {
    iterations++;

    // Values of the cordinates, unit vector and field at start of interval
    x = x1 = xx[0];
    y = y1 = yy[0];
    z = z1 = zz[0];
    vx = vx1 = uvx[0];
    vy = vy1 = uvy[0];
    vz = vz1 = uvz[0];
    point1[0] = x*Qw::m;
    point1[1] = y*Qw::m;
    point1[2] = z*Qw::m;
    fBfield->GetCartesianFieldValue(point1, bfield);

    // First approximation to the changes in the variables for step h (k1)
    dx1 = step * vx;
    dy1 = step * vy;
    dz1 = step * vz;
    dvx1 = beta * (dy1 * bz - dz1 * by);
    dvy1 = beta * (dz1 * bx - dx1 * bz);
    dvz1 = beta * (dx1 * by - dy1 * bx);

    // Next approximation to the values of the variables for step h/2
    x = x1 + dx1 / 2.0;
    y = y1 + dy1 / 2.0;
    z = z1 + dz1 / 2.0;
    vx = vx1 + dvx1 / 2.0;
    vy = vy1 + dvy1 / 2.0;
    vz = vz1 + dvz1 / 2.0;
    point1[0]=x*Qw::m;
    point1[1]=y*Qw::m;
    point1[2]=z*Qw::m;
    fBfield->GetCartesianFieldValue(point1, bfield);

    // Second approximation to the changes in the variables for step h (k2)
    dx2 = step * vx;
    dy2 = step * vy;
    dz2 = step * vz;
    dvx2 = beta * (dy2*bz - dz2*by);
    dvy2 = beta * (dz2*bx - dx2*bz);
    dvz2 = beta * (dx2*by - dy2*bx);
    

    // Next approximation to the values of the variables for step h/2
    x = x1 + dx2 / 2.0;
    y = y1 + dy2 / 2.0;
    z = z1 + dz2 / 2.0;
    vx = vx1 + dvx2 / 2.0;
    vy = vy1 + dvy2 / 2.0;
    vz = vz1 + dvz2 / 2.0;
    point1[0]=x*Qw::m;
    point1[1]=y*Qw::m;
    point1[2]=z*Qw::m;
    fBfield->GetCartesianFieldValue(point1, bfield);

    // Third approximation to the changes in the variables for step h (k3)
    dx3 = step * vx;
    dy3 = step * vy;
    dz3 = step * vz;
    dvx3 = beta * (dy3*bz - dz3*by);
    dvy3 = beta * (dz3*bx - dx3*bz);
    dvz3 = beta * (dx3*by - dy3*bx);

    // Next approximation to the values of the variables for step h, not h/2
    x = x1 + dx3;
    y = y1 + dy3;
    z = z1 + dz3;
    vx = vx1 + dvx3;
    vy = vy1 + dvy3;
    vz = vz1 + dvz3;
    point1[0]=x*Qw::m;
    point1[1]=y*Qw::m;
    point1[2]=z*Qw::m;
    fBfield->GetCartesianFieldValue(point1, bfield);

    // Fourth approximation to the changes in the variables for step h (k4)
    dx4 = step * vx;
    dy4 = step * vy;
    dz4 = step * vz;
    dvx4 = beta * (dy4*bz - dz4*by);
    dvy4 = beta * (dz4*bx - dx4*bz);
    dvz4 = beta * (dx4*by - dy4*bx);

    // Evaluate the path integral (B x dl)
    fBdl += dx4 * bx + dy4 * by + dz4 * bz;
    fBdlx += (dz4*by - dy4*bz);
    fBdly += (dx4*bz - dz4*bx);
    fBdlz += (dy4*bx - dx4*by);

    // Final estimates of trajectory
    xx[1] = xx[0] + (dx1 + 2.0*dx2 + 2.0*dx3 + dx4) / 6.0;
    yy[1] = yy[0] + (dy1 + 2.0*dy2 + 2.0*dy3 + dy4) / 6.0;
    zz[1] = zz[0] + (dz1 + 2.0*dz2 + 2.0*dz3 + dz4) / 6.0;
    uvx[1] = uvx[0] + (dvx1 + 2.0*dvx2 + 2.0*dvx3 + dvx4) / 6.0;
    uvy[1] = uvy[0] + (dvy1 + 2.0*dvy2 + 2.0*dvy3 + dvy4) / 6.0;
    uvz[1] = uvz[0] + (dvz1 + 2.0*dvz2 + 2.0*dvz3 + dvz4) / 6.0;

    // Starting point for next step
    xx[0] = xx[1];
    yy[0] = yy[1];
    zz[0] = zz[1];
    uvx[0] = uvx[1];
    uvy[0] = uvy[1];
    uvz[0] = uvz[1];

  } // end of while loop

    // Reverse coordinates for backward swimming
  if (r0.Z() > z_end) {
    xx[0] = -xx[0];
    zz[0] = -zz[0];
    uvy[0] = -uvy[0];
  }

  // Actual position of the track
  r0 = TVector3(xx[0]*Qw::m, yy[0]*Qw::m, zz[0]*Qw::m);
  uv0 = TVector3(uvx[0], uvy[0], uvz[0]);

  return true;
}
