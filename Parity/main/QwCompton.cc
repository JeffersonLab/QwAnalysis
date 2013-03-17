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


// Qweak headers
#include "QwLog.h"
#include "QwRootFile.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwEPICSEvent.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwHistogramHelper.h"
#include "QwSubsystemArrayParity.h"

// Compton headers
#include "QwScaler.h"
#include "QwBeamLine.h"
#include "QwComptonPhotonDetector.h"
#include "QwComptonElectronDetector.h"



int main(int argc, char* argv[])
{
  /// Change some default settings for use by the Compton analyzer
  QwEventBuffer::SetDefaultDataFileStem("Compton_");
  QwRootFile::SetDefaultRootFileStem("Compton_");


  /// Fill the search paths for the parameter files; this sets a static
  /// variable within the QwParameterFile class which will be used by
  /// all instances.  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Analysis/prminput");


  /// Set the command line arguments and the configuration filename
  gQwOptions.SetCommandLine(argc, argv);
  /// Define the command line options
  DefineOptionsParity(gQwOptions);

  /// Message logging facilities
  gQwLog.ProcessOptions(&gQwOptions);


  // Load histogram definitions
  gQwHists.LoadHistParamsFromFile("compton_hists.in");


  ///  Create the event buffer
  QwEventBuffer eventbuffer;
  eventbuffer.ProcessOptions(gQwOptions);

  ///  Start loop over all runs
  while (eventbuffer.OpenNextStream() == CODA_OK) {

    ///  Begin processing for the first run


    ///  Set the current event number for parameter file lookup
    QwParameterFile::SetCurrentRunNumber(eventbuffer.GetRunNumber());


    ///  Create an EPICS event
    QwEPICSEvent epicsevent;
    epicsevent.ProcessOptions(gQwOptions);
    epicsevent.LoadChannelMap("compton_epics_table.map");


    ///  Load the detectors from file
    QwSubsystemArrayParity detectors(gQwOptions);
    detectors.ProcessOptions(gQwOptions);

    ///  Create the helicity pattern
    QwHelicityPattern helicitypattern(detectors);
    helicitypattern.ProcessOptions(gQwOptions);

    // Create a configuration options output
    QwSubsystemArrayParity configoutput(detectors);


    //  Open the ROOT file
    QwRootFile* rootfile = new QwRootFile(eventbuffer.GetRunLabel());
    if (! rootfile) QwError << "QwAnalysis made a boo boo!" << QwLog::endl;
    rootfile->WriteParamFileList("mapfiles", detectors);
    //  Construct histograms
    rootfile->ConstructHistograms("mps_histo", detectors);
    rootfile->ConstructHistograms("hel_histo", helicitypattern);

    //  Construct tree branches
    rootfile->ConstructTreeBranches("Mps_Tree", "MPS event data tree", detectors);
    rootfile->ConstructTreeBranches("Hel_Tree", "Helicity event data tree", helicitypattern);
    rootfile->ConstructTreeBranches("Burst_Tree", "Burst level data tree", helicitypattern.GetBurstYield(),"yield_");
    rootfile->ConstructTreeBranches("Burst_Tree", "Burst level data tree", helicitypattern.GetBurstAsymmetry(),"asym_");
    rootfile->ConstructTreeBranches("Burst_Tree", "Burst level data tree", helicitypattern.GetBurstDifference(),"diff_");
    rootfile->ConstructTreeBranches("Slow_Tree", "EPICS and slow control tree", epicsevent);
    rootfile->ConstructTreeBranches("Config_Tree","Configuration Tree",configoutput,"conf");
    // Construct indices to get from one tree to the other
    rootfile->ConstructIndices("Mps_Tree","Slow_Tree");
    rootfile->ConstructIndices("Hel_Tree","Slow_Tree");
    rootfile->ConstructIndices("Mps_Tree","Config_Tree");
    rootfile->ConstructIndices("Hel_Tree","Config_Tree");

    // Summarize the ROOT file structure
    rootfile->PrintTrees();
    rootfile->PrintDirs();


    //  Clear the running sums
    helicitypattern.ClearRunningSum();
    helicitypattern.ClearBurstSum();


    ///  Start loop over events
    while (eventbuffer.GetNextEvent() == CODA_OK) {

      // First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()) {
        // Send ROC configuration event data to the subsystem objects.
        eventbuffer.FillSubsystemConfigurationData(configoutput);
      }

      //  Secondly, process EPICS events
      if (eventbuffer.IsEPICSEvent()) {
        eventbuffer.FillEPICSData(epicsevent);
        if (epicsevent.HasDataLoaded()) {
          epicsevent.CalculateRunningValues();

          rootfile->FillTreeBranches(epicsevent);
          rootfile->FillTree("Slow_Tree");
        }
      }

      // Now, if this is not a physics event, go back and get a new event.
      if (! eventbuffer.IsPhysicsEvent()) continue;



      // Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(detectors);

      // Process this events
      detectors.ProcessEvent();

      // The event pass the event cut constraints
      if (detectors.ApplySingleEventCuts()) {

        // Fill the histograms
        rootfile->FillHistograms(detectors);

        // Fill the tree
        rootfile->FillTreeBranches(detectors);
        rootfile->FillTree("Mps_Tree");


        //  Somehow test to see if the laser state has changed.  If it has,
        //  then calculate and print the running sums so far.
        //  Maybe instead of here, it should be done as soon as we've picked up an
        //  event, instead of waiting to build a complete pattern.

        if (detectors.CheckForEndOfBurst()){
          //  Do the burst versions of
          helicitypattern.CalculateRunningAverage();
          // Maybe we need to clear the burst sums also
        }


        // Helicity pattern
        helicitypattern.LoadEventData(detectors);


        // TODO We need another check here to test for pattern validity.  Right
        // now the first 24 cycles are also added to the histograms.
        if (helicitypattern.IsCompletePattern()) {

          // Calculate the asymmetry
          helicitypattern.CalculateAsymmetry();
          if (helicitypattern.IsGoodAsymmetry()) {

            // Fill histograms
            rootfile->FillHistograms(helicitypattern);

            // Fill tree branches
            rootfile->FillTreeBranches(helicitypattern);
            rootfile->FillTree("Hel_Tree");

            // Clear the data
            helicitypattern.ClearEventData();
          }


          //  Somehow test to see if the laser state has changed.  If it has,
          //  then calculate and print the running sums so far.
          //  Maybe instead of here, it should be done as soon as we've picked up an
          //  event, instead of waiting to build a complete pattern.

        }

      } // detectors.ApplySingleEventCuts()


      // Burst mode
      if (helicitypattern.IsEndOfBurst()) {
        helicitypattern.CalculateBurstAverage();
        helicitypattern.AccumulateRunningBurstSum();

        // Fill tree branches
        rootfile->FillTreeBranches(helicitypattern.GetBurstYield());
        rootfile->FillTreeBranches(helicitypattern.GetBurstAsymmetry());
        rootfile->FillTreeBranches(helicitypattern.GetBurstDifference());
        rootfile->FillTree("Burst_Tree");

        // Clear the data
        helicitypattern.ClearBurstSum();
      }

    } // end of loop over events
    rootfile->FillTreeBranches(configoutput);
    rootfile->FillTree("Config_Tree");

    QwMessage << "Last event processed: "
              << eventbuffer.GetEventNumber() << QwLog::endl;

    // Calculate running averages over helicity patterns
    if (helicitypattern.IsRunningSumEnabled()) {
      helicitypattern.CalculateRunningAverage();
      if (helicitypattern.IsBurstSumEnabled()) {
        helicitypattern.CalculateRunningBurstAverage();
      }
    }


    /*  Write to the root file, being sure to delete the old cycles  *
     *  which were written by Autosave.                              *
     *  Doing this will remove the multiple copies of the ntuples    *
     *  from the root file.                                          *
     *                                                               *
     *  Then, we need to delete the histograms here.                 *
     *  If we wait until the subsystem destructors, we get a         *
     *  segfault; but in additional to that we should delete them     *
     *  here, in case we run over multiple runs at a time.           */
    rootfile->Write(0,TObject::kOverwrite);

    // Close ROOT file
    rootfile->Close();
    delete rootfile; rootfile = 0;

    // Close data file and print run summary
    eventbuffer.CloseStream();

    //  Report run summary
    eventbuffer.ReportRunSummary();
    eventbuffer.PrintRunTimes();

  } // end of loop over runs

  QwMessage << "I have done everything I can do..." << QwLog::endl;

  return 0;
}
