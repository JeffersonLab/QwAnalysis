// System headers
#include <vector>

// ROOT headers
#include <TFile.h>
#include <TTree.h>
#include <TKey.h>

// Qweak headers
#include "QwLog.h"

// Main function
Int_t main(Int_t argc, Char_t* argv[])
{
  if (argc < 1) {
    QwError << "Please specify a filename." << QwLog::endl;
    return 1;
  }

  // Fill list of trees we are interested in
  std::vector< TString > treelist;
  treelist.push_back("hit_tree");
  treelist.push_back("event_tree");

  // Fill list of branches we are interested in
  std::vector< std::pair< TString, TObject* > > branchlist;
  branchlist.push_back(std::pair< TString, TObject* > ("hits",0));
  branchlist.push_back(std::pair< TString, TObject* > ("events",0));

  // Open the file
  TString name = TString(argv[1]);
  TFile* file = new TFile(name);

  // Look for all trees
  for (size_t tree = 0; tree < treelist.size(); tree++) {
    // Get object and check whether it is a tree
    TObject* object = file->Get(treelist.at(tree));
    if (! object) {
      QwWarning << "No key " << treelist.at(tree) << QwLog::endl;
      continue;
    }
    if (! object->InheritsFrom("TTree")) {
      QwWarning << "Object " << treelist.at(tree) << " is not a tree" << QwLog::endl;
      continue;
    }
    TTree* thistree = dynamic_cast<TTree*>(object);
    if (! thistree) {
      QwWarning << "No tree " << treelist.at(tree) << QwLog::endl;
      continue;
    }
    Int_t nevents = thistree->GetEntries();
    QwMessage << "Number of entries in " << treelist.at(tree) << ": "
              << nevents << QwLog::endl;

    // Set up the branches
    for (size_t branch = 0; branch < branchlist.size(); branch++) {
      if (thistree->FindBranch(branchlist.at(branch).first)) {
        thistree->SetBranchAddress(branchlist.at(branch).first,
                                   &branchlist.at(branch).second);
      } else {
        QwWarning << "No branch " << branchlist.at(branch).first << " "
                  << "in tree " << treelist.at(tree)
                  << QwLog::endl;
      }
    }

    // Loop over all events
    for (Int_t event = 0; event < nevents; event++) {
      QwMessage << "Event " << event << QwLog::endl;
      thistree->GetEntry(event);
      for (size_t branch = 0; branch < branchlist.size(); branch++) {
        if (branchlist.at(branch).second)
          branchlist.at(branch).second->Print();
      }
    }
  }

  // Close file
  file->Close();
}

