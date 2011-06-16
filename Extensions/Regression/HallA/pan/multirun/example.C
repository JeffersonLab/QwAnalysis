//////////////////////////////////////////////////////////////////////
//
// example.C
//  Bryan Moffit - June 2005
//
//  ROOT macro to show how to run a typical multirun analysis.
//
//   Output rootfile: slug0.root
//   Input variable file (used by TaVarChooser): chooser.txt
//   Input Run/Slug list: slug0_runlist.txt
//

void example() 
{

  // Make sure the properly libraries are compiled and loaded.
  TString libs = gSystem->GetLibraries();
  if(!libs.Contains("TaVarChooser"))
    gROOT->LoadMacro("multirun/TaVarChooser.C+");
  if(!libs.Contains("TaPairSelector"))
    gROOT->LoadMacro("multirun/TaPairSelector.C+");
  if(!libs.Contains("TaMakePairFile"))
    gROOT->LoadMacro("multirun/TaMakePairFile.C+");

  // Start and Finish the analysis.
  TaMakePairFile *mpf = new TaMakePairFile("slug0.root",
					   "multirun/chooser.txt");
  mpf->SetRunList("slug0_runlist.txt");
  mpf->RunLoop();
  mpf->Finish();



}
