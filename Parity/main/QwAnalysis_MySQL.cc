
#include "TApplication.h"
#include "boost/shared_ptr.hpp"

#include "QwLog.h"
#include "QwOptions.h"
#include "QwOptionsParity.h"

#include "QwAnalysis_MySQL.h"

Bool_t kInQwBatchMode = kFALSE;
//Bool_t bRING_READY;


Int_t
main(Int_t argc, Char_t* argv[])
{

  Bool_t bDebug    = kFALSE;
  Bool_t bHelicity = kTRUE;
  Bool_t bTree     = kTRUE;
  Bool_t bHisto    = kTRUE;

  //either the DISPLAY not set, or JOB_ID defined, we take it as in batch mode
  if ( getenv("DISPLAY")==NULL || getenv("JOB_ID")!=NULL ) kInQwBatchMode = kTRUE;

  gROOT->SetBatch(kTRUE);

  gQwHists.LoadHistParamsFromFile(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/parity_hists.in");

  TStopwatch timer;

  QwCommandLine cmdline;
  cmdline.Parse(argc, argv);

  // Setup screen and file logging
//   gQwLog.InitLogFile("qwanalysis_mysql.log");
//   gQwLog.SetFileThreshold(QwLog::kDebug);
//   gQwLog.SetScreenThreshold(QwLog::kDebug);

  // Set up command line and file options processing
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("Parity/prminput/qweak_mysql.conf");
//  gQwOptions.SetConfigFile("qweak_mysql.conf");
  DefineOptionsParity(gQwOptions);

  // Start testing
  QwDebug << "qwdb_test:  Hello there!" << QwLog::endl;

  //  gQwDatabase.Connect();
  //  QwMessage << "Database server version is " << gQwDatabase.GetServerVersion() << QwLog::endl;

  //  gQwDatabase.PrintServerInfo();


  QwEventBuffer QwEvt;

  QwSubsystemArrayParity QwDetectors;
  VQwSubsystemParity * subsystem_tmp;//VQwSubsystemParity is the top most parent class for Parity subsystems.

  subsystem_tmp = NULL;

  QwDetectors.push_back(new QwBeamLine("Injector BeamLine"));
  QwDetectors.GetSubsystem("Injector BeamLine")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_beamline.map");
  QwDetectors.GetSubsystem("Injector BeamLine")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_pedestal.map");
  QwDetectors.GetSubsystem("Injector BeamLine")->LoadEventCuts(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_beamline_eventcuts.in");//Pass the correct cuts file.
  QwDetectors.push_back(new QwHelicity("Helicity info"));
  QwDetectors.GetSubsystem("Helicity info")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_helicity.map");
  QwDetectors.GetSubsystem("Helicity info")->LoadInputParameters("");

  QwDetectors.push_back(new QwLumi("Luminosity Monitors"));
  QwDetectors.GetSubsystem("Luminosity Monitors")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_beamline.map");//current map file is for the beamline.
  QwDetectors.GetSubsystem("Luminosity Monitors")->LoadEventCuts(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_lumi_eventcuts.in");//Pass the correct cuts file.


  QwHelicityPattern QwHelPat  (QwDetectors);
  QwEventRing       fEventRing(QwDetectors,8,8,8);
  //Event ring of 8; 8 hold off events, 8 minimum failed events is a beam trip

  Double_t evnum=0.0;

  QwDatabase *qw_test_DB = NULL; 
  UInt_t run_id      = 0;
  UInt_t analysis_id = 0;

  for(Int_t run = cmdline.GetFirstRun(); run <= cmdline.GetLastRun(); run++)
    {
      
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
      //  OpenAllFiles(io, run);


      //      TString rootfilename = Form("%s/Qweak_BeamLine_%d.root", getenv("QW_ROOTFILES_DIR"), run);

      TFile rootfile(Form("%s/Qweak_MySQL_%d.root", getenv("QW_ROOTFILES_DIR"), run),
		     "RECREATE","Qweak MySQL ROOT file");

      if(bHisto)
	{
	  rootfile.cd();
	  QwDetectors.ConstructHistograms(rootfile.mkdir("mps_histo"));

	  if(bHelicity)
	    {
	      rootfile.cd();
	      QwHelPat.ConstructHistograms(rootfile.mkdir("hel_histo"));
	    }
	}


      TTree *mpstree = NULL;
      TTree *heltree = NULL;

      std::vector <Double_t> mpsvector;
      std::vector <Double_t> helvector;

      if(bTree)
	{
	  rootfile.cd();
	  mpstree = new TTree("MPS_Tree","MPS event data tree");
	  mpsvector.reserve(6000);
	  // if one defines more than 600 words in the full ntuple
	  // results are going to get very very crazy.
	  mpstree->Branch("evnum",&evnum,"evnum/D");

	  TString dummystr="";

	  //	  ( (QwBeamLine*)QwDetectors.GetSubsystem("Injector BeamLine")   )->ConstructBranchAndVector(mpstree, dummystr, mpsvector);
	  //	  ( (QwBeamLine*)QwDetectors.GetSubsystem("Helicity info")       )->ConstructBranchAndVector(mpstree, dummystr, mpsvector);
	  //	  ( (QwBeamLine*)QwDetectors.GetSubsystem("Luminosity Monitors") )->ConstructBranchAndVector(mpstree, dummystr, mpsvector);
	  QwDetectors.ConstructBranchAndVector(mpstree, dummystr, mpsvector);
	  // at some point we want to have some thing like that but it need to be implement in QwSubsystemArray

	  //        rootfile.cd();

	  if(bHelicity)
	    {
	      rootfile.cd();
	      heltree = new TTree("HEL_Tree","Helicity event data tree");
	      helvector.reserve(6000);
	      //          TString dummystr="";
	      QwHelPat.ConstructBranchAndVector(heltree, dummystr, helvector);
	    }
	}

      Int_t falied_events_counts = 0;//count falied total events
      Int_t event_number = 0;

      while (QwEvt.GetEvent() == CODA_OK)
	{
	  event_number = QwEvt.GetEventNumber();


	  if      ( QwEvt.IsROCConfigurationEvent() ) QwEvt.FillSubsystemConfigurationData(QwDetectors);
	  if      ( !QwEvt.IsPhysicsEvent()         ) continue;

	  //  Check to see if we want to process this event.
	  if      ( event_number < cmdline.GetFirstEvent() ) continue;
	  else if ( event_number > cmdline.GetLastEvent()  ) break;
	  else
	    {
	      if(bDebug)
		{
		  printf("==================================================== \n");
		  printf(" new event:: number = %d\n", event_number);
		  printf("==================================================== \n");
		}

	      QwEvt.FillSubsystemData(QwDetectors);
	      QwDetectors.ProcessEvent();

	      //currently QwHelicity::ApplySingleEventCuts() will check for actual helicity bit for 1 or 0 and falied the test if it is different
	      if( QwDetectors.ApplySingleEventCuts() )
		{
		  //The event pass the event cut constraints
		  //QwDetectors.Do_RunningSum();//accimulate the running sum to calculate the event base running AVG
		  if(bHelicity)
		    {
		      fEventRing.push(QwDetectors); //add event to the ring
		      if( fEventRing.IsReady() ) QwHelPat.LoadEventData(fEventRing.pop());
		    }

		  if(bHisto) QwDetectors.FillHistograms();

		  if(bTree)
		    {
		      evnum = (Double_t) event_number;
		      ( (QwBeamLine*)QwDetectors.GetSubsystem("Injector BeamLine")  )->FillTreeVector(mpsvector);
		      ( (QwHelicity*)QwDetectors.GetSubsystem("Helicity info")      )->FillTreeVector(mpsvector);
		      ( (QwLumi*)    QwDetectors.GetSubsystem("Luminosity Monitors"))->FillTreeVector(mpsvector);
		      mpstree->Fill();
		    }

		  if(bHelicity&&QwHelPat.IsCompletePattern())
		    {
		      //std::cout<<" Complete quartet  "<<QwEvt.GetEventNumber()<<std::endl;
		      QwHelPat.CalculateAsymmetry();
		      //	      QwHelPat.Print();
		      if(bHisto) QwHelPat.FillHistograms();

		      if(bTree){
			QwHelPat.FillTreeVector(helvector);
			heltree->Fill();
		      }
		      QwHelPat.ClearEventData();
		    }
		}
	      else
		{
		  printf(" Falied event %d\n", event_number);
		  fEventRing.FailedEvent(QwDetectors.GetEventcutErrorFlag()); //event cut failed update the ring status
		  falied_events_counts++;
		}
	      if(event_number%1000==0){
		printf("\tNumber of events processed so far: %12d\n", event_number);
	      }
	    }


	}




  
      QwHelPat.CalculateRunningAverage();//this will calculate running averages for Asymmetries and Yields per quartet
      timer.Stop();

      /*  Write to the root file, being sure to delete the old cycles  *
       *  which were written by Autosave.                              *
       *  Doing this will remove the multiple copies of the ntuples    *
       *  from the root file.                                          *
       *                                                               *
       *  Then, we need to delete the histograms here.                 *
       *  If we wait until the subsystem destructors, we get a         *
       *  segfault; but in additiona to that we should delete them     *
       *  here, in case we run over multiple runs at a time.           */
      if(bHisto||bTree) rootfile.Write(0,TObject::kOverwrite);
      if(bHisto)
	{
	  printf("QwDetectors.DeleteHistograms\n"); QwDetectors.DeleteHistograms();
	  printf("QwHelPat.DeleteHistograms\n\n");  QwHelPat.DeleteHistograms();
	}
      
      QwEvt.CloseDataFile();
      QwEvt.ReportRunSummary();
      

     
      qw_test_DB  = new QwDatabase();
      // GetRunID() and GetAnalysisID have their own Connect() and Disconnect() functions.
      run_id      = qw_test_DB->GetRunID(QwEvt);
      analysis_id = qw_test_DB->GetAnalysisID(QwEvt);

      printf("main:: Run # %d Run ID %d and Analysis ID %d\n", run, run_id, analysis_id);

      QwHelPat.FillDB(qw_test_DB);
    
      delete qw_test_DB; qw_test_DB = NULL;
    } //end of run loop
  

  return 0;
}




void PrintInfo(TStopwatch& timer)
{
  std::cout << "CPU time used:  "  << timer.CpuTime() << " s"
	    << std::endl
	    << "Real time used: " << timer.RealTime() << " s"
	    << std::endl << std::endl;
  return;
}
