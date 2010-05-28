#include "QwTrackingAnalyzer.h"

#include <cstdlib>

// Qweak Tracking headers
#include "QwTypes.h"
#include "Det.h"
#include "QwTrackingTreeRegion.h"

// Qweak tracking worker
#include "QwTrackingWorker.h"


//ClassImp(QwTrackingAnalyzer)

QwTrackingAnalyzer::QwTrackingAnalyzer (const char* name) : VQwAnalyzer (name)
{
  // Read geometry
  fGeometry.FillDetec((getenv_safe_string("QWANALYSIS") + "/Tracking/prminput/qweak.geo").c_str());

  // Read options
  fOptions.Get((getenv_safe_string("QWANALYSIS") + "/Tracking/prminput/qweak.options").c_str());

  // Create the tracking worker
  fTrackingWorker = new QwTrackingWorker("trackingworker");

  // Test histogram
  fHisto1 = new TH1F("fHisto1", "fHisto1", 100, 0.0, 100000.0);
}

QwTrackingAnalyzer::~QwTrackingAnalyzer()
{
  delete fTrackingWorker;
  delete fHisto1;
}

void QwTrackingAnalyzer::Process()
{
  Event* event = fTrackingWorker->ProcessHits(*fHitList);
  QwTrackingTreeLine* tl = event->treeline[kPackageUp][kRegionID2][kTypeDriftHDC][kDirectionX];
  if (tl)
    fHisto1->Fill(tl->chi);
}
