// Standard C and C++ headers
#include <iostream>
#include <string>
#include <sys/time.h>

// ROOT headers
#include <TRandom3.h>

// Qweak headers
#include "QwLog.h"
#include "QwMagneticField.h"
#include "QwOptions.h"


#define NSAMPLES 1000000

// Debug level
static const bool kDebug = true;
static const bool kConvert = true;

int main (int argc, char* argv[])
{
  timeval time_start, time_finish;

  // Initialize the field map
  std::string name = getenv_safe_string("QW_FIELDMAP") + "/peiqing_2007";
  gettimeofday(&time_start, 0);
  QwMagneticField* magneticfield = new QwMagneticField();
  gettimeofday(&time_finish, 0);
  int time_initialize_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
  int time_initialize_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
  if (time_initialize_usec < 0) { time_initialize_usec += 1000000.0; time_initialize_sec--; }
  std::cout << "Initialization: "
            << time_initialize_sec << " sec and "
            << time_initialize_usec << " usec" << std::endl;

  // Set data reduction factor
  //magneticfield->SetDataReductionFactor(4);
  //std::cout << "Data reduction factor set to "
  //          << magneticfield->GetDataReductionFactor() << std::endl;


  if (kConvert) {

    // Read the text field map
    gettimeofday(&time_start, 0);
    magneticfield->ReadFieldMapFile(name + ".dat");
    gettimeofday(&time_finish, 0);
    int time_reading_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
    int time_reading_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
    if (time_reading_usec < 0) { time_reading_usec += 1000000.0; time_reading_sec--; }
    std::cout << "Reading field map (text): "
              << time_reading_sec << " sec and "
              << time_reading_usec << " usec" << std::endl;

    // Write the binary field map
    gettimeofday(&time_start, 0);
    magneticfield->WriteBinaryFile(name + ".bin");
    gettimeofday(&time_finish, 0);
    int time_writing_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
    int time_writing_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
    if (time_writing_usec < 0) { time_writing_usec += 1000000.0; time_writing_sec--; }
    std::cout << "Writing field map (binary): "
              << time_writing_sec << " sec and "
              << time_writing_usec << " usec" << std::endl;


    // Delete old field map
    gettimeofday(&time_start, 0);
    delete magneticfield;
    gettimeofday(&time_finish, 0);
    int time_destruction_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
    int time_destruction_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
    if (time_destruction_usec < 0) { time_destruction_usec += 1000000.0; time_destruction_sec--; }
    std::cout << "Destruction: "
              << time_destruction_sec << " sec and "
              << time_destruction_usec << " usec" << std::endl;

    // Create new field map
    gettimeofday(&time_start, 0);
    magneticfield = new QwMagneticField();
    gettimeofday(&time_finish, 0);
    int time_initialize2_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
    int time_initialize2_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
    if (time_initialize2_usec < 0) { time_initialize2_usec += 1000000.0; time_initialize2_sec--; }
    std::cout << "Initialization: "
              << time_initialize2_sec << " sec and "
              << time_initialize2_usec << " usec" << std::endl;

  }

  // Read the binary field map
  gettimeofday(&time_start, 0);
  magneticfield->ReadBinaryFile(name + ".bin");
  gettimeofday(&time_finish, 0);
  int time_reading2_sec  = ((int) time_finish.tv_sec  - (int) time_start.tv_sec);
  int time_reading2_usec = ((int) time_finish.tv_usec - (int) time_start.tv_usec);
  if (time_reading2_usec < 0) { time_reading2_usec += 1000000.0; time_reading2_sec--; }
  std::cout << "Reading field map (binary): "
            << time_reading2_sec << " sec and "
            << time_reading2_usec << " usec" << std::endl;

  // Initialize common variables for all tests
  double mean = 30.0;
  double sigma = 3.0;
  double point[3];
  double field[3];
  TRandom3 random;
  int time_sampling_sec;
  int time_sampling_usec;
  double time_sampling;

  // Multilinear interpolation
  magneticfield->SetInterpolationMethod(kMultiLinear);
  std::cout << "Interpolation method set to multilinear, method "
            << magneticfield->GetInterpolationMethod() << std::endl;

  point[0] = 30.0; point[1] = 30.0; point[2] = 30.0;
  magneticfield->GetCartesianFieldValue(point, field);
  std::cout << "Field at " << point[0] << "," << point[1] << "," << point[2]
            << " is " << field[0] << "," << field[1] << "," << field[2] << " T"
            << std::endl;

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
  std::cout << "Sampling (total of " << NSAMPLES << " samples): "
            << time_sampling_sec << " sec and "
            << time_sampling_usec << " usec" << std::endl;
  std::cout << "Sampling (per sample): " << time_sampling / NSAMPLES << " usec" << std::endl;


  // Nearest-neighbor interpolation
  magneticfield->SetInterpolationMethod(kNearestNeighbor);
  std::cout << "Interpolation method set to nearest-neighbor, method "
            << magneticfield->GetInterpolationMethod() << std::endl;

  point[0] = 30.0; point[1] = 30.0; point[2] = 30.0;
  magneticfield->GetCartesianFieldValue(point, field);
  std::cout << "Field at " << point[0] << "," << point[1] << "," << point[2]
            << " is " << field[0] << "," << field[1] << "," << field[2] << " T"
            << std::endl;

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
  std::cout << "Sampling (total of " << NSAMPLES << " samples): "
            << time_sampling_sec << " sec and "
            << time_sampling_usec << " usec" << std::endl;
  std::cout << "Sampling (per sample): " << time_sampling / NSAMPLES << " usec" << std::endl;


  // Delete the field map
  delete magneticfield;

  return 0;
}
