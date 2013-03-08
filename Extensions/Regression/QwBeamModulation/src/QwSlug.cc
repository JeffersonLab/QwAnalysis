#define QW_SLUG_CC
#include "../include/QwSlug.hh"
#include "../include/headers.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <string>
#include "TMatrixD.h"
#include "TVectorD.h"
#include "TH3.h"

QwSlug::QwSlug(TChain *tree):
  fNDetector(0), fNMonitor(0), fCurrentCut(100), 
  fRunRange(false), fAverageComputed(false),
  fQuartetAnalysis(false), fSingleRun(false),
  fSingleRunlet(false)
{
  fFileExtension = "_average";
  Init(tree);
}

QwSlug::~QwSlug()
{
  if (!fChain) return;
  delete fChain->GetCurrentFile();

}

Int_t QwSlug::GetEntry(Long64_t entry)
{
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t QwSlug::LoadTree(Long64_t entry)
{
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void QwSlug::Init(TChain *tree)
{
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);
   Notify();
}

Bool_t QwSlug::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void QwSlug::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}

void QwSlug::PrintError(TString error)
{

  std::cout << error << std::endl;
  return;
}

void QwSlug::GetOptions(Char_t **options)
{
  Int_t i = 0;
  size_t index = 0;

  while(options[i] != NULL){
    fOptions.push_back(options[i]);
    std::string option (fOptions[i].Data());
    
    if(option.compare(0, 6, "--runs") == 0){
//       fOptions.push_back(options[i + 1]);
      std::string option (fOptions[i + 1].Data());
      index = option.find_first_of(":", 1);
      fLowerRunNumber = atoi(option.substr(0, option.length() - index -1).c_str());
      fUpperRunNumber = atoi(option.substr(index + 1, option.length() - 1).c_str());
      fRunRange = true;
    }
    if(option.compare(0, 20, "--correct-by-quartet") == 0){
      std::cout << "Correcting data on a quartet-by-quartet basis." << std::endl;
      fQuartetAnalysis = true;
      fFileExtension = "_quartet";
    }
    if(option.compare(0, 13, "--run-in-slug") == 0){
      std::cout << "Analyzing single run in slug." << std::endl;
//       fOptions.push_back(options[i + 1]);
      fSingleRun = true;
      SetRunNumber(atoi(options[i + 1]));
    }

    if(option.compare(0, 16, "--runlet-in-slug") == 0){
      std::cout << "Analyzing single runlet in slug." << std::endl;
//       fOptions.push_back(options[i + 1]);
      fSingleRunlet = true;
      SetRunletNumber(atoi(options[i + 1]));
    }
    i++;
  }

  return;
}

void QwSlug::SetupBranchAddress()
{
  std::cout << "Setup Branch" << std::endl;

  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("ErrorFlag", 1);
  fChain->SetBranchStatus("run", 1);
  fChain->SetBranchStatus("runlet", 1);
  //  fChain->SetBranchStatus("diff_qwk_energy", 1);
  fChain->SetBranchStatus("diff_qwk_bpm3c12X", 1);
  fChain->SetBranchStatus("yield_qwk_charge", 1);
  fChain->SetBranchStatus("slug_std.regGlobErrorCode", 1);

  fChain->SetBranchAddress("run",&run, &b_run);
  fChain->SetBranchAddress("runlet",&runlet, &b_runlet);
//   fChain->SetBranchAddress("diff_qwk_energy",&diff_qwk_energy, &b_diff_qwk_energy);
  fChain->SetBranchAddress("diff_qwk_bpm3c12X",&diff_qwk_bpm3c12X, &b_diff_qwk_bpm3c12X);
  fChain->SetBranchAddress("ErrorFlag",&ErrorFlag, &b_ErrorFlag);
  fChain->SetBranchAddress("yield_qwk_charge",&yield_qwk_charge ,&b_yield_qwk_charge);
  fChain->SetBranchAddress("slug_std.regGlobErrorCode",&regGlobErrorCode ,&b_regGlobErrorCode);

  for(Int_t i = 0; i < fNDetector; i++){
    fChain->SetBranchStatus(Form("%s", DetectorList[i].Data()), 1);
    fChain->SetBranchAddress(Form("%s", DetectorList[i].Data()), &DetBranch[i]);
    fChain->SetBranchAddress(Form("reg_%s", DetectorList[i].Data()), &LRBDetBranch[i]);

    fChain->SetBranchStatus(Form("%s_Device_Error_Code", DetectorList[i].Data()), 1);
    fChain->SetBranchAddress(Form("%s_Device_Error_Code", DetectorList[i].Data()), &DetErrorCode);
  }
  for(Int_t i = 0; i < fNMonitor; i++){
    fChain->SetBranchStatus(Form("%s", MonitorList[i].Data()), 1);
    fChain->SetBranchAddress(Form("%s", MonitorList[i].Data()), &MonBranch[i]);
    fChain->SetBranchStatus(Form("%s_Device_Error_Code", MonitorList[i].Data()), 1);
    fChain->SetBranchAddress(Form("%s_Device_Error_Code", MonitorList[i].Data()), &MonErrorCode);
  }
}

