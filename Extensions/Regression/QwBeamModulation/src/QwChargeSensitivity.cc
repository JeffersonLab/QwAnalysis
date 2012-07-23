#define QwChargeSensitivity_cxx
#include "../include/QwChargeSensitivity.hh"
#include "../include/headers.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#ifdef QwChargeSensitivity_cxx

const char QwChargeSensitivity::red[8] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
const char QwChargeSensitivity::other[8] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
const char QwChargeSensitivity::normal[8] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

QwChargeSensitivity::QwChargeSensitivity(TChain *tree):
  fInit(false), fNEvents(0), fNCycles(0)
{
   Init(tree);
}

QwChargeSensitivity::~QwChargeSensitivity()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t QwChargeSensitivity::GetEntry(Long64_t entry)
{
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t QwChargeSensitivity::LoadTree(Long64_t entry)
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

void QwChargeSensitivity::Init(TChain *tree)
{
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);
   Notify();

   fChain->SetBranchStatus("*", 0);
   fChain->SetBranchStatus("mps_counter", 1);
   fChain->SetBranchStatus("yield_ramp", 1);
   fChain->SetBranchStatus("yield_bm_pattern_number", 1);
   fChain->SetBranchStatus("ErrorFlag", 1);
   fChain->SetBranchStatus("asym_qwk_charge", 1);

   fChain->SetBranchAddress("mps_counter", &mps_counter, &b_mps_counter);
   fChain->SetBranchAddress("yield_ramp", &yield_ramp_hw_sum, &b_yield_ramp);
   fChain->SetBranchAddress("yield_bm_pattern_number", &yield_bm_pattern_number, &b_yield_bm_pattern_number);
   fChain->SetBranchAddress("ErrorFlag", &ErrorFlag, &b_ErrorFlag);
   fChain->SetBranchAddress("asym_qwk_charge", &asym_qwk_charge_hw_sum, &b_asym_qwk_charge);
}

Bool_t QwChargeSensitivity::Notify()
{
   return kTRUE;
}

void QwChargeSensitivity::Show(Long64_t entry)
{
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t QwChargeSensitivity::Cut(Long64_t entry)
{
   return 1;
}

#endif

Bool_t QwChargeSensitivity::FileSearch(TString filename, TChain *chain)
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

    while((chain_element = (TChainElement *)next())){
      std::cout << "Adding :: "
		<< filename
		<< " to data chain"
		<< std::endl;
    }
  } 
    return c_status;

}

void QwChargeSensitivity::LoadRootFile(TString filename, TChain *tree)
{
  Bool_t found = FileSearch(filename, tree);
  
  if(!found){
    filename = Form("Qweak_%d.00*.trees.root", run_number);
    found = FileSearch(filename, tree);
    std::cerr << "Couldn't find QwPass<#>_*.trees.root trying "
	      << filename
	      << std::endl;
    if(!found){
      std::cerr << "Unable to locate requested file :: "
		<< filename
		<< std::endl;
      exit(1);
    }
  }
}

void QwChargeSensitivity::CalculateChargeSlope()
{

  Double_t d_mean = 0;
  Double_t m_mean = 0;
  Double_t sigma_dd = 0;
  Double_t sigma_dm = 0;
  Double_t sigma_mm = 0;
  Double_t slope = 0;
  Double_t sigma_slope = 0;

  fNCycles++;

  if(fNEvents < 3){
    std::cout << red << "Error: no data to process!" << std::endl;
    return;
  }
  std::cout << "Calculating Charge nonlinearity. Number of events: " << fNEvents << std::endl;

  for(Int_t det = 0; det < fNDetector; det++){
    
    for(Int_t event = 0; event < fNEvents; event++) d_mean += DetectorData[det][event];
    d_mean /= fNEvents;
    
    for(Int_t event = 0; event < fNEvents; event++) m_mean += ChargeAsymData[event];
    m_mean /= fNEvents;
    
    for(Int_t event = 0; event < fNEvents; event++){
      sigma_dd += (DetectorData[det][event] - d_mean)*(DetectorData[det][event] - d_mean); 
      sigma_dm += (DetectorData[det][event] - d_mean)*(ChargeAsymData[event] - m_mean);
      sigma_mm += (ChargeAsymData[event] - m_mean)*(ChargeAsymData[event] - m_mean);
      
    }
    DetectorData[det].clear();
    ChargeAsymData.clear();

  slope = sigma_dm/sigma_mm;
  sigma_slope = TMath::Sqrt((sigma_dd - ( (sigma_dm*sigma_dm)/sigma_mm) )/(sigma_mm*( fNEvents -2 )));

  DetectorSlope[det].push_back(slope);
  DetectorSlopeError[det].push_back(sigma_slope);

  d_mean = 0;
  m_mean = 0;
  sigma_dd = 0;
  sigma_dm = 0;
  sigma_mm = 0;
  slope = 0;
  sigma_slope = 0;

  }

}

