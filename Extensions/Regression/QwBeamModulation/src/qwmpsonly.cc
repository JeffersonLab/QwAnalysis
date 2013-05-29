#include "headers.h"
#include "QwModulation.hh"
#include "QwMpsOnly.hh"

Int_t main(Int_t argc, Char_t *argv[])
{

  TString filename;

  TChain *mps_tree = new TChain("mps_slug");

  QwMpsOnly *mps_only = new QwMpsOnly(mps_tree);

  mps_only->output = gSystem->Getenv("BMOD_OUT");
  if(!gSystem->OpenDirectory(mps_only->output)){
    mps_only->PrintError("Cannot open output directory.\n");
    mps_only->PrintError("Directory needs to be set as env variable and contain:\n\t slopes/ \n\t regression/ \n\t diagnostics/ \n\t rootfiles/"); 
    exit(1);
  }

  mps_only->GetOptions(argv);

  if( !(mps_only->fRunNumberSet) ){
    mps_only->PrintError("Error Loading:  no run number specified");
    exit(1);
  }

  // This is the default filename format.  If doing a segment analysis
  // it is changes in QwModulation::FileSearch()

  //  filename = Form("QwPass*_%d*.trees.root", modulation->run_number);

  filename = Form("%s_%d*.root", mps_only->fFileStem.Data(), mps_only->run_number);
  mps_only->LoadRootFile(filename, mps_tree);
  mps_only->SetFileName(filename);
  mps_only->SetupMpsBranchAddress();

  std::cout << "Setting Branch Addresses of detectors/monitors" << std::endl;

  mps_only->ReadConfig("");

  mps_only->Scan();
//   modulation->fNumberEvents = mps_tree->GetEntries();

  std::cout << "Finished scanning data -- building relevant data vectors" << std::endl;
  mps_only->BuildDetectorData();
  mps_only->BuildMonitorData();

  //
  // This could be done more cleverly ... anyway these need to be initialized before
  // checking the error codes.
  //

  mps_tree->SetBranchStatus("ramp_block0", 1);   
  mps_tree->SetBranchStatus("ramp_block1", 1);   
  mps_tree->SetBranchStatus("ramp_block2", 1);   
  mps_tree->SetBranchStatus("ramp_block3", 1);   

  mps_only->BuildCoilData();
  mps_only->BuildDetectorSlopeVector();
  mps_only->BuildMonitorSlopeVector();

  std::cout << "Starting to pilfer the data" << std::endl;
  mps_only->PilferData();
  mps_only->BuildDetectorAvSlope();
  mps_only->BuildMonitorAvSlope();
  mps_only->CalculateWeightedSlope();
  mps_only->MatrixFill();
  
  std::cout << "Closing Mps_Tree" << std::endl;
  delete mps_tree;
  mps_only->Clean();


  // The second half isn't ready yet. So exit.
  //  exit(1);

  std::cout << "Creating Slug(let) chain now." << std::endl;
  TChain *hel_tree = new TChain("slug");
  mps_only->Init(hel_tree);

  gSystem->Setenv("QW_ROOTFILES", "/w/hallc/qweak/QwAnalysis/run1/pass5b_slugs/");

  filename = Form("sluglet%d_*root", mps_only->run_number);
  mps_only->LoadRootFile(filename, hel_tree, true);
  mps_only->SetFileName(filename);

  mps_only->SetupHelBranchAddress();
  mps_only->ReadConfig("hel");

  std::cout << "Calculating Corrections" << std::endl;
  mps_only->ComputeAsymmetryCorrections();

  return 0;

}