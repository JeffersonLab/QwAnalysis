/*------------------------------------------------------------------------*//*!

 \file QwMockDataAnalysis.cc

 \brief Parity mock data analysis, test code

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
#include "QwBeamLine.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwHistogramHelper.h"
#include "QwMainCerenkovDetector.h"
#include "QwSubsystemArrayParity.h"


// Number of events to generate
#define NEVENTS 1000
// Number of variables to correlate
#define NVARS 4


// Multiplet structure
static const int kMultiplet = 4;

// Debug level
static bool bDebug = false;

// Activate components
static bool bHisto = true;
static bool bTree = true;
static bool bBeamLine = true;
static bool bHelicity = true;
static bool bQuartz = true;

int main(int argc, char* argv[])
{
  // First, we set the command line arguments and the configuration filename,
  // and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwmockdataanalysis.conf");
  // Define the command line options
  DefineOptionsParity(gQwOptions);


  // Load histogram definitions
  gQwHists.LoadHistParamsFromFile(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/parity_hists.in");

  // Detector array
  QwSubsystemArrayParity detectors;
  if (bBeamLine) {
    detectors.push_back(new QwBeamLine("Injector BeamLine"));
    detectors.GetSubsystem("Injector BeamLine")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_beamline.map");
    detectors.GetSubsystem("Injector BeamLine")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_pedestal.map");
  }
  if (bQuartz) {
    detectors.push_back(new QwMainCerenkovDetector("Main detector"));
    //detectors.GetSubsystem("Main detector")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_adc.map");

    detectors.GetSubsystem("Main detector")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_adc.map");

    detectors.GetSubsystem("Main detector")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_pedestal.map");
  }
  if (bHelicity) {
    detectors.push_back(new QwHelicity("Helicity info"));
    detectors.GetSubsystem("Helicity info")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_helicity.map");
    detectors.GetSubsystem("Helicity info")->LoadInputParameters("");
  }
  QwHelicityPattern helicitypattern(detectors);

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
    TString datafilename = TString("QwMock_") + Form("%ld.log",run);
    if (eventbuffer.OpenDataFile(datafilename,"R") != CODA_OK) {
      std::cout << "Error: could not open file!" << std::endl;
      return 0;
    }
    eventbuffer.ResetControlParameters();


    // ROOT file output (histograms)
    TString rootfilename = TString(getenv("QWSCRATCH")) + TString("/rootfiles/QwMock_") + Form("%ld.root",run);
    TFile rootfile(rootfilename, "RECREATE", "QWeak ROOT file");
    if (bHisto) {
      rootfile.cd();
      detectors.ConstructHistograms(rootfile.mkdir("mps_histo"));
      if (bHelicity) {
        rootfile.cd();
        helicitypattern.ConstructHistograms(rootfile.mkdir("hel_histo"));
      }
    }

    // ROOT file output (trees)
    TTree *mpstree;
    TTree *heltree;
    Int_t eventnumber;
    std::vector <Double_t> mpsvector;
    std::vector <Double_t> helvector;
    if (bTree) {
      rootfile.cd();
      mpstree = new TTree("MPS_Tree","MPS event data tree");
      mpsvector.reserve(6000);
      mpstree->Branch("eventnumber",&eventnumber,"eventnumber/F");
      TString dummystr="";
      detectors.ConstructBranchAndVector(mpstree, dummystr, mpsvector);
      rootfile.cd();
      if (bHelicity) {
        rootfile.cd();
        heltree = new TTree("HEL_Tree","Helicity event data tree");
        helvector.reserve(6000);
        TString dummystr="";
        helicitypattern.ConstructBranchAndVector(heltree, dummystr, helvector);
      }
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
      eventnumber = eventbuffer.GetEventNumber();
      if      (eventnumber < eventnumber_min) continue;
      else if (eventnumber > eventnumber_max) break;

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

      // Check for helicity validity (TODO I'd prefer to use kUndefinedHelicity)
      if (bHelicity && helicity->GetHelicityDelayed() == -9999) continue;

      // Fill the histograms
      if (bHisto) detectors.FillHistograms();

      // Fill the MPS tree
      if (bTree) {
        eventnumber = eventbuffer.GetEventNumber();
        detectors.FillTreeVector(mpsvector);
        mpstree->Fill();
      }
      // Fill the helicity tree
      if (bHelicity && helicitypattern.IsCompletePattern()) {
        helicitypattern.CalculateAsymmetry();
        if (bHisto) helicitypattern.FillHistograms();
        if (bTree) {
          helicitypattern.FillTreeVector(helvector);
          heltree->Fill();
        }
        helicitypattern.ClearEventData();
      }

      // Periodically print event number
      if ((bDebug && eventbuffer.GetEventNumber() % 1000 == 0)
                  || eventbuffer.GetEventNumber() % 10000 == 0)
        std::cout << "Number of events processed so far: "
                  << eventbuffer.GetEventNumber() << std::endl;

    } // end of loop over events

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
