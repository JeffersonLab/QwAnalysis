#ifndef __VQwAnalyzer_h__
#define __VQwAnalyzer_h__

// System headers
#include <iostream>

// ROOT headers
#include <TFile.h>
#include <TTree.h>

// Qweak headers
#include "QwLog.h"
#include "VQwSystem.h"

// Forward declarations
class QwSubsystemArray;

class VQwAnalyzer : public VQwSystem {

  private:
    // Declare assignment operator private
    VQwAnalyzer& operator= (const VQwAnalyzer &value) {
      return *this;
    };

  protected:
    QwSubsystemArray* fDetectors;	//!

    TFile* fRootFile;
    TTree* fRootTree;

    // Flags
    Bool_t kHisto;
    Bool_t kTree;

  public:
    VQwAnalyzer (const char* name = 0): VQwSystem (name) { };
    virtual ~VQwAnalyzer() { };

    virtual QwSubsystemArray* GetSubsystemArray() { return fDetectors; };

    virtual void OpenRootFile() { // TODO this could be in VQwAnalyzer itself
      QwError << "VQwAnalyzer::OpenRootFile not implemented!" << QwLog::endl;
      return;
    };
    virtual void CloseRootFile() {
//       // Write ROOT file
//       fRootFile->Write(0, TObject::kOverwrite);
//       // Delete histograms
//       if (kHisto) fDetectors->DeleteHistograms();
//       // Close ROOT file
//       fRootFile->Close();
//       delete fRootFile;

      return;
    };

    virtual void ProcessEvent() {
      QwError << "VQwAnalyzer::ProcessEvent not implemented!" << QwLog::endl;
      return;
    };

  ClassDef(VQwAnalyzer,1); // corresponding ClassImp in QwRoot.h
};

#endif // __VQwAnalyzer_h__
