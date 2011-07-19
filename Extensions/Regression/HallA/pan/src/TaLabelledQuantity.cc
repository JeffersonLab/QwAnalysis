//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//        TaLabelledQuantity.cc  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaLabelledQuantity.cc,v 1.5 2003/07/31 16:11:59 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    A class of quantities with associated labels, units, and flags.
//
////////////////////////////////////////////////////////////////////////

#include "TaLabelledQuantity.hh"

#ifndef NODICT
ClassImp(TaLabelledQuantity)
#endif

// Constructors/destructors/operators

TaLabelledQuantity& TaLabelledQuantity::operator=(const TaLabelledQuantity& qty)
{
  if ( &qty != this )
    {
      fName = qty.fName;
      fVal = qty.fVal;
      fUnits = qty.fUnits;
      fFlags = qty.fFlags;
    }
  return *this;
}

// Major functions

// Data access functions

// Non member functions

ostream& operator<< (ostream& s, const TaLabelledQuantity q)
{

  // Print a labelled quantity to the given output stream as
  // "name: value units".  (Flags are not printed.)

  return s << q.GetName() << ": " << q.GetVal() << " " << q.GetUnits();
}

