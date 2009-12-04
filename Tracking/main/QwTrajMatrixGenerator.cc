/*------------------------------------------------------------------------*//*!

 \file QwTrajMatrixGenerator.cc \ingroup QwTrackingAnl

 \author J. Pan
 \date Thu Dec  3 15:22:20 CST 2009

 \brief Generate a momentum look-up table indexed by position and direction

*//*-------------------------------------------------------------------------*/

// Z coordinate:
// Z0: start plane = -250 cm, Z1: midplane = 0 cm, Z2: end plane = +250 cm
// B field values are available from z=-250 cm to z=250 cm

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>

// ROOT headers
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>

// Qweak Tracking headers
#include "Det.h"
#include "options.h"
#include "QwTrajectory.h"
#include "QwMagneticField.h"

#define TEST

// Temporary global variables for sub-programs
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
Options opt;



int main (int argc, char* argv[])
{

  double degree = 180.0/3.1415926535897932;

  QwMagneticField* B_Field = new QwMagneticField();
  QwTrajectory* trajectory = new QwTrajectory();


  B_Field->ReadFieldMap(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/MainMagnet_FieldMap.dat");
  trajectory->SetMagneticField(B_Field);

  int    gridnum;
  double p,dp;
  double x[3],y[3],z[3],ux[3],uy[3],uz[3];
  double theta,phi;
  double dx,dy;
  double dtheta,dphi;
  double step_size;
  TVector3 startpoint,startdirection,endpoint,enddirection;

  step_size = 0.1; // [cm]
  z[0] = -250.0; // [cm]
  z[1] =  0.0; // [cm]
  z[2] =  250.0; // [cm]

  dp = 0.001;  // [GeV]
  dx = dy = 0.1; // [cm]
  dtheta = 0.1; // [degree]
  dphi = 1.0; // [degree]

#ifdef TEST
  dp = 0.2;  // [GeV]
  dx = dy = 5.0; // [cm]
  dtheta = 1.0; // [degree]
  dphi = 5.0; // [degree]
#endif

  //open file and set up output tree
  TString rootfilename=std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QwTrajMatrix.root";
  TFile* rootfile = new TFile(rootfilename,"RECREATE","Qweak momentum matrix");
  rootfile->cd();

  TTree *momentum_tree = new TTree("Momentum_Tree","momentum data tree");

  momentum_tree->Branch("gridnum",&gridnum,"gridnum/I");
  momentum_tree->Branch("x0", &x[0], "x0/D");
  momentum_tree->Branch("y0", &y[0], "y0/D");
  momentum_tree->Branch("z0", &z[0], "z0/D");
  momentum_tree->Branch("ux0",&ux[0],"ux0/D");
  momentum_tree->Branch("uy0",&uy[0],"uy0/D");
  momentum_tree->Branch("uz0",&uz[0],"uz0/D");

  momentum_tree->Branch("x1", &x[1], "x1/D");
  momentum_tree->Branch("y1", &y[1], "y1/D");
  momentum_tree->Branch("z1", &z[1], "z1/D");
  momentum_tree->Branch("ux1",&ux[1],"ux1/D");
  momentum_tree->Branch("uy1",&uy[1],"uy1/D");
  momentum_tree->Branch("uz1",&uz[1],"uz1/D");

  momentum_tree->Branch("x2", &x[2], "x2/D");
  momentum_tree->Branch("y2", &y[2], "y2/D");
  momentum_tree->Branch("z2", &z[2], "z2/D");
  momentum_tree->Branch("ux2",&ux[2],"ux2/D");
  momentum_tree->Branch("uy2",&uy[2],"uy2/D");
  momentum_tree->Branch("uz2",&uz[2],"uz2/D");

  momentum_tree->Branch("p",  &p,    "p/D");

  std::cout<<"Start to generate data"<<std::endl;

  //calculate matrix
  gridnum = 0;
  for (p=1.05;p<=1.155;p=p+dp){
    for (x[0]=-100.0;x[0]<=100.0;x[0]=x[0]+dx){
      for (y[0]=-100.0;y[0]<=100.0;y[0]=y[0]+dy){
        for (theta=4.0/degree;theta<=13.0/degree;theta = theta+dtheta){
          for (phi=0.0;phi<=359.0/degree;phi = phi+dphi){

            //std::cout<<x[0]<<","<<y[0]<<","<<theta<<","<<phi<<std::endl;

            // rough keep-out zones (approximately, coil orientation +/-2.5 degree)
            if ( (phi>(22.5-2.5)/degree && phi<(22.5+2.5)/degree) ||
                 (phi>(67.5-2.5)/degree && phi<(67.5+2.5)/degree) ||
                 (phi>(112.5-2.5)/degree && phi<(112.5+2.5)/degree) ||
                 (phi>(157.5-2.5)/degree && phi<(157.5+2.5)/degree) ||
                 (phi>(202.5-2.5)/degree && phi<(202.5+2.5)/degree) ||
                 (phi>(247.5-2.5)/degree && phi<(247.5+2.5)/degree) ||
                 (phi>(292.5-2.5)/degree && phi<(292.5+2.5)/degree) ||
                 (phi>(337.5-2.5)/degree && phi<(337.5+2.5)/degree)  )
               continue;

            double R2 = x[0]*x[0]+y[0]*y[0];
            if (  R2> 100.0*100.0 || R2< 20.0*20.0) continue;  // roughly confine to 20.0 cm < r < 100.0 cm

            startpoint = TVector3(x[0],y[0],z[0]);
            ux[0] = sin(theta)*cos(phi);
            uy[0] = sin(theta)*sin(phi);
            uz[0] = cos(theta);
            startdirection = TVector3(ux[0],uy[0],uz[0]);

            //raytrace from startplane to midplane
            endpoint = trajectory->Integrate(p, startpoint, startdirection, step_size, z[1]);
            x[1] = endpoint.X();
            y[1] = endpoint.Y();
            //z[1] = endpoint.Z();
            ux[1] = trajectory->GetDirectionX();
            uy[1] = trajectory->GetDirectionY();
            uz[1] = trajectory->GetDirectionZ();

            //raytrace from midplane to endplane
            startpoint = endpoint;
            endpoint = trajectory->Integrate(p, startpoint, startdirection, step_size, z[2]);
            x[2] = endpoint.X();
            y[2] = endpoint.Y();
            //z[2] = endpoint.Z();
            ux[2] = trajectory->GetDirectionX();
            uy[2] = trajectory->GetDirectionY();
            uz[2] = trajectory->GetDirectionZ();

            gridnum++;

            //std::cout<<"No. "<<gridnum<<"   p="<<p<<std::endl;
            //std::cout<<"(x0,y0,z0), (ux0,uy0,uz0) : ("<<x[0]<<","<<y[0]<<","<<z[0]<<"), ("
            //         <<ux[0]<<","<<uy[0]<<","<<uz[0]<<")"<<std::endl;
            //std::cout<<"(x1,y1,z1), (ux1,uy1,uz1) : ("<<x[1]<<","<<y[1]<<","<<z[1]<<"), ("
            //         <<ux[1]<<","<<uy[1]<<","<<uz[1]<<")"<<std::endl;
            //std::cout<<"(x2,y2,z2), (ux2,uy2,uz2) : ("<<x[2]<<","<<y[2]<<","<<z[2]<<"), ("
            //         <<ux[2]<<","<<uy[2]<<","<<uz[2]<<")"<<std::endl;

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
      } //endof y loop
    } //end of x loop
  } //end of p loop

  std::cout<<"done."<<std::endl<<"generated "<<gridnum<<" trajectories."<<std::endl;
  rootfile->Write(0, TObject::kOverwrite);
  rootfile->Close();
  delete rootfile;

  delete trajectory;
  delete B_Field;

  return 0;
}
