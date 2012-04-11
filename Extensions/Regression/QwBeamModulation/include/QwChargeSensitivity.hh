#ifndef QW_CHARGE_SENSITIVITY_hh
#define QW_CHARGE_SENSITIVITY_hh

#include <fstream>
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class QwChargeSensitivity {

private:

  static const Int_t fNMaxDet = 40; 
  static const Int_t fDeviceErrorCode = 6;
  static const Int_t fBranchSize = 13;

public :
  TChain          *fChain;

  Int_t           fCurrent; 
  Int_t           fNDetector;  
  //
  Double_t        ErrorFlag;
  Double_t        yield_bm_pattern_number;
  Double_t        yield_qwk_charge;
  Double_t        mps_counter; 
  Double_t        yield_ramp_hw_sum;
  Double_t        yield_ramp_block0;
  Double_t        yield_ramp_block1;
  Double_t        yield_ramp_block2;
  Double_t        yield_ramp_block3;
  Double_t        yield_ramp_num_samples;
  Double_t        yield_ramp_Device_Error_Code;
  Double_t        yield_ramp_hw_sum_raw;
  Double_t        yield_ramp_block0_raw;
  Double_t        yield_ramp_block1_raw;
  Double_t        yield_ramp_block2_raw;
  Double_t        yield_ramp_block3_raw;
  Double_t        yield_ramp_sequence_number;
  Double_t        yield_qwk_mdallbars_hw_sum;
  Double_t        yield_qwk_mdallbars_block0;
  Double_t        yield_qwk_mdallbars_block1;
  Double_t        yield_qwk_mdallbars_block2;
  Double_t        yield_qwk_mdallbars_block3;
  Double_t        yield_qwk_mdallbars_num_samples;
  Double_t        yield_qwk_mdallbars_Device_Error_Code;
  Double_t        asym_qwk_charge_hw_sum;
  Double_t        asym_qwk_charge_block0;
  Double_t        asym_qwk_charge_block1;
  Double_t        asym_qwk_charge_block2;
  Double_t        asym_qwk_charge_block3;
  Double_t        asym_qwk_charge_num_samples;
  Double_t        asym_qwk_charge_Device_Error_Code;

  TBranch        *b_ErrorFlag;
  TBranch        *b_yield_bm_pattern_number;
  TBranch        *b_yield_qwk_charge;
  TBranch        *b_mps_counter;
  TBranch        *b_yield_ramp; 
  TBranch        *b_yield_qwk_mdallbars; 
  TBranch        *b_asym_qwk_charge; 
  //

  Bool_t fInit;

  std::fstream config;

  FILE* charge_sens;

  Int_t  run_number;
  Int_t  fNEvents;
  Int_t  fNCycles;

  Double_t        DetBranch[fNMaxDet][fBranchSize];  
  Double_t        DetectorMean[fNMaxDet];

  static const char red[8];
  static const char other[8];
  static const char normal[8];

  std::vector <TString> DetectorList;

  std::vector <Double_t> ChargeAsymData;
  std::vector <Double_t> AverageDetectorSlope;
  std::vector <Double_t> AverageDetectorSlopeError;

  std::vector < std::vector <Double_t> > DetectorData;
  std::vector < std::vector <Double_t> > DetectorSlope;
  std::vector < std::vector <Double_t> > DetectorSlopeError;

   QwChargeSensitivity(TChain *chain=0);
  ~QwChargeSensitivity();
  
  Int_t    Cut(Long64_t entry);
  Int_t    GetEntry(Long64_t entry);
  Int_t    ReadConfig(QwChargeSensitivity *);
  Int_t    ErrorCodeCheck(void);

  Long64_t LoadTree(Long64_t entry);

  void     Init(TChain *tree);
  void     Loop();
  void     Show(Long64_t entry = -1);
  void     LoadRootFile(TString, TChain *);
  void     ScanData(void);
  void     BuildDetectorVectors(void);
  void     CalculateChargeSlope();
  void     CalculateAverageSlope();
  void     Write(void);

  Bool_t   Notify();
  Bool_t   FileSearch(TString, TChain *);
  Bool_t   CheckFlags(void);
  Bool_t   IfExists(const char *);

};

#endif


