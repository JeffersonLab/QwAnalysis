/*! \file   QwTrajMatrix.cc
 *
 *  \author Jie Pan <jpan@jlab.org>
 *  \author Wouter Deconinck <wdconinc@mit.edu>
 *  \date   Sun Feb 14 2010
 *
 *  \brief  Generate the momentum matrix and define the searching method
 */

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "QwTrajMatrix.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

    QwTrajMatrix::QwTrajMatrix(){};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

    QwTrajMatrix::~QwTrajMatrix(){};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

///  Generate a momentum look-up table indexed by momentum, position and direction:
///  a family of trajectories starting from the target to endplane will be generated.
///  Momentum and scattering direction vary for each trajectory. We record the inter-
///  section position and direction at the focal plane (z=570 cm)
///
///  Z coordinate:
///  Z0: start plane = -250 cm, Z1: endplane = +250 cm, Z2: focalplane = +570 cm
///  B field values are available from z=-250 cm to z=250 cm

void QwTrajMatrix::GenerateTrajMatrix() {

    // local variables
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

    QwRayTracer* raytracer = new QwRayTracer();
    //raytracer->LoadMagneticFieldMap();

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

                    raytracer->SetStartAndEndPoints(startpoint, startdirection,
                                         endpoint, enddirection);

                    //raytrace from startplane (z=-250 cm) to endplane (z=+250 cm)
                    // p in [GeV] for the integration
                    raytracer->Integrate((double)p*0.001, step_size);
                    x[1] =  raytracer->GetHitLocationX();
                    y[1] =  raytracer->GetHitLocationY();
                    ux[1] = raytracer->GetHitDirectionX();
                    uy[1] = raytracer->GetHitDirectionY();
                    uz[1] = raytracer->GetHitDirectionZ();

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
    delete raytracer;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int QwTrajMatrix::ReadTrajMatrixFile( std::vector <QwPartialTrackParameter> *backtrackparametertable ) {

    Int_t   index;
    Double_t position_r,position_phi;
    Double_t direction_theta,direction_phi;

    TVector3 startpoint,startdirection,endpoint,enddirection;

    //open file
    TString rootfilename=std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QwTrajMatrix.root";

    if (gSystem->AccessPathName(rootfilename)) {
       std::cout <<std::endl<< "Cannot find the momentum look-up table!"<<std::endl;
       std::cout<<"Would you like to generate one? (Y/N)"<<std::endl;
       std::cout<<"It's going to take a while, maybe a few hours, to generate the file. ";

       char choice;
       std::cin >> choice;
       if (choice == 'y' || choice == 'Y'){
          std::cout <<std::endl<<std::endl;
          GenerateTrajMatrix();
       }
       else {
          std::cout <<std::endl<<std::endl<< "Program termined."<<std::endl<<std::endl;
          exit(0);
       }
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
    backtrackparametertable->reserve(numberOfEntries);

    std::cout<<"total grid points : "<<numberOfEntries<<std::endl;

    for ( Int_t i=0; i<numberOfEntries; i++) {
        momentum_tree->GetEntry(i);

        // only z=+570 cm plane data
        backtrackparameter.fPositionR = (float)position_r;
        backtrackparameter.fPositionPhi = (float)position_phi;
        backtrackparameter.fDirectionTheta = (float)direction_theta;
        backtrackparameter.fDirectionPhi = (float)direction_phi;

        backtrackparametertable->push_back(backtrackparameter);
    }

    std::cout<<"...done."<<std::endl;
    rootfile->Close();
    delete rootfile;
    return 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....