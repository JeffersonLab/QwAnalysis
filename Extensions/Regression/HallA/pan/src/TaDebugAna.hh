//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaDebugAna.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaDebugAna.hh,v 1.5 2003/07/31 16:11:59 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Debug analysis.  This class derives from TaStandardAna.  No ROOT
//    file is created, and some basic information for each event is
//    printed.  It also prints statistics on differences and
//    asymmetries of beam monitors and detectors periodically.
//
////////////////////////////////////////////////////////////////////////

#ifndef PAN_TaDebugAna
#define PAN_TaDebugAna

#include <TObject.h>
#include <TTree.h> 
#include "TaStandardAna.hh"

//class TaStatistics;
class TaRun;
class VaEvent;

class TaDebugAna: public TaStandardAna {
  
public:
  
  // Constructors/destructors/operators
  TaDebugAna();
  
private:
  
  // Member functions
  void EventAnalysis ();
  
  // Data members
  
#ifndef NODICT
  ClassDef(TaDebugAna, 0)  // Debugging data analysis
#endif
};

#endif
