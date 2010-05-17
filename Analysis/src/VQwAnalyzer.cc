#include "VQwAnalyzer.h"
ClassImp(VQwAnalyzer);

// Qweak headers
#include "QwSubsystemArray.h"

// Initialize static objects
TFile* VQwAnalyzer::fRootFile = NULL;
TTree* VQwAnalyzer::fRootTree = NULL;

/**
 * Open and initialize the ROOT file
 * @param run Run number
 */
void VQwAnalyzer::OpenRootFile(Int_t run)
{
  // NOTE It seems that histogram and tree filling is thread safe, but we cannot
  // hve a single ROOT file for every analyzer object.  Probably we can open the
  // ROOT file a level up in QwControl, pass the pointer, and cd() into it in here
  // before we construct the histograms

  // If no ROOT file has been opened yet
  if (fRootFile == NULL) {

    // Open ROOT file
    fRootFile = new TFile(Form(TString(getenv("QWSCRATCH")) + "/rootfiles/Qweak_%d.root", run),
			 "RECREATE",
			 "QWeak ROOT file with real events");

    // Construct tree
    fRootFile->cd();
    if (kTree) fRootTree = new TTree("tree", "QwAnalysis");

    // Construct histograms
    fRootFile->cd();
    if (kHisto) fDetectors->ConstructHistograms(fRootFile->mkdir("histo"));

  } // end of if no ROOT file yet

};

/**
 * Close and clean-up the ROOT file
 */
void VQwAnalyzer::CloseRootFile()
{
  // Write ROOT file
  fRootFile->Write(0, TObject::kOverwrite);

  // Delete histograms
  if (kHisto) fDetectors->DeleteHistograms();

  // Close ROOT file
  fRootFile->Close();
  delete fRootFile;

  // Clean-up
  fRootFile = NULL;
  fRootTree = NULL;
};

