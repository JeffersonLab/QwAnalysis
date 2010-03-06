//
//  date: Sun Nov 29 22:28:23 CST 2009
//
//  jpan: QTOR field map reading class.
//        Modified from the Qweak geant4 simulation code
//

#include "QwMagneticField.h"

// Boost headers
#ifdef __USE_BOOST_IOSTREAMS
// Boost IOstreams headers
// There is support for gzipped iostreams as magnetic field maps.  Compile with
// -D __USE_BOOST_IOSTREAMS
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/file.hpp>
#endif

// ROOT headers
#include "TMath.h"

// Qweak headers
#include "QwLog.h"
#include "QwUnits.h"

/**
 * Default constructor with optional field map
 */
QwMagneticField::QwMagneticField(const std::string filename)
  :fInvertX(false),fInvertY(false),fInvertZ(false)
{
  QwDebug << "###### Calling QwMagneticField::QwMagneticField " << QwLog::endl;

  // initialize field map parameters
  // here: from the QTOR field map file

  rMinFromMap =     2.0;
  rMaxFromMap =   300.0;

  // new field map boundaries
  phiMinFromMap =    0.0;
  phiMaxFromMap =  359.0;

  zMinFromMap = -250.0;
  zMaxFromMap =  250.0;

  gridstepsize_r   = 2.0;
  gridstepsize_phi = 1.0;
  gridstepsize_z   = 2.0;

  fUnitBfield = Qw::kG; // units of new field map, tested it (1kG = 0.1T)
                        // used field unit tesla withs results in a too strong field

  // initialize variables
  //Initialize();

  // Set some initial variables
  fInterpolationMethod = kTrilinearInterpolation; // trilinear interpolation
  fDataReductionFactor = 1; // no data reduction
  fFieldScalingFactor = 1.0; // no scaling

  // Initialize the grid
  InitializeGrid();

  // Load field map if requested
  if (filename.length() > 0) ReadFieldMapFile(filename);

  QwDebug << "###### Leaving QwMagneticField::QwMagneticField " << QwLog::endl;
}


/**
 * Initialize the grid size and boundaries
 */
