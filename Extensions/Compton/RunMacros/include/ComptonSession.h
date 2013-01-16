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
    std::vector<Int_t> id;
    TString cut_string;
    TREE_TYPE_t type;

    /**
     * Returns the number of complete laser cycles
     */
    Int_t NumberOfCycles() {
      return (start.size()>=2 && start.size()==end.size()) ? start.size()-1 : 0;
    }

    /**
     * Returns a TCut of the laser off period for the specified cycle
     */
    TCut GetCutOff(Int_t cycle) {
      if( cycle >= NumberOfCycles() )
        return TCut();

      return TCut(Form("(%s>=%d&&%s<=%d)||"
            "(%s>=%d&&%s<=%d)",
            cut_string.Data(),start[cycle],cut_string.Data(),end[cycle],
            cut_string.Data(),start[cycle+1],cut_string.Data(),end[cycle+1]));
    }

    /**
     * Returns a TCut of the laser on period for the specified cycle
     */
    TCut GetCutOn(Int_t cycle) {
      if( cycle >= NumberOfCycles() )
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

  /*
   *  \brief Find and store laser cycles in database
   */
  void FindLaserCycles();

  /*
   *  \brief Retrieves the laser cycles already in the database
   */
  void LaserCyclesFromDB();

  Int_t runnumber() {
    return fRunNumber;
  }

  ComptonDatabase* db() {
    return fDB;
  }

  Compton::LaserCycles GetLaserCycles(Compton::TREE_TYPE_t type);

private:
  Int_t fRunNumber;
  Int_t fRunID; // ID number in database
  ComptonDatabase *fDB;

  Compton::LaserCycles fLaserCycles[2]; // For the number of trees

  // Keep a copy of the chains
  TChain *fChains[2];
};

#endif
