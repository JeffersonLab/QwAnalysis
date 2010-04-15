/*!
 * \file   QwBlinder.h
 * \brief  A class for blinding data, adapted from G0 blinder class.
 *
 * \author Peiqing Wang
 * \date   2010-04-14
 */

#ifndef __QWBLINDER__
#define __QWBLINDER__

#include <vector>
#include <iostream>
#include <iomanip>
#include "TString.h"
#include "TMD5.h"

#include "QwHelicity.h"
#include "QwDatabase.h"

class QwBlinder{
 public:
  QwBlinder(QwDatabase* sql, UInt_t seed_id, Bool_t enable_blinding = kTRUE);
  ~QwBlinder(){};

  void  WriteFinalValuesToDB();
  void  PrintFinalValues();

 protected:
  // scheme:
  // Acode = Aactual  +  F x sgn(lambda/2), F is an encrypted factor  with |F|< 0.06ppm
  void  BlindMe(Double_t &value){if(fBlindingEnabled){value += fBlindFactor;}};
  void  UnBlindMe(Double_t &value){if(fBlindingEnabled){value -= fBlindFactor;}};

 private:
  QwBlinder() {};                                                    // No deafult constructor.
  QwBlinder (const QwBlinder& tb) {};                                // no use for copy constructor
  const QwBlinder& operator= (const QwBlinder& tb) { return *this; } // no use for assignment

  //  Variables and functions used in blinding the detector asymmetries.
  Double_t fBlindFactor;      /// The factor to be mutliplied to all detector asymmetries.
  Bool_t   fBlindingEnabled;  /// Boolean to enable or disable blinding calculations.

  QwDatabase* fSQL;    /// Store pointer to SQL object
  UInt_t fSeedID;      /// Database ID of seed used to generate this blinding factor (seeds.seed_id)
  TString fSeed;       /// RNG seed string

  vector<UChar_t>  fDigest;     /// Checksum in raw hex
  string           fBFChecksum; /// Checksum in ASCII Hex

  UInt_t   fMaxTests;     /// Maximum number of test values to store
  vector<UInt_t>   fTestNumber;     /// Vector of test numbers (IDs)
  vector<Double_t> fTestValue;      /// Vector of test values
  vector<Double_t> fBlindTestValue; /// Vector of test values, after blinding

  void InitBlinders();                        /// Initializes fBlindFactor from fSeed.
  void SetTestValues(TString &barestring);    /// Initializes the test values, fTestNumber, fTestValue, fBlindTestValue, if fBlindFactor is set.
  Int_t UseMD5(TString &barestring);          /// Returns an integer from a string using MD5
  Int_t UseStringManip(TString &barestring);  /// Returns an integer from a string using a character manipulation algorithm
  Int_t UsePseudorandom(TString &barestring); /// Returns an integer from a string using a version of the helicity bit pseudorandom algorithm.

  Int_t ReadSeed();          ///  Reads fSeed from the fSQL object based on fSeedID
  void WriteChecksum();     ///  Writes fSeedID and fBFChecksum to fSQL for this analysis ID
  void WriteTestValues();   ///  Writes fTestNumber and fBlindTestValue to fSQL for this analysis ID

  Bool_t CheckTestValues(); ///  Recomputes fBlindTestValue to check for memory errors 
  vector<UChar_t> GenerateDigest(const char* input, TString digest_name = "md5");
};

#endif