void QwSlug::ReadSensitivities(void)
{

  std::string line;
  std::fstream sens_file;  
  char *token;

  Int_t n = -1;
  Int_t i = 0;

  sens_file.open("config/slopes.dat");
  if(!sens_file.is_open()){
    std::cout << "Error opening slopes file" << std::endl;
    exit(1);
  }

  fSensitivity.resize(fNDetector);
  fSensitivityError.resize(fNDetector);

  while(sens_file.good()){
    getline(sens_file, line);
    token = new char[line.size() + 1];
    strcpy(token, line.c_str());
    token = strtok(token, " ,");
    while(token){
      if(strncmp(token, "det", 1) == 0){
	token = strtok(NULL, " ,");
	n++;
	token = strtok(NULL, " ,");
      }
      else{
	if(n > -1){
	  fSensitivity[n].push_back(atof(token) );
	  token = strtok(NULL, " ,");

	  fSensitivityError[n].push_back(atof(token) );
	  token = strtok(NULL, " ,");
	}
      }
    }
  }
  for(Int_t det = 0; det < fNDetector; det++){
    for(Int_t mon = 0; mon < fNMonitor; mon++){
      std::cout << fSensitivity[det][mon] << "\t"
		<< fSensitivityError[det][mon]
		<< std::endl;
    }
  }
  sens_file.close();
  
  return;
}

void QwSlug::SetSensitivities(std::vector < std::vector <Double_t> > sensitivity, std::vector < std::vector <Double_t> > error)
{

  fSensitivity.resize(fNDetector);
  fSensitivityError.resize(fNDetector);

  for(Int_t det = 0; det < fNDetector; det++){
    fSensitivity[det].resize(fNMonitor);
    fSensitivityError[det].resize(fNMonitor);
    for(Int_t i = 0; i < fNMonitor; i++)
      {
	fSensitivity[det][i] = sensitivity[det][i];
	fSensitivityError[det][i] = error[det][i];
      }
  }
  
  return;
}

void QwSlug::SetSensitivities(std::vector < std::vector <Double_t> > sensitivity)
{
  fSensitivity.resize(fNDetector);

  for(Int_t det = 0; det < fNDetector; det++){
    fSensitivity[det].resize(fNMonitor);
    for(Int_t i = 0; i < fNMonitor; i++)
      {

	fSensitivity[det][i] = sensitivity[det][i];
      }
  }
  
  return;
}

void QwSlug::ComputePearsonCoefficient()
{

  if (fChain == 0) return;

  if(!fAverageComputed){
    std::cout << "Must compute averages first." << std::endl;
    exit(1);
  }
  correlations.ResizeTo(fNMonitor, fNMonitor);

  Long64_t nentries = fChain->GetEntries();

  TMatrixD s_ij(fNMonitor, fNMonitor);
  TVectorD var(fNMonitor);
  
   s_ij.Zero();
   var.Zero();

   std::cerr << "Compute Correlations.  Entries:\t" << nentries << std::endl;

   for (Long64_t entry = 0; entry < nentries; entry++) {

     LoadTree(entry);
     if (entry < 0) break;
     fChain->GetEntry(entry);

     if(entry % 100000 == 0) std::cout << "Analyzing entry:\t" << entry << std::endl;

     if(ErrorCodeCheck() == 0){
       for(Int_t i = 0; i < fNMonitor; i++){
	 for(Int_t j = 0; j < fNMonitor; j++){
	   s_ij(i, j) += (MonBranch[i][0] - fPositionDiff[i])*(MonBranch[j][0] - fPositionDiff[j]);
	 }
	 var[i] += TMath::Power((MonBranch[i][0] - fPositionDiff[i]), 2);
       }
     }
   }

   for(Int_t i = 0; i < fNMonitor; i++){
     for(Int_t j = 0; j < fNMonitor; j++){
       correlations(i,j) = s_ij(i,j)/( TMath::Sqrt(var[i])*TMath::Sqrt(var[j]) );
     }
   }

   correlations.Print();
   
  return;
}

