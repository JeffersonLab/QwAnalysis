#ifndef QwData_h
#define QwData_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <fstream>
#include "TMatrixD.h"
#include "TH2.h"

class QwData {

private:

  Double_t fSlope;
  Double_t fError;

public:

  Int_t    entries;
  Double_t weight_slope;
  Double_t inv_variance;
  Double_t slope;
  Double_t error;
  Double_t ChiSquare;
  Double_t NDF;
  Double_t ChiSquareNDF;

  QwData();
  ~QwData();

  QwData operator+(QwData& );
//   QwData   operator=(QwData& );
  void   operator+=(QwData& );

  Double_t GetSlope(void);
  Double_t GetError(void);
  Double_t GetRMS();

  void     CalcAverage(void);
};

#endif
