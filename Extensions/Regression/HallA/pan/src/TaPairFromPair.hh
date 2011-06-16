#ifndef PAN_TaPairFromPair
#define PAN_TaPairFromPair
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaPairFromPair.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPairFromPair.hh,v 1.14 2009/08/11 20:18:29 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Class which makes and analyzes opposite helicity event pairs
//    from a data stream structured as helicity window pairs.  Derived
//    from VaPair.
//
//////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "PanTypes.hh"
#include "VaPair.hh"

class TaCutList;
class VaEvent;
class TaRun;

class TaPairFromPair : public VaPair {
  
public:
  
  // Constructors/destructors/operators
  TaPairFromPair();
  TaPairFromPair(const TaPairFromPair& copy);
  TaPairFromPair& operator=( const TaPairFromPair& assign);  
  
  ~TaPairFromPair();
  
private:
  
  // Private member functions
  void CheckSequence (VaEvent&, TaRun&); // look for helicity/synch errors

#ifndef NODICT
  ClassDef(TaPairFromPair, 0)  // Event pair from window pair helicity structure
#endif
};

#endif
