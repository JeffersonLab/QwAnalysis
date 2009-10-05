/*------------------------------------------------------------------------*//*!

 \file QwHitContainerRead.cc
 \ingroup QwTrackingAnl

 \brief Example code for reading a QwHitContainer

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

  // Open file
  TFile* file = new TFile("hitlist.root");
  TTree* tree = (TTree*) file->Get("tree");
  TBranch* branch = tree->GetBranch("hits");
  QwHitRootContainer* rootlist = new QwHitRootContainer();
  branch->SetAddress(&rootlist);

  // Loop over the events in the file
  int fEntries = tree->GetEntries();
  for (int fEvtNum = 0; fEvtNum < fEntries; fEvtNum++) {

    // Print event number
    if (kDebug) std::cout << "Event: " << fEvtNum << std::endl;

    // Read next event into QwHitRootContainer format
    branch->GetEntry(fEvtNum);
    if (kDebug) {
      std::cout << "QwHitRootContainer hitlist: ";
      std::cout << "(" << rootlist->GetSize() << " hits)" << std::endl;
      rootlist->Print();
    }

    // Convert into the QwHitContainer format
    QwHitContainer* hitlist = rootlist->Convert();

    // Print hit list
    if (kDebug) {
      std::cout << "QwHitContainer hitlist: ";
      std::cout << "(" << hitlist->size() << " hits)" << std::endl;
      hitlist->Print();
    }

    // Do something

    // Clear rootlist (because GetEntry() adds to the TOrdCollection!!!)
    rootlist->Clear();

    // Delete objects
    delete hitlist;
  }
  delete rootlist;

  // Output results
  std::cout << "Successfully read " << fEntries << " events." << std::endl;

  // Close file
  file->Close();

  return 0;
}
