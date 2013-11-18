#include "headers.h"
#include "QwDataQuality.hh"

Int_t main(Int_t argc, Char_t *argv[])
{

  TString filename;

  TChain *tree = new TChain("tree");

  return 1;

  QwDataQuality *qwdata = new QwDataQuality(tree);

  filename = argv[1];

  std::cout << "Opening: " << filename << std::endl;

  // qwdata->LoadRootFile(filename, tree);
  
  return 0;

}
