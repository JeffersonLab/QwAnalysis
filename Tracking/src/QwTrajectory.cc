
/*! \file   QwTrajectory.cc
 *  \author J. Pan
 *  \date   Thu Nov 26 12:27:10 CST 2009
 *  \brief  Raytrace in magnetic field to bridging R2/R3 partial tracks.
 *
 *  \ingroup QwTracking
 *
 *   Integrating the equations of motion for electrons in the QTOR.
 *   The 4'th order Runge-Kutta method is used.
 *
 *   The Newton-Raphson method is used to solve for the momentum of the track.
 *
 */

// jpan: This is a simple bridging routine. It works for simulated event now.
// More works need to be done on the restruction part of the tracking code
// to provide resonable bridging candidates.


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include <cstdlib>

#include "QwMagneticField.h"
#include "QwTrajectory.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#define TESTEVENT
//#define BACKSWIM

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrajectory::QwTrajectory() {

    for (int i=0;i<2;i++) {
        fPositionX[i]=0.0;     // position
        fPositionY[i]=0.0;
        fPositionZ[i]=0.0;
        fDirectionX[i] = 0.0;  // unit vector
        fDirectionY[i] = 0.0;
        fDirectionZ[i] = 0.0;
    }

    fBdlx = 0.0;
    fBdly = 0.0;
    fBdlz = 0.0;

    LoadMomentumMatrix();
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

int QwTrajectory::BridgeFrontBackPartialTrack(TVector3 startpoint,
                                              TVector3 startpointdirection,
                                              TVector3 endpoint,
                                              TVector3 endpointdirection) {

    int status;
    SetMagneticField(fBfield);
    status = Filter(startpoint,startpointdirection,endpoint,endpointdirection);

#ifdef TESTEVENT
//test code: shoot an electron from target with ~8 deg scattering angle
    //startpoint = TVector3(0.0,0.0,-650.0);
    //direction = TVector3(0.0,sin(8.0*3.1415927/180.0),cos(8.0*3.1415927/180.0));
    //endpoint = TVector3(2.38, 322.6, 571.1);

//This single ideal test event is chosen from G4 simulation with momentum=1.148817139
// endpoint direction is calculated from hit in trigger scintillator and C-bar
    std::cout<<std::endl<<"Test the bridging code with an ideal MC event:"<<std::endl;

    double testUx = 71.882591-70.384743;
    double testUy = 328.381226-322.522278;
    double testUz = 570.717773-555.592896;
    double testR = sqrt(testUx*testUx+testUy*testUy+testUz*testUz);
    endpointdirection = TVector3(testUx/testR,testUy/testR,testUz/testR);

    testUx = 29.548246; //momentum component of the hit at R2 back chamber wireplane 6
    testUy = 170.25393;
    testUz = 1136.2637;
    testR = sqrt(testUx*testUx+testUy*testUy+testUz*testUz);
    startpointdirection = TVector3(testUx/testR,testUy/testR,testUz/testR);

    endpoint = TVector3(71.882591,328.381226,570.717773);    // hit position on Cerenkov bar
    startpoint = TVector3(9.0694189,53.198646,-280.2352);   //hit position on R2 back chamber

#ifdef BACKSWIM
    // backward swiming
    TVector3 swap = startpoint;
    startpoint = endpoint;
    endpoint = swap;
    swap = startpointdirection;
    startpointdirection = endpointdirection;
    endpointdirection = swap;
    //end of defination of backward swiming
#endif

    std::cout<<"swiming from : ("<<startpoint.X()<<","<<startpoint.Y()<<","<<startpoint.Z()<<")   "
    <<"direction ("<<startpointdirection.X()<<","<<startpointdirection.Y()<<","<<startpointdirection.Z()<<")  "
    <<"with momentum "<<testR*0.001<<" GeV"<<std::endl;
    std::cout<<"expected hit at : ("<<endpoint.X()<<","<<endpoint.Y()<<","<<endpoint.Z()<<")   "
    <<"direction ("<<endpointdirection.X()<<", "<<endpointdirection.Y()<<", "<<endpointdirection.Z()<<")"<<std::endl;
    status = 0;
#endif
//end of test event

    if (status == 0)
        status = Shooting(startpoint,startpointdirection,endpoint,endpointdirection);
    if (status == 0) return 0;
    else return -1;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrajectory::Shooting(TVector3 startpoint, TVector3 startpointdirection,
                           TVector3 endpoint, TVector3 endpointdirection) {

    double res = 1.0; //position determination [cm]
    double step_size = 0.2; // integration step size [cm]
    double dp = 0.05; //momentum variation [GeV]

    double p[2],x[2],y[2],r[2],xh,yh,rh;
    TVector3 localendpoint;

    if (startpoint.Z()<=endpoint.Z())
        p[0] = EstimateInitialMomentum(startpointdirection);
    else
        p[0] = EstimateInitialMomentum(endpointdirection);

    std::cout<<"estimated momentum: "<<p[0]<<std::endl;

    xh = endpoint.X();
    yh = endpoint.Y();
    rh = sqrt(xh*xh+yh*yh);

    int n;
    for (n=1; n<=MAX_ITERATION; n++) {

        localendpoint = Integrate(p[0]-dp, startpoint, startpointdirection, step_size, endpoint.Z());
        x[0]=localendpoint.X();
        y[0]=localendpoint.Y();

        localendpoint = Integrate(p[0]+dp, startpoint, startpointdirection, step_size, endpoint.Z());
        x[1]=localendpoint.X();
        y[1]=localendpoint.Y();

        r[0] = sqrt(x[0]*x[0]+y[0]*y[0]);
        r[1] = sqrt(x[1]*x[1]+y[1]*y[1]);

        p[1] = p[0] -0.5*dp*(r[0]+r[1]-2.0*rh)/(r[1]-r[0]);

        //std::cout<<"momentum: p0, p1:  "<<p[0]<<", "<<p[1]<<std::endl;
        //std::cout<<"hit location: x0, y0, x1, y1, r0, r1:  "<<x[0]<<", "<<y[0]
        //         <<", "<<x[1]<<", "<<y[1]<<", "<<r[0]<<", "<<r[1]<<std::endl;

        localendpoint = Integrate(p[1], startpoint, startpointdirection, step_size, endpoint.Z());
        x[0]=localendpoint.X();
        y[0]=localendpoint.Y();
        r[0] = sqrt(x[0]*x[0]+y[0]*y[0]);

        //TODO - need to take into account of the energy loss along the trajectories, assume ~2 MeV for now.
        //     - need slope matching as well

        double dx = x[0]-xh;
        double dy = y[0]-yh;
        double dr = sqrt(dx*dx+dy*dy);
        if ( dr<res) {
            if (startpoint.Z()<=endpoint.Z())
                fMomentum = p[1]+0.002;
            else
                fMomentum = p[1]-0.002;
            std::cout<<"Converged after "<<n<<" iterations at ("
            <<localendpoint.X()<<", "<<localendpoint.Y()<<", "<<localendpoint.Z()
            <<"), with momentum "<<fMomentum<<" GeV and direction ("
            <<fDirectionX[0]<<", "<<fDirectionY[0]<<", "<<fDirectionZ[0]<<")" <<std::endl;
            return 0;
        }

        p[0] = p[1];
        /*
                // renormalization of the start direction vector
                double ur = sqrt(startdirection.X()*startdirection.X()+
                                 startdirection.Y()*startdirection.Y()+
                                 startdirection.Z()*startdirection.Z());
                startdirection.SetX(startdirection.X()/ur);
                startdirection.SetY(startdirection.Y()/ur);
                startdirection.SetZ(startdirection.Z()/ur);

                std::cout<<"new direction: ("<<startdirection.X()<<","<<startdirection.Y()<<","
                         <<startdirection.Z()<<")"<<std::endl;
        */
    }

    std::cerr<<"Can't converge after "<<n<<" iterations."<<std::endl;
    std::cerr<<"Hit at at ("<<localendpoint.X()<<", "<<localendpoint.Y()<<", "<<localendpoint.Z()
    <<"), with momentum "<<p[1]<<" GeV and direction ("
    <<fDirectionX[0]<<", "<<fDirectionY[0]<<", "<<fDirectionZ[0]<<")" <<std::endl;
    return -1;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// The filtering limits will be determined from MC simulation results
int QwTrajectory::Filter(TVector3 startpoint, TVector3 startdirection,
                         TVector3 endpoint, TVector3 enddirection) {

    double start_theta, start_phi, end_theta, end_phi;

    start_theta = acos(startdirection.Z())*DEGREE;
    start_phi = atan2(startdirection.Y(),startdirection.X())*DEGREE;

    end_theta = acos(enddirection.Z())*DEGREE;
    end_phi = atan2(enddirection.Y(),enddirection.X())*DEGREE;

    if (start_theta<4.0*DEGREE || start_theta>13.0*DEGREE) {    // scattering angle limit
        std::cerr<<"Filter: scattering angle ("<<start_theta<<" degree) is out of range"<<std::endl;
        return -1;
    }

    double dtheta = end_theta-start_theta;
    double dphi = end_phi - start_phi;
    if (dtheta<8.0*DEGREE || dtheta>25.0*DEGREE || fabs(dphi)>5.0*DEGREE) { //bending angle limit in B field
        std::cerr<<"Filter: bending angles (theta="<<dtheta<<" degree, phi="<<dphi
        <<" degree) in B field are out of range"<<std::endl;
        return -1;
    }

    return 0;

}

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

TVector3 QwTrajectory::Integrate(double E0,
                                 TVector3 startpoint,
                                 TVector3 direction,
                                 double step,
                                 double z_endplane) {

// local variables
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

    double beta = -0.2998/E0;
    fBdlx=0.0;
    fBdly=0.0;
    fBdlz=0.0;

// convert linear dimensions cm -> m
    step = step*0.01;
    z_endplane = z_endplane*0.01;

    fPositionX[0]=startpoint.X()*0.01;
    fPositionY[0]=startpoint.Y()*0.01;
    fPositionZ[0]=startpoint.Z()*0.01;

    //reverse coordinates for backward swiming
    if (startpoint.Z()>z_endplane) {
        fPositionX[0]=-fPositionX[0];
        fPositionZ[0]=-fPositionZ[0];
        fDirectionX[0]=direction.X();
        fDirectionY[0]=-direction.Y();
        fDirectionZ[0]=direction.Z();
        z_endplane=-z_endplane;
    } else { //forward swiming
        fDirectionX[0]=direction.X();
        fDirectionY[0]=direction.Y();
        fDirectionZ[0]=direction.Z();
    }

    while ( fabs(fPositionZ[0]-z_endplane)>=step ) {  // integration loop

        //values of the cordinates, unit vector and field at start of interval
        x1=fPositionX[0];
        y1=fPositionY[0];
        z1=fPositionZ[0];
        vx1=fDirectionX[0];
        vy1=fDirectionY[0];
        vz1=fDirectionZ[0];

        point[0]=x1*100.0;
        point[1]=y1*100.0;
        point[2]=z1*100.0;
        fBfield->GetFieldValue(point, bfield, BSCALE);
        bx = bfield[0];
        by = bfield[1];
        bz = bfield[2];

//          std::cout<<"x1, y1, z1, bx, by, bz :    "
//                    <<", "<<point[0]<<", "<<point[1]<<", "<<point[2]<<",      "
//                    <<bx*10000.0<<", "<<by*10000.0<<", "<<bz*10000.0<<std::endl;

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
        fPositionX[1]=fPositionX[0]+(dx1+2.0*dx2+2.0*dx3+dx4)/6.0;
        fPositionY[1]=fPositionY[0]+(dy1+2.0*dy2+2.0*dy3+dy4)/6.0;
        fPositionZ[1]=fPositionZ[0]+(dz1+2.0*dz2+2.0*dz3+dz4)/6.0;
        fDirectionX[1]=fDirectionX[0]+(dvx1+2.0*dvx2+2.0*dvx3+dvx4)/6.0;
        fDirectionY[1]=fDirectionY[0]+(dvy1+2.0*dvy2+2.0*dvy3+dvy4)/6.0;
        fDirectionZ[1]=fDirectionZ[0]+(dvz1+2.0*dvz2+2.0*dvz3+dvz4)/6.0;

        fPositionX[0]=fPositionX[1];
        fPositionY[0]=fPositionY[1];
        fPositionZ[0]=fPositionZ[1];
        fDirectionX[0]=fDirectionX[1];
        fDirectionY[0]=fDirectionY[1];
        fDirectionZ[0]=fDirectionZ[1];

//          std::cout<<"fPositionX, fPositionY, fPositionZ, bx, by, bz :    "
//                    <<", "<<fPositionX[0]*100.0<<", "<<fPositionY[0]*100.0<<", "<<fPositionZ[0]*100.0<<",      "
//                    <<bx*10000.0<<", "<<by*10000.0<<", "<<bz*10000.0<<std::endl;

    } //end of while loop

    //reverse coordinates for backward swiming
    if (startpoint.Z()>z_endplane) {
        fPositionX[0]=-fPositionX[0];
        fPositionZ[0]=-fPositionZ[0];
        fDirectionY[0]=-fDirectionY[0];
    }

    return TVector3(fPositionX[0]*100.0,fPositionY[0]*100.0,fPositionZ[0]*100.0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrajectory::LoadMomentumMatrix() {

    Double_t p;
    //Double_t x[3],y[3],z[3],ux[3],uy[3],uz[3];
    Double_t position_r[3],position_phi[3];
    Double_t direction_theta[3],direction_phi[3];

    //Double_t theta,phi;
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

    momentum_tree->SetBranchAddress("position_r0", &position_r[0]);
    momentum_tree->SetBranchAddress("position_phi0", &position_phi[0]);
    momentum_tree->SetBranchAddress("direction_theta0",&direction_theta[0]);
    momentum_tree->SetBranchAddress("direction_phi0",&direction_phi[0]);

    momentum_tree->SetBranchAddress("position_r1", &position_r[1]);
    momentum_tree->SetBranchAddress("position_phi1", &position_phi[1]);
    momentum_tree->SetBranchAddress("direction_theta1",&direction_theta[1]);
    momentum_tree->SetBranchAddress("direction_phi1",&direction_phi[1]);

    momentum_tree->SetBranchAddress("position_r2", &position_r[2]);
    momentum_tree->SetBranchAddress("position_phi2", &position_phi[2]);
    momentum_tree->SetBranchAddress("direction_theta2",&direction_theta[2]);
    momentum_tree->SetBranchAddress("direction_phi2",&direction_phi[2]);

    momentum_tree->SetBranchAddress("p",  &p);


    QwPartialTrackParameter backtrackparameter;

    Int_t numberOfEntries = momentum_tree->GetEntries();
    std::cout<<"Total grid points: "<<numberOfEntries<<std::endl;

    //build index (phi always changes from 0 to 360 degree with dphi=1.0 degree)
    int momentum_min = (int) (100*momentum_tree->GetMinimum("p"));  //dp = 0.01 GeV
    int momentum_max = (int) (100*momentum_tree->GetMaximum("p")) +1 ;
    int position_r0_min = (int) (2*momentum_tree->GetMinimum("position_r0"));  // dr ~0.5 cm
    int position_r0_max = (int) (2*momentum_tree->GetMaximum("position_r0"))+1;
    int direction_theta0_min = (int) (2*momentum_tree->GetMinimum("direction_theta0")); //dtheta = 0.5 degree
    int direction_theta0_max = (int) (2*momentum_tree->GetMaximum("direction_theta0")) +1;


    std::cout<<"p_min, p_max: "<<momentum_min<<","<<momentum_max<<std::endl;
    std::cout<<"r0_min, r0_max: "<<position_r0_min<<","<<position_r0_max<<std::endl;
    std::cout<<"theta0_min, theta0_max: "<<direction_theta0_min<<","<<direction_theta0_max<<std::endl;

    int gridsize_momentum = momentum_max-momentum_min+1;
    int gridsize_position_r0 = position_r0_max-position_r0_min+1;
    int gridsize_position_phi0 = 360;
    int gridsize_direction_theta0 = direction_theta0_max-direction_theta0_min+1;
    uint index_size = gridsize_momentum*gridsize_position_r0*gridsize_position_phi0*gridsize_direction_theta0;

    fTableIndex = new int[index_size];
    fBackTrackParameterTable.reserve(numberOfEntries);

    std::cout<<"Start to read data"<<std::endl;

    for ( Int_t i=0; i<numberOfEntries; i++) {
        momentum_tree->GetEntry(i);
        backtrackparameter.fPositionR = (float)position_r[2];
        backtrackparameter.fPositionPhi = (float)position_phi[2];
        backtrackparameter.fDirectionTheta = (float)direction_theta[2];
        backtrackparameter.fDirectionPhi = (float)direction_phi[2];

        // NOTE direction_phi[0] ~ position_phi[0], so we only need to index one of them
        int index_momentum = (int)(p*100)-momentum_min;
        int index_pos_r = (int)(2*position_r[0])-position_r0_min;
        int index_pos_phi = (int)position_phi[0];
        int index_dir_theta = (int)(2*direction_theta[0])-direction_theta0_min;
        // int index_dir_phi = (int)direction_phi[0];

        //std::cout<<"index_momentum "<<index_momentum<<",  index_pos_r "<<index_pos_r
        //         <<",  index_pos_phi "<<index_pos_phi<<",  index_dir_theta "<<index_dir_theta<<std::endl;

        //  build indexing array
        //  fTableIndex[index_momentum][index_pos_r][index_pos_phi][index_dir_theta] = i;
        int multi_index = index_momentum*gridsize_position_r0*gridsize_position_phi0*gridsize_direction_theta0
                          + index_pos_r*gridsize_position_phi0*gridsize_direction_theta0
                          + index_pos_phi*gridsize_direction_theta0
                          + index_dir_theta ;
        fTableIndex[multi_index] = i;

        fBackTrackParameterTable.push_back( backtrackparameter);
    }

    rootfile->Close();
    delete rootfile;

    return 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

