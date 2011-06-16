//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaCutInterval.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaCutInterval.hh,v 1.10 2003/07/31 16:11:58 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Cut intervals.
//    A cut interval consists of a cut type, cut value, and two event
//    numbers, corresponding to the start and end of the cut condition.
//
////////////////////////////////////////////////////////////////////////

#ifndef PAN_TaCutInterval
#define PAN_TaCutInterval

#include "TObject.h"
#include "PanTypes.hh"

class VaEvent;

class TaCutInterval
{

public:

  // Constructors/destructors/operators

  TaCutInterval ( const Cut_t cu, 
		  const Int_t val, 
		  const EventNumber_t e0, 
		  const EventNumber_t e1 );
  TaCutInterval();
  virtual ~TaCutInterval();
  TaCutInterval (const TaCutInterval& copy);
  TaCutInterval& operator= (const TaCutInterval& assign);

  // Major functions

  Bool_t Inside (const VaEvent& ev, const UInt_t lex, const UInt_t hex) const;

  // Access functions

  void SetEnd( const EventNumber_t e1 );
  const Cut_t GetCut() const;
  const Int_t GetVal() const;

  friend ostream& operator<< (ostream& s, const TaCutInterval q);

private:

  Cut_t fCut;           // Cut type, e.g. low beam, sequence error, malfunction...
  Int_t fVal;              // Cut value, nonzero, e.g. type of sequence error
  EventNumber_t fBegin;    // Number of first event of cut interval
  EventNumber_t fEnd;      // Number of first event after end of cut interval

#ifndef NODICT
  ClassDef(TaCutInterval, 0)  // Cut Intervals
#endif

};

#endif
