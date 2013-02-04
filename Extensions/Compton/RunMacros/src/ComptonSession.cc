#include "ComptonSession.h"
#include "ComptonDatabase.h"

// ROOT includes
#include <TChain.h>
#include <TSystem.h>

// Standard includes
#include <iostream>
#include <vector>

// Include boost to help us out
#include <boost/filesystem.hpp>

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

  // Get the source ID's
  fHelID = fDB->GetSourceID("qrt");
  fMpsID = fDB->GetSourceID("mps");


  // Initialize the laser cycle structures
  TString cuts[2] = { "mps_counter","pat_counter" };
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
  for(Int_t type = 0; type < 2; type++ ) {
    Int_t id;
    if(type == 0 ) {
      id = fMpsID;
    } else {
      id = fHelID;
    }
    Int_t n_off = fLaserCycles[type].NumberOfCycles(0);
    Int_t n_on = fLaserCycles[type].NumberOfCycles(1);
    Int_t n_pat = fLaserCycles[type].NumberOfPatterns();

    // First store the OFF cycles
    for(Int_t cycle = 0; cycle < n_off; cycle++ ) {
      fLaserCycles[type].off_id.push_back(
          fDB->StoreLaserCycle(fRunID,id,cycle,0,
            fLaserCycles[type].start[cycle],fLaserCycles[type].end[cycle]));
    }

    // Now store the ON cycles
    for(Int_t cycle = 0; cycle < n_on; cycle++ ) {
      fLaserCycles[type].on_id.push_back(
          fDB->StoreLaserCycle(fRunID,id,cycle,1,
            fLaserCycles[type].start[cycle],fLaserCycles[type].end[cycle]));
    }

    // Now store the patterns (defined as off-on-off)
    for(Int_t cycle = 0; cycle < n_pat; cycle++ ) {
      fLaserCycles[type].pat_id.push_back(
          fDB->StoreLaserPattern(fRunID,id,cycle,
            fLaserCycles[type].off_id[cycle],
            fLaserCycles[type].on_id[cycle],
            fLaserCycles[type].off_id[cycle+1]));
    }
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

TChain* ComptonSession::GetChain(Compton::TREE_TYPE_t type)
{
  if(type == Compton::MPS_TREE_TYPE)
    return fChains[0];

  return fChains[1];
}

Bool_t ComptonSession::SetWebDir(TString dir)
{
  fWebDir = dir+Form("/run_%d",fRunNumber);
  if(boost::filesystem::exists(fWebDir.Data()))
    return kTRUE; // Already exists, so don't create it

  // But if it doesn't exist, try to create it first!
  return boost::filesystem::create_directories(fWebDir.Data());
}

Bool_t ComptonSession::SetStorageDir(TString dir)
{
  fStorageDir = dir+Form("/run_%d",fRunNumber);
  if(boost::filesystem::exists(fStorageDir.Data()))
    return kTRUE; // Already exists, so don't create it

  // But if it doesn't exist, try to create it first!
  return boost::filesystem::create_directories(fStorageDir.Data());
}