void QwSlug::ComputeSlugCorrectedAsymmetry()
{

  Double_t error_temp = 0;
  Double_t correction = 0;
  Double_t temp = 0;

  fDetectorAsym.resize(fNDetector);
  fDetectorAsymError.resize(fNDetector);

  std::cout << "Correcting by slug averaged values." << std::endl; 

  for(Int_t det = 0; det < fNDetector; det++){
    for(Int_t mon = 0; mon < fNMonitor; mon++){
      correction += fPositionDiff[mon]*fSensitivity[det][mon]; 

      temp = ComputeProductError(fPositionDiff[mon], fSensitivity[det][mon], fPositionDiffError[mon], fSensitivityError[det][mon]);
      error_temp = ComputeSumError(temp, error_temp);
    }
    fDetectorAsym[det] = fRawDetectorAsym[det] - correction;
    fDetectorAsymError[det] = ComputeSumError(temp, fRawDetectorAsymError[det]);
    correction = 0;
  }

  return;
} 

Double_t QwSlug::ComputeProductError(Double_t x, Double_t y, Double_t xerr, Double_t yerr)
{
  Double_t square_error = 0;

  if( (x || y) == 0) return(0.0); 

  square_error = (TMath::Power(xerr/x, 2) + TMath::Power(yerr/y, 2));

  return( x*y*TMath::Sqrt(square_error) );
}

Double_t QwSlug::ComputeSumError(Double_t x, Double_t y)
{
  return( TMath::Sqrt( TMath::Power(x, 2) + TMath::Power(y, 2) ) );
}

void QwSlug::ComputeQuartetCorrectedAsymmetry()
{
  if (fChain == 0) return;

   fNEntries = fChain->GetEntries();
//    Int_t n = 0;

//    TH1D *histo = new TH1D("histo", "histo", 1000, -0.01, 0.01);
   std::vector <TH1D *> histo;

   for(Int_t det = 0; det < fNDetector; det++){
     histo.push_back(new TH1D(Form("histo%i", det), 
			      Form("histo%i", det), 
			      1000, -0.01, 0.01) );
   }

   Double_t correction = 0;

   std::cerr << "Making quartet-by-quartet corrections.  Entries:\t" << fNEntries << std::endl;

   fDetectorAsym.resize(fNDetector);
   fDetectorAsymError.resize(fNDetector);
   
   for (Long64_t entry = 0; entry < fNEntries; entry++) {

     LoadTree(entry);
     if (entry < 0) break;
     fChain->GetEntry(entry);

     if(entry % 100000 == 0) std::cout << "Analyzing entry:\t" << entry << std::endl;

     if(ErrorCodeCheck() == 0){
       for(Int_t det = 0; det < fNDetector; det++){
	 for(Int_t mon = 0; mon < fNMonitor; mon++){
 	   correction +=  fSensitivity[det][mon]*MonBranch[mon][0];
	 }
	 fDetectorAsym[det] = DetBranch[det][0] - correction;
	 histo[det]->Fill(fDetectorAsym[det]);
	 correction = 0;
       }
     }
   }
   for(Int_t det = 0; det < fNDetector; det++){
     fDetectorAsym[det]= histo[det]->GetMean();
     fDetectorAsymError[det] = (histo[det]->GetRMS())/TMath::Sqrt(fNEntries);
   }

   for(Int_t det = 0; det < fNDetector; det++){
     delete histo[det];
   }
}

