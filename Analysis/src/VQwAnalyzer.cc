#include "VQwAnalyzer.h"
ClassImp(VQwAnalyzer);

// Qweak headers
#include "QwSubsystemArray.h"

void VQwAnalyzer::OpenRootFile(Int_t run)
{
  // NOTE It seems that histogram and tree filling is thread safe, but we cannot
  // have a single ROOT file for every analyzer object.  Probably we can open the
  // ROOT file a level up in QwRoot, pass the pointer, and cd() into it in here
  // before we construct the histograms

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
}

void VQwAnalyzer::CloseRootFile()
{
  // Write ROOT file
  fRootFile->Write(0, TObject::kOverwrite);
  // Delete histograms
  if (kHisto) fDetectors->DeleteHistograms();
  // Close ROOT file
  fRootFile->Close();
  delete fRootFile;

  return;
}

