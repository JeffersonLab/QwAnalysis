/*------------------------------------------------------------------------*//*!

 \defgroup QwAnalysis_BL QwAnalysis_BeamLine

 \section myoverview Overview of the Qweak parity data analysis (beamline)

    Put something here

 \section MS1 More Stuff

    Put something here

    \subsection MS2 More Substuff

    Put some more stuff here

    \subsection MS3 Even More Substuff

    More stuff


*//*-------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*//*!

 \file QwAnalysis_BeamLine.cc

 \brief main(...) function for the qwanalysis_beamline executable

*//*-------------------------------------------------------------------------*/

#include "QwAnalysis_BeamLine.h"
#include "TApplication.h"
#include <boost/shared_ptr.hpp>
#include "QwMapFile.h"

#include "QwLog.h"

Bool_t kInQwBatchMode = kFALSE;
Bool_t bRING_READY;


///
/// \ingroup QwAnalysis_BL
Int_t main(Int_t argc, Char_t* argv[])
{
  QwMapFile *rootfile=NULL; //The map file
  // First, we set the command line arguments and the configuration filename,
  // and we define the options that can be used in them (using QwOptions).
  gQwOptions.SetCommandLine(argc, argv);
  gQwOptions.SetConfigFile("qwanalysis_beamline.conf");
  // Define the command line options
  DefineOptionsParity(gQwOptions);



  Bool_t bDebug=kFALSE;
  Bool_t bHelicity=kTRUE;
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
  QwDetectors.push_back(new QwHelicity("Helicity info"));
  QwDetectors.GetSubsystemByName("Helicity info")->LoadChannelMap("qweak_helicity.map");
  QwDetectors.GetSubsystemByName("Helicity info")->LoadInputParameters("");
  QwDetectors.push_back(new QwLumi("Luminosity Monitors"));
  QwDetectors.GetSubsystemByName("Luminosity Monitors")->LoadChannelMap("qweak_lumi.map");//current map file is for the beamline.
  QwDetectors.GetSubsystemByName("Luminosity Monitors")->LoadEventCuts("qweak_lumi_eventcuts.in");//Pass the correct cuts file.

  QwSubsystemArrayParity runningsum;
  runningsum.Copy(&QwDetectors);

  QwDetectors.GetSubsystemByName("Injector BeamLine")->LoadGeometryDefinition("qweak_beamline_geometry.map"); //read in the gemoetry of the beamline


  QwDetectors.ProcessOptions(gQwOptions);//Recommonded to call this routine after LoadChannelMap(..) routines. Some times the cmd options override the map file settings.

  QwHelicityPattern QwHelPat(QwDetectors);//multiplet size is set within the QwHelicityPattern class
  QwHelPat.ProcessOptions(gQwOptions);


  fEventRing.SetupRing(QwDetectors);//set up the ring with QwDetector array array with CMD ring parameters

  // Loop over all runs
  while (QwEvt.OpenNextStream() == CODA_OK){
      //  Begin processing for the first run.
      //  Start the timer.
      timer.Start();



      //Map file access setup
      if (rootfile){
	std::cout<<"after rootfile!=NULL \n";
	rootfile->Close();
	rootfile=NULL;
      }
      TString theMemMapFile;
      theMemMapFile = getenv_safe_TString("QW_ROOTFILES");
      theMemMapFile += "/QwMemMapFile.map";
      //theMemMapFile = "QwMemMapFile.map";
      std::cout<<" ROOT map file name "<<theMemMapFile<<std::endl;
      rootfile = new QwMapFile(theMemMapFile,"Memory Mapped File","RECREATE");
      rootfile->Print();
      if(!rootfile)
	std::cerr<<"ROOT file could not be created. Exiting!!!"<<std::endl;
      //end of Map file access  setup


      if(bHisto){
	QwDetectors.ConstructHistograms();
	if(bHelicity)
	  {
	    QwHelPat.ConstructHistograms();
	  }
      }


      std::cout<<"Map File Created\n";
      rootfile->Update();
      rootfile->Print();


      Int_t falied_events_counts=0;//count falied total events


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





	  if(bHelicity && QwHelPat.IsCompletePattern() && bRING_READY){
	    QwHelPat.CalculateAsymmetry();
	    if (QwHelPat.IsGoodAsymmetry()){
	      if(bHisto)
		QwHelPat.FillHistograms();
	      QwHelPat.ClearEventData();
	    }
	  }

	  if(bHisto){
	    QwDetectors.FillHistograms();
	    if ((QwEvt.GetEventNumber()%400)==0)
	      rootfile->Update();//Update the map file
	  }

	}else{
	  fEventRing.FailedEvent(QwDetectors.GetEventcutErrorFlag()); //event cut failed update the ring status
	  falied_events_counts++;
	}

	if(QwEvt.GetEventNumber()%1000==0){
	  //std::cout << "Number of events processed so far: "<< QwEvt.GetEventNumber()<<"\r";
	  QwMessage << "Number of events processed so far: "<< QwEvt.GetEventNumber() <<QwLog::endl;
	}
      }



      QwMessage<< "Number of events processed so far: "<< QwEvt.GetEventNumber() << "\r";

      //This will print running averages
      QwHelPat.CalculateRunningAverage();//this will calculate running averages for Asymmetries and Yields per quartet
      std::cout<<"Event Based Running average"<<std::endl;
      std::cout<<"==========================="<<std::endl;
      QwDetectors.CalculateRunningAverage();//this will calculate running averages for Yields per event basis
      timer.Stop();

      /*  We need to delete the histograms here.                       *
       *  If we wait until the subsystem destructors, we get a         *
       *  segfault; but in additiona to that we should delete them     *
       *  here, in case we run over multiple runs at a time.           */
      if(bHisto){
	//std::cout<<"QwDetectors.DeleteHistograms\n";
	QwDetectors.DeleteHistograms();
	//std::cout<<"QwHelPat.DeleteHistograms\n";
	QwHelPat.DeleteHistograms();
      }

      if(bHisto){

	rootfile->Close();
      }
      QwEvt.CloseStream();
      QwEvt.ReportRunSummary();





      QwDetectors.GetEventcutErrorCounters();//print the event cut error summery for each sub system
      std::cout<<"QwAnalysis_Beamline Total events falied "<<falied_events_counts<< std::endl;

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
