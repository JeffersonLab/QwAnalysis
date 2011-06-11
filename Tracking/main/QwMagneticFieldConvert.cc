// Standard C and C++ headers
#include <iostream>
#include <string>
#include <sys/time.h>

// ROOT headers
#include <TRandom3.h>

// Qweak headers
#include "QwLog.h"
#include "QwOptions.h"
#include "QwParameterFile.h"
#include "QwMagneticField.h"


#define NSAMPLES 1000000

int main (int argc, char* argv[])
{
  // Search path for parameter files
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Tracking/prminput");

  // Option definitions of standard objects
  QwMagneticField::DefineOptions(gQwOptions);

  // Custom options of this executable
  gQwOptions.AddOptions("Magnetic field map actions")
    ("convert",po::value<bool>()->default_bool_value(false),
     "Convert field map from ASCII to binary");
  gQwOptions.AddOptions("Magnetic field map actions")
    ("write-scaled",po::value<bool>()->default_bool_value(false),
     "Write scaled field instead of original");
  gQwOptions.AddOptions("Magnetic field map actions")
    ("timing",po::value<bool>()->default_bool_value(false),
     "Time magnetic field reading routines");
  gQwOptions.AddOptions("Magnetic field map actions")
    ("name",po::value<string>()->default_value("peiqing_2007"),
     "Field map basename in QW_FIELDMAP (no extension)");

  // Set command line
  gQwOptions.SetCommandLine(argc, argv);

  // Get options
  bool timing = gQwOptions.GetValue<bool>("timing");
  bool convert = gQwOptions.GetValue<bool>("convert");

  // Name of field map file
  string basename = gQwOptions.GetValue<string>("name");
  std::string name = getenv_safe_string("QW_FIELDMAP") + "/" + basename;


  timeval time_start, time_finish;

  // Initialize the field map
  gettimeofday(&time_start, 0);
  QwMagneticField* magneticfield = new QwMagneticField();
  gettimeofday(&time_finish, 0);
  if (timing) {
    int time_initialize_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
    int time_initialize_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
    if (time_initialize_usec < 0) { time_initialize_usec += 1000000.0; time_initialize_sec--; }
    QwMessage << "Initialization: "
              << time_initialize_sec << " sec and "
              << time_initialize_usec << " usec" << QwLog::endl;
  }

  // Set data reduction factor
  //magneticfield->SetDataReductionFactor(2);
  //QwMessage << "Data reduction factor set to "
  //          << magneticfield->GetDataReductionFactor() << QwLog::endl;


  // Initialize common variables for all tests
  double mean = 30.0;
  double sigma = 3.0;
  double point[3];
  double field[3];
  TRandom3 random;
  int time_sampling_sec;
  int time_sampling_usec;
  double time_sampling;


  // Read the text field map
  gettimeofday(&time_start, 0);
  magneticfield->ReadFieldMapFile(name + ".dat");
  gettimeofday(&time_finish, 0);
  if (timing) {
    int time_reading_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
    int time_reading_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
    if (time_reading_usec < 0) { time_reading_usec += 1000000.0; time_reading_sec--; }
    QwMessage << "Reading field map (text): "
              << time_reading_sec << " sec and "
              << time_reading_usec << " usec" << QwLog::endl;
  }
  point[0] = 38.2683 * Qw::cm; point[1] = 92.388 * Qw::cm; point[2] = 100.0 * Qw::cm;
  magneticfield->GetCartesianFieldValue(point, field);
  QwMessage << "Field at " << point[0]/Qw::cm << "," << point[1]/Qw::cm << "," << point[2]/Qw::cm << " cm "
            << "is " << field[0]/Qw::kG << "," << field[1]/Qw::kG << "," << field[2]/Qw::kG << " kG"
            << QwLog::endl;

  // Conversion from text field map to binary field map
  if (convert) {

    // Write the binary field map
    gettimeofday(&time_start, 0);
    magneticfield->WriteBinaryFile(name + ".bin");
    gettimeofday(&time_finish, 0);
    if (timing) {
      int time_writing_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
      int time_writing_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
      if (time_writing_usec < 0) { time_writing_usec += 1000000.0; time_writing_sec--; }
      QwMessage << "Writing field map (binary): "
                << time_writing_sec << " sec and "
                << time_writing_usec << " usec" << QwLog::endl;
    }

    // Delete old field map
    gettimeofday(&time_start, 0);
    delete magneticfield;
    gettimeofday(&time_finish, 0);
    if (timing) {
      int time_destruction_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
      int time_destruction_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
      if (time_destruction_usec < 0) { time_destruction_usec += 1000000.0; time_destruction_sec--; }
      QwMessage << "Destruction: "
                << time_destruction_sec << " sec and "
                << time_destruction_usec << " usec" << QwLog::endl;
    }

    // Create new field map
    gettimeofday(&time_start, 0);
    magneticfield = new QwMagneticField();
    gettimeofday(&time_finish, 0);
    if (timing) {
      int time_initialize2_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
      int time_initialize2_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
      if (time_initialize2_usec < 0) { time_initialize2_usec += 1000000.0; time_initialize2_sec--; }
      QwMessage << "Initialization: "
                << time_initialize2_sec << " sec and "
                << time_initialize2_usec << " usec" << QwLog::endl;
    }

  } // end of conversion

  // Read the binary field map
  gettimeofday(&time_start, 0);
  magneticfield->ReadBinaryFile(name + ".bin");
  gettimeofday(&time_finish, 0);
  if (timing) {
    int time_reading2_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
    int time_reading2_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
    if (time_reading2_usec < 0) { time_reading2_usec += 1000000.0; time_reading2_sec--; }
    QwMessage << "Reading field map (binary): "
              << time_reading2_sec << " sec and "
              << time_reading2_usec << " usec" << QwLog::endl;
  }

  // Multilinear interpolation
  magneticfield->SetInterpolationMethod(kMultiLinear);
  QwMessage << "Interpolation method set to multilinear, method "
            << magneticfield->GetInterpolationMethod() << QwLog::endl;

  point[0] = 38.2683 * Qw::cm; point[1] = 92.388 * Qw::cm; point[2] = 100.0 * Qw::cm;
  magneticfield->GetCartesianFieldValue(point, field);
  QwMessage << "Field at " << point[0]/Qw::cm << "," << point[1]/Qw::cm << "," << point[2]/Qw::cm << " cm "
            << "is " << field[0]/Qw::kG << "," << field[1]/Qw::kG << "," << field[2]/Qw::kG << " kG"
            << QwLog::endl;

  if (timing) {
    gettimeofday(&time_start, 0);
    for (int i = 0; i < NSAMPLES; i++) {
      point[0] = random.Gaus(mean, sigma);
      point[1] = random.Gaus(mean, sigma);
      point[2] = random.Gaus(mean, sigma);
      magneticfield->GetCartesianFieldValue(point, field);
    }
    gettimeofday(&time_finish, 0);
    time_sampling_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
    time_sampling_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
    if (time_sampling_usec < 0) { time_sampling_usec += 1000000.0; time_sampling_sec--; }
    time_sampling = 1000000.0 * time_sampling_sec + time_sampling_usec;
    QwMessage << "Sampling (total of " << NSAMPLES << " samples): "
              << time_sampling_sec << " sec and "
              << time_sampling_usec << " usec" << QwLog::endl;
    QwMessage << "Sampling (per sample): " << time_sampling / NSAMPLES << " usec" << QwLog::endl;
  }

  // Nearest-neighbor interpolation
  magneticfield->SetInterpolationMethod(kNearestNeighbor);
  QwMessage << "Interpolation method set to nearest-neighbor, method "
            << magneticfield->GetInterpolationMethod() << QwLog::endl;

  point[0] = 38.2683 * Qw::cm; point[1] = 92.388 * Qw::cm; point[2] = 100.0 * Qw::cm;
  magneticfield->GetCartesianFieldValue(point, field);
  QwMessage << "Field at " << point[0]/Qw::cm << "," << point[1]/Qw::cm << "," << point[2]/Qw::cm << " cm "
            << "is " << field[0]/Qw::kG << "," << field[1]/Qw::kG << "," << field[2]/Qw::kG << " kG"
            << QwLog::endl;

  if (timing) {
    gettimeofday(&time_start, 0);
    for (int i = 0; i < NSAMPLES; i++) {
      point[0] = random.Gaus(mean, sigma);
      point[1] = random.Gaus(mean, sigma);
      point[2] = random.Gaus(mean, sigma);
      magneticfield->GetCartesianFieldValue(point, field);
    }
    gettimeofday(&time_finish, 0);
    time_sampling_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
    time_sampling_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
    if (time_sampling_usec < 0) { time_sampling_usec += 1000000.0; time_sampling_sec--; }
    time_sampling = 1000000.0 * time_sampling_sec + time_sampling_usec;
    QwMessage << "Sampling (total of " << NSAMPLES << " samples): "
              << time_sampling_sec << " sec and "
              << time_sampling_usec << " usec" << QwLog::endl;
    QwMessage << "Sampling (per sample): " << time_sampling / NSAMPLES << " usec" << QwLog::endl;
  }

  // Delete the field map
  delete magneticfield;

  return 0;
}
