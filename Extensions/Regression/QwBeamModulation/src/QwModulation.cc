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
  fNModType(5), fXinit(false), fYinit(false), fEinit(false), 
  fXPinit(false), fYPinit(false), fSingleCoil(false) 
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
   fChain->SetBranchStatus("fgy1", 1);
   fChain->SetBranchStatus("fgy2", 1);

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

  fChain->SetBranchAddress("yield_qwk_charge",&yield_qwk_charge,&b_yield_qwk_charge);
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
  Int_t code = 0;

  if( type.CompareTo("mps_tree", TString::kIgnoreCase) == 0 ){
    subblock = ((ramp_block3+ramp_block0)-(ramp_block2+ramp_block1));
    
    for(Int_t i = 0; i < fNMonitor; i++){
      if( (Int_t)MonBranch[i + 1][fDeviceErrorCode] != 0 ){
	code = 1;
      }
    }
    for(Int_t i = 0; i < fNDetector; i++){
      if( (Int_t)DetBranch[i][fDeviceErrorCode] != 0){
	code = 1;
      }
    }
    if( !((subblock > -50) && (subblock < 50)) )
      code = 1;
    if( (ramp_hw_sum > 0) && ((UInt_t)ErrorFlag != 0x4018080)  ){
#ifdef __VERBOSE_ERRORS
      std::cout << red << "Mps Tree::Modulation ErrorFlag" << normal << std::endl;
#endif
      code = 1;
    }

    if( (ramp_hw_sum < 0) && ((UInt_t)ErrorFlag != 0) ){
#ifdef __VERBOSE_ERRORS
      std::cout << red << "Mps Tree::Natural Motion ErrorFlag" << normal << std::endl;
#endif
      code = 1;
    }

  }
  if( type.CompareTo("hel_tree", TString::kIgnoreCase) == 0 ){
    subblock = ((yield_ramp_block3+yield_ramp_block0)-(yield_ramp_block2+yield_ramp_block1));
    
    for(Int_t i = 0; i < fNMonitor; i++){
      if( (Int_t)HMonBranch[i][fDeviceErrorCode] != 0 ){
	code = 1;
      }
    }
    for(Int_t i = 0; i < fNDetector; i++){
      if( (Int_t)HDetBranch[i][fDeviceErrorCode] != 0 ){
	code = 1;
      }
    }
    if( !((subblock > -50) && (subblock < 50)) )
      code = 1;
    if(yield_qwk_mdallbars_Device_Error_Code != 0){
      code = 1;
    }
  }
  
  return( code );
}

void QwModulation::MatrixFill()
{

  TMatrixD AMatrix(fNDetector, fNModType);
  TMatrixD AMatrixE(fNDetector, fNModType);

  Double_t determinant;

  CheckFlags();

  for(Int_t j = 0; j < fNDetector; j++){
    for(Int_t k = 0; k < fNModType; k++){
      AMatrix(j,k) = AvDetectorSlope[k][j];
      AMatrixE(j,k) = AvDetectorSlopeError[k][j];
    }
  }
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
    }
  }
  std::cout << "\t\t\t\t::R Matrix:: " << std::endl;
  RMatrix.Print("%11.10f");
  
  std::cout << "\t\t\t\t::R Matrix Error::"<< std::endl;
  RMatrixE.Print("%11.10f");
  
  TMatrixD RMatrixInv = RMatrix;
  RMatrixInv.Invert(&determinant);

  RMatrixInv.Print("%11.10f");
  std::cout << determinant << std::endl;
  TMatrixD Identity(fNMonitor, fNMonitor);
  Identity.Mult(RMatrixInv, RMatrix);
  Identity.Print();

  TMatrixD SMatrix(fNDetector, fNModType);
  SMatrix.Mult(AMatrix, RMatrixInv);

  std::cout << "\n\n\t\t\t\t::SMatrix::\n" << std::endl;
  SMatrix.Print();

  for(Int_t i = 0; i < fNDetector; i++){
    for(Int_t j = 0; j < fNModType; j++){
      YieldSlope[i][j] = SMatrix(i,j);
    }
  }
//   exit(1);
  Write();

  if(fSensHumanReadable == 1){
    std::cout << "Exiting after sensitivities." << std::endl;
    exit(1);
  }
}

void QwModulation::ComputePositionMean()
{
}

