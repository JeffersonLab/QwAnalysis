/*------------------------------------------------------------------------*//*!

 \defgroup QwAnalysis_ADC QwAnalysis_ADC

 \section myoverview Overview of the Qweak parity data analysis (non-beamline)

    Put something here

 \section MS1 More Stuff

    Put something here

    \subsection MS2 More Substuff

    Put some more stuff here

    \subsection MS3 Even More Substuff

    More stuff


*//*-------------------------------------------------------------------------*/


/*------------------------------------------------------------------------*//*!

 \file QwAnalysis_ADC.cc

 \brief main(...) function for the QwAnalysis_ADC executable

*//*-------------------------------------------------------------------------*/
 
#include "QwAnalysis_ADC.h"
#include "TApplication.h"


#include <boost/shared_ptr.hpp>


//-------------------------------------------------------------------------
//include here to prevent  compiling error!!!
#include "Det.h"
#include "Qset.h"
#include "Qevent.h"
#include "Qoptions.h"
#include "options.h"
#include "treeregion.h"
#include "treedo.h"
#include "tree.h"


Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[1100];
treeregion *rcTreeRegion[kNumPackages][kNumRegions][kNumTypes][kNumDirections];
Options opt;

//--------------------------------------------------------------------------

Bool_t kInQwBatchMode = kFALSE;

