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
 * Method to print arrays conveniently
 * @param stream Output stream
 * @param v Array of 3 doubles
 * @return Output stream
 */
inline ostream& operator<< (ostream& stream, const double v[3])
{
  return stream << "(" << v[0] << "," << v[1] << "," << v[2] << ")";
}

/**
 * Default constructor with optional field map
 */
QwMagneticField::QwMagneticField()
{
  // Check number of field components
  if (value_n < 3) {
    QwError << "Number of field components should be at least three!" << QwLog::endl;
  }

  // No field map yet
  fField = 0;

  // Initialize parameters
  SetScaleFactor(1.0);
  SetTranslation(0.0);
  SetRotation(0.0);

  // Process options
  ProcessOptions(gQwOptions);
}

/**
 * Destructor
 */
QwMagneticField::~QwMagneticField()
{
  if (fField) delete fField;
}

/**
 * Define the options for this subsystem
 * @param options Options object
 */
void QwMagneticField::DefineOptions(QwOptions& options)
{
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.bfil",po::value<double>()->default_value(1),
     "Rescaling factor");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.trans",po::value<double>()->default_value(0),
     "Translation [cm]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.rot",po::value<double>()->default_value(-90.0 + 22.5),
     "Rotation [deg]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.zmin",po::value<double>()->default_value(-250),
     "Minimum of z [cm]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.zmax",po::value<double>()->default_value(+250),
     "Maximum of z [cm]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.zstep",po::value<double>()->default_value(2),
     "Step size of z [cm]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.rmin",po::value<double>()->default_value(2),
     "Minimum of r [cm]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.rmax",po::value<double>()->default_value(300),
     "Maximum of r [cm]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.rstep",po::value<double>()->default_value(2),
     "Step size of r [cm]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.phimin",po::value<double>()->default_value(-0.5),
     "Minimum of phi [deg]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.phimax",po::value<double>()->default_value(360.5),
     "Maximum of phi [deg]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.phistep",po::value<double>()->default_value(1.0),
     "Step size of phi [deg]");
}

/**
 * Process the options for this subsystem
 * @param options Options object
 */
void QwMagneticField::ProcessOptions(QwOptions& options)
{
  // Scaling, translation, rotation options
  double bfil  = options.GetValue<double>("QwMagneticField.bfil");
  double trans = Qw::cm  * options.GetValue<double>("QwMagneticField.trans");
  double rot   = Qw::deg * options.GetValue<double>("QwMagneticField.rot");

  // Grid options
  double zmin    = Qw::cm  * options.GetValue<double>("QwMagneticField.zmin");
  double zmax    = Qw::cm  * options.GetValue<double>("QwMagneticField.zmax");
  double zstep   = Qw::cm  * options.GetValue<double>("QwMagneticField.zstep");
  double rmin    = Qw::cm  * options.GetValue<double>("QwMagneticField.rmin");
  double rmax    = Qw::cm  * options.GetValue<double>("QwMagneticField.rmax");
  double rstep   = Qw::cm  * options.GetValue<double>("QwMagneticField.rstep");
  double phimin  = Qw::deg * options.GetValue<double>("QwMagneticField.phimin");
  double phimax  = Qw::deg * options.GetValue<double>("QwMagneticField.phimax");
  double phistep = Qw::deg * options.GetValue<double>("QwMagneticField.phistep");

  // Fill vector of grid min, max, and step size
  std::vector<double> min, max, step;
  std::vector<size_t> wrap;
  min.push_back(zmin);   max.push_back(zmax);   step.push_back(zstep);   wrap.push_back(0);
  min.push_back(rmin);   max.push_back(rmax);   step.push_back(rstep);   wrap.push_back(0);
  min.push_back(phimin); max.push_back(phimax); step.push_back(phistep); wrap.push_back(2);

  // Set options
  SetScaleFactor(bfil);
  SetTranslation(trans);
  SetRotation(rot);

  // Create new magnetic field
  fField = new QwInterpolator<field_t,value_n>(min,max,step);
  fField->SetWrapCoordinate(wrap);
}

/**
 * Read the magnetic field from an ANSYS map file in text format
 * @param filename ANSYS map file name
 * @return True if read successfully
 */
bool QwMagneticField::ReadFieldMapFile(const std::string filename)
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

/**
 * Read the magnetic field from an ANSYS map file in gzipped text format
 * @param filename ANSYS map file name
 * @return True if read successfully
 */
bool QwMagneticField::ReadFieldMapZip(const std::string filename)
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

/**
 * Read the magnetic field from an ANSYS map text stream
 * @param input Input stream
 * @return True if read successfully
 */
