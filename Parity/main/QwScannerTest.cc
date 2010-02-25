/**********************************************************\
* File: QwScannerTest.cc                                   *
*                                                          *
* Author: J. Pan                                           *
* jpan@jlab.org                                            *
*                                                          *
* Fri Aug 14 00:52:07 CDT 2009                             *
\**********************************************************/

/*------------------------------------------------------------------------*//*!

 \file QwScannerTest.cc

 \brief test program for the focal plane scanner

*//*-------------------------------------------------------------------------*/

#include "QwAnalysis_ADC.h"
#include "QwHelicityPattern.h"
#include "TApplication.h"
#include <boost/shared_ptr.hpp>


// C and C++ headers
#include <iostream>

// Boost math library for random number generation
#include <boost/random.hpp>

// Coda headers
#include "THaCodaFile.h"

// Qweak headers
#include "QwVQWK_Channel.h"
#include "QwSIS3801_Channel.h"
#include "QwEventBuffer.h"
#include "QwSubsystemArrayParity.h"
#include "QwSubsystemArrayTracking.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"

Bool_t kInQwBatchMode = kFALSE;

int main(Int_t argc,Char_t* argv[])
{
  // Debug level
  static int kDebug = 1;

  /// First, we set the command line arguments and the configuration filename,
  /// and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwanalysis_beamline.conf");
  // Define the command line options
  DefineOptionsParity(gQwOptions);


  //either the DISPLAY not set, or JOB_ID defined, we take it as in batch mode
  if (getenv("DISPLAY")==NULL
      ||getenv("JOB_ID")!=NULL) kInQwBatchMode = kTRUE;
  gROOT->SetBatch(kTRUE);

  ///  Fill the search paths for the parameter files; this sets a static
  ///  variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH"))+"/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Analysis/prminput");

  ///
  ///  Load the histogram parameter definitions (from parity_hists.txt) into the global
  ///  histogram helper: QwHistogramHelper
  ///
  if (kDebug) std::cout<<"Loading histogram parameter definition file: "<<std::endl;
  gQwHists.LoadHistParamsFromFile("scanner_hists.in");

  TStopwatch timer;

  ///
  /// Instantiate event buffer
  QwEventBuffer QwEvt;

  ///
  /// Instantiate one subsytem for all eight main detectors plus one fully assembled
  /// background detector plus two channels (there are two channels for each fully
  /// assembled detector) for noise setup.

//    QwSubsystemArrayParity QwDetectors;
  QwSubsystemArray QwDetectors;
//  QwDetectors.push_back(new QwMainCerenkovDetector("MainDetectors"));
//  QwDetectors.GetSubsystem("MainDetectors")->LoadChannelMap("qweak_adc.map");

  // Test code for the focal plane scanner
  if (kDebug) std::cout<<"Instantiate the scanner subsystem:"<<std::endl;
  QwDetectors.push_back (new QwScanner( "FPS" ));
  QwDetectors.GetSubsystem("FPS")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Analysis/prminput/scanner_channel.map" );
  QwDetectors.GetSubsystem("FPS")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Analysis/prminput/scanner_pedestal.map");


  ///
  ///Specifies the same helicity pattern used by all subsystems
  ///to calculate asymmetries. The pattern is defined in the
  ///QwHelicityPattern class.
  //QwHelicityPattern QwHelPat(QwDetectors);


  //////////////////////////////////////////////////////////
  //  Generate mock data for scanner
  //////////////////////////////////////////////////////////
  if (kDebug) std::cout<<"\nGenerating mock data:\n\n";

  // Get subsystem
  QwScanner* scanner = dynamic_cast<QwScanner*> (QwDetectors.GetSubsystem("FPS"));

  // Loop over all runs
  UInt_t runnumber_min = (UInt_t) gQwOptions.GetIntValuePairFirst("run");
  UInt_t runnumber_max = (UInt_t) gQwOptions.GetIntValuePairLast("run");
  for (UInt_t run  = runnumber_min;
              run <= runnumber_max;
              run++) {

    TString filename = std::string(getenv("QWSCRATCH"))+"/data" +
                       TString("/QwRun_") + Form("%ld.",run) + TString("log");

    if (kDebug) std::cout<<"Generating mock data for the scanner: "<<filename<<std::endl;

    if (QwEvt.OpenDataFile(filename,"W") != CODA_OK) {
      std::cout << "Error: could not open file!" << std::endl;
      return 0;
    }
    QwEvt.ResetControlParameters();
    QwEvt.EncodePrestartEvent(run, 0); // prestart: runnumber, runtype
    QwEvt.EncodeGoEvent();

    // Event generation loop
    Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
    Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");
    for (Int_t event = eventnumber_min; event <= eventnumber_max; event++) {

      QwDetectors.ClearEventData();
      QwDetectors.RandomizeEventData();

      // Write this event to file
      QwEvt.EncodeSubsystemData(QwDetectors);

      // Periodically print event number
      if (kDebug && event % 1000 == 0 && event !=0)
        std::cout << "Generated " << event << " events so far." << std::endl;

    }  //end of event loop

    QwEvt.EncodeEndEvent();
    QwEvt.CloseDataFile();
    QwEvt.ReportRunSummary();

  } //end of run loop



  //////////////////////////////////////////////////////
  //  Processing scanner mock data
  //////////////////////////////////////////////////////
  if (kDebug)  std::cout<<"\nProcessing mock data:\n\n";

  // Loop over all runs
  for (UInt_t run  = runnumber_min;
              run <= runnumber_max;
              run++) {

      //  Begin processing for the first run.
      //  Start the timer.
      timer.Start();

      //  Try to open the data file.
      if (QwEvt.OpenDataFile(run) != CODA_OK)
	{
	  //  The data file can't be opened.
	  //  Get ready to process the next run.
	  std::cerr << "ERROR:  Unable to find data files for run "
		    << run << ".  Moving to the next run.\n"
		    << std::endl;
	  timer.Stop();
	  continue;
	}
      QwEvt.ResetControlParameters();
      //  Open the data files and root file
      //    OpenAllFiles(io, run);

      TString rootfilename=std::string(getenv("QW_ROOTFILES_DIR"))+Form("/Qweak_Scanner_%d.root",run);
      std::cout<<" rootfilename="<<rootfilename<<"\n";
      TFile rootfile(rootfilename,"RECREATE","QWeak ROOT file");

    if (kDebug) std::cout<<" ====>>>> Creating histograms:"<<std::endl;
    rootfile.cd();
    TString prefix = TString("scanner_");
    scanner->ConstructHistograms(rootfile.mkdir("scanner_histo"),prefix);
    scanner->ConstructTrees(&rootfile);

    int EvtCounter = 0;
    Int_t eventnumber = -1;
    Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
    Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");
    while (QwEvt.GetEvent() == CODA_OK){

       EvtCounter++;
      //  Loop over events in this CODA file
      //  First, do processing of non-physics events...
      if (QwEvt.IsROCConfigurationEvent()){
	//  Send ROC configuration event data to the subsystem objects.
        if (kDebug) std::cout<<"ROC Configuration Event." <<std::endl;
	QwEvt.FillSubsystemConfigurationData(QwDetectors);
      }

      //  Now, if this is not a physics event, go back and get
      //  a new event.
      if (! QwEvt.IsPhysicsEvent()) {
          std::cout<<"Event "<<EvtCounter<<" : not a physics event."<<std::endl;
          continue;
          }

      //  Check to see if we want to process this event.
      eventnumber = QwEvt.GetEventNumber();
      if      (eventnumber < eventnumber_min) continue;
      else if (eventnumber > eventnumber_max) break;


      if (QwEvt.GetEventNumber()%100==0 | (QwEvt.GetEventNumber()<=10) ) {
        std::cout << "Number of events processed so far: "
                  << QwEvt.GetEventNumber() << "\n";
      }

      //  Fill the subsystem objects with their respective data for this event.
      QwEvt.FillSubsystemData(QwDetectors);

      // Process this events
      QwDetectors.ProcessEvent();

      QwDetectors.FillHistograms();

      scanner->FillTrees();

    }
    std::cout << "Number of events processed: "
	      << QwEvt.GetEventNumber() << std::endl;
    timer.Stop();

    rootfile.Write(0,TObject::kOverwrite);

    QwDetectors.DeleteHistograms();

    QwEvt.CloseDataFile();
    QwEvt.ReportRunSummary();
    PrintInfo(timer, run);


  } //end of run loop


  std::cout<<"Done!"<<std::endl;
  return 0;
}


void PrintInfo(TStopwatch& timer, Int_t run)
{
  std::cout << "Analysis of run "  << run << std::endl
            << "CPU time used:  "  << timer.CpuTime() << " s"
            << std::endl
            << "Real time used: " << timer.RealTime() << " s"
            << std::endl << std::endl;
  return;
}
