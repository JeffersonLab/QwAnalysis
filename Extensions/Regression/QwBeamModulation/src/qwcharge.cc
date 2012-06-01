#include "../include/QwChargeSensitivity.hh"
#include "../include/headers.h"

Int_t main(Int_t argc, Char_t *argv[])
{

  TString filename;

  Int_t run_number;

  TChain * chain = new TChain("Hel_Tree");

  if(argv[1] == NULL) exit(1);

  run_number = atoi(argv[1]);
  QwChargeSensitivity *nonlinearity = new QwChargeSensitivity(chain);
  nonlinearity->run_number = run_number;
  
  filename = Form("QwPass*_%i.00*.trees.root", run_number);

  nonlinearity->LoadRootFile(filename, chain);
  nonlinearity->ReadConfig(nonlinearity);
  nonlinearity->BuildDetectorVectors();
  nonlinearity->ScanData();
  std::cout << "Finished scanning." << std::endl;
  nonlinearity->CalculateAverageSlope();
//   if( !(nonlinearity->CheckFlags()) ){
//     std::cout << "YOU SHALL NOT PASS!!!!!!" << std::endl;
//     exit(1);
//   }

  std::cout << "Charge sensitivity calculated" << std::endl;
  nonlinearity->Write();
  return 0;
}
