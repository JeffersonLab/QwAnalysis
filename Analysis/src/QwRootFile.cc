#include "QwRootFile.h"


const Long64_t QwRootFile::kMaxTreeSize = 10000000000LL;


/**
 * Constructor with relative filename
 */
QwRootFile::QwRootFile(const TString& run_label)
: fOnline(kFALSE),
  fEnableTree(kTRUE),fEnableHisto(kTRUE),
  fEnableMps(kTRUE),fEnableHel(kTRUE),
  fUpdateInterval(400)
{
  // Process the configuration options
  ProcessOptions(gQwOptions);

  // Check for the 'online' flag
  if (fOnline) {

    // Set up map file
    TString mapfilename = getenv_safe_TString("QW_ROOTFILES");
    mapfilename += "/QwMemMapFile.map";
    fMapFile = new QwMapFile(mapfilename, "Memory Mapped File", "RECREATE");
    if (! fMapFile)
      QwError << "Memory-mapped file " << mapfilename
              << " could not be opened!" << QwLog::endl;

  // Otherwise we are in offline mode
  } else {

    TString rootfilename = getenv_safe_TString("QW_ROOTFILES");
    rootfilename += Form("/Qweak_%s.root", run_label.Data());
    fRootFile = new TFile(rootfilename, "RECREATE", "QWeak ROOT file");
    if (! fRootFile)
      QwError << "ROOT file " << rootfilename
              << " could not be opened!" << QwLog::endl;

  }
}


/**
 * Destructor
 */
QwRootFile::~QwRootFile()
{
  if (fMapFile) {
    fMapFile->Close();
    delete fMapFile;
    fMapFile = 0;
  }

  if (fRootFile) {
    fRootFile->Close();
    delete fRootFile;
    fRootFile = 0;
  }
}

/**
 * Defines configuration options using QwOptions functionality.
 * @param options Options object
 */
void QwRootFile::DefineOptions(QwOptions &options)
{
  // Define the histogram and tree options
  options.AddOptions()
    ("disable-trees", po::value<bool>()->default_value(false)->zero_tokens(),
     "disable output to trees");
  options.AddOptions()
    ("disable-histos", po::value<bool>()->default_value(false)->zero_tokens(),
     "disable output to histograms");

  // Define the helicity window versus helicity pattern options
  options.AddOptions()
    ("disable-mps", po::value<bool>()->default_value(false)->zero_tokens(),
     "disable helicity window output (block, hwsum)");
  options.AddOptions()
    ("disable-hel", po::value<bool>()->default_value(false)->zero_tokens(),
     "disable helicity pattern output (yield, asymmetry)");

  // Define the tree output prescaling options
  options.AddOptions()
    ("num-accepted-events", po::value<int>()->default_value(0),
     "number of accepted consecutive events");
  options.AddOptions()
    ("num-discarded-events", po::value<int>()->default_value(0),
     "number of discarded consective events");
}


/**
 * Parse the configuration options and store in class fields
 * @param options Options object
 */
void QwRootFile::ProcessOptions(QwOptions &options)
{
  // Option 'online' defined in QwEventBuffer, but accessible everywhere
  fOnline = options.GetValue<bool>("online");

  // Options 'disable-trees' and 'disable-histos' for disabling
  // tree and histogram output
  fEnableTree  = ! options.GetValue<bool>("disable-trees");
  fEnableHisto = ! options.GetValue<bool>("disable-histos");

  // Options 'disable-mps' and 'disable-hel' for disabling
  // helicity window and helicity pattern output
  fEnableMps = ! options.GetValue<bool>("disable-mps");
  fEnableHel = ! options.GetValue<bool>("disable-hel");

  // Options 'num-accepted-events' and 'num-discarded-events' for
  // prescaling of the tree output
  fNumEventsToSave = options.GetValue<int>("num-accepted-events");
  fNumEventsToSkip = options.GetValue<int>("num-discarded-events");
  fNumEventsCycle = fNumEventsToSave + fNumEventsToSkip;
}


