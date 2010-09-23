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
#include "ToyQwChannel.h"

class JbCorrelator: public  CorrelatorEntity  {

 private:
  Double_t *Pvec, *Yvec;
  TString mCore;
 
  // histograms
  enum {mxHA=128}; TH1 * hA[mxHA];
  void initHistos();
  LinRegBevPeb linReg;

 public:
  JbCorrelator(const char *core); 
  void print();
  void init();
  void finish();
  void addEvent(Double_t **valPA); // array of pointers
  
};

#endif
