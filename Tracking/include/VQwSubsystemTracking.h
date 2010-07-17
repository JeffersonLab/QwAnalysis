#ifndef __VQWSUBSYSTEMTRACKING__
#define __VQWSUBSYSTEMTRACKING__

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "VQwSubsystem.h"

///
/// \ingroup QwTracking

// Forward declarations
class QwHitContainer;
class QwDetectorInfo;

class VQwSubsystemTracking: virtual public VQwSubsystem {
  /******************************************************************
   *  Class: VQwSubsystemTracking
   *         Virtual base class for the classes containing the
   *         event-based information from each tracking subsystem.
   *         This will define the interfaces used in communicating
   *         with the CODA routines.
   *
   ******************************************************************/
  public:

    /// Constructor with name
    VQwSubsystemTracking(TString name): VQwSubsystem(name) {
      SetEventTypeMask(0xfffe); // do not accept 0x1
    };
    /// Default destructor
    virtual ~VQwSubsystemTracking() { };


    /// \brief Construct the branch and tree vector
    virtual void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values) { };
    /// \brief Construct the branch and tree vector
    virtual void ConstructBranchAndVector(TTree *tree, std::vector<Double_t>& values) {
      TString tmpstr("");
      ConstructBranchAndVector(tree,tmpstr,values);
    };
    /// \brief Construct the branch and tree vector
    virtual void ConstructBranch(TTree *tree, TString& prefix) { };
    /// \brief Construct the branch and tree vector based on the trim file
    virtual void ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file) { };
    /// \brief Fill the tree vector
    virtual void FillTreeVector(std::vector<Double_t>& values) { };


    /// Load geometry definition for tracking subsystems (required)
    virtual Int_t LoadGeometryDefinition(TString mapfile) = 0;

    /// Get the detector geometry information
    virtual Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detect_info) = 0;


    /// Get the hit list
    virtual void  GetHitList(QwHitContainer & grandHitContainer) = 0;

  protected:

    /// Tree indices
    size_t fTreeArrayIndex;
    size_t fTreeArrayNumEntries;

  private:

    /// Private default constructor
    VQwSubsystemTracking() { };

}; // class VQwSubsystemTracking

#endif // __VQWSUBSYSTEMTRACKING__
