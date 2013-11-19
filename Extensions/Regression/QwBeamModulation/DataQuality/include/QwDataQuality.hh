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

  Double_t fXmaximum;
  Double_t fXminimum;
  Double_t fYmaximum;
  Double_t fYminimum;
  Double_t fNbins;

  Bool_t fFileOpen;

  TChain *fChain;

  std::vector <Double_t> fValue;
  std::vector <Double_t> fWeight;
  std::vector <Double_t> fError;
  std::vector <Double_t> fRMS;
  std::vector <Double_t> fRunlet;

public:


  QwDataQuality(TChain *tree);
  ~QwDataQuality();

  void SetTitle(TString);
  void SetXaxisTitle(TString);
  void SetYaxisTitle(TString);
  void HistoByWien(TString, Int_t);
  void HistoByWien(TString, Int_t, TString);
  void PullByWien(TString, Int_t);
  void FillHistoByWien(Int_t, TString, TH1F *);
  void FillHistoByWien(Int_t, TString, TH1F *, TString);
  void FillPullByWien(Int_t, TString, TH1F *, Double_t);
  void LoadRootFile(TString, TChain *);
  void PlotByWien(TString, Int_t);
  void PlotByWien(TString, Int_t, TString);
  void GetDataByWien(Int_t, TString);
  void GetDataByWien(Int_t, TString, TString);
  void ClearVectors(void);
  void FitGraphWithStats(TGraphErrors *, TF1 *, Float_t, Float_t, Float_t, Float_t);
  void FitGraphWithStats(TH1F *, TF1 *, Float_t, Float_t, Float_t, Float_t);

  TString GetTitle(void);
  TString GetXaxisTitle(void);
  TString GetYaxisTitle(void);
  TString GetBranchName(TString);
  TString GetLeafName(TString);

  Bool_t FileSearch(TString, TChain *);
  Bool_t IfExists(const char *);


};

#endif
