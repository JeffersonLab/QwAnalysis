
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

class QwMpsOnly {

private:
  static const Int_t kNMaxMon = 100;//number used to allocate sufficient space
  static const Int_t kNMaxDet = 100;
  static const Int_t kNMaxCoil = 100;
  static const Int_t kNMod = 5;//kNMod is distinct modulation patterns
  static const Int_t kNCoil = 10;//actual number of coils is sine + cosine.
  //This is distinct from fNCoil which is number of coils used in an analysis.
  static const Int_t kBranchSize = 13;
  static const Int_t kDeviceErrorCode = 6;
  static const Int_t kError = 1;
  static const Int_t kRampPedestal = 128;
  static const Double_t kDegreesExcluded = 45;
  static const Double_t kErrorFlag = -999999;
  static const Double_t PI = 3.14159265358979312;
  static const Double_t kDegToRad = 1.74532925199432955e-02;

  char *fg[4];//used by FindRampPeriodAndOffset() and Write()
  Bool_t fFractionalDetectorSlopes;
  Bool_t fFullCycle;
  Bool_t f2DFit;
  Bool_t fNewEbpm;
  Int_t fMacroCycleNum;
  Int_t fCycleNum[kNMod];
  Int_t fUseDataTertile;
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
  Int_t fNCoil;
  Int_t fPedestal;
  Int_t fNModEvents;
  Int_t fCurrentCut;
  Int_t fLowerSegment;
  Int_t fUpperSegment;


  Double_t fDegPerMPS;
  Double_t fEnergyCoefficientCorrectionFactor;
  Double_t fPreviousRampValue;
  Double_t fMaxRampNonLinearity;
  Double_t fLowRamp;//low bound on ramp for coefficient fits
  Double_t fHighRamp;//high bound on ramp for coefficient fits
  Double_t fRampMax;
  Double_t fRampLength;
  Double_t fRampPeriod;
  Double_t fRampOffset;
  Double_t fUnitConvert[kNMaxMon];
  Double_t rampPeriodFitRslt[4];
  Double_t rampPeriodFitRsltErr[4];
  Double_t rampOffsetFitRslt[4];
  Double_t rampOffsetFitRsltErr[4];
  Double_t rampExcludedRegion[4][kNMod];

  TString output;
  TString configFileName;

public :

  TChain          *fChain;

  TString         fFileName;

  Bool_t fCutNonlinearRegions;
  Bool_t fMakeFriendTree;

  Int_t           fCurrent;
  Int_t           fFirstEntry;
  Int_t           fLastEntry;
  Int_t           fNDetector;  
  Int_t           fNMonitor;
  Int_t           run_number;
  Int_t           fNumberEvents;
  Int_t           ramp_good;
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


  Double_t        newEbpm;

  Int_t           ModulationEvents[kNMod];
  Double_t        Eigenvaluess[kNMaxCoil];
  Double_t        Eigenvectors[kNMaxCoil][kNMaxMon];
  Double_t        AsymmetryCorrection[kNMaxDet];
  Double_t        AsymmetryCorrectionQ[kNMaxDet];
  Double_t        MonBranch[kNMaxMon][kBranchSize];  
  Double_t        DetBranch[kNMaxDet][kBranchSize];  
  Double_t        HMonBranch[kNMaxMon][kBranchSize];  
  Double_t        YMonBranch[kNMaxMon][kBranchSize];  
  Double_t        HDetBranch[kNMaxDet][kBranchSize];  
  Double_t        DeviceAsymmetryCorrection[kNMaxDet][kNMaxMon];
  Double_t        AverageAsymmetryCorrection[kNMaxDet][kNMaxMon];
  Double_t        PositionDiffMean[kNMaxDet][kNMaxMon];
  Double_t        YieldSlope[kNMaxDet][kNMaxMon];
  Double_t        YieldSlopeError[kNMaxDet][kNMaxMon];
  Double_t        MonitorMean[kNMod][kNMaxMon];
  Double_t        DetectorMean[kNMod][kNMaxDet];
  Double_t        MonitorMeanError[kNMod][kNMaxMon];
  Double_t        DetectorMeanError[kNMod][kNMaxDet];
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

  TBranch        *b_newEbpm;

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
  TBranch        *b_ramp_good;
  TBranch        *b_mx1;
  TBranch        *b_mx2;
  TBranch        *b_my1;
  TBranch        *b_my2;
  TBranch        *b_mx1_dec;
  TBranch        *b_mx2_dec;
  TBranch        *b_my1_dec;
  TBranch        *b_my2_dec;

  static const char red[8];
  static const char other[8];
  static const char normal[8];

  std::fstream config;
  std::fstream input;
  std::fstream slopes;
  std::fstream macrocycle_slopes;
  std::fstream macrocycle_coeffs;
  std::fstream macrocycle_cos_res;
  std::fstream macrocycle_sin_res;
  std::fstream eigenvectors;
  std::fstream diagnostic;
  std::fstream coil_sens;
  std::fstream charge_sens;  

  FILE *regression;

  std::vector <TString> DetectorList;
  std::vector <TString> MonitorList;
  std::vector <TString> HDetectorList;
  std::vector <TString> HMonitorList;
  std::vector <TString> YMonitorList;

  std::vector<Int_t> fOmitCoil;

