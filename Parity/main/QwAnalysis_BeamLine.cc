/*------------------------------------------------------------------------*//*!

 \file QwAnalysis_BeamLine.cc

 \brief main(...) function for the qwanalysis_beamline executable

*//*-------------------------------------------------------------------------*/

#include "QwAnalysis_BeamLine.h"
#include "TApplication.h"
#include <boost/shared_ptr.hpp>

#include "QwLog.h"

#include "QwFakeHelicity.h"
#include "QwBeamMod.h"

#include "QwFakeHelicity.h"

Bool_t kInQwBatchMode = kFALSE;
Bool_t bRING_READY;
Bool_t bSkip= kFALSE;;
Bool_t bSave= kTRUE;
Int_t fEVENTS2SKIP, fEVENTS2SAVE;


///
/// \ingroup QwAnalysis_BL
Int_t main(Int_t argc, Char_t* argv[])
{

  // First, we set the command line arguments and the configuration filename,
  // and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwanalysis_beamline.conf");
  // Define the command line options
  DefineOptionsParity(gQwOptions);

  //QwAnalysis_BeamLine Options
  gQwOptions.AddOptions()("skip", po::value<int>()->default_value(0),"No. of events to skip");
  gQwOptions.AddOptions()("take", po::value<int>()->default_value(1000),"No. of events to save into tree = (take - skip)");


  // modified value for maximum size of tree
  Long64_t kMAXTREESIZE = 10000000000LL;
  // standard value for maximum size of tree in root source
  //  Long64_t kMAXTREESIZE = 1900000000LL;

  Bool_t bDebug=kFALSE;
  Bool_t bHelicity=kFALSE;
  Bool_t bTree=kTRUE;
  Bool_t bHisto=kTRUE;


//either the DISPLAY not set, or JOB_ID defined, we take it as in batch mode
  if (getenv("DISPLAY")==NULL||getenv("JOB_ID")!=NULL)
    kInQwBatchMode = kTRUE;
  gROOT->SetBatch(kTRUE);



  ///  Fill the search paths for the parameter files; this sets a static
  ///  variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS")+"/Parity/prminput");

  ///
  ///  Load the histogram parameter definitions (from parity_hists.txt) into the global
  ///  histogram helper: QwHistogramHelper
  ///
  gQwHists.LoadHistParamsFromFile("qweak_parity_hists.in");

  TStopwatch timer;

  QwEventBuffer QwEvt;
  QwEvt.ProcessOptions(gQwOptions);

  QwEventRing fEventRing;
  fEventRing.ProcessOptions(gQwOptions);//load ring parameters from the CMD or config file

  QwSubsystemArrayParity QwDetectors;
  VQwSubsystemParity * subsystem_tmp;//VQwSubsystemParity is the top most parent class for Parity subsystems.
  subsystem_tmp = 0;

  QwDetectors.push_back(new QwBeamLine("Injector BeamLine"));
  QwDetectors.GetSubsystemByName("Injector BeamLine")->LoadChannelMap("qweak_beamline.map");
  QwDetectors.GetSubsystemByName("Injector BeamLine")->LoadInputParameters("qweak_pedestal.map");
  QwDetectors.GetSubsystemByName("Injector BeamLine")->LoadEventCuts("qweak_beamline_eventcuts.in");//Pass the correct cuts file.
  QwDetectors.push_back(new QwFakeHelicity("Helicity info"));
  QwDetectors.GetSubsystemByName("Helicity info")->LoadChannelMap("qweak_helicity.map");
  QwDetectors.GetSubsystemByName("Helicity info")->LoadInputParameters("");
  QwDetectors.push_back(new QwLumi("Luminosity Monitors"));
  QwDetectors.GetSubsystemByName("Luminosity Monitors")->LoadChannelMap("qweak_lumi.map");//current map file is for the beamline.
  QwDetectors.GetSubsystemByName("Luminosity Monitors")->LoadEventCuts("qweak_lumi_eventcuts.in");//Pass the correct cuts file.

  QwDetectors.push_back(new QwBeamMod("BeamMod"));
  QwDetectors.GetSubsystemByName("BeamMod")->LoadChannelMap("qweak_modulation.map");


  // Running sum
  QwSubsystemArrayParity runningsum;
  runningsum.Copy(&QwDetectors);

  QwDetectors.GetSubsystemByName("Injector BeamLine")->LoadGeometryDefinition("qweak_beamline_geometry.map"); //read in the gemoetry of the beamline

  QwDetectors.ProcessOptions(gQwOptions);//Recommonded to call this routine after LoadChannelMap(..) routines. Some times the cmd options override the map file settings.

  QwHelicityPattern QwHelPat(QwDetectors);//multiplet size is set within the QwHelicityPattern class
  QwHelPat.ProcessOptions(gQwOptions);


  //Tree events scaling parameters
   if (gQwOptions.HasValue("skip"))
    fEVENTS2SKIP=gQwOptions.GetValue<int>("skip");
   if (gQwOptions.HasValue("take"))
    fEVENTS2SAVE=gQwOptions.GetValue<int>("take");
   ///

   fEventRing.SetupRing(QwDetectors);//set up the ring with QwDetector array array with CMD ring parameters

   Double_t evnum=0.0;


   // Loop over all runs
   while (QwEvt.OpenNextStream() == CODA_OK){
      //  Begin processing for the first run.
      //  Start the timer.
      timer.Start();


      //  Open the data files and root file
      //    OpenAllFiles(io, run);

      TString rootfilename = getenv_safe_TString("QW_ROOTFILES") + Form("/Qweak_BeamLine_%s.root",
									QwEvt.GetRunLabel().Data());
      std::cout<<" rootfilename="<<rootfilename<<"\n";
      TFile rootfile(rootfilename,
		     "RECREATE","QWeak ROOT file");

      if(bHisto){
	rootfile.cd();
	QwDetectors.ConstructHistograms(rootfile.mkdir("mps_histo"));
	if(bHelicity)
	  {
	    rootfile.cd();
	    QwHelPat.ConstructHistograms(rootfile.mkdir("hel_histo"));
	  }
      }


      TTree *mpstree =0;
      TTree *heltree =0;

      std::vector <Double_t> mpsvector;
      std::vector <Double_t> helvector;

      if(bTree)
	{
	  rootfile.cd();
	  mpstree=new TTree("MPS_Tree","MPS event data tree");
	  mpstree->SetMaxTreeSize(kMAXTREESIZE);
	  mpsvector.reserve(6000);
	  // if one defines more than 600 words in the full ntuple
	  // results are going to get very very crazy.
	  mpstree->Branch("evnum",&evnum,"evnum/D");
	  TString dummystr="";
	  QwDetectors.ConstructBranchAndVector(mpstree, dummystr, mpsvector);
	  // at some point we want to have some thing like that but it need to be implement in QwSubsystemArray
	  rootfile.cd();
	  if(bHelicity)
	    {
	      rootfile.cd();
	      heltree = new TTree("HEL_Tree","Helicity event data tree");
	      heltree->SetMaxTreeSize(kMAXTREESIZE);
	      helvector.reserve(6000);
	      TString dummystr="";
	      QwHelPat.ConstructBranchAndVector(heltree, dummystr, helvector);
	    }


	}

      Int_t failed_events_counts=0;//count failed total events

      //  Clear the single-event running sum at the beginning of the
      //  runlet.
      runningsum.ClearEventData();
      QwHelPat.ClearRunningSum();


      //  Clear the running sum of the burst values at the beginning of the
      //  runlet.
      QwHelPat.ClearBurstSum();

      // Loop over events in this CODA file
      while (QwEvt.GetNextEvent() == CODA_OK) {
	//  First, do processing of non-physics events...
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



	if (QwDetectors.ApplySingleEventCuts()){//The event pass the event cut constraints


	  if(bHelicity){

	    fEventRing.push(QwDetectors);//add event to the ring
	    bRING_READY=fEventRing.IsReady();

	    if (bRING_READY){//check to see ring is ready
	      QwHelPat.LoadEventData(fEventRing.pop());
	    }


	  }


	  // Accumulate the running sum to calculate the event based running average
	  runningsum.AccumulateRunningSum(QwDetectors);


	  if(bHisto) QwDetectors.FillHistograms();


	  if(bTree){
	    evnum=QwEvt.GetEventNumber();

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
	    QwHelPat.CalculateAsymmetry();
	    //QwHelPat.Print();
	    if (QwHelPat.IsGoodAsymmetry()){
	      if(bHisto) QwHelPat.FillHistograms();

	      if(bTree && bSave){
		QwHelPat.FillTreeVector(helvector);
		heltree->Fill();
	      }

	      //  Do we do the burst analysis here, where we know we
	      //  have a complete pattern, or do we do it below, but
	      //  need to test for a complete pattern being done?

	      QwHelPat.ClearEventData();
	    }
	  }
	}else{
	  fEventRing.FailedEvent(QwDetectors.GetEventcutErrorFlag()); //event cut failed update the ring status
	  failed_events_counts++;
	}

	if (QwEvt.GetEventNumber() % 1000 == 0) {
	  QwMessage << "Number of events processed so far: "
		    << QwEvt.GetEventNumber() <<"\r"<< QwLog::endl;
	}

	// TODO (wdc) QwEventBuffer should have Bool_t AtEndOfBurst()
	//if (QwEvt.AtEndOfBurst()){
	if (QwEvt.GetEventNumber() % 100 == 0) {
	  {
	    QwHelPat.AccumulateRunningBurstSum();
	    QwHelPat.CalculateBurstAverage();
	    QwHelPat.ClearBurstSum();

	  }
	}

      }  // Ends "while (QwEvt.GetNextEvent() == CODA_OK)" loop




      std::cout << "Number of events processed so far: "
		<< QwEvt.GetEventNumber() << std::endl;

      //This will print running averages
      //  In burst mode, the following should return the multi-burst
      //  averaged running averages, and in normal mode the "regular"
      //  running averages.
      QwHelPat.CalculateRunningAverage();//this will calculate running averages for Asymmetries and Yields per quartet
      QwHelPat.CalculateRunningBurstAverage();

      std::cout<<"Event Based Running average"<<std::endl;
      std::cout<<"==========================="<<std::endl;
      // This will calculate running averages for Yields per event basis
      runningsum.CalculateRunningAverage();

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
      if(bHisto||bTree)rootfile.Write(0,TObject::kOverwrite);
      if(bHisto){
	std::cout<<"QwDetectors.DeleteHistograms\n";
	QwDetectors.DeleteHistograms();
	std::cout<<"QwHelPat.DeleteHistograms\n";
	QwHelPat.DeleteHistograms();
      }

      QwEvt.CloseStream();
      QwEvt.ReportRunSummary();





      //QwHelPat.Print();


      QwDetectors.GetEventcutErrorCounters();//print the event cut error summery for each sub system
      std::cout<<"QwAnalysis_Beamline Total events failed "<<failed_events_counts<< std::endl;

      PrintInfo(timer);

    } //end of run loop

  std::cerr << "I have done everything I can do..." << std::endl;

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
