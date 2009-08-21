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
#define NVARS 3


// Multiplet structure
static const int kMultiplet = 4;

// Debug
static bool kDebug = false;

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
  QwHelicityPattern QwHelPat(detectors, kMultiplet);

  // Get the helicity
  QwHelicity* helicity = (QwHelicity*) detectors.GetSubsystem("Helicity info");

  // Possible scenarios:
  // - everything is random, no correlations at all, no asymmetries at all
  // - variations in the mean of position, current, yield over the course of
  //   a run (linearly with run number, change mean/sigma as function of event
  //   number)
  // - one parameter has a helicity-correlated asymmetry, every other parameter
  //   is random and independently distributed
  // - two parameters have independent helicity-correlated asymmetries
  // - two parameters have correlated helicity-correlated asymmetries

  // Get the channels we want to correlate
  QwBeamLine* beamline = (QwBeamLine*) detectors.GetSubsystem("Injector BeamLine");

  // Some BCMs
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

  // Set asymmetry for one BCM
  Double_t bcm_asym = 1.0e-5;
  bcm[7]->SetRandomEventAsymmetry(bcm_asym);

  // Some BPMs
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


    // Helicity initialization loop
    unsigned int seed = 0x2; // 24-bit seed, should be larger than 0x1, 0x55 = 0101 0101
    helicity->SetFirst24Bits(seed);


    // Event generation loop
    if (kDebug) std::cout << "Starting event loop..." << std::endl;
    for (Int_t event = commandline.GetFirstEvent();
               event <= commandline.GetLastEvent(); event++) {

      // First clear the event
      detectors.ClearEventData();

      // Set the event, pattern and phase number
      // - event number increments for every event
      // - pattern number increments for every multiplet
      // - phase number gives position in multiplet
      helicity->SetEventPatternPhase(event, event / kMultiplet, event % kMultiplet + 1);

      // Run the helicity predictor
      helicity->RunPredictor();
      // Concise helicity printout
      if (kDebug) {
        // - actual helicity
        if      (helicity->GetHelicityActual() == 0) std::cout << "-";
        else if (helicity->GetHelicityActual() == 1) std::cout << "+";
        else std::cout << "?";
        // - delayed helicity
        if      (helicity->GetHelicityDelayed() == 0) std::cout << "(-) ";
        else if (helicity->GetHelicityDelayed() == 1) std::cout << "(+) ";
        else std::cout << "(?) ";
        if (event % kMultiplet + 1 == 4) {
          std::cout << std::hex << helicity->GetRandomSeedActual() << std::dec << ",  \t";
          std::cout << std::hex << helicity->GetRandomSeedDelayed() << std::dec << std::endl;
        }
      }

      // Fill the detectors with randomized data
      int myhelicity = helicity->GetHelicityActual() ? +1 : -1;
      detectors.RandomizeEventData(myhelicity);

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
        z[var] = bcm_mean + bcm_sigma * normal();
        C[var][var] = 1.0;
      }
      /* Sigma =
           1.00000   0.50000   0.50000
           0.50000   2.00000   0.30000
           0.50000   0.30000   1.50000
         C =
           1.00000   0.50000   0.50000
           0.00000   1.32288   0.03780
           0.00000   0.00000   1.11739
       */
      C[0][0] = 1.0; C[0][1] = 0.5;     C[0][2] = 0.5;
      C[1][0] = 0.0; C[1][1] = 1.32288; C[1][2] = 0.03780;
      C[2][0] = 0.0; C[2][1] = 0.0;     C[2][2] = 1.11739;
      for (int i = 0; i < NVARS; i++)
        for (int j = 0; j < NVARS; j++)
          x[i] += C[j][i] * z[j];

      // Assign to BCMs
      bcm[0]->SetHardwareSum(x[0]);
      bcm[1]->SetHardwareSum(x[1]);
      bcm[2]->SetHardwareSum(x[2]);


      // Write this event to file
      eventbuffer.EncodeSubsystemData(detectors);


      // Periodically print event number
      if ((kDebug && event % 1000 == 0)
                  || event % 10000 == 0)
        std::cout << "Generated " << event << " events." << std::endl;


    } // end of event loop


    eventbuffer.EncodeEndEvent();
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

  } // end of run loop

} // end of main
