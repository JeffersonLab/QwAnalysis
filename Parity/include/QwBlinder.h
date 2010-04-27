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
#include "QwSubsystemArrayParity.h"

//  Backup type definition for ULong64_t; needed with some older ROOT versions.
#if !defined(ULong64_t)
#if defined(R__WIN32) && !defined(__CINT__)
typedef unsigned __int64   ULong64_t; //Portable unsigned long integer 8 bytes
#else
typedef unsigned long long ULong64_t; //Portable unsigned long integer 8 bytes
#endif
#endif


class QwBlinder{
 public:
  QwBlinder(QwDatabase* sql, UInt_t seed_id, Bool_t enable_blinding = kTRUE);
  ~QwBlinder(){};

  void  WriteFinalValuesToDB();
  void  PrintFinalValues();

  // scheme:
  // Acode = Aactual  + fBlindFactor
  // where fBlindFactor = F x sgn(lambda/2), F is an encrypted factor  with |F|< 0.06ppm
  // this offset fBlindFactor will be applied on the block and blocksum of the asymmetry
  //
  // For blinding the helicity correlated differences of the detectors, we'd have to do:
  // D_blinded = (A_raw + fBlindFactor) * Y_raw
  // where A_raw = D_raw/Y_raw is the unblinded asymmetry 

  void  BlindMe(Double_t &value){  if(fBlindingEnabled) value += fBlindFactor; };
  void  UnBlindMe(Double_t &value, TString type){  if(fBlindingEnabled) value -= fBlindFactor; };


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

  vector<UChar_t>  fDigest;         /// Checksum in raw hex
  string           fBFChecksum;     /// Checksum in ASCII Hex

  UInt_t   fMaxTests;               /// Maximum number of test values to store
  vector<UInt_t>   fTestNumber;     /// Vector of test numbers (IDs)
  vector<Double_t> fTestValue;      /// Vector of test values
  vector<Double_t> fBlindTestValue; /// Vector of test values, after blinding

  void InitBlinders();                        /// Initializes fBlindFactor from fSeed.
  void SetTestValues(TString &barestring);    /// Initializes the test values, fTestNumber, fTestValue,
                                              /// fBlindTestValue, if fBlindFactor is set.
  Int_t UseMD5(TString &barestring);          /// Returns an integer from a string using MD5
  Int_t UseStringManip(TString &barestring);  /// Returns an integer from a string using
                                              /// a character manipulation algorithm
  Int_t UsePseudorandom(TString &barestring); /// Returns an integer from a string using
                                              /// a version of the helicity bit pseudorandom algorithm.

  Int_t ReadSeed();          ///  Reads fSeed from the fSQL object based on fSeedID
  void WriteChecksum();     ///  Writes fSeedID and fBFChecksum to fSQL for this analysis ID
  void WriteTestValues();   ///  Writes fTestNumber and fBlindTestValue to fSQL for this analysis ID
  Bool_t CheckTestValues(); ///  Recomputes fBlindTestValue to check for memory errors 

  vector<UChar_t> GenerateDigest(const char* input);
};

#endif
