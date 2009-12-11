/*------------------------------------------------------------------------*//*!

 \file QwTrajMatrixGenerator.cc \ingroup QwTracking

 \author J. Pan
 \date Thu Dec  3 15:22:20 CST 2009

 \brief Generate a momentum look-up table indexed by position and direction

*//*-------------------------------------------------------------------------*/

// Z coordinate:
// Z0: start plane = -250 cm, Z1: endplane = +250 cm, Z2: focalplane = +570 cm
// B field values are available from z=-250 cm to z=250 cm
//
// A family of trajectories starting from the target to endplane will be generated.
// Momentum and scattering direction vary for each trajectory. We record the inter-
// section position and direction at start-plane, end-plane and focalplane
//
//
// - each RK4 integration takes ~25 ms if integration step=0.1 cm
// - use the nearest-neighbor 'interpolation' to the magnetic field map,
//   each RK4 integration time is reduced to ~20 ms with step=0.1 cm
// - if set the step=1.0 cm, the RK4 takes 2.2 ms to generate a trajectory
//

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>

// ROOT headers
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TStopwatch.h>

// Qweak Tracking headers
#include "Det.h"
#include "QwTrajectory.h"
#include "QwMagneticField.h"

#include "QwBridge.h"

// Temporary global variables for sub-programs
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];



