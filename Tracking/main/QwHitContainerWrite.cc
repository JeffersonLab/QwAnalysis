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
  //((VQwSubsystemTracking*) QwDetectors->GetSubsystem("R2"))->GetDetectorInfo(detector_info);
  ((VQwSubsystemTracking*) QwDetectors->GetSubsystem("R3"))->GetDetectorInfo(detector_info);

  // Load the simulated event file
  QwTreeEventBuffer* treebuffer =
    new QwTreeEventBuffer (std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/QweakSim.root");
  treebuffer->SetDetectorInfo(detector_info);
  treebuffer->SetDebugLevel(0);

  // Load the geometry (ignore)
  Qset qset;
  qset.FillDetectors((std::string(getenv("QWANALYSIS"))+"/Tracking/prminput/qweak.geo").c_str());
  qset.LinkDetectors();
  qset.DeterminePlanes();

  // Open file
  TFile* file = new TFile("hitlist.root", "RECREATE");
  TTree* tree = new TTree("tree", "Hit list");
  QwHitRootContainer* rootlist = new QwHitRootContainer();
  QwHitContainer* hitlist = 0;
  tree->Branch("hits","QwHitRootContainer",&rootlist);

  // Loop over the events in the file
  int fEntries = treebuffer->GetEntries();
  for (int fEvtNum = 0; fEvtNum < fEntries; fEvtNum++) {

    // Get hit list as QwHitContainer (new hitlist)
    hitlist = treebuffer->GetHitList(fEvtNum);

    // Replace rootlist with QwHitContainer
    rootlist->Replace(hitlist);

    // Save the event to tree
    tree->Fill();

    // Delete objects that are created in previous statements
    delete hitlist;
  }
  delete rootlist;

  // Print results
  tree->Print();

  // Write and close file
  file->Write();
  file->Close();

  return 0;
}
