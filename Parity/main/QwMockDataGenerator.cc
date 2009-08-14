/*------------------------------------------------------------------------*//*!

 \file QwMockData.cc

 \brief Parity mock data generator, test code

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <iostream>

// Boost math library for random number generation
#include <boost/random.hpp>

// CODA headers (for CODA status)
#include "THaCodaFile.h"

// Qweak headers
#include "QwBeamLine.h"
#include "QwCommandLine.h"
#include "QwEventBuffer.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwSubsystemArrayParity.h"
#include "QwVQWK_Channel.h"


// Number of variables to correlate
#define NVARS 8


// Debug
static bool kDebug = true;

int main(int argc, char* argv[])
{
  // Parse command line arguments
  QwCommandLine commandline;
  commandline.Parse(argc, argv);

  // Event buffer
  QwEventBuffer eventbuffer;

  // Detector array
  QwSubsystemArrayParity detectors;
  detectors.push_back(new QwBeamLine("Injector BeamLine"));
  detectors.GetSubsystem("Injector BeamLine")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_beamline.map");
  detectors.GetSubsystem("Injector BeamLine")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_pedestal.map");
  detectors.push_back(new QwHelicity("Helicity info"));
  detectors.GetSubsystem("Helicity info")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_helicity.map");
  detectors.GetSubsystem("Helicity info")->LoadInputParameters("");
  QwHelicityPattern QwHelPat(detectors,4);


  // Possible scenarios:
  // - everything is random, no correlations at all, no asymmetries at all
  // - one parameter has a helicity-correlated asymmetry, every other parameter
  //   is random and independently distributed
  // - two parameters have independent helicity-correlated asymmetries
  // - two parameters have correlated helicity-correlated asymmetries

  // Get the channels we want to correlate
  QwBeamLine* beamline = (QwBeamLine*) detectors.GetSubsystem("Injector BeamLine");

  QwBCM* bcm[8];
  bcm[0] = beamline->GetBCM("qwk_bcm0l00");
  bcm[1] = beamline->GetBCM("qwk_bcm0l01");
  bcm[2] = beamline->GetBCM("qwk_bcm0l02");
  bcm[3] = beamline->GetBCM("qwk_bcm0l03");
  bcm[4] = beamline->GetBCM("qwk_bcm0l04");
  bcm[5] = beamline->GetBCM("qwk_bcm0l05");
  bcm[6] = beamline->GetBCM("qwk_bcm0l06");
  bcm[7] = beamline->GetBCM("qwk_bcm0l07");
  Double_t bcm_mean = 1.0e7;
  Double_t bcm_sigma = 1.0e4;
  for (int i = 0; i < 8; i++)
    bcm[i]->SetRandomEventParameters(bcm_mean, bcm_sigma);

  QwBPMStripline* bpm[2];
  bpm[0] = beamline->GetBPMStripline("qwk_0r06");
  bpm[1] = beamline->GetBPMStripline("qwk_0l06");
  Double_t bpm_meanX = 5.0;
  Double_t bpm_sigmaX = 4.0e-6;
  Double_t bpm_meanY = -2.5;
  Double_t bpm_sigmaY = 2.0e-6;
  for (int i = 0; i < 2; i++)
    bpm[i]->SetRandomEventParameters(bpm_meanX, bpm_sigmaX, bpm_meanY, bpm_sigmaY);


  // Initialize randomness provider and distribution
  boost::mt19937 randomnessGenerator(999); // Mersenne twister with seed (see below)
  boost::normal_distribution<double> normalDistribution;
  boost::variate_generator
    < boost::mt19937, boost::normal_distribution<double> >
      normal(randomnessGenerator, normalDistribution);
  // WARNING: This variate_generator will return the SAME random values as the
  // variate_generator in QwVQWK_Channel when used with the same default seed!
  // Therefore you really should give an explicitly different seed for the
  // mt19937 randomness generator.



  // Run generation loop
  for (int run = commandline.GetFirstRun(); run <= commandline.GetLastRun(); run++) {

    // Open new output file
    // (giving run number as argument to OpenDataFile confuses the segment search)
    TString filename = TString("QwMock_") + Form("%ld.",run) + TString("log");
    if (eventbuffer.OpenDataFile(filename,"W") != CODA_OK) {
      std::cout << "Error: could not open file!" << std::endl;
      return 0;
    }
    eventbuffer.ResetControlParameters();
    eventbuffer.EncodePrestartEvent(run, 0); // prestart: runnumber, runtype
    eventbuffer.EncodeGoEvent();

    // Event generation loop
    for (Int_t event = commandline.GetFirstEvent();
               event <= commandline.GetLastEvent(); event++) {

      // First create the independent random data
      detectors.ClearEventData();
      detectors.RandomizeEventData();


      // Secondly introduce correlations between variables
      //
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


      // Write this event to file
      eventbuffer.EncodeSubsystemData(detectors);


      // Periodically print event number
      if (kDebug && event % 1000 == 0)
        std::cout << "Generated " << event << " events." << std::endl;


    } // end of event loop


    eventbuffer.EncodeEndEvent();
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

  } // end of run loop

}
