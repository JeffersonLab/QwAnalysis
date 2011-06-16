#ifndef PAN_TaPairFromQuad
#define PAN_TaPairFromQuad
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaPairFromQuad.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPairFromQuad.hh,v 1.7 2009/08/11 20:18:30 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Class which makes and analyzes opposite helicity event pairs
//    from a data stream structured as helicity window quadruples.
//    Derived from VaPair.
//
//////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "PanTypes.hh"
#include "VaPair.hh"

class TaCutList;
class VaEvent;
class TaRun;

class TaPairFromQuad : public VaPair {
  
public:
  
  // Constructors/destructors/operators
  TaPairFromQuad();
  TaPairFromQuad(const TaPairFromQuad& copy);
  TaPairFromQuad& operator=( const TaPairFromQuad& assign);  
  
  ~TaPairFromQuad();
  
  // Major functions
  ErrCode_t RunInit(const TaRun&);
  
private:
  
  // Private member functions
  void CheckSequence (VaEvent&, TaRun&); // look for helicity/synch errors

  // Data members
  static Int_t   fgQuadCount; // Quad window counter

#ifndef NODICT
  ClassDef(TaPairFromQuad, 0)  // Event pair from window quadruple helicity structure
#endif
};

#endif
