//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaFeedbackAna.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaFeedbackAna.hh,v 1.2 2003/07/31 16:11:59 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Feedback analysis. Exclusive task is to compute and send feedback 
//    parameters to the source.   
//
////////////////////////////////////////////////////////////////////////

#ifndef PAN_TaFeedbackAna
#define PAN_TaFeedbackAna

#include <TObject.h>
#include "VaAnalysis.hh"

//class TaStatistics;
class TaRun;

class TaFeedbackAna: public VaAnalysis {
  
public:
  
  // Constructors/destructors/operators
  TaFeedbackAna();
  ~TaFeedbackAna();
  // We should not need to copy or assign an analysis, so copy
  // constructor and operator= are private.
  
  // Major functions
  
  // Data access functions
  
private:
  
  // We should not need to copy or assign an analysis, so copy
  // constructor and operator= are private.
  TaFeedbackAna(const TaFeedbackAna& copy);
  TaFeedbackAna& operator=( const TaFeedbackAna& assign);
  
  // Member functions
  void EventAnalysis ();
  void PairAnalysis ();
  void InitChanLists ();
  
  // Data members
  
#ifndef NODICT
  ClassDef(TaFeedbackAna, 0)  // Analysis of feedback parameters
#endif
};

#endif
