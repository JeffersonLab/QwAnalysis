#ifndef __VQWSUBSYSTEMTRACKING__
#define __VQWSUBSYSTEMTRACKING__

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "VQwSubsystem.h"
#include "QwGeometry.h"
#include "QwRootFile.h"

// Forward declarations
class QwHitContainer;


/**
 *  \class VQwsubsystemTracking
 *  \ingroup QwTracking
 *
 *  \brief Base class for tracking subsystems
 *
 *  This base class for tracking subsystems provides functionality for
 *  constructing tree branches, loading and manipuliting geometry info,
 *  and retrieval of hit lists from the tracking subsystem.
 */
class VQwSubsystemTracking: virtual public VQwSubsystem {
  /******************************************************************
   *  Class: VQwSubsystemTracking
   *         Virtual base class for the classes containing the
   *         event-based information from each tracking subsystem.
   *         This will define the interfaces used in communicating
   *         with the CODA routines.
   *
   ******************************************************************/
  private:

    /// Private default constructor (not implemented, will throw linker error on use)
    VQwSubsystemTracking();

  public:

    /// Constructor with name
    VQwSubsystemTracking(TString name): VQwSubsystem(name) {
      SetEventTypeMask(0xfffe); // do not accept 0x1
      fDetectorInfo.clear(); // clear detector info
    };
    /// Default destructor
    virtual ~VQwSubsystemTracking() {
      // Clear detector info objects
      QwGeometry::iterator i;
      for (i = fDetectorInfo.begin(); i != fDetectorInfo.end(); i++) {
        delete *i;
      }
      fDetectorInfo.clear();
    };


    /// \name Tree and branch construction and maintenance
    /// These functions are optional for tracking subsystems; some tracking
    /// subsystems might not need to write anything to a tree.
    // @{
    /// \brief Construct the branch and tree vector
    using VQwSubsystem::ConstructBranchAndVector;
    virtual void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values) { };
    /// \brief Construct the branch and tree vector
    virtual void ConstructBranch(TTree *tree, TString& prefix) { };
    /// \brief Construct the branch and tree vector based on the trim file
    virtual void ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file) { };
    /// \brief Fill the tree vector
    virtual void FillTreeVector(std::vector<Double_t>& values) const { };
    // @}


    /// Load geometry definition for tracking subsystems
    Int_t LoadGeometryDefinition(TString mapfile);

    /// Get the hit list
    virtual void  GetHitList(QwHitContainer & grandHitContainer) = 0;


    /// Get the detector geometry information
    const QwGeometry& GetDetectorInfo() const { return fDetectorInfo; };

    /// Hardware error summary
    virtual void FillHardwareErrorSummary() {};

  protected:

    /// Geometry information of this subsystem
    QwGeometry fDetectorInfo;

    /// Tree indices
    size_t fTreeArrayIndex;
    size_t fTreeArrayNumEntries;

    Double_t fF1TDCResolutionNS;

}; // class VQwSubsystemTracking

#endif // __VQWSUBSYSTEMTRACKING__
