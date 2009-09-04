/*------------------------------------------------------------------------*//*!

 \defgroup QwCompton QwCompton

 \section comptonoverview Inclusion of the Compton polarimeter in the Qweak framework

    Although an existing analysis framework, developed at CMU, was available
    (and has been successfully used for beam tests at HIgS), we decided that
    it would be easier to incorporate the Compton polarimeter analysis in the
    Qweak framework.  The main advantages are that parity-type operations are
    readily available, that delayed helicity reporting is handled transparently
    to the user (i.e. the detector subsystem analysis writer), and that the
    particulars of the CODA data structure are abstracted away completely.

 \section comptonphilosphy Analysis philosophy

    In the Qweak analysis code all variables (i.e. electronic module channels)
    can be 'added' and 'subtracted' between helicity states.  This makes it
    easy to get an asymmetry on a helicity multiplet level, but also to get
    asymmetries or sums between different variables.

    It might seem funny at first sight that there are functions that define
    the sum, difference or ratio of two compton detectors.  But, after a
    moment's thought, that is exactly what you need when you calculate an
    asymmetry over a helicity multiplet:
    \f[
          P_{beam} = \frac{1}{P_{laser}} \frac{N^+ - N^-}{N^+ + N^-}.
    \f]
    In this case you perform all three of these operations between the data
    collected during the different beam helicity states (or alternatively,
    between the two laser helicity states, which could give us access to
    systematic differences between the two beam helicity states).

    \subsection comptonphotondetector Photon detector subsystem

    The only subsystem of the Compton polarimeter that has been implemented
    (partially) at the moment is the photon detector.  The sampling ADC, and
    integrating ADC/TDC have CODA readout support in the Qweak analysis code,
    making it easy to take asymmetries of different helicity events.

    The main difficulty in the code will be to access the laser helicity state
    through EPICS.

    \subsection comptonelectrondetector Electron detector subsystem

    The electron detector has not been implemented yet.  The GEM detectors in
    region 1 of Qweak use the same V1495 boards and are partially implemented.


*//*-------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!

 \file QwCompton.cc

 \ingroup QwCompton

 \brief Compton data analysis

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <iostream>

// Boost math library for random number generation
#include <boost/random.hpp>

// ROOT headers
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>

// CODA headers (for CODA status)
#include "THaCodaFile.h"

// Qweak headers
#include "QwCommandLine.h"
#include "QwEventBuffer.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwHistogramHelper.h"
#include "QwSubsystemArrayParity.h"

// Compton headers
#include "QwComptonPhotonDetector.h"
#include "QwComptonElectronDetector.h" // not really implemented (wdc)


// Multiplet structure
static const int kMultiplet = 4;

// Debug level
static bool bDebug = true;

// Activate components (no support for trees yet)
static bool bHisto = true;
static bool bHelicity = true;
static bool bComptonPhoton = true;
static bool bComptonElectron = true;

int main(int argc, char* argv[])
{
  // Load histogram definitions
  gQwHists.LoadHistParamsFromFile(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/parity_hists_compton.in");

  // Detector array
  QwSubsystemArrayParity detectors;
  if (bComptonPhoton) {
    detectors.push_back(new QwComptonPhotonDetector("Compton Photon Detector"));
    detectors.GetSubsystem("Compton Photon Detector")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/compton_channels.map");
    detectors.GetSubsystem("Compton Photon Detector")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/compton_pedestal.map");
  }
  if (bComptonElectron) {
    detectors.push_back(new QwComptonElectronDetector("Compton Electron Detector"));
    detectors.GetSubsystem("Compton Electron Detector")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/compton_channels.map");
    detectors.GetSubsystem("Compton Electron Detector")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/compton_pedestal.map");
  }
  QwHelicityPattern helicitypattern(detectors,kMultiplet);

  // Get the helicity
  QwHelicity* helicity = (QwHelicity*) detectors.GetSubsystem("Helicity info");

  // Event buffer
  QwEventBuffer eventbuffer;


  // Parse command line arguments
  QwCommandLine commandline;
  commandline.Parse(argc, argv);

  // Loop over the requested runs
  for (int run = commandline.GetFirstRun(); run <= commandline.GetLastRun(); run++) {

    // Data file (input)
    TString datafilename = TString("Compton_") + Form("%ld.log",run);
    if (eventbuffer.OpenDataFile(datafilename,"R") != CODA_OK) {
      std::cout << "Error: could not open file!" << std::endl;
      return 0;
    }
    eventbuffer.ResetControlParameters();


    // Loop over events in this CODA file
    while (eventbuffer.GetEvent() == CODA_OK) {

      // First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()) {
        // Send ROC configuration event data to the subsystem objects.
        eventbuffer.FillSubsystemConfigurationData(detectors);
      }

      // Now, if this is not a physics event, go back and get a new event.
      if (! eventbuffer.IsPhysicsEvent()) continue;

      //  Check to see if we want to process this event.
      if (eventbuffer.GetEventNumber() < commandline.GetFirstEvent()) continue;
      else if (eventbuffer.GetEventNumber() > commandline.GetLastEvent()) break;

      // Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(detectors);

      // Process this events
      detectors.ProcessEvent();

      // Helicity pattern
      if (bHelicity)
        helicitypattern.LoadEventData(detectors);

      // Print the helicity information
      if (bHelicity && bDebug) {
        // - actual helicity
        if      (helicity->GetHelicityActual() == 0) std::cout << "-";
        else if (helicity->GetHelicityActual() == 1) std::cout << "+";
        else std::cout << "?";
        // - delayed helicity
        if      (helicity->GetHelicityDelayed() == 0) std::cout << "(-) ";
        else if (helicity->GetHelicityDelayed() == 1) std::cout << "(+) ";
        else std::cout << "(?) ";
        if (helicity->GetPhaseNumber() == kMultiplet) {
          std::cout << std::hex << helicity->GetRandomSeedActual() << std::dec << ",  \t";
          std::cout << std::hex << helicity->GetRandomSeedDelayed() << std::dec << std::endl;
        }
      }


      // Fill the histograms
      if (bHisto) detectors.FillHistograms();

      // TODO We need another check here to test for pattern validity.  Right
      // now the first 24 cycles are also added to the histograms.
      if (bHelicity && helicitypattern.IsCompletePattern()) {
        helicitypattern.CalculateAsymmetry();
        if (bHisto) helicitypattern.FillHistograms();
        helicitypattern.ClearEventData();
      }

      // Periodically print event number
      if ((bDebug && eventbuffer.GetEventNumber() % 1000 == 0)
                  || eventbuffer.GetEventNumber() % 10000 == 0)
        std::cout << "Number of events processed so far: "
                  << eventbuffer.GetEventNumber() << std::endl;

    } // end of loop over events

    // Close data file and print run summary
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

  } // end of loop over runs


}