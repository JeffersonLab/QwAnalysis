#include "QwControl.h"

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>
using std::cout; using std::endl;

// ROOT headers
#include <TROOT.h>

// Qweak headers
#include "QwRoot.h"
#include "QwOptions.h"
#include "QwHistogramHelper.h"
#include "QwParameterFile.h"

// Qweak analyzer
#include "VQwAnalyzer.h"

// Qweak dataserver
#include "VQwDataserver.h"


// Global pointers
QwRoot* gQwRoot = NULL;
QwControl* gQwControl = NULL;
VQwAnalyzer* gQwAnalyzer = NULL;
VQwDataserver* gQwDataserver = NULL;

QwControl* QwControl::fExists = NULL;  // Pointer to this interface


//--------------------------------------------------------------------------
QwControl::QwControl (const char* appClassName, int* argc, char** argv,
		      void* options, int numOptions, bool noLogo) :
  TRint (appClassName, argc, argv, options, numOptions, noLogo )
{
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

void QwControl::StartDataserver(VQwDataserver* dataserver)
{
  // Create the dataserver
  gQwDataserver = dataserver;
  if (! gQwDataserver)
    return;
  gQwRoot->SetDataserver(gQwDataserver);
}

void QwControl::StartAnalyzer(VQwAnalyzer* analyzer)
{
  // Create the analyzer
  gQwAnalyzer = analyzer;
  if (! gQwAnalyzer)
    return;
  gQwRoot->SetAnalyzer(gQwAnalyzer);

  // Start the analyzer
  gQwRoot->Start();

  // When the analyzer has finished
  if (fIsBatch) {
    while (! gQwRoot->IsFinished()) sleep(1);
    exit(0);
  } else {
    Run();
  }
}
