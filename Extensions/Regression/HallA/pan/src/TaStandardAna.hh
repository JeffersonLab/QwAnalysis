//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaStandardAna.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaStandardAna.hh,v 1.4 2009/08/06 18:36:03 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Standard analysis.  This class derives from VaAnalysis.  It
//    simply puts differences and asymmetries of beam monitors and
//    detectors into the output root file using the AutoPairAna lists,
//    and prints statistics on these quantities periodically.
//
////////////////////////////////////////////////////////////////////////

#ifndef PAN_TaStandardAna
#define PAN_TaStandardAna

#include <TObject.h>
#include <TTree.h> 
#include "VaAnalysis.hh"

//class TaStatistics;
class TaRun;
class VaEvent;

class TaStandardAna: public VaAnalysis {
  
public:
  
  // Constructors/destructors/operators
  TaStandardAna();
  ~TaStandardAna();
  // We should not need to copy or assign an analysis, so copy
  // constructor and operator= are private.
  
  // Major functions
  
  // Data access functions
  
private:
  
  // We should not need to copy or assign an analysis, so copy
  // constructor and operator= are private.
  TaStandardAna(const TaStandardAna& copy);
  TaStandardAna& operator=( const TaStandardAna& assign);

protected:
  
  // Member functions
  void EventAnalysis ();
  void PairAnalysis ();
  void MultipletAnalysis ();
  void InitChanLists ();
  
  // Data members
  
#ifndef NODICT
  ClassDef(TaStandardAna, 0)  // Standard data analysis
#endif
};

#endif
