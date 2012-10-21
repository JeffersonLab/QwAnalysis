#define QwModulation_cxx
#include "../include/QwModulation.hh"
#include "../include/headers.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#ifdef QwModulation_cxx

const char QwModulation::red[8] = { 0x1b, '[', '1', ';', '3', '1', 'm', 0 };
const char QwModulation::other[8] = { 0x1b, '[', '1', ';', '3', '2', 'm', 0 };
const char QwModulation::normal[8] = { 0x1b, '[', '0', ';', '3', '9', 'm', 0 };

QwModulation::QwModulation(TChain *tree):
  fXModulation(0),fYModulation(3),fEModulation(2),
  fXPModulation(1),fYPModulation(4), fNEvents(0),
  fReduceMatrix_x(0), fReduceMatrix_y(0), fReduceMatrix_xp(0),
  fReduceMatrix_yp(0), fReduceMatrix_e(0), fSensHumanReadable(0),
  fNModType(5), fPedestal(0), fXNevents(0), fXPNevents(0), 
  fENevents(0),fYNevents(0), fYPNevents(0),fCurrentCut(40),
  fXinit(false), fYinit(false), fEinit(false), fXPinit(false), 
  fYPinit(false), fSingleCoil(false), fRunNumberSet(false), 
  fPhaseConfig(false), fFileSegment(""), fFileStem("QwPass*") 
{
   Init(tree);
}

QwModulation::~QwModulation()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t QwModulation::GetEntry(Long64_t entry)
{
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t QwModulation::LoadTree(Long64_t entry)
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

void QwModulation::ReadChargeSensitivity(){

  std::string line;
  char *token;

  charge_sens.open(fChargeFile, fstream::in);

  if(!charge_sens.is_open()){
    PrintError("Error opening charge sensitivies file."); 
    exit(1);
  }

  while(charge_sens.good()){
    getline(charge_sens, line);
    token = new char[line.size() + 1];
    strcpy(token, line.c_str());
    token = strtok(token, " ,");
    while(token){
      ChargeSensitivity.push_back(atof(token));
      token = strtok(NULL, " ,");
      ChargeSensitivityError.push_back(atof(token));
      token = strtok(NULL, " ,");
    }
  }
  charge_sens.close();
  return;

}

void QwModulation::GetOptions(Char_t **options){
  Int_t i = 0;

  TString flag;

  while(options[i] != NULL){
    flag = options[i];

    if(flag.CompareTo("--run", TString::kExact) == 0){
      std::string option(options[i+1]);
      flag.Clear();
      fRunNumberSet = true;
      run_number = atoi(options[i + 1]);

      std::cout << other << "Processing run number:\t" 
		<< run_number
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--phase-config", TString::kExact) == 0){
      std::string option(options[i+1]);
      flag.Clear();
      fPhaseConfig = true;
      ReadPhaseConfig(options[i + 1]);
      std::cout << other << "Setting external phase values:\t" 
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--charge-sens", TString::kExact) == 0){
      fCharge = true;
      flag.Clear();
      fChargeFile = Form("config/charge_sensitivity_%i.dat", run_number);
      std::cout << other << "Setting up pseudo 5+1 analysis:\t" << fChargeFile << normal << std::endl;
      ReadChargeSensitivity();
    }    

    if(flag.CompareTo("--file-segment", TString::kExact) == 0){
      std::string option(options[i+1]);
      fFileSegmentInclude = true;

      flag.Clear();
      fFileSegment = options[i + 1];
      Int_t index = option.find_first_of(":");
      fLowerSegment = atoi(option.substr(0, index).c_str());
      fUpperSegment = atoi(option.substr(index + 1, option.length()-index).c_str());

      std::cout << other << "Processing file segments:\t" 
		<< fLowerSegment << ":" 
		<< fUpperSegment << normal << std::endl;
    }

    if(flag.CompareTo("--file-stem", TString::kExact) == 0){
      std::string option(options[i+1]);
      fFileStem = true;

      flag.Clear();
      fFileStem = options[i + 1];

      std::cout << other << "Setting file stem to:\t" 
		<< fFileStem << ":" 
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--ramp-pedestal", TString::kExact) == 0){
      std::string option(options[i+1]);
      flag.Clear();
      fPedestal = atoi(options[i + 1]);

      std::cout << other << "Setting ramp pedestal to:\t" 
		<< fPedestal
		<< normal << std::endl;
    }    

    if(flag.CompareTo("--charge", TString::kExact) == 0){
      fCharge = true;
      flag.Clear();
      if( IfExists(options[i + 1]) ){
	flag = options[i + 1];
	fChargeFile = flag;
	std::cout << other << "Setting up pseudo 5+1 analysis w/ external nonlinearity file:\t" << flag << normal << std::endl;
	ReadChargeSensitivity();
      }
      else{
	PrintError("Could not open inpust file.  Disabling charge sensitivity analysis");
	fCharge = false;
      }
    }
    if(flag.CompareTo("--current-cut", TString::kExact) == 0){
      flag.Clear();
      fCurrentCut = atoi(options[i + 1]);
      std::cout << other << "Setting current-cut to:\t" << fCurrentCut << normal << std::endl;
    }
    if(flag.CompareTo("--help", TString::kExact) == 0){
      printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      printf("Usage: ./qwbeammod <run_number> <options>");
      printf("\n\t--charge-sens \t\tinclude charge sensitivity in overall correction to physics asymmetry.");
      printf("\n\t--charge \t\tsame as --chare-sesn except use can specify path of charge sensitivities.");
      printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      exit(0);
    }        
    i++;
  }
}

Int_t QwModulation::GetCurrentCut()
{
  return(fCurrentCut);
}
void QwModulation::SetupMpsBranchAddress()
{

   fChain->SetBranchStatus("*", 0);
   fChain->SetBranchStatus("qwk_charge", 1);
   fChain->SetBranchStatus("bm_pattern_number", 1);
   fChain->SetBranchStatus("event_number", 1);
   fChain->SetBranchStatus("ErrorFlag", 1);
   fChain->SetBranchStatus("ramp", 1);
   fChain->SetBranchStatus("fgx1", 1);
   fChain->SetBranchStatus("fgx2", 1);
   fChain->SetBranchStatus("fge", 1);
   fChain->SetBranchStatus("fgy2", 1);
   fChain->SetBranchStatus("fgy1", 1);

   fChain->SetBranchAddress("qwk_charge", &qwk_charge_hw_sum, &b_qwk_charge);
   fChain->SetBranchAddress("bm_pattern_number", &bm_pattern_number, &b_bm_pattern_number);
   fChain->SetBranchAddress("event_number", &event_number, &b_event_number);
   fChain->SetBranchAddress("ErrorFlag", &ErrorFlag, &b_ErrorFlag);
   fChain->SetBranchAddress("fgx1", &fgx1_hw_sum, &b_fgx1);
   fChain->SetBranchAddress("fgy1", &fgy1_hw_sum, &b_fgy1);
   fChain->SetBranchAddress("fgx2", &fgx2_hw_sum, &b_fgx2);
   fChain->SetBranchAddress("fgy2", &fgy2_hw_sum, &b_fgy2);
   fChain->SetBranchAddress("fge", &fge_hw_sum, &b_fge);
   fChain->SetBranchAddress("ramp", &ramp_hw_sum, &b_ramp);

}

void QwModulation::SetupHelBranchAddress()
{

  TString fPrefix[fNMaxMon] = {"diff_", "diff_", "diff_", "diff_", "diff_"};

  fChain->SetBranchStatus("*", 0);
  fChain->SetBranchStatus("yield_qwk_charge", 1);
  fChain->SetBranchStatus("yield_bm_pattern_number", 1);
  fChain->SetBranchStatus("mps_counter", 1);
  fChain->SetBranchStatus("yield_ramp", 1);
  fChain->SetBranchStatus("ErrorFlag", 1);
  fChain->SetBranchStatus("yield_qwk_charge", 1);
  fChain->SetBranchStatus("asym_qwk_charge", 1);

  fChain->SetBranchAddress("yield_qwk_charge",&yield_qwk_charge_hw_sum ,&b_yield_qwk_charge);
  fChain->SetBranchAddress("yield_bm_pattern_number",&yield_bm_pattern_number,&b_yield_bm_pattern_number);
  fChain->SetBranchAddress("mps_counter",&mps_counter,&b_mps_counter);
  fChain->SetBranchAddress("yield_ramp",&yield_ramp_hw_sum,&b_yield_ramp);
  fChain->SetBranchAddress("ErrorFlag",&ErrorFlag, &b_ErrorFlag);
  fChain->SetBranchAddress("yield_qwk_mdallbars", &yield_qwk_mdallbars_hw_sum, &b_yield_qwk_mdallbars);
  fChain->SetBranchAddress("asym_qwk_charge", &asym_qwk_charge_hw_sum, &b_asym_qwk_charge);

  for(Int_t i = 0; i < (Int_t)(DetectorList.size()); i++){
    HDetectorList.push_back(Form("asym_%s", DetectorList[i].Data()));
    fChain->SetBranchStatus(Form("asym_%s", DetectorList[i].Data()), 1);
    fChain->SetBranchAddress(Form("asym_%s", DetectorList[i].Data()),&HDetBranch[i]);
  }
  for(Int_t i = 0; i < (Int_t)(MonitorList.size()); i++){
    HMonitorList.push_back(Form("%s%s", fPrefix[i].Data(), MonitorList[i].Data()));
    fChain->SetBranchStatus(Form("%s%s", fPrefix[i].Data(), MonitorList[i].Data()), 1);
    fChain->SetBranchAddress(Form("%s%s", fPrefix[i].Data(), MonitorList[i].Data()),&HMonBranch[i]);

    YMonitorList.push_back(Form("yield_%s", MonitorList[i].Data()));
    fChain->SetBranchStatus(Form("yield_%s", MonitorList[i].Data()), 1);
    fChain->SetBranchAddress(Form("yield_%s", MonitorList[i].Data()),&YMonBranch[i]);

    std::cout << "Monitors: " << HMonitorList[i] << "\t" << YMonitorList[i] << std::endl;
  }
}

void QwModulation::Init(TChain *tree)
{
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);
   Notify();
}

