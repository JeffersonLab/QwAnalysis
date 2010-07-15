#include "QwRootFile.h"


const Long64_t QwRootFile::kMaxTreeSize = 10000000000LL;


/**
 * Constructor with relative filename
 */
QwRootFile::QwRootFile(const TString& run_label)
: fEnableMapFile(kFALSE),
  fEnableTree(kTRUE),fEnableHisto(kTRUE),
  fEnableMps(kTRUE),fEnableHel(kTRUE),
  fUpdateInterval(400)
{
  // Process the configuration options
  ProcessOptions(gQwOptions);

  // Check for the memory-mapped file flag
  if (fEnableMapFile) {

    // Set up map file
    TString mapfilename = getenv_safe_TString("QW_ROOTFILES");
    mapfilename += "/QwMemMapFile.map";
    fMapFile = new QwMapFile(mapfilename, "Memory Mapped File", "RECREATE");
    if (! fMapFile)
      QwError << "Memory-mapped file " << mapfilename
              << " could not be opened!" << QwLog::endl;
    else
      fMapFile->Print();

    //    // Disable tree in map file mode
    //    fEnableTree = false; //Since we are using a tree within the map file we no longer scifically set this to false in RT mode

  // Otherwise we are in offline mode
  } else {

    TString rootfilename = getenv_safe_TString("QW_ROOTFILES");
    rootfilename += Form("/%s%s.root", fRootFileStem.Data(), run_label.Data());
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
  // Define the ROOT filename stem
  options.AddOptions("Default options")
    ("rootfile-stem", po::value<std::string>()->default_value("Qweak_"),
     "stem of the output ROOT filename");

  // Define the memory map option
  options.AddOptions()
    ("enable-mapfile", po::value<bool>()->default_value(false)->zero_tokens(),
     "enable output to memory-mapped file");

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
  options.AddOptions()
    ("mapfile-update-interval", po::value<int>()->default_value(400),
     "Events between a map file update");
   options.AddOptions()("trim-tree",
                       po::value<std::string>()->default_value("tree_trim.in"),
                       "Contains subsystems/elements to be included in the tree");

  // Define the autoflush and autosave option (default values by ROOT)
  options.AddOptions()
    ("autoflush", po::value<int>()->default_value(-30000000),
     "TTree autoflush value");
  options.AddOptions()
    ("autosave", po::value<int>()->default_value(300000000),
     "TTree autosave value");
}


/**
 * Parse the configuration options and store in class fields
 * @param options Options object
 */
void QwRootFile::ProcessOptions(QwOptions &options)
{
  // Option 'root-stem' to specify ROOT file stem
  fRootFileStem = TString(options.GetValue<std::string>("rootfile-stem"));

  // Option 'mapfile' to enable memory-mapped ROOT file
  fEnableMapFile = options.GetValue<bool>("enable-mapfile");

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

  //Update interval for the map file
  fUpdateInterval = options.GetValue<int>("mapfile-update-interval");

  // Autoflush and autosave
  fAutoFlush = options.GetValue<int>("autoflush");
  if ((ROOT_VERSION_CODE < ROOT_VERSION(5,26,00)) && fAutoFlush != -30000000){
    std::cout << QwLog::endl;
    QwWarning << "QwRootFile::ProcessOptions:  "
	      << "The 'autoflush' flag is not supported by ROOT version "
	      << ROOT_RELEASE
	      << QwLog::endl;
  }
  fAutoSave  = options.GetValue<int>("autosave");

  fTreeTrim_Filename = options.GetValue<std::string>("trim-tree").c_str();
  QwMessage << "Tree trim definition file " << fTreeTrim_Filename << QwLog::endl;
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
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,26,00)
  fMpsTree->SetAutoFlush(fAutoFlush);
#endif
  fMpsTree->SetAutoSave(fAutoSave);

  // Reserve space for vector
  // If one defines more than 6000 words in the full ntuple
  // results are going to get very very crazy.
  fMpsVector.reserve(6000);

  // Associate branches with vector
  TString dummystr = "";
  if (fEnableMapFile){
    //Access the tree trimming definition file
    QwParameterFile trim_tree(fTreeTrim_Filename);
    detectors.ConstructBranch(fMpsTree, dummystr, trim_tree);
  }
  else
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
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,26,00)
  fHelTree->SetAutoFlush(fAutoFlush);
#endif
  fHelTree->SetAutoSave(fAutoSave);

  // Reserve space for vector
  // If one defines more than 6000 words in the full ntuple
  // results are going to get very very crazy.
  fHelVector.reserve(6000);

  // Associate branches with vector
  TString dummystr = "";

  if (fEnableMapFile){
    //Access the tree trimming definition file
    QwParameterFile trim_tree(fTreeTrim_Filename);
    helicity_pattern.ConstructBranch(fHelTree, dummystr, trim_tree);
  }
  else
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
  // One cycle starts with fNumEventsToTake accepted events
  if (fNumEventsCycle > 0) {
    fCurrent_event = detectors.GetCodaEventNumber() % fNumEventsCycle;
    if (fCurrent_event > fNumEventsToSave) return;
  }

  // Fill the vector
  if (!fEnableMapFile)
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
  //helicity_pattern.FillTreeVector(fHelVector);

  // Output ROOT tree prescaling

  // One cycle starts with fNumEventsToTake accepted events

  if (fNumEventsCycle > 0) {
    if (fCurrent_event > fNumEventsToSave) return;
  }


  // Fill the tree
  if (!fEnableMapFile)
    helicity_pattern.FillTreeVector(fHelVector);
  // Fill the tree
  fHelTree->Fill();
}
