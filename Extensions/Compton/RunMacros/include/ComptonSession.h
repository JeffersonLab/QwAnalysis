#ifndef COMPTONSESSION_H
#define COMPTONSESSION_H

#include <TROOT.h> // So we can use most ROOT typedefs
#include <TCut.h>

class TChain;
class ComptonDatabase;

namespace Compton {
  // Below this value, laser is considered off (measured in arbitrary units)
  const Double_t  kLaserLowLimit = 20000.0;

  // Laser must be off for at least this many
  // consecutive entries to be considered off
  const Int_t kLaserMinimumOffEntries[2] = {12000,3000};

  // Deefine a tree type
  enum TREE_TYPE_t {
    MPS_TREE_TYPE,
    HEL_TREE_TYPE
  };

  /*
   * A structure containing the laser cycles for the specified tree
   */
  struct LaserCycles {
    std::vector<Int_t> start;
    std::vector<Int_t> end;
    std::vector<Int_t> off_id;
    std::vector<Int_t> on_id;
    std::vector<Int_t> pat_id;
    TString cut_string;
    TREE_TYPE_t type;

    /**
     * Returns the number of complete laser cycles
     */
    Int_t NumberOfCycles(Int_t state = 1) {
      if(state) // On cycles
        return (start.size()>=2 && start.size()==end.size())
          ? start.size()-1 : 0;

      // Off cycles
      return (start.size()>=2 && start.size()==end.size()) ? start.size() : 0;
    }

    Int_t NumberOfPatterns() {
      // For now, we'll just define it the same as the on cycles
      return NumberOfCycles(1);
    }

    /**
     * Returns a TCut of the laser off period for the specified cycle
     */
    TCut GetCutOff(Int_t cycle) {
      if( cycle >= NumberOfCycles(0) )
        return TCut();

      return TCut(Form("(%s>=%d&&%s<=%d)",
            cut_string.Data(),start[cycle],cut_string.Data(),end[cycle]));
    }

    /**
     * Returns a TCut of the laser on period for the specified cycle
     */
    TCut GetCutOn(Int_t cycle) {
      if( cycle >= NumberOfCycles(1) )
        return TCut();

      return TCut(Form("(%s>=%d&&%s<=%d)",
            cut_string.Data(),end[cycle],cut_string.Data(),start[cycle+1]));
    }
  };

}


class ComptonSession {
public:
  ComptonSession(Int_t runnumber,TString db_file,TString pass);
  ~ComptonSession();

  Bool_t SetWebDir(TString webdir);
  Bool_t SetStorageDir(TString webdir);

  /*
   *  \brief Find and store laser cycles in database
   */
  void FindLaserCycles();

  /*
   *  \brief Retrieves the laser cycles already in the database
   */
  void LaserCyclesFromDB();

  Int_t RunNumber() {
    return fRunNumber;
  }

  TString WebDir() {
    return fWebDir;
  }


  TString StorageDir() {
    return fStorageDir;
  }

  ComptonDatabase* db() {
    return fDB;
  }

  Compton::LaserCycles GetLaserCycles(Compton::TREE_TYPE_t type);

  TChain* GetChain(Compton::TREE_TYPE_t type);

  Int_t MpsID() { return fMpsID; }
  Int_t HelID() { return fHelID; }

private:
  Int_t fRunNumber;
  Int_t fRunID; // ID number in database
  Int_t fHelID; // ID number in database
  Int_t fMpsID; // ID number in database
  ComptonDatabase *fDB;
  TString fWebDir;  // Directory for output of images
  TString fStorageDir; // Directory for larger file/data storage

  Compton::LaserCycles fLaserCycles[2]; // For the number of trees

  // Keep a copy of the chains
  TChain *fChains[2];
};

#endif
