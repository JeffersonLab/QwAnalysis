#ifndef QwDataQuality_h
#define QwDataQuality_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <fstream>
#include "TMatrixD.h"
#include "TH2.h"

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


public:


  QwDataQuality(TChain *tree);
  ~QwDataQuality();

  void SetTitle(TString);
  void SetXaxisTitle(TString);
  void SetYaxisTitle(TString);
  void HistoByWien(TString, Int_t);
  void FillHistoByWien(Int_t, TString, TH1F *, TString);
  void LoadRootFile(TString, TChain *);

  TString GetTitle(TString);
  TString GetXaxisTitle(TString);
  TString GetYaxisTitle(TString);
  TString GetBranchName(TString);
  TString GetLeafName(TString);

  Bool_t FileSearch(TString, TChain *);
  Bool_t IfExists(const char *);


};

#endif
