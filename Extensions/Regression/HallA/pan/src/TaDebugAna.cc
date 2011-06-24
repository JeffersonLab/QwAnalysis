//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//           TaDebugAna.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaDebugAna.cc,v 1.4 2003/07/31 16:11:59 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    Debug analysis.  This class derives from TaStandardAna.  No ROOT
//    file is created, and some basic information for each event is
//    printed.  It also prints statistics on differences and
//    asymmetries of beam monitors and detectors periodically.
//
////////////////////////////////////////////////////////////////////////

#include "TaDebugAna.hh"
#include "VaEvent.hh"
#include "TaRun.hh"
#include "TaLabelledQuantity.hh"
#include "TaDevice.hh"

#ifndef NODICT
ClassImp(TaDebugAna)
#endif

// Constructors/destructors/operators

TaDebugAna::TaDebugAna(): TaStandardAna()
{
  fDoRoot = false;
}


// Private member functions

void TaDebugAna::EventAnalysis()
{
  // Event analysis.

  TaStandardAna::EventAnalysis();
  fEvt->MiniDump();
}

