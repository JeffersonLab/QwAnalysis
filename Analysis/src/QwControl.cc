#include "VQwSystem.h"
ClassImp(VQwSystem);
#include "QwControl.h"
ClassImp(QwControl);

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>

// ROOT headers
#include <TROOT.h>
#include <TBenchmark.h>

// Qweak headers
#include "QwLog.h"
#include "QwOptions.h"
#include "QwParameterFile.h"
#include "QwHistogramHelper.h"

// Qweak analyzer
#include "VQwAnalyzer.h"

// Qweak dataserver
#include "VQwDataserver.h"

// Global pointers
QwControl* gQwControl = NULL;
VQwAnalyzer* gQwAnalyzer = NULL;
VQwDataserver* gQwDataserver = NULL;


//---------------------------------------------------------------------------
/**
 * Start a QwControl thread
 * @param arg QwControl object pointer
 * @return Null
 */
void* QwControlThread (void* arg)
{
  // Threaded running of the command prompt
  // so that other tasks may be performed
  TThread::Printf("New QwControl thread\n");
  if (! ((TObject*) arg)->InheritsFrom("QwControl")) {
    TThread::Printf(" Error... QwControl base class not supplied\n");
    return NULL;
  }
  QwControl* qr = (QwControl*) arg;
  qr->Run();
  return NULL;
}

//---------------------------------------------------------------------------
/**
 * Start a VQwAnalyzer thread
 * @param arg QwControl object pointer
 * @return Null
 */
void* QwAnalyzerThread (void* arg)
{
  // Threaded running of the analyzer
  // so that other tasks may be performed
  TThread::Printf("New VQwAnalyzer thread\n");
  if (! ((TObject*) arg)->InheritsFrom("VQwAnalyzer")) {
    TThread::Printf(" Error... VQwAnalyzer base class not supplied\n");
    return NULL;
  }
  VQwAnalyzer* ana = (VQwAnalyzer*) arg;
  ana->ProcessEvent();
  TThread::Printf("VQwAnalyzer thread done\n");
  return NULL;
}

//-----------------------------------------------------------------------------
/**
 * Constructor
 * @param name Name of this system
 */
