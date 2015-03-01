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
inline std::ostream& operator<< (std::ostream& stream, const double v[3])
{
  return stream << "(" << v[0] << "," << v[1] << "," << v[2] << ")";
}

/**
 * Default constructor with optional field map
 */
QwMagneticField::QwMagneticField(QwOptions& options, const bool suppress_read_field_map)
{
  // Check number of field components
  if (value_n < 3) {
    QwError << "Number of field components should be at least three!" << QwLog::endl;
  }

  // No field map yet
  fField = 0;

  // Initialize parameters
  SetReferenceCurrent(8960.0);
  SetActualCurrent(8921.0);
  SetTranslation(0.0);
  SetRotation(0.0);

  // Process options
  ProcessOptions(options);

  // Read field map
  if (! suppress_read_field_map) ReadFieldMap();
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
  // Options
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.mapfile",po::value<std::string>()->default_value("peiqing_2011.dat"),
     "Field map file");

  options.AddOptions("Magnetic field map")
    ("QwMagneticField.current",po::value<double>()->default_value(0.0),
     "Actual current of run to analyze");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.reference",po::value<double>()->default_value(8960.0),
     "Reference current of field map");

  options.AddOptions("Magnetic field map")
    ("QwMagneticField.trans",po::value<double>()->default_value(0),
     "Translation [cm]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.rot",po::value<double>()->default_value(0),
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
    ("QwMagneticField.rmax",po::value<double>()->default_value(260),
     "Maximum of r [cm]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.rstep",po::value<double>()->default_value(2),
     "Step size of r [cm]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.phimin",po::value<double>()->default_value(0),
     "Minimum of phi [deg]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.phimax",po::value<double>()->default_value(360),
     "Maximum of phi [deg]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.phistep",po::value<double>()->default_value(1),
     "Step size of phi [deg]");
  options.AddOptions("Magnetic field map")
    ("QwMagneticField.phiwrap",po::value<int>()->default_value(1),
     "Wrap-around of phi (number of equivalent grid points)");
}

/**
 * Process the options for this subsystem
 * @param options Options object
 */
void QwMagneticField::ProcessOptions(QwOptions& options)
{
  // Field currents
  SetActualCurrent(options.GetValue<double>("QwMagneticField.current"));
  SetReferenceCurrent(options.GetValue<double>("QwMagneticField.reference"));
  // Override if necessary
  LoadBeamProperty("beam_property.map");

  // Translation and rotation
  double trans = Qw::cm  * options.GetValue<double>("QwMagneticField.trans");
  double rot   = Qw::deg * options.GetValue<double>("QwMagneticField.rot");
  SetTranslation(trans);
  SetRotation(rot);

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
  int phiwrap = options.GetValue<int>("QwMagneticField.phiwrap");

  // The order is z, r, phi (no wrapping in r or z)
  fMin.push_back(zmin);   fMax.push_back(zmax);   fStep.push_back(zstep);   fWrap.push_back(0);
  fMin.push_back(rmin);   fMax.push_back(rmax);   fStep.push_back(rstep);   fWrap.push_back(0);
  fMin.push_back(phimin); fMax.push_back(phimax); fStep.push_back(phistep); fWrap.push_back(phiwrap);

  // Determine magnetic field file from environment variables
  std::string fieldmap = 
    options.GetValue<std::string>("QwMagneticField.mapfile");
  SetFilename(fieldmap);
}

/**
 * Read the magnetic field map
 * @return True if read successfully
 */
bool QwMagneticField::ReadFieldMap()
{
  // Delete existing magnetic field
  if (fField) delete fField;

  // Create new magnetic field
  fField = new QwInterpolator<field_t,value_n>(fMin,fMax,fStep);
  fField->SetWrapCoordinate(fWrap);

  // Add path to filename
  std::string filename = getenv_safe_string("QW_FIELDMAP") + "/" + fFilename;

  // Depending on form of filename, read zipped/regular/binary field map
  bool status = false;
  if (filename.find(".dat") != std::string::npos) {
    status = ReadFieldMapFile(filename);
  }
  if (filename.find(".dat.gz") != std::string::npos) {
    status = ReadFieldMapZip(filename);
  }
  if (filename.find(".bin") != std::string::npos) {
    status = fField->ReadBinaryFile(filename);
    // Try the regular map as a fallback option
    if (status == false) {
      filename.replace(filename.find("bin"),3,"dat");
      status = ReadFieldMapFile(filename);
    }
  }

  // Check whether we succeeded
  if (status == false) {
    QwError   << "Could not load magnetic field map!" << QwLog::endl;
    QwWarning << "Filename: " << filename << QwLog::endl;
  }

  return status;
}