  std::vector <Double_t> sens;
  std::vector <Double_t> phase;
  std::vector <Double_t> correction;
  std::vector <Double_t> correction_charge;
  std::vector <Double_t> ChargeSensitivity;
  std::vector <Double_t> ChargeSensitivityError;
  std::vector < std::vector <Double_t> > CoilData;
  std::vector < std::vector <Double_t> > CoilData_copy;
  std::vector < std::vector <Double_t> > DetectorData;
  std::vector < std::vector <Double_t> > MonitorData;
  std::vector < std::vector <Double_t> > AvDetectorSlope;
  std::vector < std::vector <Double_t> > AvMonitorSlope;
  std::vector < std::vector <Double_t> > AvDetectorSlopeError;
  std::vector < std::vector <Double_t> > AvMonitorSlopeError;
  
  std::vector < std::vector <std::vector <Double_t> > > MonitorSlope;
  std::vector < std::vector <std::vector <Double_t> > > DetectorSlope;
  std::vector < std::vector <std::vector <Double_t> > > MonitorData_copy;
  std::vector < std::vector <std::vector <Double_t> > > DetectorData_copy;
  std::vector < std::vector <std::vector <Double_t> > > MonitorSlopeError;
  std::vector < std::vector <std::vector <Double_t> > > DetectorSlopeError;
  std::vector < std::vector <std::vector <Double_t> > > DetectorSinResidual;
  std::vector < std::vector <std::vector <Double_t> > > DetectorCosResidual;
  std::vector < std::vector <std::vector <Double_t> > > DetectorSinResidualError;
  std::vector < std::vector <std::vector <Double_t> > > DetectorCosResidualError;

  Bool_t fXinit;
  Bool_t fYinit;
  Bool_t fEinit;
  Bool_t fXPinit;
  Bool_t fYPinit;
  Bool_t fSingleCoil;
  Bool_t fCharge;
  Bool_t fTransverseData;
  Bool_t fFileSegmentInclude;
  Bool_t fRunNumberSet;
  Bool_t fPhaseConfig;
  Bool_t fChiSquareMinimization;

  TString fChargeFile;
  TString fFileSegment;
  TString fFileStem;
  TString fSetStem;

  QwMpsOnly(TChain *tree = 0);
  ~QwMpsOnly();

  Int_t   AddFriendTree();
  void     BuildCoilData(); 
  void     BuildDetectorAvSlope();
  void     BuildDetectorData();
  void     BuildDetectorSlopeVector();
  void     BuildMonitorData();  
  void     BuildMonitorSlopeVector();
  void     BuildDetectorResidualVector();
  void     BuildMonitorAvSlope();
  void     BuildNewEBPM();
  void     Calculate2DSlope(Int_t, Int_t);
  void     CalculateCoefficients(Int_t);
  void     CalculateResiduals();
  Int_t    CalculateWeightedSlope(Int_t);
  void     CheckFlags(void);
  Int_t    CheckRampLinearity(TString);
  void     Clean(void);
  void     CleanFolders(void);
  void     ComputeAsymmetryCorrections(); 
  void     ComputeErrors(TMatrixD, TMatrixD, TMatrixD, TMatrixD);
  void     ComputeSlopeErrors(TMatrixD, TMatrixD, TMatrixD, TMatrixD);
  void     CopyDataVectors(Int_t);
  Int_t    ConvertPatternNumber(Int_t);
  Int_t    Cut(Long64_t entry);
  Int_t    ErrorCodeCheck(TString);
  Bool_t   FileSearch(TString, TChain *, Bool_t slug = false);
  Double_t FindChiSquareMinAMatrix(Int_t, Int_t);
  Double_t FindChiSquareMinRMatrix(Int_t, Int_t);
  Double_t FindChiSquareMinAMatrixError(Int_t, Int_t);
  Double_t FindChiSquareMinRMatrixError(Int_t, Int_t);
  Double_t FindDegPerMPS();
  Int_t    FindRampExcludedRegions();
  Int_t    FindRampPeriodAndOffset();
  Int_t    FindRampRange();
  Int_t    GetCurrentCut();
  Double_t GetDegPerMPS();
  Int_t    GetEntry(Long64_t entry);
  void     GetOptions(Int_t, Char_t **);
  TString  GetOutput();
  Bool_t   IfExists(const char *);
  void     Init(TChain *tree);
  Long64_t LoadTree(Long64_t entry);
  Bool_t   LoadRootFile(TString, TChain *, Bool_t slug = false);
  Int_t    MakeFriendTree(Bool_t);
  void     MatrixFill(Bool_t);
  Bool_t   Notify();
  Int_t    PilferData();
  void     PrintAverageSlopes();
  void     PrintError(TString);
  Int_t    ProcessMicroCycle(Int_t, Int_t *, Int_t *, Int_t*);
  void     ReadChargeSensitivity();
  Int_t    ReadConfig(TString opt = "");
  Int_t    ReadPhaseConfig(Char_t *);
  void     ReduceMatrix(Int_t);
  void     Scan(Bool_t);
  void     SetDegPerMPS(Double_t);
  void     SetFileName(TString &); 
//   void     SetFlags(void);
  void     SetHuman(void);
  void     SetMaxRampNonLinearity(Double_t);
  void     SetPhaseValues(Double_t *);
  void     SetOutput(char *);
  void     SetRampScaleAndOffset();
  void     SetupHelBranchAddress(void); 
  void     SetupMpsBranchStatuses(Bool_t, Bool_t, Bool_t); 
  void     SetupMpsBranchAddresses(Bool_t, Bool_t, Bool_t); 
  void     Show(Long64_t entry = -1);  
  void     Write(Bool_t);
};

#endif


