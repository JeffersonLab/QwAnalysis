#include "QwTrackingAnalyzer.h"
ClassImp(QwTrackingAnalyzer);

// System headers
#include <cstdlib>

// Qweak headers
#include "Det.h"
#include "QwTypes.h"
#include "QwOptions.h"

// Qweak tracking subsystems
#include "QwSubsystemArrayTracking.h"
#include "QwGasElectronMultiplier.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwTriggerScintillator.h"

// Qweak tracking systems
#include "QwHitContainer.h"
#include "QwTrackingWorker.h"
#include "QwEvent.h"

QwTrackingAnalyzer::QwTrackingAnalyzer (const char* name) : VQwAnalyzer (name)
{
  // Detector array
  fDetectors = new QwSubsystemArrayTracking;

  // Get the detector array for tracking (to avoid the dynamic_cast here)
  QwSubsystemArrayTracking* detectors = GetSubsystemArrayTracking();

  // Region 1 GEM
  detectors->push_back(new QwGasElectronMultiplier("R1"));
  detectors->GetSubsystem("R1")->LoadChannelMap("qweak_R1.map");
  detectors->GetSubsystem("R1")->LoadQweakGeometry("qweak_new.geo");

  // Region 2 HDC
  detectors->push_back(new QwDriftChamberHDC("R2"));
  detectors->GetSubsystem("R2")->LoadChannelMap("qweak_cosmics_hits.map");
  detectors->GetSubsystem("R2")->LoadQweakGeometry("qweak_new.geo");

  // Region 3 VDC
  detectors->push_back(new QwDriftChamberVDC("R3"));
  detectors->GetSubsystem("R3")->LoadChannelMap("TDCtoDL.map");
  detectors->GetSubsystem("R3")->LoadQweakGeometry("qweak_new.geo");

  // Trigger scintillators
  detectors->push_back(new QwTriggerScintillator("TS"));
  detectors->GetSubsystem("TS")->LoadChannelMap("trigscint_cosmics.map");


  // Tracking worker
  fTrackingWorker = new QwTrackingWorker("trackingworker");

  // Hit list
  fHitList = new QwHitContainer;
}

QwTrackingAnalyzer::~QwTrackingAnalyzer()
{
  delete fDetectors;
  delete fHitList;
  delete fTrackingWorker;
}

QwSubsystemArrayTracking* QwTrackingAnalyzer::GetSubsystemArrayTracking()
{
  QwSubsystemArrayTracking* detectors =
    dynamic_cast<QwSubsystemArrayTracking*>(fDetectors);
  if (! detectors) {
    QwError << "QwTrackingAnalyzer does not have a valid subsystem array "
            << "for tracking!" << QwLog::endl;
  }
  return detectors;
}

void QwTrackingAnalyzer::ProcessEvent()
{
  // Get the subsystem array for tracking
  QwSubsystemArrayTracking* detectors = GetSubsystemArrayTracking();
  if (! detectors) return;

  // Get the hit list
  fHitList->Clear();
  detectors->GetHitList(fHitList);
  fHitList->Print();

  // Do the tracking
  if (kTracking)
    QwEvent* event = fTrackingWorker->ProcessHits(detectors,fHitList);

  // Fill the histograms and trees
  if (kHisto) fDetectors->FillHistograms();
  if (kTree)  fRootTree->Fill();
}
