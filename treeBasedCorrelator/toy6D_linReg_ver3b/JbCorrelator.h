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
 
  // histograms
  enum {mxHA=128}; TH1 * hA[mxHA];
  void initHistos();
  LinRegBevPeb linReg;

 public:
  JbCorrelator(const char *core); 
  void print();
  void init(int nP, int nY);
  void finish();
  void addEvent(double *Pvec, double *Yvec);

};

#endif
