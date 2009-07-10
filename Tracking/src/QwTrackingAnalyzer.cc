#include "QwTrackingAnalyzer.h"

#include <cstdlib>

// Qweak Tracking headers
#include "QwTypes.h"
#include "Det.h"
#include "QwTrackingTreeRegion.h"
#include "options.h"
using namespace QwTracking;

// Qweak tracking worker
#include "QwTrackingWorker.h"

// rcDETRegion is a 3D array of lists (yes!)
Det* rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
// rcTreeRegion is a 4D array of search trees
QwTrackingTreeRegion *rcTreeRegion[kNumPackages][kNumRegions][kNumTypes][kNumDirections];
// rcDET is a list of detectors
Det rcDET[NDetMax];
// opt is a global options object (yuk!)
Options opt;


//ClassImp(QwTrackingAnalyzer)

QwTrackingAnalyzer::QwTrackingAnalyzer (const char* name) : VQwAnalyzer (name)
{
  // Read geometry
  fGeometry.FillDetec((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.geo").c_str());

  // Read options
  fOptions.Get((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.options").c_str());

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
