#include "QwMatrixLookup.h"

// ROOT headers
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TStopwatch.h>

// ROOT Math Interpolator headers
#if defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)
#include <Math/Interpolator.h>
#endif

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"
#include "QwOptions.h"
#include "QwRayTracer.h"
#include "QwInterpolator.h"
#include "QwTrack.h"
#include "QwPartialTrack.h"

/**
 * Constructor
 */
QwMatrixLookup::QwMatrixLookup(QwOptions& options)
{
  // Set the front and back reference plane locations for which the track
  // parameters are stored in the trajectory matrix.
  fFrontRefPlane = -250.0 * Qw::cm;
  fBackRefPlane  =  570.0 * Qw::cm;

  // Look-up table parameters, unless overriden when read in from file

  // Track momentum
  double P_MAX   = 1180.0 * Qw::MeV;
  double P_MIN   = 980.0 * Qw::MeV;
  double DP      = 10.0 * Qw::MeV;

  // Target vertex longitudinal position
  // NOTE This is equivalent to the polar angle theta at the reference plane
  double Z_MAX   = -630.0 * Qw::cm;
  double Z_MIN   = -670.0 * Qw::cm;
  double DZ      = 2.0 * Qw::cm;

  // Position radius at front reference plane
  double R_MAX   = 100.0 * Qw::cm;
  double R_MIN   = 30.0 * Qw::cm;
  double DR      = 1.0 * Qw::cm;

  // Azimuthal angle at front reference plane
  double PHI_MAX = 360.0 * Qw::deg;
  double PHI_MIN = 0.0 * Qw::deg;
  double DPHI    = 1.0 * Qw::deg;

  // Create new interpolator from 4 float coordinates to 4 output values
  fMatrix = new QwInterpolator<float,4>(4);
  fMatrix->SetInterpolationMethod(kNearestNeighbor);

  // The optimal storage is to have all momentum points close together (p < z < r < phi)
  //fMin.push_back(P_MIN);   fMax.push_back(P_MAX);   fStep.push_back(DP);
  //fMin.push_back(Z_MIN);   fMax.push_back(Z_MAX);   fStep.push_back(DZ);
  //fMin.push_back(R_MIN);   fMax.push_back(R_MAX);   fStep.push_back(DR);
  //fMin.push_back(PHI_MIN); fMax.push_back(PHI_MAX); fStep.push_back(DPHI);

  // But the ROOT file currently has this differently (z < phi < r < p)
  fMin.push_back(Z_MIN);   fMax.push_back(Z_MAX);   fStep.push_back(DZ);
  fMin.push_back(PHI_MIN); fMax.push_back(PHI_MAX); fStep.push_back(DPHI);
  fMin.push_back(R_MIN);   fMax.push_back(R_MAX);   fStep.push_back(DR);
  fMin.push_back(P_MIN);   fMax.push_back(P_MAX);   fStep.push_back(DP);

  fMatrix->SetMinimumMaximumStep(fMin,fMax,fStep);
}

/**
 * Destructor
 */
QwMatrixLookup::~QwMatrixLookup()
{
  delete fMatrix;
}

/**
 * Load the trajectory matrix
 * @param filename Filename
 * @return True if successful
 */
