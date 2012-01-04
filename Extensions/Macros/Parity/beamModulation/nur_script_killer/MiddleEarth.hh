#ifndef Middle_Earth_h
#define Middle_Earth_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

class MiddleEarth {
public :
  TChain          *fChain;
  Int_t           fCurrent; 

  static const Int_t     good_mod_data;

  Double_t        event_number;
  Double_t        ramp_hw_sum;
  Double_t        ramp_block0;
  Double_t        ramp_block1;
  Double_t        ramp_block2;
  Double_t        ramp_block3;
  Double_t        ramp_num_samples;
  Double_t        ramp_Device_Error_Code;
  Double_t        ramp_hw_sum_raw;
  Double_t        ramp_block0_raw;
  Double_t        ramp_block1_raw;
  Double_t        ramp_block2_raw;
  Double_t        ramp_block3_raw;
  Double_t        ramp_sequence_number;
  Double_t        bm_pattern_number;
  Double_t        ErrorFlag;
  
  TBranch        *b_event_number;  
  TBranch        *b_ramp;   
  TBranch        *b_bm_pattern_number; 
  TBranch        *b_ErrorFlag; 
  
  Bool_t fXinit;
  Bool_t fYinit;
  Bool_t fEinit;
  Bool_t fXPinit;
  Bool_t fYPinit;

  Int_t  run_number;

   MiddleEarth(TChain *chain=0);
  ~MiddleEarth();
  
  Int_t    Cut(Long64_t entry);
  Int_t    GetEntry(Long64_t entry);
  Long64_t LoadTree(Long64_t entry);
  void     Init(TChain *tree);
  void     Loop();
  Bool_t   Notify();
  void     Show(Long64_t entry = -1);

  Bool_t   FileSearch(TString, TChain *);
  Bool_t   CheckFlags(void);

  void     LoadRootFile(TString, TChain *);
  void     ScanData(void);
};

#endif


