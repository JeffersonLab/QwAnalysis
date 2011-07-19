#ifndef PAN_TaBlind
#define PAN_TaBlind

//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaBlind.hh  (interface)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaBlind.hh,v 1.5 2004/06/18 09:07:22 rsholmes Exp $
//
////////////////////////////////////////////////////////////////////////
//
//  Methods to facilitate blind analysis.  Inspired by BaBar/E158.
//
//  A string is used to provide an initial seed for a random number
//  generator.  Asymmetries are hidden with
//
//    A_hidden = ((A_true * Asign - AMean) * BlindSign 
//               + BlindOffset * AOffScale + AMean) * Asign
//
//  where
//
//    Asign is the slow helicity reversal sign.
//    AMean is a supplied constant, typically the expected mean when
//       Asign is 1.
//    AOffScale is a supplied constant, typically the expected 
//      error
//    BlindSign is supplied by the blinding algorithm and is -1 or +1
//    BlindOffset is supplied by the blinding algorithm and is in range 
//      -1 to +1
//
//  (Note that E158 fixes BlindSign = 1, in which case the value of
//  AMean is immaterial and the hidden asymmetry is simply
//
//    A_hidden = A_true + BlindOffset * AOffScale * Asign
//
//  We might do this too, and in fact I don't know why one would want
//  a random BlindSign at all, but this class is written to allow it.)
//
//  (Also note that E158 does not include Asign in the blinding
//  methods; instead it is assumed that A_true has the "correct" sign,
//  as will A_hidden.)
//
//  This is done by Blind and undone by UnBlind.
//
//  To keep the sign of the asymmetry correlations with beam
//  parameters, beam differences should get multiplied by BlindSign:
//
//    D_blind = D_true * BlindSign
//
//  This is done by BlindSignOnly.
//
////////////////////////////////////////////////////////////////////////

#include "Rtypes.h"
#include <string>
#include <vector>
#include <iostream>

//#define SIGNFLIP            // comment out to fix sign at +1

using namespace std;

class TaBlind : public string 
{

public:

  // Constructors/destructors/operators

  TaBlind () {};
  TaBlind (const string& s, const Double_t as, const Double_t am, const Double_t ao);
  TaBlind (const char* c, const Double_t as, const Double_t am, const Double_t ao);
  virtual ~TaBlind() {};

  // Major functions

  Double_t Blind (Double_t atrue) const;     // blind a true asymmetry
  Double_t UnBlind (Double_t ablind) const;  // unblind a blinded asymmetry
  Double_t BlindSignOnly (Double_t atrue) const; // blind or unblind with no offset
  Bool_t Blinding () const {return !(fBlindSign == 1 && fBlindOffset == 0);} // is blinding on?

private:
  
  // Private member functions
  TaBlind (const TaBlind& tb) {}; // no use for copy constructor
  const TaBlind& operator= (const TaBlind& tb) { return *this; } // no use for assign

  void Create (const string s); // Constructor utility

  // Data members
  Double_t fASign;       // slow helicity reversal sign
  Double_t fAMean;       // value around which we blind
  Double_t fAOffScale;   // scale of offset
  Double_t fBlindSign;   // secret sign
  Double_t fBlindOffset; // secret offset

#ifndef NODICT
  ClassDef(TaBlind, 0)   // Result blinding tools
#endif

};

#endif