bool QwMagneticField::ReadFieldMap(std::istream& input)
{
  // Check whether field map exists
  if (fField == 0) {
    QwWarning << "Trying to read field map without object to put values in."
              << QwLog::endl;
    return false;
  }

  // Declare variables
  double r, z, phi;
  field_t bx, by, bz, bx_new, by_new;

  // Check for stream
  if (!input.good()) {
    QwError << "Error: Could not open field map stream!" << QwLog::endl;
    return false;
  }

  QwMessage << "Reading magnetic field map ";

  // Loop over stream until end-of-file
  // Note: input.good() only says whether next operation *might* succeed
  while (! input.fail()) {

    // Progress bar
    if (fField->GetCurrentEntries() % (fField->GetMaximumEntries() / 10) == 0) {
      int pct = fField->GetCurrentEntries() / (fField->GetMaximumEntries() / 100);
      QwMessage << pct << "%" << QwLog::flush;
    }
    if (fField->GetCurrentEntries() % (fField->GetMaximumEntries() / 10) != 0
     && fField->GetCurrentEntries() % (fField->GetMaximumEntries() / 40) == 0) {
      QwMessage << "." << QwLog::flush;
    }

    // Read a line with three position coordinates and three field components
    input >> r >> z >> phi >> bx >> by >> bz;
    if (! input.good()) continue;

    // Fix the units
    r *= Qw::cm; z *= Qw::cm; phi *= Qw::deg; 
    bx *= Qw::kG; by *= Qw::kG; bz *= Qw::kG;

    // Correct for translation along z
    z -= fTranslation;

    // Correct for rotation around z
    phi -= fRotation;
    if (phi < 0.0*Qw::pi) phi += 2.0*Qw::pi;
    if (phi > 2.0*Qw::pi) phi -= 2.0*Qw::pi;
    bx_new =  bx * fRotationCos + by * fRotationSin;
    by_new = -bx * fRotationSin + by * fRotationCos;
    bx = bx_new; by = by_new;

    // Construct the coordinate
    double coord[3] = {z, r, phi};

    // Construct the field vector
    field_t field[value_n];
    field[0] = bx;
    field[1] = by;
    field[2] = bz;

    if (value_n == 5) {
      // Calculate the radial and azimuthal field components
      double br =    bx * cos(phi) + by * sin(phi);
      double bphi = -bx * sin(phi) + by * cos(phi);

      // Construct the field vector
      field[3] = br;
      field[4] = bphi;
    }

    bool status = fField->Set(coord, field);
    if (! status) {
      QwError << "Problem assigning field to coordinate!" << QwLog::endl;
      QwError << coord[0] << "," << coord[1] << "," << coord[2] << QwLog::endl;
    }
  }
  QwMessage << QwLog::endl;

  if (abs(fField->GetCurrentEntries() / fField->GetMaximumEntries() - 1) > 0.00001) {
    QwWarning << "Expected " << fField->GetMaximumEntries() << " entries, "
              << "but only read " << fField->GetCurrentEntries() << "." << QwLog::endl;
    QwMessage << "Coverage of the z bins:" << QwLog::endl;
    fField->PrintCoverage(0);
    QwMessage << "Coverage of the r bins:" << QwLog::endl;
    fField->PrintCoverage(1);
    QwMessage << "Coverage of the phi bins:" << QwLog::endl;
    fField->PrintCoverage(2);
  }

  return true;
}

/**
 * Get a field value
 * @param coord_xyz[] Cartesian coordinates (x,y,z)
 * @param field[] Field components (x,y,z,r,phi) (return)
 */
void QwMagneticField::GetFieldValue(
	const double coord_xyz[3],
	double field[value_n]) const
{
  // Convert from cartesian to cylindrical coordinates
  double z   = coord_xyz[2];
  double r   = sqrt(coord_xyz[0] * coord_xyz[0] + coord_xyz[1] * coord_xyz[1]);
  double phi = atan2(coord_xyz[1], coord_xyz[0]); if (phi < 0.0) phi += 2.0*Qw::pi;

  // The ordering is important!  It has to agree with how the ordering is
  // defined on initialization of the field map.  Since tracks are in fairly
  // constant phi planes, it makes sense to keep phi as the most significant
  // index.  The largest changes happen in z, so that should be the least
  // significant index.  This will allow us to use caching more efficiently.
  double coord_zrf[3] = {z, r, phi};

  // The magnetic field object was not defined, return zero field and complain
  if (!this) {
    QwWarning << "No field map defined: assuming zero field!" << QwLog::endl;
    for (unsigned int i = 0; i < value_n; i++) field[i] = 0.0;
    return;
  }

  // Retrieve field value
  bool status = fField->GetValue(coord_zrf,field);

  // Warn if the coordinate was inside the field boundaries,
  // but we still encountered a problem.
  if (status == false && fField->InBounds(coord_zrf)) {
    QwWarning << "Could not get field value at (z,r,phi) = " << coord_zrf << QwLog::endl;
    QwWarning << "Will use field value (x,y,z) = " << field << QwLog::endl;
  }
}
