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

// mdall
//   Double_t sens[5] = {-0.001501, 77.79, -0.001439, 90.25e-6, 5.832};
//   Double_t error[5] = {0.00002025, 1.056, 7.5146e-6, 27.48e-6, 1.084};

// md1barsum
//  Double_t sens[5] = {-0.007246, 50.23, -0.001531, -12.79e-6, 3.23};
//  Double_t error[5] = {0.00007591, 3.31, 6.341e-6, 53.99e-6, 2.20};

// md2barsum
//   Double_t sens[5] = {-6410e-6, 27.99, -0.0010722, 4520e-6, 34.15};
//   Double_t error[5] = {80.22e-6, 3.39, 6.44e-6, 58.35e-6, 2.19};

// md3barsum
//   Double_t sens[5] = {-0.001589, 68.46, -0.001290, 5817e-6, 38.82};
//   Double_t error[5] = {0.00005215, 2.39, 7.0731e-6, 63.86e-6, 2.28};

// md4barsum
//   Double_t sens[5] = {2127e-6, 118, -0.001629, 3411e-6, 28.85};
//   Double_t error[5] = {68.83e-6, 3.63, 9.37e-6, 61.12e-6, 2.69};

// md5barsum
//   Double_t sens[5] = {4290e-6, 120.2, -1570e-6, 44.57e-6, 7.99};
//   Double_t error[5] = {59.5e-6, 3.18, 7.8e-6, 67.31e-6, 2.56};

// md6barsum
//   Double_t sens[5] = {2599e-6, 120.1, -1636.6e-6, -2977e-6, 25.19};
//   Double_t error[5] = {72.65e-6, 3.6, 8.78e-6, 56.45e-6, 2.29};

// md7barsum
//   Double_t sens[5] = {-729.9e-6, 80.7, -0.0013878, -5367e-6, -30.08};
//   Double_t error[5] = {63.5e-6, 2.76, 7.0731e-6, 65.56e-6, 2.56};

// md8barsum
//   Double_t sens[5] = {-5210e-6, 41.3, -1314e-6, -3951e-6, -16.07};
//   Double_t error[5] = {73.15e-6, 3.02, 7.83e-6, 51.51e-6, 2.33};

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
