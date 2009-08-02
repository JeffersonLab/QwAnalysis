/**********************************************************\
* File:  Analysis.cc                                       *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-16 15:40>                           *
\**********************************************************/

/*------------------------------------------------------------------------*//*!

 \defgroup QwAnalysis QwAnalysis

 \section myoverview Overview

   Qweak Analysis Framework

   Each subsystem will have a class derived from "VQwSubsystem", and
   will be responsible for decoding of it's own data stream and any
   special event processing required. QwSubsystemArray will handle
   mutiple "VQwSubsystem" objects and one call on the QwSubsystemArray
   will handle all the calls to that method in each subsystem.  Each
   susbsytem will also own the histograms and ntupling functions used
   for its data.

   Once the CODA event are decoded into QwHitCOntainer data structure.
   Routines from QwASCIIEventBuffer will update rcDET and rcDETRegion.
   Then QTR routines can take control over.

*//*-------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*//*!

 \file QwAnalysis.cc

 \brief main(...) function for the QwAnalysis executable

*//*-------------------------------------------------------------------------*/


/*

*/


#include "QwAnalysis.h"
#include "TApplication.h"

#include <boost/shared_ptr.hpp>

#include <sys/time.h>

#include "QwHitContainer.h"



//#include "Qevent.h"
#include "Qoptions.h"
#include "options.h"
#include "QwTrackingTreeRegion.h"
#include "QwTrackingWorker.h"
#include "QwTrackingTree.h"



//Temporary global variables for sub-programs
bool bWriteGlobal = false;
Options opt;

Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
QwASCIIEventBuffer AsciiEvents1;


Bool_t kInQwBatchMode = kFALSE;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 /////  IMPORTANT :- This version of the code decodes actual QWEAK hits from Mark Pitt's data file 62398. Obtain a copy from ///////
  ////  /u/group/qweak/pitt/scratch/data at jlabl1 server in JLAB  ///////////////////////////////////////////////////////////////
   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///
