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
#include "QwInterpolator.h"

/**
 * Default constructor with optional field map
 */
QwMagneticField::QwMagneticField()
{
  QwDebug << "###### Calling QwMagneticField::QwMagneticField " << QwLog::endl;

  // Initialize field map parameters
  // Here: from the QTOR field map file

  double rMinFromMap =     2.0;
  double rMaxFromMap =   300.0;

  double phiMinFromMap =    0.5; // grrrrr: due to the rotation...
  double phiMaxFromMap =  359.5;

  double zMinFromMap = -250.0;
  double zMaxFromMap =  250.0;

  double gridstepsize_r   = 2.0;
  double gridstepsize_phi = 1.0;
  double gridstepsize_z   = 2.0;

  fUnitBfield = Qw::kG; // units of new field map, tested it (1kG = 0.1T)
                        // used field unit tesla withs results in a too strong field

  // Field map grid parameters
  std::vector<double> min, max, step;
  min.push_back(zMinFromMap);   max.push_back(zMaxFromMap);   step.push_back(gridstepsize_z);
  min.push_back(rMinFromMap);   max.push_back(rMaxFromMap);   step.push_back(gridstepsize_r);
  min.push_back(phiMinFromMap); max.push_back(phiMaxFromMap); step.push_back(gridstepsize_phi);

  // Create the field map interpolator
  fField = new QwInterpolator<float,5>(min,max,step);
  fField->SetInterpolationMethod(kMultiLinear);

  // Scale factor
  SetFieldScalingFactor(1.04); // BFIL
  // Translation
  SetTranslation(0.0 * Qw::cm);
  // Rotation in degrees, half an octant offset
  SetRotation((-90.0 + 22.5) * Qw::deg);

  QwDebug << "###### Leaving QwMagneticField::QwMagneticField " << QwLog::endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/**
 * Read the magnetic field from an ANSYS map file in text format
 * @param filename ANSYS map file name
 * @return True if read successfully
 */
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
/**
 * Read the magnetic field from an ANSYS map file in gzipped text format
 * @param filename ANSYS map file name
 * @return True if read successfully
 */
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
/**
 * Read the magnetic field from an ANSYS map text stream
 * @param input Input stream
 * @return True if read successfully
 */
const bool QwMagneticField::ReadFieldMap(std::istream& input)
{
  QwDebug << "###### Calling QwMagneticField::ReadFieldMap " << QwLog::endl;

  QwMessage << "------------------------------" << QwLog::endl;
  QwMessage << " Magnetic field interpolation " << QwLog::endl;
  QwMessage << "------------------------------" << QwLog::endl;

  // Declare variables
  double r, z, phi, phi_rad;
  field_t bx, by, bz, br, bphi, bx_new, by_new;

  // Check for stream
  if (!input.good()) {
    QwError << "Error: Could not open field map stream!" << QwLog::endl;
    return false;
  }
  field_t field[5];
  while (input.good()) {

    // Read a line with three position coordinates and three field components
    input >> r >> z >> phi >> bx >> by >> bz;

    // Correct for translation along z
    z -= fTranslation;

    // Correct for rotation around z
    phi -= fRotationDeg; if (phi < 0) phi += 360; if (phi > 360) phi -= 360;
    bx_new =  bx * fRotationCos + by * fRotationSin;
    by_new = -bx * fRotationSin + by * fRotationCos;
    bx = bx_new; by = by_new;

    // Calculate the radial field components
    phi_rad = phi * Qw::deg2rad;
    br =    bx * cos(phi_rad) + by * sin(phi_rad);
    bphi = -bx * sin(phi_rad) + by * cos(phi_rad);

    // Construct the coordinate
    double coord[3] = {z, r, phi};

    // Construct the field
    field[0] = bx * fUnitBfield * fFieldScalingFactor;
    field[1] = by * fUnitBfield * fFieldScalingFactor;
    field[2] = bz * fUnitBfield * fFieldScalingFactor;
    field[3] = br * fUnitBfield * fFieldScalingFactor;
    field[4] = bphi * fUnitBfield * fFieldScalingFactor;

    bool status = fField->Set(coord, field);
    if (! status) {
      QwError << "Problem assigning field to coordinate!" << QwLog::endl;
      QwError << coord[0] << "," << coord[1] << "," << coord[2] << QwLog::endl;
    }

    // Progress bar
    if (fField->GetCurrentEntries() % (fField->GetMaximumEntries() / 10) == 0)
      QwMessage << 100 * fField->GetCurrentEntries() / fField->GetMaximumEntries() << "%" << std::flush;
    if (fField->GetCurrentEntries() % (fField->GetMaximumEntries() / 10) != 0
     && fField->GetCurrentEntries() % (fField->GetMaximumEntries() / 40) == 0)
      QwMessage << "." << std::flush;
  };
  QwMessage << QwLog::endl;

  QwMessage << "Read from stream " << fField->GetCurrentEntries() << " entries" << QwLog::endl;
  QwMessage << "The grid size is " << fField->GetMaximumEntries() << QwLog::endl;

  QwDebug << "###### Leaving QwMagneticField::ReadFieldMap " << QwLog::endl;

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/**
 * Destructor: delete the field interpolator
 */
QwMagneticField::~QwMagneticField()
{
  delete fField;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/**
 * Get a field value
 * @param point[] Point coordinates (x,y,z)
 * @param field[] Field components (x,y,z,r,phi) (return)
 */
void QwMagneticField::GetFieldValue(const double point[3], double field[5]) const
{
  double r = sqrt(point[0] * point[0] + point[1] * point[1]);
  double phi = atan2(point[1], point[0]) * Qw::rad2deg;
  if (phi < 0) phi += 360; if (phi > 360) phi -= 360;
  double z = point[2];

  double coord[3] = {z, r, phi}; // ordering important!

  if (! fField->GetValue(coord,field))
    QwError << "Could not get field value!" << QwLog::endl;
}