void QwSlug::ComputeCorrectedAsymmetry()
{
  if (fChain == 0) return;

  TFile file(Form("$QW_ROOTFILES/reg_tree_%i.root", fRunNumber),"RECREATE");
//   TFile file(Form("reg_tree_%i.root", fRunNumber),"RECREATE");

  TTree *reg_tree = new TTree("Reg_Tree", "Regressed Analysis Results Tree");

  Double_t correction = 0;
  Double_t detector = 0;

  for(Int_t det = 0; det < fNDetector; det++){
    reg_tree->Branch(Form("reg_%s", DetectorList[det].Data()), &detector, Form("reg_%s/D", DetectorList[det].Data()));
    reg_tree->Branch(Form("%s", DetectorList[det].Data()), &DetBranch[det][0], Form("%s/D", DetectorList[det].Data()));
  }
  for(Int_t i = 0; i < fNMonitor; i++){
    reg_tree->Branch(Form("%s", MonitorList[i].Data()), &MonBranch[i][0], Form("%s/D", MonitorList[i].Data()));
  }
//   reg_tree->Branch("diff_qwk_energy",&diff_qwk_energy, "diff_qwk_energy/D");
  reg_tree->Branch("diff_qwk_bpm3c12X",&diff_qwk_bpm3c12X, "diff_qwk_bpm3c12X/D");
  reg_tree->Branch("run", &run, "run/D");
  reg_tree->Branch("runlet", &runlet, "runlet/D");

  reg_tree->Print();
  fNEntries = fChain->GetEntries();
//    Int_t n = 0;

  fDetectorAsym.resize(fNDetector);
  fDetectorAsymError.resize(fNDetector);

   std::cerr << "Making correction to individual run.  Entries:\t" << fNEntries << std::endl;

    for (Long64_t entry = 0; entry < fNEntries; entry++) {

     LoadTree(entry);
     if (entry < 0) break;
     fChain->GetEntry(entry);

     if(entry % 100000 == 0) std::cout << "Analyzing entry:\t" << entry << std::endl;

     if(ErrorCodeCheck() == 0){
       for(Int_t det = 0; det < fNDetector; det++){
	 for(Int_t mon = 0; mon < fNMonitor; mon++){
 	   correction +=  fSensitivity[det][mon]*MonBranch[mon][0];
	 }
	 detector = DetBranch[det][0] - correction;
	 correction = 0;
       }
       reg_tree->Fill();
     }
   }
   file.Write();
}

void QwSlug::SetRunNumber(Int_t run_number)
{
  fRunNumber = run_number;
}

void QwSlug::SetRunletNumber(Int_t runlet_number)
{
  fRunletNumber = runlet_number;
}

void QwSlug::ComputeAverages()
{
  if (fChain == 0) return;

   fNEntries = fChain->GetEntries();

   Int_t n = 0;

   std::vector <Double_t> mu_mon;
   std::vector <Double_t> mu_det;
   std::vector <Double_t> mu_det_lrb;

   fPositionDiff.resize(fNMonitor);
   fRawDetectorAsym.resize(fNDetector);
   fLRBDetectorAsym.resize(fNDetector);
   fPositionDiffError.resize(fNMonitor);
   fRawDetectorAsymError.resize(fNDetector);
   fLRBDetectorAsymError.resize(fNDetector);

   mu_mon.resize(fNMonitor);
   mu_det.resize(fNDetector);

   mu_det_lrb.resize(fNDetector);

   std::cerr << "Compute Averages.  Entries:\t" << fNEntries << std::endl;

   for (Long64_t entry = 0; entry < fNEntries; entry++) {

     LoadTree(entry);
     if (entry < 0) break;
     fChain->GetEntry(entry);

     if(entry % 100000 == 0) std::cout << "Analyzing entry:\t" << entry << std::endl;

     if(ErrorCodeCheck() == 0){
       for(Int_t mon = 0; mon < fNMonitor; mon++){
 	 fPositionDiff[mon] += MonBranch[mon][0];
	 if(n == 0){
	   fPositionDiffError[mon] = 0;
	   mu_mon[mon] = MonBranch[mon][0]; 
	 }
	 if(n > 0){
	   fPositionDiffError[mon] += (MonBranch[mon][0] - mu_mon[mon])*(MonBranch[mon][0] - (mu_mon[mon] + (MonBranch[mon][0] - mu_mon[mon])/n));  
	   mu_mon[mon] += MonBranch[mon][0];
	   mu_mon[mon] /= n;
	 }

       }

       for(Int_t det = 0; det < fNDetector; det++){
 	 fRawDetectorAsym[det] += DetBranch[det][0];
  	 fLRBDetectorAsym[det] += LRBDetBranch[det][0];
	 if(n == 0){
	   fRawDetectorAsymError[det] = 0;
	   mu_det[det] = DetBranch[det][0]; 
 	   mu_det_lrb[det] = LRBDetBranch[det][0]; 
	 }
	 if(n > 0){
	   fRawDetectorAsymError[det] += (DetBranch[det][0] - mu_det[det])*(DetBranch[det][0] - (mu_det[det] + (DetBranch[det][0] - mu_det[det])/n));  
 	   fLRBDetectorAsymError[det] += (LRBDetBranch[det][0] - mu_det_lrb[det])*(LRBDetBranch[det][0] - (mu_det_lrb[det] + (LRBDetBranch[det][0] - mu_det_lrb[det])/n));  

	   mu_det[det] += DetBranch[det][0];
	   mu_det[det] /= n;

 	   mu_det_lrb[det] += LRBDetBranch[det][0];
 	   mu_det_lrb[det] /= n;
	 }
       }

       n++;
     }
   }
   
   for(Int_t mon = 0; mon < fNMonitor; mon++){
      fPositionDiff[mon] /= n;
      fPositionDiffError[mon] /= (n - 1);
      // Variance
      fPositionDiffError[mon] = TMath::Sqrt(fPositionDiffError[mon]);
      // One more Sqrt(n) to get the error
      fPositionDiffError[mon] /= TMath::Sqrt(n);
   }

   for(Int_t det = 0; det < fNDetector; det++){
      fRawDetectorAsym[det] /= n;
      fRawDetectorAsymError[det] /= (n - 1);
      fLRBDetectorAsym[det] /= n;
      fLRBDetectorAsymError[det] /= (n - 1);
      // Variance
      fRawDetectorAsymError[det] = TMath::Sqrt(fRawDetectorAsymError[det]);
      fLRBDetectorAsymError[det] = TMath::Sqrt(fLRBDetectorAsymError[det]);
      // One more Sqrt(n) to get the error
      fRawDetectorAsymError[det] /= TMath::Sqrt(n);
      fLRBDetectorAsymError[det] /= TMath::Sqrt(n);
   }

    fAverageComputed = true;

}

