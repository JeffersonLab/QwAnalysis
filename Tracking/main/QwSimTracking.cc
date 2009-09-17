/*------------------------------------------------------------------------*//*!

 \file QwSimTracking.cc \ingroup QwTrackingAnl

 \brief Example of the QwTreeEventBuffer class to read QweakSimG4 events

 This example illustrates the use of the QwTreeEventBuffer class.  It loads
 the file Tracking/prminput/QweakSim.root, which was produced with a 3-fold
 trigger in QweakSimG4 (hits in all of HDC, VDC-front, and VDC-back).  For
 every event a QwHitContainer is filled, which is then printed to std::cout.

*//*-------------------------------------------------------------------------*/

// Standard C and C++ headers
#include <iostream>
#include <cstdlib>

// ROOT headers
#include <TFile.h>
#include <TTree.h>

// Qweak Tracking headers
#include "Det.h"
#include "Qset.h"
#include "Qoptions.h"
#include "options.h"

#include "QwTrackingWorker.h"
#include "QwEvent.h"

// Qweak Tree event buffer header
#include "QwTreeEventBuffer.h"

#include "QwCommandLine.h"
#include "QwSubsystemArrayTracking.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"
#include "QwField.h"

//Temporary global variables for sub-programs
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
Options opt;


// Debug level
static const bool kDebug = false;
// ROOT file output
static const bool kTree = false;
static const bool kHisto = false;
// Magnetic field
static const bool kField = false;

int main (int argc, char* argv[])
{
  // Parse command line options
  QwCommandLine cmdline;
  cmdline.Parse(argc, argv);

  // Fill the search paths for the parameter files
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH"))+"/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput");

  // Handle for the list of VQwSubsystemTracking objects
  QwSubsystemArrayTracking* detectors = new QwSubsystemArrayTracking();
  // Region 2 HDC
  detectors->push_back(new QwDriftChamberHDC("R2"));
  detectors->GetSubsystem("R2")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R2"))->LoadQweakGeometry("qweak_new.geo");
  // Region 3 VDC
  detectors->push_back(new QwDriftChamberVDC("R3"));
  detectors->GetSubsystem("R3")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R3"))->LoadQweakGeometry("qweak_new.geo");

  // Get vector with detector info (by region, plane number)
  std::vector< std::vector< QwDetectorInfo > > detector_info;
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R2"))->GetDetectorInfo(detector_info);
  ((VQwSubsystemTracking*) detectors->GetSubsystem("R3"))->GetDetectorInfo(detector_info);
  // TODO This is handled incorrectly, it just adds the three package after the
  // existing three packages from region 2...  GetDetectorInfo should descend
  // into the packages and add only the detectors in those packages.
  // Alternatively, we could implement this with a singly indexed vector (with
  // only an id as primary index) and write a couple of helper functions to
  // select the right subvectors of detectors.

  // Load the simulated event file
  std::string filename = std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QweakSim.root";
  QwTreeEventBuffer* treebuffer = new QwTreeEventBuffer (filename, detector_info);
  treebuffer->SetDebugLevel(1);
  treebuffer->DisableResolutionEffects();

  // Load the geometry
  Qset qset;
  qset.FillDetectors((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();
  cout << "[QwTracking::main] Geometry loaded" << endl; // R3,R2

  // Set global options
  Qoptions qoptions;
  qoptions.Get((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.options").c_str());

  // Load field map
  QwField* qtor = 0;
  if (kField) qtor = new QwField(std::string(getenv("QWFIELDMAP")));

  // Create the tracking worker
  QwTrackingWorker *trackingworker = new QwTrackingWorker("qwtrackingworker");
  if (kDebug) trackingworker->SetDebugLevel(1);

  // Open ROOT file
  TFile* file = 0;
  if (kHisto || kTree) {
    file = new TFile("tracking.root", "RECREATE"); file->cd();
  }
  // Create ROOT tree
  TTree* tree = 0;
  QwPartialTrack* parttrack = 0;
  QwTrackingTreeLine* tlX = 0;
  QwTrackingTreeLine* tlU = 0;
  QwTrackingTreeLine* tlV = 0;
  QwHit* hit = 0;
  if (kTree) {
    tree = new TTree("tree","Track list");
    tree->Branch("track","QwPartialTrack",&parttrack);
    tree->Branch("tlX","QwTrackingTreeLine",&tlX);
    tree->Branch("tlU","QwTrackingTreeLine",&tlU);
    tree->Branch("tlV","QwTrackingTreeLine",&tlV);
    tree->Branch("hit","QwHit",&hit);
  }

  // Loop over the events
  int fEntries = treebuffer->GetEntries();
  for (int fEvtNum  = cmdline.GetFirstEvent();
           fEvtNum <= cmdline.GetLastEvent() && fEvtNum < fEntries; fEvtNum++) {

    // Get hit list
    QwHitContainer* hitlist = treebuffer->GetHitList(fEvtNum);

    // Print hit list
    if (kDebug) {
      std::cout << "Printing hitlist ..." << std::endl;
      hitlist->Print();
    }

    // Process the hit list through the tracking worker
    QwEvent *event = trackingworker->ProcessHits(detectors, hitlist);

    // Do something with this event

    // Get the first partial track in the upper region 2 HDC
    parttrack = event->parttrack[kPackageUp][kRegionID2][kTypeDriftHDC];
    if (parttrack) {
      tlX = parttrack->tline[kDirectionX];
      tlU = parttrack->tline[kDirectionU];
      tlV = parttrack->tline[kDirectionV];
    }
    // Save to tree if there is a partial track
    if (kTree) if (parttrack) tree->Fill();

    // Delete the event again
    delete hitlist;
    delete event;
  }

  std::cout << "Number of good partial tracks found: "
	    << trackingworker->ngood << std::endl;

  // Print results
  //tree->Print();

  // Write and close file
  if (kTree || kHisto) {
    file->Write();
    file->Close();
    delete file;
  }

  // Delete objects
  delete detectors;
  delete treebuffer;
  delete trackingworker;

  return 0;
}
