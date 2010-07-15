/*------------------------------------------------------------------------*//*!

 \defgroup QwMoller QwMoller

*//*-------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*//*!

 \file QwMoller.cc

 \ingroup QwMoller

 \brief Moller data analysis

*//*-------------------------------------------------------------------------*/

// C and C++ headers
#include <iostream>
#include <math.h>
#include <sstream>

// Boost math library for random number generation
#include <boost/random.hpp>

// ROOT headers
#include <TROOT.h>
#include <TFile.h>
#include <TTree.h>


// Qweak headers
#include "QwLog.h"
#include "QwOptionsParity.h"
#include "QwEventBuffer.h"
#include "QwHelicity.h"
#include "QwHelicityPattern.h"
#include "QwHistogramHelper.h"
#include "QwSubsystemArrayParity.h"

// Moller headers
#include "QwMollerDetector.h"

//function declarations
void calc_asym(float m, float m0, float p, float p0, float r, float &a, float &da);
void calc_instant_bpol(char, int, QwMollerDetector *);

//global variables
static const int scal_num = 96;
float gscalar[scal_num], gscaler_old[scal_num], gscaler_new[scal_num], gscaler_change[scal_num];

// Debug level
static bool bDebug = false;

// Activate components
static bool bTree = true;
static bool bHisto = true;
static bool bHelicity = true;
static bool bTrueHelicity = false;
static bool bFakeHelicity = false;
static bool bMoller = true;

void setOptions();

/// Multiplet structure ought to be changed so dynamic loading in config file is allowable
static const int kMultiplet = 4;