Int_t QwSlug::ErrorCodeCheck()
{

  Int_t code = 0;


    for(Int_t i = 0; i < fNMonitor; i++){
      if( (Int_t)MonErrorCode[i] != 0 ){
	code = 1;
      }
    }
    for(Int_t i = 0; i < fNDetector; i++){
      if( (Int_t)DetErrorCode[i] != 0 ){
	code = 1;
      }
    }

    if( (UInt_t)ErrorFlag != 0)
      code = 1;

    if(yield_qwk_charge < fCurrentCut){
      code = 1;
    }

    if(regGlobErrorCode != 0){
      code = 1;
    }

    if(fSingleRun){
      if(run != fRunNumber){
	code = 1;
      }
    }

    if(fSingleRunlet){
      if(!fSingleRun){
	std::cout << "Must set run number." << std::endl;
	exit(1);
      }
      if(runlet != fRunletNumber){
	code = 1;
      }
    }
  return( code );
}

Bool_t QwSlug::FileSearch(TString filename, TChain *chain)
{

  TString file_directory;
  Bool_t c_status = kFALSE;

  file_directory = gSystem->Getenv("QW_ROOTFILES");

  c_status = chain->Add(Form("%s/%s",file_directory.Data(), filename.Data()));
  std::cout << "Trying to open :: "
            << Form("%s/%s",file_directory.Data(), filename.Data())
            << std::endl;

  if(c_status){
    TString chain_name = chain->GetName();
    TObjArray *fileElements = chain->GetListOfFiles();
    TIter next(fileElements);
    TChainElement *chain_element = NULL;

    while((chain_element = (TChainElement*)next())){
      std::cout << "Adding :: "
		<< filename
		<< " to data chain"
		<< std::endl;
    }
  } 
    return c_status;

}

void QwSlug::LoadRootFile(TString filename, TChain *tree)
{
  Bool_t found = FileSearch(filename, tree);
  
//   if(!found){
//     filename = Form("Qweak_%d.*.trees.root", run_number);
//     found = FileSearch(filename, tree);
//     std::cerr << "Couldn't find QwPass<#>_*.trees.root trying "
// 	      << filename
// 	      << std::endl;
    if(!found){
      std::cerr << "Unable to locate requested file :: "
		<< filename
		<< std::endl;
      exit(1);
    }
//   }
}

Int_t QwSlug::ReadConfig()
{
  std::string line;

  char *token;

  config.open("config/setup_slug.config", std::ios_base::in);
  if(!config.is_open()){
    std::cout << "Error opening config file" << std::endl;
    exit(1);
  }

  while(config.good()){
    getline(config, line);
    token = new char[line.size() + 1];
    strcpy(token, line.c_str());
    token = strtok(token, " ,.");
    while(token){
      if(strcmp("mon", token) == 0){

       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " .,"); 
       	std::cout << "\t\tMonitor is: " << token << std::endl;
	MonitorList.push_back(token); 
       }

      if(strcmp("det", token) == 0){

       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " .,"); 
       	std::cout << "\t\tDetector is: " << token << std::endl; 
	DetectorList.push_back(token);
      }
      else 
       	token = strtok(NULL, " .,"); 
    }
  }
  fNDetector = DetectorList.size();
  fNMonitor = MonitorList.size();

  config.close();

  return 0;
}
