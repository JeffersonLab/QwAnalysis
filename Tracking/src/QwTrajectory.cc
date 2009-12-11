
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

#include "QwTrajectory.h"

#include <cstdlib>

#include "QwMagneticField.h"

//#include <boost/multi_array.hpp>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#define TESTEVENT
//#define BACKSWIM

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrajectory::QwTrajectory() {

    for (int i=0;i<2;i++) {
        xx[i]=0.0;     // position
        yy[i]=0.0;
        zz[i]=0.0;
        uvx[i] = 0.0;  // unit vector
        uvy[i] = 0.0;
        uvz[i] = 0.0;
    }

    bdlx = 0.0;
    bdly = 0.0;
    bdlz = 0.0;

    LoadMomentumMatrix();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

QwTrajectory::~QwTrajectory() {

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void QwTrajectory::LoadMagneticFieldMap() {

    B_Field = new QwMagneticField();
    B_Field->ReadFieldMapFile(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/MainMagnet_FieldMap.dat");

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrajectory::BridgeFrontBackPartialTrack(TVector3 startpoint,
                                              TVector3 startpointdirection,
                                              TVector3 endpoint,
                                              TVector3 endpointdirection) {

    int status;
    SetMagneticField(B_Field);
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

int QwTrajectory::Shooting(TVector3 fStartPoint,
                           TVector3 fStartDirection,
                           TVector3 fEndPoint,
                           TVector3 fEndDirection) {

    double res = 1.0; //position determination [cm]
    double step_size = 0.2; // integration step size [cm]
    double dp = 0.05; //momentum variation [GeV]

    double p[2],x[2],y[2],r[2],xh,yh,rh;
    TVector3 endpoint;

    if (fStartPoint.Z()<=fEndPoint.Z())
        p[0] = EstimateInitialMomentum(fStartDirection);
    else
        p[0] = EstimateInitialMomentum(fEndDirection);

    std::cout<<"estimated momentum: "<<p[0]<<std::endl;

    xh = fEndPoint.X();
    yh = fEndPoint.Y();
    rh = sqrt(xh*xh+yh*yh);

    int n;
    for (n=1; n<=MAX_ITERATION; n++) {

        endpoint = Integrate(p[0]-dp, fStartPoint, fStartDirection, step_size, fEndPoint.Z());
        x[0]=endpoint.X();
        y[0]=endpoint.Y();

        endpoint = Integrate(p[0]+dp, fStartPoint, fStartDirection, step_size, fEndPoint.Z());
        x[1]=endpoint.X();
        y[1]=endpoint.Y();

        r[0] = sqrt(x[0]*x[0]+y[0]*y[0]);
        r[1] = sqrt(x[1]*x[1]+y[1]*y[1]);

        p[1] = p[0] -0.5*dp*(r[0]+r[1]-2.0*rh)/(r[1]-r[0]);

        //std::cout<<"momentum: p0, p1:  "<<p[0]<<", "<<p[1]<<std::endl;
        //std::cout<<"hit location: x0, y0, x1, y1, r0, r1:  "<<x[0]<<", "<<y[0]
        //         <<", "<<x[1]<<", "<<y[1]<<", "<<r[0]<<", "<<r[1]<<std::endl;

        endpoint = Integrate(p[1], fStartPoint, fStartDirection, step_size, fEndPoint.Z());
        x[0]=endpoint.X();
        y[0]=endpoint.Y();
        r[0] = sqrt(x[0]*x[0]+y[0]*y[0]);

        //TODO - need to take into account of the energy loss along the trajectories, assume ~2 MeV for now.
        //     - need slope matching as well

        double dx = x[0]-xh;
        double dy = y[0]-yh;
        double dr = sqrt(dx*dx+dy*dy);
        if ( dr<res) {
            if (fStartPoint.Z()<=fEndPoint.Z())
                momentum = p[1]+0.002;
            else
                momentum = p[1]-0.002;
            std::cout<<"Converged after "<<n<<" iterations at ("
            <<endpoint.X()<<", "<<endpoint.Y()<<", "<<endpoint.Z()
            <<"), with momentum "<<momentum<<" GeV and direction ("
            <<GetDirectionX()<<", "<<GetDirectionY()<<", "
            <<GetDirectionZ()<<")" <<std::endl;
            return 0;
        }

        p[0] = p[1];
        /*
                // renormalization of the start direction vector
                double ur = sqrt(fStartDirection.X()*fStartDirection.X()+
                                 fStartDirection.Y()*fStartDirection.Y()+
                                 fStartDirection.Z()*fStartDirection.Z());
                fStartDirection.SetX(fStartDirection.X()/ur);
                fStartDirection.SetY(fStartDirection.Y()/ur);
                fStartDirection.SetZ(fStartDirection.Z()/ur);

                std::cout<<"new direction: ("<<fStartDirection.X()<<","<<fStartDirection.Y()<<","
                         <<fStartDirection.Z()<<")"<<std::endl;
        */
    }

    std::cerr<<"Can't converge after "<<n<<" iterations."<<std::endl;
    std::cerr<<"Hit at at ("<<endpoint.X()<<", "<<endpoint.Y()<<", "<<endpoint.Z()
    <<"), with momentum "<<p[1]<<" GeV and direction ("
    <<GetDirectionX()<<", "<<GetDirectionY()<<", "<<GetDirectionZ()<<")" <<std::endl;
    return -1;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// The filtering limits will be determined from MC simulation results
int QwTrajectory::Filter(TVector3 fStartPoint, TVector3 fStartDirection, TVector3 fEndPoint, TVector3 fEndDirection) {

    double degree = 180.0/3.1415926535897932;
    double fStartTheta, fStartPhi, fEndTheta, fEndPhi;

    fStartTheta = acos(fStartDirection.Z())*degree;
    fStartPhi = atan2(fStartDirection.Y(),fStartDirection.X())*degree;

    fEndTheta = acos(fEndDirection.Z())*degree;
    fEndPhi = atan2(fEndDirection.Y(),fEndDirection.X())*degree;

    if (fStartTheta<4.0*degree || fStartTheta>13.0*degree) {    // scattering angle limit
        std::cerr<<"Filter: scattering angle ("<<fStartTheta<<" degree) is out of range"<<std::endl;
        return -1;
    }

    double dtheta = fEndTheta-fStartTheta;
    double dphi = fEndPhi - fStartPhi;
    if (dtheta<8.0*degree || dtheta>25.0*degree || fabs(dphi)>5.0*degree) { //bending angle limit in B field
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
    double Point[3];
    double Bfield[3];

    double beta = -0.2998/E0;
    bdlx=0.0;
    bdly=0.0;
    bdlz=0.0;

// convert linear dimensions cm -> m
    step = step*0.01;
    z_endplane = z_endplane*0.01;

    xx[0]=startpoint.X()*0.01;
    yy[0]=startpoint.Y()*0.01;
    zz[0]=startpoint.Z()*0.01;

    //reverse coordinates for backward swiming
    if (startpoint.Z()>z_endplane) {
        xx[0]=-xx[0];
        zz[0]=-zz[0];
        uvx[0]=direction.X();
        uvy[0]=-direction.Y();
        uvz[0]=direction.Z();
        z_endplane=-z_endplane;
    } else { //forward swiming
        uvx[0]=direction.X();
        uvy[0]=direction.Y();
        uvz[0]=direction.Z();
    }

    while ( fabs(zz[0]-z_endplane)>=step ) {  // integration loop

        //values of the cordinates, unit vector and field at start of interval
        x1=xx[0];
        y1=yy[0];
        z1=zz[0];
        vx1=uvx[0];
        vy1=uvy[0];
        vz1=uvz[0];

        Point[0]=x1*100.0;
        Point[1]=y1*100.0;
        Point[2]=z1*100.0;
        B_Field->GetFieldValue(Point, Bfield, BSCALE);
        bx = Bfield[0];
        by = Bfield[1];
        bz = Bfield[2];

//          std::cout<<"x1, y1, z1, bx, by, bz :    "
//                    <<", "<<Point[0]<<", "<<Point[1]<<", "<<Point[2]<<",      "
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
        Point[0]=x*100.0;
        Point[1]=y*100.0;
        Point[2]=z*100.0;
        B_Field->GetFieldValue(Point, Bfield, BSCALE);
        bx = Bfield[0];
        by = Bfield[1];
        bz = Bfield[2];

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
        Point[0]=x*100.0;
        Point[1]=y*100.0;
        Point[2]=z*100.0;
        B_Field->GetFieldValue(Point, Bfield, BSCALE);
        bx = Bfield[0];
        by = Bfield[1];
        bz = Bfield[2];

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
        Point[0]=x*100.0;
        Point[1]=y*100.0;
        Point[2]=z*100.0;
        B_Field->GetFieldValue(Point, Bfield, BSCALE);
        bx = Bfield[0];
        by = Bfield[1];
        bz = Bfield[2];

//evaluate the path integral (b x dl)
        bdlx=bdlx+step*(vz*by-vy*bz);
        bdly=bdly+step*(vx*bz-vz*bx);
        bdlz=bdlz+step*(vy*bx-vx*by);

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

//          std::cout<<"xx, yy, zz, bx, by, bz :    "
//                    <<", "<<xx[0]*100.0<<", "<<yy[0]*100.0<<", "<<zz[0]*100.0<<",      "
//                    <<bx*10000.0<<", "<<by*10000.0<<", "<<bz*10000.0<<std::endl;

    } //end of while loop

    //reverse coordinates for backward swiming
    if (startpoint.Z()>z_endplane) {
        xx[0]=-xx[0];
        zz[0]=-zz[0];
        uvy[0]=-uvy[0];
    }

    return TVector3(xx[0]*100.0,yy[0]*100.0,zz[0]*100.0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrajectory::LoadMomentumMatrix() {

    Double_t degree = 180.0/3.1415926535897932;
    Int_t    gridnum;
    Double_t p;
    //Double_t x[3],y[3],z[3],ux[3],uy[3],uz[3];
    Double_t position_r[3],position_phi[3];
    Double_t direction_theta[3],direction_phi[3];

    //Double_t theta,phi;
    TVector3 startpoint,startdirection,endpoint,enddirection;

    //open file
    TString rootfilename=std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QwTrajMatrix.root";
    TFile* rootfile = TFile::Open(rootfilename,"read");
    rootfile->cd();

    TTree *momentum_tree = (TTree *)rootfile->Get("Momentum_Tree");

//  momentum_tree->SetBranchAddress("gridnum",&gridnum);

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


    PartialTrackParameter backtrackparameter;

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

//NOTE jpan: I am not sure the performance of the boost multi_array.
// It could be slower than a native 1D array.

    uint index_size = gridsize_momentum*gridsize_position_r0*gridsize_position_phi0*gridsize_direction_theta0;
    table_index = new int[index_size];

    // Create a 4D array to store index
//     typedef boost::multi_array<int, 4> indexing_array;
//     indexing_array table_index(boost::extents[momentum_max-momentum_min][position_r0_max-position_r0_min]
//                                [360][direction_theta0_max-direction_theta0_min]);

    //std::vector <PartialTrackParameter> backtrackparametertable;
    backtrackparametertable.reserve(numberOfEntries);

    std::cout<<"Start to read data"<<std::endl;

    for ( uint i=0; i<numberOfEntries; i++) {
        momentum_tree->GetEntry(i);
        backtrackparameter.position_r = (float)position_r[2];
        backtrackparameter.position_phi = (float)position_phi[2];
        backtrackparameter.direction_theta = (float)direction_theta[2];
        backtrackparameter.direction_phi = (float)direction_phi[2];

        // NOTE direction_phi[0] ~ position_phi[0], so we only need to index one of them
        int index_momentum = (int)(p*100)-momentum_min;
        int index_pos_r = (int)(2*position_r[0])-position_r0_min;
        int index_pos_phi = (int)position_phi[0];
        int index_dir_theta = (int)(2*direction_theta[0])-direction_theta0_min;
        // int index_dir_phi = (int)direction_phi[0];

        //std::cout<<"index_momentum "<<index_momentum<<",  index_pos_r "<<index_pos_r
        //         <<",  index_pos_phi "<<index_pos_phi<<",  index_dir_theta "<<index_dir_theta<<std::endl;

        //  build indexing array
        //table_index[index_momentum][index_pos_r][index_pos_phi][index_dir_theta] = i;

        int multi_index = index_momentum*gridsize_position_r0*gridsize_position_phi0*gridsize_direction_theta0
                          + index_pos_r*gridsize_position_phi0*gridsize_direction_theta0
                          + index_pos_phi*gridsize_direction_theta0
                          + index_dir_theta ;
        table_index[multi_index] = i;

        backtrackparametertable.push_back( backtrackparameter);
    }

    rootfile->Close();

    delete rootfile;

    return 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

