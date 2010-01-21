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
    the sum, difference or ratio of two Compton detectors.  But, after a
    moment's thought, that is exactly what you need when you calculate an
    asymmetry over a helicity multiplet:
    \f[
          P_{beam} = \frac{1}{P_{laser}} \frac{N^+ - N^-}{N^+ + N^-}.
    \f]
    In this case you perform all three of these operations between the data
    collected during the different beam helicity states (or alternatively,
    between the two laser helicity states, which could give us access to
    systematic differences between the two beam helicity states).

    The details of how this addition and subtraction is performed have to be
    specified by the subsystem developer.  For example, in the case of the
    QwComptonPhotonDetector, each helicity window consists of a varying number
    of sampled photon detector pulses.  The goal could be to use the above
    formula to get an 'asymmety spectrum' of the pulses.  In this case, one
    would have to take care not to add 10 positive helicity events and only
    8 negative helicity events.  The normalization has to be included in the
    addition and subtraction.  The same thing is true in accumulator mode,
    where the zeroth accumulator (sample counter) should be used to normalize.

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
#include "QwLog.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwHistogramHelper.h"
#include "QwSubsystemArrayParity.h"

// Compton headers
#include "QwComptonPhotonDetector.h"
#include "QwComptonElectronDetector.h"
//
#include "MQwSIS3320_Channel.h"

// Multiplet structure
static const int kMultiplet = 4;

// Debug level
static bool bDebug = false;

// Activate components
static bool bTree = true;
static bool bHisto = true;
static bool bHelicity = false;
static bool bComptonPhoton = true;
static bool bComptonElectron = true;

