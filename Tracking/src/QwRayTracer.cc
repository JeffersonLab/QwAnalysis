
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
#include "QwBridge.h"
#include "QwRayTracer.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#define VSIZE 100

//#define TESTEVENT
//#define BACKSWIM

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwRayTracer::QwRayTracer() {

    fStartPosition = TVector3(0.0,0.0,0.0);
    fStartDirection = TVector3(0.0,0.0,0.0);
    fStartPositionR = 0.0;
    fStartPositionPhi = 0.0;
    fStartDirectionTheta = 0.0;
    fStartDirectionPhi = 0.0;

    fEndPosition = TVector3(0.0,0.0,0.0);
    fEndDirection = TVector3(0.0,0.0,0.0);
    fEndPositionR = 0.0;
    fEndPositionPhi = 0.0;
    fEndDirectionTheta = 0.0;
    fEndDirectionPhi = 0.0;

    fHitLocation = TVector3(0.0,0.0,0.0);
    fHitDirection = TVector3(0.0,0.0,0.0);
    fHitLocationR = 0.0;
    fHitLocationPhi = 0.0;
    fHitDirectionTheta = 0.0;
    fHitDirectionPhi = 0.0;

    fBdlx = 0.0;
    fBdly = 0.0;
    fBdlz = 0.0;

    fPositionXOff = 0.0;
    fPositionYOff = 0.0;
    fDirectionXOff = 0.0;
    fDirectionYOff = 0.0;
    fDirectionZOff = 0.0;
    fPositionROff = 0.0;
    fPositionPhiOff = 0.0;
    fDirectionThetaOff = 0.0;
    fDirectionPhiOff = 0.0;

    //fBfield = NULL;

    fSimFlag = 0;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwRayTracer::~QwRayTracer() {

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwMagneticField* QwRayTracer::fBfield = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// Need a static member function to initialize the static *fBfield.
bool QwRayTracer::LoadMagneticFieldMap(const std::string filename)
{
    // If the field has already been loaded, return successfully
    if (fBfield) return true;

    // Otherwise reload the field map
    fBfield = new QwMagneticField();
    return fBfield->ReadFieldMapFile(filename);
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwRayTracer::SetStartAndEndPoints(TVector3 startposition, TVector3 startdirection,
                                        TVector3 endposition, TVector3 enddirection) {

    fStartPosition = startposition;
    fStartDirection = startdirection;
    fEndPosition = endposition;
    fEndDirection = enddirection;

    fStartPositionR = sqrt(fStartPosition.X()*fStartPosition.X()+fStartPosition.Y()*fStartPosition.Y());
    fStartPositionPhi = atan2(fStartPosition.Y(),fStartPosition.X())*DEGREE;
    if (fStartPositionPhi<0) fStartPositionPhi = fStartPositionPhi+360.0;

    fStartDirectionTheta = acos(fStartDirection.Z())*DEGREE;
    fStartDirectionPhi = atan2(fStartDirection.Y(),fStartDirection.X())*DEGREE;
    if (fStartDirectionPhi<0) fStartDirectionPhi = fStartDirectionPhi+360.0;

    fEndPositionR = sqrt(fEndPosition.X()*fEndPosition.X()+fEndPosition.Y()*fEndPosition.Y());
    fEndPositionPhi = atan2(fEndPosition.Y(),fEndPosition.X())*DEGREE;
    if (fEndPositionPhi<0) fEndPositionPhi = fEndPositionPhi+360.0;

    fEndDirectionTheta = acos(fEndDirection.Z())*DEGREE;
    fEndDirectionPhi = atan2(fEndDirection.Y(),fEndDirection.X())*DEGREE;
    if (fEndDirectionPhi<0) fEndDirectionPhi = fEndDirectionPhi+360.0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwRayTracer::BridgeFrontBackPartialTrack() {

    int status;

    fMatchFlag = -2;

    status = Filter();
    if (status == -1)  return -1;

    status = Shooting();

    // if (status == -1)  status = DoForcedBridging();

    return status;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

bool QwRayTracer::Bridge(QwPartialTrack* front, QwPartialTrack* back)
{
    int status;

    fMatchFlag = -2;
    status = Filter();
    if (status == -1)  return -1;

    status = Shooting();

    // status = Shooting();

    // if (status == -1)  status = DoForcedBridging();

    return status;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// The angle limits will be determined from MC simulation results
int QwRayTracer::Filter()
{

    // swimming direction limit
    if (fStartPosition.Z() >= fEndPosition.Z()) {
        std::cerr<<"Filter: start point location is at downstream of end point location"<<std::endl;
        fMatchFlag = -1;
        return -1;
    }

    // scattering angle limit
    if ((fStartDirectionTheta < 4.0) || (fStartDirectionTheta > 13.0)) {
        std::cerr<<"Filter: scattering angle ("<<fStartDirectionTheta<<" degree) is out of range"<<std::endl;
        fMatchFlag = -1;
        return -1;
    }

    //bending angle limit in B field
    double dtheta = (fEndDirectionTheta-fStartDirectionTheta);
    if (dtheta<6.0 || dtheta>22.0) {
        std::cerr<<"Filter: bending angles (dtheta="<<dtheta
        <<" degree) in B field are out of range"<<std::endl;
        fMatchFlag = -1;
        return -1;
    }

    double dphi = (fEndDirectionPhi - fStartDirectionPhi);
    if (fabs(dphi)>10.0) {
        std::cerr<<"Filter: bending angles (dphi="<<dphi
        <<" degree) in B field are out of range"<<std::endl;
        fMatchFlag = -1;
        return -1;
    }

    //scattering vertex limits and position phi limits (QTOR keep-out zone)
    // front track position and angles at z= -250 cm plane
    double r = fabs(fStartPosition.Z()-(-250.0))/fStartDirection.Z();
    double x = fStartPosition.X() + r*fStartDirection.X();
    double y = fStartPosition.Y() + r*fStartDirection.Y();

    double position_r = sqrt(x*x+y*y);
    double position_phi = fStartPositionPhi;
    double direction_theta = fStartDirectionTheta;

    double vertex_z = -250.0 - position_r/tan(acos(fStartDirection.Z()));

    if (vertex_z<(-672.0) || vertex_z>(-628.0)) {
        std::cerr<<"Filter: scattering vertex z="<<vertex_z<<" cm is out of range"<<std::endl;
        fMatchFlag = -1;
        return -1;
    }

    // TODO: add in QTOR keep-out zones

    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwRayTracer::Filter(QwPartialTrack* front, QwPartialTrack* back)
{
    // Scattering angle limit
    if ((front->GetMomentumDirectionTheta() < 4.0 / DEGREE)
     || (front->GetMomentumDirectionTheta() > 13.0 / DEGREE)) {
        QwError << "Filter: scattering angle (" << front->GetMomentumDirectionTheta() * DEGREE
                << " degree) is out of range" << QwLog::endl;
        fMatchFlag = -1;
        return -1;
    }

    // Bending angle limit in B field
    double dtheta = (back->GetMomentumDirectionTheta() - front->GetMomentumDirectionTheta());
    double dphi   = (back->GetMomentumDirectionPhi()   - front->GetMomentumDirectionPhi());
    if (dtheta < 5.0 / DEGREE || dtheta>25.0 || fabs(dphi)>10.0) {
        std::cerr<<"Filter: bending angles (dtheta="<<dtheta<<" degree, dphi="<<dphi
        <<" degree) in B field are out of range"<<std::endl;
        fMatchFlag = -1;
        return -1;
    }

    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwRayTracer::Shooting() {

    double res = 0.2; //0.5; //position determination [cm]
    double step_size = 1.0; // integration step size [cm]
    double dp = 0.005; // 0.01; //momentum variation [GeV]

    double p[2],x[2],y[2],r[2];

    p[0] = p[1] = EstimateInitialMomentum(fStartDirection);
    //std::cout<<"estimated momentum: "<<p[0]<<std::endl;

    int n;
    for (n=1; n<=MAX_ITERATION; n++) {

        Integrate(p[0]-dp, step_size);
        x[0]=fHitLocation.X();
        y[0]=fHitLocation.Y();

        Integrate(p[0]+dp, step_size);
        x[1]=fHitLocation.X();
        y[1]=fHitLocation.Y();

        r[0] = sqrt(x[0]*x[0]+y[0]*y[0]);
        r[1] = sqrt(x[1]*x[1]+y[1]*y[1]);

        if (r[0]!=r[1])
            p[1] = p[0] -0.5*dp*(r[0]+r[1]-2.0*fEndPositionR)/(r[1]-r[0]);

        Integrate(p[1], step_size);
        x[0]=fHitLocation.X();
        y[0]=fHitLocation.Y();

        fHitLocationR = sqrt(x[0]*x[0]+y[0]*y[0]);
        fHitLocationPhi = atan2(y[0],x[0])*DEGREE;
        if (fHitLocationPhi<0.0) fHitLocationPhi+=360;

        fHitDirectionTheta = acos(fHitDirection.Z())*DEGREE;
        fHitDirectionPhi = atan2(fHitDirection.Y(),fHitDirection.X())*DEGREE;
        if (fHitDirectionPhi<0.0) fHitDirectionPhi+=360;

        fPositionXOff = fHitLocation.X() - fEndPosition.X();
        fPositionYOff = fHitLocation.Y() - fEndPosition.Y();

        fDirectionXOff = fHitDirection.X() - fEndDirection.X();
        fDirectionYOff = fHitDirection.Y() - fEndDirection.Y();
        fDirectionZOff = fHitDirection.Z() - fEndDirection.Z();

        fPositionROff = fHitLocationR - fEndPositionR;
        fPositionPhiOff = fHitLocationPhi - fEndPositionPhi;

        fDirectionThetaOff = fHitDirectionTheta - fEndDirectionTheta;
        fDirectionPhiOff = fHitDirectionPhi - fEndDirectionPhi;

        if ( fPositionROff<res) {
            fMomentum = p[1];
            std::cout<<"Converged after "<<n<<" iterations."<<std::endl;

            if (fMomentum<0.980 || fMomentum>1.165) {
                std::cerr<<"Out of momentum range: determined momentum by shooting: "
                << fMomentum<<" GeV"<<std::endl;
                return -1;
            }

            fMatchFlag = 1;
            return 0;
        }

        p[0] = p[1];
    }

    std::cerr<<"Can't converge after "<<n<<" iterations."<<std::endl;
    std::cerr<<"Hit at at ("<<fHitLocation.X()<<", "<<fHitLocation.Y()<<", "<<fHitLocation.Z()
    <<"), with momentum "<<p[1]<<" GeV and direction ("
    <<fHitDirection.X()<<", "<<fHitDirection.Y()<<", "<<fHitDirection.Z()<<")" <<std::endl;
    return -1;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

double QwRayTracer::EstimateInitialMomentum(TVector3 direction) {

    double cth = direction.Z();        // cos(theta) = uz/r, where ux,uy,uz form a unit vector
    double wp = 0.93828;              // proton mass [GeV]
    double e0 = 1.165;                // beam energy [GeV]
    double e = e0/(1.0+e0/wp*(1.0-cth)) - 0.012; //kinematics for e+p scattering, target energy loss ~12 MeV
    return e;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//************************************************************
//     RK4 integration for trajectory and field integral.
//     linear dimensions must be in m and b in tesla
//     beta=qc/e=-0.2998/e(GeV) [coul.(m/s)/j]
//*************************************************************
//     the coupled differential equations are :
//         dx/ds=vx
//         dy/ds=vy
//         dz/ds=vz
//         dvx/ds=beta*(vy*bz-vz*by)
//         dvy/ds=beta*(vz*bx-vx*bz)
//         dvz/ds=beta*(vx*by-vy*bx)
//     where ds is the displacement along the trajectory (=h, the int. step)
//
//input: values of the cordinates (x0,y0,z0), slopes (vx0,vy0,vz0) at starting point
//output: coordinates of endpoint, direction at endpoint and field integral
//*************************************************************

// If the endpoint is at upstream and startpoint is at downstream,
// the electron will swim backward

int QwRayTracer::Integrate(double e0, double step) {

// local variables
    double xx[2],yy[2],zz[2];
    double uvx[2],uvy[2],uvz[2];
    double x,y,z,x1,y1,z1;
    double vx,vy,vz,vx1,vy1,vz1;
    double dx1,dy1,dz1;
    double dx2,dy2,dz2;
    double dx3,dy3,dz3;
    double dx4,dy4,dz4;
    double dvx1,dvy1,dvz1;
    double dvx2,dvy2,dvz2;
    double dvx3,dvy3,dvz3;
    double dvx4,dvy4,dvz4;

    double bx,by,bz;  // B field components
    double point[3];
    double bfield[3];

    double beta = -0.2998/e0;
    fBdlx=0.0;
    fBdly=0.0;
    fBdlz=0.0;

// convert linear dimensions cm -> m
    step = step*0.01;
    double z_endplane = fEndPosition.Z()*0.01;

    xx[0]=fStartPosition.X()*0.01;
    yy[0]=fStartPosition.Y()*0.01;
    zz[0]=fStartPosition.Z()*0.01;

    //reverse coordinates for backward swiming
    if (fStartPosition.Z()*0.01>z_endplane) {
        xx[0]=-xx[0];
        zz[0]=-zz[0];
        uvx[0]= fStartDirection.X();
        uvy[0]=-fStartDirection.Y();
        uvz[0]= fStartDirection.Z();
        z_endplane=-z_endplane;
    } else { //forward swiming
        uvx[0]=fStartDirection.X();
        uvy[0]=fStartDirection.Y();
        uvz[0]=fStartDirection.Z();
    }

    while ( fabs(zz[0]-z_endplane)>=step ) {  // integration loop

        //values of the cordinates, unit vector and field at start of interval
        x1=xx[0];
        y1=yy[0];
        z1=zz[0];
        vx1=uvx[0];
        vy1=uvy[0];
        vz1=uvz[0];

        point[0]=x1*100.0;
        point[1]=y1*100.0;
        point[2]=z1*100.0;
        QwRayTracer::fBfield->GetFieldValue(point, bfield, BSCALE);
        bx = bfield[0];
        by = bfield[1];
        bz = bfield[2];

//     first approximation to the changes in the variables for step h (k1)
        dx1=step*vx1;
        dy1=step*vy1;
        dz1=step*vz1;
        dvx1=step*beta*(vy1*bz-vz1*by);
        dvy1=step*beta*(vz1*bx-vx1*bz);
        dvz1=step*beta*(vx1*by-vy1*bx);

//     next approximation to the values of the variables for step h/2
        x=x1+dx1/2.0;
        y=y1+dy1/2.0;
        z=z1+dz1/2.0;
        vx=vx1+dvx1/2.0;
        vy=vy1+dvy1/2.0;
        vz=vz1+dvz1/2.0;
        point[0]=x*100.0;
        point[1]=y*100.0;
        point[2]=z*100.0;
        QwRayTracer::fBfield->GetFieldValue(point, bfield, BSCALE);
        bx = bfield[0];
        by = bfield[1];
        bz = bfield[2];

//     second approximation to the changes in the variables for step h (k2)
        dx2=step*vx;
        dy2=step*vy;
        dz2=step*vz;
        dvx2=step*beta*(vy*bz-vz*by) ;
        dvy2=step*beta*(vz*bx-vx*bz);
        dvz2=step*beta*(vx*by-vy*bx);

//     next approximation to the values of the variables for step h/2
        x=x1+dx2/2.0;
        y=y1+dy2/2.0;
        z=z1+dz2/2.0;
        vx=vx1+dvx2/2.0;
        vy=vy1+dvy2/2.0;
        vz=vz1+dvz2/2.0;
        point[0]=x*100.0;
        point[1]=y*100.0;
        point[2]=z*100.0;
        QwRayTracer::fBfield->GetFieldValue(point, bfield, BSCALE);
        bx = bfield[0];
        by = bfield[1];
        bz = bfield[2];

//    third approximation to the changes in the variables for step h (k3)
        dx3=step*vx;
        dy3=step*vy;
        dz3=step*vz;
        dvx3=step*beta*(vy*bz-vz*by);
        dvy3=step*beta*(vz*bx-vx*bz);
        dvz3=step*beta*(vx*by-vy*bx);

// next approximation to the values of the variables for step h, not h/2
        x=x1+dx3;
        y=y1+dy3;
        z=z1+dz3;
        vx=vx1+dvx3;
        vy=vy1+dvy3;
        vz=vz1+dvz3;
        point[0]=x*100.0;
        point[1]=y*100.0;
        point[2]=z*100.0;
        QwRayTracer::fBfield->GetFieldValue(point, bfield, BSCALE);
        bx = bfield[0];
        by = bfield[1];
        bz = bfield[2];

//evaluate the path integral (b x dl)
        fBdlx=fBdlx+step*(vz*by-vy*bz);
        fBdly=fBdly+step*(vx*bz-vz*bx);
        fBdlz=fBdlz+step*(vy*bx-vx*by);

//fourth approximation to the changes in the variables for step h (k4)
        dx4=step*vx;
        dy4=step*vy;
        dz4=step*vz;
        dvx4=step*beta*(vy*bz-vz*by);
        dvy4=step*beta*(vz*bx-vx*bz);
        dvz4=step*beta*(vx*by-vy*bx);

//final estimates of trajectory
        xx[1]=xx[0]+(dx1+2.0*dx2+2.0*dx3+dx4)/6.0;
        yy[1]=yy[0]+(dy1+2.0*dy2+2.0*dy3+dy4)/6.0;
        zz[1]=zz[0]+(dz1+2.0*dz2+2.0*dz3+dz4)/6.0;
        uvx[1]=uvx[0]+(dvx1+2.0*dvx2+2.0*dvx3+dvx4)/6.0;
        uvy[1]=uvy[0]+(dvy1+2.0*dvy2+2.0*dvy3+dvy4)/6.0;
        uvz[1]=uvz[0]+(dvz1+2.0*dvz2+2.0*dvz3+dvz4)/6.0;

        xx[0]=xx[1];
        yy[0]=yy[1];
        zz[0]=zz[1];
        uvx[0]=uvx[1];
        uvy[0]=uvy[1];
        uvz[0]=uvz[1];

    } //end of while loop

    //reverse coordinates for backward swiming
    if (fStartPosition.Z()*0.01>z_endplane) {
        xx[0]=-xx[0];
        zz[0]=-zz[0];
        uvy[0]=-uvy[0];
    }

    fHitLocation = TVector3(xx[0]*100.0,yy[0]*100.0,zz[0]*100.0);
    fHitDirection = TVector3(uvx[0],uvy[0],uvz[0]);

    return 0;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwBridge* QwRayTracer::GetBridgingInfo() {

    QwBridge* bridgeinfo = new QwBridge();

    bridgeinfo->xOff = fPositionXOff;
    bridgeinfo->yOff = fPositionYOff;

    // and something?

    return bridgeinfo;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwRayTracer::PrintInfo() {

    std::cout<<std::endl<<"   Front/back bridging information"<<std::endl;
    std::cout<<"====================================================================="<<std::endl;
    std::cout<<"swiming from :    location (x,y,z) : ("<<fStartPosition.X()<<", "
    <<fStartPosition.Y()<<", "
    <<fStartPosition.Z()<<")"<<std::endl;

    std::cout<<"                         (R,PHI,Z) : ("<<fStartPositionR<<", "
    <<fStartPositionPhi<<", "
    <<fStartPosition.Z()<<")"<<std::endl;

    std::cout<<"              direction (ux,uy,uz) : ("<<fStartDirection.X()<<", "
    <<fStartDirection.Y()<<", "
    <<fStartDirection.Z()<<")"<<std::endl;

    std::cout<<"                       (theta,phi) : ("<<fStartDirectionTheta<<", "
    <<fStartDirectionPhi<<")"<<std::endl;

    std::cout<<"          to :    location (x,y,z) : ("<<fEndPosition.X()<<", "
    <<fEndPosition.Y()<<", "
    <<fEndPosition.Z()<<")"<<std::endl;

    std::cout<<"                         (R,PHI,Z) : ("<<fEndPositionR<<", "
    <<fEndPositionPhi<<", "
    <<fEndPosition.Z()<<")"<<std::endl;

    std::cout<<"              direction (ux,uy,uz) : ("<<fEndDirection.X()<<", "
    <<fEndDirection.Y()<<", "
    <<fEndDirection.Z()<<")"<<std::endl;

    std::cout<<"                       (theta,phi) : ("<<fEndDirectionTheta<<", "
    <<fEndDirectionPhi<<")"<<std::endl;

    std::cout<<" matched hit :    location (x,y,z) : ("<<fHitLocation.X()<<", "
    <<fHitLocation.Y()<<", "
    <<fHitLocation.Z()<<")"<<std::endl;

    std::cout<<"                         (R,PHI,Z) : ("<<fHitLocationR<<", "
    <<fHitLocationPhi<<", "
    <<fHitLocation.Z()<<")"<<std::endl;

    std::cout<<"              direction (ux,uy,uz) : ("<<fHitDirection.X()<<", "
    <<fHitDirection.Y()<<", "
    <<fHitDirection.Z()<<")"<<std::endl;

    std::cout<<"                       (theta,phi) : ("<<fHitDirectionTheta<<", "
    <<fHitDirectionPhi<<")"<<std::endl;

    std::cout<<"       error :            dR, dPHI : "<<fPositionROff<<", "<<fPositionPhiOff<<" [cm,deg]"<<std::endl;
    std::cout<<"                      dtheta, dphi : "<<fDirectionThetaOff<<", "<<fDirectionPhiOff<<" [deg,deg]"<<std::endl;

    std::cout<<"                          momentum : "<<fMomentum<<" GeV"<<std::endl;
    std::cout<<"====================================================================="<<std::endl<<std::endl;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//
// NOTE Below the relevant part of the function that originally read the tracks
// from the Geant4 MC.  It contains a.o. the smearing parameters.
//
//     //Set start and end point and direction
//     for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane1_NbOfHits && i1 < VSIZE; i1++) {
//         //we don't care about gamma particles now
//         // TODO assign particle type for R2 hits in geant4
//         //if (fRegion2_ChamberFront_WirePlane1_ParticleType.at(i1) != 2)
//         {
//             // Get the position and direction
//             double x = fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX.at(i1);
//             double y = fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY.at(i1);
//             double z = fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ.at(i1);
//             double xMomentum = fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumX.at(i1);
//             double yMomentum = fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumY.at(i1);
//             double zMomentum = fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumZ.at(i1);
//
//             // smear off the positions to ~500 um
//             double sigma = 0.00;  // 0.05 [cm]
//             x = gRandom->Gaus(x,sigma);
//             y = gRandom->Gaus(y,sigma);
//             startposition->push_back( TVector3(x,y,z));
//
//             // smear off direction to ~0.1 degree
//             double momentum = sqrt(xMomentum*xMomentum+yMomentum*yMomentum+zMomentum*zMomentum);
//             double ux = xMomentum/momentum;
//             double uy = yMomentum/momentum;
//             double uz = zMomentum/momentum;
//             double directionTheta = acos(uz)*DEGREE;
//             double directionPhi = atan2(uy,ux)*DEGREE;
//             double sigma_theta = 0.0;
//             double sigma_phi = 0.0;
//             directionTheta = gRandom->Gaus(directionTheta, sigma_theta);
//             directionPhi = gRandom->Gaus(directionPhi, sigma_phi);
//             if (directionPhi<0) directionPhi = directionPhi+360.0;
//
//             ux = sin(directionTheta/DEGREE)*cos(directionPhi/DEGREE);
//             uy = sin(directionTheta/DEGREE)*sin(directionPhi/DEGREE);
//             uz = cos(directionTheta/DEGREE);
//             startdirection->push_back( TVector3(ux,uy,uz));
//         }
//     }
//
//     for (int i1 = 0; i1 < fRegion3_ChamberFront_WirePlaneU_NbOfHits && i1 < VSIZE; i1++) {
//         if (fRegion3_ChamberFront_WirePlaneU_ParticleType.at(i1) != 2) { //we don't care about gamma particles now
//
//             // Get the position and direction
//             double x = fRegion3_ChamberFront_WirePlaneU_GlobalPositionX.at(i1);
//             double y = fRegion3_ChamberFront_WirePlaneU_GlobalPositionY.at(i1);
//             double z = fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ.at(i1);
//             //fEndPosition = TVector3(x,y,z);
//
//             double xMomentum = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.at(i1);
//             double yMomentum = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.at(i1);
//             double zMomentum = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.at(i1);
//
//             // smear off the positions to ~500 um
//             double sigma = 0.00;  // 0.05 [cm]
//             x = gRandom->Gaus(x,sigma);
//             y = gRandom->Gaus(y,sigma);
//             endposition->push_back( TVector3(x,y,z));
//
//             // smear off direction to ~0.1 degree
//             double momentum = sqrt(xMomentum*xMomentum+yMomentum*yMomentum+zMomentum*zMomentum);
//             double ux = xMomentum/momentum;
//             double uy = yMomentum/momentum;
//             double uz = zMomentum/momentum;
//             double directionTheta = acos(uz)*DEGREE;
//             double directionPhi = atan2(uy,ux)*DEGREE;
//             double sigma_theta = 0.0;
//             double sigma_phi = 0.0;
//             directionTheta = gRandom->Gaus(directionTheta, sigma_theta);
//             directionPhi = gRandom->Gaus(directionPhi, sigma_phi);
//             if (directionPhi<0) directionPhi = directionPhi+360.0;
//
//             ux = sin(directionTheta/DEGREE)*cos(directionPhi/DEGREE);
//             uy = sin(directionTheta/DEGREE)*sin(directionPhi/DEGREE);
//             uz = cos(directionTheta/DEGREE);
//             enddirection->push_back( TVector3(ux,uy,uz));
//         }
//     }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwRayTracer::GetBridgingResult(Double_t *buffer) {

    buffer[0] = fStartPosition.X();
    buffer[1] = fStartPosition.Y();
    buffer[2] = fStartPosition.Z();
    buffer[3] = fStartPositionR;
    buffer[4] = fStartPositionPhi;
    buffer[5] = fStartDirection.X();
    buffer[6] = fStartDirection.Y();
    buffer[7] = fStartDirection.Z();
    buffer[8] = fStartDirectionTheta;
    buffer[9] = fStartDirectionPhi;

    buffer[10] = fEndPosition.X();
    buffer[11] = fEndPosition.Y();
    buffer[12] = fEndPosition.Z();
    buffer[13] = fEndPositionR;
    buffer[14] = fEndPositionPhi;
    buffer[15] = fEndDirection.X();
    buffer[16] = fEndDirection.Y();
    buffer[17] = fEndDirection.Z();
    buffer[18] = fEndDirectionTheta;
    buffer[19] = fEndDirectionPhi;

    buffer[20] = fHitLocation.X();
    buffer[21] = fHitLocation.Y();
    buffer[22] = fHitLocation.Z();
    buffer[23] = fHitLocationR;
    buffer[24] = fHitLocationPhi;
    buffer[25] = fHitDirection.X();
    buffer[26] = fHitDirection.Y();
    buffer[27] = fHitDirection.Z();
    buffer[28] = fHitDirectionTheta;
    buffer[29] = fHitDirectionPhi;

    buffer[30] = fPositionROff;
    buffer[31] = fPositionPhiOff;
    buffer[32] = fDirectionThetaOff;
    buffer[33] = fDirectionPhiOff;

    // jpan: fMomentum is the determined momentum on the z=-250 cm plane.
    // In order to get the scattered momentum (P') inside the target,
    // we should use MC simulation to get the energy loss from scattering
    // vertex to z=-250 cm plane as a correction. This correction should
    // be added onto P' to get the final determined momentum.
    // When the correct scattering angle is determined, it could be used for
    // this purpose.

    double momentum_correction = 0.0; //0.032;  // assume 32 MeV with multi-scattering, etc.
    double PP = fMomentum + momentum_correction;
    buffer[34] = PP;

    double Mp = 0.938272013;    // Mass of the Proton
    double P0 = Mp*PP/(Mp-PP*(1-cos(fStartDirectionTheta/DEGREE))); //pre-scattering energy
    double Q2 = 2.0*Mp*(P0-PP);
    buffer[35] = P0;
    buffer[36] = Q2;

// NOTE Had to comment the fPrimary variables as they are not read by anymore
//     buffer[37] = fPrimary_OriginVertexKineticEnergy;
//     buffer[38] = fPrimary_PrimaryQ2;

//     double MomentumOff = PP-fPrimary_OriginVertexKineticEnergy;
//     double Q2_Off = Q2-fPrimary_PrimaryQ2;
//     buffer[39] = MomentumOff;
//     buffer[40] = Q2_Off;
//     buffer[41] = fPrimary_CrossSectionWeight;

    buffer[42] = fMatchFlag;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
