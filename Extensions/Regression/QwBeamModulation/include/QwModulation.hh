
//////////////////////////////////////////////////////////
//
//    QwModulation.hh
//    
//    Author: Joshua Hoskins
//
//////////////////////////////////////////////////////////

#ifndef QwModulation_h
#define QwModulation_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <fstream>
#include "TMatrixD.h"

class QwModulation {

private:
  Int_t fDetectorRead;
  Int_t fMonitorRead;
  Int_t fXModulation;
  Int_t fYModulation;
  Int_t fEModulation;
  Int_t fXPModulation;
  Int_t fYPModulation;
  Int_t fNEvents;
  Int_t fReduceMatrix_x;
  Int_t fReduceMatrix_y;
  Int_t fReduceMatrix_xp;
  Int_t fReduceMatrix_yp;
  Int_t fReduceMatrix_e;
  Int_t fSensHumanReadable;
  Int_t fNModType;

  Int_t fXNevents;
  Int_t fXPNevents;
  Int_t fENevents;
  Int_t fYNevents;
  Int_t fYPNevents;
  Int_t fCurrentCut;
  Int_t fLowerSegment;
  Int_t fUpperSegment;

  static const Int_t fNMaxMon = 6;
  static const Int_t fNMaxDet = 35;
  static const Int_t fNMaxCoil = 6;
  static const Int_t fBranchSize = 13;
  static const Int_t fDeviceErrorCode = 6;
  static const Int_t fError = 1;
  static const Int_t fRampPedestal = 128;

public :

  TChain          *fChain;

  TString         fFileName;

  Int_t           fCurrent;
  Int_t           fNDetector;  
  Int_t           fNMonitor;
  Int_t           run_number;
  Int_t           fNumberEvents;

  // Definitions for branches in the Mps_Tree

  Double_t        qwk_charge_hw_sum;
  Double_t        qwk_charge_block0;
  Double_t        qwk_charge_block1;
  Double_t        qwk_charge_block2;
  Double_t        qwk_charge_block3;
  Double_t        qwk_charge_num_samples;
  Double_t        qwk_charge_Device_Error_Code;
  Double_t        bm_pattern_number;
  Double_t        event_number;
  Double_t        ErrorFlag;
  Double_t        fgx1_hw_sum;
  Double_t        fgx1_block0;
  Double_t        fgx1_block1;
  Double_t        fgx1_block2;
  Double_t        fgx1_block3;
  Double_t        fgx1_num_samples;
  Double_t        fgx1_Device_Error_Code;
  Double_t        fgx1_hw_sum_raw;
  Double_t        fgx1_block0_raw;
  Double_t        fgx1_block1_raw;
  Double_t        fgx1_block2_raw;
  Double_t        fgx1_block3_raw;
  Double_t        fgx1_sequence_number;
  Double_t        fgy1_hw_sum;
  Double_t        fgy1_block0;
  Double_t        fgy1_block1;
  Double_t        fgy1_block2;
  Double_t        fgy1_block3;
  Double_t        fgy1_num_samples;
  Double_t        fgy1_Device_Error_Code;
  Double_t        fgy1_hw_sum_raw;
  Double_t        fgy1_block0_raw;
  Double_t        fgy1_block1_raw;
  Double_t        fgy1_block2_raw;
  Double_t        fgy1_block3_raw;
  Double_t        fgy1_sequence_number;
  Double_t        fgx2_hw_sum;
  Double_t        fgx2_block0;
  Double_t        fgx2_block1;
  Double_t        fgx2_block2;
  Double_t        fgx2_block3;
  Double_t        fgx2_num_samples;
  Double_t        fgx2_Device_Error_Code;
  Double_t        fgx2_hw_sum_raw;
  Double_t        fgx2_block0_raw;
  Double_t        fgx2_block1_raw;
  Double_t        fgx2_block2_raw;
  Double_t        fgx2_block3_raw;
  Double_t        fgx2_sequence_number;
  Double_t        fgy2_hw_sum;
  Double_t        fgy2_block0;
  Double_t        fgy2_block1;
  Double_t        fgy2_block2;
  Double_t        fgy2_block3;
  Double_t        fgy2_num_samples;
  Double_t        fgy2_Device_Error_Code;
  Double_t        fgy2_hw_sum_raw;
  Double_t        fgy2_block0_raw;
  Double_t        fgy2_block1_raw;
  Double_t        fgy2_block2_raw;
  Double_t        fgy2_block3_raw;
  Double_t        fgy2_sequence_number;
  Double_t        fge_hw_sum;
  Double_t        fge_block0;
  Double_t        fge_block1;
  Double_t        fge_block2;
  Double_t        fge_block3;
  Double_t        fge_num_samples;
  Double_t        fge_Device_Error_Code;
  Double_t        fge_hw_sum_raw;
  Double_t        fge_block0_raw;
  Double_t        fge_block1_raw;
  Double_t        fge_block2_raw;
  Double_t        fge_block3_raw;
  Double_t        fge_sequence_number;
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

