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
#include "QwCommandLine.h"
#include "QwEventBuffer.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwHistogramHelper.h"
#include "QwSubsystemArrayParity.h"


// Number of events to generate
#define NEVENTS 1000
// Number of variables to correlate
#define NVARS 4


// Debug level
static bool bDebug = true;

// Activate components
static bool bHisto = true;
static bool bTree = true;
static bool bHelicity = true;

int main(int argc, char* argv[])
{
  // Load histogram definitions
  gQwHists.LoadHistParamsFromFile(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/parity_hists.in");

  // Detector array
  QwSubsystemArrayParity detectors;
  detectors.push_back(new QwBeamLine("Injector BeamLine"));
  detectors.GetSubsystem("Injector BeamLine")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_beamline.map");
  detectors.GetSubsystem("Injector BeamLine")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_pedestal.map");
  if (bHelicity) {
    detectors.push_back(new QwHelicity("Helicity info"));
    detectors.GetSubsystem("Helicity info")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/mock_qweak_helicity.map");
    detectors.GetSubsystem("Helicity info")->LoadInputParameters("");
  }
  QwHelicityPattern helicitypattern(detectors,4);

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
    TString datafilename = TString("QwMock_") + Form("%ld.log",run);
    if (eventbuffer.OpenDataFile(datafilename,"R") != CODA_OK) {
      std::cout << "Error: could not open file!" << std::endl;
      return 0;
    }
    eventbuffer.ResetControlParameters();

    // ROOT file (output)
    TString rootfilename = TString("QwMock_") + Form("%ld.root",run);
    TFile rootfile(rootfilename, "RECREATE", "QWeak ROOT file");
    if (bHisto) {
      rootfile.cd();
      detectors.ConstructHistograms(rootfile.mkdir("mps_histo"));
    }
    TTree *mpstree;
    Int_t eventnumber;
    std::vector <Double_t> mpsvector;
    if (bTree) {
      rootfile.cd();
      mpstree = new TTree("MPS_Tree","MPS event data tree");
      mpsvector.reserve(6000);
      mpstree->Branch("eventnumber",&eventnumber,"eventnumber/F");
      TString dummystr="";
      ((QwBeamLine*)detectors.GetSubsystem("Injector BeamLine"))->ConstructBranchAndVector(mpstree, dummystr, mpsvector);
      if (bHelicity)
        ((QwBeamLine*)detectors.GetSubsystem("Helicity info"))->ConstructBranchAndVector(mpstree, dummystr, mpsvector);
      rootfile.cd();
    }


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
        if (helicity->GetPhaseNumber() == 4) {
          std::cout << std::hex << helicity->GetRandomSeedActual() << std::dec << ",  \t";
          std::cout << std::hex << helicity->GetRandomSeedDelayed() << std::dec << std::endl;
        }
      }


      // Fill the histograms
      if (bHisto) detectors.FillHistograms();

      // Fill the tree
      if (bTree) {
        eventnumber = eventbuffer.GetEventNumber();
        ((QwBeamLine*)detectors.GetSubsystem("Injector BeamLine"))->FillTreeVector(mpsvector);
        if (bHelicity)
          ((QwHelicity*)detectors.GetSubsystem("Helicity info"))->FillTreeVector(mpsvector);
        mpstree->Fill();
      }

      // Periodically print event number
      if (eventbuffer.GetEventNumber() % 1000 == 0)
        std::cout << "Number of events processed so far: "
                  << eventbuffer.GetEventNumber() << std::endl;

    } // end of loop over events

    // Close ROOT file
    rootfile.Write(0,TObject::kOverwrite);
    if (bHisto) {
      detectors.DeleteHistograms();
    }

    // Close data file and print run summary
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

  } // end of loop over runs


}
