#ifndef QwPlotHelper_hh
#define QwPlotHelper_hh

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <fstream>
#include "TMatrixD.h"
#include "TH2.h"
#include <TVectorD.h>
#include "headers.h"
#include "QwDataContainer.hh"

class QwPlotHelper {

private:


public:

  TString fTitle;
  TString fTitleX;
  TString fTitleY;

  QwPlotHelper();
  ~QwPlotHelper();

  void ScaleTGraph(TGraphErrors *tgraph);

  void SetPlotTitle(TString, TString, TString);
  void InOutErrorPlotSens(QwDataContainer &, QwDataContainer &, Int_t type=0, TString option="");
  void InOutErrorPlotAsym(QwDataContainer &, QwDataContainer &, TString option="");
  void InOutErrorPlotAvAsym(QwDataContainer &, QwDataContainer &, TString option="");

  Double_t GetMaximum(Double_t *);
  Double_t GetMinimum(Double_t *);

};

#endif
