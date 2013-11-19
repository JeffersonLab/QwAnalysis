#include "headers.h"
#include "QwDataQuality.hh"

Int_t main(Int_t argc, Char_t *argv[])
{
  // TApplication theApp("App", &argc, argv);

  TString filename;

  TChain *tree = new TChain("tree");

  QwDataQuality *qwdata = new QwDataQuality(tree);

  if(!argv[1]){
    std::cerr << "Must specify filename.  Exiting." << std::endl;
    exit(1);
  }

  filename = argv[1];

  std::cout << "Opening: " << filename << std::endl;

  if(!gSystem->OpenDirectory(gSystem->Getenv("QW_ROOTFILES"))){
    std::cerr << "You must set $QW_ROOTFILES enviroment variable. Exiting." << std::endl;
    exit(1);
  }

  qwdata->LoadRootFile(filename, tree);

  // qwdata->SetTitle("BCM12DD by Wien");
  // qwdata->SetXaxisTitle("bcm12dd");
  // qwdata->SetYaxisTitle("counts");

  // qwdata->HistoByWien("diff_bcmdd12/value", 1);

  qwdata->SetTitle("BCM12DD by Wien");
  qwdata->SetYaxisTitle("bcm12dd");
  qwdata->SetXaxisTitle("runlet number");

  qwdata->PlotByWien("diff_bcmdd12/value", 1);

  // theApp.Run();
  
  return 0;

}