int main(int argc, char* argv[])
{

  std::cout << std::endl << " -= Moller Run Analyzer Started =-" << std::endl;
  //QwParameterFile::AppendToSearchPath(".");
  gQwOptions.SetConfigFile("moller.flags");

  float average=0, counter=0;

  /// Set the expected options, command line arguments and the configuration filename
  setOptions();
  gQwOptions.SetCommandLine(argc, argv);

  DefineOptionsParity(gQwOptions);
  gQwOptions.Parse();

  std::cout << "bcm_offsets: " << gQwOptions.GetValue<float>("bcm1_offset") << ' ' << gQwOptions.GetValue<float>("bcm2_offset") << ' ' << \
               gQwOptions.GetValue<float>("bcm3_offset") << std::endl;

  std::cout << "bcm_gains: " << gQwOptions.GetValue<float>("bcm1_gain") << ' ' << gQwOptions.GetValue<float>("bcm2_gain") << ' ' << 
               gQwOptions.GetValue<float>("bcm3_gain") << std::endl;
 
  std::cout << "norm_bcm: " << gQwOptions.GetValue<float>("norm_bcm") << std::endl;

  /// Message logging facilities
  gQwLog.InitLogFile("Moller.log", QwLog::kTruncate);
  gQwLog.SetScreenThreshold(QwLog::kMessage);
  gQwLog.SetFileThreshold(QwLog::kDebug);

  ///  Fill the search paths for the parameter files; this sets a static
  ///  variable within the QwParameterFile class which will be used by
  ///  all instances.  The "scratch" directory should be first.
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QW_PRMINPUT"));
  QwParameterFile::AppendToSearchPath(getenv_safe_string("QWANALYSIS") + "/Parity/prminput");

  // Load histogram definitions
  gQwHists.LoadHistParamsFromFile("moller_hists.in");



  // Detector array
  QwSubsystemArrayParity detectors;

  // Moller subsystem
  QwMollerDetector* moll_detect = 0;

  detectors.push_back(new QwMollerDetector("Moller Detector"));
  moll_detect = dynamic_cast<QwMollerDetector*> (detectors.GetSubsystemByName("Moller Detector"));



  if (moll_detect) {
    moll_detect->LoadChannelMap("moller_channels.map");
    moll_detect->SetEventTypeMask(9);
    if (bDebug){
      moll_detect->print();
    }
  } else {
    QwError << "Could not initialize moller detector!" << QwLog::endl;
  }

  // Helicity subsystem
  detectors.push_back(new QwHelicity("Helicity info"));
  detectors.GetSubsystemByName("Helicity info")->LoadChannelMap("moller_helicity.map");
  //detectors.GetSubsystemByName("Helicity info")->LoadInputParameters("");

  // Get the helicity
  QwHelicity* helicity = (QwHelicity*) detectors.GetSubsystemByName("Helicity info");
  detectors.UpdateEventTypeMask();
  // Helicity pattern
  QwHelicityPattern helicitypattern(detectors);

  // Get the SIS7200 channel for debugging
  QwSTR7200_Channel *sampling = 0;

  // Event buffer
  QwEventBuffer eventbuffer;

  std::string fileName;

  /// System setup complete,
  /// Here's where we would loop over all specified runs
  //UInt_t runnumber_min = (UInt_t) gQwOptions.GetIntValuePairFirst("run");
  //UInt_t runnumber_max = (UInt_t) gQwOptions.GetIntValuePairLast("run");
  //std::cout << "RUN: " << runnumber_min << " to " << runnumber_max << std::endl;
  for (UInt_t run = 8; run <= 8; run++) {

    // Data file (input)
    if (gQwOptions.HasValue("input_file")){
      fileName = gQwOptions.GetValue<std::string>("input_file");
    } else if (eventbuffer.OpenDataFile(TString("Moller_") + Form("%ld.log",run),"R")) {
      fileName = TString("Moller_") + Form("%ld.log",run);
    } else {
      QwError << "No Input File Specified!" << std::endl;
      return 1;
    }
  
    // Open Data file
    if (eventbuffer.OpenDataFile(fileName,"R") != CODA_OK) {
      QwError << "Could not open file!" << QwLog::endl;
      return 1;
    }

    // ROOT file output (histograms)
    TString rootfilename = getenv_safe("QW_ROOTFILES")
                         + TString("/Moller_") + Form("%ld.root",run);
    TFile rootfile(rootfilename, "RECREATE", "QWeak ROOT file");
 
    if (bHisto) {
      rootfile.cd();
      detectors.ConstructHistograms(rootfile.mkdir("moller_histo"));
      if (bHelicity) {
        rootfile.cd();
        helicitypattern.ConstructHistograms(rootfile.mkdir("hel_histo"));
      }
    }

    // ROOT file output (expert tree)
    if (bTree) {
      rootfile.cd();
      detectors.ConstructTree(rootfile.mkdir("expert"));
    }


    // ROOT file output (trees)
    TTree *mpstree;
    TTree *heltree;
    Int_t eventnumber;
    std::vector <Double_t> mpsvector;
    std::vector <Double_t> helvector;

    if (bTree) {
      // MPS events
      rootfile.cd();
      mpstree = new TTree("MPS_Tree","MPS event data tree");
      mpsvector.reserve(6000);
      mpstree->Branch("eventnumber",&eventnumber,"eventnumber/F");
      TString dummystr="";
      detectors.ConstructBranchAndVector(mpstree, dummystr, mpsvector);

      rootfile.cd();
      if (bHelicity) {
        // Helicity events (per multiplet)
        rootfile.cd();
        heltree = new TTree("HEL_Tree","Helicity event data tree");
        helvector.reserve(6000);
        TString dummystr="";
        helicitypattern.ConstructBranchAndVector(heltree, dummystr, helvector);
      }
    }



    while (eventbuffer.GetEvent() == CODA_OK) {
      
      /// First, do processing of non-physics events...
      if (eventbuffer.IsROCConfigurationEvent()) {
          std::cout << "it is ROC CONFIG event\n";
          // Send ROC configuration event data to the subsystem objects.
        // eventbuffer.FillSubsystemConfigurationData(detectors);
      }
  
      if (!eventbuffer.IsPhysicsEvent()) continue;
  
      ///  Check to see if we want to process this event.
      Int_t eventnumber = eventbuffer.GetEventNumber();
      //if      (eventnumber < eventnumber_min) continue;
      //else if (eventnumber > eventnumber_max) break;

      /// Add event into the subsystem 
      eventbuffer.FillSubsystemData(detectors);

 
      if (eventbuffer.GetEventType() == 1){
        helicity->SetHelicityReported(0);
      } else if (eventbuffer.GetEventType() == 4){
        helicity->SetHelicityReported(1);
      } else {
        continue;
      }

//      helicity->SetEventPatternPhase(eventnumber, eventnumber / kMultiplet, eventnumber % kMultiplet + 1);
//      helicity->RunPredictor();
      detectors.ProcessEvent();
//      std::cout << "event " << eventbuffer.GetEventNumber() << " event type " << eventbuffer.GetEventType() <<'\n';

      // Helicity pattern
      if (bTrueHelicity)
        helicitypattern.LoadEventData(detectors);
  
  /*  if (g0evok .eqv. .true.) then   

            call fill_scal_hists

            call calc_beamoff_scaler()

            !beam polarization for a time bin
            call calc_instant_bpol(run_number,run_number_chars)

            !beam polarization for each helicity pair
            call calc_bpol_byhpair
            call calc_bpol_byquartet
          end if
*/
      // Print the helicity information
      if (bHelicity && true) {
        std::cout <<"hel: "<< helicity->GetHelicityReported() << std::endl;
        // - actual helicity
        std::cout << (helicity->GetHelicityReported() == 0 ? "-" : helicity->GetHelicityReported() == 1 ? "+" : "?");
        // - delayed helicity
        std::cout << (helicity->GetHelicityActual() == 0 ? "(-) " : helicity->GetHelicityActual() == 1 ? "(+) " : "(?) ");
        if (helicity->GetPhaseNumber() == kMultiplet) {
          std::cout << std::uppercase << std::hex << helicity->GetRandomSeedActual() << ",  \t";
          std::cout << helicity->GetRandomSeedDelayed() << std::dec << std::endl;
        }
      }

    


      // Fill the histograms
      if (bHisto) detectors.FillHistograms();

      // Fill the expert tree
      if (bTree) detectors.FillTree();

      // Fill the tree
      if (bTree) {
        eventnumber = eventbuffer.GetEventNumber();
        detectors.FillTreeVector(mpsvector);
        mpstree->Fill();
      }

      // TODO We need another check here to test for pattern validity.  Right
      // now the first 24 cycles are also added to the histograms.
      if (bHelicity && helicitypattern.IsCompletePattern()) {
        helicitypattern.CalculateAsymmetry();
        if (bHisto) helicitypattern.FillHistograms();
       // helicitypattern.ClearEventData();
      }


      // Periodically print event number
      if ((eventbuffer.GetEventNumber() % 1000 == 0) || eventbuffer.GetEventNumber() % 10000 == 0){
        QwMessage << "Number of events processed so far: " << eventbuffer.GetEventNumber() << QwLog::endl;
      }

      float coincidentUp = moll_detect->GetDataForChannelInModule(0, "coincidences");
      float accidentUp = moll_detect->GetDataForChannelInModule(0, "accidentals");
      
      float coincidentDown = moll_detect->GetDataForChannelInModule(1, "Coincidences");
      float accidentDown = moll_detect->GetDataForChannelInModule(1, "Accidentals");

//      std::cout << "cu:" << coincidentUp << " au:" << accidentUp << " cd:" << coincidentDown << " ad:" << accidentDown << std::endl;

      float a = 0, da = 0;

      float scale = 1;
      /*calc_instant_bpol(0, 0, moll_detect);
      calc_asym(coincidentUp, accidentUp, coincidentDown, accidentDown, scale, a, da); 
  
      std::cout << "result: " << a << '±' << da << std::endl;
      average = (average*counter+a)/(counter+1);
      counter++;*/
    } // end of loop over events

    std::cout << "Last event processed: " << eventbuffer.GetEventNumber() << std::endl;
    std::cout << "avg: " << average << '\n';
    // Close ROOT file
    rootfile.Write(0,TObject::kOverwrite);
    // Delete histograms
    if (bHisto) {
      detectors.DeleteHistograms();
      if (bHelicity) helicitypattern.DeleteHistograms();
    }

    // Close data file and print run summary
    eventbuffer.CloseDataFile();
    eventbuffer.ReportRunSummary();

  } // end of loop over runs

  std::cout << std::endl << " -= Moller Run Analyzer Ended =-" << std::endl << std::endl;
  return 0;
}

