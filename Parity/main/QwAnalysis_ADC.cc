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
#include "QwHelicityPattern.h"
#include "QwHelicity.h"
#include "TApplication.h"
#include <boost/shared_ptr.hpp>

Bool_t kInQwBatchMode = kFALSE;

// Multiplet structure
static const int kMultiplet = 4;

// Debug level
//static bool bDebug = true;

// Activate components
static bool bTree = true;
static bool bHelicity=true;

///
/// \ingroup QwAnalysis_ADC
int main(Int_t argc,Char_t* argv[])
{
    /// First, we set the command line arguments and the configuration filename,
    /// and we define the options that can be used in them (using QwOptions).
    gQwOptions.SetCommandLine(argc, argv);
    gQwOptions.SetConfigFile("qwanalysis_adc.conf");
    // Define the command line options
    DefineOptionsParity(gQwOptions);


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

    ///
    /// Instantiate event buffer
    QwEventBuffer eventbuffer;

    ///
    /// Instantiate one subsytem for all eight main detectors Bar1-Bar8, plus one fully assembled
    /// background detector Bar0 (there are two channels for each fully assembled detector)
    /// plus 3 additional diagnostic detector channels AnciD1-AnciD3 for noise setup.
    QwSubsystemArrayParity detectors;
    detectors.push_back(new QwMainCerenkovDetector("MainDetectors"));
    detectors.GetSubsystem("MainDetectors")->LoadChannelMap("qweak_adc.map");
    //detectors.GetSubsystem("Main detector")->             LoadInputParameters("qweak_pedestal.map");
    ///
    ///Specifies the same helicity pattern used by all subsystems
    ///to calculate asymmetries. The pattern is defined in the
    ///QwHelicityPattern class.
    if (bHelicity) {
        detectors.push_back(new QwHelicity("Helicity info"));
        detectors.GetSubsystem("Helicity info")->LoadChannelMap("qweak_helicity.map");
        detectors.GetSubsystem("Helicity info")->LoadInputParameters("");
    }
    QwHelicityPattern helicitypattern(detectors);//multiplet size is set within the QwHelicityPattern class

    // Get the helicity
    QwHelicity* helicity = (QwHelicity*) detectors.GetSubsystem("Helicity info");

//   QwMainCerenkovDetector sum_outer(""), sum_inner(""), diff(""), sum(""), asym("");

//   sum_outer.LoadChannelMap("qweak_adc.map");
//   sum_inner.LoadChannelMap("qweak_adc.map");
//   sum.LoadChannelMap("qweak_adc.map");
//   diff.LoadChannelMap("qweak_adc.map");
//   asym.LoadChannelMap("qweak_adc.map");

    Double_t evnum=0.0;

    // Loop over all runs
    UInt_t runnumber_min = (UInt_t) gQwOptions.GetIntValuePairFirst("run");
    UInt_t runnumber_max = (UInt_t) gQwOptions.GetIntValuePairLast("run");
    for (UInt_t run  = runnumber_min;
                run <= runnumber_max;
                run++) {

        //  Begin processing for the first run.
        //  Start the timer.
        timer.Start();

        //  Try to open the data file.
        if (eventbuffer.OpenDataFile(run) != CODA_OK) {
            //  The data file can't be opened.
            //  Get ready to process the next run.
            std::cerr << "ERROR:  Unable to find data files for run "
            << run << ".  Moving to the next run.\n"
            << std::endl;
            timer.Stop();
            continue;
        }
        eventbuffer.ResetControlParameters();
        //  Open the data files and root file
        //    OpenAllFiles(io, run);

        TString rootfilename=std::string(getenv("QW_ROOTFILES_DIR"))+Form("/Qweak_ADC_%d.root",run);
        std::cout<<" rootfilename="<<rootfilename<<"\n";
        TFile rootfile(rootfilename,"RECREATE","QWeak ROOT file");


        //  To pass a subdirectory named "subdir", we would do:
        //    detectors.at(1)->ConstructHistograms(rootfile.mkdir("subdir"));
            rootfile.cd();
            detectors.ConstructHistograms(rootfile.mkdir("mps_histo"));
            if (bHelicity) {
                rootfile.cd();
                helicitypattern.ConstructHistograms(rootfile.mkdir("hel_histo"));
            }


        TTree *mpstree;
        TTree *heltree;

        std::vector <Double_t> mpsvector;
        std::vector <Double_t> helvector;

        if (bTree) {
            rootfile.cd();
            mpstree=new TTree("MPS_Tree","MPS event data tree");
            mpsvector.reserve(6000);
            mpstree->Branch("evnum",&evnum,"evnum/D");
            TString dummystr="";

            ((QwMainCerenkovDetector*)detectors.GetSubsystem("MainDetectors"))->ConstructBranchAndVector(mpstree, dummystr, mpsvector);
	  ((QwHelicity*)detectors.GetSubsystem("Helicity info"))->ConstructBranchAndVector(mpstree, dummystr, mpsvector);
            rootfile.cd();
            if (bHelicity) {
                rootfile.cd();
                heltree = new TTree("HEL_Tree","Helicity event data tree");
                helvector.reserve(6000);
                TString dummystr="";
                helicitypattern.ConstructBranchAndVector(heltree, dummystr, helvector);
            }
        }

        //    TDirectory *asymdir = rootfile.mkdir("qrt");
        //    sum.ConstructHistograms(asymdir,"yield" );
        //    asym.ConstructHistograms(asymdir,"asym" );

//     TTree *heltree = new TTree("HEL_Tree","Helicity event data tree");
//     std::vector<Double_t> heltreevector;
//     Double_t  evnum = 0.0;
//     heltreevector.reserve(600);
//     heltree->Branch("evnum", &evnum, "evnum/F");
//     ((QwMainCerenkovDetector*)detectors.at(0))->ConstructBranchAndVector(heltree, "", heltreevector);


//     TTree *qrttree = new TTree("QRT_Tree","Quartet data tree");
//     std::vector<Double_t> qrttreevector;
//     Double_t  qrtnum = 0.0;
//     qrttreevector.reserve(600);
//     qrttree->Branch("qrtnum", &qrtnum, "qrtnum/F");
//     sum.ConstructBranchAndVector(qrttree, "yield", qrttreevector);
//     asym.ConstructBranchAndVector(qrttree, "asym", qrttreevector);


        Int_t eventnumber = -1;
        Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
        Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");
        while (eventbuffer.GetEvent() == CODA_OK) {

            if (eventbuffer.IsROCConfigurationEvent()) {
                eventbuffer.FillSubsystemConfigurationData(detectors);
            }

            // Now, if this is not a physics event, go back and get a new event.
            if (! eventbuffer.IsPhysicsEvent()) continue;

            // Check to see if we want to process this event.
            eventnumber = eventbuffer.GetEventNumber();
            if      (eventnumber < eventnumber_min) continue;
            else if (eventnumber > eventnumber_max) break;

            eventbuffer.FillSubsystemData(detectors);
            detectors.ProcessEvent();

            // Helicity pattern
            if (bHelicity)
                helicitypattern.LoadEventData(detectors);

            // Check for helicity validity (TODO I'd prefer to use kUndefinedHelicity)
            if (bHelicity && helicity->GetHelicityDelayed() == -9999) continue;

            // Fill the histograms
            detectors.FillHistograms();

            // Fill the detector trees
            if (bTree) {
                evnum = eventbuffer.GetEventNumber();
                detectors.FillTreeVector(mpsvector);
                mpstree->Fill();
            }
            // Fill the helicity tree
            if (bHelicity && helicitypattern.IsCompletePattern()) {
                helicitypattern.CalculateAsymmetry();
                helicitypattern.FillHistograms();
                if (bTree) {
                    helicitypattern.FillTreeVector(helvector);
                    heltree->Fill();
                }
                helicitypattern.ClearEventData();
            }


            if (eventbuffer.GetEventNumber()%1000==0) {
                std::cerr << "Number of events processed so far: "
                << eventbuffer.GetEventNumber() << "\r";
            }

            //  Fill the subsystem objects with their respective data for this event.
            //eventbuffer.FillSubsystemData(detectors);


            //  Fill the histograms for the QwDriftChamber subsystem object.
            //detectors.FillHistograms();

//       if (((QwMainCerenkovDetector*)detectors.at(0))->IsGoodEvent()){
// 	evnum = eventbuffer.GetEventNumber();
// 	((QwMainCerenkovDetector*)detectors.at(0))->FillTreeVector(heltreevector);
// 	heltree->Fill();
//       }


//       //  Quick construction of an asymmetry.
//       //  This basically just takes the pattern as always +--+.
//       //
//       if (eventbuffer.GetEventNumber()%4 == 0){
// 	sum_outer = *((QwMainCerenkovDetector*)detectors.at(0));
//       } else if (eventbuffer.GetEventNumber()%4 == 1){
// 	sum_inner = *((QwMainCerenkovDetector*)detectors.at(0));
//       } else if (eventbuffer.GetEventNumber()%4 == 2){
// 	sum_inner += *((QwMainCerenkovDetector*)detectors.at(0));
//       } else if (eventbuffer.GetEventNumber()%4 == 3){
// 	sum_outer += *((QwMainCerenkovDetector*)detectors.at(0));
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
        << eventbuffer.GetEventNumber() << std::endl;
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
        detectors.DeleteHistograms();
	helicitypattern.DeleteHistograms();


        eventbuffer.CloseDataFile();
        eventbuffer.ReportRunSummary();


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


void PrintInfo(TStopwatch& timer, Int_t run) {
    std::cout << "Analysis of run "  << run << std::endl
    << "CPU time used:  "  << timer.CpuTime() << " s"
    << std::endl
    << "Real time used: " << timer.RealTime() << " s"
    << std::endl << std::endl;
    return;
}
