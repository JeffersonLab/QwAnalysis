#ifndef QwDataContainer_hh
#define QwDataContainer_hh

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <fstream>
#include "TMatrixD.h"
#include "TH2.h"
#include <TVectorD.h>
#include "headers.h"
#include "QwDBConnection.hh"
#include "QwData.hh"

class QwDataContainer: public QwDBConnection {

private:

  TString fFileName;

  QwDBConnection dbase;

public:

  enum variable_t {var_x, var_xp, var_e, var_y, var_yp};
  enum quality_t {raw, corr};

  std::vector <TVectorD> fSensitivity;
  std::vector <TVectorD> fCorrection;
  std::vector <TVectorD> fAsymmetry;
  std::vector <TVectorD> fPositionDiff;
  std::vector <TVectorD> fRMS;

  TVectorD fRunNumber;
  TVectorD fAvSensx;
  TVectorD fAvSensxp;
  TVectorD fAvSense;
  TVectorD fAvSensy;
  TVectorD fAvSensyp;

  TVectorD fCorrx;
  TVectorD fCorrxp;
  TVectorD fCorre;
  TVectorD fCorry;
  TVectorD fCorryp;

  TVectorD fAvRawAsym;
  TVectorD fAvCorrAsym;

  TVectorD fRawAsym;
  TVectorD fCorrAsym;

  TVectorD fNumberEntries;

  TVectorD fRawRMS;
  TVectorD fCorrRMS;

  TVectorD fRawError;
  TVectorD fCorrError;

  TVectorD fAvRawError;
  TVectorD fAvCorrError;

  TVectorD fTotalCorr;
  TVectorD fZero;
  TVectorD fWienNumber;


  TString  fMonitor;
  TString  fDetector;

  TString  fMysqlSlope;
  TString  fMysqlDet;
  TString  fMysqlIHWP;
  TString  fMysqlMon;
  TString  fMysqlQuery;

  Bool_t   fDBase;
  Bool_t   fRunRange;
  Int_t    fNumberLines;
  Int_t    fMysqlRunUpper;
  Int_t    fMysqlRunLower;


  Int_t    entries;
  Int_t    fNumberMonitor;


  std::vector <TString> fOptions;
  std::vector <Int_t>   fBlackList;

  QwDataContainer();
  ~QwDataContainer();

  void ReadDataFile(TString);
  void ResizeDataElements(Int_t);
  void SetFileName(TString);
  void ScaleTGraph(TGraphErrors *);
  void PlotSensitivities(void);
  void PlotCorrections(void);
  void PlotDetectorRMS(void);
  void CalculateAverage(void);
  void GetOptions(Char_t **);
  void PlotDBSensitivities(void);
  void GetDBSensitivities(TString, TString, TString);
  void GetDBAsymmetry(TString, TString, TString);
  void GetBlackList(void);
  void FillDataVector(TString);
  void FillDataVector(Int_t);
  void SetRunRange(Int_t, Int_t);
  void SetDetector(TString);
  void SetSlope(TString);
  void SetIHWP(TString);
  void SetMonitor(TString);
  void Print(void);
  void Close(void);
  void ConnectDB(void);

  TString  SetBlackList(TString, TString);
  TString  BuildQuery(TString);

  Int_t GetFileSize(TString);
  Int_t QueryDB(const char* db=0);

  Double_t GetMaximum(TVectorD);
  Double_t GetMinimum(TVectorD);
  Double_t GetMaximum(Double_t *);
  Double_t GetMinimum(Double_t *);

  //  TVectorD push_back(Double_t);

};

#endif