Bool_t QwModulation::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void QwModulation::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t QwModulation::Cut(Long64_t entry)
{
   return 1;
}

void QwModulation::ReduceMatrix(Int_t i)
{
  //
  //  i = 0, gives X, Y, E in the matrix
  //  i = 1, gives X, X',E in the matrix
  //
  switch(i){
  case 0:
    fReduceMatrix_xp = 1;
    fReduceMatrix_yp = 1;
    fYModulation = 1;
    fNModType = 3;
    break;

  case 1:
    fReduceMatrix_y  = 1;
    fReduceMatrix_yp = 1;
    fNModType = 3;
    break;

  default:
    exit(1);
    break;
  }
  return;
}

void QwModulation::SetHuman()
{
  fSensHumanReadable = 1;
  std::cout << "Setting Human Readble Mode" << std::endl;
  return;
}

Int_t QwModulation::ErrorCodeCheck(TString type)
{

  Double_t subblock = 0;

  Int_t bmodErrorFlag = 0;

  if( type.CompareTo("mps_tree", TString::kIgnoreCase) == 0 ){
    subblock = ((ramp_block3+ramp_block0)-(ramp_block2+ramp_block1));
    
    for(Int_t i = 0; i < fNMonitor; i++){
      if( (Int_t)MonBranch[i + 1][fDeviceErrorCode] != 0 ){
 	bmodErrorFlag = 1;
      }
    }
    for(Int_t i = 0; i < fNDetector; i++){
      if( (Int_t)DetBranch[i][fDeviceErrorCode] != 0){
	bmodErrorFlag = 1;
      }
    }
    if(qwk_charge_Device_Error_Code != 0){
      bmodErrorFlag = 1;
    }

    if(qwk_charge_hw_sum < fCurrentCut){
      bmodErrorFlag = 1;
    }

    if( !((subblock > -50) && (subblock < 50)) )
      bmodErrorFlag = 1;
    if( (ramp_hw_sum > fPedestal) && ((UInt_t)ErrorFlag != 0x4018080)  ){

#ifdef __VERBOSE_ERRORS

      std::cout << red << "Mps Tree::Modulation ErrorFlag" << normal << std::endl;

#endif

      bmodErrorFlag = 1;
    }

    if( (ramp_hw_sum < fPedestal) && ((UInt_t)ErrorFlag != 0) ){

#ifdef __VERBOSE_ERRORS

      std::cout << red << "Mps Tree::Natural Motion ErrorFlag" << normal << std::endl;

#endif

      bmodErrorFlag = 1;
    }

  }
  if( type.CompareTo("hel_tree", TString::kIgnoreCase) == 0 ){
    subblock = ((yield_ramp_block3+yield_ramp_block0)-(yield_ramp_block2+yield_ramp_block1));
    
    for(Int_t i = 0; i < fNMonitor; i++){
      if( (Int_t)HMonBranch[i][fDeviceErrorCode] != 0 ){
	bmodErrorFlag = 1;
      }
    }
    for(Int_t i = 0; i < fNDetector; i++){
      if( (Int_t)HDetBranch[i][fDeviceErrorCode] != 0 ){
	bmodErrorFlag = 1;
      }
    }
//     if( !((subblock > -50) && (subblock < 50)) )
//       bmodErrorFlag = 1;
//     if(yield_qwk_mdallbars_Device_Error_Code != 0){
//       bmodErrorFlag = 1;
//     }

    if( ((UInt_t)ErrorFlag != 0) && ((UInt_t)ErrorFlag != 67207296) ){
//     if( ((UInt_t)ErrorFlag != 0) ){
      bmodErrorFlag = 1;
    }
    
    if(yield_qwk_charge_hw_sum < fCurrentCut){
      bmodErrorFlag = 1;
    }


  }
  
  return( bmodErrorFlag );
}

