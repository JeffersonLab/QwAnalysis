 #include "../include/headers.h"
#include "QwSlug.hh"
#include "QwDataContainer.hh"
#include "QwPlotHelper.hh"
#include <fstream>

Int_t main(Int_t argc, Char_t *argv[])
{

  TString filename;

  TChain *slug_tree = new TChain("slug");

  QwSlug *slug = new QwSlug(slug_tree);

  Int_t slug_number = 0;

  TString file_directory;

//   Int_t slug_list[4] = {33, 35, 37, 39};    // IN
//   Int_t slug_list[6] = {31, 32, 34, 36, 38, 40};    // OUT

  std::fstream slugf;
  std::fstream lrb_slugf;
  std::fstream bpmf;

  file_directory = gSystem->Getenv("QW_ROOTFILES");

  if(argv[1] == NULL){
    //    slug->PrintError("Error Loading:  no run number specified");
    exit(1);
  }
  slug->Init(slug_tree);
  slug->GetOptions(argv);

  if(slug->fRunRange){

    for(Int_t i = slug->fLowerRunNumber; i < (slug->fUpperRunNumber + 1); i++){
//     for(Int_t i = 0; i < 6; i++){
      filename = Form("slug%i.root", i);
//       filename = Form("slug%i.root", slug_list[i]);
      if(slug_tree->Add(Form("%s/%s",file_directory.Data(), filename.Data()))){
	slug_tree->AddFriend("slug_std = slug",Form("%s/%s_lrb_std.root", file_directory.Data(), filename.Data())); 
      }
      else{
	slug->PrintError(Form("Failed to find file:\t%s", filename.Data()));
      }
      std::cout << Form("Adding::\t%s", filename.Data()) << std::endl;
      filename.Clear();
    }
  }else{
    slug_number = atoi(argv[1]);
    filename = Form("slug%i.root", slug_number);
    slug->LoadRootFile(filename, slug_tree);
    slug_tree->AddFriend("slug_std = slug",Form("%s/slug%i_lrb_std.root", file_directory.Data(), slug_number)); 
  }

  slug->ReadConfig();
  std::cout << "Reading in sensitivities." << std::endl;
  slug->ReadSensitivities();
  slug->SetupBranchAddress();
//   slug->SetSensitivities(sens, error);
  std::cout << "Calculating Slug Averages..." << std::endl;
  slug->ComputeAverages();
  
  for(Int_t i = 0; i < slug->fNMonitor; i++){
    std::cout << Form("Average monitor [%s]:\t", slug->MonitorList[i].Data()) << slug->fPositionDiff[i] 
	      << " +- " << slug->fPositionDiffError[i] << std::endl;
  }
  for(Int_t i = 0; i < slug->fNDetector; i++){
    std::cout << Form("Average detector [%s]:\t", slug->DetectorList[i].Data()) << slug->fRawDetectorAsym[i] 
	      << " +- " << slug->fRawDetectorAsymError[i] << std::endl;
  }
  if(slug->fQuartetAnalysis) 
    slug->ComputeQuartetCorrectedAsymmetry();
  else 
    slug->ComputeSlugCorrectedAsymmetry();

  
  for(Int_t i = 0; i < slug->fNDetector; i++){
    std::cout << Form("Corrected detector [%s]:\t", slug->DetectorList[i].Data()) << slug->fDetectorAsym[i] 
 	      << " +- " << slug->fDetectorAsymError[i] << std::endl;
//     std::cout << Form("LRB Corrected detector [%s]:\t", slug->DetectorList[i].Data()) << slug->fLRBDetectorAsym[i] 
//  	      << " +- " << slug->fLRBDetectorAsymError[i] << std::endl;
  }


  if(slug->fRunRange){
//     slugf.open(Form("slug/slug_%i-%i%s_%s.dat", slug->fLowerRunNumber, 
// 		    slug->fUpperRunNumber, slug->fFileExtension.Data(), slug->DetectorList[0].Data()), std::fstream::out | std::fstream::app);
//     bpmf.open(Form("slug/bpm_%i-%i%s_%s.dat", slug->fLowerRunNumber, 
// 		   slug->fUpperRunNumber, slug->fFileExtension.Data(), slug->DetectorList[0].Data()), std::fstream::out | std::fstream::app);
//     //     slugf.open("slug/slug_wien0_out.dat", std::fstream::out | std::fstream::app);
  }else{
//     slugf.open(Form("slug/slug%s_%s.dat", slug->fFileExtension.Data(), slug->DetectorList[0].Data()), std::fstream::out | std::fstream::app);
//     lrb_slugf.open(Form("slug/lrb_slug%s_%s.dat", slug->fFileExtension.Data(), slug->DetectorList[0].Data()), std::fstream::out | std::fstream::app);
//     bpmf.open(Form("slug/bpm%s_%s.dat", slug->fFileExtension.Data(), slug->DetectorList[0].Data()), std::fstream::out | std::fstream::app);
    slugf.open(Form("slug/slug%s_%i.dat", slug->fFileExtension.Data(), slug_number), std::fstream::out | std::fstream::app);
    lrb_slugf.open(Form("slug/lrb_slug%s_%i.dat", slug->fFileExtension.Data(), slug_number), std::fstream::out | std::fstream::app);
    bpmf.open(Form("slug/bpm%s_%i.dat", slug->fFileExtension.Data(), slug_number), std::fstream::out | std::fstream::app);
  }
  if(!slugf.is_open() || !bpmf.is_open()){
    slug->PrintError("Error opening file.");
    exit(1);
  }
  for(Int_t i = 0; i < (Int_t)(slug->fNDetector); i++){
    slugf << slug_number << " " << slug->fNEntries << " " 
 	  << slug->fRawDetectorAsym[i] << " " << slug->fRawDetectorAsymError[i] << " " 
 	  << slug->fDetectorAsym[i]  << " " << slug->fDetectorAsymError[i] << std::endl;
    lrb_slugf << slug_number << " " << slug->fNEntries << " "
	      << slug->fRawDetectorAsym[i] << " " << slug->fRawDetectorAsymError[i] << " " 
	      << slug->fLRBDetectorAsym[i]  << " " << slug->fLRBDetectorAsymError[i] << std::endl;
  }
  for(Int_t i = 0; i < slug->fNMonitor; i++){
    bpmf << slug_number << " " << slug->fNEntries << " "
	 << slug->MonitorList[i].Data() << " "
	 << slug->fPositionDiff[i] << " "
	 << slug->fPositionDiffError[i] << std::endl;
  }
  slugf.close();
  lrb_slugf.close();
  bpmf.close();


  return 0;

}
