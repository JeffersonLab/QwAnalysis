/*------------------------------------------------------------------------*//*!

 \file QwAnalysis.cc

 \brief main(...) function for the qwanalysis executable

*//*-------------------------------------------------------------------------*/

// System headers
#include <iostream>
#include <fstream>
#include <vector>
#include <new>

// Boost headers
#include <boost/shared_ptr.hpp>

// ROOT headers
#include <Rtypes.h>
#include <TROOT.h>
#include <TFile.h>

// Qweak headers
#include "QwLog.h"
#include "QwRootFile.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwHistogramHelper.h"
#include "QwSubsystemArrayParity.h"
#include "QwHelicityPattern.h"
#include "QwEventRing.h"
#include "QwEPICSEvent.h"

// Qweak subsystems
// (for correct dependency generation)
#include "QwHelicity.h"
#include "QwFakeHelicity.h"
#include "QwBeamLine.h"
#include "QwMainCerenkovDetector.h"
#include "QwScanner.h"
#include "QwLumi.h"

// for correlator
#include "QwCorrelator.h"



Int_t main(Int_t argc, Char_t* argv[])
{
  ///  First, fill the search paths for the parameter files; this sets a
  ///  static variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");

  ///  Then, we set the command line arguments and the configuration filename,
  ///  and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.AddConfigFile("qwparity.conf");
  gQwOptions.AddConfigFile("qweak_mysql.conf");
  ///  Define the command line options
  DefineOptionsParity(gQwOptions);
  //Load command line options for the histogram/tree helper class
  gQwHists.ProcessOptions(gQwOptions);
  /// Setup screen and file logging
  gQwLog.ProcessOptions(&gQwOptions);

  ///  Load the histogram parameter definitions (from parity_hists.txt) into the global
  ///  histogram helper: QwHistogramHelper
  gQwHists.LoadHistParamsFromFile("qweak_parity_hists.in");
  gQwHists.LoadHistParamsFromFile("qweak_correlation_hists.in"); // add new
  gQwHists.LoadTreeParamsFromFile("Qweak_Tree_Trim_List.in");


  ///  Create the event buffer
  QwEventBuffer eventbuffer;
  eventbuffer.ProcessOptions(gQwOptions);

  ///  Create an EPICS event
  QwEPICSEvent epicsevent;
  epicsevent.LoadEpicsVariableMap("EpicsTable.map");


  ///  Load the detectors from file
  QwSubsystemArrayParity detectors(gQwOptions);
  detectors.ProcessOptions(gQwOptions);

  ///  Create the helicity pattern
  QwHelicityPattern helicitypattern(detectors);
  helicitypattern.ProcessOptions(gQwOptions);

  ///  Create the event ring
  QwEventRing eventring;
  eventring.ProcessOptions(gQwOptions);
  //  Set up the ring with subsysten array with CMD ring parameters
  eventring.SetupRing(detectors); // TODO (wdc) in QwEventRing constructor?

  ///  Create the running sum
  QwSubsystemArrayParity runningsum(detectors);

  ///  Set up the database connection
  QwDatabase database(gQwOptions);


  // initialization of correlator
  QwCorrelator correlator;
  correlator. SetInputVector(detectors);
  std::cout<<correlator<<std::endl;


  ///  Start loop over all runs
  QwRootFile* rootfile = 0;
  while (eventbuffer.OpenNextStream() == CODA_OK) {

    //  Begin processing for the first run.

    //  Open the ROOT file
    rootfile = new QwRootFile(eventbuffer.GetRunLabel());
    if (! rootfile) QwError << "QwAnalysis made a boo boo!" << QwLog::endl;


    //  Construct histograms
    rootfile->ConstructHistograms(detectors);
    rootfile->ConstructHistograms(helicitypattern);

    correlator.SetHistos();
    
    //  Construct tree branches
    rootfile->ConstructTreeBranches(detectors);
    rootfile->ConstructTreeBranches(helicitypattern);
    Int_t failed_events_counts = 0; // count failed total events
    // TODO (wdc) failed event counter in QwEventRing?


    //  Clear the single-event running sum at the beginning of the runlet
    runningsum.ClearEventData();
    helicitypattern.ClearRunningSum();
    //  Clear the running sum of the burst values at the beginning of the runlet
    helicitypattern.ClearBurstSum();


    ///  Start loop over events
    while (eventbuffer.GetNextEvent() == CODA_OK) {

      //  First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()) {
        //  Send ROC configuration event data to the subsystem objects.
        eventbuffer.FillSubsystemConfigurationData(detectors);
      }

      //  Secondly, process EPICS events
      if (eventbuffer.IsEPICSEvent()) {
        eventbuffer.FillEPICSData(epicsevent);
        epicsevent.CalculateRunningValues();
        helicitypattern.UpdateBlinder(&database,epicsevent);
      }


      //  Now, if this is not a physics event, go back and get a new event.
      if (! eventbuffer.IsPhysicsEvent()) continue;


      //  Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(detectors);

      //  Process the subsystem data
      detectors.ProcessEvent();


      if(eventbuffer.GetEventNumber()>200) { // tmp skip firts events
	correlator.AddEvent();
      }

      // The event pass the event cut constraints
      if (detectors.ApplySingleEventCuts()) {

        // Add event to the ring
        eventring.push(detectors);

        // Check to see ring is ready
        if (eventring.IsReady()) {
          helicitypattern.LoadEventData(eventring.pop());
        }


        // Accumulate the running sum to calculate the event based running average
        runningsum.AccumulateRunningSum(detectors);


        // Fill the histograms
        rootfile->FillHistograms(detectors);

        // Fill the tree branches
        rootfile->FillTreeBranches(detectors);

        // Calculate helicity pattern asymmetry
        if (helicitypattern.IsCompletePattern() && eventring.IsReady()) {
          // Update the blinder if conditions have changed
          helicitypattern.UpdateBlinder(&database,detectors);
          // Calculate the asymmetry
          helicitypattern.CalculateAsymmetry();
          if (helicitypattern.IsGoodAsymmetry()) {
            // Fill histograms
            rootfile->FillHistograms(helicitypattern);
            // Fill tree branches
            rootfile->FillTreeBranches(helicitypattern);
            // Clear the data
            helicitypattern.ClearEventData();
          }
        }

      } else {
        eventring.FailedEvent(detectors.GetEventcutErrorFlag()); //event cut failed update the ring status
        failed_events_counts++;
      }

      // Burst mode
      if (eventbuffer.IsEndOfBurst()) {
        helicitypattern.AccumulateRunningBurstSum();
        helicitypattern.CalculateBurstAverage();
        helicitypattern.ClearBurstSum();
      }

    } // end of loop over events

    QwMessage << "Number of events processed at end of run: "
              << eventbuffer.GetEventNumber() << std::endl;


    // Calculate running averages over helicity patterns
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

    /*  Write to the root file, being sure to delete the old cycles  *
     *  which were written by Autosave.                              *
     *  Doing this will remove the multiple copies of the ntuples    *
     *  from the root file.                                          *
     *                                                               *
     *  Then, we need to delete the histograms here.                 *
     *  If we wait until the subsystem destructors, we get a         *
     *  segfault; but in additiona to that we should delete them     *
     *  here, in case we run over multiple runs at a time.           */
    rootfile->Write(0,TObject::kOverwrite);

    //  Delete histograms
    rootfile->DeleteHistograms(detectors);
    rootfile->DeleteHistograms(helicitypattern);

    //  Close event buffer stream
    eventbuffer.CloseStream();



    //  Print the event cut error summery for each subsystem
    detectors.GetEventcutErrorCounters();

  
    //  Read from the datebase
    if (database.AllowsReadAccess()) {

      // GetRunID(), GetRunletID(), and GetAnalysisID have their own Connect() and Disconnect() functions.
      UInt_t run_id      = database.GetRunID(eventbuffer);
      UInt_t runlet_id   = database.GetRunletID(eventbuffer);
      UInt_t analysis_id = database.GetAnalysisID(eventbuffer);

     //  Write to from the datebase
     QwMessage << "QwAnalysis.cc::"
                << " Run Number "  << QwColor(Qw::kBoldMagenta) << eventbuffer.GetRunNumber() << QwColor(Qw::kNormal)
                << " Run ID "      << QwColor(Qw::kBoldMagenta) << run_id << QwColor(Qw::kNormal)
                << " Runlet ID "   << QwColor(Qw::kBoldMagenta) << runlet_id << QwColor(Qw::kNormal)
                << " Analysis ID " << QwColor(Qw::kBoldMagenta) << analysis_id
                << QwLog::endl;
    }

    // Each sussystem has its own Connect() and Disconnect() functions.
    if (database.AllowsWriteAccess()) {
      helicitypattern.FillDB(&database);
      epicsevent.FillDB(&database);
    }


    QwMessage << "Total events failed " << failed_events_counts << QwLog::endl;

    //  Report run summary
    eventbuffer.ReportRunSummary();
    eventbuffer.PrintRunTimes();
  } // end of loop over runs

  QwMessage << "I have done everything I can do..." << QwLog::endl;
  correlator.PrintSummary();

  return 0;
}