void QwModulation::ComputeErrors(TMatrixD Y, TMatrixD eY, TMatrixD A, TMatrixD eA)
{
  std::cout << "============================(Trying) to compute the damn errors!============================" << std::endl;
  TMatrixD var(fNMonitor, fNModType);
  TMatrixD temp(fNModType, fNMonitor);
  TMatrixD Errorm(fNDetector, fNModType);  
  TMatrixD Errord(fNDetector, fNModType);  
  TMatrixD Error(fNDetector, fNModType);  

    for(Int_t i = 0; i < fNMonitor; i++){
      for(Int_t k = 0; k < fNModType; k++){
	for(Int_t n = 0; n < fNModType; n++){
	  for(Int_t j = 0; j < fNModType; j++){
	    var(i, k) += TMath::Power(A(i, n), 2)*TMath::Power(eA(n, j), 2)*TMath::Power(A(j, k), 2);
	  }
	}
      }
    }
       
    for(Int_t m = 0; m < fNDetector; m++){    

      for(Int_t i = 0; i < fNMonitor; i++){
	for(Int_t j = 0; j < fNModType; j++){
 	  Errorm(m, i) += var(j, i)*TMath::Power( Y(m, j),2);
	}
      }
 
       for(Int_t i = 0; i < fNModType; i++)
         Errorm(m, i) = TMath::Sqrt(Errorm(m, i));
       for(Int_t i = 0; i < fNMonitor; i++){
	 for(Int_t j = 0; j < fNModType; j++){
	   Errord(m, i) += TMath::Power( A(j, i),2)*TMath::Power(eY(m, j) ,2);
	 }
       }
    
       for(Int_t i = 0; i < fNModType; i++)
	 Errord(m,i) = TMath::Sqrt(Errord(m,i));
    
     for(Int_t i = 0; i < fNModType; i++){
       Error(m, i) = TMath::Power(Errord(m, i), 2) + TMath::Power(Errorm(m, i), 2);
       Error(m, i) = TMath::Sqrt(Error(m, i));
       YieldSlopeError[m][i] = Error(m, i);
     }

    }
    std::cout << other << "Errors?!" << normal << std::endl;
    Error.Print();
}

void QwModulation::MatrixFill()
{

  TMatrixD AMatrix(fNDetector, fNModType);
  TMatrixD AMatrixE(fNDetector, fNModType);

  Double_t determinant;

  CheckFlags();

  diagnostic.open(Form("%s/diagnostics/diagnostic%s_%i.dat", output.Data(), fFileSegment.Data(), run_number) , fstream::out);

  for(Int_t j = 0; j < fNDetector; j++){
    diagnostic << DetectorList[j] << std::endl;
    for(Int_t k = 0; k < fNModType; k++){
      AMatrix(j,k) = AvDetectorSlope[k][j];
      AMatrixE(j,k) = AvDetectorSlopeError[k][j];
      if( (diagnostic.is_open() && diagnostic.good()) ){
	diagnostic << AvDetectorSlope[k][j] << " +- " 
		   << AvDetectorSlopeError[k][j] << " ";
      }
    }
    diagnostic << std::endl;
  }
  diagnostic << std::endl;
  std::cout << "\t\t\t\t::A Matrix::"<< std::endl;
  AMatrix.Print("%11.10f");

  std::cout << "\t\t\t\t::A Matrix Error::"<< std::endl;
  AMatrixE.Print("%11.10f");

  TMatrixD RMatrix(fNMonitor, fNModType);
  TMatrixD RMatrixE(fNMonitor, fNModType);

  for(Int_t j = 0; j < fNMonitor; j++){
    for(Int_t k = 0; k < fNModType; k++){
      RMatrix(j,k) = AvMonitorSlope[k][j];
      RMatrixE(j,k) = AvMonitorSlopeError[k][j];

      if( (diagnostic.is_open() && diagnostic.good()) ){
	diagnostic << AvMonitorSlope[k][j] << " +- " 
		   << AvMonitorSlopeError[k][j] << " ";
      }
    }
    diagnostic << std::endl;
  }
  std::cout << "\t\t\t\t::R Matrix:: " << std::endl;
  RMatrix.Print("%11.10f");
  
  std::cout << "\t\t\t\t::R Matrix Error::"<< std::endl;
  RMatrixE.Print("%11.10f");
  
  TMatrixD RMatrixInv = RMatrix;
  RMatrixInv.Invert(&determinant);
  std::cout << "\t\t\t\t::R Matrix Inverse:: " << std::endl;
  RMatrixInv.Print("%11.10f");

  std::cout << determinant << std::endl;
  TMatrixD Identity(fNMonitor, fNMonitor);
  Identity.Mult(RMatrixInv, RMatrix);
  Identity.Print();

  TMatrixD SMatrix(fNDetector, fNModType);
  SMatrix.Mult(AMatrix, RMatrixInv);

  std::cout << "\n\n\t\t\t\t::SMatrix::\n" << std::endl;
  SMatrix.Print();

  ComputeErrors(AMatrix, AMatrixE, RMatrixInv, RMatrixE);

  for(Int_t i = 0; i < fNDetector; i++){
    for(Int_t j = 0; j < fNModType; j++){
      YieldSlope[i][j] = SMatrix(i,j);
    }
  }
  Write();

  if(fSensHumanReadable == 1){
    std::cout << "Exiting after sensitivities." << std::endl;
    exit(1);
  }
}

