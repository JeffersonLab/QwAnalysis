/*! \file   QwBridge.cc
 *
 *  \author J. Pan
 *  \date   Thu Nov 26 11:44:51 CST 2009
 *
 *  \brief  Bridging R2/R3 partial tracks.
 *
 *
 *  The Newton-Raphson method is used to solve for the momentum of the track.
 *
 *  \ingroup QwTrackingAnl
 *
 */

// jpan: This is a simple bridging routine. It works for simulated event now.
// More works need to be done on the restruction part of the tracking code
// to provide resonable bridging candidates.

#include "QwBridge.h"

QwBridge::QwBridge() {

    trajectory = new QwTrajectory();

};
QwBridge::~QwBridge() {};

int QwBridge::LoadMagneticFieldMap() {
    B_Field = new QwMagneticField();
    B_Field->ReadFieldMap(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/MainMagnet_FieldMap.dat");
};

int QwBridge::BridgeFrontBackPartialTrack(TVector3 startpoint, TVector3 direction, TVector3 endpoint) {

    trajectory->SetMagneticField(B_Field);

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
       std::cout<<"Test event endpoint direction ("<<testUx/testR<<", "
                <<testUy/testR<<", "<<testUz/testR<<")"<<std::endl;
       testUx = 29.548246; //momentum component of the hit at R2 back chamber wireplane 6
       testUy = 170.25393;
       testUz = 1136.2637;
       testR = sqrt(testUx*testUx+testUy*testUy+testUz*testUz);
       startpoint = TVector3(9.0694189,53.198646,-280.2352);   //hit position on R2 back chamber
       direction = TVector3(testUx/testR,testUy/testR,testUz/testR);
       endpoint = TVector3(71.882591,328.381226,570.717773);    // hit position on Cerenkov bar

    std::cout<<"swiming from : ("<<startpoint.X()<<","<<startpoint.Y()<<","<<startpoint.Z()<<")   ";
    std::cout<<"direction : ("<<direction.X()<<","<<direction.Y()<<","<<direction.Z()<<")  ";
    std::cout<<"with momentum "<<testR*0.001<<" GeV"<<std::endl;
//end of test event

    int status = Shooting(startpoint,direction,endpoint);
    if (status == 0) return 0;
    else return -1;
};

int QwBridge::Shooting(TVector3 fStartPoint, TVector3 fDirection, TVector3 fEndPoint) {

    double res = 0.01; //position determination [cm]
    double p[2],x[2],y[2],r[2],xh,yh,rh;
    double step_size = 0.1; // integration step size [cm]
    double dp = 0.01; //momentum variation [GeV]
    TVector3 endpoint;

    p[0] = trajectory->EstimateInitialMomentum(fDirection);

    xh = fEndPoint.X();
    yh = fEndPoint.Y();
    rh = sqrt(xh*xh+yh*yh);

    int n;
    for (n=0; n<10; n++) {

        endpoint = trajectory->Integrate(p[0]-dp, fStartPoint, fDirection, step_size, fEndPoint.Z());
        x[0]=endpoint.X();
        y[0]=endpoint.Y();

        endpoint = trajectory->Integrate(p[0]+dp, fStartPoint, fDirection, step_size, fEndPoint.Z());
        x[1]=endpoint.X();
        y[1]=endpoint.Y();

        r[0] = sqrt(x[0]*x[0]+y[0]*y[0]);
        r[1] = sqrt(x[1]*x[1]+y[1]*y[1]);

        p[1] = p[0] -0.5*dp*(r[0]+r[1]-2.0*rh)/(r[1]-r[0]);

//        std::cout<<"momentum: p0, p1:  "<<p[0]<<", "<<p[1]<<std::endl;
//        std::cout<<"hit location: x0, y0, x1, y1, r0, r1:  "<<x[0]<<", "<<y[0]
//                 <<", "<<x[1]<<", "<<y[1]<<", "<<r[0]<<", "<<r[1]<<std::endl;

        endpoint = trajectory->Integrate(p[1], fStartPoint, fDirection, step_size, fEndPoint.Z());
        x[0]=endpoint.X();
        y[0]=endpoint.Y();
        r[0] = sqrt(x[0]*x[0]+y[0]*y[0]);

  //TODO - need to take into account of the energy loss along the trajectories, assume ~6 MeV for now.
  //     - need slope matching as well

        if (abs(r[0]-rh)<res) {
            momentum = p[1]+0.006;
            std::cout<<"Expected hit location: ("<<xh<<", "<<yh<<", "<<fEndPoint.Z()<<")"<<std::endl;
            std::cout<<"Converged after "<<n+1<<" iterations at ("<<endpoint.X()<<", "<<endpoint.Y()<<", "<<endpoint.Z()
            <<"), with momentum "<<momentum<<" GeV and direction ("
            <<trajectory->GetDirectionX()<<", "<<trajectory->GetDirectionY()<<", "
            <<trajectory->GetDirectionZ()<<")" <<std::endl;
            return 0;
        }

        p[0] = p[1];
    }
    std::cerr<<"Can't converge after "<<n+1<<" iterations."<<std::endl;
    return -1;
};