///
/// \ingroup QwAnalysis_ADC
int main(Int_t argc,Char_t* argv[])
{
  //either the DISPLAY not set, or JOB_ID defined, we take it as in batch mode
  if (getenv("DISPLAY")==NULL
      ||getenv("JOB_ID")!=NULL) kInQwBatchMode = kTRUE;
  gROOT->SetBatch(kTRUE);

  ///  Fill the search paths for the parameter files; this sets a static
  ///  variable within the QwParameterFile class which will be used by
  ///  all instances.
  ///  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH"))+"/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Parity/prminput");

  ///
  ///  Load the histogram parameter definitions (from parity_hists.txt) into the global 
  ///  histogram helper: QwHistogramHelper
  ///
  gQwHists.LoadHistParamsFromFile("parity_hists.in");

  TStopwatch timer;

  QwCommandLine cmdline;
  cmdline.Parse(argc, argv);

  ///
  /// Instantiate event buffer
  QwEventBuffer QwEvt;

  ///
  /// Instantiate one subsytem for all eight main detectors plus one fully assembled
  /// background detector plus two channels (there are two channels for each fully
  /// assembled detector) for noise setup.   
  QwSubsystemArray QwDetectors;
  QwDetectors.push_back(new QwQuartzBar("MainDetectors"));
  QwDetectors.GetSubsystem("MainDetectors")->LoadChannelMap("qweak_adc.map");


//   QwQuartzBar sum_outer(""), sum_inner(""), diff(""), sum(""), asym("");

//   sum_outer.LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_adc.map");
//   sum_inner.LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_adc.map");
//   sum.LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_adc.map");
//   diff.LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_adc.map");
//   asym.LoadChannelMap(std::string(getenv("QWANALYSIS"))+"/Parity/prminput/qweak_adc.map");

  for(Int_t run = cmdline.GetFirstRun(); run <= cmdline.GetLastRun(); run++){
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

    TString rootfilename=std::string(getenv("QW_ROOTFILES_DIR"))+Form("/Qweak_ADC_%d.root",run);
    std::cout<<" rootfilename="<<rootfilename<<"\n";
    TFile rootfile(rootfilename,
		     "RECREATE","QWeak ROOT file with histograms");



    //  Create the histograms for the QwDriftChamber subsystem object.
    //  We can create a subfolder in the rootfile first, if we want,
    //  and then pass it into the constructor.
    //
    //  To pass a subdirectory named "subdir", we would do:
    //    QwDetectors.at(1)->ConstructHistograms(rootfile.mkdir("subdir"));
    QwDetectors.ConstructHistograms();

    //    TDirectory *asymdir = rootfile.mkdir("qrt");
    //    sum.ConstructHistograms(asymdir,"yield" );
    //    asym.ConstructHistograms(asymdir,"asym" );

    rootfile.cd();
//     TTree *heltree = new TTree("HEL_Tree","Helicity event data tree");
//     std::vector<Double_t> heltreevector;
//     Double_t  evnum = 0.0;
//     heltreevector.reserve(600);
//     heltree->Branch("evnum", &evnum, "evnum/F");
//     ((QwQuartzBar*)QwDetectors.at(0))->ConstructBranchAndVector(heltree, "", heltreevector);


//     TTree *qrttree = new TTree("QRT_Tree","Quartet data tree");
//     std::vector<Double_t> qrttreevector;
//     Double_t  qrtnum = 0.0;
//     qrttreevector.reserve(600);
//     qrttree->Branch("qrtnum", &qrtnum, "qrtnum/F");
//     sum.ConstructBranchAndVector(qrttree, "yield", qrttreevector);
//     asym.ConstructBranchAndVector(qrttree, "asym", qrttreevector);


    while (QwEvt.GetEvent() == CODA_OK){
      //  Loop over events in this CODA file
      //  First, do processing of non-physics events...
      if (QwEvt.IsROCConfigurationEvent()){
	//  Send ROC configuration event data to the subsystem objects.
	QwEvt.FillSubsystemConfigurationData(QwDetectors);
      }

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
      QwDetectors.FillHistograms();

//       if (((QwQuartzBar*)QwDetectors.at(0))->IsGoodEvent()){
// 	evnum = QwEvt.GetEventNumber();
// 	((QwQuartzBar*)QwDetectors.at(0))->FillTreeVector(heltreevector);
// 	heltree->Fill();
//       }


//       //  Quick construction of an asymmetry.
//       //  This basically just takes the pattern as always +--+.
//       //
//       if (QwEvt.GetEventNumber()%4 == 0){
// 	sum_outer = *((QwQuartzBar*)QwDetectors.at(0));
//       } else if (QwEvt.GetEventNumber()%4 == 1){
// 	sum_inner = *((QwQuartzBar*)QwDetectors.at(0));
//       } else if (QwEvt.GetEventNumber()%4 == 2){
// 	sum_inner += *((QwQuartzBar*)QwDetectors.at(0));
//       } else if (QwEvt.GetEventNumber()%4 == 3){
// 	sum_outer += *((QwQuartzBar*)QwDetectors.at(0));
// 	diff.Difference(sum_outer, sum_inner);
// 	sum.Sum(sum_outer, sum_inner);
// 	asym.Ratio(diff, sum);


// 	qrtnum++;

// 	//	sum.FillHistograms();
// 	//	asym.FillHistograms();
// 	if (sum.IsGoodEvent()){
// 	  sum.FillTreeVector(qrttreevector);
// 	  asym.FillTreeVector(qrttreevector);
// 	  qrttree->Fill();
// 	} else {
// 	  std::cerr << "Throw out a bad quartet" << std::endl;
// 	};
//    }




    }
    std::cout << "Number of events processed so far: "
	      << QwEvt.GetEventNumber() << std::endl;
    timer.Stop();

    /*  Write to the root file, being sure to delete the old cycles  *
     *  which were written by Autosave.                              *
     *  Doing this will remove the multiple copies of the ntuples    *
     *  from the root file.                                          */
    rootfile.Write(0,TObject::kOverwrite);

    //  Delete the histograms for the QwDriftChamber subsystem object.
    //  In G0, we had to do:
    //     Hists->ClearHists();//destroy the histogram objects
    //     NTs->ClearNTs(io); //destroy nts according to the i/o flags
    //     CloseAllFiles(io); //close all the output files
    QwDetectors.DeleteHistograms();


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
