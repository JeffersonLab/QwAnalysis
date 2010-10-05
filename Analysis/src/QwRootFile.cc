#include "QwRootFile.h"
#include "QwHistogramHelper.h"
#include "QwRunCondition.h"

const Long64_t QwRootFile::kMaxTreeSize = 10000000000LL;


/**
 * Constructor with relative filename
 */
QwRootFile::QwRootFile(const TString& run_label)
: fEnableMapFile(kFALSE),fUpdateInterval(400)
{
  // Process the configuration options
  ProcessOptions(gQwOptions);

  // Check for the memory-mapped file flag
  if (fEnableMapFile) {

    // Set up map file
    TString mapfilename = getenv_safe_TString("QW_ROOTFILES");
    mapfilename += "/QwMemMapFile.map";
    fMapFile = new QwMapFile(mapfilename, "Memory Mapped File", "RECREATE");
    //Reload the tree trim new file for real time mode.
    gQwHists.LoadTreeParamsFromFile("Qweak_RT_Tree_Trim_List.in");
    //Reload the histo trim new file for real time mode.    
    gQwHists.LoadHistParamsFromFile("Qweak_RT_Hist_Trim_List.in");
    if (! fMapFile)
      QwError << "Memory-mapped file " << mapfilename
              << " could not be opened!" << QwLog::endl;
    else
      fMapFile->Print();

    //    // Disable tree in map file mode
    //    fDisableAllTrees = true; //Since we are using a tree within the map file we no longer scifically set this to false in RT mode

  // Otherwise we are in offline mode
  } else {

    TString rootfilename = getenv_safe_TString("QW_ROOTFILES");
    rootfilename += Form("/%s%s.root", fRootFileStem.Data(), run_label.Data());
    fRootFile = new TFile(rootfilename, "RECREATE", "QWeak ROOT file");
    if (! fRootFile) {
      QwError << "ROOT file " << rootfilename
              << " could not be opened!" << QwLog::endl;
    }
    else {

      TString run_condition_name = Form("%s_condition", run_label.Data());
      TList *run_cond_list = (TList*) fRootFile -> FindObjectAny(run_condition_name);
      if(not run_cond_list) { 
	QwRunCondition run_condition(
				     gQwOptions.GetArgc(), 
				     gQwOptions.GetArgv(),
				     Form("%s_condition", run_label.Data())
				     );
	
	fRootFile -> WriteObject(
				 run_condition.Get(),
				 run_condition.GetName()
				 );
      }
      //   delete run_cond_list;
    }
  }
}


/**
 * Destructor
 */
QwRootFile::~QwRootFile()
{
  // Close the map file
  if (fMapFile) {
    fMapFile->Close();
    delete fMapFile;
    fMapFile = 0;
  }

  // Close the ROOT file
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

  // Define the autoflush and autosave option (default values by ROOT)
  options.AddOptions()
    ("autoflush", po::value<int>()->default_value(-30000000),
     "TTree autoflush value");
  options.AddOptions()
    ("autosave", po::value<int>()->default_value(300000000),
     "TTree autosave value");

  options.AddOptions()
    ("circular-buffer", po::value<int>()->default_value(0),
     "Max. no.of entries to kept in the memory mapped tree");
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
  if (options.GetValue<bool>("disable-trees"))  DisableTree(".*");
  if (options.GetValue<bool>("disable-histos")) DisableHisto(".*");

  // Options 'disable-mps' and 'disable-hel' for disabling
  // helicity window and helicity pattern output
  if (options.GetValue<bool>("disable-mps"))  DisableTree("Mps_Tree");
  if (options.GetValue<bool>("disable-hel"))  DisableTree("Hel_Tree");

  // Options 'num-accepted-events' and 'num-discarded-events' for
  // prescaling of the tree output
  fNumEventsToSave = options.GetValue<int>("num-accepted-events");
  fNumEventsToSkip = options.GetValue<int>("num-discarded-events");

  // Update interval for the map file
  fUpdateInterval = options.GetValue<int>("mapfile-update-interval");

  fCircularBufferSize = options.GetValue<int>("circular-buffer");

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
}
