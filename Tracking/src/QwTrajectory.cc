
/*! \file   QwTrajectory.cc
 *  \author J. Pan
 *  \date   Thu Nov 26 12:27:10 CST 2009
 *  \brief  Raytrace in magnetic field to bridging R2/R3 partial tracks.
 *
 *  \ingroup QwTracking
 *
 *   Firstly, unresonable bridging candidates will be filtered out, others
 *   which can pass through the filter will be matched in a look-up table.
 *   If there is a match, the momentum will be determined by interpolation.
 *   In case there is not a match in the look-up table, the bridging candidates
 *   will then be sent into a shooting routine for bridging and momentum
 *   determination. The Newton-Raphson method is used in the shooting routine,
 *   and the 4'th order Runge-Kutta method is used for tntegrating the equations
 *   of motion for electrons in the QTOR magnetic field.
 *
 */

// jpan: This is a simple bridging routine. It works for simulated event now.
// More works need to be done on the restruction part of the tracking code
// to provide resonable bridging candidates.


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include <cstdlib>

#include "QwMagneticField.h"
#include "QwBridge.h"
#include "QwTrajectory.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#define VSIZE 100

//#define TESTEVENT
//#define BACKSWIM

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrajectory::QwTrajectory() {

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

    fBfield = NULL;

    fSimFlag = 0;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrajectory::~QwTrajectory() {

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrajectory::LoadMagneticFieldMap() {

    fBfield = new QwMagneticField();
    fBfield->ReadFieldMapFile(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/MainMagnet_FieldMap.dat");

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrajectory::SetStartAndEndPoints(TVector3 startposition, TVector3 startdirection,
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

int QwTrajectory::BridgeFrontBackPartialTrack() {

    int status;

    fMatchFlag = -2;
    status = Filter();
    if (status == -1)  return -1;

    status = SearchTable();
    if (status == -1)  status = Shooting();

    // status = Shooting();

    // if (status == -1)  status = DoForcedBridging();

    return status;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// The angle limits will be determined from MC simulation results
int QwTrajectory::Filter() {

    // swimming direction limit
    if (fStartPosition.Z()>=fEndPosition.Z()) {
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
    double dphi = (fEndDirectionPhi - fStartDirectionPhi);
    if (dtheta<5.0 || dtheta>25.0 || fabs(dphi)>10.0) {
        std::cerr<<"Filter: bending angles (dtheta="<<dtheta<<" degree, dphi="<<dphi
        <<" degree) in B field are out of range"<<std::endl;
        fMatchFlag = -1;
        return -1;
    }

    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrajectory::SearchTable() {

    // front track position and angles at z= -250 cm plane
    double r = fabs(fStartPosition.Z()-(-250.0))/fStartDirection.Z();
    double x = fStartPosition.X() + r*fStartDirection.X();
    double y = fStartPosition.Y() + r*fStartDirection.Y();

    double position_r = sqrt(x*x+y*y);
    double position_phi = fStartPositionPhi;
    double direction_theta = fStartDirectionTheta;

    double vertex_z = -250.0 - position_r/tan(acos(fStartDirection.Z()));

    // expected hit position and angles at z= +570 cm plane
    r = fabs(fEndPosition.Z()-570.0)/fEndDirection.Z();
    x = fEndPosition.X() + r*fEndDirection.X();
    y = fEndPosition.Y() + r*fEndDirection.Y();

    double expectedhitposition_r = sqrt(x*x+y*y);
    double expectedhitposition_phi = atan2(y,x)*DEGREE;
    if (expectedhitposition_phi<0.0)
        expectedhitposition_phi = expectedhitposition_phi + 360.0;

    bool isintable =   ((int)position_r >= R_MIN) && ((int)position_r < R_MAX) &&
                       ((int)position_phi >= PHI_MIN) && ((int)position_phi <PHI_MAX) &&
                       ((int)vertex_z > VERTEXZ_MIN) && ((int)vertex_z < VERTEXZ_MAX);

    // search the table
    if ( ! isintable ) {
        std::cout<<"vertex_z="<<vertex_z<<std::endl;
        std::cout<<"position_r="<<position_r<<std::endl;
        std::cout<<"position_phi="<<position_phi<<std::endl;
        std::cout<<"direction_theta="<<direction_theta<<std::endl;
        std::cout<<"This potential track is not listed in the table."<<std::endl;
        return -1;
    }

    // find the index of the neighbour tracks
    int index_pos_r = ((int)(position_r+0.5)-R_MIN)/DR;
    int index_pos_phi = ((int)(position_phi+0.5)-PHI_MIN)/DPHI;
    int index_vertex_z = ((int)(vertex_z+0.5)-VERTEXZ_MIN)/DZ;

    // build two subsets of the table
    std::vector <QwPartialTrackParameter> backtrackparametersublist;
    QwPartialTrackParameter backtrackparameter;
    double *iP = new double[P_MAX-P_MIN+1];  //hold momentum for inteprolation
    double *iR = new double[P_MAX-P_MIN+1];  //hold radial position for inteprolation

    // NOTE jpan: ROOT::Math::GSLInterpolator::Eval(double) requires that
    // x values must be monotonically increasing.

    for (int p = P_MIN; p<=P_MAX; p+=DP) {
        // build index
        int index_momentum = ((int)p-P_MIN)/DP;
        unsigned int index = index_momentum*R_GRIDSIZE*PHI_GRIDSIZE*Z_GRIDSIZE
                     + index_pos_r*PHI_GRIDSIZE*Z_GRIDSIZE
                     + index_pos_phi*Z_GRIDSIZE
                     + index_vertex_z ;

        // keep iR monotonically increasing with the index
        int ind = (P_MAX-P_MIN)/DP-index_momentum;

        backtrackparameter = fBackTrackParameterTable.at(index);

        iP[ind] = ((double)p)/1000.0;
        iR[ind] = backtrackparameter.fPositionR;
        backtrackparametersublist.push_back( backtrackparameter );
        //std::cout<<"ind, iP, iR:   "<<ind<<", "<<iP[ind]<<", "<<iR[ind]<<std::endl;
    }

    //NOTE The dr/dp on focal plane is about 1 cm per 10 MeV

    if ( (backtrackparametersublist.front().fPositionR  < expectedhitposition_r) &&
            (backtrackparametersublist.back().fPositionR   > expectedhitposition_r) ) {
        std::cout<<"No match in look-up table!"<<std::endl;
        return -1;
    }

    // the hit is within the momentum limits, do interpolation for momentum

    // We can choose among the following methods:
    // CSPLINE, LINEAR, POLYNOMIAL,
    // CSPLINE_PERIODIC, AKIMA, AKIMA_PERIODIC
    UInt_t size = backtrackparametersublist.size();

#if defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)

#if ROOT_VERSION_CODE >= ROOT_VERSION(5,22,0)
    // Newer version of the MathMore plugin use kPOLYNOMIAL
    ROOT::Math::Interpolator inter(size, ROOT::Math::Interpolation::kPOLYNOMIAL);
#else
    // Older version of the MathMore plugin use POLYNOMIAL
    ROOT::Math::Interpolator inter(size, ROOT::Math::Interpolation::POLYNOMIAL);
#endif

    inter.SetData(size, iR, iP);
    fMomentum = inter.Eval(expectedhitposition_r);  // + 0.0016;  // [GeV]

    delete iP;
    delete iR;

    // take the track parameter from the nearest track
    if (fMomentum<0.98 || fMomentum>1.165) {
        std::cerr<<"Out of momentum range: determined momentum by looking up table: "
        << fMomentum<<" GeV"<<std::endl;
        return -1;
    }

    backtrackparameter = backtrackparametersublist.at(((int)(fMomentum*1000.0+0.5)-P_MIN)/DP);

    // set matching info
    double sin_theta = sin(backtrackparameter.fDirectionTheta/DEGREE);
    double cos_theta = cos(backtrackparameter.fDirectionTheta/DEGREE);
    double sin_phi = sin(backtrackparameter.fDirectionPhi/DEGREE);
    double cos_phi = cos(backtrackparameter.fDirectionPhi/DEGREE);
    double sin_PHI = sin(backtrackparameter.fPositionPhi/DEGREE);
    double cos_PHI = cos(backtrackparameter.fPositionPhi/DEGREE);

    fHitLocation = TVector3(backtrackparameter.fPositionR*cos_PHI,
                            backtrackparameter.fPositionR*sin_PHI,
                            570.0);

    fHitDirection = TVector3(sin_theta*cos_phi,sin_theta*sin_phi,cos_theta);

    // extend to z = fEndPosition.Z() plane
    r = (fEndPosition.Z()-570.0)/fHitDirection.Z();
    x = fHitLocation.X() + r*fHitDirection.X();
    y = fHitLocation.Y() + r*fHitDirection.Y();
    fHitLocation = TVector3(x,y,fEndPosition.Z());
    fHitLocationR = sqrt(x*x+y*y);
    fHitLocationPhi = atan2(y,x)*DEGREE;
    if (fHitLocationPhi<0.0)  fHitLocationPhi += 360.0;
    fHitLocation = TVector3(x,y,fEndPosition.Z());

    fHitDirectionTheta = acos(fHitDirection.Z())*DEGREE;
    fHitDirectionPhi = atan2(fHitDirection.Y(),fHitDirection.X())*DEGREE;
    if (fHitDirectionPhi<0.0) fHitDirectionPhi+=360.0;

    // errors
    fPositionROff = fHitLocationR - fEndPositionR;
    fPositionPhiOff = fHitLocationPhi - fEndPositionPhi;
    fDirectionThetaOff = fHitDirectionTheta - fEndDirectionTheta;
    fDirectionPhiOff = fHitDirectionPhi - fEndDirectionPhi;

    fPositionXOff = fHitLocation.X() - fEndPosition.X();
    fPositionYOff = fHitLocation.Y() - fEndPosition.Y();

    fDirectionXOff = fHitDirection.X() - fEndDirection.X();
    fDirectionYOff = fHitDirection.Y() - fEndDirection.Y();
    fDirectionZOff = fHitDirection.Z() - fEndDirection.Z();

    if ( fabs(fPositionROff)<2.0 && fabs(fPositionPhiOff)<4.0 &&
            fabs(fDirectionThetaOff)<1.0 && fabs(fDirectionPhiOff)<4.0 ) {

        //std::cout<<"======>>>>> Found a good match in the look-up table"<<std::endl;
        fMatchFlag = 0;
        return 0;
    } else {
        std::cout<<"Didn't find a good match in the look-up table"<<std::endl;
        return -1;
    }

#else // defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)

    // No support for ROOT MathMore: warn user and return failure
    std::cout << "No support for QwTrajectory interpolation." << std::endl;
    std::cout << "Please install the ROOT MathMore plugin and recompile." << std::endl;
    return -1;

#endif // defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrajectory::Shooting() {

    double res = 1.0; //position determination [cm]
    double step_size = 1.0; // integration step size [cm]
    double dp = 0.01; //momentum variation [GeV]

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

        //TODO - need to take into account of the energy loss along the trajectories, assume ~7 MeV for now.
        //     - need slope matching as well

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
            fMomentum = p[1]; // + 0.007;
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

double QwTrajectory::EstimateInitialMomentum(TVector3 direction) {

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

int QwTrajectory::Integrate(double e0, double step) {

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
        fBfield->GetFieldValue(point, bfield, BSCALE);
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
        fBfield->GetFieldValue(point, bfield, BSCALE);
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
        fBfield->GetFieldValue(point, bfield, BSCALE);
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
        fBfield->GetFieldValue(point, bfield, BSCALE);
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

int QwTrajectory::LoadMomentumMatrix() {

    Int_t   index;
    Double_t position_r,position_phi;
    Double_t direction_theta,direction_phi;

    TVector3 startpoint,startdirection,endpoint,enddirection;

    //open file
    TString rootfilename=std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QwTrajMatrix.root";

    if (gSystem->AccessPathName(rootfilename)) {
        std::cerr <<std::endl<< "Cannot find the momentum look-up table!"<<std::endl;
        std::cerr<<"Run qwtrajmatrixgenerator to generate one."<<std::endl<<std::endl;
        return 0;
    }

    TFile* rootfile = new TFile(rootfilename,"read");
    if (! rootfile) return 0;
    rootfile->cd();

    TTree *momentum_tree = (TTree *)rootfile->Get("Momentum_Tree");

    std::cout<<"Read data from look-up table"<<std::endl;

    momentum_tree->SetBranchAddress("index",&index);
    momentum_tree->SetBranchAddress("position_r", &position_r);
    momentum_tree->SetBranchAddress("position_phi", &position_phi);
    momentum_tree->SetBranchAddress("direction_theta",&direction_theta);
    momentum_tree->SetBranchAddress("direction_phi",&direction_phi);

    Int_t numberOfEntries = momentum_tree->GetEntries();

    QwPartialTrackParameter backtrackparameter;
    fBackTrackParameterTable.reserve(numberOfEntries);

    std::cout<<"total grid points : "<<numberOfEntries<<std::endl;

    for ( Int_t i=0; i<numberOfEntries; i++) {
        momentum_tree->GetEntry(i);

        // only z=+570 cm plane data
        backtrackparameter.fPositionR = (float)position_r;
        backtrackparameter.fPositionPhi = (float)position_phi;
        backtrackparameter.fDirectionTheta = (float)direction_theta;
        backtrackparameter.fDirectionPhi = (float)direction_phi;

        fBackTrackParameterTable.push_back(backtrackparameter);
    }

    std::cout<<"...done."<<std::endl;
    rootfile->Close();
    delete rootfile;
    return 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwBridge* QwTrajectory::GetBridgingInfo() {

    QwBridge* bridgeinfo = new QwBridge();

    bridgeinfo->xOff = fPositionXOff;
    bridgeinfo->yOff = fPositionYOff;

    // and something?

    return bridgeinfo;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrajectory::PrintInfo() {

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

void QwTrajectory::GenerateLookUpTable() {

    UInt_t    gridnum;
    UInt_t    index;
    UInt_t    ind_p,ind_r,ind_phi,ind_z;

    Int_t  p;
    Int_t  r;
    Int_t  phi;
    Int_t  vertex_z;

    Double_t  theta;
    Double_t  x[3],y[3],z[3],ux[3],uy[3],uz[3];

    Double_t  position_r,position_phi;   //z=570 cm plane
    Double_t  direction_theta,direction_phi;

    Double_t  step_size = 1.0; // [cm]
    TVector3  startpoint,startdirection,endpoint,enddirection;

    z[0] = -250.0; // [cm]
    z[1] =  250.0; // [cm]
    z[2] =  570.0; // [cm]

    UInt_t grid_size = P_GRIDSIZE*Z_GRIDSIZE*R_GRIDSIZE*PHI_GRIDSIZE;

    //open file and set up output tree
    TString rootfilename=std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QwTrajMatrix.root";
    TFile* rootfile = new TFile(rootfilename,"RECREATE","Qweak momentum matrix");
    rootfile->cd();

    TTree *momentum_tree = new TTree("Momentum_Tree","momentum data tree");

    // This table is linearly indexed with 1D index
    momentum_tree->Branch("index",&index,"index/I");

// position and direction at focal plane
    momentum_tree->Branch("position_r", &position_r, "position_r/D");
    momentum_tree->Branch("position_phi", &position_phi, "position_phi/D");
    momentum_tree->Branch("direction_theta",&direction_theta,"direction_theta/D");
    momentum_tree->Branch("direction_phi",&direction_phi,"direction_phi/D");

    std::cout<<"Start to generate trajectory data, total grid size: "<<grid_size<<std::endl;

    // Create a timer
    TStopwatch timer;
    timer.Start();

    // calculate traj's, the loop sequence should be kept as
    // p_loop{  r_loop{ phi_loop{ vertex_z loop{} } } } to
    // make sure the table is indexed correctly

    gridnum = 0;

    for (p=P_MIN; p<=P_MAX; p+=DP) {
        for (r=R_MIN;r<=R_MAX; r+=DR) {      // r and phi are specified on z=-250 cm plane
            for (phi=PHI_MIN;phi<=PHI_MAX; phi+=DPHI) {
                for (vertex_z=VERTEXZ_MIN; vertex_z<=VERTEXZ_MAX; vertex_z+=DZ) {

                    ind_p = (p-P_MIN)/DP;
                    ind_r = (r-R_MIN)/DR;
                    ind_phi = (phi-PHI_MIN)/DPHI;
                    ind_z = (vertex_z-VERTEXZ_MIN)/DZ;
                    index = ind_p*R_GRIDSIZE*PHI_GRIDSIZE*Z_GRIDSIZE +
                            ind_r*PHI_GRIDSIZE*Z_GRIDSIZE +
                            ind_phi*Z_GRIDSIZE +
                            ind_z;

                    //intersection position and direction with the z=-250 cm plane
                    theta = atan2(r,(-250.0-vertex_z))*DEGREE;

                    ux[0] = sin(theta/DEGREE)*cos((double)phi/DEGREE);
                    uy[0] = sin(theta/DEGREE)*sin((double)phi/DEGREE);
                    uz[0] = cos(theta/DEGREE);
                    startdirection = TVector3(ux[0],uy[0],uz[0]);

                    x[0] = (double)r*cos((double)phi/DEGREE);
                    y[0] = (double)r*sin((double)phi/DEGREE);
                    startpoint = TVector3(x[0],y[0],z[0]);

                    enddirection = TVector3(0.0,0.0,0.0);
                    endpoint = TVector3(0.0,0.0,z[1]);

                    SetStartAndEndPoints(startpoint, startdirection,
                                         endpoint, enddirection);

                    //raytrace from startplane (z=-250 cm) to endplane (z=+250 cm)
                    // p in [GeV] for the integration
                    Integrate((double)p*0.001, step_size);
                    x[1] = GetHitLocationX();
                    y[1] = GetHitLocationY();
                    ux[1] = GetHitDirectionX();
                    uy[1] = GetHitDirectionY();
                    uz[1] = GetHitDirectionZ();

                    // Extending the back stright tracks onto z=+570 cm focal plane
                    double R = (z[2]-z[1])/uz[1];
                    x[2] = x[1] + R*ux[2];
                    y[2] = y[1] + R*uy[2];
                    ux[2] = ux[1];
                    uy[2] = uy[1];
                    uz[2] = uz[1];

                    position_r = sqrt(x[2]*x[2]+y[2]*y[2]);
                    position_phi = atan2(y[2],x[2])*DEGREE;
                    if (position_phi<0.0) position_phi = position_phi+360.0;
                    direction_theta = acos(uz[2])*DEGREE;
                    direction_phi = atan2(uy[2],ux[2])*DEGREE;
                    if (direction_phi<0.0) direction_phi = direction_phi+360.0;

                    gridnum++;

                    //fill tree
                    momentum_tree->Fill();

                    if (gridnum % 1000 == 0)   {
                        std::cout<<"."<<std::flush;
                        if ((gridnum % 100000) == 0 || gridnum == grid_size)   {
                            std::cout<<100*gridnum/grid_size<<"%"<<std::flush;
                            momentum_tree->AutoSave();
                            gDirectory->Purge();
                        }
                    }
                } //end of vertex_z loop
            } //end of phi loop
        } //end of r loop
    } //end of p loop

    timer.Stop();

    std::cout<<"  done."<<std::endl<<"generated "<<gridnum<<" trajectories."<<std::endl;

    std::cout<<"CPU time used:  " << timer.CpuTime() << " s "
    << "(" << timer.CpuTime() / gridnum << " s per trajectory)" << std::endl
    << "Real time used: " << timer.RealTime() << " s "
    << "(" << timer.RealTime() / gridnum << " s per trajectory)" << std::endl;

    rootfile->Write(0, TObject::kOverwrite);
    rootfile->Close();

    delete rootfile;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// ReadSimPartialTrack access the disk and read a single event from the geant4 simulated
// root file for each call, and set the start/end positions and directions. There is no
// buffer for the event. The effiency is low in this way, but ok for test purpose.

int QwTrajectory::ReadSimPartialTrack(const TString filename, int evtnum,
                                      std::vector<TVector3> *startposition,
                                      std::vector<TVector3> *startdirection,
                                      std::vector<TVector3> *endposition,
                                      std::vector<TVector3> *enddirection) {

    if (fSimFlag == 0) {
        //initialize tree

        fSimFlag = 1;
    }

    // Get an entry from the tree
    // Allocate memory and initialize them
    // Reserve Space

    // Region2 WirePlane1
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX.reserve(VSIZE);
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY.reserve(VSIZE);
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ.reserve(VSIZE);
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumX.reserve(VSIZE);
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumY.reserve(VSIZE);
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumZ.reserve(VSIZE);

    // Region3 WirePlaneU
    fRegion3_ChamberFront_WirePlaneU_GlobalPositionX.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneU_GlobalPositionY.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.reserve(VSIZE);
    fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.reserve(VSIZE);

    //Clear

    // Region2 WirePlane1
    fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit = 0;
    fRegion2_ChamberFront_WirePlane1_NbOfHits = 0;
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX.clear();
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY.clear();
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ.clear();
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumX.clear();
    fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumY.clear();

    // Region3 WirePlaneU
    fRegion3_ChamberFront_WirePlaneU_HasBeenHit = 0;
    fRegion3_ChamberFront_WirePlaneU_NbOfHits = 0;
    fRegion3_ChamberFront_WirePlaneU_GlobalPositionX.clear();
    fRegion3_ChamberFront_WirePlaneU_GlobalPositionY.clear();
    fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ.clear();
    fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.clear();
    fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.clear();

    fPrimary_OriginVertexKineticEnergy = 0.0;
    fPrimary_CrossSectionWeight = 0.0;
    //fMomentumOff = 0.0;

    // Open ROOT file
    TFile *simrootfile = new TFile (filename);
    TTree *g4simtree = (TTree*) simrootfile->Get("QweakSimG4_Tree");
    g4simtree->SetMakeClass(1);

    int totalentries = g4simtree->GetEntries();
    if (evtnum>=totalentries) {
        delete g4simtree;
        delete simrootfile;
        std::cerr<<"Out of range: Event number "<<evtnum<<" >= Total events "<<totalentries<<std::endl;
        return -1;
    }

    // Attach to region 1 branches
    // WirePlane1
    g4simtree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneHasBeenHit",
                                &fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit);
    g4simtree->SetBranchAddress("Region2.ChamberFront.WirePlane1.NbOfHits",
                                &fRegion2_ChamberFront_WirePlane1_NbOfHits);
    g4simtree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalPositionX",
                                &fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX);
    g4simtree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalPositionY",
                                &fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY);
    g4simtree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalPositionZ",
                                &fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ);
    g4simtree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalMomentumX",
                                &fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumX);
    g4simtree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalMomentumY",
                                &fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumY);
    g4simtree->SetBranchAddress("Region2.ChamberFront.WirePlane1.PlaneGlobalMomentumZ",
                                &fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumZ);

    // Attach to region 3 branches
    g4simtree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.HasBeenHit",
                                &fRegion3_ChamberFront_WirePlaneU_HasBeenHit);
    g4simtree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.NbOfHits",
                                &fRegion3_ChamberFront_WirePlaneU_NbOfHits);
    g4simtree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.ParticleType",
                                &fRegion3_ChamberFront_WirePlaneU_ParticleType);
    g4simtree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalPositionX",
                                &fRegion3_ChamberFront_WirePlaneU_GlobalPositionX);
    g4simtree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalPositionY",
                                &fRegion3_ChamberFront_WirePlaneU_GlobalPositionY);
    g4simtree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalPositionZ",
                                &fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ);
    g4simtree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalMomentumX",
                                &fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX);
    g4simtree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalMomentumY",
                                &fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY);
    g4simtree->SetBranchAddress("Region3.ChamberFront.WirePlaneU.GlobalMomentumZ",
                                &fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ);

    g4simtree->SetBranchAddress("Primary.OriginVertexKineticEnergy",
                                &fPrimary_OriginVertexKineticEnergy);
    g4simtree->SetBranchAddress("Primary.PrimaryQ2",
                                &fPrimary_PrimaryQ2);
    g4simtree->SetBranchAddress("Primary.CrossSectionWeight",
                                &fPrimary_CrossSectionWeight);


    g4simtree->GetBranch("Region2.ChamberFront.WirePlane1.PlaneHasBeenHit")->GetEntry(evtnum);
    g4simtree->GetBranch("Region3.ChamberFront.WirePlaneU.HasBeenHit")->GetEntry(evtnum);

    if (fRegion2_ChamberFront_WirePlane1_PlaneHasBeenHit == 5 &&
            fRegion3_ChamberFront_WirePlaneU_HasBeenHit == 5) {
        g4simtree->GetEntry(evtnum);
    } else {
        std::cerr << "Reading G4Sim Event: skip an empty event - event#" << evtnum << std::endl;
    }

    // In case of taking the first hit only, let
    //fRegion2_ChamberFront_WirePlane1_NbOfHits = 1;
    //fRegion3_ChamberFront_WirePlaneU_NbOfHits = 1;

    //Set start and end point and direction
    for (int i1 = 0; i1 < fRegion2_ChamberFront_WirePlane1_NbOfHits && i1 < VSIZE; i1++) {

        // Get the position and direction
        double x = fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionX.at(i1);
        double y = fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionY.at(i1);
        double z = fRegion2_ChamberFront_WirePlane1_PlaneGlobalPositionZ.at(i1);
        double xMomentum = fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumX.at(i1);
        double yMomentum = fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumY.at(i1);
        double zMomentum = fRegion2_ChamberFront_WirePlane1_PlaneGlobalMomentumZ.at(i1);

        // smear off the positions to ~500 um
        double sigma = 0.05;  // 0.05 [cm]
        x = gRandom->Gaus(x,sigma);
        y = gRandom->Gaus(y,sigma);
        startposition->push_back( TVector3(x,y,z));

        // smear off direction to ~0.1 degree
        double momentum = sqrt(xMomentum*xMomentum+yMomentum*yMomentum+zMomentum*zMomentum);
        double ux = xMomentum/momentum;
        double uy = yMomentum/momentum;
        double uz = zMomentum/momentum;
        double directionTheta = acos(uz)*DEGREE;
        double directionPhi = atan2(uy,ux)*DEGREE;
        double sigma_theta = 0.1;
        double sigma_phi = 0.1;
        directionTheta = gRandom->Gaus(directionTheta, sigma_theta);
        directionPhi = gRandom->Gaus(directionPhi, sigma_phi);
        if (directionPhi<0) directionPhi = directionPhi+360.0;

        ux = sin(directionTheta/DEGREE)*cos(directionPhi/DEGREE);
        uy = sin(directionTheta/DEGREE)*sin(directionPhi/DEGREE);
        uz = cos(directionTheta/DEGREE);
        startdirection->push_back( TVector3(ux,uy,uz));
    }

    for (int i1 = 0; i1 < fRegion3_ChamberFront_WirePlaneU_NbOfHits && i1 < VSIZE; i1++) {
        if (fRegion3_ChamberFront_WirePlaneU_ParticleType.at(i1) != 2) { //we don't care about gamma particles now

            // Get the position and direction
            double x = fRegion3_ChamberFront_WirePlaneU_GlobalPositionX.at(i1);
            double y = fRegion3_ChamberFront_WirePlaneU_GlobalPositionY.at(i1);
            double z = fRegion3_ChamberFront_WirePlaneU_GlobalPositionZ.at(i1);
            //fEndPosition = TVector3(x,y,z);

            double xMomentum = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumX.at(i1);
            double yMomentum = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumY.at(i1);
            double zMomentum = fRegion3_ChamberFront_WirePlaneU_GlobalMomentumZ.at(i1);

            // smear off the positions to ~500 um
            double sigma = 0.05;  // 0.05 [cm]
            x = gRandom->Gaus(x,sigma);
            y = gRandom->Gaus(y,sigma);
            endposition->push_back( TVector3(x,y,z));

            // smear off direction to ~0.1 degree
            double momentum = sqrt(xMomentum*xMomentum+yMomentum*yMomentum+zMomentum*zMomentum);
            double ux = xMomentum/momentum;
            double uy = yMomentum/momentum;
            double uz = zMomentum/momentum;
            double directionTheta = acos(uz)*DEGREE;
            double directionPhi = atan2(uy,ux)*DEGREE;
            double sigma_theta = 0.1;
            double sigma_phi = 0.1;
            directionTheta = gRandom->Gaus(directionTheta, sigma_theta);
            directionPhi = gRandom->Gaus(directionPhi, sigma_phi);
            if (directionPhi<0) directionPhi = directionPhi+360.0;

            ux = sin(directionTheta/DEGREE)*cos(directionPhi/DEGREE);
            uy = sin(directionTheta/DEGREE)*sin(directionPhi/DEGREE);
            uz = cos(directionTheta/DEGREE);
            enddirection->push_back( TVector3(ux,uy,uz));
        }
    }
    fPrimary_OriginVertexKineticEnergy = fPrimary_OriginVertexKineticEnergy*0.001;
    //std::cout<<"origin vertex momentum: "<<fPrimary_OriginVertexKineticEnergy<<std::endl;

    delete g4simtree;
    delete simrootfile;
    return 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrajectory::GetBridgingResult(Double_t *buffer) {

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
    buffer[34] = fMomentum;

    double Mp = 0.938272013;    // Mass of the Proton
    double P0 = Mp*fMomentum/(Mp-fMomentum*(1-cos(fStartDirectionTheta/DEGREE))); //pre-scattering energy
    double Q2 = 2.0*Mp*(P0-fMomentum);
    buffer[35] = P0;
    buffer[36] = Q2;

    buffer[37] = fPrimary_OriginVertexKineticEnergy;
    buffer[38] = fPrimary_PrimaryQ2;

    double MomentumOff = fMomentum-fPrimary_OriginVertexKineticEnergy;
    double Q2_Off = Q2-fPrimary_PrimaryQ2;
    buffer[39] = MomentumOff;
    buffer[40] = Q2_Off;
    buffer[41] = fPrimary_CrossSectionWeight;
    buffer[42] = fMatchFlag;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
