#include "QwTrackingAnalyzer.h"

#include <cstdlib>

// Qweak Tracking headers
#include "QwTypes.h"
#include "QwTrackingTreeRegion.h"

// Qweak tracking worker
#include "QwTrackingWorker.h"

QwTrackingAnalyzer::QwTrackingAnalyzer (const char* name) : VQwAnalyzer (name)
{
  // Create the tracking worker
  //fTrackingWorker = new QwTrackingWorker("trackingworker");

  // Test histogram
  fHisto1 = new TH1F("fHisto1", "fHisto1", 100, 0.0, 100000.0);
}

QwTrackingAnalyzer::~QwTrackingAnalyzer()
{
  //delete fTrackingWorker;
  delete fHisto1;
}

void QwTrackingAnalyzer::Process()
{
}
