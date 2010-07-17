/**********************************************************\
* File: QwSubsystemArrayTracking.h                         *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-22 15:50>                           *
\**********************************************************/

#ifndef __QWSUBSYSTEMARRAYTRACKING__
#define __QWSUBSYSTEMARRAYTRACKING__

// Qweak headers
#include "QwSubsystemArray.h"
#include "QwHitContainer.h"
#include "VQwSubsystemTracking.h"


///
/// \ingroup QwTracking
class QwSubsystemArrayTracking:  public QwSubsystemArray {

  public:

    /// Default constructor
    QwSubsystemArrayTracking(): QwSubsystemArray(CanContain) { };
    /// Constructor with options
    QwSubsystemArrayTracking(QwOptions& options): QwSubsystemArray(options, CanContain) { };
    /// Constructor with map file
    QwSubsystemArrayTracking(const char* filename): QwSubsystemArray(filename, CanContain) { };
    /// Default destructor
    virtual ~QwSubsystemArrayTracking() { };

    /// \brief Get the subsystem with the specified name
    VQwSubsystemTracking* GetSubsystemByName(const TString& name);

    /// \brief Construct the branch and tree vector
    void ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t>& values);
    /// \brief Construct the branch and tree vector with trim tree feature
    void ConstructBranchAndVector(TTree *tree, std::vector<Double_t>& values) {
      TString tmpstr("");
      ConstructBranchAndVector(tree,tmpstr,values);
    };
    /// \brief Construct the branch for the flat tree
    void ConstructBranch(TTree *tree, TString& prefix);
    /// \brief Construct the branch for the flat tree with tree trim files accepted
    void ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& detectors);
    /// \brief Fill the tree vector
    void FillTreeVector(std::vector<Double_t>& values);

    // Update the wire-based hit list from each subsystem
    void GetHitList(QwHitContainer& hitlist);
    void GetHitList(QwHitContainer* hitlist) { GetHitList(*hitlist); };

  protected:

    /// Test whether this subsystem array can contain a particular subsystem
    static Bool_t CanContain(VQwSubsystem* subsys) {
      return (dynamic_cast<VQwSubsystemTracking*>(subsys) != 0);
    };

}; // class QwSubsystemArrayTracking

#endif // __QWSUBSYSTEMARRAYTRACKING__
