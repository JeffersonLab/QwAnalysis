#ifndef PAN_TaPairFromOctet
#define PAN_TaPairFromOctet
//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaPairFromOctet.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaPairFromOctet.hh,v 1.2 2009/08/11 20:18:09 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Class which makes and analyzes opposite helicity event pairs
//    from a data stream structured as helicity window octets.
//    Derived from VaPair.
//
//////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include "PanTypes.hh"
#include "VaPair.hh"

class TaCutList;
class VaEvent;
class TaRun;

class TaPairFromOctet : public VaPair 
{
public:
  
  // Constructors/destructors/operators
  TaPairFromOctet();
  TaPairFromOctet(const TaPairFromOctet& copy);
  TaPairFromOctet& operator=( const TaPairFromOctet& assign);  
  
  ~TaPairFromOctet();
  
  // Major functions
  ErrCode_t RunInit(const TaRun&);
  
private:
  
  // Private member functions
  void CheckSequence (VaEvent&, TaRun&); // look for helicity/synch errors

  // Data members
  static Int_t   fgOctetCount; // Octet window counter

#ifndef NODICT
  ClassDef(TaPairFromOctet, 0)  // Event pair from window octet helicity structure
#endif
};

#endif
