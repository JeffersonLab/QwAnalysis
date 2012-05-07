#ifndef QwDiagnostic_h
#define QwDiagnostic_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <fstream>
#include "TMatrixD.h"
#include "TH2.h"
#include "QwData.hh"
#include "TProfile.h"

class QwDiagnostic {

private:
  Int_t kDetectorRead;
  Int_t kMonitorRead;
  Int_t kXModulation;
  Int_t kYModulation;
  Int_t kEModulation;
  Int_t kXPModulation;
  Int_t kYPModulation;
  Int_t fNEvents;
  Int_t fNModType;

  static const Int_t fNMaxMon = 6;
  static const Int_t fNMaxDet = 30;
  static const Int_t fNMaxCoil = 6;
  static const Int_t fBranchSize = 13;
  static const Int_t fDeviceErrorCode = 6;
  static const Int_t fError = 1;
  static const Int_t fCurrentMin = 175;
  static const Int_t fRampPedestal = 128;

public :

  TChain          *fChain;

  TString         fFileName;

  Int_t           fCurrent;
  Int_t           fNDetector;  
  Int_t           fNMonitor;
  Int_t           run_number;

  static const char red[8];
  static const char other[8];
  static const char normal[8];

  std::fstream config;
  std::fstream input;

  FILE *slope_diagnostic;
  FILE *correction;
  FILE *charge_asym_diagnostic;
  FILE *regression;

  std::vector <TString> DetectorList;
  std::vector <TString> MonitorList;

  std::vector <Int_t> run;

  std::vector <Double_t> AsymMean;
  std::vector <Double_t> AsymRMS;
  std::vector <Double_t> AsymEntries;
  std::vector <Double_t> RawAsymMean;
  std::vector <Double_t> RawAsymRMS;

  std::vector < std::vector <Double_t> > Sens;

  std::vector <TH1F *>  DiffHistogram;
  std::vector <TH1F *>  AsymHistogram;
  std::vector <TH1F *>  RawAsymHistogram;
  std::vector <TH1F *>  CorrectionHistogram;
  std::vector <TH1F *>  TotalCorrectionHistogram;
  std::vector <TProfile *> ChargeAsymHistogram;
  std::vector <TProfile *> NaturalSensitivity;

  std::vector < std::vector <TProfile *> > SensHistogram;
  std::vector < std::vector <TProfile *> > SensHistogramCorr;
//   std::vector < std::vector <TH2F *> > ChargeAsymHistogram;

  std::vector <QwData *> DiffSlope;
  std::vector <QwData *> ChargeAsym;  
  std::vector <QwData *> TotalCorrection;

  std::vector <std::vector <QwData *> > Correction;
  std::vector <std::vector <QwData *> > NatSlope;

  std::vector <std::vector <std::vector <QwData *> > > Slope;
  std::vector <std::vector <std::vector <QwData *> > > SlopeCorr;

  QwDiagnostic(TChain *tree = 0);
  ~QwDiagnostic();

  Int_t    Cut(Long64_t entry);
  Int_t    GetEntry(Long64_t entry);
  Int_t    ReadConfig(QwDiagnostic *);
  Int_t    ErrorCodeCheck(TString);
  Int_t    GetModNumber(void);

  Long64_t LoadTree(Long64_t entry);

  void     Init(TChain *tree);
  void     Scan(QwDiagnostic *);
  void     Show(Long64_t entry = -1);  
  void     LoadRootFile(TString, TChain *);
  void     SetFileName(TString &);
  void     SetupHelBranchAddress(void); 
  void     Clean(void);
  void     BuildVectors(void);
  void     Write(void);
  void     ReadSlopes(void);
  void     PrintError(TString);
  void     CleanFolders(void);
  void     ReadSensitivities(void);

  Bool_t   Notify(void);
  Bool_t   FileSearch(TString, TChain *);
  Bool_t   IfExists(const char *);
  
};

#endif