/// \ingroup QwAnalysis
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

  QwSubsystemArrayTracking QwDetectors;//HAndle for the list of VQwSubsystemTracking objects.
  VQwSubsystemTracking * subsystem_tmp;//VQwSubsystemTracking is the top most parent class for Tracking subsystems.




  QwDetectors.push_back(new QwGasElectronMultiplier("R1"));

  QwDetectors.push_back(new QwDriftChamberHDC("R2"));
  QwDetectors.GetSubsystem("R2")->LoadChannelMap("qweak_cosmics_hits.map");//this map file was created by Mark Pitt to run actual QWEAK hits -Rakitha (23/10/2008)

  QwDetectors.push_back(new QwDriftChamberVDC("R3"));
  QwDetectors.GetSubsystem("R3")->LoadChannelMap("qweak_cosmics_hits.map");





  ////---------------------------------------------------------
  //Setting up the QTR  for Region 2
  //code to generate rcDET and rcDETRegion
  std::vector< std::vector< QwDetectorInfo > > detector_info;


  subsystem_tmp= (VQwSubsystemTracking *)QwDetectors.GetSubsystem("R2");
  //this will load the detector geometry for Region 2 into QwDetectorInfo list.
  //detector geometry is stored in "qweak_new.geo".
  subsystem_tmp->LoadQweakGeometry("qweak_new.geo");
  //detector geometry in to the detector_info
  subsystem_tmp->GetDetectorInfo(detector_info);





  //Setting up the QTR  for Region 3
  //code to generate rcDET and rcDETRegion
  subsystem_tmp= (VQwSubsystemTracking *)QwDetectors.GetSubsystem("R3");
  //this will load the detector geometry for Region 3 into QwDetectorInfo list.
  //detector geometry is stored in "qweak_new.geo".
  subsystem_tmp->LoadQweakGeometry("qweak_new.geo");
  //detector geometry in to the detector_info
  subsystem_tmp->GetDetectorInfo(detector_info);



  // Add  geometry details of all the detectors  into the rcDET & rcDETRegion structures.
  AsciiEvents1.InitrcDETRegion(detector_info);


  ////---------------------------------------------------------

  // Disable 'fake' detectors in region 2 cosmic data:
  //   third and fourth plane for region 2 direction X, U, V
  //   (they are trigger channels, always firing on wire 1)
  rcDETRegion[kPackageUp][kRegionID2][kDirectionX]->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionX]->nextsame->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionU]->nextsame->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->nextsame->nextsame->SetInactive();
  rcDETRegion[kPackageUp][kRegionID2][kDirectionV]->nextsame->nextsame->nextsame->SetInactive();



  Qoptions qoptions;
  qoptions.Get((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.options").c_str());
  std::cout << "[QwAnalysis::main] Options loaded" << std::endl; // R3,R2

  QwTrackingWorker *trackingworker = new QwTrackingWorker("qwtrackingworker");
  trackingworker->SetDebugLevel(0);
  //--------------------------------------







  QwDetectors.push_back(new QwTriggerScintillator("TS"));

  QwDetectors.push_back(new QwMainDetector("MD"));
  QwDetectors.GetSubsystem("MD")->LoadChannelMap("maindet_cosmics.map");


  QwDetectors.push_back ( new QwFocalPlaneScanner ( "FPS" ) );
  QwDetectors.GetSubsystem("FPS")->LoadChannelMap ( "scanner_cosmics.map" );
  QwDetectors.GetSubsystem("FPS")->LoadInputParameters("");

  boost::shared_ptr<QwHitContainer> fHitList;
  QwHitContainer grandHitList;

  char *hostname, *session, *tmp;

  for(UInt_t run = (UInt_t) cmdline.GetFirstRun(); run <= (UInt_t) cmdline.GetLastRun(); run++) {

    //  Begin processing for the first run.
    //  Start the timer.
    timer.Start();
    /* Does OnlineAnaysis need several runs? by jhlee */
    if (cmdline.DoOnlineAnalysis()) {
      /* Modify the call below for your ET system, if needed.
	 OpenETStream( ET host name , $SESSION , mode)
	 mode=0: wait forever
	 mode=1: timeout quickly
      */

      hostname = getenv("HOSTNAME");
      session  = getenv("SESSION");
      /* std::cout << "hostname is "<< hostname <<" and session is "<< session << ". " << std::endl; */
      if (hostname==NULL || session==NULL){
	timer.Stop(); /*  don't need the timer, thus Stop; */

	if     (hostname==NULL && session!=NULL) tmp = " \"HOSTNAME\" ";
	else if(hostname!=NULL && session==NULL) tmp = " ET \"SESSION\" ";
	else                                     tmp = " \"HOSTNAME\" and ET \"SESSION\" ";

	std::cerr << "ERROR:  the" << tmp
		  << "variable(s) is(are) not defined in your environment.\n"
		  << "        This is needed to run the online analysis."
		  << std::endl;
	exit(EXIT_FAILURE);
      }
      else {
	std::cout << "Try to open the ET station. " << std::endl;
	if(eventbuffer.OpenETStream(hostname, session, 0) == CODA_ERROR ){
	  std::cerr << "ERROR:  Unable to open the ET station "
		    << run << ".  Moving to the next run.\n"
		    << std::endl;
	  timer.Stop();
	  continue;
	}
      }
    }
    else {
      //  Try to open the data file.
      if (eventbuffer.OpenDataFile(run) == CODA_ERROR){
	//  The data file can't be opened.
	//  Get ready to process the next run.
	std::cerr << "ERROR:  Unable to find data files for run "
		  << run << ".  Moving to the next run.\n"
		  << std::endl;
	timer.Stop();
	continue;
      }
    }

    eventbuffer.ResetControlParameters();

    //     //  Configure database access mode, and load the calibrations
    //     //  from the database.

    create_root_file(run, QwDetectors);

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

    //Creating the TTree
    //Create the Tree
    TTree *HitTree = new TTree("HitTree","Hit event data tree");
    std::vector<Float_t>hitvector;
    Int_t evnum=0;
    //    char buff1[5];
    TString prebase;

    hitvector.reserve(7000); //each hit neeed 7 vector locations
    HitTree->Branch("evnum",&evnum,"evenum/I");
    prebase="";

    QwDetectors.ConstructHistograms();
    ConstructBranchAndVector(HitTree,prebase,hitvector,grandHitList.size());//contruct branches for each wire in the system




    while (eventbuffer.GetEvent() == CODA_OK){
      //  Loop over events in this CODA file
      //  First, do processing of non-physics events...

      //  Now, if this is not a physics event, go back and get
      //  a new event.
      if (! eventbuffer.IsPhysicsEvent()) continue;

      //  Check to see if we want to process this event.
      if      (eventbuffer.GetEventNumber() < cmdline.GetFirstEvent()) continue;
      else if (eventbuffer.GetEventNumber() > cmdline.GetLastEvent()) break;

      if(eventbuffer.GetEventNumber() % 100 == 0) {
	std::cerr << "Number of events processed so far: "
		  << eventbuffer.GetEventNumber() << "\r";
	std::cout << "Number of good partial tracks found: "
		  << trackingworker->ngood << std::endl;
      }

      //  Fill the subsystem objects with their respective data for this event.



      eventbuffer.FillSubsystemData(QwDetectors);



      //Reading the current event number
      evnum=eventbuffer.GetEventNumber();


      QwDetectors.ProcessEvent();

      //QwDetectors.GetSubsystem("R2")->CalculateDriftDistance();


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
      //SaveSubList(grandHitList);//Print a sub set of  hits list to a file
      AsciiEvents1.GetrcDETRegion(grandHitList,evnum);

      // Print hit list
      grandHitList.Print();

      // Process the hit list through the tracking worker (i.e. do track reconstruction)
      cout << "---" << endl;
      timeval start, finish;
      gettimeofday(&start, 0);
      QwEvent *event = trackingworker->ProcessHits(&grandHitList);
      gettimeofday(&finish, 0);

      // Now we can access the event and its partial tracks
      // (e.g. list the partial track in the upper region 2 HDC)
      QwPartialTrack* listoftracks = event->parttrack[kPackageUp][kRegionID2][kTypeDriftHDC];
      for (QwPartialTrack* track = listoftracks;
                           track; track = track->next) {
        cout << *track << endl;
        cout << "Average residual: " << track->CalculateAverageResidual() << endl;
        cout << "in x: " << track->GetTreeLine(kDirectionX)->CalculateAverageResidual() << endl;
        cout << "in u: " << track->GetTreeLine(kDirectionU)->CalculateAverageResidual() << endl;
        cout << "in v: " << track->GetTreeLine(kDirectionV)->CalculateAverageResidual() << endl;
      } // but unfortunately this is still void
      cout << "Tracking stage took " << ((int) finish.tv_usec - (int) start.tv_usec) << " usec" << endl;
      cout << "---" << endl;
      delete listoftracks;
      delete event;





      //Now filling the vector with necessary data
      FillTreeVector(hitvector,grandHitList);


      //  Pass the QwHitContainer to the QwTrackingWorker class

      //  Call the recontruction routines of the QwtrackingWorker class.

      //fill the TTree
      HitTree->Fill();



    }
    std::cout << "Number of events processed so far: "
	      << eventbuffer.GetEventNumber() << std::endl;
    std::cout << "Number of good partial tracks found: "
	      << trackingworker->ngood << std::endl;
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




void PrintInfo(TStopwatch& timer, UInt_t run)
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
  grandHitList.GetSubList_Dir(kRegionID2,kPackageUp,kDirectionV,sublist );//I'm getting sub list of hits with region=2,package=1,direction=X -Rakitha (10/23/2008)
  //grandHitList.GetSubList_Plane(kRegionID2,kPackageUp,3,sublist );//I'm getting sub list of hits with region=2,package=1,wire plane=3  -Rakitha (10/23/2008)
  for (p1=sublist.begin();p1!=sublist.end();p1++){ // (p=grandHitList.GetStartOfHits(kRegionID2,1,kDirectionU);p !=grandHitList.GetEndOfHits(kRegionID2,1,kDirectionU);p++){
    qwhit=p1->GetDetectorID();
    hitTime=p1->GetRawTime();
    //std::cout<<" R "<<qwhit.fRegion<<std::endl;
    fprintf(out_file2," R %d Pkg  %d Pl %d  Dir  %d Wire  %d Hit time %f\n ",qwhit.fRegion,qwhit.fPackage,qwhit.fPlane,qwhit.fDirection,qwhit.fElement,hitTime);
  }



}


void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values,Int_t size){
  TString list="Region/F:Package/F:Direction/F:Plane/F:Element/F:DriftTime/F:DriftDistance/F";

  TString basename,b1,b2,b3;
  std::cout<<" QwHitContainer size "<< size<<std::endl;
  values.clear();
  Int_t subscript;



  //CURRENTLY REGION 2 HAS ONLY 1 PACKAGE AND 12 PLANES. FIRST 6 PLANES HAVE 33 WIRES IN EACH PLANE, WHILE LAST 6 HAS ONLY 2 WIRES PER PLANE
  for (Int_t R=2; R<4;R++){
    b1="R";
    b1+=R;
    b2="_P1";//we have only one pkg for R2 and R3 current dummy state in the qweak code
    b1+=b2;
    for(Int_t P=1;P<7;P++){//for first 6 planes each with 33 wires
      b3="_PLN";
      b3+=P;
      for(Int_t W=1;W<34;W++){ //we have 33 wires on each plane
	basename=prefix;
	basename+=b1;
	basename+=b3;
	basename+="_Wire";
	basename+=W;
	list="DriftTime/F:DriftDistance/F";
	values.push_back(0.0);
	values.push_back(0.0);
	if (R==2)
	  subscript=(P-1)*33+W;
	//we have set of dummy hits from Region 3 which we use the same configuration as current Region 2
	else if(R==3)
	  subscript=33*6+2*6+(P-1)*33+W;//added the total wires in the R2
	//std::cout<<" base name "<< basename<<" subs "<<subscript<<std::endl;
	tree->Branch(basename,&(values[subscript]),list);
      }
    }
    for(Int_t P=1;P<7;P++){//last 6 planes each with 2 wires
       b3="_PLN";
      b3+=(P+6);
      for(Int_t W=1;W<3;W++){ //we have 2 wires on each plane
	basename=prefix;
	basename+=b1;
	basename+=b3;
	basename+="_Wire";
	basename+=W;
	list="DriftTime/F:DriftDistance/F";
	values.push_back(0.0);
	values.push_back(0.0);
	//we have set of dummy hits from Region 3 which we use the same configuration as current Region 2
	if (R==2)
	  subscript=6*33+(P-1)*2+W;
	else if(R==3)
	  subscript=33*6+2*6+6*33+(P-1)*2+W;//added the total wires in the R2

	//std::cout<<" base name "<< basename<<" subs "<<subscript<<std::endl;
	tree->Branch(basename,&(values[subscript]),list);
      }
    }
  }

};
void  FillTreeVector(std::vector<Float_t> &values,QwHitContainer &grandHitList){

  Float_t dtime,ddistance;

  Int_t subscript=0;
  QwDetectorID qwhit;

  std::list<QwHit>::iterator p;

  for (p=grandHitList.begin();p!=grandHitList.end();p++){
    qwhit=p->GetDetectorID();
    dtime=p->GetRawTime();
    ddistance=p->GetDriftDistance();
    if (qwhit.fRegion==2 && qwhit.fPlane < 7)//for plane below 6 in Region 2
      subscript=(qwhit.fPlane-1)*33+qwhit.fElement;
    else if (qwhit.fRegion==2 && qwhit.fPlane > 6)//for plane below 6 above in Region 2
      subscript=6*33+((qwhit.fPlane-1)-6)*2+qwhit.fElement;
    else if (qwhit.fRegion==3 && qwhit.fPlane < 7)//for plane below 6 in Region 3 dummy hits
      subscript=(33*6+2*6)+(qwhit.fPlane-1)*33+qwhit.fElement;
    else if (qwhit.fRegion==3 && qwhit.fPlane > 6)//for plane below 6 above in Region 3 dummy hits
      subscript=(33*6+2*6)+6*33+((qwhit.fPlane-1)-6)*2+qwhit.fElement;

    values[subscript]=dtime;
    values[subscript+1]=ddistance;



  }

};


// 1. reproduce a root file that contains the same structures compared with the existed one by using a different approach.
// 2. introduce a better way to create a ROOT file by using TObject Class in order to fulfill QwTracking reconstruction algorithm demand (by Wouther)
void create_root_file(const UInt_t run , QwSubsystemArrayTracking &tracking_detectors)
{
  // Qweak Requested Time, page 87 in JLab E02-020 :
  // 198 + 223 days = 10104 hours = 606240 mins = 121248 run (if one run is 5 mins)
  // thus 6 digits will be maximum of the possible run number digit.
  // if one run is 15 mins, the total possible run number is 40416.
  char   buffer[24];
  // the size of char buffer is decided by 16 characters (tmp_Qweak_.root + '\0')
  // + 6 digit possible run number + 2 characters (reserved)
  sprintf(buffer, "tmp_Qweak_%d.root", run);

  // auto_ptr<TFile> tempROOTFile (new TFile(buffer, "RECREATE", "Qweak ROOT file with histograms"));
  TFile *tmpROOTFile = new TFile(buffer, "RECREATE", "Qweak ROOT file with histograms");
  if ( tmpROOTFile->IsZombie() ) {
    printf("Error recreating %s file", buffer);
    delete tmpROOTFile; tmpROOTFile=0;
    exit(EXIT_FAILURE);
  }

  TTree *hitEventTree = new TTree("hitEventTree", "hit event tree");
  tracking_detectors.ConstructHistograms();


  /*---------------------------

  To be continued.

  -----------------------------*/


  tmpROOTFile->Write();


  // finally, clean up
  delete hitEventTree; hitEventTree = 0;
  delete tmpROOTFile; tmpROOTFile=0;

};
