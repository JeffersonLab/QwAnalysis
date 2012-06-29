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
#include "TMultiGraph.h"
#include "TLegend.h"

class QwPlotHelper {

private:
  TMultiGraph *mgraph;

  TLegend *legend;

  Style_t fMarkerStyleIN;
  Style_t fMarkerStyleOUT;
  Style_t fMarkerStyleNULL;
  Style_t fMarkerStyleAVG;

  Color_t fMarkerColorIN;
  Color_t fMarkerColorOUT;
  Color_t fMarkerColorNULL;
  Color_t fMarkerColorAVG;

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
  void InOutErrorPlotAsym(QwDataContainer &, QwDataContainer &, QwDataContainer &, QwDataContainer &, TString option="");
  void InOutErrorPlotAvAsym(QwDataContainer &, QwDataContainer &, TString option="");
  void InOutErrorPlotAvAsym(QwDataContainer &, QwDataContainer &, QwDataContainer &, QwDataContainer &, TString option="");

  void SetStyle(Style_t, Style_t, Style_t, Style_t);
  void SetColor(Color_t, Color_t, Color_t, Color_t);

  Double_t GetMaximum(Double_t *);
  Double_t GetMinimum(Double_t *);

  TMultiGraph *GetMultiGraph();

  TLegend *GetLegend();

};

#endif
