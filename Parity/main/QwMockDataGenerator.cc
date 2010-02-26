/*------------------------------------------------------------------------*//*!

 \file QwMockDataGenerator.cc

 \brief Parity mock data generator, test code

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <iostream>

// Boost math library for random number generation
#include <boost/random.hpp>

// CODA headers (for CODA status)
#include "THaCodaFile.h"

// Qweak headers
#include "QwLog.h"
#include "QwBeamLine.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwMainCerenkovDetector.h"
#include "QwSubsystemArrayParity.h"
#include "QwVQWK_Channel.h"


// Number of variables to correlate
#define NVARS 3


// Multiplet structure
static const int kMultiplet = 4;

// Beam trips on qwk_bcm0l03
static const bool kBeamTrips = true;

// Debug
static const bool kDebug = false;

int main(int argc, char* argv[])
{
  // First, we set the command line arguments and the configuration filename,
  // and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwmockdataanalysis.conf");
  // Define the command line options
  DefineOptionsParity(gQwOptions);


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
  detectors.push_back(new QwMainCerenkovDetector("Main detector"));
  detectors.GetSubsystem("Main detector")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_adc.map");
  detectors.GetSubsystem("Main detector")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_pedestal.map");
  QwHelicityPattern QwHelPat(detectors);

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


  // Get the beamline channels we want to correlate
  QwBeamLine* beamline = (QwBeamLine*) detectors.GetSubsystem("Injector BeamLine");

  // Get some BCMs
  QwBCM* bcm[8];
  bcm[0] = beamline->GetBCM("qwk_bcm0l00");
  bcm[1] = beamline->GetBCM("qwk_bcm0l01");
  bcm[2] = beamline->GetBCM("qwk_bcm0l02");
  bcm[3] = beamline->GetBCM("qwk_bcm0l03");
  bcm[4] = beamline->GetBCM("qwk_bcm0l04");
  bcm[5] = beamline->GetBCM("qwk_bcm0l05");
  bcm[6] = beamline->GetBCM("qwk_bcm0l06");
  bcm[7] = beamline->GetBCM("qwk_bcm0l07");
  Double_t bcm_mean = 2.5e7;
  Double_t bcm_sigma = 2.5e6;
  for (int i = 0; i < 8; i++)
    bcm[i]->SetRandomEventParameters(bcm_mean, bcm_sigma);
  // Set helicity asymmetry for the BCMs (0,1,2 are correlated)
  bcm[3]->SetRandomEventAsymmetry(3.0e-3);
  bcm[4]->SetRandomEventAsymmetry(4.0e-4);
  bcm[5]->SetRandomEventAsymmetry(5.0e-5);
  bcm[6]->SetRandomEventAsymmetry(6.0e-6);
  bcm[7]->SetRandomEventAsymmetry(7.0e-7);

  // Set a current noise component (amplitude, phase, frequency)
  beamline->GetBCM("qwk_bcm0l03")->AddRandomEventDriftParameters(2.0e6, 0, 60*Qw::Hz);
  beamline->GetBCM("qwk_bcm0l03")->AddRandomEventDriftParameters(3.3e5, 0, 120*Qw::Hz);
  beamline->GetBCM("qwk_bcm0l03")->AddRandomEventDriftParameters(4.0e4, 0, 180*Qw::Hz);
  beamline->GetBCM("qwk_bcm0l03")->AddRandomEventDriftParameters(5.3e3, 0, 240*Qw::Hz);


  // Get some BPMs
  QwBPMStripline* bpm[2];
  bpm[0] = beamline->GetBPMStripline("qwk_0r06");
  bpm[1] = beamline->GetBPMStripline("qwk_0l06");
  Double_t bpm_meanX = 6.0;
  Double_t bpm_sigmaX = 3.0e-3;
  Double_t bpm_meanY = -1.5;
  Double_t bpm_sigmaY = 4.0e-3;
  for (int i = 0; i < 2; i++)
    bpm[i]->SetRandomEventParameters(bpm_meanX, bpm_sigmaX, bpm_meanY, bpm_sigmaY);


  // Get the main detector channels we want to correlate
  QwMainCerenkovDetector* maindetector = (QwMainCerenkovDetector*) detectors.GetSubsystem("Main detector");
  Double_t bar_mean = 2.0e7;
  Double_t bar_sigma = 3.0e4;
  Double_t bar_asym = 4.0e-4;
  maindetector->SetRandomEventParameters(bar_mean, bar_sigma);
  maindetector->SetRandomEventAsymmetry(bar_asym);
  // Specific values
  maindetector->GetChannel("Bar2Left")->SetRandomEventAsymmetry(2.0e-2);
  maindetector->GetChannel("Bar2Right")->SetRandomEventAsymmetry(2.0e-2);
  maindetector->GetChannel("Bar3Left")->SetRandomEventAsymmetry(3.0e-3);
  maindetector->GetChannel("Bar3Right")->SetRandomEventAsymmetry(3.0e-3);
  maindetector->GetChannel("Bar4Left")->SetRandomEventAsymmetry(4.0e-4);
  maindetector->GetChannel("Bar4Right")->SetRandomEventAsymmetry(4.0e-4);
  maindetector->GetChannel("Bar5Left")->SetRandomEventAsymmetry(5.0e-5);
  maindetector->GetChannel("Bar5Right")->SetRandomEventAsymmetry(5.0e-5);
  maindetector->GetChannel("Bar6Left")->SetRandomEventAsymmetry(6.0e-6);
  maindetector->GetChannel("Bar6Right")->SetRandomEventAsymmetry(6.0e-6);
  maindetector->GetChannel("Bar7Left")->SetRandomEventAsymmetry(7.0e-7);
  maindetector->GetChannel("Bar7Right")->SetRandomEventAsymmetry(7.0e-7);
  maindetector->GetChannel("Bar8Left")->SetRandomEventAsymmetry(8.0e-8);
  maindetector->GetChannel("Bar8Right")->SetRandomEventAsymmetry(8.0e-8);

  // Set a asymmetric helicity asymmetry on one of the bars
  maindetector->GetChannel("Bar1Left")->SetRandomEventAsymmetry(5.0e-5);
  maindetector->GetChannel("Bar1Right")->SetRandomEventAsymmetry(-5.0e-5);

  // Set a drift component (amplitude, phase, frequency)
  maindetector->GetChannel("Bar3Left")->AddRandomEventDriftParameters(3.0e6, 0, 60*Qw::Hz);
  maindetector->GetChannel("Bar3Left")->AddRandomEventDriftParameters(6.0e5, 0, 120*Qw::Hz);
  maindetector->GetChannel("Bar3Left")->AddRandomEventDriftParameters(4.5e5, 0, 240*Qw::Hz);

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



  // Loop over all runs
  UInt_t runnumber_min = (UInt_t) gQwOptions.GetIntValuePairFirst("run");
  UInt_t runnumber_max = (UInt_t) gQwOptions.GetIntValuePairLast("run");
  for (UInt_t run  = runnumber_min;
              run <= runnumber_max;
              run++) {

    // Open new output file
    // (giving run number as argument to OpenDataFile confuses the segment search)
    TString filename = Form("QwMock_%ld.log", run);
    if (eventbuffer.OpenDataFile(filename,"W") != CODA_OK) {
      std::cout << "Error: could not open file!" << std::endl;
      return 0;
    }
    eventbuffer.ResetControlParameters();
    eventbuffer.EncodePrestartEvent(run, 0); // prestart: runnumber, runtype
    eventbuffer.EncodeGoEvent();


    // Helicity initialization loop
    helicity->SetEventPatternPhase(-1, -1, -1);
    // 24-bit seed, should be larger than 0x1, 0x55 = 0101 0101
    // Consecutive runs should have no trivially related seeds:
    // e.g. with 0x2 * run, the first two files will be just 1 MPS offset...
    unsigned int seed = 0x1234 ^ run;
    helicity->SetFirst24Bits(seed & 0xFFFFFF);


    // Retrieve the requested range of event numbers
    if (kDebug) std::cout << "Starting event loop..." << std::endl;
    Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
    Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");
    // Warn when only few events are requested, probably a problem in the input
    if (abs(eventnumber_max - eventnumber_min) < 10)
      QwWarning << "Only " << abs(eventnumber_max - eventnumber_min)
                << " events will be generated." << QwLog::endl;
    // Event generation loop
    for (Int_t event = eventnumber_min; event <= eventnumber_max; event++) {

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


      // Cause a beam trip :-)
      //
      // If the kBeamTrips flag is set, beam trips are included every 'period'
      // events.  The beam trip has a length of 'length' events.  The BCM is
      // still randomized, but the mean is adjusted by a linear ramp down.  It
      // then jumps up immediately again.
      if (kBeamTrips) {

        // Time assuming one ms for every helicity window
        double helicity_window = Qw::ms;
        double time = event * helicity_window;

        // Period = time between trips
        // Length = length of a trip
        double period = Qw::hour / 17.0;
        double length = 1.0 * Qw::sec;

        // Periodicity
        if (fmod(time, period) >= period - length) {
          // Do the ramp down
          QwBCM* bcm = beamline->GetBCM("qwk_bcm0l03");
          double scale = double(period - fmod(time, period)) / double(length);
          bcm->SetRandomEventParameters(bcm_mean * scale, bcm_sigma);
        }

        // Set the scale back to what it was after a trip
        if (fmod(time, period) < helicity_window) {
          QwBCM* bcm = beamline->GetBCM("qwk_bcm0l03");
          bcm->SetRandomEventParameters(bcm_mean, bcm_sigma);
        }
      } // end of beam trips


      // Fill the detectors with randomized data
      int myhelicity = helicity->GetHelicityActual() ? +1 : -1;

      // Pass the event number as a time unit
      beamline->GetBCM("qwk_bcm0l03")->SetEventNumber(event);
      maindetector->GetChannel("Bar3Left")->SetEventNumber(event);

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

    QwMessage << "Wrote mock data run " << filename << " successfully." << QwLog::endl;

  } // end of run loop

} // end of main
