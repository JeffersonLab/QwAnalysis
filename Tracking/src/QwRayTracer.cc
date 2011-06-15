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
#include "QwBridge.h"
#include "QwRayTracer.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Maximum number of iterations for Newton's method and Runge-Kutta method
#define MAX_ITERATIONS_NEWTON 10
#define MAX_ITERATIONS_RUNGEKUTTA 5000
//#define PI 3.1415926
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwMagneticField* QwRayTracer::fBfield = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Method to print vectors conveniently
 * @param stream Output stream
 * @param v Vector
 * @return Output stream
 */
inline ostream& operator<< (ostream& stream, const TVector3& v)
{
  stream << "(" << v.X() << "," << v.Y() << "," << v.Z() << ")";
  return stream;
}

/**
 * Constructor: set all member field to zero
 */
QwRayTracer::QwRayTracer()
{
  fBdlx = 0.0;
  fBdly = 0.0;
  fBdlz = 0.0;

  fMomentum = 0.0;
  fScatteringAngle = 0.0;
  
  fStartPosition = TVector3(0.0,0.0,0.0);
  fHitPosition   = TVector3(0.0,0.0,0.0);
  fHitDirection  = TVector3(0.0,0.0,0.0);

  fPositionROff = 0.0;
  fPositionPhiOff = 0.0;
  fDirectionThetaOff = 0.0;
  fDirectionPhiOff = 0.0;

  fPositionXOff = 0.0;
  fPositionYOff = 0.0;

  fDirectionXOff = 0.0;
  fDirectionYOff = 0.0;
  fDirectionZOff = 0.0;
 
  fSimFlag = 0;
  fMatchFlag = 0;

  // Load magnetic field
  LoadMagneticFieldMap();
}

/**
 * Destructor
 */
QwRayTracer::~QwRayTracer()
{
  // TODO Someone needs to delete the magnetic field!!!
  QwMessage << "Don't forget to delete the magnetic field!" << QwLog::endl;
}

/**
 * Load the magnetic field map
 * @param filename Filename
 * @return True if the field map was successfully loaded
 */
bool QwRayTracer::LoadMagneticFieldMap()
{
  // If the field has already been loaded, return successfully
  if (fBfield) return true;

  // Otherwise reload the field map
  fBfield = new QwMagneticField();

  // What could possibly go wrong?
  return true;
}


/**
 * Bridge the front and back partial tracks using the ray-tracing technique
 * @param front Front partial track
 * @param back Back partial track
 * @return Zero if successful, non-zero error code if failed
 */
