
/*------------------------------------------------------------------------*//*!

 \file QwFeedback.cc

 \brief main(...) function for the qwanalysis feedback executable to set IA and/or PITA slopes

*//*-------------------------------------------------------------------------*/

// System headers
#include <iostream>
#include <fstream>
#include <vector>
#include <new>

// Boost headers
#include <boost/shared_ptr.hpp>

// Qweak headers
#include "QwLog.h"
#include "QwRootFile.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwHistogramHelper.h"
#include "QwSubsystemArrayParity.h"
#include "QwHelicityCorrelatedFeedback.h"
#include "QwEventRing.h"
#include "QwEPICSEvent.h"
//#include "QwEPICSControl.h"
//#include "GreenMonster.h"

// Qweak subsystems
// (for correct dependency generation)
#include "QwHelicity.h"
#include "QwFakeHelicity.h"
#include "QwBeamLine.h"
#include "QwMainCerenkovDetector.h"
#include "QwScanner.h"
#include "QwLumi.h"
#include "QwBeamMod.h"
#include "QwVQWK_Channel.h"




Int_t main(Int_t argc, Char_t* argv[])
{
  ///  First, fill the search paths for the parameter files; this sets a
  ///  static variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  //QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Extensions/QwFeedback/prminput");


  // Then set the command line arguments and the configuration filename, 
  // and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.AddConfigFile("qwparity.conf");
  gQwOptions.AddConfigFile("qweak_mysql.conf");
    // Define the command line options
  DefineOptionsParity(gQwOptions);
  //Load command line options for the histogram/tree helper class
  gQwHists.ProcessOptions(gQwOptions);
   /// Setup screen and file logging
  gQwLog.ProcessOptions(&gQwOptions);

  ///  Load the histogram parameter definitions (from parity_hists.txt) into the global
  ///  histogram helper: QwHistogramHelper
  //gQwHists.LoadHistParamsFromFile("qweak_parity_hists.in");
  //gQwHists.LoadTreeParamsFromFile("Qweak_Tree_Trim_List.in");


  ///  Create the event buffer
  QwEventBuffer eventbuffer;
  eventbuffer.ProcessOptions(gQwOptions);



  ///  Create the database connection
  QwDatabase database(gQwOptions);


  ///  Start loop over all runs
  QwRootFile* rootfile = 0;

  
  /*
  ///  Create an EPICS control event
  QwEPICSControl fEPICSCtrl;
  GreenMonster   fScanCtrl;
  //  Let's clear the values for the scan data & set "not clean".
  fScanCtrl.Open();
  fScanCtrl.SCNSetStatus(SCN_INT_NOT);
  fScanCtrl.SCNSetValue(1,0);
  fScanCtrl.SCNSetValue(2,0);
  fScanCtrl.CheckScan();
  fScanCtrl.PrintScanInfo();
  fScanCtrl.Close();
  
  fEPICSCtrl.Print_Qasym_Ctrls();
  */

  
  // Loop over all runs
  while (eventbuffer.OpenNextStream() == CODA_OK){
    //  Begin processing for the first run.
 
    ///  Set the current event number for parameter file lookup
    QwParameterFile::SetCurrentRunNumber(eventbuffer.GetRunNumber());


    ///  Create an EPICS event
    QwEPICSEvent epicsevent;
    epicsevent.LoadEpicsVariableMap("EpicsTable.map");

    ///  Load the detectors from file
    QwSubsystemArrayParity detectors(gQwOptions);
    detectors.ProcessOptions(gQwOptions);

    ///  Create the helicity pattern
    QwHelicityCorrelatedFeedback helicitypattern(detectors);
    helicitypattern.ProcessOptions(gQwOptions);
    helicitypattern.LoadParameterFile("qweak_fb_prm.in");
    /*
      helicitypattern.UpdateGMClean(0);
      helicitypattern.FeedIASetPoint(0);
      helicitypattern.UpdateGMClean(1);
    */

    ///  Create the event ring
    QwEventRing eventring;
    eventring.ProcessOptions(gQwOptions);

    ///  Set up the ring with the subsysten array
    eventring.SetupRing(detectors);

    ///  Create the running sum
    QwSubsystemArrayParity runningsum(detectors);


    //  Initialize the database connection.
    database.SetupOneRun(eventbuffer);
    
    //  Open the ROOT file
    rootfile = new QwRootFile(eventbuffer.GetRunLabel());
    if (! rootfile) QwError << "QwAnalysis made a boo boo!" << QwLog::endl;

    //  Construct histograms
    rootfile->ConstructHistograms("mps_histo", detectors);
    rootfile->ConstructHistograms("hel_histo", helicitypattern);

    //  Construct tree branches
    rootfile->ConstructTreeBranches("Mps_Tree", "MPS event data tree", detectors);
    rootfile->ConstructTreeBranches("Hel_Tree", "Helicity event data tree", helicitypattern);

    // Summarize the ROOT file structure
    rootfile->PrintTrees();
    rootfile->PrintDirs();

    Int_t failed_events_counts = 0; // count failed total events
    // TODO (wdc) failed event counter in QwEventRing?


    //  Clear the single-event running sum at the beginning of the runlet
    runningsum.ClearEventData();
    helicitypattern.ClearRunningSum();
    //  Clear the running sum of the burst values at the beginning of the runlet
    helicitypattern.ClearBurstSum();



   

    // Loop over events in this CODA file
    while (eventbuffer.GetNextEvent() == CODA_OK) {
      //  First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()){
	//  Send ROC configuration event data to the subsystem objects.
	eventbuffer.FillSubsystemConfigurationData(detectors);
      }

       //  Secondly, process EPICS events
      if (eventbuffer.IsEPICSEvent()) {
        eventbuffer.FillEPICSData(epicsevent);
        epicsevent.CalculateRunningValues();
        helicitypattern.UpdateBlinder(epicsevent);
      }

      //  Dump out of the loop when we see the end event.
      if (eventbuffer.GetEndEventCount()>0){
	QwMessage << "Number of events processed at end of run: " << eventbuffer.GetEventNumber() << std::endl;
	break;
      }

      //  Now, if this is not a physics event, go back and get a new event.
      if (!eventbuffer.IsPhysicsEvent()) continue;


      //  Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(detectors);

      //  Process the subsystem data
      detectors.ProcessEvent();

       // The event pass the event cut constraints
      if (detectors.ApplySingleEventCuts()) {

        // Accumulate the running sum to calculate the event based running average
        runningsum.AccumulateRunningSum(detectors);

        // Fill the histograms
        rootfile->FillHistograms(detectors);

        // Fill the tree branches
        rootfile->FillTreeBranches(detectors);
        rootfile->FillTree("Mps_Tree");

        // Add event to the ring
        eventring.push(detectors);

        // Check to see ring is ready
        if (eventring.IsReady()) {

          // Load the event into the helicity pattern
          helicitypattern.LoadEventData(eventring.pop());

          // Calculate helicity pattern asymmetry
          if (helicitypattern.IsCompletePattern()) { 

            // Update the blinder if conditions have changed
            helicitypattern.UpdateBlinder(detectors);

            // Calculate the asymmetry
            helicitypattern.CalculateAsymmetry();
            if (helicitypattern.IsGoodAsymmetry()) {
	      helicitypattern.ApplyFeedbackCorrections();//apply IA feedback
              // Fill histograms
              rootfile->FillHistograms(helicitypattern);
              // Fill tree branches
              rootfile->FillTreeBranches(helicitypattern);
              rootfile->FillTree("Hel_Tree");
              // Clear the data
              helicitypattern.ClearEventData();	      
            }

          } // helicitypattern.IsCompletePattern()

        } // eventring.IsReady()


      // Failed single event cuts
      } else {
        eventring.FailedEvent(detectors.GetEventcutErrorFlag()); //event cut failed update the ring status
	//	QwMessage << "FailedEven: "<< eventbuffer.GetEventNumber() << std::endl;

        failed_events_counts++;
      }

      // Burst mode
      if (eventbuffer.IsEndOfBurst()) {
        helicitypattern.AccumulateRunningBurstSum();
        helicitypattern.CalculateBurstAverage();
        helicitypattern.ClearBurstSum();
      } 

      

    }   // end of loop over events  

    QwMessage << "Number of events processed at end of run: "
              << eventbuffer.GetEventNumber() << std::endl;


    // Calculate running averages over helicity patterns
    /*
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
    */
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
    database.SetupOneRun(eventbuffer);

    // Each sussystem has its own Connect() and Disconnect() functions.
    if (database.AllowsWriteAccess()) {
      helicitypattern.FillDB(&database);
      epicsevent.FillDB(&database);
    }


    QwMessage << "Total events failed " << failed_events_counts << QwLog::endl;

    //  Report run summary
    eventbuffer.ReportRunSummary();
    eventbuffer.PrintRunTimes();

  } //end of run loop


  QwMessage << "I have done everything I can do..." << QwLog::endl; 
 
  return 0;
}
