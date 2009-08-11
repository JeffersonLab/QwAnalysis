/*------------------------------------------------------------------------*//*!

 \file QwMockData.cc

 \brief Parity mock data generator, test code

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <iostream>

// Boost math library for random number generation
#include <boost/random.hpp>

// Coda headers
#include "THaCodaFile.h"

// Qweak headers
#include "QwVQWK_Channel.h"
#include "QwEventBuffer.h"
#include "QwSubsystemArrayParity.h"
#include "QwBeamLine.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"


// Number of events to generate
#define NEVENTS 1000
// Number of variables to correlate
#define NVARS 4


// Debug level
static int kDebug = 1;

int main(int argc, char* argv[])
{
  // Detector array
  QwSubsystemArrayParity detectors;
  detectors.push_back(new QwBeamLine("Injector BeamLine"));
  detectors.GetSubsystem("Injector BeamLine")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_beamline.map");
  detectors.GetSubsystem("Injector BeamLine")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_pedestal.map");
  QwBeamLine* beamline = (QwBeamLine*) detectors.GetSubsystem("Injector BeamLine");

  // Get the channels we want to correlate
  QwBCM* bcm = beamline->GetBCM("qwk_bcm0l02");

  // Possible scenarios:
  // - everything is random, no correlations at all
  // - one parameter has a helicity-correlated asymmetry
  // - two parameters have independent helicity-correlated asymmetries
  // - two parameters have correlated helicity-correlated asymmetries


  // Event buffer
  QwEventBuffer eventBuffer;
  // Output file
  TString filename("mockdata.dat");
  if (eventBuffer.OpenDataFile(filename,"W") != CODA_OK) {
    std::cout << "Error: could not open file!" << std::endl;
    return 0;
  }
  eventBuffer.ResetControlParameters();


  // Initialize randomness provider and distribution
  boost::mt19937 randomnessGenerator; // Mersenne twister
  boost::normal_distribution<double> normalDistribution;
  boost::variate_generator
    < boost::mt19937, boost::normal_distribution<double> >
      normal(randomnessGenerator, normalDistribution);


  // Data block
  Double_t block[4] = {0.0, 0.0, 0.0, 0.0};
  UInt_t sequencenumber = 0;


  // Event generation loop
  for (Int_t event = 1; event <= NEVENTS; event++) {
    if (event % 1000 == 0 || NEVENTS < 10)
      std::cout << "Generated " << event << " events." << std::endl;


    // N-dimensional correlated normal random variables:
    //   X = C' * Z
    // with
    //   X correlated and normally distributed,
    //   Z independent and normally distributed,
    //   C the Cholesky decomposition of the positive-definite covariance matrix
    //     (C should probably be calculated offline)
    //
    double z[NVARS], x[NVARS];
    double C[NVARS][NVARS];
    for (int var = 0; var < NVARS; var++) {
      x[var] = 0.0;
      z[var] = normal();
      C[var][var] = 1.0;
    }
    for (int i = 0; i < NVARS; i++)
      for (int j = 0; j < NVARS; j++)
        x[i] += C[j][i] * z[j];
    // Unused for now


    // Clear event
    bcm->ClearEventData();

    // Set event to random value
    block[0] = 1000.4999;
    block[1] = 2001.5001;
    block[2] = 3002.0001;
    block[3] = 4003.9999;
    bcm->SetEventData(block, event);
    if (kDebug) bcm->Print();

    // Fill in appropriate buffer
    std::vector<UInt_t> buffer;
    bcm->EncodeEventData(buffer);
    // Print this buffer
    if (kDebug) {
      std::cout << std::endl << "Buffer: ";
      for (size_t i = 0; i < buffer.size(); i++)
        std::cout << std::hex << buffer.at(i) << " ";
      std::cout << std::dec << std::endl;
    }




    // Write this event to file
    eventBuffer.EncodeSubsystemData(detectors);

    // Invert this whole process as a cross check
//     vqwk_test.ProcessEvBuffer(buffer, 6);
//     vqwk_test.ProcessEvent();
//     if (kDebug) vqwk_test.Print();

    // Compare
//     for (int i = 0; i < 4; i++)
//       std::cout << i << ": " << block[i] << " -> "
//                 << vqwk.GetBlockValue(i) << " -> "
//                 << vqwk_test.GetBlockValue(i) << std::endl;


    // Process into CODA format


    // Append to file


  } // end of event loop


  eventBuffer.CloseDataFile();
  eventBuffer.ReportRunSummary();


}
