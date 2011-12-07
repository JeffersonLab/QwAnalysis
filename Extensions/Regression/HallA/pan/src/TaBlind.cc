//**********************************************************************
//
//     HALL A C++/ROOT Parity Analyzer  Pan           
//
//       TaBlind.hh  (implementation)
//
// Author:  R. Holmes <http://mep1.phy.syr.edu/~rsholmes>, A. Vacheret <http://www.jlab.org/~vacheret>, R. Michaels <http://www.jlab.org/~rom>, K.Paschke
// @(#)pan/src:$Name:  $:$Id: TaBlind.cc,v 1.7 2004/06/18 09:07:22 rsholmes Exp $
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

#include "TaBlind.hh"
#include <string>
#include "TRandom.h"
#include "TaString.hh"

#define PRODUCTION
#ifndef NODICT
ClassImp(TaBlind)
#endif

// Constructors/destructors/operators

  ;
TaBlind::TaBlind (const string& s, const Double_t as, const Double_t am, const Double_t ao):
  fAMean(am),
  fAOffScale(ao)
{
  fASign = (as >= 0 ? 1 : -1);
  Create (s);
}

TaBlind::TaBlind (const char* c, const Double_t as, const Double_t am, const Double_t ao):
  fAMean(am),
  fAOffScale(ao)
{
  // Non-stringy constructor for use in CINT

  fASign = (as >= 0 ? 1 : -1);
  Create (string (c));
}

// Major functions

Double_t 
TaBlind::Blind (Double_t atrue) const    
{
  // blind a true asymmetry
#ifdef SIGNFLIP
  return ((atrue * fASign - fAMean) * fBlindSign  
	  + fBlindOffset * fAOffScale + fAMean) * fASign;
#else
  return atrue + fBlindOffset * fAOffScale * fASign;
#endif
}

Double_t 
TaBlind::UnBlind (Double_t ablind) const
{
  // unblind a blinded asymmetry
#ifdef SIGNFLIP
  return ((ablind * fASign - fAMean - fBlindOffset * fAOffScale) * fBlindSign 
	  + fAMean) * fASign;
#else
  return ablind - fBlindOffset * fAOffScale * fASign;
#endif
}

Double_t 
TaBlind::BlindSignOnly (Double_t atrue) const 
{
  // blind or unblind with no offset (used for differences and beam asymmetry)
#ifdef SIGNFLIP
  return atrue * fBlindSign;
#else
  return atrue;
#endif
}

// Private member functions

void
TaBlind::Create (const string s) 
{
  // Constructor utility

  // String s is hashed to create a random number seed, and then
  // BlindSign and BlindOffset are set up.  No blinding if s is "" or
  // "none" (or case variants thereof).

  if (s == "" || TaString(s).ToLower() == "none")
    {
      clog << "TaBlind::Create: No blinding" << endl;
      fBlindSign = 1;
      fBlindOffset = 0;
    }
  else
    {
      // The hash: start with a "random" integer, rotate last 31 bits
      // 1 bit to the left, XOR a character, mask off 31 bits, repeat.

      // This is not claimed to have any great theoretical properties,
      // just that it's hard to predict what you get.

      UInt_t res (2696372);
      const char* p = s.c_str();
      while (*p) 
	res = (((res<<1) + (res>>30)) ^ *p++) & 0x7FFFFFFF;
      clog << "TaBlind::Create: '" << s << "' hashes to " << res << endl;
      TRandom r(res);
      //      fBlindOffset = r.Uniform(-1.,1.);
      fBlindOffset = -1 + 2*r.Uniform(1.);
#ifdef SIGNFLIP
      fBlindSign = 2*((Double_t)(r.Integer(2)))-1;
#else
      fBlindSign = 1;
#endif

#ifndef PRODUCTION
      // Printout only for debugging... obviously we remove this for
      // production...
      clog << "TaBlind::Create: Blind sign and offset are " << fBlindSign 
	   << " " << fBlindOffset << endl;
      clog << "TaBlind::Create: Input sign, mean, and offscale are "
	   << fASign
	   << " " << fAMean
	   << " " << fAOffScale << endl;
#endif
    }
}

// Non member functions

