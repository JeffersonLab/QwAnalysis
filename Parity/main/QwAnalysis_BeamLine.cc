#include "QwAnalysis_BeamLine.h"
#include "TApplication.h"
#include <boost/shared_ptr.hpp>



Bool_t kInQwBatchMode = kFALSE;



int main(Int_t argc,Char_t* argv[])
{

  Bool_t bHelicity=kTRUE;

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
  
  QwDetectors.push_back(new QwBeamLine("Injector BeamLine"));
  QwDetectors.GetSubsystem("Injector BeamLine")->LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_beamline.map");
  QwDetectors.GetSubsystem("Injector BeamLine")->LoadInputParameters(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_pedestal.map");

  //  if(bHelicity)
    QwHelicityPattern QwHelPat(QwDetectors,2);

  ///////

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
      //    OpenAllFiles(io, run);
      TString rootfilename=std::string(getenv("QW_ROOTFILES_DIR"))+Form("/Qweak_BeamLine_%d.root",run);
      std::cout<<" rootfilename="<<rootfilename<<"\n";
      TFile rootfile(rootfilename,
		     "RECREATE","QWeak ROOT file with histograms");
      

      QwDetectors.ConstructHistograms(rootfile.mkdir("mps_histo"));
      if(bHelicity)
	QwHelPat.ConstructHistograms(rootfile.mkdir("hel_histo"));

      rootfile.cd();
      TTree *mpstree = new TTree("MPS_Tree","MPS event data tree");
      std::vector <Float_t> mpsvector;
      Float_t evnum=0.0;
      mpsvector.reserve(6000); 
      // if one defines more than 600 words in the full ntuple
      // results are going to get very very crazy.
      mpstree->Branch("evnum",&evnum,"evnum/F");
      TString dummystr="";
      ((QwBeamLine*)QwDetectors.GetSubsystem("Injector BeamLine"))->ConstructBranchAndVector(mpstree, dummystr, mpsvector);


      rootfile.cd();
//       if(bHelicity)
// 	{
// 	  TTree *heltree = new TTree("HEL_Tree","Helicity event data tree");
// 	  std::vector <Float_t> helvector;
// 	  Float_t evnumhel=0.0;
// 	  helvector.reserve(6000); 
// 	  heltree->Branch("hel_evnum",&evnumhel,"hel_evnum/F");
// 	  TString dummystr="";
// 	  ((QwBeamLine*)QwDetectors.GetSubsystem("Injector BeamLine"))->ConstructBranchAndVector(heltree, dummystr, helvector);
// 	}


      while (QwEvt.GetEvent() == CODA_OK)
	{
	  //Loop over events in this CODA file

	  //  First, do processing of non-physics events...
	  if (QwEvt.IsROCConfigurationEvent())
	    {
	      //  Send ROC configuration event data to the subsystem objects.
	      QwEvt.FillSubsystemConfigurationData(QwDetectors);
	    }
	  
	  //  Now, if this is not a physics event, go back and get a new event.
	  if (! QwEvt.IsPhysicsEvent()) continue;
	  
	  //  Check to see if we want to process this event.
	  if (QwEvt.GetEventNumber() < cmdline.GetFirstEvent()) continue;
	  else if (QwEvt.GetEventNumber() > cmdline.GetLastEvent()) break;
	  
	  //  Fill the subsystem objects with their respective data for this event.
	  QwEvt.FillSubsystemData(QwDetectors);
	
	  
	  QwDetectors.ProcessEvent();
	  if(bHelicity)
	    QwHelPat.LoadEventData(QwDetectors);	  

	  QwDetectors.FillHistograms();
   	  evnum=QwEvt.GetEventNumber();
   	  ((QwBeamLine*)QwDetectors.GetSubsystem("Injector BeamLine"))->FillTreeVector(mpsvector);
   	  mpstree->Fill();


	  if(bHelicity&&QwHelPat.IsCompletePattern())
	    {
	      QwHelPat.CalculateAsymmetry();
	      QwHelPat.FillHistograms();
	      QwHelPat.ClearEventData();
	    }
	      
	  if(QwEvt.GetEventNumber()%1000==0)
	    {
	      std::cerr << "Number of events processed so far: " 
			<< QwEvt.GetEventNumber() << "\r";
	    }         
      } 

      std::cout << "Number of events processed so far: " 
		<< QwEvt.GetEventNumber() << std::endl;
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
      rootfile.Write(0,TObject::kOverwrite);
      QwDetectors.DeleteHistograms();
      
      QwEvt.CloseDataFile();
      QwEvt.ReportRunSummary();
      
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
