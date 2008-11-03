
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



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////  IMPORTANT :- This version of the code decode actual QWEAK hits from Mark Pitt's data file 62398. Obtain a copy from ///////
  ////  /u/group/qweak/pitt/scratch/data at jlabl1 server in JLAB  ///////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(Int_t argc,Char_t* argv[])
{
  
  out_file = fopen(FILE_NAME, "wt");//will store grand hit list (only for degubbing) - rakitha
  out_file2 = fopen(FILE_NAME2, "wt");//will store sub hit list (only for degubbing) - rakitha

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
  QwDetectors.GetSubsystem("R2")->LoadChannelMap("qweak_cosmics_hits.map");//this map file was created by Mark Pitt to run actual QWEAK hits -Rakitha (23/10/2008)
  QwDetectors.push_back(new QwDriftChamberVDC("R3"));
  QwDetectors.GetSubsystem("R3")->LoadChannelMap("qweak_cosmics_hits.map");
  QwDetectors.push_back(new QwMainDetector("MD"));
  QwDetectors.GetSubsystem("MD")->LoadChannelMap("maindet_cosmics.map");  

  boost::shared_ptr<QwHitContainer> fHitList;
  QwHitContainer grandHitList;
  





 
        

 


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

       QwDetectors.GetHitList(grandHitList);       

      //sorting the grand hit list
      grandHitList.sort();
      SaveHits(grandHitList);//Print the entire grand hit list to a file 
      SaveSubList(grandHitList);//Print a sub set of  hits list to a file 
      

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


void SaveHits(QwHitContainer & grandHitList)
{

  
  Double_t hitTime;
  QwDetectorID qwhit;
  //std::cout<<"Printing Grand hit list"<<std::endl;
  std::list<QwHit>::iterator p;  
  fprintf(out_file," NEW EVENT \n");
  for (p=grandHitList.begin();p!=grandHitList.end();p++){
    qwhit=p->GetDetectorID();
    hitTime=p->GetRawTime();
    //std::cout<<" R "<<qwhit.fRegion<<" Pkg "<<qwhit.fPackage<<" Dir "<<qwhit.fDirection<<" W "<<qwhit.fElement<<std::endl;
    fprintf(out_file," R %d Pkg  %d Pl %d  Dir  %d Wire  %d Hit time %f\n ",qwhit.fRegion,qwhit.fPackage,qwhit.fPlane,qwhit.fDirection,qwhit.fElement,hitTime);
    
  }
 
}


void SaveSubList(QwHitContainer & grandHitList){

 std::vector<QwHit> sublist;  

  //std::copy(grandHitList.begin(),grandHitList.end(),sublist.begin());
  
  
  Double_t hitTime;
  QwDetectorID qwhit;
  //std::cout<<"Printing Grand hit list"<<std::endl;
  std::list<QwHit>::iterator p;
  std::vector<QwHit>::iterator p1;
  
  fprintf(out_file2," NEW EVENT \n");
  grandHitList.GetSubList_Dir(kRegionID2,1,kDirectionV,sublist );//I'm getting sub list of hits with region=2,package=1,direction=X -Rakitha (10/23/2008)
  //grandHitList.GetSubList_Plane(kRegionID2,1,3,sublist );//I'm getting sub list of hits with region=2,package=1,wire plane=3  -Rakitha (10/23/2008)
  for (p1=sublist.begin();p1!=sublist.end();p1++){ // (p=grandHitList.GetStartOfHits(kRegionID2,1,kDirectionU);p !=grandHitList.GetEndOfHits(kRegionID2,1,kDirectionU);p++){
    qwhit=p1->GetDetectorID();
    hitTime=p1->GetRawTime();
    //std::cout<<" R "<<qwhit.fRegion<<std::endl;
    fprintf(out_file2," R %d Pkg  %d Pl %d  Dir  %d Wire  %d Hit time %f\n ",qwhit.fRegion,qwhit.fPackage,qwhit.fPlane,qwhit.fDirection,qwhit.fElement,hitTime);
  }
  


}
