#ifndef JBCORRELATOR_H
#define JBCORRELATOR_H
/*********************************************************************
 * $Id: $
 * \author Jan Balewski, MIT, 2010
 *********************************************************************
 * Descripion:
 * Finds correlation and alphas for  QW event 
 *********************************************************************/

#include <assert.h>
#include <TString.h> 
class TH1;

#include "LinReg_Bevington_Pebay.h"

class JbCorrelator  {
 private:

  TString mCore;
  int nP,nY;
  TString *Pname, *Yname;
  
  // histograms
  enum {mxHA=32}; TH1 * hA[mxHA];

  // monitoring histos for iv & dv
  TH1 ** h1iv, **h2iv, ** h1dv, **h2dv;
  void initHistos();
  LinRegBevPeb linReg;

 public:
  JbCorrelator(const char *core); 
  ~JbCorrelator();
  void print();
  void init(int nP0, int nY0, TString *Pname0, TString *Yname0);
  void finish();
  void addEvent(double *Pvec, double *Yvec);
  void exportAlphas(TString outPath);

};

#endif
