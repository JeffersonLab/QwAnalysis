///  Qweak Analysis Framework

///  Each subsystem will have a class derived from "VQwSubsystem",
///  and will be responsible for decoding of it's own data stream
///  and any special event processing required.
///  Each susbsytem will also own the histograms and ntupling 
///  functions used for its data.

/*  ------------
    
*/

#include "QwAnalysis.h"
#include "TApplication.h"

Bool_t kInQwBatchMode = kFALSE;



int main(Int_t argc,Char_t* argv[])
{
  //either the DISPLAY not set, or JOB_ID defined, we take it as in batch mode
  if (getenv("DISPLAY")==NULL
      ||getenv("JOB_ID")!=NULL) kInQwBatchMode = kTRUE;
  gROOT->SetBatch(kTRUE);

  gQwHists.LoadHistParamsFromFile(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/cosmics_hists.in");
  
  TStopwatch timer;

  QwCommandLine cmdline;
  cmdline.Parse(argc, argv);

  TFile *rootfile;

  QwEventBuffer QwEvt;
  
  std::vector<VQwSubsystem*> QwDetectors;

  QwDetectors.resize(3,NULL);  // QwDetectors[0]==GEM, QwDetectors[1]==Region2
  QwDetectors.at(1) = new QwDriftChamber("R1");
  TString mapfile = TString(getenv("QWANALYSIS")) 
    + "/Tracking/prminput/gzero_wc.map";
  QwDetectors.at(1)->LoadChannelMap(mapfile);

  QwDetectors.at(2) = new QwMainDetector("MD");
  QwDetectors.at(2)->LoadChannelMap(TString(getenv("QWANALYSIS")) + 
				    "/Tracking/prminput/maindet_cosmics.map");
  

  for(Int_t run = cmdline.GetFirstRun(); run <= cmdline.GetLastRun(); run++){
    //   Int_t run = 62310;
    //   Int_t firstevent = 1;
    //   Int_t lastevent  = 50000;  // Run 62310 has over 5,241,000 events.

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

    //     //  Configure database access mode, and load the calibrations
    //     //  from the database.


    //  Open the data files and root file
    //    OpenAllFiles(io, run);
    rootfile = new TFile(Form("Qweak_%d.root",run),
			 "RECREATE","QWeak ROOT file with histograms");

    //  Create the histograms for the QwDriftChamber subsystem object.
    //  We can create a subfolder in the rootfile first, if we want,
    //  and then pass it into the constructor.
    //
    //  To pass a subdirectory named "subdir", we would do:
    //    QwDetectors.at(1)->ConstructHistograms(rootfile->mkdir("subdir"));
    QwDetectors.at(1)->ConstructHistograms();
    QwDetectors.at(2)->ConstructHistograms();


    while (QwEvt.GetEvent() == CODA_OK){
      //  Loop over events in this CODA file
      //  First, do processing of non-physics events...

      //  Now, if this is not a physics event, go back and get
      //  a new event.
      if (! QwEvt.IsPhysicsEvent()) continue;
      
      //  Check to see if we want to process this event.
      if (QwEvt.GetEventNumber() < cmdline.GetFirstEvent()) continue;
      else if (QwEvt.GetEventNumber() > cmdline.GetLastEvent()) break;

      if(QwEvt.GetEventNumber()%1000==0) {
	std::cerr << "Number of events processed so far: " 
		  << QwEvt.GetEventNumber() << "\r";
      }

      //  Fill the subsystem objects with their respective data for this event.
      QwEvt.FillSubsystemData(QwDetectors);
      
      
      //  Fill the histograms for the QwDriftChamber subsystem object.
      QwDetectors.at(1)->ProcessEvent();
      QwDetectors.at(1)->FillHistograms();

      QwDetectors.at(2)->ProcessEvent();
      QwDetectors.at(2)->FillHistograms();


    }    
    std::cout << "Number of events processed so far: " 
	      << QwEvt.GetEventNumber() << std::endl;
    timer.Stop();

    /*  Write to the root file, being sure to delete the old cycles  *
     *  which were written by Autosave.                              *
     *  Doing this will remove the multiple copies of the ntuples    *
     *  from the root file.                                          */
    if(rootfile != NULL) rootfile->Write(0,TObject::kOverwrite);
    
    //  Delete the histograms for the QwDriftChamber subsystem object.
    //  In G0, we had to do:
    //     Hists->ClearHists();//destroy the histogram objects
    //     NTs->ClearNTs(io); //destroy nts according to the i/o flags
    //     CloseAllFiles(io); //close all the output files
    QwDetectors.at(1)->DeleteHistograms();
    QwDetectors.at(2)->DeleteHistograms();
    
    QwEvt.CloseDataFile();
    QwEvt.ReportRunSummary();


//     // Write to SQL DB if chosen on command line
//     if (sql->GetDBFlag() && sql->GetDBAccessLevel()=="rw") {
//       //Fill the analysis table. Moved right before filling the rest of the DB
//       //For the 2nd pass we need to retrieve the values linking to the 
//       //analysis_id of the 1st pass.
//       sql->FillAnalysisTable(run);

//       sql->SetPrintSQL(kFALSE);
      

//       QwEpics->WriteDatabase(sql);
//     }
    
    PrintInfo(timer);

  } //end of run loop
  

//   //  The deletion lines are in reverse order of the creation of the
//   //  objects.
//   //  Also, we don't need to check against the objects being NULL
//   //  before deletion.
  
  delete rootfile;

  for (size_t i=0; i<QwDetectors.size(); i++){
    if (QwDetectors[i]!=NULL) delete QwDetectors[i];
  }

  return 0;
}


// void OpenAllFiles(G0ReplayPrms* io, Run run)
// {
//   if(io->GetSegmentIsSelectedFlg()){//--seg flag
//     io->OpenDataFile(run,io->GetSegment());//ugly but maybe less confusing??
//   }
//   else{    
//     if (io->DataFileIsSegmented(run)){
//       // Don't do anything.  The file will be opened by
//       // the method "io->OpenNextSegment(run)" below.
//     } else {
//       io->OpenDataFile(run); 
//     }
//   }
//   io->OpenRootFile();
//   return;
// }

// void CloseAllFiles(G0ReplayPrms* io)
// {
//   io->CloseRootFile();
//   //io->CloseRateFile();
//   io->CloseDataFile();
//   io->CloseAsymFile();
//   io->CloseBitErrFile();
//   return;
// };

void PrintInfo(TStopwatch& timer)
{
  std::cout << "CPU time used:  "  << timer.CpuTime() << " s" 
	    << std::endl
	    << "Real time used: " << timer.RealTime() << " s" 
	    << std::endl << std::endl;
  return;
}
