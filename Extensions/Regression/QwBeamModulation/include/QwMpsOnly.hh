
//////////////////////////////////////////////////////////
//
//    QwMpsOnly.hh
//    
//    Author: Joshua Hoskins
//    Modified: Don Jones
//////////////////////////////////////////////////////////

#ifndef QwMpsOnly_h
#define QwMpsOnly_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <fstream>
#include <vector>
#include "TMatrixD.h"
#include "QwMpsOnly.hh"

class QwMpsOnly {

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
  Int_t fPedestal;

  Int_t fNModEvents;
  Int_t fCurrentCut;
  Int_t fLowerSegment;
  Int_t fUpperSegment;


  Double_t fDegPerEntry;
  Double_t fPreviousRampValue;
  Double_t fMaxRampNonLinearity;
  Double_t fRampMax;
  Double_t fRampMin;
  Double_t fRampPeriod;
  Double_t fRampOffset;
  Double_t fRampReturnSlope;

  static const Int_t kNMaxMon = 6;
  static const Int_t kNMaxDet = 35;
  static const Int_t kNMaxCoil = 5;
  static const Int_t kBranchSize = 13;
  static const Int_t kDeviceErrorCode = 6;
  static const Int_t kError = 1;
  static const Int_t kRampPedestal = 128;
  static const Double_t PI = 3.14159265358979312;
  static const Double_t kDegToRad = 1.74532925199432955e-02;

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
  Double_t        qwk_charge_Device_Error_Code;
  Double_t        bm_pattern_number;
  Double_t        event_number;
  Double_t        ErrorFlag;
  Double_t        fgx1_hw_sum;
  Double_t        fgx1_Device_Error_Code;
  Double_t        fgy1_hw_sum;
  Double_t        fgy1_Device_Error_Code;
  Double_t        fgx2_hw_sum;
  Double_t        fgx2_Device_Error_Code;
  Double_t        fgy2_hw_sum;
  Double_t        fgy2_Device_Error_Code;
  Double_t        fge_hw_sum;
  Double_t        fge_Device_Error_Code;
  Double_t        ramp_hw_sum;
  Double_t        ramp_block0;
  Double_t        ramp_block1;
  Double_t        ramp_block2;
  Double_t        ramp_block3;
  Double_t        ramp_Device_Error_Code;
  Double_t        ramp_filled;

  Int_t           ModulationEvents[kNMaxCoil];
  Double_t        AsymmetryCorrection[kNMaxDet];
  Double_t        AsymmetryCorrectionQ[kNMaxDet];
  Double_t        MonBranch[kNMaxMon][kBranchSize];  
  Double_t        DetBranch[kNMaxDet][kBranchSize];  
  Double_t        HMonBranch[kNMaxMon][kBranchSize];  
  Double_t        YMonBranch[kNMaxMon][kBranchSize];  
  Double_t        HDetBranch[kNMaxDet][kBranchSize];  
  Double_t        CoilBranch[kNMaxCoil][kBranchSize];
  Double_t        DeviceAsymmetryCorrection[kNMaxDet][kNMaxMon];
  Double_t        AverageAsymmetryCorrection[kNMaxDet][kNMaxMon];
  Double_t        PositionDiffMean[kNMaxDet][kNMaxMon];
  Double_t        YieldSlope[kNMaxDet][kNMaxMon];
  Double_t        YieldSlopeError[kNMaxDet][kNMaxMon];
  Double_t        MonitorMean[kNMaxMon];
  Double_t        DetectorMean[kNMaxDet];
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
  Double_t        yield_qwk_charge_Device_Error_Code;
  Double_t        pattnum; 
  Double_t        yield_ramp_hw_sum;
  Double_t        yield_ramp_Device_Error_Code;
  Double_t        yield_qwk_mdallbars_hw_sum;
  Double_t        yield_qwk_mdallbars_Device_Error_Code;
  Double_t        asym_qwk_charge_hw_sum;
  Double_t        asym_qwk_charge_Device_Error_Code;

  TBranch        *b_yield_bm_pattern_number;
  TBranch        *b_yield_qwk_charge;
  TBranch        *b_pattnum;
  TBranch        *b_yield_ramp; 
  TBranch        *b_yield_qwk_mdallbars; 
  TBranch        *b_asym_qwk_charge; 
  TBranch        *b_ramp_filled; 

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
  std::vector <Double_t> phase;
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
  Bool_t fRunNumberSet;
  Bool_t fPhaseConfig;

  TString fChargeFile;
  TString fFileSegment;
  TString fFileStem;
  TString fSetStem;
  TString output;

  QwMpsOnly(TChain *tree = 0);
  ~QwMpsOnly();

  Int_t    Cut(Long64_t entry);
  Int_t    GetEntry(Long64_t entry);
  Int_t    ReadConfig(TString opt = "");
  Int_t    ReadPhaseConfig(Char_t *);
  Int_t    ErrorCodeCheck(TString);
  Int_t    CheckRampLinearity(TString);
  Int_t    ConvertPatternNumber(Int_t);
  Int_t    GetCurrentCut();
  Int_t    ProcessMicroCycle(Int_t, Int_t *, Int_t *, Int_t*);
  Long64_t LoadTree(Long64_t entry);
  Double_t FindDegPerEntry();
  Double_t GetDegPerEntry();
  Double_t Sine(Double_t, Double_t *, Bool_t);
  void     Init(TChain *tree);
  void     Scan(void);
  void     SetMaxRampNonLinearity(Double_t);
  void     PilferData();
  void     Show(Long64_t entry = -1);  
  void     LoadRootFile(TString, TChain *, Bool_t slug = false);
  void     BuildDetectorData();
  void     BuildMonitorData();  
  void     BuildCoilData(); 
  void     BuildMonitorSlopeVector();
  void     BuildDetectorSlopeVector();
  void     BuildMonitorAvSlope();
  void     BuildDetectorAvSlope();
  void     FindRampRange();
  void     FindRampPeriodAndOffset();
  void     CalculateWeightedSlope(Int_t);
  void     CalculateSlope(Int_t);
  void     Calculate2DSlope(Int_t);
  void     SetRampScaleAndOffset();
  void     MatrixFill();
  void     ComputeErrors(TMatrixD, TMatrixD, TMatrixD, TMatrixD);
  void     SetFileName(TString &);
  void     ComputeAsymmetryCorrections(); 
  void     MakeRampFilled(Bool_t);
  void     PrintAverageSlopes();
  void     ReduceMatrix(Int_t);
  void     SetDegPerEntry(Double_t);
  void     SetHuman(void);
  void     SetupMpsBranchAddress(void); 
  void     SetupHelBranchAddress(void); 
  void     PrintError(TString);
  void     SetPhaseValues(Double_t *);
  void     Write();
  void     Clean(void);
  void     CleanFolders(void);
  void     GetOptions(Char_t **);
//   void     SetFlags(void);
  void     CheckFlags(void);
  void     ReadChargeSensitivity();

  Bool_t   Notify();
  Bool_t   FileSearch(TString, TChain *, Bool_t slug = false);
  Bool_t   IfExists(const char *);
};

#endif