/**
 * Construct the histogram of the subsystem array
 * @param detectors Subsystem array
 */
void QwRootFile::ConstructHistograms(QwSubsystemArray& detectors)
{
  // Return if we do not want histo or mps information
  if (! fEnableHisto) return;
  if (! fEnableMps) return;

  // Create the histograms in a directory
  if (fRootFile) {
    fRootFile->cd();
    detectors.ConstructHistograms(fRootFile->mkdir("mps_histo"));
  }

  // No support for directories in a map file
  if (fMapFile) {
    detectors.ConstructHistograms();
  }
}


/**
 * Construct the histogram of the helicity pattern
 * @param helicity_pattern Helicity pattern
 */
void QwRootFile::ConstructHistograms(QwHelicityPattern& helicity_pattern)
{
  // Return if we do not want histo or hel information
  if (! fEnableHisto) return;
  if (! fEnableHel) return;

  // Create the histograms in a directory
  if (fRootFile) {
    fRootFile->cd();
    helicity_pattern.ConstructHistograms(fRootFile->mkdir("hel_histo"));
  }

  // No support for directories in a map file
  if (fMapFile) {
    helicity_pattern.ConstructHistograms();
  }
}


/**
 * Construct the tree branches of the subsystem array
 * @param detectors Subsystem array
 */
void QwRootFile::ConstructTreeBranches(QwSubsystemArrayParity& detectors)
{
  // Return if we do not want tree or mps information
  if (! fEnableTree) return;
  if (! fEnableMps) return;

  // Go to top level directory
  cd();

  // Create tree
  fMpsTree = new TTree("Mps_Tree", "MPS event data tree");
  fMpsTree->SetMaxTreeSize(kMaxTreeSize);

  // Reserve space for vector
  // If one defines more than 6000 words in the full ntuple
  // results are going to get very very crazy.
  fMpsVector.reserve(6000);

  // Associate branches with vector
  TString dummystr = "";
  detectors.ConstructBranchAndVector(fMpsTree, dummystr, fMpsVector);
}


/**
 * Construct the tree branches of the helicity pattern
 * @param helicity_pattern Helicity pattern
 */
void QwRootFile::ConstructTreeBranches(QwHelicityPattern& helicity_pattern)
{
  // Return if we do not want tree or hel information
  if (! fEnableTree) return;
  if (! fEnableHel) return;

  // Go to top level directory
  cd();

  // Create tree
  fHelTree = new TTree("Hel_Tree", "Helicity event data tree");
  fHelTree->SetMaxTreeSize(kMaxTreeSize);

  // Reserve space for vector
  // If one defines more than 6000 words in the full ntuple
  // results are going to get very very crazy.
  fHelVector.reserve(6000);

  // Associate branches with vector
  TString dummystr = "";
  helicity_pattern.ConstructBranchAndVector(fHelTree, dummystr, fHelVector);
}


/**
 * Fill the tree branches of the subsystem array
 * @param detectors Subsystem array
 */
void QwRootFile::FillTreeBranches(QwSubsystemArrayParity& detectors)
{
  // Return if we do not want tree or mps information
  if (! fEnableTree) return;
  if (! fEnableMps) return;

  // Output ROOT tree prescaling
  // One cycle starts with fNumEventsToTake acc
  UInt_t current_event = detectors.GetCodaEventNumber() % fNumEventsCycle;
  if (current_event > fNumEventsToSave) return;

  // Fill the vector
  detectors.FillTreeVector(fMpsVector);
  // Fill the tree
  fMpsTree->Fill();
}


/**
 * Fill the tree branches of the helicity pattern
 * @param helicity_pattern Helicity pattern
 */
void QwRootFile::FillTreeBranches(QwHelicityPattern& helicity_pattern)
{
  // Return if we do not want tree or hel information
  if (! fEnableTree) return;
  if (! fEnableHel) return;

  // Fill the vector
  helicity_pattern.FillTreeVector(fHelVector);
  // Fill the tree
  fHelTree->Fill();
}