void QwModulation::ComputeAsymmetryCorrections()
{
  //**************************************************************
  //
  // Time to calculate some Corrections 
  //
  //**************************************************************

  TFile file(Form("%s/rootfiles/bmod_tree%s_%i.root", output.Data(), fFileSegment.Data(), run_number),"RECREATE");

  TTree *mod_tree = new TTree("Mod_Tree", "Modulation Analysis Results Tree");

  Int_t fEvCounter = 0;

  Double_t temp_correction = 0;
  Double_t monitor_correction[fNMaxDet][fNMaxMon];

  correction.resize(fNDetector);
  mod_tree->Branch("mps_counter", &mps_counter, "mps_counter/D"); 
  mod_tree->Branch("yield_qwk_charge", &yield_qwk_charge_hw_sum, "yield_qwk_charge/D"); 

  mod_tree->Branch("yield_ramp_block0", &yield_ramp_block0, "yield_ramp_block0/D"); 
  mod_tree->Branch("yield_ramp_block1", &yield_ramp_block1, "yield_ramp_block1/D"); 
  mod_tree->Branch("yield_ramp_block2", &yield_ramp_block2, "yield_ramp_block2/D"); 
  mod_tree->Branch("yield_ramp_block3", &yield_ramp_block3, "yield_ramp_block3/D"); 

  mod_tree->Branch("yield_bm_pattern_number", &yield_bm_pattern_number, "yield_bm_pattern_number/D"); 
  mod_tree->Branch("yield_ramp", &yield_ramp_hw_sum, "yield_ramp_hw_sum/D"); 
  mod_tree->Branch("ErrorFlag", &ErrorFlag, "ErrorFlag/D"); 
  mod_tree->Branch("yield_qwk_mdallbars_Device_Error_Code", &yield_qwk_mdallbars_Device_Error_Code, "yield_qwk_mdallbars_Device_Error_Code/D"); 
  mod_tree->Branch("yield_qwk_mdallbars", &yield_qwk_mdallbars_hw_sum, "yield_qwk_mdallbars/D"); 
  mod_tree->Branch("asym_qwk_charge", &asym_qwk_charge_hw_sum, "asym_qwk_charge/D"); 

  for(Int_t i = 0; i < fNDetector; i++){
    mod_tree->Branch(HDetectorList[i], &HDetBranch[i][0], Form("%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("raw_%s",HDetectorList[i].Data()), &HDetBranch[i][0], Form("raw_%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("corr_%s", HDetectorList[i].Data()), &AsymmetryCorrection[i], Form("corr_%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("raw_corr_%s", HDetectorList[i].Data()), &AsymmetryCorrection[i], Form("raw_corr_%s/D", HDetectorList[i].Data())); 
    if(fCharge){
      mod_tree->Branch(Form("corr_%s_charge", HDetectorList[i].Data()), &AsymmetryCorrectionQ[i], 
		       Form("corr_%s_charge/D", HDetectorList[i].Data())); 
    }
    mod_tree->Branch(Form("%s_Device_Error_Code", HDetectorList[i].Data()), &HDetBranch[i][fDeviceErrorCode], 
		     Form("%s_Device_Error_Code/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("correction_%s", DetectorList[i].Data()), &correction[i], Form("correction_%s/D", DetectorList[i].Data())); 

    if(fCharge){
      mod_tree->Branch(Form("correction_%s_charge", DetectorList[i].Data()), &correction[i], 
		       Form("correction_%s_charge/D", DetectorList[i].Data())); 
    }
    std::cout << HDetectorList[i] << std::endl;
  }
  for(Int_t j = 0; j < fNMonitor; j++){
    mod_tree->Branch(HMonitorList[j], &HMonBranch[j][0], Form("%s/D", HMonitorList[j].Data())); 
    mod_tree->Branch(Form("raw_%s", HMonitorList[j].Data()), &HMonBranch[j][0], Form("raw_%s/D", HMonitorList[j].Data())); 
    mod_tree->Branch(Form("%s_Device_Error_Code", HMonitorList[j].Data()), &HMonBranch[j][fDeviceErrorCode], 
		     Form("%s_Device_Error_Code/D", HMonitorList[j].Data())); 

    mod_tree->Branch(YMonitorList[j], &YMonBranch[j][0], Form("%s/D", YMonitorList[j].Data())); 
    mod_tree->Branch(Form("%s_Device_Error_Code", YMonitorList[j].Data()), &YMonBranch[j][fDeviceErrorCode], 
		     Form("%s_Device_Error_Code/D", YMonitorList[j].Data())); 
    std::cout << HMonitorList[j] << "\t" << YMonitorList[j] << std::endl;
  }

  for(Int_t i = 0; i < fNDetector; i++){
    for(Int_t j = 0; j < fNMonitor; j++){
      mod_tree->Branch(Form("corr_%s_%s", HDetectorList[i].Data(), HMonitorList[j].Data()), &monitor_correction[i][j], 
		       Form("corr_%s_%s/D", HDetectorList[i].Data(), HMonitorList[j].Data())); 
    }
  }

  if(fChain == 0) return;
  Long64_t nentries = fChain->GetEntries();

  std::cout << other << "Entries in Hel_Tree:\t" 
	    << nentries << normal << std::endl;

  for(Long64_t i = 0; i < nentries; i++){
    LoadTree(i);
    if(i < 0) break;
    fChain->GetEntry(i);
    ++fEvCounter;
    
    if( (ErrorCodeCheck("hel_tree") == 0) ){
      for(Int_t j = 0; j < fNDetector; j++){
	for(Int_t k = 0; k < fNMonitor; k++){
  	  temp_correction += YieldSlope[j][k]*HMonBranch[k][0]; 
	  monitor_correction[j][k] = YieldSlope[j][k]*HMonBranch[k][0];
	  if(fCharge) 
	  if( (i % 100000) == 0 ){}
	}
   	correction[j] = temp_correction;                                  

	if(fCharge) correction_charge[j] = temp_correction + ChargeSensitivity[j]*asym_qwk_charge_hw_sum;                                  
	if(fCharge) AsymmetryCorrectionQ[j] = HDetBranch[j][0] - correction_charge[j];

	else
	  AsymmetryCorrection[j] = HDetBranch[j][0] - correction[j];

	temp_correction = 0;
      }
      mod_tree->Fill();
    }

    if( (i % 100000) == 0 )std::cout << "Processing:\t" << i << std::endl;
  }

  file.Write();
  file.Close();

  return;
}


void QwModulation::CalculateWeightedSlope()
{

  Double_t mean = 0;
  Double_t mean_error = 0;

  for(Int_t i = 0; i < fNModType; i++){
    for(Int_t j = 0; j < fNDetector; j++){
      for(Int_t k = 0; k < (Int_t)DetectorSlope[i][j].size(); k++){
	mean += ( DetectorSlope[i][j][k]/(TMath::Power(DetectorSlopeError[i][j][k],2)) );
        mean_error += (1/TMath::Power(DetectorSlopeError[i][j][k],2));
      }
      if(mean_error > 0){
	mean /= mean_error;
	AvDetectorSlope[i].push_back(mean);
	AvDetectorSlopeError[i].push_back(TMath::Sqrt(1/mean_error));
	mean = 0;
	mean_error = 0;
      }
      else{ 
	mean = 0;
	mean_error = 0;
	std::cout << "\n[empty]Detector Weighted Mean:= "<< mean << " +/- " << mean_error << std::endl; 
	AvDetectorSlope[i].push_back(mean);
	AvDetectorSlopeError[i].push_back(mean_error);

      }
    }
  }
  for(Int_t i = 0; i < fNModType; i++){
    for(Int_t j = 0; j < fNMonitor; j++){
      for(Int_t k = 0; k < (Int_t)MonitorSlope[i][j].size(); k++){
	mean += ( MonitorSlope[i][j][k]/TMath::Power(MonitorSlopeError[i][j][k],2) );
        mean_error += (1/TMath::Power(MonitorSlopeError[i][j][k],2));
      }
      if(mean_error > 0){
	mean /= mean_error;
	AvMonitorSlope[i].push_back(mean);
	AvMonitorSlopeError[i].push_back(TMath::Sqrt(1/mean_error));

	mean = 0;
	mean_error = 0;
      }
      else{
	mean = 0;
	mean_error = 0;
	std::cout << "Monitor Weighted Mean:= "<< mean << " / " << mean_error << std::endl; 
	AvMonitorSlope[i].push_back(mean);
	AvMonitorSlopeError[i].push_back(mean_error);
      }
    }
  }

  DetectorSlope.clear();
  DetectorSlopeError.clear();
  MonitorSlope.clear();
  MonitorSlopeError.clear();

  std::cout << "Weighted Averages calculated." << std::endl;

}

void QwModulation::CalculateSlope(Int_t fNModType)
{

  Double_t c_mean = 0;
  Double_t d_mean = 0;
  Double_t sigma_cc = 0;
  Double_t sigma_dc = 0;
  Double_t sigma_dd = 0;
  Double_t sigma_slope = 0;
  Double_t slope = 0;


  if(!fPhaseConfig){
    Double_t temp[5]={0.26, 0.26, 0.0, 1.08, 1.08};              
    SetPhaseValues(temp); 
  }

  if(fNEvents < 3){
    std::cout << red << "Error in run:: Number of good events too small, exiting." << normal << std::endl;
    return;
  }
  
  if(CoilData[fNModType].size() <= 0){
    std::cout << "!!!!!!!!!!!!!!!!! Illegal Coil vector length:\t" << CoilData[fNModType].size() << std::endl;
    return;
  }
   
  for(Int_t det = 0; det < fNDetector; det++){
    
    if(DetectorData[det].size() <= 0){
      std::cout << "!!!!!!!!!!!!!!!!! Illegal Detector vector length:\t" << DetectorData[det].size() << std::endl;
      return;
    }

   //*******************************

    if(fNModType == fXModulation)  fXNevents += fNEvents;
    if(fNModType == fYModulation)  fYNevents += fNEvents;
    if(fNModType == fEModulation)  fENevents += fNEvents;
    if(fNModType == fXPModulation) fXPNevents += fNEvents;
    if(fNModType == fYPModulation) fYPNevents += fNEvents;

    //*******************************
   
    for(Int_t evNum = 0; evNum < fNEvents; evNum++) c_mean += TMath::Sin( (TMath::Pi()/180)*0.091548*CoilData[fNModType][evNum] + phase[fNModType]);
      c_mean /=fNEvents;
      
      for(Int_t evNum = 0; evNum < fNEvents; evNum++) d_mean += DetectorData[det][evNum];
      d_mean /=fNEvents;

      for(Int_t evNum = 0; evNum < fNEvents; evNum++){
	sigma_cc += (TMath::Sin( (TMath::Pi()/180)*0.091548*CoilData[fNModType][evNum] + phase[fNModType] ) - c_mean)*(TMath::Sin( (TMath::Pi()/180)*0.091548*CoilData[fNModType][evNum] + phase[fNModType] ) - c_mean);
	sigma_dc += (DetectorData[det][evNum] - d_mean)*(TMath::Sin( (TMath::Pi()/180)*0.091548*CoilData[fNModType][evNum] + phase[fNModType] ) - c_mean);
	sigma_dd += (DetectorData[det][evNum] - d_mean)*(DetectorData[det][evNum] - d_mean);

	// Clear instances after computation
	DetectorData[det].clear();
    }

      slope = sigma_dc/sigma_cc;
      sigma_slope = TMath::Sqrt((sigma_dd - ( (sigma_dc*sigma_dc)/sigma_cc) )/(sigma_cc*( fNEvents -2 )));

      //
      // Load Yields in to make Yield Correction a little easier in the end.
      //
      if(fSensHumanReadable == 1){
	 DetectorSlope[fNModType][det].push_back(1e6*slope/( TMath::Abs(d_mean) ));
	 DetectorSlopeError[fNModType][det].push_back(1e6*sigma_slope/( TMath::Abs(d_mean) ));
      }else{
	 DetectorSlope[fNModType][det].push_back(slope/( TMath::Abs(d_mean) ));
	 DetectorSlopeError[fNModType][det].push_back(sigma_slope/( TMath::Abs(d_mean) ));
      }
      
      c_mean = 0;
      d_mean = 0;
      slope = 0;
      sigma_slope = 0;
      sigma_cc = 0;
      sigma_dc = 0;
      sigma_dd = 0;
    }

    for(Int_t mon = 0; mon < fNMonitor; mon++){
      if(MonitorData[mon].size() <= 0){
	std::cout << "!!!!!!!!!!!!!!!!! Illegal Monitor vector length:\t" << MonitorData[mon].size() << std::endl;
	return;
      }
      for(Int_t evNum = 0; evNum < fNEvents; evNum++) c_mean += TMath::Sin( (TMath::Pi()/180)*0.091548*CoilData[fNModType][evNum] + phase[fNModType] );
      c_mean /=fNEvents;
      
      for(Int_t evNum = 0; evNum < fNEvents; evNum++) d_mean += MonitorData[mon][evNum];
      d_mean /=fNEvents;
  
      for(Int_t evNum = 0; evNum < fNEvents; evNum++){
	sigma_cc += (TMath::Sin( (TMath::Pi()/180)*0.091548*CoilData[fNModType][evNum] + phase[fNModType] ) - c_mean)*(TMath::Sin( (TMath::Pi()/180)*0.091548*CoilData[fNModType][evNum] +phase[fNModType] ) - c_mean);
	sigma_dc += (MonitorData[mon][evNum] - d_mean)*(TMath::Sin( (TMath::Pi()/180)*0.091548*CoilData[fNModType][evNum] + phase[fNModType] ) - c_mean);
	sigma_dd += (MonitorData[mon][evNum] - d_mean)*(MonitorData[mon][evNum] - d_mean);
	// Clear instances after computation
	MonitorData[mon].clear();
      }
      
      slope = sigma_dc/sigma_cc;
      sigma_slope = TMath::Sqrt((sigma_dd - (sigma_dc*sigma_dc)/sigma_cc)/(sigma_cc*(fNEvents -2 )));
      
      MonitorSlope[fNModType][mon].push_back(slope);
      MonitorSlopeError[fNModType][mon].push_back(sigma_slope);

      c_mean = 0;
      d_mean = 0;
      slope = 0;
      sigma_slope = 0;
      sigma_cc = 0;
      sigma_dc = 0;
      sigma_dd = 0;  
      
    }
    // Same as above.
    CoilData[fNModType].clear();

    // These need to be set so we know if we have a full set of modulation data

    if(fNModType == fXModulation)  fXinit = true;
    if(fNModType == fYModulation)  fYinit = true;
    if(fNModType == fEModulation)  fEinit = true;
    if(fNModType == fXPModulation) fXPinit = true;
    if(fNModType == fYPModulation) fYPinit = true;

    return;
}

void QwModulation::SetFileName(TString & filename)
{
  fFileName = filename;
  return;
}

Int_t QwModulation::ConvertPatternNumber(Int_t global)
{
  Int_t key[16] = {0, 1, 2, 3, 4, 0, 0, 0, 0, 
		   0, 0, 0, 1, 2, 3, 4};
  if(global < 0) return(-1);
  if(global < 11) fSingleCoil = true;

  return(key[global]);
}

void QwModulation::PilferData()
{

  Int_t fEvCounter = 0;
  Int_t error[5] = {0};
  Int_t pattern = -1;

  if (fChain == 0) return;
  Long64_t nentries = fChain->GetEntries();

  std::cout << "Number of entries: " << nentries << std::endl;

  for(Long64_t i = 0; i < nentries; i++){

    LoadTree(i);
    if(i < 0) break;
    fChain->GetEntry(i);

    if((i % 100000) == 0) std::cout << other << "processing: " << i << normal << std::endl;
    pattern = ConvertPatternNumber((Int_t)bm_pattern_number);

    if(fReduceMatrix_x != 1){
      if(pattern == 0 && ramp_hw_sum > fPedestal && i < nentries){
	std::cout << "X Modulation found" << std::endl;
	do{
	  fChain->GetEntry(i);
	  if( (ErrorCodeCheck("mps_tree") != 0) ){
	    ++i;
	    error[0]++;
	    continue;
	  }
	  if(fNEvents > 3924) break;

	  for(Int_t j = 0; j < fNDetector; j++){
	    DetectorData[j].push_back(DetBranch[j][0]);
	  }
	  for(Int_t j = 0; j < fNMonitor; j++){
	    MonitorData[j].push_back(MonBranch[j+1][0]);
	  }
	  
	  CoilData[fXModulation].push_back(ramp_hw_sum);
	  ++fEvCounter;
	  ++fNEvents;
	  ++i;
	}while(pattern == 0 && ramp_hw_sum > fPedestal && i < nentries);

	CalculateSlope(fXModulation);
	fNEvents = 0;
      }
    }

    if(fReduceMatrix_y != 1){      
      if(pattern == 1 && ramp_hw_sum > fPedestal && i < nentries){
	std::cout << "Y Modulation found" << std::endl;
	do{
	  fChain->GetEntry(i);
	  
	  if( (ErrorCodeCheck("mps_tree") != 0) ){
	    ++i;
	    error[1]++;
	    continue;
	  }
	  if(fNEvents > 3924) break;
	  for(Int_t j = 0; j < fNDetector; j++){
	    DetectorData[j].push_back(DetBranch[j][0]);
	  }
	  for(Int_t j = 0; j < fNMonitor; j++){
	    MonitorData[j].push_back(MonBranch[j+1][0]);
	  }
	  CoilData[fYModulation].push_back(ramp_hw_sum);
	  ++fEvCounter;
	  ++fNEvents;
	  ++i;
	}while(pattern == 1 && ramp_hw_sum > fPedestal && i < nentries);
	CalculateSlope(fYModulation);
	fNEvents = 0;
      }
    }
    if(fReduceMatrix_e != 1){    
      if(pattern == 2 && ramp_hw_sum > fPedestal && i < nentries){
	std::cout << "E Modulation found" << std::endl;
	do{
	  fChain->GetEntry(i);
	  
	  if( (ErrorCodeCheck("mps_tree") != 0) ){
	    ++i;
	    error[2]++;
	    continue;
	  }
	  if(fNEvents > 3924) break;
	  for(Int_t j = 0; j < fNDetector; j++){
	    DetectorData[j].push_back(DetBranch[j][0]);
	  }
	  for(Int_t j = 0; j < fNMonitor; j++){
	    MonitorData[j].push_back(MonBranch[j+1][0]);
	  }
	  
	  CoilData[fEModulation].push_back(ramp_hw_sum);
	  ++fEvCounter;
	  ++fNEvents;
	  ++i;
	}while(pattern == 2 && ramp_hw_sum > fPedestal && i < nentries);
	CalculateSlope(fEModulation);
	fNEvents = 0;
      }
    }

    if(fReduceMatrix_xp != 1){
      if(pattern == 3 && ramp_hw_sum > fPedestal && i < nentries){
	std::cout << "XP Modulation found" << std::endl;
	do{
	  fChain->GetEntry(i);
	  
	  if( (ErrorCodeCheck("mps_tree") != 0) ){
	    ++i;
	    error[3]++;
	    continue;
	  }
	  if(fNEvents > 3924) break;
	  for(Int_t j = 0; j < fNDetector; j++){
	    DetectorData[j].push_back(DetBranch[j][0]);
	  }
	  for(Int_t j = 0; j < fNMonitor; j++){
	    MonitorData[j].push_back(MonBranch[j+1][0]);
	  }
	  
	  CoilData[fXPModulation].push_back(ramp_hw_sum);
	  ++fEvCounter;
	  ++fNEvents;
	  ++i;
	}while(pattern == 3 && ramp_hw_sum > fPedestal && i < nentries);
	CalculateSlope(fXPModulation);
	fNEvents = 0;
      }
    }

      if(fReduceMatrix_yp != 1){      
	if(pattern == 4 && ramp_hw_sum > fPedestal && i < nentries){
	  std::cout << "YP Modulation found" << std::endl;
	  do{
	  fChain->GetEntry(i);

	  if( (ErrorCodeCheck("mps_tree") != 0) ){
	    ++i;
	    error[4]++;
	    continue;
	  }
	  if(fNEvents > 3924) break;
	  for(Int_t j = 0; j < fNDetector; j++){
	    DetectorData[j].push_back(DetBranch[j][0]);
	  }
	  for(Int_t j = 0; j < fNMonitor; j++){
	    MonitorData[j].push_back(MonBranch[j+1][0]);
	  }
	  
	  CoilData[fYPModulation].push_back(ramp_hw_sum);
	  ++fEvCounter;
	  ++fNEvents;
	  ++i;
	}while(pattern == 4 && ramp_hw_sum > fPedestal && i < nentries);
	  CalculateSlope(fYPModulation);
	  fNEvents = 0;
	}
      }
  }
  for(Int_t i = 0; i < 5; i++)
    std::cout << "Error:\t" << error[i] << std::endl;

  std::cout << "::Done with Pilfer::" << std::endl;


  return;
}

void QwModulation::Clean()
{
  //
  // This function serves the purpose of deallocating 
  // memory for unused vectors in the QwModulation Class.
  // Should be run after finishing with the slope calculation.
  //

    DetectorData.clear();
    MonitorData.clear();
    AvDetectorSlope.clear();
    AvDetectorSlopeError.clear();
    AvMonitorSlope.clear();
    AvMonitorSlopeError.clear();
    CoilData.clear();
    MonitorSlope.clear();
    MonitorSlopeError.clear();
    DetectorSlope.clear();
    DetectorSlopeError.clear();

    return;
}

void QwModulation::BuildDetectorData()
{
  for(Int_t i = 0; i < fNDetector; i++)
    DetectorData.push_back(std::vector <Double_t>());
  std::cout << "Detector block size: " << DetectorData.size() << std::endl;
  return;
}

void QwModulation::BuildDetectorAvSlope()
{
  for(Int_t i = 0; i < fNModType; i++){
    AvDetectorSlope.push_back(std::vector <Double_t>());
    AvDetectorSlopeError.push_back(std::vector <Double_t>());
  }
  std::cout << "Average Detector vector size: " << AvDetectorSlope.size() << std::endl;
  return;
}

void QwModulation::BuildCoilData()
 {
   for(Int_t i = 0; i < fNModType; i++)
     CoilData.push_back(std::vector <Double_t>());
   std::cout << "Coil block size: " << CoilData.size() << std::endl;
   
   return;
 }

void QwModulation::BuildMonitorData()
{
  for(Int_t i = 0; i < fNMonitor; i++)
    MonitorData.push_back(std::vector <Double_t>());

  std::cout << "Monitor block size: " << MonitorData.size() << std::endl;

  return;
}

void QwModulation::BuildMonitorAvSlope()
{
  for(Int_t i = 0; i < fNModType; i++){
    AvMonitorSlope.push_back(std::vector <Double_t>());
    AvMonitorSlopeError.push_back(std::vector <Double_t>());
  }
  std::cout << "Average Monitor vector size: " << AvMonitorSlope.size() << std::endl;
  return;
}

void QwModulation::BuildDetectorSlopeVector()
{
  DetectorSlope.resize(5);
  DetectorSlopeError.resize(5);
  for(Int_t i = 0; i < 5; i++){
    DetectorSlope[i].resize(fNDetector);
    DetectorSlopeError[i].resize(fNDetector);
  }
  return;
}

void QwModulation::BuildMonitorSlopeVector()
{
  MonitorSlope.resize(5);
  MonitorSlopeError.resize(5);
  for(Int_t i = 0; i < 5; i++){
    MonitorSlope[i].resize(fNMonitor);
    MonitorSlopeError[i].resize(fNMonitor);
  }
  return;
}

void QwModulation::SetPhaseValues(Double_t *val)
{

  phase.resize(5);
//   std::cout << other << "Default phase information:\t" << normal << std::endl;
  for(Int_t i = 0; i < fNModType; i++){
    phase[i] = val[i];
//     std::cout << other << phase[i] << normal << std::endl;
  }

  return;
}

Int_t QwModulation::ReadPhaseConfig(Char_t *file)
{

  std::string line;
  
  char *token;

  std::fstream fphase;


  fphase.open(file, std::ios_base::in);
  if(!fphase.is_open()){
    std::cout << red << "Error opening phase config file.  Using default values." 
	      << normal << std::endl;
    fPhaseConfig = false;
    return 1;
  }

  phase.resize(fNModType);
  while(fphase.good()){
    getline(fphase, line);
    token = new char[line.size() + 1];
    strcpy(token, line.c_str());
    token = strtok(token, " ,.");
    while(token){
      if(strcmp("x", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " ,"); 
       	std::cout << other << "\t\tX phase is: " << token << normal << std::endl;
	phase[fXModulation] = atof(token); 
       }
      if(strcmp("xp", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " ,"); 
       	std::cout << other << "\t\tXP phase is: " << token << normal << std::endl;
	phase[fXPModulation] = atof(token); 
       }
      if(strcmp("e", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " ,"); 
       	std::cout << other << "\t\tE phase is: " << token << normal << std::endl;
	phase[fEModulation] = atof(token); 
       }
      if(strcmp("y", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " ,"); 
       	std::cout << other << "\t\tY phase is: " << token << normal << std::endl;
	phase[fYModulation] = atof(token); 
       }
      if(strcmp("yp", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " ,"); 
       	std::cout << other << "\t\tYP phase is: " << token << normal << std::endl;
	phase[fYPModulation] = atof(token); 
       }
      else 
       	token = strtok(NULL, " ,"); 
    }
    fPhaseConfig = true;
  }

  return 0;
}

Int_t QwModulation::ReadConfig(QwModulation *meteor)
{
  std::string line;

  char *token;

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
       	std::cout << other << "\t\tMonitor is: " << token << normal << std::endl;
	meteor->MonitorList.push_back(token); 
       }
      if(strcmp("det", token) == 0){
       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " .,"); 
       	std::cout << other << "\t\tDetector is: " << token << normal << std::endl; 
	meteor->DetectorList.push_back(token);
      }
      else 
       	token = strtok(NULL, " .,"); 
    }
  }
  fNDetector = DetectorList.size();
  fNMonitor = MonitorList.size();

  if( (fNDetector > fNMaxDet) || (fNMonitor > fNMaxMon) )
    {
      std::cout << red << "Error :: Exceeded maximum number of detectors(monitors)" 
		<< red << "Detectors:\t" << fNDetector << "\tMax:\t" << fNMaxDet
		<< red << "Monitors:\t" << fNMonitor   << "\tMax:\t" << fNMaxMon
		<< normal << std::endl;
      exit(1);
    }

  config.close();

  return 0;
}

void QwModulation::Scan(QwModulation *meteor)
{
                               
   for(Int_t i = 0; i < (Int_t)fNDetector; i++){
     fChain->SetBranchStatus(DetectorList[i], 1);
     fChain->SetBranchAddress(DetectorList[i], &DetBranch[i]);
   }
   for(Int_t i = 0; i < (Int_t)fNMonitor; i++){
     fChain->SetBranchStatus(MonitorList[i], 1);
     fChain->SetBranchAddress(MonitorList[i], &MonBranch[i+1]);
   }
}

Bool_t QwModulation::FileSearch(TString filename, TChain *chain)
{

  TString file_directory;
  Bool_t c_status = kFALSE;

  file_directory = gSystem->Getenv("QW_ROOTFILES");

  if(fFileSegmentInclude){
    for(Int_t i = fLowerSegment; i <= fUpperSegment; i++){
      filename = Form("%s_%d.*%d.trees.root", fFileStem.Data(), run_number, i);
      std::cout << other << "Adding:: " 
		<< filename << normal << std::endl;
      if(!(chain->Add(Form("%s/%s",file_directory.Data(), filename.Data()))) ){
	std::cout << red << "Error chaining segment:\t" << filename << normal << std::endl;
        exit(1);
      }
    }
  }
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

void QwModulation::LoadRootFile(TString filename, TChain *tree)
{
  Bool_t found = FileSearch(filename, tree);
  
  if(!found){
    filename = Form("Qweak_%d.*.trees.root", run_number);
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

void QwModulation::Write(){
  //*********************************************
  //
  //  Slopes.dat is really a deprecated file 
  //  at this point.  I will remove it soon.
  //
  //********************************************

  gSystem->Exec("umask 002");

  slopes.open(Form("%s/slopes/slopes%s_%i.dat", output.Data(), fFileSegment.Data(), run_number) , fstream::out);
  regression = fopen(Form("%s/regression/regression%s_%i.dat", output.Data(), fFileSegment.Data(), run_number), "w");

  if( (slopes.is_open() && slopes.good()) ){
    for(Int_t i = 0; i < fNDetector; i++){
      slopes << "det " << DetectorList[i] << std::endl;
      for(Int_t j = 0; j < fNModType; j++){
	slopes << YieldSlope[i][j] << "\t"
	       << YieldSlopeError[i][j] << std::endl;
      }
    }
  }

  else{
    std::cout << red << "Error opening slopes.dat" << normal << std::endl;
    std::cout << other << "Cleaning up output files....." << normal << std::endl;
    CleanFolders();
    exit(1);
  }

  if( (diagnostic.is_open() && diagnostic.good()) ){
    diagnostic << "\n#Failed events in x,xp,e,y,yp\n" << std::endl;
    diagnostic << fXNevents  << std::endl;
    diagnostic << fXPNevents << std::endl;
    diagnostic << fENevents  << std::endl;
    diagnostic << fYNevents  << std::endl;
    diagnostic << fYPNevents << std::endl;
    diagnostic << (fXNevents + fXPNevents + fENevents + fYNevents + fYPNevents) << std::endl;
  }


  //***************************************************************
  // Write output file for regression - This will go into the DB.  
  // All that is included here is the slopes(and errors soon...),
  // qwlibra must be run to get other run info.
  //***************************************************************

  fprintf(regression, "#run=%i\tevents=%i\n",run_number, fNumberEvents);
  if( regression != NULL ){
    for(Int_t i = 0; i < fNDetector; i++){
      for(Int_t j = 0; j < fNModType; j++){
	fprintf(regression, "%s/%s : %i : %e : %e\n", DetectorList[i].Data(), MonitorList[j].Data(),
		j, YieldSlope[i][j], YieldSlopeError[i][j]);
      }
    }
  }

  else{
    std::cout << red << "Error opening regression.dat" << normal << std::endl;
    std::cout << other << "Cleaning up output files....." << normal << std::endl;
    gSystem->Exec(Form("rm -rf regression_%i", run_number));
    exit(1);
  }

  fclose(regression);
  slopes.close();
  diagnostic.close();
  
  return;
}

void QwModulation::PrintError(TString error){

  std::cout << red << error << normal << std::endl;

  return;
}

void QwModulation::CleanFolders()
{
  gSystem->Exec(Form("rm -rf regression_%i", run_number));
  gSystem->Exec(Form("rm -rf slopes_%i", run_number));

  return;
}

// void QwModulation::SetFlags()
// {

//   fXinit  = false;
//   fYinit  = false;
//   fEinit  = false;
//   fXPinit = false;
//   fYPinit = false;
//   fSingleCoil = false;

//   return;
// }

void QwModulation::CheckFlags()
{

  if( !(fXinit && fYinit && fEinit && fXPinit && fYPinit) ){
    PrintError("Not enough modulation cycles in this run -- exiting");
    CleanFolders();
    exit(1);
  }
  return;

}

Bool_t QwModulation::IfExists(const char *file)
{
  if(FILE *file_to_check = fopen(file, "r")){
    fclose(file_to_check);
    return true;
  }
  return false;
}
#endif

//  LocalWords:  SetupMpsBranchAddress
