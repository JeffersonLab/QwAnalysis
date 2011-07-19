#include <iostream>
#include <vector>

#include "TTree.h"
#include "TBranch.h"
#include "TChain.h"

const Double_t LOWLIMIT = 20000.0;

// Laser must be off for at least this many
// consecutive entries to be considered off
const Int_t minEntries = 3000;


/////////////////////////////////////////////////////////////////////////////
//*** This function cycles through the laser power entries, creating an ***//
//*** array of values for starting and ending points of the laser off   ***//
//*** periods. Even indices 0,2,4... are beginning points and odd       ***//
//*** indices 1,3,5... are endpoints. Laser off entries are those that  ***//
//*** have a value of 1 or less.                                        ***//
///////////////////////////////////////////////////////////////////////////// 

std::vector<Int_t> cutOnLaser(TTree *tree)
{
  std::vector<Int_t> cuts;
  Double_t power;
  Int_t n = 0, numEntries = 0;
  Bool_t flag = kFALSE;

  tree->SetBranchStatus("*",0); // turn off all branches
  tree->SetBranchStatus("yield_sca_laser_PowT",1); // turn on only this branch
  tree->SetBranchAddress("yield_sca_laser_PowT",&power);

  numEntries = tree->GetEntries();
  std::cout << "Tree has " << numEntries << " entries." << std::endl;
  for (Int_t i = 0; i < numEntries; i++) {
    tree->GetEntry(i);
    if (n == minEntries) {
      cuts.push_back(i-minEntries);
      std::cout << "cut[" << cuts.size()-1 << "]=" << cuts.back() << std::endl;
      flag = kTRUE;
    }

    if (power <= LOWLIMIT) n++;
    else n = 0;

    if (flag) {
      if (n == 0 || i == numEntries-1) {
        cuts.push_back(i-1);
        std::cout << "cut[" << cuts.size()-1 << "]=" << cuts.back() << std::endl;
        flag = kFALSE;
      }
    }
  }
  tree->SetBranchStatus("*",1); // turn on all branches again
  return cuts;
}


///////////////////////////////////////////////
//Same as above except takes TChain as input.//
///////////////////////////////////////////////

std::vector<Int_t> cutOnLaser(TChain *chain)
{
  std::vector<Int_t> cuts;
  Double_t power;
  Int_t n = 0, numEntries = 0;
  Bool_t flag = kFALSE;

  chain->SetBranchStatus("*",0); // turn off all branches
  chain->SetBranchStatus("yield_sca_laser_PowT",1); // turn on only this branch
  chain->SetBranchAddress("yield_sca_laser_PowT",&power);

  numEntries = chain->GetEntries();
  std::cout << "Chain has " << numEntries << " entries." << std::endl;
  for (Int_t i = 0; i < numEntries; i++) {
    chain->GetEntry(i);
    if (n == minEntries) {
      cuts.push_back(i-minEntries);
      std::cout << "cut[" << cuts.size()-1 << "]=" << cuts.back() << std::endl;
      flag = kTRUE;
    }

    if (power <= LOWLIMIT) n++;
    else n = 0;

    if (flag) {
      if (n == 0 || i == numEntries-1) {
        cuts.push_back(i-1);
        std::cout << "cut[" << cuts.size()-1 << "]=" << cuts.back() << std::endl;
        flag = kFALSE;
      }
    }
  }
  chain->SetBranchStatus("*",1); // turn on all branches again
  return cuts;
}
