#ifndef __QWROOTFILE__
#define __QWROOTFILE__

// ROOT headers
#include <TFile.h>
#include <TTree.h>

// Qweak headers
#include "QwOptions.h"
#include "QwMapFile.h"
#include "QwSubsystemArray.h"
#include "QwHelicityPattern.h"
#include "QwSubsystemArrayParity.h"
/// \todo TODO (wdc) QwRootFile should not depend on QwHelicityPattern or
/// QwSubsystemArrayParity.  Maybe need to extend using QwRootFileParity,
/// which would allow for different structure in tracking.

class QwRootFile {

  public:

    /// \brief Constructor with run label
    QwRootFile(const TString& run_label);
    /// \brief Destructor
    virtual ~QwRootFile();

    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);

    /// Is the ROOT file active?
    Bool_t IsRootFile() { return (fRootFile); };
    /// Is the map file active?
    Bool_t IsMapFile()  { return (fMapFile); };

    /// Construct the tree branches of the subsystem array
    void ConstructTreeBranches(QwSubsystemArrayParity& detectors);
    /// Construct the tree branches of the helicity pattern
    void ConstructTreeBranches(QwHelicityPattern& helicity_pattern);

    /// Fill the tree branches of the subsystem array
    void FillTreeBranches(QwSubsystemArrayParity& detectors);
    /// Fill the tree branches of the helicity pattern
    void FillTreeBranches(QwHelicityPattern& helicity_pattern);

    /// Construct histograms of the subsystem array
    void ConstructHistograms(QwSubsystemArray& detectors);
    /// Construct histograms of the helicity pattern
    void ConstructHistograms(QwHelicityPattern& helicity_pattern);

    /// Fill histograms of the subsystem array
    void FillHistograms(QwSubsystemArray& detectors) {
      static Int_t count = 0;
      if (fEnableHisto && fEnableMps) detectors.FillHistograms();
      if (++count % fUpdateInterval == 0) Update();
    };
    /// Fill histograms of the helicity pattern
    void FillHistograms(QwHelicityPattern& helicity_pattern) {
      if (fEnableHisto && fEnableHel) helicity_pattern.FillHistograms();
    };

    /// Delete histograms of the subsystem array
    void DeleteHistograms(QwSubsystemArray& detectors) {
      if (fEnableHisto && fEnableMps) detectors.DeleteHistograms();
    }
    /// Delete histograms of the helicity pattern
    void DeleteHistograms(QwHelicityPattern& helicity_pattern) {
      if (fEnableHisto && fEnableHel) helicity_pattern.DeleteHistograms();
    }


    // Map file only functionality
    void Update() { if (fMapFile) fMapFile->Update(); }

    // Wrapped common functionality
    Int_t Write(const char* name = 0, Int_t option = 0, Int_t bufsize = 0) {
      Int_t retval;
      if (fMapFile)  retval = fMapFile->Write(name, option, bufsize);
      if (fRootFile) retval = fRootFile->Write(name, option, bufsize);
      return retval;
    }
    void Close()  { if (fMapFile) fMapFile->Close();  if (fRootFile) fRootFile->Close(); }
    void Print()  { if (fMapFile) fMapFile->Print();  if (fRootFile) fRootFile->Print(); }
    void ls()     { if (fMapFile) fMapFile->ls();     if (fRootFile) fRootFile->ls(); }

    Bool_t cd(const char* path = 0) {
      Bool_t status = kTRUE;
      if (fMapFile)  status &= fMapFile->cd(path);
      if (fRootFile) status &= fRootFile->cd(path);
      return status;
    }

    TDirectory* mkdir(const char* name, const char* title = "") {
      // TMapFile has no suport for mkdir
      if (fRootFile) return fRootFile->mkdir(name, title);
      else return 0;
    }

  private:

    QwRootFile() { }; // Private default constructor

    Bool_t fOnline;
    Bool_t fEnableTree;
    Bool_t fEnableHisto;
    Bool_t fEnableMps;
    Bool_t fEnableHel;

    /// ROOT file
    TFile*     fRootFile;

    /// Map file
    QwMapFile* fMapFile;
    Int_t fUpdateInterval;

    /// Trees and associated vectors
    TTree*                fMpsTree;
    std::vector<Double_t> fMpsVector;
    TTree*                fHelTree;
    std::vector<Double_t> fHelVector;

    /// Prescaling of events written to tree
    Int_t fNEventsToSkip;
    Int_t fNEventsToSave;

    /// Maximum tree size
    static const Long64_t kMaxTreeSize;

};

#endif // __QWROOTFILE__