int QwRayTracer::Bridge(
			const QwPartialTrack* front,
			const QwPartialTrack* back)
{
  // Clear the list of tracks
  ClearListOfTracks();

  // Ray-tracing parameters
  double res = 0.5 * Qw::cm; //0.5 * Qw::cm; // position determination resolution
  double step = 1.0 * Qw::cm; // integration step size
  double dp = 10.0 / Qw::GeV; // 10.0 * Qw::MeV; // momentum variation

  double p[2],x[2],y[2],r[2];

  // Front track position and direction
  TVector3 start_position = front->GetPosition(-330.685 * Qw::cm);
  fStartPosition = start_position;
  TVector3 start_direction = front->GetMomentumDirection();
  fScatteringAngle = start_direction.Theta();
  // Estimate initial momentum based on front track
  p[0] = p[1] = EstimateInitialMomentum(start_direction)/Qw::GeV;
  //std::cout<<"estimated momentum = "<<p[0]<<" GeV, dp="<<dp<<" GeV\n";

  // Back track position and direction
  TVector3 end_position = back->GetPosition(439.625 * Qw::cm);
  TVector3 end_direction = back->GetMomentumDirection();

  //std::cout<<"start position(x,y,z)="<<start_position.X()<<","<<start_position.Y()<<", "<<start_position.Z()<<"\n";
  //std::cout<<"end_position(x,y,z)="<<end_position.X()<<","<<end_position.Y()<<", "<<end_position.Z()<<"\n";

  fPositionROff = end_position.Perp();

  TVector3 position, direction;
  int iterations = 0;
  while (fabs(fPositionROff) >= res && iterations < MAX_ITERATIONS_NEWTON) {
    iterations++;

    // p0 - dp
    position = start_position;
    direction = start_direction;
    IntegrateRK4(position, direction, p[0] - dp, end_position.Z(), step);
    x[0] = position.X();
    y[0] = position.Y();

    // p0 + dp
    position = start_position;
    direction = start_direction;
    IntegrateRK4(position, direction, p[0] + dp, end_position.Z(), step);
    x[1] = position.X();
    y[1] = position.Y();

    // Calculate difference
    r[0] = sqrt(x[0]*x[0] + y[0]*y[0]);
    r[1] = sqrt(x[1]*x[1] + y[1]*y[1]);

    // Correction p1 = f(p0)
    if (r[0] != r[1])
      p[1] = p[0] - 0.5 * dp * (r[0] + r[1] - 2.0 * end_position.Perp()) / (r[1] - r[0]);

    // p1
    position = start_position;
    direction = start_direction;
    IntegrateRK4(position, direction, p[1], end_position.Z(), step);
    // x[0] = position.X();
    // y[0] = position.Y();

    // Store differences
    fPositionXOff = position.X() - end_position.X();
    fPositionYOff = position.Y() - end_position.Y();

    fDirectionXOff = direction.X() - end_direction.X();
    fDirectionYOff = direction.Y() - end_direction.Y();
    fDirectionZOff = direction.Z() - end_direction.Z();

    fPositionROff   = position.Perp() - end_position.Perp();
    fPositionPhiOff = position.Phi()  - end_position.Phi();

    fDirectionThetaOff = direction.Theta() - end_direction.Theta();
    fDirectionPhiOff   = direction.Phi()   - end_direction.Phi();

    p[0] = p[1];
    //std::cout<<"p[0]== "<<p[0]<<" GeV,  ";
    //std::cout<<"hit at ("<<x[0]<<","<<y[0]<<")\n";
  }

  if (iterations < MAX_ITERATIONS_NEWTON) {

    fMomentum = p[1]*Qw::GeV;
    QwMessage << "Converged after " << iterations << " iterations." << QwLog::endl;

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
	
    double kinetics[3]={0.0};
    double vertexz=0.0;
    vertexz=CalculateVertex(fStartPosition,fScatteringAngle);
    CalculateKinetics(vertexz,fScatteringAngle,fMomentum,kinetics);
    QwTrack* track = new QwTrack(front,back);
    //track->fMomentum = fMomentum;

    track->fMomentum = kinetics[0] / Qw::GeV;
    track->fTotalEnergy = kinetics[1] / Qw::GeV;
    track->fQ2 = kinetics[2]/ (Qw::GeV * Qw::GeV);
    track->fScatteringAngle = fScatteringAngle * Qw::rad2deg;//180 /PI;
    track->fVertexZ=vertexz;
    track->fPositionRoff = fPositionROff;
    track->fPositionPhioff = fPositionPhiOff;
    track->fDirectionThetaoff = fDirectionThetaOff;
    track->fDirectionPhioff = fDirectionPhiOff;
    track->SetPackage(front->GetPackage());


    QwBridge* bridge = new QwBridge();
    track->fBridge = bridge;

    bridge->fStartPosition = start_position;
    bridge->fStartDirection = start_direction;
    bridge->fEndPositionGoal = end_position;
    bridge->fEndDirectionGoal = end_direction;
    bridge->fEndPositionActual = position;
    bridge->fEndDirectionActual = direction;

    fHitPosition = position;
    fHitDirection = direction;

    fListOfTracks.push_back(track);


    fMatchFlag = 1;
    return 0;

  } else {

    QwMessage << "Can't converge after " << iterations << " iterations." << QwLog::endl;
    //QwMessage << "Hit at at " << fHitPosition * (1/Qw::cm) << " cm with momentum " << p[1]
    //          << " GeV and direction " << fHitDirection << QwLog::endl;
    return -1;
  }
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

  r0[0]/=Qw::m;
  r0[1]/=Qw::m;
  r0[2]/=Qw::m;
  z_end/=Qw::m;
  step /=Qw::m;

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
    point1[0]=x*Qw::m;
    point1[1]=y*Qw::m;
    point1[2]=z*Qw::m;
    fBfield->GetCartesianFieldValue(point1, bfield);

    // First approximation to the changes in the variables for step h (k1)
    dx1 = step * vx;
    dy1 = step * vy;
    dz1 = step * vz;
    // dvx1 = step * beta * (vy * bz - vz * by);
    // dvy1 = step * beta * (vz * bx - vx * bz);
    // dvz1 = step * beta * (vx * by - vy * bx);
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
    // dvx2 = step * beta * (vy*bz - vz*by);
    // dvy2 = step * beta * (vz*bx - vx*bz);
    // dvz2 = step * beta * (vx*by - vy*bx);
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
    // dvx3 = step * beta * (vy*bz - vz*by);
    // dvy3 = step * beta * (vz*bx - vx*bz);
    // dvz3 = step * beta * (vx*by - vy*bx);
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
    // dvx4 = step * beta * (vy*bz - vz*by);
    // dvy4 = step * beta * (vz*bx - vx*bz);
    // dvz4 = step * beta * (vx*by - vy*bx);
    dvx4 = beta * (dy4*bz - dz4*by);
    dvy4 = beta * (dz4*bx - dx4*bz);
    dvz4 = beta * (dx4*by - dy4*bx);

    // Evaluate the path integral (B x dl)
    // fBdlx += step * (vz*by - vy*bz);
    // fBdly += step * (vx*bz - vz*bx);
    // fBdlz += step * (vy*bx - vx*by);
    fBdlx += (dz4*by - dy4*bz);
    fBdly += (dx4*bz - dz4*bx);
    fBdlz += (dy4*bx - dx4*by);
    //std::cout<<"Bfield(x,y,z)"<<bx<<", "<<by<<", "<<bz<<"\n";
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


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwBridge* QwRayTracer::GetBridgingInfo() {

  QwBridge* bridgeinfo = new QwBridge();

  bridgeinfo->xOff = fPositionXOff;
  bridgeinfo->yOff = fPositionYOff;

  // and something?

  return bridgeinfo;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwRayTracer::PrintInfo() {

  std::cout<<std::endl<<"   Front/back bridging information"<<std::endl;
  std::cout<<"====================================================================="<<std::endl;
  std::cout<<" matched hit :    location (x,y,z) : "<<fHitPosition<<std::endl;

  std::cout<<"                         (R,PHI,Z) : ("<<fHitPosition.Perp()<<", "
	   <<fHitPosition.Phi() / Qw::deg<<", "
	   <<fHitPosition.Z()<<")"<<std::endl;

  std::cout<<"              direction (ux,uy,uz) : "<<fHitDirection<<std::endl;

  std::cout<<"                       (theta,phi) : ("<<fHitDirection.Theta() / Qw::deg<<", "
	   <<fHitDirection.Phi() / Qw::deg<<")"<<std::endl;

  std::cout<<"       error :            dR, dPHI : "<<fPositionROff<<", "<<fPositionPhiOff/Qw::deg<<" [cm,deg]"<<std::endl;
  std::cout<<"                      dtheta, dphi : "<<fDirectionThetaOff/Qw::deg<<", "<<fDirectionPhiOff/Qw::deg<<" [deg,deg]"<<std::endl;

  std::cout<<"                          momentum : "<<fMomentum / Qw::GeV << " GeV" << std::endl;
  std::cout<<"====================================================================="<<std::endl<<std::endl;
}



void QwRayTracer::GetBridgingResult(Double_t *buffer) {

  buffer[0] = fHitPosition.X();
  buffer[1] = fHitPosition.Y();
  buffer[2] = fHitPosition.Z();
  buffer[3] = fHitDirection.X();
  buffer[4] = fHitDirection.Y();
  buffer[5] = fHitDirection.Z();

  buffer[6] = fPositionROff;
  buffer[7] = fPositionPhiOff;
  buffer[8] = fDirectionThetaOff;
  buffer[9] = fDirectionPhiOff;

  // jpan: fMomentum is the determined momentum on the z=-250 cm plane.
  // In order to get the scattered momentum (P') inside the target,
  // we should use MC simulation to get the energy loss from scattering
  // vertex to z=-250 cm plane as a correction. This correction should
  // be added onto P' to get the final determined momentum.
  // When the correct scattering angle is determined, it could be used for
  // this purpose.

  // test code for momentum correction
  double vertex_z = fStartPosition.Z() - sqrt(fStartPosition.X()*fStartPosition.X()+fStartPosition.Y()*fStartPosition.Y())/tan(fScatteringAngle);
  double length = 0.0;
  if(vertex_z<(-650.0+35.0/2))
    length = ((-650.0+35.0/2)-vertex_z)/cos(fScatteringAngle); //path length in target after scattering
  double momentum_correction = (length/35.0)*48.0;  //assume 48 MeV total energy loss through the full target length
    
  momentum_correction = 0.0; // 32.0 * Qw::MeV;  // assume 32 MeV with multi-scattering, etc.
    
  double PP = fMomentum + momentum_correction;
  buffer[10] = PP;

  double Mp = 938.272013;    // Mass of the Proton in MeV
    
  double P0 = Mp*PP/(Mp-PP*(1-cos(fScatteringAngle))); //pre-scattering energy
  double Q2 = 2.0*Mp*(P0-PP);
  buffer[11] = P0;
  buffer[12] = Q2;

  buffer[13] = fScatteringAngle;
  buffer[14] = vertex_z;
    
  buffer[15] = fMatchFlag;
    
  //std::cout<<"===========================\n";
  //std::cout<<"eloss in target = "<<momentum_correction<< " MeV\n";
  //std::cout<<"P0="<<P0<<" MeV,  P'="<<PP<<" MeV,  theta="<<fScatteringAngle/Qw::deg<<" deg,  Q2="<<Q2<<" (MeV/c)^2\n";
    
  // NOTE Had to comment the fPrimary variables as they are not read by anymore
  //     buffer[37] = fPrimary_OriginVertexKineticEnergy;
  //     buffer[38] = fPrimary_PrimaryQ2;

  //     double MomentumOff = PP-fPrimary_OriginVertexKineticEnergy;
  //     double Q2_Off = Q2-fPrimary_PrimaryQ2;
  //     buffer[39] = MomentumOff;
  //     buffer[40] = Q2_Off;
  //     buffer[41] = fPrimary_CrossSectionWeight;

}


