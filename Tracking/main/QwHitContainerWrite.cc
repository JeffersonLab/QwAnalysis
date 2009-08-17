/*------------------------------------------------------------------------*//*!

 \file QwEventDisplay.cc
 \ingroup QwTrackingAnl

 \brief Example code for the event display routines

*//*-------------------------------------------------------------------------*/

// Standard C and C++ headers
#include <iostream>

// ROOT headers
#include <TFile.h>
#include <TTree.h>

// Qweak headers
#include "QwTreeEventBuffer.h"
#include "QwSubsystemArrayTracking.h"
#include "QwDriftChamberHDC.h"
#include "QwDriftChamberVDC.h"

#include "QwHitRootContainer.h"

// Ignore the next few lines
#include "Qoptions.h"
#include "options.h"
#include "Qset.h"
#include "Det.h"
Det *rcDETRegion[kNumPackages][kNumRegions][kNumDirections];
Det rcDET[NDetMax];
Options opt;

static const bool kDebug = false;

int main (int argc, char* argv[])
{
  // Fill the search paths for the parameter files
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWSCRATCH"))+"/setupfiles");
  QwParameterFile::AppendToSearchPath(std::string(getenv("QWANALYSIS"))+"/Tracking/prminput");

  // Handle for the list of VQwSubsystemTracking objects
  QwSubsystemArrayTracking* QwDetectors = new QwSubsystemArrayTracking();
  // Region 2 HDC
  QwDetectors->push_back(new QwDriftChamberHDC("R2"));
  QwDetectors->GetSubsystem("R2")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) QwDetectors->GetSubsystem("R2"))->LoadQweakGeometry("qweak_new.geo");
  // Region 3 VDC
  QwDetectors->push_back(new QwDriftChamberVDC("R3"));
  QwDetectors->GetSubsystem("R3")->LoadChannelMap("qweak_cosmics_hits.map");
  ((VQwSubsystemTracking*) QwDetectors->GetSubsystem("R3"))->LoadQweakGeometry("qweak_new.geo");

  // Get vector with detector info (by region, plane number)
  std::vector< std::vector< QwDetectorInfo > > detector_info;
  ((VQwSubsystemTracking*) QwDetectors->GetSubsystem("R2"))->GetDetectorInfo(detector_info);
  ((VQwSubsystemTracking*) QwDetectors->GetSubsystem("R3"))->GetDetectorInfo(detector_info);
  // TODO This is handled incorrectly, it just adds the three package after the
  // existing three packages from region 2...  GetDetectorInfo should descend
  // into the packages and add only the detectors in those packages.
  // Alternatively, we could implement this with a singly indexed vector (with
  // only an id as primary index) and write a couple of helper functions to
  // select the right subvectors of detectors.

  // Load the simulated event file
  std::string filename = std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QweakSim.root";
  QwTreeEventBuffer* treebuffer = new QwTreeEventBuffer (filename, detector_info);
  treebuffer->SetDebugLevel(0);

  // Load the old geometry
  // TODO The only reason why we have to define the old geometry here is so
  // the uv2xy transform can find the angle.  What would make more sense is
  // to put the angle in the uv2xy constructor (with a flag to swap axes as
  // is done for region 2).  Sigh...
  Qset qset;
  qset.FillDetectors((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();

  // Open file
  TFile* file = new TFile("hitlist.root", "RECREATE");
  TTree* tree = new TTree("tree", "Hit list");
  QwHitRootContainer* rootlist = new QwHitRootContainer();
  tree->Branch("hits","QwHitRootContainer",&rootlist);

  // Loop over the events in the file
  int fEntries = treebuffer->GetEntries();
  for (int fEvtNum = 0; fEvtNum < fEntries; fEvtNum++) {

    // Print event number
    if (kDebug) std::cout << "Event: " << fEvtNum << std::endl;

    // Get hit list as QwHitContainer (new hit list)
    QwHitContainer* hitlist = treebuffer->GetHitList(fEvtNum);
    if (kDebug) hitlist->Print();

    // Replace rootlist with QwHitContainer
    rootlist->Convert(hitlist);

    // Print the hitlist
    if (kDebug) rootlist->Print();

    // Save the event to tree
    tree->Fill();

    // Delete the hit list
    delete hitlist;
  }
  // Delete the ROOT hit list
  delete rootlist;

  // Print results
  if (kDebug) tree->Print();

  // Output results
  std::cout << "Successfully wrote " << tree->GetEntries() << " events." << std::endl;

  // Write and close file
  file->Write();
  file->Close();

  return 0;
}