void QwModulation::ComputeAsymmetryCorrections()
{
  //**************************************************************
  //
  // Time to calculate some Corrections 
  //
  //**************************************************************

  //
  // This is used only when pulling in multiple runs
  //
  // TFile file(Form("$QWSCRATCH/bmod_tree_%i-%i.root", run.front(), run.back()),"update");

  TFile file(Form("$QW_ROOTFILES/bmod_tree_%i.root", run.front()),"RECREATE");

  TTree *mod_tree = new TTree("Mod_Tree", "Modulation Analysis Results Tree");

  Int_t fEvCounter = 0;

  Double_t temp_correction = 0;
  Double_t monitor_correction[fNMaxDet][fNMaxMon];

  correction.resize(fNDetector);
  mod_tree->Branch("mps_counter", &mps_counter, "mps_counter/D"); 
  mod_tree->Branch("yield_qwk_charge", &yield_qwk_charge, "yield_qwk_charge/D"); 
  mod_tree->Branch("yield_bm_pattern_number", &yield_bm_pattern_number, "yield_bm_pattern_number/D"); 
  mod_tree->Branch("yield_ramp", &yield_ramp_hw_sum, "yield_ramp_hw_sum/D"); 
  mod_tree->Branch("ErrorFlag", &ErrorFlag, "ErrorFlag/D"); 
  mod_tree->Branch("yield_qwk_mdallbars_Device_Error_Code", &yield_qwk_mdallbars_Device_Error_Code, "yield_qwk_mdallbars_Device_Error_Code/D"); 
  mod_tree->Branch("yield_qwk_mdallbars", &yield_qwk_mdallbars_hw_sum, "yield_qwk_mdallbars/D"); 
  mod_tree->Branch("asym_qwk_charge", &asym_qwk_charge_hw_sum, "asym_qwk_charge/D"); 

  for(Int_t i = 0; i < fNDetector; i++){
    mod_tree->Branch(HDetectorList[i], &HDetBranch[i][0], Form("%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("corr_%s", HDetectorList[i].Data()), &AsymmetryCorrection[i], Form("corr_%s/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("%s_Device_Error_Code", HDetectorList[i].Data()), &HDetBranch[i][fDeviceErrorCode], 
		     Form("%s_Device_Error_Code/D", HDetectorList[i].Data())); 
    mod_tree->Branch(Form("correction_%s", DetectorList[i].Data()), &correction[i], Form("correction_%s/D", DetectorList[i].Data())); 
    std::cout << HDetectorList[i] << std::endl;
  }
  for(Int_t j = 0; j < fNMonitor; j++){
    mod_tree->Branch(HMonitorList[j], &HMonBranch[j][0], Form("%s/D", HMonitorList[j].Data())); 
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
    
    if( (ErrorCodeCheck("hel_tree") == 0 && yield_qwk_mdallbars_hw_sum != 0) ){
      for(Int_t j = 0; j < fNDetector; j++){
	for(Int_t k = 0; k < fNMonitor; k++){
	  temp_correction += YieldSlope[j][k]*HMonBranch[k][0];
	  monitor_correction[j][k] = YieldSlope[j][k]*HMonBranch[k][0];
	  if( (i % 100000) == 0 ){}
	}

   	correction[j] = temp_correction;                                  
//   	correction = temp_correction + 0.0167297*asym_qwk_charge_hw_sum;

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

void QwModulation::CalculateSlope(Int_t fModType)
{

  Double_t c_mean = 0;
  Double_t d_mean = 0;
  Double_t sigma_cc = 0;
  Double_t sigma_dc = 0;
  Double_t sigma_dd = 0;
  Double_t sigma_slope = 0;
  Double_t slope = 0;
  Double_t fPhase[5]={0.26, 0.26, 0.0, 1.08, 1.08};

  if(fNEvents < 2){
    std::cout << red << "Error in run:: Number of good events too small, exiting." << normal << std::endl;
    return;
  }
  
  if(CoilData[fModType].size() <= 0){
    std::cout << "!!!!!!!!!!!!!!!!! Illegal Coil vector length:\t" << CoilData[fModType].size() << std::endl;
    return;
  }
    
    for(Int_t det = 0; det < fNDetector; det++){
    
      if(DetectorData[det].size() <= 0){
	std::cout << "!!!!!!!!!!!!!!!!! Illegal Detector vector length:\t" << DetectorData[det].size() << std::endl;
	return;
      }
    
      for(Int_t evNum = 0; evNum < fNEvents; evNum++) c_mean += TMath::Sin( (2*TMath::Pi()*((CoilData[fModType][evNum]-fRampPedestal))/(3754-fRampPedestal)) + fPhase[fModType]);
      c_mean /=fNEvents;
      
      for(Int_t evNum = 0; evNum < fNEvents; evNum++) d_mean += DetectorData[det][evNum];
      d_mean /=fNEvents;

      for(Int_t evNum = 0; evNum < fNEvents; evNum++){
	sigma_cc += (TMath::Sin( (2*TMath::Pi()*((CoilData[fModType][evNum]-fRampPedestal))/(3754-fRampPedestal)) + fPhase[fModType] ) - c_mean)*(TMath::Sin( (2*TMath::Pi()*((CoilData[fModType][evNum]-fRampPedestal))/(3754-fRampPedestal)) + fPhase[fModType] ) - c_mean);
	sigma_dc += (DetectorData[det][evNum] - d_mean)*(TMath::Sin( (2*TMath::Pi()*((CoilData[fModType][evNum]-fRampPedestal))/(3754-fRampPedestal)) + fPhase[fModType] ) - c_mean);
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
	 DetectorSlope[fModType][det].push_back(1e6*slope/( TMath::Abs(d_mean) ));
	 DetectorSlopeError[fModType][det].push_back(1e6*sigma_slope/( TMath::Abs(d_mean) ));
      }else{
	 DetectorSlope[fModType][det].push_back(slope/( TMath::Abs(d_mean) ));
	 DetectorSlopeError[fModType][det].push_back(sigma_slope/( TMath::Abs(d_mean) ));
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
      for(Int_t evNum = 0; evNum < fNEvents; evNum++) c_mean += TMath::Sin( (2*TMath::Pi()*((CoilData[fModType][evNum]-fRampPedestal))/(3754-fRampPedestal)) + fPhase[fModType] );
      c_mean /=fNEvents;
      
      for(Int_t evNum = 0; evNum < fNEvents; evNum++) d_mean += MonitorData[mon][evNum];
      d_mean /=fNEvents;
  
      for(Int_t evNum = 0; evNum < fNEvents; evNum++){
	sigma_cc += (TMath::Sin( ((2*TMath::Pi()*(CoilData[fModType][evNum]-fRampPedestal))/(3754-fRampPedestal)) + fPhase[fModType] ) - c_mean)*(TMath::Sin( ((2*TMath::Pi()*(CoilData[fModType][evNum]-fRampPedestal))/(3754-fRampPedestal)) +fPhase[fModType] ) - c_mean);
	sigma_dc += (MonitorData[mon][evNum] - d_mean)*(TMath::Sin( ((2*TMath::Pi()*(CoilData[fModType][evNum]-fRampPedestal))/(3754-fRampPedestal)) + fPhase[fModType] ) - c_mean);
	sigma_dd += (MonitorData[mon][evNum] - d_mean)*(MonitorData[mon][evNum] - d_mean);
	// Clear instances after computation
	MonitorData[mon].clear();
      }
      
      slope = sigma_dc/sigma_cc;
      sigma_slope = TMath::Sqrt((sigma_dd - (sigma_dc*sigma_dc)/sigma_cc)/(sigma_cc*(fNEvents -2 )));
      
      MonitorSlope[fModType][mon].push_back(slope);
      MonitorSlopeError[fModType][mon].push_back(sigma_slope);

      c_mean = 0;
      d_mean = 0;
      slope = 0;
      sigma_slope = 0;
      sigma_cc = 0;
      sigma_dc = 0;
      sigma_dd = 0;  
      
    }
    // Same as above.
    CoilData[fModType].clear();

    // These need to be set so we know if we have a full set of modulation data

    if(fModType == fXModulation)  fXinit = true;
    if(fModType == fYModulation)  fYinit = true;
    if(fModType == fEModulation)  fEinit = true;
    if(fModType == fXPModulation) fXPinit = true;
    if(fModType == fYPModulation) fYPinit = true;

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
      if(pattern == 0 && ramp_hw_sum > 0 && i < nentries){
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
	}while(pattern == 0 && ramp_hw_sum > 0 && i < nentries);

	CalculateSlope(fXModulation);
	fNEvents = 0;
      }
    }

    if(fReduceMatrix_y != 1){      
      if(pattern == 1 && ramp_hw_sum > 0 && i < nentries){
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
	}while(pattern == 1 && ramp_hw_sum > 0 && i < nentries);
	CalculateSlope(fYModulation);
	fNEvents = 0;
      }
    }
    if(fReduceMatrix_e != 1){    
      if(pattern == 2 && ramp_hw_sum > 0 && i < nentries){
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
	}while(pattern == 2 && ramp_hw_sum > 0 && i < nentries);
	CalculateSlope(fEModulation);
	fNEvents = 0;
      }
    }

    if(fReduceMatrix_xp != 1){
      if(pattern == 3 && ramp_hw_sum > 0 && i < nentries){
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
	}while(pattern == 3 && ramp_hw_sum > 0 && i < nentries);
	CalculateSlope(fXPModulation);
	fNEvents = 0;
      }
    }

      if(fReduceMatrix_yp != 1){      
	if(pattern == 4 && ramp_hw_sum > 0 && i < nentries){
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
	}while(pattern == 4 && ramp_hw_sum > 0 && i < nentries);
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

Int_t QwModulation::ReadConfig(QwModulation *meteor)
{
  std::string line;

  char *token;

  config.open("config/setup.config");
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
       	std::cout << red << "\t\tMonitor is: " << token << normal << std::endl;
	meteor->MonitorList.push_back(token); 
       }
      if(strcmp("det", token) == 0){

       	// Here the extra strtok(NULL, " .,") keeps scanning for next token

       	token = strtok(NULL, " .,"); 
       	std::cout << red << "\t\tDetector is: " << token << normal << std::endl; 
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
		<< other << "Detectors:\t" << fNDetector << "\tMax:\t" << fNMaxDet
		<< other << "Monitors:\t" << fNMonitor   << "\tMax:\t" << fNMaxMon
		<< normal << std::endl;
      exit(1);
    }

  config.close();

  return 0;
}

void QwModulation::LoadRunList()
{
  Int_t temp;

  input.open("../config/run_list.txt", fstream::in);
  if(!input.is_open()){
    std::cout << "Could not open config/run_list.txt -- exiting" << std::endl;
    exit(1);
  }

  while(input.good()){
    input >> temp;
    run.push_back(temp);
  }
  input.close();

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
  gSystem->Exec(Form("rm -rf slopes_%i", run.front()));
  gSystem->Exec(Form("mkdir slopes_%i", run.front()));
  gSystem->Exec(Form("rm -rf regression_%i", run.front()));
  gSystem->Exec(Form("mkdir regression_%i", run.front()));

  slopes.open(Form("slopes_%i/slopes.dat", run.front()) , fstream::out);
  regression = fopen(Form("regression_%i/regression.dat", run.front()), "w");

  if( (slopes.is_open() && slopes.good()) ){
    for(Int_t i = 0; i < fNDetector; i++){
      slopes << "det " << DetectorList[i] << std::endl;
      for(Int_t j = 0; j < fNModType; j++){
	slopes << YieldSlope[i][j] << std::endl;
      }
    }
  }

  else{
    std::cout << red << "Error opening slopes.dat" << normal << std::endl;
    std::cout << other << "Cleaning up output files....." << normal << std::endl;
    CleanFolders();
    exit(1);
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
	fprintf(regression, "%s/%s : %i : %e\n", DetectorList[i].Data(), MonitorList[j].Data(),
		j, YieldSlope[i][j]);
      }
    }
  }

  else{
    std::cout << red << "Error opening regression.dat" << normal << std::endl;
    std::cout << other << "Cleaning up output files....." << normal << std::endl;
    gSystem->Exec(Form("rm -rf regression_%i", run.front()));
    exit(1);
  }

  fclose(regression);
  slopes.close();
  
  return;
}

void QwModulation::PrintError(TString error){

  std::cout << red << error << normal << std::endl;

  return;
}

void QwModulation::CleanFolders()
{
  gSystem->Exec(Form("rm -rf regression_%i", run.front()));
  gSystem->Exec(Form("rm -rf slopes_%i", run.front()));

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
#endif

//  LocalWords:  SetupMpsBranchAddress