void calc_instant_bpol(char run_number, int run_number_chars, QwMollerDetector *detector){
  
  float qm[3],qp[3],bcurm,bcurp,bcur;
  float a,da,bp,dbp,r,t;
  
  float *scal = detector->GetRawChannelArray();

/*  for(int i = 0; i < 96; i++){
    std::cout << scal[i] << ' ';
  }
  std::cout << std::endl;
*/
  float timebin = gQwOptions.GetValue<float>("timebin");
  
  
/*  bool first = true;
  for(int i = 0; i < 96; i++){
    scal[i] = scal[i] + gscaler_change[i];
  }
*/
  float bcm1_gain = gQwOptions.GetValue<float>("bcm1_gain");
  float bcm2_gain = gQwOptions.GetValue<float>("bcm2_gain");
  float bcm3_gain = gQwOptions.GetValue<float>("bcm3_gain");

  float bcm1_offset = gQwOptions.GetValue<float>("bcm1_offset");
  float bcm2_offset = gQwOptions.GetValue<float>("bcm2_offset");
  float bcm3_offset = gQwOptions.GetValue<float>("bcm3_offset");

  float pol_factor =  gQwOptions.GetValue<float>("pol_factor");

  int bcm1_index = gQwOptions.GetValue<float>("bcm1_index");
  int bcm2_index = gQwOptions.GetValue<float>("bcm2_index");
  int bcm3_index = gQwOptions.GetValue<float>("bcm3_index");

  int norm_bcm = gQwOptions.GetValue<float>("norm_bcm");

  int clock_index = gQwOptions.GetValue<float>("clock_index");

  //Do nothing if time since last calculation is less than timebin
  if ((scal[clock_index]+scal[clock_index+32])/1e6 < timebin) return;

  float bcur_limit =   gQwOptions.GetValue<float>("bcur_limit");
  // calculation for last timebin

  // charge
  qm[1] = bcm1_gain * (scal[bcm1_index] - scal[clock_index] * bcm1_offset);
  qm[2] = bcm2_gain * (scal[bcm2_index] - scal[clock_index] * bcm2_offset);
  qm[3] = bcm3_gain * (scal[bcm3_index] - scal[clock_index] * bcm3_offset);

  qp[1] = bcm1_gain * (scal[bcm1_index+32] - scal[clock_index+32] * bcm1_offset);
  qp[2] = bcm2_gain * (scal[bcm2_index+32] - scal[clock_index+32] * bcm2_offset);
  qp[3] = bcm3_gain * (scal[bcm3_index+32] - scal[clock_index+32] * bcm3_offset);

  // beam currents
  bcurm = qm[norm_bcm]/scal[clock_index]*1e6;
  bcurp = qp[norm_bcm]/scal[clock_index+32]*1e6;
  bcur = (qm[norm_bcm] + qp[norm_bcm])/(scal[clock_index] + scal[clock_index+32])*1e6;

  // require both beam currents to be above limit 
  if (bcurm > bcur_limit && bcurp > bcur_limit){

    //normalization factor
    r = qm[norm_bcm]/qp[norm_bcm];
  
    //get asymmetry
    size_t i=7;
    size_t j=10;
    calc_asym(scal[i],scal[j],scal[i+32],scal[j+32],r,a,da);
    
    if (da != 0) {
      //beam polarization
      bp = a * pol_factor;
      dbp = da/sqrt(1-a*a)*pol_factor;
    
      //time
//      float t = gscaler[clock_index]+gscaler[clock_index+32] - (scal[clock_index] + scal[clock_index+32])/2.0;
  //    t = t/1e6;
    
      //output
/*      std::stringstream out;
      out << run_number;
      std::string outfile = "DAT/bpol_" + out.str() + ".dat"; 
      std::ofstream ofile(outfile.c_str(), std::fstream::app);
      ofile << bcur << bp << dbp;
      ofile.close();
  */  }
  }

  return;
}