void QwMagneticField::InitializeGrid()
{
  QwDebug << "###### Calling QwMagneticField::Initialize " << QwLog::endl;

  gridstepsize_r   *= fDataReductionFactor;
  gridstepsize_phi *= fDataReductionFactor;
  gridstepsize_z   *= fDataReductionFactor;

  nGridPointsInR   = int ( (rMaxFromMap   - rMinFromMap)   / gridstepsize_r  ) + 1;
  nGridPointsInZ   = int ( (zMaxFromMap   - zMinFromMap)   / gridstepsize_z  ) + 1;
  nGridPointsInPhi = int ( (phiMaxFromMap - phiMinFromMap) / gridstepsize_phi) + 1;

  fGridSize = nGridPointsInR * nGridPointsInPhi * nGridPointsInZ;

  BFieldGridData_X.clear();
  BFieldGridData_Y.clear();
  BFieldGridData_Z.clear();
  BFieldGridData_R.clear();
  BFieldGridData_Phi.clear();

  BFieldGridData_X.resize(fGridSize);
  BFieldGridData_Y.resize(fGridSize);
  BFieldGridData_Z.resize(fGridSize);
  BFieldGridData_R.resize(fGridSize);
  BFieldGridData_Phi.resize(fGridSize);

  // Set the field values to zero
  for (int i = 0; i < fGridSize; i++) {
    BFieldGridData_X[i]   = 0.0;
    BFieldGridData_Y[i]   = 0.0;
    BFieldGridData_Z[i]   = 0.0;
    BFieldGridData_R[i]   = 0.0;
    BFieldGridData_Phi[i] = 0.0;
  }

  QwDebug << "###### Leaving QwMagneticField::Initialize " << QwLog::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
const bool QwMagneticField::ReadFieldMapFile(const std::string filename)
{
  // Open the field map file
  std::ifstream inputfile;
  inputfile.open(filename.c_str(), std::ios_base::in);
  // Check for success
  if (!inputfile.good()) {
    QwError << "Could not open field map file!" << QwLog::endl;
    QwError << "File name: " << filename << QwLog::endl;
    return false;
  }

  // Read the input field map stream
  return ReadFieldMap(inputfile);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
const bool QwMagneticField::ReadFieldMapZip(const std::string filename)
{
#ifdef __USE_BOOST_IOSTREAMS
  // Create a gzip filter for the field map file
  boost::iostreams::filtering_istream inputfile;
  inputfile.push(boost::iostreams::gzip_decompressor());
  inputfile.push(boost::iostreams::file_source(filename));

  // Read the input field map stream
  return ReadFieldMap(inputfile);
#else
  QwWarning << "Compressed input files not supported!" << QwLog::endl;
  return false;
#endif
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
const bool QwMagneticField::ReadFieldMap(std::istream& input)
{
  QwDebug << "###### Calling QwMagneticField::ReadFieldMap " << QwLog::endl;

  int r_index;
  int phi_index;
  int z_index;
  int oct;
  r_index = 0;
  phi_index = 0;
  z_index = 0;
  oct = 0;
  int ind = 0;

  int r = 0, z = 0, phi = 0;
  field_t bx = 0.0, by = 0.0, bz = 0.0, br = 0.0, bphi = 0.0;


  // Check for stream
  if (!input.good()) {
    QwError << "Error: Could not open field map stream!" << QwLog::endl;
    return false;
  }


  QwMessage << "------------------------------" << QwLog::endl;
  QwMessage << " Magnetic field interpolation " << QwLog::endl;
  QwMessage << "------------------------------" << QwLog::endl;

  unsigned int entries = 0;
  while (input.good()) {

    // Read a line of data (into integer position and float/double field)
    input >> r >> z >> phi >> bx >> by >> bz;

    // Calculate the grid-normalized coordinates
    double r_grid   = (r   - rMinFromMap)   / gridstepsize_r;
    double phi_grid = (phi - phiMinFromMap) / gridstepsize_phi;
    double z_grid   = (z   - zMinFromMap)   / gridstepsize_z;

    // Calculate the local coordinates and cell position
    double r_int, phi_int, z_int;
    double r_local   = modf(r_grid,   &r_int);
    double phi_local = modf(phi_grid, &phi_int);
    double z_local   = modf(z_grid,   &z_int);
    unsigned int ind_r   = static_cast<int>(r_int);
    unsigned int ind_phi = static_cast<int>(phi_int);
    unsigned int ind_z   = static_cast<int>(z_int);

    // Data reduction: discard if the point is not on a grid node
    if (fabs(z_local)   > 0.1) continue;
    if (fabs(r_local)   > 0.1) continue;
    if (fabs(phi_local) > 0.1) continue;

    // Calculate the index
    ind = Index(ind_r, ind_phi, ind_z);

    // Expand the field map if necessary
    if (ind >= fGridSize) {
      unsigned int oldsize = 0;
      oldsize = fGridSize;
      unsigned int newsize = (int)(1.10 * fGridSize) + 1;
      //QwDebug << "[QwMagneticField] Warning: Need to resize the field map"
      //                      << " from " << oldsize
      //                      << " to "   << newsize
      //                      << " at index " << ind << QwLog::endl;
      fGridSize = newsize;
      BFieldGridData_X.resize(newsize);
      BFieldGridData_Y.resize(newsize);
      BFieldGridData_Z.resize(newsize);
      BFieldGridData_R.resize(newsize);
      BFieldGridData_Phi.resize(newsize);
    }

    // Calculate the radial field components
    br =    bx * TMath::Cos(phi * TMath::DegToRad()) + by * TMath::Sin(phi * TMath::DegToRad());
    bphi = -bx * TMath::Sin(phi * TMath::DegToRad()) + by * TMath::Cos(phi * TMath::DegToRad());

    // Store the field components after scaling
    BFieldGridData_X[ind]   = bx   * fUnitBfield * fFieldScalingFactor;
    BFieldGridData_Y[ind]   = by   * fUnitBfield * fFieldScalingFactor;
    BFieldGridData_Z[ind]   = bz   * fUnitBfield * fFieldScalingFactor;
    BFieldGridData_R[ind]   = br   * fUnitBfield * fFieldScalingFactor;
    BFieldGridData_Phi[ind] = bphi * fUnitBfield * fFieldScalingFactor;

    //QwDebug << "bx by bz :" << bx << ", " << by << ", " << bz << QwLog::endl;

    // Progress bar (TODO This doesn't work if you guess the grid size wrong)
    if (entries % (fGridSize / 10) == 0)
      QwMessage << 100 * entries / fGridSize << "%" << std::flush;
    if (entries % (fGridSize / 10) != 0 && entries % (fGridSize / 40) == 0)
      QwMessage << "." << std::flush;

    entries++;
  };
  QwMessage << QwLog::endl;

  QwMessage << "Read from stream " << entries << " entries" << QwLog::endl;
  QwMessage << "The grid size is " << fGridSize << QwLog::endl;

  QwDebug << "###### Leaving QwMagneticField::ReadFieldMap " << QwLog::endl;

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
QwMagneticField::~QwMagneticField()
{
  // stub
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void QwMagneticField::GetFieldValueFromGridCell( const int GridPoint_R,
							const int GridPoint_Phi,
							const int GridPoint_Z,
							double *BFieldGridValue ) const
{
  // get the value of the cell ix, iy,iz
//   BFieldGridValue[0] = BFieldGridData_X[GridPoint_R][GridPoint_Phi][GridPoint_Z]  ;
//   BFieldGridValue[1] = BFieldGridData_Y[GridPoint_R][GridPoint_Phi][GridPoint_Z]  ;
//   BFieldGridValue[2] = BFieldGridData_Z[GridPoint_R][GridPoint_Phi][GridPoint_Z]  ;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void QwMagneticField::GetCartesianFieldValue(const double point[3], double *field, double scalefactor) const
{
  // Calculate radius and phi
  double r = sqrt(point[0] * point[0] + point[1] * point[1]);
  double z = point[2];

  // Field map boundaries in z
  if (z < zMinFromMap || z > zMaxFromMap){
    field[0] = 0.0;
    field[1] = 0.0;
    field[2] = 0.0;
    return;
  }
  // Field map boundaries in r
  if((r < rMinFromMap * rMinFromMap) || (r > rMaxFromMap * rMinFromMap)){
    field[0] = 0.0;
    field[1] = 0.0;
    field[2] = 0.0;
    return;
  }

  double trPhy = TMath::Pi() / 8.0;
  double fpoint[3] = { point[0] * TMath::Sin(trPhy) + point[1] * TMath::Cos(trPhy),
                      -point[0] * TMath::Cos(trPhy) + point[1] * TMath::Sin(trPhy),
                       point[2] };


  double phi = TMath::ATan2(fpoint[1], fpoint[0]) * TMath::RadToDeg();
  if (phi < 0) phi += 360;

  double rpoint[3] = { r, phi, z };

  double field_xyzrf[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  switch (fInterpolationMethod) {
    case kTrilinearInterpolation:
      CalculateTrilinear(rpoint, field_xyzrf);
      break;
    case kNearestNeighborInterpolation:
      CalculateNearestNeighbor(rpoint, field_xyzrf);
      break;
    default:
      break;
  }

  // Field rotation
  field[0] = field_xyzrf[0] * TMath::Sin(trPhy) - field_xyzrf[1] * TMath::Cos(trPhy);
  field[1] = field_xyzrf[0] * TMath::Cos(trPhy) + field_xyzrf[1] * TMath::Sin(trPhy);
  field[2] = field_xyzrf[2];

  // Field scaling
  field[0] *= scalefactor;
  field[1] *= scalefactor;
  field[2] *= scalefactor;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void QwMagneticField::GetCylindricalFieldValue(const double point[3], double *field, double scalefactor) const
{
  // Calculate radius and phi
  double xyRadius = sqrt(point[0] * point[0] + point[1] * point[1]);
  double z = point[2];

  // Field map boundaries in z
  if (z < zMinFromMap || z > zMaxFromMap){
    field[0] = 0.0;
    field[1] = 0.0;
    field[2] = 0.0;
    return;
  }
  // Field map boundaries in r
  if(xyRadius < rMinFromMap || xyRadius > rMaxFromMap){
    field[0] = 0.0;
    field[1] = 0.0;
    field[2] = 0.0;
    return;
  }

  double trPhy = TMath::Pi() / 8.0;
  double fpoint[3] = { point[0] * TMath::Sin(trPhy) + point[1] * TMath::Cos(trPhy),
                      -point[0] * TMath::Cos(trPhy) + point[1] * TMath::Sin(trPhy),
                       point[2] };


  double phi = TMath::ATan2(fpoint[1], fpoint[0]) * TMath::RadToDeg();
  if (phi < 0) phi += 360;

  double rpoint[3] = { xyRadius, phi, z };

  double field_xyzrf[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
  switch (fInterpolationMethod) {
    case kTrilinearInterpolation:
      CalculateTrilinear(rpoint, field_xyzrf);
      break;
    case kNearestNeighborInterpolation:
      CalculateNearestNeighbor(rpoint, field_xyzrf);
      break;
    default:
      break;
  }

  // Field scaling (TODO no support for rotation yet)
  field[0] = field_xyzrf[3] * scalefactor;
  field[1] = field_xyzrf[4] * scalefactor;
  field[2] = field_xyzrf[2] * scalefactor;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void QwMagneticField::CalculateTrilinear(const double point[3], double *field) const
{
  double r = point[0];
  double phi = point[1];
  double z = point[2];

  // Calculate the grid-normalized coordinates
  double r_grid   = (r   - rMinFromMap)   / gridstepsize_r;
  double phi_grid = (phi - phiMinFromMap) / gridstepsize_phi;
  double z_grid   = (z   - zMinFromMap)   / gridstepsize_z;

  // Calculate the local coordinates and cell position
  double r_int, phi_int, z_int;
  double r_local   = modf(r_grid,   &r_int);
  double phi_local = modf(phi_grid, &phi_int);
  double z_local   = modf(z_grid,   &z_int);
  unsigned int ind_r   = static_cast<int>(r_int);
  unsigned int ind_phi = static_cast<int>(phi_int);
  unsigned int ind_z   = static_cast<int>(z_int);

//   printf("r_rem %f, phi_rem %f, z_rem %f\n",r_rem,phi_rem,z_rem);

  int ind1 = Index(ind_r, ind_phi, ind_z);

//   printf("ind 1 = %d, r = %f, z = %f, phi = %f\n",ind1, xyRadius - r_rem, z - z_rem, phi - phi_rem);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind1],BFieldGridData_Y[ind1],BFieldGridData_Z[ind1]);

  int ind2 = Index(ind_r, ind_phi, ind_z + 1);

//   printf("ind 2 = %d, r = %f, z = %f, phi = %f\n",ind2, xyRadius - r_rem, z - z_rem + gridstepsize_z, phi - phi_rem);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind2],BFieldGridData_Y[ind2],BFieldGridData_Z[ind2]);

  int ind3 = Index(ind_r, ind_phi + 1, ind_z);

//   printf("ind 3 = %d, r = %f, z = %f, phi = %f\n",ind3, xyRadius - r_rem, z - z_rem, phi - phi_rem + gridstepsize_phi);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind3],BFieldGridData_Y[ind3],BFieldGridData_Z[ind3]);

  int ind4 = Index(ind_r, ind_phi + 1, ind_z + 1);

//   printf("ind 4 = %d, r = %f, z = %f, phi = %f\n",ind4, xyRadius - r_rem, z - z_rem + gridstepsize_z, phi - phi_rem + gridstepsize_phi);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind4],BFieldGridData_Y[ind4],BFieldGridData_Z[ind4]);

  int ind5 = Index(ind_r + 1, ind_phi, ind_z);

//   printf("ind 5 = %d, r = %f, z = %f, phi = %f\n",ind5, xyRadius - r_rem + gridstepsize_r, z - z_rem, phi - phi_rem);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind5],BFieldGridData_Y[ind5],BFieldGridData_Z[ind5]);

  int ind6 = Index(ind_r + 1, ind_phi, ind_z + 1);

//   printf("ind 6 = %d, r = %f, z = %f, phi = %f\n",ind6, xyRadius - r_rem + gridstepsize_r, z - z_rem + gridstepsize_z, phi - phi_rem);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind6],BFieldGridData_Y[ind6],BFieldGridData_Z[ind6]);

  int ind7 = Index(ind_r + 1, ind_phi + 1, ind_z);

//   printf("ind 7 = %d, r = %f, z = %f, phi = %f\n",ind7, xyRadius - r_rem + gridstepsize_r, z - z_rem, phi - phi_rem + gridstepsize_phi);
//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind7],BFieldGridData_Y[ind7],BFieldGridData_Z[ind7]);

  int ind8 = Index(ind_r + 1, ind_phi + 1, ind_z + 1);


  if(ind1 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind2 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind3 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind4 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind5 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind6 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind7 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi) ||
     ind8 >= (int)(nGridPointsInR*nGridPointsInZ*nGridPointsInPhi)){

    field[0] = 0.0;
    field[1] = 0.0;
    field[2] = 0.0;
    field[3] = 0.0;
    field[4] = 0.0;
    return;
  }

  double Bx_ANSYS, By_ANSYS, Bz_ANSYS, Br_ANSYS, Bphi_ANSYS;

  // Full 3-dimensional version: trilinear interpolation
  Bx_ANSYS =
    BFieldGridData_X[ind1] * (1-r_local) * (1-phi_local) * (1-z_local) +
    BFieldGridData_X[ind2] * (1-r_local) * (1-phi_local) *    z_local  +
    BFieldGridData_X[ind3] * (1-r_local) *    phi_local  * (1-z_local) +
    BFieldGridData_X[ind4] * (1-r_local) *    phi_local  *    z_local  +
    BFieldGridData_X[ind5] *    r_local  * (1-phi_local) * (1-z_local) +
    BFieldGridData_X[ind6] *    r_local  * (1-phi_local) *    z_local  +
    BFieldGridData_X[ind7] *    r_local  *    phi_local  * (1-z_local) +
    BFieldGridData_X[ind8] *    r_local  *    phi_local  *    z_local ;

  By_ANSYS =
    BFieldGridData_Y[ind1] * (1-r_local) * (1-phi_local) * (1-z_local) +
    BFieldGridData_Y[ind2] * (1-r_local) * (1-phi_local) *    z_local  +
    BFieldGridData_Y[ind3] * (1-r_local) *    phi_local  * (1-z_local) +
    BFieldGridData_Y[ind4] * (1-r_local) *    phi_local  *    z_local  +
    BFieldGridData_Y[ind5] *    r_local  * (1-phi_local) * (1-z_local) +
    BFieldGridData_Y[ind6] *    r_local  * (1-phi_local) *    z_local  +
    BFieldGridData_Y[ind7] *    r_local  *    phi_local  * (1-z_local) +
    BFieldGridData_Y[ind8] *    r_local  *    phi_local  *    z_local ;

  Bz_ANSYS =
    BFieldGridData_Z[ind1] * (1-r_local) * (1-phi_local) * (1-z_local) +
    BFieldGridData_Z[ind2] * (1-r_local) * (1-phi_local) *    z_local  +
    BFieldGridData_Z[ind3] * (1-r_local) *    phi_local  * (1-z_local) +
    BFieldGridData_Z[ind4] * (1-r_local) *    phi_local  *    z_local  +
    BFieldGridData_Z[ind5] *    r_local  * (1-phi_local) * (1-z_local) +
    BFieldGridData_Z[ind6] *    r_local  * (1-phi_local) *    z_local  +
    BFieldGridData_Z[ind7] *    r_local  *    phi_local  * (1-z_local) +
    BFieldGridData_Z[ind8] *    r_local  *    phi_local  *    z_local ;

  Br_ANSYS =
    BFieldGridData_R[ind1] * (1-r_local) * (1-phi_local) * (1-z_local) +
    BFieldGridData_R[ind2] * (1-r_local) * (1-phi_local) *    z_local  +
    BFieldGridData_R[ind3] * (1-r_local) *    phi_local  * (1-z_local) +
    BFieldGridData_R[ind4] * (1-r_local) *    phi_local  *    z_local  +
    BFieldGridData_R[ind5] *    r_local  * (1-phi_local) * (1-z_local) +
    BFieldGridData_R[ind6] *    r_local  * (1-phi_local) *    z_local  +
    BFieldGridData_R[ind7] *    r_local  *    phi_local  * (1-z_local) +
    BFieldGridData_R[ind8] *    r_local  *    phi_local  *    z_local ;

  Bphi_ANSYS =
    BFieldGridData_Phi[ind1] * (1-r_local) * (1-phi_local) * (1-z_local) +
    BFieldGridData_Phi[ind2] * (1-r_local) * (1-phi_local) *    z_local  +
    BFieldGridData_Phi[ind3] * (1-r_local) *    phi_local  * (1-z_local) +
    BFieldGridData_Phi[ind4] * (1-r_local) *    phi_local  *    z_local  +
    BFieldGridData_Phi[ind5] *    r_local  * (1-phi_local) * (1-z_local) +
    BFieldGridData_Phi[ind6] *    r_local  * (1-phi_local) *    z_local  +
    BFieldGridData_Phi[ind7] *    r_local  *    phi_local  * (1-z_local) +
    BFieldGridData_Phi[ind8] *    r_local  *    phi_local  *    z_local ;

  field[0] = Bx_ANSYS;
  field[1] = By_ANSYS;
  field[2] = Bz_ANSYS;
  field[3] = Br_ANSYS;
  field[4] = Bphi_ANSYS;

//   printf("Bx %f, By %f, Bz %f\n",BFieldGridData_X[ind1],BFieldGridData_Y[ind1],BFieldGridData_Z[ind1]);
//   printf("Bix %f, Biy %f, Biz %f\n",Bx_ANSYS,By_ANSYS,Bz_ANSYS);
//   printf("Btx %f, Bty %f, Btz %f\n",Bfield[0],Bfield[1],Bfield[2]);

} // end of QwMagneticField::CalculateTrilinear()

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void QwMagneticField::CalculateNearestNeighbor(const double point[3], double *field) const
{
  double r = point[0];
  double phi = point[1];
  double z = point[2];

  // Calculate the grid-normalized coordinates
  double r_grid   = (r   - rMinFromMap)   / gridstepsize_r;
  double phi_grid = (phi - phiMinFromMap) / gridstepsize_phi;
  double z_grid   = (z   - zMinFromMap)   / gridstepsize_z;

  // Calculate the local coordinates and cell position
  double r_int, phi_int, z_int;
  double r_local   = modf(r_grid,   &r_int);
  double phi_local = modf(phi_grid, &phi_int);
  double z_local   = modf(z_grid,   &z_int);
  unsigned int ind_r   = static_cast<int>(r_int);
  unsigned int ind_phi = static_cast<int>(phi_int);
  unsigned int ind_z   = static_cast<int>(z_int);

  // Fast 3-dimensional nearest neighbor
  if (r_local > 0.5)   ind_r++;
  if (phi_local > 0.5) ind_phi++;
  if (z_local > 0.5)   ind_z++;

  unsigned int ind = Index(ind_r, ind_phi, ind_z);

  if (ind >= (nGridPointsInR * nGridPointsInZ * nGridPointsInPhi)) {
    field[0] = 0.0;
    field[1] = 0.0;
    field[2] = 0.0;
    field[3] = 0.0;
    field[4] = 0.0;
    return;
  }

  field[0] = BFieldGridData_X[ind];
  field[1] = BFieldGridData_Y[ind];
  field[2] = BFieldGridData_Z[ind];
  field[3] = BFieldGridData_R[ind];
  field[4] = BFieldGridData_Phi[ind];

} // QwMagneticField::CalculateNearestNeighbor()

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