  Double_t        AsymmetryCorrection[fNMaxDet];
  Double_t        AsymmetryCorrectionQ[fNMaxDet];
  Double_t        MonBranch[fNMaxMon][fBranchSize];  
  Double_t        DetBranch[fNMaxDet][fBranchSize];  
  Double_t        HMonBranch[fNMaxMon][fBranchSize];  
  Double_t        YMonBranch[fNMaxMon][fBranchSize];  
  Double_t        HDetBranch[fNMaxDet][fBranchSize];  
  Double_t        CoilBranch[fNMaxCoil][fBranchSize];
  Double_t        DeviceAsymmetryCorrection[fNMaxDet][fNMaxMon];
  Double_t        AverageAsymmetryCorrection[fNMaxDet][fNMaxMon];
  Double_t        PositionDiffMean[fNMaxDet][fNMaxMon];
  Double_t        YieldSlope[fNMaxDet][fNMaxMon];
  Double_t        YieldSlopeError[fNMaxDet][fNMaxMon];
  Double_t        MonitorMean[fNMaxMon];
  Double_t        DetectorMean[fNMaxDet];

  Double_t        qwk_targetX_hw_sum;

  TBranch        *b_qwk_charge;    
  TBranch        *b_bm_pattern_number;    
  TBranch        *b_event_number;    
  TBranch        *b_ErrorFlag;    
  TBranch        *b_fgx1;   
  TBranch        *b_fgy1;   
  TBranch        *b_fgx2;   
  TBranch        *b_fgy2;   
  TBranch        *b_fge;   
  TBranch        *b_ramp;  
  TBranch        *b_qwk_targetX_hw_sum;

  // Definitions for branches in the Hel_Tree

  Double_t        yield_bm_pattern_number;
  Double_t        yield_qwk_charge_hw_sum;
  Double_t        yield_qwk_charge_block0;
  Double_t        yield_qwk_charge_block1;
  Double_t        yield_qwk_charge_block2;
  Double_t        yield_qwk_charge_block3;
  Double_t        yield_qwk_charge_num_samples;
  Double_t        yield_qwk_charge_Device_Error_Code;
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

  TBranch        *b_yield_bm_pattern_number;
  TBranch        *b_yield_qwk_charge;
  TBranch        *b_mps_counter;
  TBranch        *b_yield_ramp; 
  TBranch        *b_yield_qwk_mdallbars; 
  TBranch        *b_asym_qwk_charge; 

  static const char red[8];
  static const char other[8];
  static const char normal[8];

  std::fstream config;
  std::fstream input;
  std::fstream slopes;
  std::fstream diagnostic;
  std::fstream charge_sens;  

  FILE *regression;

  std::vector <TString> DetectorList;
  std::vector <TString> MonitorList;
  std::vector <TString> HDetectorList;
  std::vector <TString> HMonitorList;
  std::vector <TString> YMonitorList;

  std::vector <Double_t> sens;
  std::vector <Double_t> correction;
  std::vector <Double_t> correction_charge;
  std::vector <Double_t> ChargeSensitivity;
  std::vector <Double_t> ChargeSensitivityError;

  std::vector < std::vector <Double_t> > CoilData;
  std::vector < std::vector <Double_t> > DetectorData;
  std::vector < std::vector <Double_t> > MonitorData;
  std::vector < std::vector <Double_t> > AvDetectorSlope;
  std::vector < std::vector <Double_t> > AvMonitorSlope;
  std::vector < std::vector <Double_t> > AvDetectorSlopeError;
  std::vector < std::vector <Double_t> > AvMonitorSlopeError;
  
  std::vector < std::vector <std::vector <Double_t> > > MonitorSlope;
  std::vector < std::vector <std::vector <Double_t> > > DetectorSlope;
  std::vector < std::vector <std::vector <Double_t> > > MonitorSlopeError;
  std::vector < std::vector <std::vector <Double_t> > > DetectorSlopeError;

  Bool_t fXinit;
  Bool_t fYinit;
  Bool_t fEinit;
  Bool_t fXPinit;
  Bool_t fYPinit;
  Bool_t fSingleCoil;
  Bool_t fCharge;
  Bool_t fFileSegmentInclude;

  TString fChargeFile;
  TString fFileSegment;

  QwModulation(TChain *tree = 0);
  ~QwModulation();

  Int_t    Cut(Long64_t entry);
  Int_t    GetEntry(Long64_t entry);
  Int_t    ReadConfig(QwModulation *);
  Int_t    ErrorCodeCheck(TString);
  Int_t    ConvertPatternNumber(Int_t);
  Int_t    GetCurrentCut();

  Long64_t LoadTree(Long64_t entry);

  void     Init(TChain *tree);
  void     Scan(QwModulation *);
  void     PilferData();
  void     Show(Long64_t entry = -1);  
  void     LoadRootFile(TString, TChain *);
  void     BuildDetectorData();
  void     BuildMonitorData();
  void     BuildCoilData(); 
  void     BuildMonitorSlopeVector();
  void     BuildDetectorSlopeVector();
  void     BuildMonitorAvSlope();
  void     BuildDetectorAvSlope();
  void     CalculateWeightedSlope();
  void     CalculateSlope(Int_t);
  void     MatrixFill();
  void     ComputeErrors(TMatrixD, TMatrixD, TMatrixD, TMatrixD);
  void     SetFileName(TString &);
  void     ComputeAsymmetryCorrections(); 
  void     ReduceMatrix(Int_t);
  void     SetHuman(void);
  void     SetupMpsBranchAddress(void); 
  void     SetupHelBranchAddress(void); 
  void     PrintError(TString);
  void     Write();
  void     Clean(void);
  void     CleanFolders(void);
  void     GetOptions(Char_t **);
//   void     SetFlags(void);
  void     CheckFlags(void);
  void     ReadChargeSensitivity();

  Bool_t   Notify();
  Bool_t   FileSearch(TString, TChain *);
  Bool_t   IfExists(const char *);
  
};

#endif


