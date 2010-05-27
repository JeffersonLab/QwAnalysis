
#include "TApplication.h"
#include "boost/shared_ptr.hpp"

#include "QwLog.h"
#include "QwOptions.h"
#include "QwOptionsParity.h"

#include "QwAnalysis_MySQL.h"

#include "QwEPICSEvent.h"
#include "QwEventBuffer.h"

Bool_t kInQwBatchMode = kFALSE;
Bool_t bRING_READY;
Bool_t bSkip= kFALSE;;
Bool_t bSave= kTRUE;
Int_t fEVENTS2SKIP, fEVENTS2SAVE;

Int_t
main(Int_t argc, Char_t* argv[])
{

 // First, we set the command line arguments and the configuration filename,
  // and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwanalysis_beamline.conf");
  gQwOptions.SetConfigFile("qweak_mysql.conf");
  DefineOptionsParity(gQwOptions);

  // modified value for maximum size of tree
  Long64_t kMAXTREESIZE = 10000000000LL;
  // standard value for maximum size of tree in root source
  //  Long64_t kMAXTREESIZE = 1900000000LL;

  Bool_t bDebug    = kFALSE;
  Bool_t bHelicity = kTRUE;
  Bool_t bTree     = kFALSE;
  Bool_t bHisto    = kFALSE;

  //either the DISPLAY not set, or JOB_ID defined, we take it as in batch mode
  if ( getenv("DISPLAY")==NULL || getenv("JOB_ID")!=NULL ) kInQwBatchMode = kTRUE;

  gROOT->SetBatch(kTRUE);



  ///  Fill the search paths for the parameter files; this sets a static
  ///  variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(std::string(getenv("QW_PRMINPUT")));
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Parity/prminput");


  ///
  ///  Load the histogram parameter definitions (from parity_hists.txt) into the global
  ///  histogram helper: QwHistogramHelper
  ///
  gQwHists.LoadHistParamsFromFile("parity_hists.in");
  // Setup screen and file logging
  gQwLog.InitLogFile("qwanalysis_mysql.log");
  gQwLog.SetFileThreshold(QwLog::kDebug);
  gQwLog.SetScreenThreshold(QwLog::kMessage);

  TStopwatch timer;

  QwEventBuffer QwEvt;
  QwEvt.ProcessOptions(gQwOptions);

  QwEPICSEvent epics_data;

  QwSubsystemArrayParity QwDetectors;
  VQwSubsystemParity * subsystem_tmp;//VQwSubsystemParity is the top most parent class for Parity subsystems.

  subsystem_tmp = NULL;


  QwDetectors.push_back(new QwMainCerenkovDetector("MainDetectors"));
  QwDetectors.GetSubsystem("MainDetectors")->LoadChannelMap("qweak_adc.map");

  QwDetectors.push_back(new QwBeamLine("Injector BeamLine"));
  QwDetectors.GetSubsystem("Injector BeamLine")->LoadChannelMap("qweak_beamline.map");
  QwDetectors.GetSubsystem("Injector BeamLine")->LoadInputParameters("qweak_pedestal.map");
  QwDetectors.GetSubsystem("Injector BeamLine")->LoadEventCuts("qweak_beamline_eventcuts.in");//Pass the correct cuts file.

  QwDetectors.push_back(new QwHelicity("Helicity info"));
  QwDetectors.GetSubsystem("Helicity info")->LoadChannelMap("qweak_helicity.map");
  QwDetectors.GetSubsystem("Helicity info")->LoadInputParameters("");

  QwDetectors.push_back(new QwLumi("Luminosity Monitors"));
  QwDetectors.GetSubsystem("Luminosity Monitors")->LoadChannelMap("qweak_lumi.map");
  QwDetectors.GetSubsystem("Luminosity Monitors")->LoadEventCuts("qweak_lumi_eventcuts.in");//Pass the correct cuts file.

  QwSubsystemArrayParity runningsum;
  runningsum.Copy(&QwDetectors);

  ((QwBeamLine*)QwDetectors.GetSubsystem("Injector BeamLine"))->LoadGeometry("qweak_beamline_geometry.map"); //read in the gemoetry of the beamline



  QwHelicityPattern QwHelPat(QwDetectors);//multiplet size is set within the QwHelicityPattern class

  //Reads Event Ring parameters from cmd
  Int_t r_size=32,r_BT=4,r_HLD=16;
  if (gQwOptions.HasValue("ring.size"))
    r_size=gQwOptions.GetValue<int>("ring.size");
  if (gQwOptions.HasValue("ring.bt"))
    r_BT=gQwOptions.GetValue<int>("ring.bt");
  if (gQwOptions.HasValue("ring.hld"))
    r_HLD=gQwOptions.GetValue<int>("ring.hld");

  //Tree events scaling parameters
   if (gQwOptions.HasValue("skip"))
    fEVENTS2SKIP=gQwOptions.GetValue<int>("skip");
   if (gQwOptions.HasValue("take"))
    fEVENTS2SAVE=gQwOptions.GetValue<int>("take");
   ///

  std::cout<<" Ring "<<r_size<<" , "<<r_BT<<" , "<<r_HLD<<std::endl;
  QwEventRing fEventRing(QwDetectors,r_size,r_HLD,r_BT);
  Double_t evnum=0.0;


  QwDatabase qw_test_DB(gQwOptions);
  
  UInt_t run_id      = 0;
  UInt_t runlet_id   = 0;
  UInt_t analysis_id = 0;

 // Loop over all runs
  while (QwEvt.OpenNextStream() == CODA_OK){
      //  Begin processing for the first run.
      //  Start the timer.
      timer.Start();


      QwEvt.ResetControlParameters();
      //  Open the data files and root file
      //    OpenAllFiles(io, run);
      //      TString rootfilename = Form("%s/Qweak_BeamLine_%d.root", getenv("QW_ROOTFILES_DIR"), run);


//       TString rootfilename=std::string(getenv("QW_ROOTFILES"))+Form("/Qweak_BeamLine_%s.root",  QwEvt.GetRunLabel().Data());
      TString rootfilename=std::string(getenv("QW_ROOTFILES"))+Form("/Qweak_%s.root",  QwEvt.GetRunLabel().Data());

      std::cout<<" rootfilename="<<rootfilename<<"\n";
      TFile rootfile(rootfilename,"RECREATE","QWeak ROOT file");

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
	  QwDetectors.ConstructBranchAndVector(mpstree, dummystr, mpsvector);


	  rootfile.cd();

	  if(bHelicity)
	    {
	      rootfile.cd();
	      heltree = new TTree("HEL_Tree","Helicity event data tree");
	      heltree->SetMaxTreeSize(kMAXTREESIZE);
	      helvector.reserve(6000);
	      //	      TString dummystr="";
	      QwHelPat.ConstructBranchAndVector(heltree, dummystr, helvector);
	    }
	}

      Int_t falied_events_counts = 0;//count falied total events



      // Loop over events in this CODA file
      while (QwEvt.GetNextEvent() == CODA_OK) {
	//  First, do processing of non-physics events...

	    if (QwEvt.IsEPICSEvent()) {
	      QwEvt.FillEPICSData(epics_data);
	      epics_data.CalculateRunningValues();
	      epics_data.PrintAverages();	      
	    }


	if (QwEvt.IsROCConfigurationEvent()){
	  //  Send ROC configuration event data to the subsystem objects.
	  QwEvt.FillSubsystemConfigurationData(QwDetectors);
	}


	//  Now, if this is not a physics event, go back and get a new event.
	if (! QwEvt.IsPhysicsEvent()) continue;

	if(bDebug){
	  std::cout<<"==================================================== \n";
	  std::cout<<" new event:: number ="<<QwEvt.GetEventNumber()<<"\n";
	  std::cout<<"==================================================== \n";
	}
	//std::cout<<"*********** event num "<<QwEvt.GetEventNumber()<<"*************************"<<std::endl;
	//  Fill the subsystem objects with their respective data for this event.
	QwEvt.FillSubsystemData(QwDetectors);


	QwDetectors.ProcessEvent();




	//currently QwHelicity::ApplySingleEventCuts() will check for actual helicity bit for 1 or 0 and falied the test if it is different
	if (QwDetectors.ApplySingleEventCuts()){//The event pass the event cut constraints



	  if(bHelicity){

	    fEventRing.push(QwDetectors);//add event to the ring
	    //std::cerr << "After QwEventRing::push()" <<std::endl;
	    bRING_READY=fEventRing.IsReady();

	    if (bRING_READY){//check to see ring is ready
	      //fEventRing.pop();
	      QwHelPat.LoadEventData(fEventRing.pop());
	    }

	    //QwHelPat.LoadEventData(QwDetectors);
	  }


	  // Accumulate the running sum to calculate the event based running average
	  runningsum.AccumulateRunningSum(QwDetectors);


	  if(bHisto) QwDetectors.FillHistograms();


	  if(bTree){
	    evnum=QwEvt.GetEventNumber();
	    //std::cout<<" event "<<evnum<<std::endl;
	    //((QwBeamLine*)QwDetectors.GetSubsystem("Injector BeamLine"))->FillTreeVector(mpsvector);
	    //((QwHelicity*)QwDetectors.GetSubsystem("Helicity info"))->FillTreeVector(mpsvector);
	    //((QwLumi*)QwDetectors.GetSubsystem("Luminosity Monitors"))->FillTreeVector(mpsvector);

	    //Tree events scaling is set here
	    if (fEVENTS2SKIP==0){
	      bSave=kTRUE;
	      bSkip=kFALSE;
	    }
	    else if (fEVENTS2SAVE==0){
	      bSkip=kTRUE;//ready to skip events
	      bSave=kFALSE;
	    }
	    else if (QwEvt.GetEventNumber()%fEVENTS2SAVE==0 && bSave){
	      bSkip=kTRUE;//ready to skip events
	      bSave=kFALSE;
	      //std::cout<<evnum<<" Ready to skip"<<std::endl;
	    }
	    else if (QwEvt.GetEventNumber()%fEVENTS2SKIP==0 && bSkip){
	      bSave=kTRUE;//read to save data
	      bSkip=kFALSE;
	      //std::cout<<evnum<<"Ready to save"<<std::endl;
	    }
	    //

	    if(bSave){
	      QwDetectors.FillTreeVector(mpsvector);
	      mpstree->Fill();
	    }
	  }

	  if(bHelicity && QwHelPat.IsCompletePattern() && bRING_READY){
	    //QwHelicity * tmp=(QwHelicity *)QwDetectors.GetSubsystem("Helicity info");
	    //std::cout<<" Complete quartet  Good Helicity "<<std::endl;
	    QwHelPat.CalculateAsymmetry();
	    //QwHelPat.Print();
	    if (QwHelPat.IsGoodAsymmetry()){
	      if(bHisto) QwHelPat.FillHistograms();

	      if(bTree && bSave){
		QwHelPat.FillTreeVector(helvector);
		heltree->Fill();
	      }
	      QwHelPat.ClearEventData();
	    }
	  }
	}else{
	  //std::cout<<" Failed event "<<QwEvt.GetEventNumber()<<std::endl;
	  fEventRing.FailedEvent(QwDetectors.GetEventcutErrorFlag()); //event cut failed update the ring status
	  falied_events_counts++;
	}

	if(QwEvt.GetEventNumber()%1000==0){
	  std::cerr << "Number of events processed so far: "
		    << QwEvt.GetEventNumber() << "\n";
	}
      }



      std::cout << "Number of events processed so far: "
		<< QwEvt.GetEventNumber() << std::endl;



      QwHelPat.CalculateRunningAverage();//this will calculate running averages for Asymmetries and Yields per quartet
      std::cout<<"Event Based Running average"<<std::endl;
      std::cout<<"==========================="<<std::endl;
      QwDetectors.CalculateRunningAverage();//this will calculate running averages for Yields per event basis
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

      QwDetectors.GetEventcutErrorCounters();//print the event cut error summery for each sub system
      std::cout<<"QwAnalysis_Beamline Total events falied "<<falied_events_counts<< std::endl;

      if (qw_test_DB.AllowsReadAccess()){
	QwMessage << "GetMonitorID(qwk_batext2) = " << qw_test_DB.GetMonitorID("qwk_batext2") << QwLog::endl;
	QwMessage << "GetMonitorID(phasemonitor) = " << qw_test_DB.GetMonitorID("phasemonitor") << QwLog::endl;
	QwMessage << "GetMonitorID(qwk_junk) = " << qw_test_DB.GetMonitorID("qwk_junk") << QwLog::endl;
	QwMessage << "GetMainDetectorID(md1neg) = " << qw_test_DB.GetMainDetectorID("md1neg") << QwLog::endl;
	QwMessage << "GetMainDetectorID(spare3) = " << qw_test_DB.GetMainDetectorID("spare3") << QwLog::endl;
	QwMessage << "GetMainDetectorID(combinationallmd) = " << qw_test_DB.GetMainDetectorID("combinationallmd") << QwLog::endl;
	QwMessage << "GetLumiDetectorID(dlumi8) = " << qw_test_DB.GetLumiDetectorID("dlumi8") << QwLog::endl;
	QwMessage << "GetLumiDetectorID(ulumi8) = " << qw_test_DB.GetLumiDetectorID("ulumi8") << QwLog::endl;
	QwMessage << "GetVersion() = " << qw_test_DB.GetVersion() << QwLog::endl;
	
	// GetRunID(), GetRunletID(), and GetAnalysisID have their own Connect() and Disconnect() functions.
	run_id      = qw_test_DB.GetRunID(QwEvt);
	runlet_id   = qw_test_DB.GetRunletID(QwEvt);
	analysis_id = qw_test_DB.GetAnalysisID(QwEvt);
	
	QwMessage << "QwAnalysis_MySQL.cc::"
		  << " Run Number "  << QwColor(Qw::kBoldMagenta) << QwEvt.GetRunNumber() << QwColor(Qw::kNormal)
		  << " Run ID "      << QwColor(Qw::kBoldMagenta) << run_id<< QwColor(Qw::kNormal)
		  << " Runlet ID "   << QwColor(Qw::kBoldMagenta) << runlet_id<< QwColor(Qw::kNormal)
		  << " Analysis ID " << QwColor(Qw::kBoldMagenta) << analysis_id
		  << QwLog::endl;
      }
      
      // Each sussystem has its own Connect() and Disconnect() functions.
      if (qw_test_DB.AllowsWriteAccess()){
	QwHelPat.FillDB(&qw_test_DB);
	epics_data.FillDB(&qw_test_DB);
      }

      QwEvt.CloseDataFile();
      QwEvt.ReportRunSummary();
      //epics_data.FillSlowControlsData(qw_test_DB);
      PrintInfo(timer);
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