int main(int argc, char* argv[])
{
  //Set the command line arguments and the configuration filename
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwcompton.conf");
  // Define the command line options
  gQwOptions.DefineOptions();

  // Message logging facilities
  gQwLog.InitLogFile("QwCompton.log", QwLog::kTruncate);
  gQwLog.SetScreenThreshold(QwLog::kMessage);
  gQwLog.SetFileThreshold(QwLog::kDebug);

  // Load histogram definitions
  gQwHists.LoadHistParamsFromFile(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/parity_hists.in");

  // Detector array
  QwSubsystemArrayParity detectors;
  // Photon detector
  QwComptonPhotonDetector* photon = 0;
  if (bComptonPhoton) {
    detectors.push_back(new QwComptonPhotonDetector("Compton Photon Detector"));
    photon = dynamic_cast<QwComptonPhotonDetector*> (detectors.GetSubsystem("Compton Photon Detector"));
    if (photon) {
      photon->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/compton_photon_channels.map");
      photon->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/compton_photon_pedestal.map");
      photon->Print();
    } else QwError << "Could not initialize photon detector!" << QwLog::endl;
  }
  // Electron detector
  QwComptonElectronDetector* electron = 0;
  if (bComptonElectron) {
    detectors.push_back(new QwComptonElectronDetector("Compton Electron Detector"));
    electron = dynamic_cast<QwComptonElectronDetector*> (detectors.GetSubsystem("Compton Electron Detector"));
    if (electron) {
      electron->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/compton_electron_channels.map");
      electron->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/compton_electron_pedestal.map");
      electron->Print();
    } else QwError << "Could not initialize electron detector!" << QwLog::endl;
  }
  // Helicity subsystem
  if (bHelicity) {
    detectors.push_back(new QwHelicity("Helicity info"));
    detectors.GetSubsystem("Helicity info")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_helicity.map");
    detectors.GetSubsystem("Helicity info")->LoadInputParameters("");
  }
  // Helicity pattern
  QwHelicityPattern helicitypattern(detectors, kMultiplet);

  // Get the SIS3320 channel for debugging
  MQwSIS3320_Channel* sampling = 0;
  if (bComptonPhoton) {
    sampling = photon->GetSIS3320Channel("compton");
  }

  // Get the helicity
  QwHelicity* helicity = (QwHelicity*) detectors.GetSubsystem("Helicity info");

  // Event buffer
  QwEventBuffer eventbuffer;


  // Loop over all runs
  UInt_t runnumber_min = (UInt_t) gQwOptions.GetIntValuePairFirst("run");
  UInt_t runnumber_max = (UInt_t) gQwOptions.GetIntValuePairLast("run");
  for (UInt_t run  = runnumber_min;
              run <= runnumber_max;
              run++) {

    // Data file (input)
    TString datafilename = TString("Compton_") + Form("%ld.log",run);
    if (eventbuffer.OpenDataFile(datafilename,"R") != CODA_OK) {
      QwError << "Could not open file!" << QwLog::endl;
      return 0;
    }
    eventbuffer.ResetControlParameters();


    // ROOT file output (histograms)
    TString rootfilename = TString(getenv("QWSCRATCH")) + TString("/rootfiles/")
                         + TString("Compton_") + Form("%ld.root",run);
    TFile rootfile(rootfilename, "RECREATE", "QWeak ROOT file");
    if (bHisto) {
      rootfile.cd();
      detectors.ConstructHistograms(rootfile.mkdir("mps_histo"));
      if (bHelicity) {
        rootfile.cd();
        helicitypattern.ConstructHistograms(rootfile.mkdir("hel_histo"));
      }
    }

    // ROOT file output (expert tree)
    if (bTree) {
      rootfile.cd();
      detectors.ConstructTree(rootfile.mkdir("expert"));
    }

    // ROOT file output (trees)
    TTree *mpstree;
    TTree *heltree;
    Int_t eventnumber;
    std::vector <Double_t> mpsvector;
    std::vector <Double_t> helvector;
    if (bTree) {
      // MPS events
      rootfile.cd();
      mpstree = new TTree("MPS_Tree","MPS event data tree");
      mpsvector.reserve(6000);
      mpstree->Branch("eventnumber",&eventnumber,"eventnumber/F");
      TString dummystr="";
      detectors.ConstructBranchAndVector(mpstree, dummystr, mpsvector);
      rootfile.cd();
      if (bHelicity) {
        // Helicity events (per multiplet)
        rootfile.cd();
        heltree = new TTree("HEL_Tree","Helicity event data tree");
        helvector.reserve(6000);
        TString dummystr="";
        helicitypattern.ConstructBranchAndVector(heltree, dummystr, helvector);
      }
    }

    // Fake helicity generation (awaiting real helicity signals)
    if (bHelicity) {
      helicity->SetEventPatternPhase(-1, -1, -1);
      unsigned int seed = 0x1234 ^ run;
      helicity->SetFirst24Bits(seed & 0xFFFFFF);
    }

    // Loop over events in this CODA file
    Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
    Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");
    while (eventbuffer.GetEvent() == CODA_OK) {

      // First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()) {
        // Send ROC configuration event data to the subsystem objects.
        eventbuffer.FillSubsystemConfigurationData(detectors);
      }

      // Now, if this is not a physics event, go back and get a new event.
      if (! eventbuffer.IsPhysicsEvent()) continue;

      //  Check to see if we want to process this event.
      Int_t eventnumber = eventbuffer.GetEventNumber();
      if      (eventnumber < eventnumber_min) continue;
      else if (eventnumber > eventnumber_max) break;

      // Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(detectors);

      // Fake helicity pattern
      if (bHelicity) {
        helicity->SetEventPatternPhase(eventnumber, eventnumber / kMultiplet, eventnumber % kMultiplet + 1);
        helicity->RunPredictor();
      }

      // Process this events
      detectors.ProcessEvent();

      // Helicity pattern
      if (bHelicity)
        helicitypattern.LoadEventData(detectors);

      // Print the helicity information
      if (bHelicity /* && bDebug */ ) {
        // - actual helicity
        if      (helicity->GetHelicityActual() == 0) QwOut << "-";
        else if (helicity->GetHelicityActual() == 1) QwOut << "+";
        else QwOut << "?";
        // - delayed helicity
        if      (helicity->GetHelicityDelayed() == 0) QwOut << "(-) ";
        else if (helicity->GetHelicityDelayed() == 1) QwOut << "(+) ";
        else QwOut << "(?) ";
        if (helicity->GetPhaseNumber() == kMultiplet) {
          QwOut << std::hex << helicity->GetRandomSeedActual() << std::dec << ",  \t";
          QwOut << std::hex << helicity->GetRandomSeedDelayed() << std::dec << QwLog::endl;
        }
      }

      // Print some debugging info
      if (bComptonPhoton && bDebug)
        sampling->Print();

      // Fill the histograms
      if (bHisto) detectors.FillHistograms();

      // Fill the expert tree
      if (bTree) detectors.FillTree();

      // Fill the tree
      if (bTree) {
        eventnumber = eventbuffer.GetEventNumber();
        detectors.FillTreeVector(mpsvector);
        mpstree->Fill();
      }


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
        QwMessage << "Number of events processed so far: "
                  << eventbuffer.GetEventNumber() << QwLog::endl;

    } // end of loop over events

    QwMessage << "Last event processed: "
              << eventbuffer.GetEventNumber() << QwLog::endl;

    // Close ROOT file
    rootfile.Write(0,TObject::kOverwrite);
    // Delete histograms
    if (bHisto) {
      detectors.DeleteHistograms();
      if (bHelicity) helicitypattern.DeleteHistograms();
    }
    // Close data file and print run summary
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

  } // end of loop over runs


}