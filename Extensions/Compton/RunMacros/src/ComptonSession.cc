#include "ComptonSession.h"
#include "ComptonDatabase.h"

// ROOT includes
#include <TChain.h>

// Standard includes
#include <iostream>
#include <vector>

ComptonSession::ComptonSession(Int_t runnumber,TString db_file,TString pass)
  : fRunNumber(runnumber)
{
  fDB = new ComptonDatabase(db_file);

  // Load up the two chains
  fChains[0] = new TChain("Mps_Tree");
  fChains[0]->Add(Form("$QW_ROOTFILES/Compton_%s_%d.*.root",
        pass.Data(),runnumber));
  fChains[1] = new TChain("Hel_Tree");
  fChains[1]->Add(Form("$QW_ROOTFILES/Compton_%s_%d.*.root",
        pass.Data(),runnumber));

  // Check if this run is already defined in the database
  if(!fDB->ExistsRun(fRunNumber)) {
    fRunID = fDB->AddRun(fRunNumber,fChains[0]->GetEntries(),
        fChains[1]->GetEntries());
  }

  // Initialize the laser cycle structures
  TString cuts[2] = { "mps_counter","pattern_counter" };
  for(Int_t type = 0; type < 2; type++ ) {
    fLaserCycles[type].type = (Compton::TREE_TYPE_t)type;
    fLaserCycles[type].cut_string = cuts[type];
  }

}

ComptonSession::~ComptonSession()
{
  if(fDB)
    delete fDB;
}

void ComptonSession::FindLaserCycles()
{
  for( Int_t type = 0; type < 2; type++ ) {
    TString prefix = type == Compton::MPS_TREE_TYPE ? "" : "yield_";
    std::cout << "Finding " << (type==Compton::MPS_TREE_TYPE ? "Mps_Tree" :
      "Hel_Tree")  << " laser cycles" << std::endl;
    Double_t power;
    Int_t n = 0, numEntries = 0;
    Bool_t flag = kFALSE;

    fChains[type]->SetBranchStatus("*",0); // turn off all branches
    fChains[type]->SetBranchStatus(prefix+"sca_laser_PowT",1); // turn on only this branch
    fChains[type]->SetBranchAddress(prefix+"sca_laser_PowT",&power);

    numEntries = fChains[type]->GetEntries();
    std::cout << "Chain has " << numEntries << " entries." << std::endl;
    for (Int_t i = 0; i < numEntries; i++) {
      fChains[type]->GetEntry(i);
      if (n == Compton::kLaserMinimumOffEntries[type]) {
        fLaserCycles[type].start.push_back(i-
            Compton::kLaserMinimumOffEntries[type]);
        std::cout << "start(" << type << ")[" << 
          fLaserCycles[type].start.size()-1 << "]="
          << fLaserCycles[type].start.back() << std::endl;
        flag = kTRUE;
      }

      if (power <= Compton::kLaserLowLimit) n++;
      else n = 0;

      if (flag) {
        if (n == 0 || i == numEntries-1) {
          fLaserCycles[type].end.push_back(i-1);
          std::cout << "end(" << type << ")[" << 
            fLaserCycles[type].end.size()-1 << "]="
            << fLaserCycles[type].end.back() << std::endl;
          flag = kFALSE;
        }
      }
    }
    // Make sure all stored laser cycles are complete
    if(fLaserCycles[type].start.size() != fLaserCycles[type].end.size()) {
      // Delete last element in start
      fLaserCycles[type].start.pop_back();
    }
    fChains[type]->SetBranchStatus("*",1); // turn on all branches again
  }
  // Store cycles in the database
  for(Int_t cycle = 0; cycle < fLaserCycles[0].start.size(); cycle++ ) {
    fLaserCycles[0].id.push_back(fDB->StoreLaserCycle(fRunID,cycle,
        fLaserCycles[0].start[cycle],
        fLaserCycles[0].end[cycle],
        fLaserCycles[1].start[cycle],
        fLaserCycles[1].end[cycle]
        ));
    fLaserCycles[1].id.push_back(fLaserCycles[0].id.back());
  }

}


Compton::LaserCycles ComptonSession::GetLaserCycles(Compton::TREE_TYPE_t type)
{
  if(type == Compton::MPS_TREE_TYPE )
    return fLaserCycles[0];

  // By default, return the Helicity tree
  return fLaserCycles[1];
}

void ComptonSession::LaserCyclesFromDB()
{
}
