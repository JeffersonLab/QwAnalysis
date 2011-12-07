#ifndef PAN_TaSimEvent
#define PAN_TaSimEvent

//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaSimEvent.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaSimEvent.hh,v 1.6 2010/02/12 16:44:30 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    An event of data, probably munged up by some varity of simulation!
//
////////////////////////////////////////////////////////////////////////

#define TASIMEVENT_VERBOSE 1

#include "VaEvent.hh"
#include "Rtypes.h"
#include "PanTypes.hh"
#include "DevTypes.hh"
#include <map>
#include <vector>
#include <iterator>
#include <string>
#include <utility>
#include "TRandom.h"

//#define FAKEHEL   // Uncomment to write in helicities (and pair/mult/timeslot) from file
//#define FAKEHELNOFILE // Uncomment to write in helicities (and pair/mult/timeslot) without file
#define FAKEDITHER // Uncomment to fake dithering

#ifdef FAKEHEL
#include <iostream>
#include <fstream>
#endif


class TRandom;
class TaDevice;
class TTree;
class TaCutList;
class TaLabelledQuantity;
class TaRun;
class TaDataBase;

class TaSimEvent : public VaEvent {

public:

  // Constructors/destructors/operators
  TaSimEvent();
  virtual ~TaSimEvent();
  TaSimEvent(const TaSimEvent &ev);
  TaSimEvent& operator=(const TaSimEvent &ev);
  TaSimEvent& CopyInPlace (const TaSimEvent& rhs);

  // Major functions
  ErrCode_t RunInit(const TaRun& run);    // initialization at start of run
  void Decode( TaDevice& devices );             // decode the event 

private:

  // Private methods
  void Create(const TaSimEvent&);
  void SetSimConstants();

  // Copies of the VaPair functions -- this is a kludge, we should 
  // have a single copy that can be called in multiple instances
  UInt_t RanBit (UInt_t hRead = 2);
  UInt_t RanBitOld (UInt_t hRead = 2);
  UInt_t RanBit24 (UInt_t hRead = 2);
  UInt_t RanBit30 (UInt_t hRead = 2);

  // Static members
#ifdef FAKEHEL
  static ifstream fgHelfile;   // fake helicity data file
#endif
#ifdef FAKEHELNOFILE
  static Int_t* fgPattern;
  static Int_t  fgNMultiplet;
  static Int_t  fgOversample;
  static Bool_t fgRandom;
  static UInt_t  fgShreg;      // value for sequence algorithm      
  static UInt_t  fgRanType;   // identifies which random bit generator
#endif

  TRandom fgR;      // Random number object
#ifdef FAKEDITHER
  Double_t fDetVsBcm[4];
  Double_t fDetNoiseR[4];
  // modify position by dithering slope for 12x, 4a x,y and 4b x,y given value
  //  and ident of dithering object
  Double_t fBpm4AXvCoil[7];   Double_t fBpm4AYvCoil[7];
  Double_t fBpm4BXvCoil[7];   Double_t fBpm4BYvCoil[7];
  Double_t fBpm12XvCoil[7];
  // modify detector by position
  Double_t fDetVsBpm12X[4];  
  Double_t fDetVsBpm4AX[4];  Double_t fDetVsBpm4BX[4];
  Double_t fDetVsBpm4AY[4];  Double_t fDetVsBpm4BY[4];
  Double_t fDet12Xoff;  
  Double_t fDet4AXoff; Double_t fDet4BXoff;
  Double_t fDet4AYoff; Double_t fDet4BYoff;
#endif

  // Data members
  
#ifndef NODICT
ClassDef(TaSimEvent,0)  // An event containing simulated data
#endif

};

#endif
