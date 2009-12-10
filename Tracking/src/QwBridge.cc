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
 *  \ingroup QwTracking
 *
 */

// jpan: This is a simple bridging routine. It works for simulated event now.
// More works need to be done on the restruction part of the tracking code
// to provide resonable bridging candidates.

#include "QwBridge.h"

#include <boost/multi_array.hpp>

#define TESTEVENT
//#define BACKSWIM

QwBridge::QwBridge() {

    trajectory = new QwTrajectory();

};
QwBridge::~QwBridge() {};

void QwBridge::LoadMagneticFieldMap() {
    B_Field = new QwMagneticField();
    B_Field->ReadFieldMapFile(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/MainMagnet_FieldMap.dat");
};

int QwBridge::BridgeFrontBackPartialTrack(TVector3 startpoint,
                                          TVector3 startpointdirection,
                                          TVector3 endpoint,
                                          TVector3 endpointdirection) {

    int status;
    trajectory->SetMagneticField(B_Field);
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

int QwBridge::Shooting(TVector3 fStartPoint, TVector3 fStartDirection, TVector3 fEndPoint, TVector3 fEndDirection) {

    double res = 1.0; //position determination [cm]
    double step_size = 0.2; // integration step size [cm]
    double dp = 0.05; //momentum variation [GeV]

    double p[2],x[2],y[2],r[2],xh,yh,rh;
    TVector3 endpoint;

    if (fStartPoint.Z()<=fEndPoint.Z())
       p[0] = trajectory->EstimateInitialMomentum(fStartDirection);
    else
       p[0] = trajectory->EstimateInitialMomentum(fEndDirection);

    std::cout<<"estimated momentum: "<<p[0]<<std::endl;

    xh = fEndPoint.X();
    yh = fEndPoint.Y();
    rh = sqrt(xh*xh+yh*yh);

    int n;
    for (n=1; n<=MAX_ITERATION; n++) {

        endpoint = trajectory->Integrate(p[0]-dp, fStartPoint, fStartDirection, step_size, fEndPoint.Z());
        x[0]=endpoint.X();
        y[0]=endpoint.Y();

        endpoint = trajectory->Integrate(p[0]+dp, fStartPoint, fStartDirection, step_size, fEndPoint.Z());
        x[1]=endpoint.X();
        y[1]=endpoint.Y();

        r[0] = sqrt(x[0]*x[0]+y[0]*y[0]);
        r[1] = sqrt(x[1]*x[1]+y[1]*y[1]);

        p[1] = p[0] -0.5*dp*(r[0]+r[1]-2.0*rh)/(r[1]-r[0]);

        //std::cout<<"momentum: p0, p1:  "<<p[0]<<", "<<p[1]<<std::endl;
        //std::cout<<"hit location: x0, y0, x1, y1, r0, r1:  "<<x[0]<<", "<<y[0]
        //         <<", "<<x[1]<<", "<<y[1]<<", "<<r[0]<<", "<<r[1]<<std::endl;

        endpoint = trajectory->Integrate(p[1], fStartPoint, fStartDirection, step_size, fEndPoint.Z());
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
            std::cout<<"Converged after "<<n<<" iterations at ("<<endpoint.X()<<", "<<endpoint.Y()<<", "<<endpoint.Z()
            <<"), with momentum "<<momentum<<" GeV and direction ("
            <<trajectory->GetDirectionX()<<", "<<trajectory->GetDirectionY()<<", "
            <<trajectory->GetDirectionZ()<<")" <<std::endl;
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
    <<trajectory->GetDirectionX()<<", "<<trajectory->GetDirectionY()<<", "
    <<trajectory->GetDirectionZ()<<")" <<std::endl;
    return -1;
};

// The filtering limits will be determined from MC simulation results

int QwBridge::Filter(TVector3 fStartPoint, TVector3 fStartDirection, TVector3 fEndPoint, TVector3 fEndDirection){

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



int QwBridge::LoadMomentumMatrix() {

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


//NOTE jpan: I am not sure the performance of the boost multi_array.
// It could be slower than a native 1D array.

//  uint index_size = (momentum_max-momentum_min)*(position_r0_max-position_r0_min)
//                 *(direction_theta0_max-direction_theta0_min)*360;
// int *index = new int[index_size];

  // Create a 4D array to store index
  typedef boost::multi_array<int, 4> indexing_array;
  typedef indexing_array::index index;
  indexing_array table_index(boost::extents[momentum_max-momentum_min][position_r0_max-position_r0_min]
                                           [360][direction_theta0_max-direction_theta0_min]);


  std::vector <PartialTrackParameter> backtrackparametertable;
  backtrackparametertable.reserve(numberOfEntries);

  std::cout<<"Start to read data"<<std::endl;

  for ( uint i=0; i<numberOfEntries; i++){
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
    table_index[index_momentum][index_pos_r][index_pos_phi][index_dir_theta] = i;

    backtrackparametertable.push_back( backtrackparameter);
  }

  rootfile->Close();

  delete rootfile;

  return 0;
};

