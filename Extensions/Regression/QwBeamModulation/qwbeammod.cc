#include "headers.h"
#include "QwModulation.hh"

Int_t main(Int_t argc, Char_t *argv[])
{

  TString filename;

  TChain *mps_tree = new TChain("Mps_Tree");

  QwModulation *modulation = new QwModulation(mps_tree);
  modulation->SetupMpsBranchAddress();

  if(argv[1] == NULL){
    modulation->PrintError("Error Loading:  no run number specified");
    exit(1);
  }

  modulation->run.push_back(atoi(argv[1]));
  for(Int_t i = 0; i < (Int_t)modulation->run.size(); i++){
    modulation->run_number = modulation->run[i];
    filename = Form("QwPass*_%d*.trees.root", modulation->run[i]);
    modulation->LoadRootFile(filename, mps_tree);
    modulation->SetFileName(filename);
  }

  std::cout << "Setting Branch Addresses of detectors/monitors" << std::endl;

  modulation->ReadConfig(modulation);
  modulation->Scan(modulation);
  modulation->fNumberEvents = mps_tree->GetEntries();

  std::cout << "Finished scanning data -- building relevant data vectors" << std::endl;
  modulation->BuildDetectorData();
  modulation->BuildMonitorData();
  modulation->BuildCoilData();
  modulation->BuildDetectorSlopeVector();
  modulation->BuildMonitorSlopeVector();

  std::cout << "Starting to pilfer the data" << std::endl;
  modulation->PilferData();
  modulation->BuildDetectorAvSlope();
  modulation->BuildMonitorAvSlope();
  modulation->CalculateWeightedSlope();
  modulation->MatrixFill();
  
  std::cout << "Casting Level 10 death on Mps_Tree!!!!" << std::endl;
  delete mps_tree;
  modulation->Clean();

  std::cout << "Creating Hel_Tree chain now." << std::endl;
  TChain *hel_tree = new TChain("Hel_Tree");
  modulation->Init(hel_tree);

  for(Int_t i = 0; i < (Int_t)modulation->run.size(); i++){
    filename = Form("QwPass*_%d*.trees.root", modulation->run[i]);
    modulation->LoadRootFile(filename, hel_tree);
    modulation->SetFileName(filename);
  }
  std::cout << "Setup Hell!" << std::endl;
  modulation->SetupHelBranchAddress();
  std::cout << "What!What! Corrections Bitches!" << std::endl;
  modulation->ComputeYieldCorrections();

  return 0;

}
