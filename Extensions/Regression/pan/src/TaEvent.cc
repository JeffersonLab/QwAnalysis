//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaEvent.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaEvent.cc,v 1.38 2003/07/31 16:11:59 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    An event of pure data, un-spoilt by the vagaries of simulation. Inherits from VaEvent.
//
////////////////////////////////////////////////////////////////////////


#include "TaEvent.hh"
#include "TaCutList.hh"
#include "TaLabelledQuantity.hh"
#include "TaRun.hh"
#include "TaString.hh"
#include "TaDevice.hh"
#include "TaDataBase.hh"
#include <iostream>
#include <iomanip>
#include <utility>
#include <cmath>

#ifndef NODICT
ClassImp(TaEvent)
#endif

// Static members

TaEvent::TaEvent(): VaEvent()
{
}

TaEvent::~TaEvent() {
}

TaEvent::TaEvent(const TaEvent& ev) : VaEvent(ev)
{
} 

TaEvent &TaEvent::operator=(const TaEvent &ev)
{
  if ( &ev != this )
    {
      VaEvent::Uncreate();
      VaEvent::Create(ev);
    }
  return *this;
}

TaEvent& 
TaEvent::CopyInPlace (const TaEvent& rhs)
{
  // Like operator=, but copy an event into existing memory, not
  // deleting and reallocating.  This should always be safe, but just
  // to minimize possible problems use this instead of operator= only
  // when pointers must be preserved.
  VaEvent::CopyInPlace(rhs);
  return *this;
}


// Major functions

ErrCode_t
TaEvent::RunInit(const TaRun& run)
{
  // Initialization at start of run.  Get quantities from database
  // which will be needed in event loop, and set other static variables
  // to initial values.
  return VaEvent::RunInit(run);
}
  

void TaEvent::Decode(TaDevice& devices) {
// Decodes all the raw data and applies all the calibrations and BPM
// rotations..  Note: This assumes the event structure remains
// constant.  We check this by verifying a constant event length.
// Also note that in order for cooked data to appear in the output,
// everywhere we have fData[cook_key] = function of fData[raw_key], 
// we MUST have a line devices.SetUsed(cook_key) if we want it.

  VaEvent::Decode(devices);
}



