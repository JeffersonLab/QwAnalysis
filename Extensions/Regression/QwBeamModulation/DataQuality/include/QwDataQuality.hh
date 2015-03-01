#ifndef QwDataQuality_h
#define QwDataQuality_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <fstream>
#include "TMatrixD.h"
#include "TH2.h"
#include "TGraphErrors.h"

class QwDataQuality {

private:

  TString fTitle;
  TString fXaxisTitle;
  TString fYaxisTitle;
  TString fFilename;
  TString fOutputName;

  Double_t fXmaximum;
  Double_t fXminimum;
  Double_t fYmaximum;
  Double_t fYminimum;
  Double_t fNbins;

  Bool_t fFileOpen;
  Bool_t fStatError;
  Bool_t fSignCorr;
  Bool_t fRun1;
  Bool_t fRun2;

  Int_t fXcanvas;
  Int_t fYcanvas;

  TChain *fChain;

  std::vector <Double_t> fValue;
  std::vector <Double_t> fWeight;
  std::vector <Double_t> fError;
  std::vector <Double_t> fRMS;
  std::vector <Double_t> fN;
  std::vector <Double_t> fRunlet;

  void DeleteHisto(TH1F *);
  void DeleteHisto(std::vector <TH1F *> histo);

public:


  QwDataQuality(TChain *tree);
  ~QwDataQuality();

  void SetTitle(TString);
  void SetXaxisTitle(TString);
  void SetYaxisTitle(TString);
  void HistoByWien(TString);
  void HistoByWien(TString, TString);
  void HistoByWien(std::vector <Double_t>);
  void PullByWien(TString);
  void FillHistoByWien(Int_t, TString, TH1F *);
  void FillHistoByWien(Int_t, TString, TH1F *, TString);
  void FillPullByWien(Int_t, TString, TH1F *, Double_t);
  void LoadRootFile(TString, TChain *);
  void PlotByWien(TString);
  void PlotByWien(TString, TString);
  void GetDataByWien(Int_t, TString);
  void GetDataByWien(Int_t, TString, TString);
  void ClearVectors(void);
  void FitGraphWithStats(TGraphErrors *, TF1 *, Float_t, Float_t, Float_t, Float_t);
  void FitGraphWithStats(TH1F *, TF1 *, Float_t, Float_t, Float_t, Float_t);
  void FillDataVector(std::vector <Double_t> &, TString);
  void FillDataVector(std::vector <Double_t> &, TString, Int_t);
  void GetWeightedAverage(std::vector <Double_t>, std::vector <Double_t>);
  void SetValue(std::vector <Double_t>);
  void SetError(std::vector <Double_t>);
  void SetRMS(std::vector <Double_t>);
  void SetXmaximum(Int_t);
  void SetYmaximum(Int_t);
  void SetXminimum(Int_t);
  void SetYminimum(Int_t);
  void SetExtrema(std::vector <Double_t>);
  void SetExtrema(std::vector <Double_t>, std::vector <Double_t>);
  void SetStatisticalError(Bool_t);
  void SetOutputFilename(TString);
  void SetSignCorr(Bool_t);
  void SetRunPeriod(TString);
  void CalculateCorrelation(TString, TString );
  void PlotCorrelation(std::vector <Double_t> x, std::vector <Double_t> y, std::vector <Double_t> x_err, std::vector <Double_t> y_err);

  Double_t GetValue(Int_t);
  Double_t GetError(Int_t);
  Double_t GetRMS(Int_t);

  TString GetTitle(void);
  TString GetXaxisTitle(void);
  TString GetYaxisTitle(void);
  TString GetBranchName(TString);
  TString GetLeafName(TString);

  Bool_t FileSearch(TString, TChain *);
  Bool_t IfExists(const char *);


};

#endif