int main (int argc, char* argv[]) {

    double degree = 180.0/3.1415926535897932;

    QwMagneticField* B_Field = new QwMagneticField();
    QwTrajectory* trajectory = new QwTrajectory();

    B_Field->ReadFieldMapFile(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/MainMagnet_FieldMap.dat");
    trajectory->SetMagneticField(B_Field);

    int    gridnum;
    double x[3],y[3],z[3],ux[3],uy[3],uz[3];
    double position_r[3],position_phi[3];
    double direction_theta[3],direction_phi[3];
    double p,dp;
    double theta,dtheta;
    double phi,dphi;
    double vertex_z,dz;
    double step_size;
    TVector3 startpoint,startdirection,endpoint,enddirection;

    step_size = 1.0; // [cm]
    z[0] = -250.0; // [cm]
    z[1] =  250.0; // [cm]
    z[2] =  570.0; // [cm]

    dp = 0.01;  // [GeV]
    dz = 1.0;    // [cm]
    dtheta = 0.5; // [degree]
    dphi = 1.0; // [degree]

    //open file and set up output tree
    TString rootfilename=std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QwTrajMatrix.root";
    TFile* rootfile = new TFile(rootfilename,"RECREATE","Qweak momentum matrix");
    rootfile->cd();

    TTree *momentum_tree = new TTree("Momentum_Tree","momentum data tree");

    momentum_tree->Branch("gridnum",&gridnum,"gridnum/I");
    momentum_tree->Branch("p", &p, "p/D");
    momentum_tree->Branch("vertex_z", &vertex_z, "vertex_z/D");

// position and direction at front startplane
    momentum_tree->Branch("x0", &x[0], "x0/D");
    momentum_tree->Branch("y0", &y[0], "y0/D");
    momentum_tree->Branch("z0", &z[0], "z0/D");
    momentum_tree->Branch("ux0",&ux[0],"ux0/D");
    momentum_tree->Branch("uy0",&uy[0],"uy0/D");
    momentum_tree->Branch("uz0",&uz[0],"uz0/D");

    momentum_tree->Branch("position_r0", &position_r[0], "position_r0/D");
    momentum_tree->Branch("position_phi0", &position_phi[0], "position_phi0/D");
    momentum_tree->Branch("direction_theta0",&direction_theta[0],"direction_theta0/D");
    momentum_tree->Branch("direction_phi0",&direction_phi[0],"direction_phi0/D");

// position and direction at back endplane
    momentum_tree->Branch("x1", &x[1], "x1/D");
    momentum_tree->Branch("y1", &y[1], "y1/D");
    momentum_tree->Branch("z1", &z[1], "z1/D");
    momentum_tree->Branch("ux1",&ux[1],"ux1/D");
    momentum_tree->Branch("uy1",&uy[1],"uy1/D");
    momentum_tree->Branch("uz1",&uz[1],"uz1/D");

    momentum_tree->Branch("position_r1", &position_r[1], "position_r1/D");
    momentum_tree->Branch("position_phi1", &position_phi[1], "position_phi1/D");
    momentum_tree->Branch("direction_theta1",&direction_theta[1],"direction_theta1/D");
    momentum_tree->Branch("direction_phi1",&direction_phi[1],"direction_phi1/D");

// position and direction at focal plane
    momentum_tree->Branch("x2", &x[2], "x2/D");
    momentum_tree->Branch("y2", &y[2], "y2/D");
    momentum_tree->Branch("z2", &z[2], "z2/D");
    momentum_tree->Branch("ux2",&ux[2],"ux2/D");
    momentum_tree->Branch("uy2",&uy[2],"uy2/D");
    momentum_tree->Branch("uz2",&uz[2],"uz2/D");

    momentum_tree->Branch("position_r2", &position_r[2], "position_r2/D");
    momentum_tree->Branch("position_phi2", &position_phi[2], "position_phi2/D");
    momentum_tree->Branch("direction_theta2",&direction_theta[2],"direction_theta2/D");
    momentum_tree->Branch("direction_phi2",&direction_phi[2],"direction_phi2/D");

    std::cout<<"Start to generate data"<<std::endl;

    // Create a timer
    TStopwatch timer;
    timer.Start();

    //calculate matrix element
    gridnum = 0;
    for (p=1.0; p<=1.160; p+=dp) {
//      for (vertex_z=-667.5; vertex_z<=-632.5; vertex_z+=dz){
        for (vertex_z=-650.0; vertex_z<=-650.0; vertex_z+=dz) {
            for (theta=5.0;theta<=12.0; theta+=dtheta) {
                for (phi=0.0;phi<360.0; phi+=dphi) {

                    //std::cout<<"p,z,theta,phi: "<<p<<","<<vertex_z<<","<<theta<<","<<phi<<std::endl;

                    // rough keep-out zones (approximately, coil orientation +/-6.0 degree)
//                     if ( (phi>(22.5-6.0)  && phi<(22.5+6.0))  ||
//                          (phi>(67.5-6.0)  && phi<(67.5+6.0))  ||
//                          (phi>(112.5-6.0) && phi<(112.5+6.0)) ||
//                          (phi>(157.5-6.0) && phi<(157.5+6.0)) ||
//                          (phi>(202.5-6.0) && phi<(202.5+6.0)) ||
//                          (phi>(247.5-6.0) && phi<(247.5+6.0)) ||
//                          (phi>(292.5-6.0) && phi<(292.5+6.0)) ||
//                          (phi>(337.5-6.0) && phi<(337.5+6.0))  )
//                         continue;

                    //intersection position and direction with the z=-250 cm plane
                    ux[0] = sin(theta/degree)*cos(phi/degree);
                    uy[0] = sin(theta/degree)*sin(phi/degree);
                    uz[0] = cos(theta/degree);
                    startdirection = TVector3(ux[0],uy[0],uz[0]);

                    double r = (z[0]-vertex_z)/uz[0];
                    x[0] = r*ux[0];
                    y[0] = r*uy[0];
                    startpoint = TVector3(x[0],y[0],z[0]);

                    position_r[0] = sqrt(x[0]*x[0]+y[0]*y[0]);
                    position_phi[0] = atan2(y[0],x[0])*degree;
                    if (position_phi[0]<0.0) position_phi[0] = position_phi[0]+360.0;
                    direction_theta[0] = theta;
                    direction_phi[0] = phi;

                    //raytrace from startplane to endplane
                    endpoint = trajectory->Integrate(p, startpoint, startdirection, step_size, z[1]);
                    x[1] = endpoint.X();
                    y[1] = endpoint.Y();
                    //z[1] = endpoint.Z();
                    ux[1] = trajectory->GetDirectionX();
                    uy[1] = trajectory->GetDirectionY();
                    uz[1] = trajectory->GetDirectionZ();

                    position_r[1] = sqrt(x[1]*x[1]+y[1]*y[1]);
                    position_phi[1] = atan2(y[1],x[1])*degree;
                    if (position_phi[1]<0.0) position_phi[1] = position_phi[1]+360.0;
                    direction_theta[1] = theta;
                    direction_phi[1] = phi;

                    // Extending the back stright tracks onto focal plane
                    r = (z[2]-z[1])/uz[1];
                    x[2] = x[1] + r*ux[2];
                    y[2] = y[1] + r*uy[2];
                    ux[2] = ux[1];
                    uy[2] = uy[1];
                    uz[2] = uz[1];

                    position_r[2] = sqrt(x[2]*x[2]+y[2]*y[2]);
                    position_phi[2] = atan2(y[2],x[2])*degree;
                    if (position_phi[2]<0.0) position_phi[2] = position_phi[2]+360.0;
                    direction_theta[2] = uz[2]*degree;
                    direction_phi[2] = atan2(uy[2],ux[2])*degree;
                    if (direction_phi[2]<0.0) direction_phi[2] = direction_phi[2]+360.0;

                    gridnum++;

                    //fill tree
                    momentum_tree->Fill();

                    if (gridnum % 100 == 0)   {
                        std::cout<<"."<<std::flush;
                        if (gridnum % 10000 == 0)   std::cout<<"|"<<std::flush;
                        momentum_tree->AutoSave();    //Call AutoSave to save the tree to disk
                        gDirectory->Purge(); //Purge old trees
                    }

                } //end of phi loop
            } //end of theta loop
        } //end of vertex_z loop
    } //end of p loop

    timer.Stop();

    std::cout<<"done."<<std::endl<<"generated "<<gridnum<<" trajectories."<<std::endl;

    std::cout<<"CPU time used:  " << timer.CpuTime() << " s "
    << "(" << timer.CpuTime() / gridnum << " s per trajectory)" << std::endl
    << "Real time used: " << timer.RealTime() << " s "
    << "(" << timer.RealTime() / gridnum << " s per trajectory)" << std::endl;

    rootfile->Write(0, TObject::kOverwrite);
    rootfile->Close();

    delete rootfile;
    delete trajectory;
    delete B_Field;

    return 0;

}
