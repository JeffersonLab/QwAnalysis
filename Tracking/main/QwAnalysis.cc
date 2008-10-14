
/**********************************************************\
* File:  Analysis.cc                                       *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-16 15:40>                           *

Qweak Analysis Framework

Each subsystem will have a class derived from "VQwSubsystem",
and will be responsible for decoding of it's own data stream
and any special event processing required.
Each susbsytem will also own the histograms and ntupling 
functions used for its data.

\**********************************************************/


#include "QwAnalysis.h"
#include "TApplication.h"

#include <boost/shared_ptr.hpp>

#include "QwHitContainer.h"




Bool_t kInQwBatchMode = kFALSE;



int main(Int_t argc,Char_t* argv[])
{
  //either the DISPLAY not set, or JOB_ID defined, we take it as in batch mode
  if (getenv("DISPLAY")==NULL
      ||getenv("JOB_ID")!=NULL) kInQwBatchMode = kTRUE;
  gROOT->SetBatch(kTRUE);

  //  Fill the search paths for the parameter files; this sets a static 
  //  variable within the QwParameterFile class which will be used by  
  //  all instances.
  //  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH"))+"/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput");

  //  Load the histogram parameter definitions into the global histogram helper
  gQwHists.LoadHistParamsFromFile("cosmics_hists.in");
  
  TStopwatch timer;

  QwCommandLine cmdline;
  cmdline.Parse(argc, argv);

  QwEventBuffer eventbuffer;
  
  QwSubsystemArray QwDetectors;

  QwDetectors.push_back(new QwDriftChamberHDC("R2"));
  QwDetectors.GetSubsystem("R2")->LoadChannelMap("gzero_wc.map");
  QwDetectors.push_back(new QwDriftChamberVDC("R3"));
  QwDetectors.GetSubsystem("R3")->LoadChannelMap("gzero_wc.map");
  QwDetectors.push_back(new QwMainDetector("MD"));
  QwDetectors.GetSubsystem("MD")->LoadChannelMap("maindet_cosmics.map");  

  boost::shared_ptr<QwHitContainer> fHitList;
  QwHitContainer grandHitList;
  QwDriftChamber * qw;





 
        

 


  for(Int_t run = cmdline.GetFirstRun(); run <= cmdline.GetLastRun(); run++){
    //  Begin processing for the first run.
    //  Start the timer.
    timer.Start();

    //  Try to open the data file.
    if (eventbuffer.OpenDataFile(run) != CODA_OK){
      //  The data file can't be opened.
      //  Get ready to process the next run.
      std::cerr << "ERROR:  Unable to find data files for run "
		<< run << ".  Moving to the next run.\n" 
		<< std::endl;
      timer.Stop();
      continue;
    }

    eventbuffer.ResetControlParameters();

    //     //  Configure database access mode, and load the calibrations
    //     //  from the database.


    //  Create the root file
    boost::shared_ptr<TFile> 
      rootfile(new TFile(Form("Qweak_%d.root",run), 
			 "RECREATE",
			 "QWeak ROOT file with histograms"));

    //  Create the histograms for the QwDriftChamber subsystem object.
    //  We can create a subfolder in the rootfile first, if we want,
    //  and then pass it into the constructor.
    //
    //  To pass a subdirectory named "subdir", we would do:
    //    QwDetectors.GetSubsystem("MD")->ConstructHistograms(rootfile->mkdir("subdir"));
    QwDetectors.ConstructHistograms();

   
    while (eventbuffer.GetEvent() == CODA_OK){
      //  Loop over events in this CODA file
      //  First, do processing of non-physics events...

      //  Now, if this is not a physics event, go back and get
      //  a new event.
      if (! eventbuffer.IsPhysicsEvent()) continue;
      
      //  Check to see if we want to process this event.
      if (eventbuffer.GetEventNumber() < cmdline.GetFirstEvent()) continue;
      else if (eventbuffer.GetEventNumber() > cmdline.GetLastEvent()) break;

      if(eventbuffer.GetEventNumber()%1000==0) {
	std::cerr << "Number of events processed so far: " 
		  << eventbuffer.GetEventNumber() << "\r";
      }

      //  Fill the subsystem objects with their respective data for this event.
      eventbuffer.FillSubsystemData(QwDetectors);
      
      QwDetectors.ProcessEvent();

      //  Fill the histograms for the subsystem objects.
      QwDetectors.FillHistograms();

      //  Build a global vector consisting of each
      //  subsystems list concatenated together.
      //  Make it a QwHitContainer class

       //updating the hitcontainer list

      //REGION 2
      qw=(QwDriftChamberHDC *)QwDetectors.GetSubsystem("R2");
      qw->getHitList(grandHitList);
      //std::cout << "After R2 "<<grandHitList.size()<<std::endl;
      //REGION 3
      qw=(QwDriftChamberVDC *)QwDetectors.GetSubsystem("R3");
      qw->getHitList(grandHitList);
      //std::cout << "After R3 "<<grandHitList.size()<<std::endl; 

      //sorting the grand hit list
      grandHitList.sort();
      

      //  Pass the QwHitContainer to the treedo class
      
      //  Call the recontruction routines of the treedo
      //  class.


    }    
    std::cout << "Number of events processed so far: " 
	      << eventbuffer.GetEventNumber() << std::endl;
    timer.Stop();

    /*  Write to the root file, being sure to delete the old cycles  *
     *  which were written by Autosave.                              *
     *  Doing this will remove the multiple copies of the ntuples    *
     *  from the root file.                                          */
    if(rootfile != NULL) rootfile->Write(0,TObject::kOverwrite);



    
   
    
    QwDetectors.DeleteHistograms();
    
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();



    
    PrintInfo(timer, run);

  } //end of run loop
  



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


