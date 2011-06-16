//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaCutInterval.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaCutInterval.cc,v 1.11 2003/07/31 16:11:58 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Cut intervals.
//    A cut interval consists of a cut type, cut value, and two event
//    numbers, corresponding to the start and end of the cut condition.
//
////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "TaCutInterval.hh"
#include "VaEvent.hh"

#ifndef NODICT
ClassImp(TaCutInterval)
#endif

// Constructors/destructors/operators

TaCutInterval::TaCutInterval ( const Cut_t cu, 
			       const Int_t val, 
			       const EventNumber_t e0, 
			       const EventNumber_t e1 ):
  fCut(cu), 
  fVal(val), 
  fBegin(e0), 
  fEnd(e1) {}


TaCutInterval::TaCutInterval() {}


TaCutInterval::~TaCutInterval() {}


TaCutInterval::TaCutInterval (const TaCutInterval& copy)
{
  fCut = copy.fCut;
  fVal = copy.fVal;
  fBegin = copy.fBegin;
  fEnd = copy.fEnd;
}


TaCutInterval& 
TaCutInterval::operator= (const TaCutInterval& assign)
{
  if (this != &assign)
    {
      fCut = assign.fCut;
      fVal = assign.fVal;
      fBegin = assign.fBegin;
      fEnd = assign.fEnd;
    }
  return *this;
}


// Major functions

Bool_t 
TaCutInterval::Inside (const VaEvent& ev, 
		       const UInt_t lex, 
		       const UInt_t hex) const 
{
  // Return true if the given event lies inside this interval when
  // extended by lex events before and hex events after.

  UInt_t f0 = (fBegin > lex) ? fBegin-lex : 0;
  return ( ( ev.GetEvNumber() >= f0 ) && 
	   ( ev.GetEvNumber() < fEnd+hex ) );
}


// Access functions

void 
TaCutInterval::SetEnd( const EventNumber_t e1 ) 
{ 
  // Set the end event of this interval to the given event number.

  fEnd = e1; 
}


const Cut_t
TaCutInterval::GetCut() const 
{
  // Return the cut type associated with this interval.

  return fCut; 
}


const Int_t 
TaCutInterval::GetVal() const 
{ 
  // Return the cut value associated with this interval.
  return fVal; 
}

// Non member functions

ostream& 
operator<< (ostream& s, const TaCutInterval q)
{
  // Print the given interval on the given output stream.

  s << int (q.fCut) << " " << q.fVal << " " << q.fBegin << " - " << q.fEnd;
  return s;
}

