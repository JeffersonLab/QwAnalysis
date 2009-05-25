#include "QwControl.h"

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>
using namespace std;

// ROOT headers
#include "TROOT.h"

// Qweak headers
#include "QwRoot.h"

// Qweak analyzers
#include "VQwAnalyzer.h"
#include "QwTrackingAnalyzer.h"

// Qweak dataservers
#include "VQwDataserver.h"
#include "QwTrackingDataserver.h"


// Global pointers
QwRoot* gQwRoot = NULL;
QwControl* gQwControl = NULL;
VQwAnalyzer* gQwAnalyzer = NULL;
VQwDataserver* gQwDataserver = NULL;

QwControl* QwControl::fExists = NULL;  // Pointer to this interface


ClassImp(QwControl)

//--------------------------------------------------------------------------
QwControl::QwControl (const char* appClassName, int* argc, char** argv,
		      void* options, int numOptions, bool noLogo) :
  TRint (appClassName, argc, argv, options, numOptions, noLogo )
{
  gQwControl = this;

  fIsBatch = false;	// Start Qw-Root interface
  fIsOnline = true;	// Use online decoded data stream

  // Command line options
  for (int i = 1; i < *argc; i++) {
    if (strcmp("--batch", argv[i]) == 0) fIsBatch = true;
    else if (strcmp("--offline", argv[i]) == 0) fIsOnline = false;
  }

  if (fIsOnline) {
    std::cout << "Warning: online analysis not yet implemented." << std::endl;
    fIsOnline = false;
  }

  // Qw-Root command prompt
  SetPrompt("Qw-Root:%d> ");

  // Qw-Root interface
  gQwRoot = new QwRoot("QwRoot", fIsBatch);
  gQwRoot->SetIsOnline(fIsOnline);

  // Prepare data sources
  if (fIsOnline) {
    // Set up online data stream
    // (start server thread?)
  } else {
    // Set up offline data stream
    // (read file list, open ROOT tree?)
  }

  // Pointer to self
  fExists = this;
}

//---------------------------------------------------------------------------
QwControl::~QwControl()
{
  // Delete global objects
  if (fExists == this) {
    fExists = NULL;
    if (gQwRoot) delete gQwRoot;
  }
}

void QwControl::StartDataserver()
{
  // Create the dataserver
  gQwDataserver = gQwRoot->CreateDataserver("QwTrackingDataserver");
  if (! gQwDataserver)
    return;
  gQwRoot->SetDataserver(gQwDataserver);
}

void QwControl::StartAnalyzer()
{
  // Create the analyzer
  gQwAnalyzer = gQwRoot->CreateAnalyzer("QwTrackingAnalyzer");
  if (! gQwAnalyzer)
    return;
  gQwRoot->SetAnalyzer(gQwAnalyzer);

  // Start the analyzer
  gQwRoot->Start();

  // When the analyzer has finished
  if (fIsBatch) {
    while (! gQwRoot->IsFinished()) sleep(1);
    exit(0);
  }
  else{
    Run();
  }
}