bool QwMatrixLookup::LoadTrajMatrix(const std::string filename)
{

#if ! defined __ROOT_HAS_MATHMORE || ROOT_VERSION_CODE < ROOT_VERSION(5,18,0)

  // No support for ROOT MathMore: warn user and return failure
  QwWarning << "No support for ROOT::Math::Interpolator was found." << QwLog::endl;
  QwWarning << "Without this support the faster momentum reconstruction method using "
            << "a track look-up table will not work. The momentum reconstruction "
            << "will always happen by slower swimming through the magnetic field."
            << QwLog::endl;
  QwWarning << "The necessary functionality is included in the ROOT MathMore plugin, "
            << "which is included on JLab CUE2 systems in /apps/root/5.26-00."
            << QwLog::endl;
  QwWarning << "No momentum look-up table will be used for track reconstruction."
            << QwLog::endl;
  return false;

#endif

  // Try to access the file
  if (gSystem->AccessPathName(TString(filename))) {
    QwWarning << "Could not find the momentum look-up table!" << QwLog::endl;
    QwWarning << "Filename: " << filename << QwLog::endl;
    QwWarning << "Without this file the faster momentum reconstruction method using "
              << "a track look-up table will not work. The momentum reconstruction "
              << "will always happen by slower swimming through the magnetic field."
              << QwLog::endl;
    QwWarning << "The necessary file is /group/qweak/QwAnalysis/datafiles/QwTrajMatrix.root"
              << QwLog::endl;
    QwWarning << "No momentum look-up table will be used for track reconstruction."
              << QwLog::endl;
    return false;
  }

  // Try to open the file
  TFile* rootfile = new TFile(TString(filename),"read");
  if (! rootfile) {
    QwWarning << "Could not open the momentum look-up table!" << QwLog::endl;
    QwWarning << "Filename: " << filename << QwLog::endl;
    QwWarning << "No momentum look-up table will be used for track reconstruction."
              << QwLog::endl;
    return false;
  }
  rootfile->cd();

  // Try to open the tree
  TTree *momentum_tree = (TTree *)rootfile->Get("Momentum_Tree");
  if (! momentum_tree) {
    QwWarning << "Could not find momentum look-up table in file!" << QwLog::endl;
    QwWarning << "Filename: " << filename << QwLog::endl;
    QwWarning << "No momentum look-up table will be used for track reconstruction."
              << QwLog::endl;
    return false;
  }


  QwMessage << "------------------------------" << QwLog::endl;
  QwMessage << "     Loading lookup table     " << QwLog::endl;
  QwMessage << "------------------------------" << QwLog::endl;

  // Connect branches
  Int_t    index;
  Double_t back_position_r, back_position_phi;
  Double_t back_direction_theta, back_direction_phi;
  momentum_tree->SetBranchAddress("index", &index);
  momentum_tree->SetBranchAddress("position_r", &back_position_r);
  momentum_tree->SetBranchAddress("position_phi", &back_position_phi);
  momentum_tree->SetBranchAddress("direction_theta", &back_direction_theta);
  momentum_tree->SetBranchAddress("direction_phi", &back_direction_phi);

  // Create value array and make references
  float value[4] = {0.0, 0.0, 0.0, 0.0};

  // Loop over all entries
  Int_t numberOfEntries = momentum_tree->GetEntries();
  for (Int_t i = 0; i < numberOfEntries; i++) {

    // Get the entry from the tree
    momentum_tree->GetEntry(i);

    // Fill the value array with stored back reference plane parameters
    value[0] = back_position_r;
    value[1] = back_position_phi;
    value[2] = back_direction_theta;
    value[3] = back_direction_phi;

    // Enter this into the interpolator
    bool status = fMatrix->Set(i, value);
    if (not status)
      QwWarning << "Look-up table entry " << i << " could not be stored." << QwLog::endl;

    // Progress bar
    if (fMatrix->GetCurrentEntries() % (fMatrix->GetMaximumEntries() / 10) == 0)
      QwMessage << fMatrix->GetCurrentEntries() / (fMatrix->GetMaximumEntries() / 100) << "%" << std::flush;
    if (fMatrix->GetCurrentEntries() % (fMatrix->GetMaximumEntries() / 10) != 0
     && fMatrix->GetCurrentEntries() % (fMatrix->GetMaximumEntries() / 40) == 0)
      QwMessage << "." << std::flush;
  }
  QwMessage << "... done." << QwLog::endl;

  // Close file
  rootfile->Close();
  delete rootfile;

  // Return successfully
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

///  Generate a momentum look-up table indexed by momentum, position and direction:
///  a family of trajectories starting from the target to endplane will be generated.
///  Momentum and scattering direction vary for each trajectory. We record the inter-
///  section position and direction at the focal plane (z=570 cm)
///
///  Z coordinate:
///  Z0: start plane = -250 cm, Z1: endplane = +250 cm, Z2: focalplane = +570 cm
///  B field values are available from z=-250 cm to z=250 cm

bool QwMatrixLookup::WriteTrajMatrix(const std::string filename)
{
  Double_t  position_r,position_phi;   //z=570 cm plane
  Double_t  direction_theta,direction_phi;

  // Load the ray tracer
  QwRayTracer* raytracer = new QwRayTracer(gQwOptions);
  // Get the boundaries of the magnetic field
  double magneticfield_min = -250.0 * Qw::cm;
  double magneticfield_max =  250.0 * Qw::cm;

  //open file and set up output tree
  TString rootfilename = TString(filename);
  TFile* rootfile = new TFile(rootfilename,"RECREATE","Qweak momentum matrix");
  rootfile->cd();

  TTree *momentum_tree = new TTree("Momentum_Tree","momentum data tree");

  // position and direction at focal plane
  momentum_tree->Branch("position_r", &position_r, "position_r/D");
  momentum_tree->Branch("position_phi", &position_phi, "position_phi/D");
  momentum_tree->Branch("direction_theta",&direction_theta,"direction_theta/D");
  momentum_tree->Branch("direction_phi",&direction_phi,"direction_phi/D");


  // Create a timer
  TStopwatch timer;
  timer.Start();

  // Step size
  Double_t  step = 1.0 * Qw::cm;

  // Create coordinate and make references
  double coord[4] = {0.0, 0.0, 0.0, 0.0};
  double& z = coord[0];
  double& phi = coord[1];
  double& r = coord[2];
  double& p = coord[3];

  // Calculate all trajectories for the look-up table
  TVector3 position, direction;
  for (unsigned int index = 0; index < fMatrix->GetMaximumEntries(); index++) {

    // Get the coordinates from the grid
    fMatrix->Coord(index, coord);

    // Calculate the polar angle theta at the front reference plane
    double theta = atan2(r,fFrontRefPlane - z);

    // Position of the track at the front reference plane
    position.SetXYZ(r * cos(phi), r * sin(phi), fFrontRefPlane);
    // Direction of the track at the front reference plane
    direction.SetMagThetaPhi(1.0, theta, phi);


    // Extend from the front reference plane to the magnetic field boundary
    position += direction * (magneticfield_min - fFrontRefPlane);

    // Raytrace with momentum p from front to back of magnetic field
    raytracer->IntegrateRK(position, direction, p, fBackRefPlane, 4, step);

    // Extend from the magnetic field boundary to the back reference plane
    position += direction * (fBackRefPlane - magneticfield_max);


    // Determine the back reference plane parameters
    position_r = position.Perp();
    position_phi = position.Phi();
    if (position_phi < 0.0) position_phi += 2.0 * Qw::pi;
    direction_phi = direction.Phi();
    if (direction_phi < 0.0) direction_phi += 2.0 * Qw::pi;
    direction_theta = direction.Theta();

    // Add the back reference plane parameters to the look-up table
    float value[4] = {(float) position_r, (float) position_phi, (float) direction_phi, (float) direction_theta};
    fMatrix->Set(coord, value);

    // Progress bar
    if (fMatrix->GetCurrentEntries() % (fMatrix->GetMaximumEntries() / 10) == 0)
      QwMessage << fMatrix->GetCurrentEntries() / (fMatrix->GetMaximumEntries() / 100) << "%" << std::flush;
    if (fMatrix->GetCurrentEntries() % (fMatrix->GetMaximumEntries() / 10) != 0
     && fMatrix->GetCurrentEntries() % (fMatrix->GetMaximumEntries() / 40) == 0)
      QwMessage << "." << std::flush;
  }
  QwMessage << QwLog::endl;

  // Stop timer
  timer.Stop();

  // Timing output
  QwMessage << "CPU time used:  " << timer.CpuTime() << " s "
            << "(" << timer.CpuTime() / fMatrix->GetCurrentEntries() << " s per trajectory)" << QwLog::endl;
  QwMessage << "Real time used: " << timer.RealTime() << " s "
            << "(" << timer.RealTime() / fMatrix->GetCurrentEntries() << " s per trajectory)" << QwLog::endl;

  // Close file
  rootfile->Write(0, TObject::kOverwrite);
  rootfile->Close();

  delete rootfile;
  delete raytracer;

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

/**
 * Bridge the front and back partial tracks using the momentum look-up table
 * @param front Front partial track
 * @param back Back partial tracks
 * @return List of reconstructed tracks
 */
const QwTrack* QwMatrixLookup::Bridge(
        const QwPartialTrack* front,
        const QwPartialTrack* back)
{
#if ! defined __ROOT_HAS_MATHMORE || ROOT_VERSION_CODE < ROOT_VERSION(5,18,0)

    // Return immediately if there is no support for ROOT::Math::Interpolator
    return 0;

#endif

    // No track found yet
    QwTrack* track = 0;

    // Return immediately if there is no look-up table
    if (! fMatrix) return track;


    // Front track position and direction at the front reference plane
    TVector3 front_position  = front->GetPosition(fFrontRefPlane);
    TVector3 front_direction = front->GetMomentumDirection();
    double front_position_r   = front_position.Perp();
    double front_position_phi = front_position.Phi();
    // TODO this should be a method of the partial track
    double vertex_z = fFrontRefPlane - front_position_r / tan(front->GetMomentumDirection().Theta());

    // Check whether this front track is in the look-up table
    double momentum = 1.165 * Qw::GeV;
    double coord[4] = {vertex_z, front_position_phi, front_position_r, momentum};
    if (! fMatrix->InBounds(coord)) {
        QwMessage << "vertex_z = " << vertex_z/Qw::cm << " cm" << QwLog::endl;
        QwMessage << "front_position_r = " << front_position_r/Qw::cm << " cm" << QwLog::endl;
        QwMessage << "front_position_phi = " << front_position_phi/Qw::deg << " deg" << QwLog::endl;
        QwMessage << "This potential track is not listed in the table."<<QwLog::endl;
        return track;
    }

    // Back track position and direction at the back reference plane
    TVector3 actual_back_position  = back->GetPosition(fBackRefPlane);
    TVector3 actual_back_direction = back->GetMomentumDirection();
    double actual_back_position_r = actual_back_position.Perp();

    //
    double p_min = fMatrix->GetMinimum(3);
    double p_max = fMatrix->GetMaximum(3);
    double dp = fMatrix->GetStepSize(3);
    double np = (p_max - p_min) / dp + 1.0;

    // Build two subsets of the table
    std::vector<double> iP;  // hold momentum for interpolation
    std::vector<double> iR;  // hold radial position for interpolation

    iP.resize(np); iR.resize(np);

    // NOTE jpan: ROOT::Math::GSLInterpolator::Eval(double) requires that
    // x values must be monotonically increasing.

    double value[4] = {0.0, 0.0, 0.0, 0.0};
    unsigned int index = 0;
    for (double p = p_max; p >= p_min; p -= dp) {

        coord[3] = p;
        fMatrix->GetValue(coord, value);

        iP[index] = p;
        iR[index] = value[0];
        index++;
    }

    // NOTE The dr/dp on focal plane is about 1 cm per 10 MeV

    if ( (iR.front() < actual_back_position_r) &&
         (iR.back()  > actual_back_position_r) ) {
        QwDebug << "No match in look-up table!" << QwLog::endl;
        return track;
    }

    // The hit is within the momentum limits, do interpolation for momentum
    momentum = 0.0;

    // We can choose among the following interpolation methods:
    //  LINEAR, POLYNOMIAL, CSPLINE, CSPLINE_PERIODIC, AKIMA, AKIMA_PERIODIC

    #if defined __ROOT_HAS_MATHMORE && ROOT_VERSION_CODE >= ROOT_VERSION(5,18,0)

      #if ROOT_VERSION_CODE >= ROOT_VERSION(5,22,0)
        // Newer version of the MathMore plugin use kPOLYNOMIAL
        UInt_t size = iP.size();
        ROOT::Math::Interpolator inter(size, ROOT::Math::Interpolation::kPOLYNOMIAL);
      #else
        // Older version of the MathMore plugin use POLYNOMIAL
        UInt_t size = iP.size();
        ROOT::Math::Interpolator inter(size, ROOT::Math::Interpolation::POLYNOMIAL);
      #endif

      inter.SetData(iR, iP);
      momentum = inter.Eval(actual_back_position_r/Qw::cm);  // + 0.0016;  // [GeV]

    #else // ! defined __ROOT_HAS_MATHMORE || ROOT_VERSION_CODE < ROOT_VERSION(5,18,0)

      // Should never get here
      momentum = 0.0;

    #endif // ! defined __ROOT_HAS_MATHMORE || ROOT_VERSION_CODE < ROOT_VERSION(5,18,0)

    QwMessage << momentum/Qw::GeV << " GeV" << QwLog::endl;


    // take the track parameter from the nearest track
    if (momentum < 0.98 * Qw::GeV || momentum > 1.165 * Qw::GeV) {
        QwMessage << "Out of momentum range: determined momentum by looking up table: "
                  << momentum/Qw::GeV << " GeV" << QwLog::endl;
        return track;
    }

    // Get the values in the table (and assign units)
    coord[3] = momentum;
    fMatrix->GetValue(coord, value);
    double& back_position_r = value[0]; back_position_r *= Qw::cm;
    double& back_position_phi = value[1]; back_position_phi *= Qw::deg;
    double& back_direction_theta = value[2]; back_direction_theta *= Qw::deg;
    double& back_direction_phi = value[3]; back_direction_phi *= Qw::deg;

    // Get the hit parameters
    double x = back_position_r * cos(back_position_phi);
    double y = back_position_r * sin(back_position_phi);
    TVector3 back_position = TVector3(x,y,fBackRefPlane);
    TVector3 back_direction;
    back_direction.SetMagThetaPhi(1.0, back_direction_theta, back_direction_phi);

    // Calculate the differences
    double diff_position_r = actual_back_position.Perp() - back_position.Perp();
    double diff_position_phi = actual_back_position.Phi() - back_position.Phi();
    double diff_direction_phi = actual_back_direction.Phi() - back_direction.Phi();
    double diff_direction_theta = actual_back_direction.Theta() - back_direction.Theta();
    if (fabs(diff_position_r) > 2.0 * Qw::cm
     || fabs(diff_position_phi) > 4.0 * Qw::deg
     || fabs(diff_direction_phi) > 4.0 * Qw::deg
     || fabs(diff_direction_theta) > 1.0 * Qw::deg ) {

        QwMessage << "Didn't find a good match in the look-up table" << QwLog::endl;
        QwMessage << "diff_position_r = " << diff_position_r/Qw::cm << " cm" << QwLog::endl;
        QwMessage << "diff_position_phi = " << diff_position_phi/Qw::deg << " deg" << QwLog::endl;
        QwMessage << "diff_direction_phi = " << diff_direction_phi/Qw::deg << " deg" << QwLog::endl;
        QwMessage << "diff_direction_theta = " << diff_direction_theta/Qw::deg << " deg" << QwLog::endl;
        return track;
    }

    track = new QwTrack(front,back);
    track->fMomentum = momentum;

    track->fStartPosition = front_position;
    track->fStartDirection = front_direction;
    track->fEndPositionGoal = back_position;
    track->fEndDirectionGoal = back_direction;
    track->fEndPositionActual = actual_back_position;
    track->fEndDirectionActual = actual_back_direction;



    //Int_t fMatchFlag; // MatchFlag = -2 : cannot match
                      // MatchFlag = -1 : potential track cannot pass through the filter
                      // MatchFlag = 0; : matched with look-up table
                      // MatchFlag = 1; : matched by using shooting method
                      // MatchFlag = 2; : potential track is forced to match

    return track;
}
