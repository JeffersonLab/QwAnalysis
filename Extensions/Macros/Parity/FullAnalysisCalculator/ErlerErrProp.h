/* header file */

#ifndef ERLERERRPROP_H
#define ERLERERRPROP_H

// System headers
#include <vector>

#include "TMatrixD.h"

using namespace std;

class ErlerErrProp{

 public:
  ErlerErrProp(){};
  ErlerErrProp(std::vector<Double_t> APVAll);
  ~ErlerErrProp(){};

  std::vector <Double_t> EvalCumulativeAsym();
  
 protected:
  Double_t EvalWts(TMatrixD &Mat, Int_t idx);
  void DropColumn(TMatrixD &Mat,Int_t colNum);

  Int_t dataSetN;
  //  TMatrixD AsymMatD, RhoMat, ErrMatNoCorr, ErrMatWithCorr;
  std::vector <Double_t> asymData;
  std::vector <Double_t> totErr;
  Double_t finalAsym;
  Double_t cumErr, cumStatErr, cumSystErr;
  std::vector <Double_t> Asym;
  
};


#endif
