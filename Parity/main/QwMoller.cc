/*------------------------------------------------------------------------*//*!

 \defgroup QwMoller QwMoller

*//*-------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!

 \file QwMoller.cc

 \ingroup QwMoller

 \brief Moller data analysis

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <iostream>
#include <math.h>
#include <sstream>

// Boost math library for random number generation
#include <boost/random.hpp>

// ROOT headers
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>


// Qweak headers
#include "QwLog.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwHistogramHelper.h"
#include "QwSubsystemArrayParity.h"

// Moller headers
#include "QwMollerDetector.h"

//global variables
static const int scal_num = 96;
float gscalar[scal_num], gscaler_old[scal_num], gscaler_new[scal_num], gscaler_change[scal_num];

// Activate components
static bool bTree = true;
static bool bHisto = true;
static bool bHelicity = true;

void setOptions();

int main(int argc, char* argv[])
{

  std::cout << std::endl << " -= Moller Run Analyzer Started =-" << std::endl;
  //QwParameterFile::AppendToSearchPath(".");
  gQwOptions.SetConfigFile("moller.flags");

  /// Set the expected options, command line arguments and the configuration filename
  setOptions();
  gQwOptions.SetCommandLine(argc, argv);

  DefineOptionsParity(gQwOptions);
  gQwOptions.Parse();

  std::cout << "bcm_offsets: " << gQwOptions.GetValue<float>("bcm1_offset") << ' ' << gQwOptions.GetValue<float>("bcm2_offset") << ' ' << \
               gQwOptions.GetValue<float>("bcm3_offset") << std::endl;

  std::cout << "bcm_gains: " << gQwOptions.GetValue<float>("bcm1_gain") << ' ' << gQwOptions.GetValue<float>("bcm2_gain") << ' ' << 
               gQwOptions.GetValue<float>("bcm3_gain") << std::endl;
 
  std::cout << "norm_bcm: " << gQwOptions.GetValue<float>("norm_bcm") << std::endl;

  /// Message logging facilities
  gQwLog.InitLogFile("Moller.log", QwLog::kTruncate);
  gQwLog.SetScreenThreshold(QwLog::kMessage);
  gQwLog.SetFileThreshold(QwLog::kDebug);

  ///  Fill the search paths for the parameter files; this sets a static
  ///  variable within the QwParameterFile class which will be used by
  ///  all instances.  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");

  // Load histogram definitions
  gQwHists.LoadHistParamsFromFile("moller_hists.in");



  // Detector array
  QwSubsystemArrayParity detectors(gQwOptions);
  detectors.ProcessOptions(gQwOptions);
  detectors.UpdateEventTypeMask();

  ///  Create the running sum
  QwSubsystemArrayParity runningsum(detectors);

  // Helicity pattern
  QwHelicityPattern helicitypattern(detectors);

  // Event buffer
  QwEventBuffer eventbuffer;

  std::string fileName;

  /// System setup complete,
  /// Here's where we would loop over all specified runs
  //UInt_t runnumber_min = (UInt_t) gQwOptions.GetIntValuePairFirst("run");
  //UInt_t runnumber_max = (UInt_t) gQwOptions.GetIntValuePairLast("run");
  //std::cout << "RUN: " << runnumber_min << " to " << runnumber_max << std::endl;
  for (UInt_t run = 15; run <= 15; run++) {

    // Data file (input)
    if (gQwOptions.HasValue("input_file")){
      fileName = gQwOptions.GetValue<std::string>("input_file");
    } else { // if (eventbuffer.OpenDataFile(TString("moller-moller_") + Form("%ld.log.0",run),"R")) {
      fileName = TString("moller-moller_") + Form("%u.log.0",run);
      //     } else {
      //       QwError << "No Input File Specified!" << std::endl;
      //       return 1;
    }
  
    // Open Data file
    if (eventbuffer.OpenDataFile(fileName,"R") != CODA_OK) {
      QwError << "Could not open file!" << QwLog::endl;
      return 1;
    }

    // ROOT file output (histograms)
    TString rootfilename = getenv_safe("QW_ROOTFILES")
                         + TString("/Moller_") + Form("%u.root",run);
    TFile rootfile(rootfilename, "RECREATE", "QWeak ROOT file");
 
    if (bHisto) {
      rootfile.cd();
      detectors.ConstructHistograms(rootfile.mkdir("moller_histo"));
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
    std::vector <Double_t> mpsvector;
    std::vector <Double_t> helvector;

    if (bTree) {
      // MPS events
      rootfile.cd();
      mpstree = new TTree("MPS_Tree","MPS event data tree");
      mpsvector.reserve(6000);
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

    

    while (eventbuffer.GetEvent() == CODA_OK) {
      
      /// First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()) {
          std::cout << "it is ROC CONFIG event\n";
          // Send ROC configuration event data to the subsystem objects.
        // eventbuffer.FillSubsystemConfigurationData(detectors);
      }
  
      if (!eventbuffer.IsPhysicsEvent()) continue;
  
      /// Add event into the subsystem 
      eventbuffer.FillSubsystemData(detectors);

      detectors.ProcessEvent();
      detectors.IncrementErrorCounters();

      /// Helicity pattern
      helicitypattern.LoadEventData(detectors);
      
      /// Accumulate the running sum to calculate the event based running average
      runningsum.AccumulateRunningSum(detectors);

      /// Print the helicity information
      //if (bHelicity && false) {
        // - actual helicity
        //std::cout << (helicity->GetHelicityReported() == 0 ? "-" : helicity->GetHelicityReported() == 1 ? "+" : "?");
        // - delayed helicity
        //std::cout << (helicity->GetHelicityActual() == 0 ? "(-) " : helicity->GetHelicityActual() == 1 ? "(+) " : "(?) ");
        //if (helicity->GetPhaseNumber() == kMultiplet) {
        //  std::cout << std::uppercase << std::hex << helicity->GetRandomSeedActual() << ",  \t";
        //  std::cout << helicity->GetRandomSeedDelayed() << std::dec << std::endl;
        //}
      //}

      /// Fill the histograms
      if (bHisto) detectors.FillHistograms();

      /// Fill the expert tree
      if (bTree) detectors.FillTree();

      /// Fill the tree
      if (bTree) {
        detectors.FillTreeVector(mpsvector);
        mpstree->Fill();
      }

      /// TODO We need another check here to test for pattern validity.  Right
      /// now the first 24 cycles are also added to the histograms.
      if (bHelicity && helicitypattern.IsCompletePattern()) {
        helicitypattern.CalculateAsymmetry();
        if (bHisto) helicitypattern.FillHistograms();
       // helicitypattern.ClearEventData();
      }


      /// Periodically print event number
      if (eventbuffer.GetEventNumber() % 5000 == 0){
        QwMessage << "Number of events processed so far: " << eventbuffer.GetEventNumber() << QwLog::endl;
      }

   } /// end of loop over events

    std::cout << "Last event processed: " << eventbuffer.GetEventNumber() << std::endl;
    /// Close ROOT file
    rootfile.Write(0,TObject::kOverwrite);

    // Close data file and print run summary
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

  } // end of loop over runs

  if (helicitypattern.IsEndOfBurst()) {
    helicitypattern.AccumulateRunningBurstSum();
    helicitypattern.CalculateBurstAverage();
    helicitypattern.ClearBurstSum();
  }

  if (helicitypattern.IsRunningSumEnabled()) {
    helicitypattern.CalculateRunningAverage();
    helicitypattern.PrintRunningAverage();
    if (helicitypattern.IsBurstSumEnabled()) {
      helicitypattern.CalculateRunningBurstAverage();
      helicitypattern.PrintRunningBurstAverage();
    }
  }

  // This will calculate running averages over single helicity events
  runningsum.CalculateRunningAverage();
  QwMessage << " Running average of events" << QwLog::endl;
  QwMessage << " =========================" << QwLog::endl;
  runningsum.PrintValue();


  std::cout << std::endl << " -= Moller Run Analyzer Ended =-" << std::endl << std::endl;
  return 0;
}

void setOptions(){
  gQwOptions.AddOptions()("EXPERIMENT", po::value<std::string>(), "Name of the experiment");
  gQwOptions.AddOptions()("input_file,f", po::value<std::string>(), "Input file to be used");
  gQwOptions.AddOptions()("input_directory,d", po::value<std::string>(), "Directory to be searched for input files, default is current");
  gQwOptions.AddOptions()("output_directory,o", po::value<std::string>(), "Directory where output files are saved");
  gQwOptions.AddOptions()("pol_factor", po::value<float>(), "This needs to be explained to me");
  gQwOptions.AddOptions()("bcm1_offset", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm2_offset", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm3_offset", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm1_gain", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm2_gain", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm3_gain", po::value<float>(), " ");
  gQwOptions.AddOptions()("norm_bcm", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm1_index", po::value<int>(), " ");
  gQwOptions.AddOptions()("bcm2_index", po::value<int>(), " ");
  gQwOptions.AddOptions()("bcm3_index", po::value<int>(), " ");
  gQwOptions.AddOptions()("clock_index", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcur_limit", po::value<float>(), " ");
  gQwOptions.AddOptions()("timebin", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcur_units", po::value<std::string>(), " ");
  gQwOptions.AddOptions()("helicity_pattern", po::value<int>()->default_value(4), "Length of the helicity window, should be 4, 8, or 16");
}