void QwChargeSensitivity::CalculateAverageSlope()
{

  Double_t temp_slope = 0;
  Double_t temp_sigma = 0;

  // Error Weighted average

  for(Int_t i = 0; i < fNDetector; i ++){
    for(Int_t j = 0; j < fNCycles; j++){
      temp_slope += DetectorSlope[i][j]/TMath::Power(DetectorSlopeError[i][j], 2);
      temp_sigma += 1/TMath::Power(DetectorSlopeError[i][j], 2);
    }
    if(temp_sigma > 0){
      temp_slope /= temp_sigma;
      temp_sigma = 1/temp_sigma;
    }

    AverageDetectorSlope.push_back(temp_slope);
    AverageDetectorSlopeError.push_back( TMath::Sqrt(temp_sigma) );
    temp_slope = 0;
    temp_sigma = 0;
  }

  // test the mofo

  for(Int_t i = 0; i < fNDetector; i ++){
    std::cout << red 
	      << AverageDetectorSlope[i] << " +- " 
	      << AverageDetectorSlopeError[i]
	      << normal << std::endl;
  }
}

void QwChargeSensitivity::BuildDetectorVectors()
{

  DetectorData.resize(fNDetector);
  DetectorSlope.resize(fNDetector);
  DetectorSlopeError.resize(fNDetector);

  // Need to set the branch addresses for the data.  Here is as good of a 
  // place as any...


   for(Int_t i = 0; i < fNDetector; i++){
     fChain->SetBranchStatus(DetectorList[i].Data(), 1);
     fChain->SetBranchAddress(DetectorList[i].Data(), &DetBranch[i]);
   }

  return;

}

void QwChargeSensitivity::ScanData(void)
{

   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntries();
   for(Long64_t i = 0; i < nentries; i++) {

     fChain->GetEntry(i);   
    
     if( (i % 10000) == 0) std::cout << "Processing:\t" << i << std::endl; 
     
     if(yield_ramp_hw_sum < 0 && i < nentries){
       std::cout << "Accumulating charge sensitivity data" << std::endl;
       do{
	 fChain->GetEntry(i);

 	 if(ErrorCodeCheck() != 0){
 	   //	   std::cout << "::Error::\t" << i << std::endl;
 	   i++;
 	   continue;
 	 }
	 
	 for(Int_t j = 0; j < fNDetector; j++){
	   DetectorData[j].push_back(DetBranch[j][0]);
	 }
	 ChargeAsymData.push_back(asym_qwk_charge_hw_sum);

	 fInit = true;
	 fNEvents++;
	 i++;
	 if(fNEvents > 42499){
	   std::cout << "entry number: " << i << std::endl;
	   CalculateChargeSlope();
	   fNEvents = 0;
	 }
       }while(yield_ramp_hw_sum < 0 && i < nentries);
     }
   }
   CalculateChargeSlope();
   fNEvents = 0;
}

Int_t QwChargeSensitivity::ErrorCodeCheck()
{

  Double_t subblock = 0;
  Int_t code = 0;

    subblock = ((yield_ramp_block3+yield_ramp_block0)-(yield_ramp_block2+yield_ramp_block1));
    
    for(Int_t i = 0; i < fNDetector; i++){
      if( (Int_t)DetBranch[i][fDeviceErrorCode] != 0 ){
	code = 1;
      }
    }
    if( !((subblock > -50) && (subblock < 50)) )
      code = 1;
    if(yield_qwk_mdallbars_Device_Error_Code != 0){
      code = 1;
    }
    if(ErrorFlag != 0){
      code = 1;
    }
  
  return( code );
}


Int_t QwChargeSensitivity::ReadConfig(QwChargeSensitivity *charge_sens)
{
  std::string line;

  char *token;
  TString temp;

  config.open("config/setup.config", std::ios_base::in);
  if(!config.is_open()){
    std::cout << red << "Error opening config file" << normal << std::endl;
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
       }
      if(strcmp("det", token) == 0){

       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " .,"); 
	temp = Form("asym_%s", token);
       	std::cout << red << "\t\tDetector is: " << temp << normal << std::endl; 
	charge_sens->DetectorList.push_back(temp);
      }
      else 
       	token = strtok(NULL, " .,"); 
    }
  }
  fNDetector = DetectorList.size();
  if( fNDetector > fNMaxDet )
    {
      std::cout << red << "Error :: Exceeded maximum number of detectors(monitors)" 
		<< other << "Detectors:\t" << fNDetector << "\tMax:\t" << normal << std::endl;
      exit(1);
    }

  config.close();

  return 0;
}

Bool_t QwChargeSensitivity::CheckFlags(void){

  if(fInit) return true;
  else 
    return false;

}

void QwChargeSensitivity::Write()
{
  
  charge_sens = fopen( Form("config/charge_sensitivity_%d.dat", run_number),"w");
  std::cout << other << "Writing non-linearity file. " << run_number << normal << std::endl;
  
  if(IfExists(Form("config/charge_sensitivity_%d.dat", run_number)) ){
    for(Int_t i = 0; i < fNDetector; i++){
      fprintf(charge_sens, "%-5.5e %-5.5e\n", AverageDetectorSlope[i], AverageDetectorSlopeError[i]); 
    }
  }
  else{
    std::cout << red << "Error opening charge sensitivity output" << normal << std::endl;
    exit(1);
  }
  fclose(charge_sens);

}

Bool_t QwChargeSensitivity::IfExists(const char *file)
{
  if(FILE *file_to_check = fopen(file, "r")){
    fclose(file_to_check);
    return true;
  }
  return false;
}
