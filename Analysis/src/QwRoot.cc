#include "VQwSystem.h"
ClassImp(VQwSystem);
#include "QwRoot.h"
ClassImp(QwRoot);

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>

// ROOT headers
#include <TROOT.h>
#include <TBenchmark.h>

// Qweak headers
#include "QwLog.h"
#include "QwOptions.h"

// Qweak analyzer
#include "VQwAnalyzer.h"

// Qweak dataserver
#include "VQwDataserver.h"

void* QwRootThread (void* arg)
{
  // Threaded running of the command prompt
  // so that other tasks may be performed

  TThread::Printf("New QwRoot thread\n");
  if (! ((TObject*) arg)->InheritsFrom("QwRoot")) {
    TThread::Printf(" Error... QwRoot base class not supplied\n");
    return NULL;
  }
  QwRoot* qr = (QwRoot*) arg;
  qr->Run();
  return NULL;
}

//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
void QwRoot::Run()
{
  // Run online or offline analysis

  gBenchmark = new TBenchmark();
  gBenchmark->Start("QwRootRunThread");

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

  gBenchmark->Show("QwRootRunThread");
  fIsFinished = true;
}

//-----------------------------------------------------------------------------
void QwRoot::Start()
{
  // Start the analysis thread. This will run in the background
  // but will produce to occasional warning or info message

  if (fRootThread) {
    printf(" Warning... deleting old QwRootThread and starting new one,\n");
    fRootThread->Delete();
  }
  printf(" Starting new QwRootThread.\n");
  fRootThread = new TThread("QwRootThread",
			   (void(*) (void*))&(QwRootThread),
			   (void*) this);
  fRootThread->Run();
  return;
}

//-----------------------------------------------------------------------------
void QwRoot::OnlineLoop()
{
  // Loop while there is online data
  while (fDataserver->GetEvent() == 0) { // TODO: CODA_OK

    // Fill subsystem data
    fDataserver->FillSubsystemData(fAnalyzers->GetSubsystemArray());

    // Process this event by analyzer
    fAnalyzers->ProcessEvent();

  } // end of loop over events
}

//-----------------------------------------------------------------------------
void QwRoot::OfflineLoop()
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
    fAnalyzers->OpenRootFile(run);

    Int_t eventnumber_min = gQwOptions.GetIntValuePairFirst("event");
    Int_t eventnumber_max = gQwOptions.GetIntValuePairLast("event");
    while (fDataserver->GetEvent() == 0) { // TODO: CODA_OK

      //  Check to see if we want to process this event.
      Int_t eventnumber = fDataserver->GetEventNumber();
      if      (eventnumber < eventnumber_min) continue;
      else if (eventnumber > eventnumber_max) break;

      // Find idle analyzer object or wait

      // Fill subsystem data
      fDataserver->FillSubsystemData(fAnalyzers->GetSubsystemArray());

      // Spin analyzer thread
//       fAnalyzerThreads = new TThread("QwAnalyzerThread",
// 				(void(*) (void*))&(QwAnalyzerThread),
// 				(void*) fAnalyzers);
//       fAnalyzerThreads->Run();

      //TThread::Ps();
      //TThread::Sleep(1);
      //fAnalyzerThreads->Join();
      //TThread::Ps();

      // Process this event by analyzer
      fAnalyzers->ProcessEvent();

      if (eventnumber % 1000 == 0) {
        QwMessage << "Number of events processed so far: "
                  << eventnumber << QwLog::endl;
      }

    } // end of loop over events

    // Close output for this run
    fAnalyzers->CloseRootFile();

    // Close event buffer
    fDataserver->CloseDataFile();

  } // end of loop over runs
}

//-----------------------------------------------------------------------------
void QwRoot::SetAnalyzer (VQwAnalyzer* analyzer)
{
  if (! analyzer->InheritsFrom("VQwAnalyzer")) {
    QwError << "QwRoot analysis has to inherit from VQwAnalyzer!" << QwLog::endl;
    return;
  }
  fAnalyzers = analyzer;
  return;
}

//-----------------------------------------------------------------------------
void QwRoot::SetDataserver (VQwDataserver* dataserver)
{
  if (! dataserver->InheritsFrom("VQwDataserver")) {
    QwError << "QwRoot dataserver has to inherit from VQwDataserver!" << QwLog::endl;
    return;
  }
  fDataserver = dataserver;
  return;
}

//-----------------------------------------------------------------------------
QwRoot::~QwRoot () {
  if (fDataserver)  delete fDataserver;
  if (fAnalyzers) delete fAnalyzers; // TODO delete all
}
