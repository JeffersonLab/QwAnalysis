#include "MiddleEarth.hh"
#include "headers.h"

Int_t main(Int_t argc, Char_t *argv[])
{

  TString filename;

  Int_t run_number;

  TChain * chain = new TChain("Mps_Tree");

  if(argv[1] == NULL) exit(1);

  run_number = atoi(argv[1]);
  MiddleEarth *grey_wizard = new MiddleEarth(chain);
  grey_wizard->run_number = run_number;
  
  filename = Form("QwPass1_%i.00[0-1].trees.root", run_number);

  grey_wizard->LoadRootFile(filename, chain);
  grey_wizard->ScanData();
  std::cout << "Finished scanning." << std::endl;

  if( !(grey_wizard->CheckFlags()) ){
    std::cout << "YOU SHALL NOT PASS!!!!!!" << std::endl;
    exit(1);
  }

  std::cout << "Rootfile is fine to be processed." << std::endl;

  return 0;
}