QwControl::QwControl (const char* name, int* argc, char** argv): VQwSystem(name)
{
  // Store global pointer
  if (gQwControl) {
    QwError << "Only one QwControl object can be started!" << QwLog::endl;
    exit(0);
  }
  gQwControl = this;

  /// Set the command line arguments and the configuration filename
  gQwOptions.SetCommandLine(*argc, argv);
  gQwOptions.SetConfigFile("qwanalysis.conf");

  /// Message logging facilities
  gQwLog.InitLogFile("QwAnalysis.log");
  gQwLog.SetScreenThreshold(QwLog::kMessage);
  gQwLog.SetFileThreshold(QwLog::kMessage);

  /// Load the histogram definitions
  gQwHists.LoadHistParamsFromFile(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/cosmics_hists.in");

  /// Fill the search paths for the parameter files
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH")) + "/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS")) + "/Tracking/prminput");

  // Default options
  fIsBatch = false;	// Start Qw-Root interface
  fIsOnline = true;	// Use online decoded data stream

  // Command line options (TODO use gQwOptions)
  for (int i = 1; i < *argc; i++) {
    if (strcmp("--batch", argv[i]) == 0) fIsBatch = true;
    else if (strcmp("--offline", argv[i]) == 0) fIsOnline = false;
  }

  if (fIsOnline) {
    std::cout << "Warning: online analysis not yet implemented." << std::endl;
    fIsOnline = false;
  }
};

//-----------------------------------------------------------------------------
/**
 * Destructor to delete the dataserver and analyzers
 */
QwControl::~QwControl () {
  if (fDataserver) delete fDataserver;
  if (fAnalyzers) delete[] fAnalyzers;
}

//---------------------------------------------------------------------------
void QwControl::StartControl()
{
  // Start the control thread
  if (fControlThread) {
    QwWarning << "Existing control thread was deleted." << QwLog::endl;
    fControlThread->Delete();
  }
  QwMessage << "Starting new control thread." << QwLog::endl;
  fControlThread = new TThread("QwControlThread", (void(*)(void*))&(QwControlThread), (void*)this);
  fControlThread->Run();
  return;
}

//---------------------------------------------------------------------------
void QwControl::Run()
{
  // Run online or offline analysis

  gBenchmark = new TBenchmark();
  gBenchmark->Start("QwControlRunThread");

  // Set up analysis parameters from config file
  //fAnalysis->FileConfig (fAnalysisSetup);

  if (fIsOnline) {
  // Online

    // Connect to CODA data stream
    //fDataserver->Connect();

    // Loop while data
    OnlineLoop();

  } else {
  // Offline

    // Loop while data
    OfflineLoop();
  }

  gBenchmark->Show("QwControlRunThread");
  fIsFinished = true;
}

//-----------------------------------------------------------------------------
void QwControl::OnlineLoop()
{
  // Loop while there is online data
  while (fDataserver->GetEvent() == 0) { // TODO: CODA_OK

    // Fill subsystem data
    fDataserver->FillSubsystemData(fAnalyzers[0]->GetSubsystemArray());

    // Process this event by analyzer
    fAnalyzers[0]->ProcessEvent();

  } // end of loop over events
}

//-----------------------------------------------------------------------------
void QwControl::OfflineLoop()
{
  // Loop over all requested runs
  Int_t runnumber_min = gQwOptions.GetIntValuePairFirst("run");
  Int_t runnumber_max = gQwOptions.GetIntValuePairLast("run");
  for (Int_t run  = runnumber_min;
             run <= runnumber_max;
             run++) {

    // Print run number
    QwMessage << "Processing run " << run << QwLog::endl;

    // Try to open the data file
    fDataserver->GetRun(run);
    // TODO Check return value

    // Open output for this run
    fAnalyzers[0]->OpenRootFile(run);

    Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
    Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");
    while (fDataserver->GetEvent() == 0) { // TODO: CODA_OK

      //  Check to see if we want to process this event.
      Int_t eventnumber = fDataserver->GetEventNumber();
      if      (eventnumber < eventnumber_min) continue;
      else if (eventnumber > eventnumber_max) break;

      // Find idle analyzer object or wait

      // Fill subsystem data
      fDataserver->FillSubsystemData(fAnalyzers[0]->GetSubsystemArray());

      // Spin analyzer thread
//       fAnalyzerThreads = new TThread("QwAnalyzerThread",
// 				(void(*) (void*))&(QwAnalyzerThread),
// 				(void*) fAnalyzers[0]);
//       fAnalyzerThreads->Run();

      //TThread::Ps();
      //TThread::Sleep(1);
      //fAnalyzerThreads->Join();
      //TThread::Ps();

      // Process this event by analyzer
      fAnalyzers[0]->ProcessEvent();

      if (eventnumber % 1000 == 0) {
        QwMessage << "Number of events processed so far: "
                  << eventnumber << QwLog::endl;
      }

    } // end of loop over events

    // Close output for this run
    fAnalyzers[0]->CloseRootFile();

    // Close event buffer
    fDataserver->CloseDataFile();

  } // end of loop over runs
}

//-----------------------------------------------------------------------------
void QwControl::SetAnalyzer (VQwAnalyzer* analyzer)
{
  if (! analyzer->InheritsFrom("VQwAnalyzer")) {
    QwError << "QwControl analysis has to inherit from VQwAnalyzer!" << QwLog::endl;
    return;
  }
  fAnalyzers[0] = analyzer;
  return;
}

//-----------------------------------------------------------------------------
void QwControl::SetDataserver (VQwDataserver* dataserver)
{
  if (! dataserver->InheritsFrom("VQwDataserver")) {
    QwError << "QwControl dataserver has to inherit from VQwDataserver!" << QwLog::endl;
    return;
  }
  fDataserver = dataserver;
  return;
}

//-----------------------------------------------------------------------------
void QwControl::StartDataserver(VQwDataserver* dataserver)
{
  // Create the dataserver
  gQwDataserver = dataserver;
  if (! gQwDataserver)
    return;
  gQwControl->SetDataserver(gQwDataserver);
}

//-----------------------------------------------------------------------------
void QwControl::StartAnalyzer(VQwAnalyzer* analyzer)
{
  // Create the analyzer
  gQwAnalyzer = analyzer;
  if (! gQwAnalyzer)
    return;
  gQwControl->SetAnalyzer(gQwAnalyzer);
}
