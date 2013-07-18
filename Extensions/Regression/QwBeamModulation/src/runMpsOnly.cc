#include "headers.h"
#include "QwMpsOnly.hh"
#include <iostream>

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

  filename = Form("%s_%d*.root", mps_only->fFileStem.Data(), mps_only->run_number);
  mps_only->LoadRootFile(filename, mps_tree, 0);
  mps_only->SetFileName(filename);
  mps_only->SetupMpsBranchAddress();
  std::cout<<"Entries: "<<mps_tree->GetEntries()<<std::endl;
  std::cout << "Setting Branch Addresses of detectors/monitors" << std::endl;

  mps_only->ReadConfig("");

  mps_only->Scan();

  std::cout << "Finished scanning data -- building relevant data vectors" << std::endl;
  mps_only->BuildDetectorData();
  mps_only->BuildMonitorData();
  if(mps_only->MakeRampFilled(1)) 
    return 1;
  mps_only->BuildCoilData();
  mps_only->BuildDetectorSlopeVector();
  mps_only->BuildMonitorSlopeVector();

  std::cout << "Starting to pilfer the data" << std::endl;
  mps_only->PilferData();
  mps_only->BuildDetectorAvSlope();
  mps_only->BuildMonitorAvSlope();
  mps_only->CalculateWeightedSlope(1);
  mps_only->MatrixFill();
  
  std::cout << "Closing Mps_Tree" << std::endl;
  delete mps_tree;
  mps_only->Clean();


  // The second half isn't ready yet. So exit.
  exit(1);

  std::cout << "Creating Slug(let) chain now." << std::endl;
  TChain *hel_tree = new TChain("slug");
  mps_only->Init(hel_tree);

  filename = Form("sluglet%d_*root", mps_only->run_number);
  mps_only->LoadRootFile(filename, hel_tree, true);
  mps_only->SetFileName(filename);

  mps_only->SetupHelBranchAddress();
  mps_only->ReadConfig("hel");

  std::cout << "Calculating Corrections" << std::endl;
  mps_only->ComputeAsymmetryCorrections();

  return 0;

}
