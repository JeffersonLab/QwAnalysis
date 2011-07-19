//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//        TaLabelledQuantity.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaLabelledQuantity.hh,v 1.8 2003/07/31 16:11:59 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//    A class of quantities with associated labels, units, and flags.
//
////////////////////////////////////////////////////////////////////////

#ifndef PAN_TaLabelledQuantity
#define PAN_TaLabelledQuantity

#include "TObject.h"
#include <string>
#include <iostream>

using namespace std;

class TaLabelledQuantity
{

public:

  // Constructors/destructors/operators
  TaLabelledQuantity (const string& qn = "", 
		      const Double_t qv = 0.0, 
		      const string& qu = "",
		      const UInt_t qf = 0): 
    fName(qn), fVal(qv), fUnits(qu), fFlags(qf) {};
  TaLabelledQuantity (const TaLabelledQuantity& qty):
    fName(qty.fName), fVal(qty.fVal), fUnits(qty.fUnits), fFlags(qty.fFlags) {};
  virtual ~TaLabelledQuantity() {}
  TaLabelledQuantity& operator=(const TaLabelledQuantity& qty);

  // Major functions

  // Data access functions
  void SetVal (const Double_t qv) { fVal = qv; }
  const string& GetName() const { return fName; }
  Double_t GetVal() const { return fVal;}
  const string& GetUnits() const { return fUnits; }
  Bool_t TestFlags(UInt_t mask) const { return fFlags & mask; }

private:

  // Data members
  string fName;   // Name of the quantity
  Double_t fVal;  // Value of the quantity
  string fUnits;  // Units for the quantity
  UInt_t fFlags;  // Flags for the quantity

#ifndef NODICT
  ClassDef(TaLabelledQuantity, 0)  // Quantity with label and units
#endif

};

// Non member functions

ostream& operator<< (ostream& s, const TaLabelledQuantity q);

#endif