/**
 * Test the field map value at a specific point to make sure nothing went wrong
 * @return True if read successfully
 */
bool QwMagneticField::TestFieldMap()
{
  bool status = true;

  // Test field value at exact grid point
  // (r = 100*cm, z = 100*cm, phi = 22.5*degree)
  double point[3] = {100.0 * Qw::cm, 0.0 * Qw::cm, 100.0 * Qw::cm};
  double exact[3] = {-0.0499845 * Qw::kG, 3.28516 * Qw::kG, -0.0112704 * Qw::kG};

  // Calculate field value
  QwMessage << "Calculating test field value at cartesian position "
    << "(" << point[0]/Qw::cm << "," << point[1]/Qw::cm << "," << point[2]/Qw::cm << ") cm "
    << QwLog::endl;
  double value[3] = {0.0, 0.0, 0.0};
  GetFieldValue(point,value);

  // Calculate difference
  double diff[3] = {0.0, 0.0, 0.0};
  double norm = 0.0;
  for (size_t i = 0; i < 3; i++) {
    diff[i] = value[i] - exact[i];
    norm += diff[i] * diff[i];
  }
  norm = sqrt(norm);

  // Output
  QwMessage << "  Value = "
    << "(" << value[0]/Qw::kG << ", " << value[1]/Qw::kG << ", " << value[2]/Qw::kG << ") kG."
    << QwLog::endl;
  QwMessage << "  Exact = "
    << "(" << exact[0]/Qw::kG << ", " << exact[1]/Qw::kG << ", " << exact[2]/Qw::kG << ") kG."
    << QwLog::endl;
  QwMessage << "  Diff  = "
    << "(" << diff[0]/Qw::kG << ", " << diff[1]/Qw::kG << ", " << diff[2]/Qw::kG << ") kG."
    << QwLog::endl;

  // Test difference (0.1 kG is of the order of 2%)
  if (norm > 0.1 * Qw::kG) {
    QwWarning << "Magnetic field is different from expected value by " << norm/Qw::kG << " kG."
              << QwLog::endl;
    status = false;
  }

  // Check validity
  if (status == false) {
    QwError   << "Fieldmap did not satisfy consistency checks!" << QwLog::endl;
  }

  // Nothing is tested, just return true
  return status;
}


/**
 * Read the magnetic field from an ANSYS map file in text format
 * @param filename ANSYS map file name
 * @return True if read successfully
 */
bool QwMagneticField::ReadFieldMapFile(const std::string& filename)
{
  // Open the field map file
  std::ifstream input;
  input.open(filename.c_str(), std::ios_base::in);
  // Check for success
  if (!input.good()) {
    QwError << "Could not open field map file!" << QwLog::endl;
    QwError << "File name: " << filename << QwLog::endl;
    return false;
  }

  // Read the input field map stream
  return ReadFieldMapStream(input);
}

/**
 * Read the magnetic field from an ANSYS map file in gzipped text format
 * @param filename ANSYS map file name
 * @return True if read successfully
 */
bool QwMagneticField::ReadFieldMapZip(const std::string& filename)
{
#ifdef __USE_BOOST_IOSTREAMS
  // Create a gzip filter for the field map file
  boost::iostreams::filtering_istream input;
  input.push(boost::iostreams::gzip_decompressor());
  input.push(boost::iostreams::file_source(filename));

  // Read the input field map stream
  return ReadFieldMapStream(input);
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
bool QwMagneticField::ReadFieldMapStream(std::istream& input)
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

  // Apply scale factor
  for (unsigned int i = 0; i < value_n; i++)
    field[i] *= fScaleFactor;

  // Warn if the coordinate was inside the field boundaries,
  // but we still encountered a problem.
  if (status == false && fField->InBounds(coord_zrf)) {
    QwWarning << "Could not get field value at (z,r,phi) = " << coord_zrf << QwLog::endl;
    QwWarning << "Will use field value (x,y,z) = " << field << QwLog::endl;
  }
}


void QwMagneticField::LoadBeamProperty(const TString & map) {
  QwParameterFile mapstr(map.Data());
  while (mapstr.ReadNextLine()) {
    mapstr.TrimComment();       // Remove everything after a comment character.
    mapstr.TrimWhitespace();    // Get rid of leading and trailing spaces.
    if (mapstr.LineIsEmpty())
      continue;

    string varname, varvalue;
    if (mapstr.HasVariablePair("=", varname, varvalue)) {
      if (varname == "QTOR") {
        SetActualCurrent(atof(varvalue.c_str()));
      }
    }
  }
}