void calc_asym(float m, float m0, float p, float p0, float r, float &a, float &da){

    float dm = m-m0;
    float dp = p-p0;

    float diff = dm - r * dp;
    float sum  = dm + r * dp;
    float sum2 = dm * dp * sum * sum;

    if (sum > 0 && sum2 > 0){
        a = diff/sum;
        da = sqrt( ((1.0 - a)*(1.0 - a)*(m + m0) + r*r*(1.0 + a)*(1.0 + a)*(p + p0))/(sum*sum));
    } else {
        a = 0;
        da = 0;
    }

}

void setOptions(){
  gQwOptions.AddOptions()("EXPERIMENT", po::value<std::string>(), "Name of the experiment");
  gQwOptions.AddOptions()("input_file,f", po::value<std::string>(), "Input file to be used");
  gQwOptions.AddOptions()("input_directory,d", po::value<std::string>(), "Directory to be searched for input files, default is current");
  gQwOptions.AddOptions()("output_directory,o", po::value<std::string>(), "Directory where output files are saved");
  gQwOptions.AddOptions()("pol_factor", po::value<float>(), "This needs to be explained to me");
  gQwOptions.AddOptions()("bcm1_offset", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm2_offset", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm3_offset", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm1_gain", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm2_gain", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm3_gain", po::value<float>(), " ");
  gQwOptions.AddOptions()("norm_bcm", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcm1_index", po::value<int>(), " ");
  gQwOptions.AddOptions()("bcm2_index", po::value<int>(), " ");
  gQwOptions.AddOptions()("bcm3_index", po::value<int>(), " ");
  gQwOptions.AddOptions()("clock_index", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcur_limit", po::value<float>(), " ");
  gQwOptions.AddOptions()("timebin", po::value<float>(), " ");
  gQwOptions.AddOptions()("bcur_units", po::value<std::string>(), " ");
  gQwOptions.AddOptions()("helicity_pattern", po::value<int>()->default_value(4), "Length of the helicity window, should be 4, 8, or 16");
}
