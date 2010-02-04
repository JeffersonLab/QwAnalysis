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



Bool_t kInQwBatchMode = kFALSE;
Bool_t bRING_READY;

///
/// \ingroup QwAnalysis_BL
Int_t main(Int_t argc, Char_t* argv[])
{

  Bool_t bDebug=kFALSE;
  Bool_t bHelicity=kTRUE;
  Bool_t bTree=kTRUE;
  Bool_t bHisto=kTRUE;

  //either the DISPLAY not set, or JOB_ID defined, we take it as in batch mode
  if (getenv("DISPLAY")==NULL
      ||getenv("JOB_ID")!=NULL) kInQwBatchMode = kTRUE;
  gROOT->SetBatch(kTRUE);
  
  gQwHists.LoadHistParamsFromFile(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/parity_hists.in");
  
  TStopwatch timer;
  
  QwCommandLine cmdline;
  cmdline.Parse(argc, argv);
  
  QwEventBuffer QwEvt;

  QwSubsystemArrayParity QwDetectors;
  VQwSubsystemParity * subsystem_tmp;//VQwSubsystemParity is the top most parent class for Parity subsystems.
  subsystem_tmp = 0;

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
  



  
  QwHelicityPattern QwHelPat(QwDetectors,4);
  //QwEventRing fEventRing(QwDetectors,120,60,8);//Event ring of 120; 60 hold off events, 8 minimum failed events is a beam trip
  QwEventRing fEventRing(QwDetectors,32,8,16); //Event ring of 32; 8 hold off events, 16  minimum failed events is a beam trip
  //QwEventRing fEventRing(QwDetectors,1200,600,600);
  Double_t evnum=0.0;

  
  for(Int_t run = cmdline.GetFirstRun(); run <= cmdline.GetLastRun(); run++)
    { 
      //  Begin processing for the first run.
      //  Start the timer.
      timer.Start();
      
      //  Try to open the data file.
      if (QwEvt.OpenDataFile(run) != CODA_OK){
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
 
      TString rootfilename=std::string(getenv("QW_ROOTFILES_DIR"))+Form("/Qweak_BeamLine_%d.root",run);
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
	  mpsvector.reserve(6000); 
	  // if one defines more than 600 words in the full ntuple
	  // results are going to get very very crazy.
	  mpstree->Branch("evnum",&evnum,"evnum/D");
	  TString dummystr="";
	  ((QwBeamLine*)QwDetectors.GetSubsystem("Injector BeamLine"))->ConstructBranchAndVector(mpstree, dummystr, mpsvector);  
	  ((QwBeamLine*)QwDetectors.GetSubsystem("Helicity info"))->ConstructBranchAndVector(mpstree, dummystr, mpsvector);  
	  ((QwBeamLine*)QwDetectors.GetSubsystem("Luminosity Monitors"))->ConstructBranchAndVector(mpstree, dummystr, mpsvector); 
	  // QwDetectors.ConstructBranchAndVector(mpstree, dummystr, mpsvector);
	  // at some point we want to have some thing like that but it need to be implement in QwSubsystemArray
	  rootfile.cd();
	  if(bHelicity)
	    {
	      rootfile.cd();
	      heltree = new TTree("HEL_Tree","Helicity event data tree");	      
	      helvector.reserve(6000); 
	      TString dummystr="";
	      QwHelPat.ConstructBranchAndVector(heltree, dummystr, helvector);
	    }

	  
	}

      Int_t falied_events_counts=0;//count falied total events

      while (QwEvt.GetEvent() == CODA_OK){
	//Loop over events in this CODA file

	//  First, do processing of non-physics events...
	if (QwEvt.IsROCConfigurationEvent()){
	  //  Send ROC configuration event data to the subsystem objects.
	  QwEvt.FillSubsystemConfigurationData(QwDetectors);
	}
	  
	//  Now, if this is not a physics event, go back and get a new event.
	if (! QwEvt.IsPhysicsEvent()) continue;

	//  Check to see if we want to process this event.
	if (QwEvt.GetEventNumber() < cmdline.GetFirstEvent()) continue;
	else if (QwEvt.GetEventNumber() > cmdline.GetLastEvent()) break;
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
	  
	  //QwDetectors.Do_RunningSum();//accimulate the running sum to calculate the event base running AVG 

	  
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


	  	  

	  if(bHisto) QwDetectors.FillHistograms(); 


	  if(bTree){
	    evnum=QwEvt.GetEventNumber();
	    //std::cout<<" event "<<evnum<<std::endl;
	    ((QwBeamLine*)QwDetectors.GetSubsystem("Injector BeamLine"))->FillTreeVector(mpsvector);
	    ((QwHelicity*)QwDetectors.GetSubsystem("Helicity info"))->FillTreeVector(mpsvector);
	    ((QwLumi*)QwDetectors.GetSubsystem("Luminosity Monitors"))->FillTreeVector(mpsvector);
	    mpstree->Fill();
	  }

	  if(bHelicity && QwHelPat.IsCompletePattern() && bRING_READY){
	    //QwHelicity * tmp=(QwHelicity *)QwDetectors.GetSubsystem("Helicity info");
	    //std::cout<<" Complete quartet  Good Helicity "<<std::endl;
	    QwHelPat.CalculateAsymmetry();
	    //QwHelPat.Print();
	    if (QwHelPat.IsGoodAsymmetry()){
	      if(bHisto) QwHelPat.FillHistograms();

	      if(bTree){
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
		    << QwEvt.GetEventNumber() << "\r";
	}         
      }

      

      std::cout << "Number of events processed so far: " 
		<< QwEvt.GetEventNumber() << std::endl;

      //This will print running averages 
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
      if(bHisto||bTree)rootfile.Write(0,TObject::kOverwrite);
      if(bHisto){
	std::cout<<"QwDetectors.DeleteHistograms\n";
	QwDetectors.DeleteHistograms();
	std::cout<<"QwHelPat.DeleteHistograms\n";
	QwHelPat.DeleteHistograms();
      }

      QwEvt.CloseDataFile();      
      QwEvt.ReportRunSummary();
      
      

     
     
      //QwHelPat.Print();	
    
      //QwDetectors.Calculate_Running_Average();//this will calculate running averages for Yields per event basis
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
