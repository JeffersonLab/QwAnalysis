/*!
 * \file   QwBlinder.h
 * \brief  A class for blinding data, adapted from G0 blinder class.
 *
 * \author Peiqing Wang
 * \date   2010-04-14
 */

#ifndef __QWBLINDER__
#define __QWBLINDER__

// System headers
#include <vector>
#include <iostream>
#include <iomanip>

// ROOT headers
#include <TString.h>
#include <TMD5.h>

// Qweak headers
#include "QwDatabase.h"
#include "QwSubsystemArrayParity.h"

// Backup type definition for ULong64_t; needed with some older ROOT versions.
#if !defined(ULong64_t)
#if defined(R__WIN32) && !defined(__CINT__)
typedef unsigned __int64   ULong64_t; // Portable unsigned long integer 8 bytes
#else
typedef unsigned long long ULong64_t; // Portable unsigned long integer 8 bytes
#endif
#endif


/**
 * \class QwBlinder
 * \ingroup QwAnalysis
 *
 * \brief Class for blinding data, adapted from G0 blinder class
 *
 * 1. Asymmetry blinding scheme:
 * \f[
 *   Asym_{blinded} = (Asym_{actual} + fBlindingOffset) * fBlindingFactor
 * \f]
 * where \f$ fBlindingOffset = F \times sign(\lambda/2) \f$, F is an encrypted factor
 * with |F| < 0.06 ppm.
 * This offset fBlindingOffset will be applied on the block and blocksum of the asymmetry.
 *
 * 2. Difference blinding scheme:
 * For blinding the helicity correlated differences of the detectors, we'd have to do:
 * \f[
 *   Diff_{blinded} = (Diff_{raw} + Yield_{raw} * fBlindingOffset) * fBlindingFactor)
 * \f]
 * where \f$ Asym_{raw} = Diff_{raw} / Yield_{raw} \f$ is the unblinded asymmetry,
 * and \f$ Asym_{blinded} = Diff_{blinded} / Yield_{blinded} \f$ the blinded asymmetry.
 * Blinding the differences allows that the difference can be written to the output
 * ROOT files without compromising the blinding.
 *
 */
class QwBlinder {

  public:

    /// Available blinding strategies
    enum EQwBlindingStrategy {
      kDisabled,
      kAdditive,
      kMultiplicative,
      kAdditiveMultiplicative
    };

    /// \brief Constructor with default seed string and without database
    QwBlinder(const TString& seed, const EQwBlindingStrategy blinding_strategy = kAdditive);
    /// \brief Constructor with database
    QwBlinder(QwDatabase* sql, const UInt_t seed_id, const EQwBlindingStrategy blinding_strategy = kAdditive);
    /// \brief Default destructor
    virtual ~QwBlinder();

    void  WriteFinalValuesToDB();
    void  PrintFinalValues();
    void FillDB(QwDatabase *db, TString datatype);

    /// Asymmetry blinding
    void  BlindValue(Double_t& value) const {
      switch (fBlindingStrategy) {
        case kAdditive:
          value += fBlindingOffset; break;
        case kMultiplicative:
          value *= fBlindingFactor; break;
        case kAdditiveMultiplicative:
          value = (value + fBlindingOffset) * fBlindingFactor; break;
        default: break;
      }
    };
    /// Asymmetry unblinding
    void  UnBlindValue(Double_t& value) const {
      switch (fBlindingStrategy) {
        case kAdditive:
          value -= fBlindingOffset; break;
        case kMultiplicative:
          value /= fBlindingFactor; break;
        case kAdditiveMultiplicative:
          value = value / fBlindingFactor - fBlindingOffset; break;
        default: break;
      }
    };

    /// Difference blinding
    void  BlindValue(Double_t& value, const Double_t& yield) const {
      switch (fBlindingStrategy) {
        case kAdditive:
          value += yield * fBlindingOffset; break;
        case kMultiplicative:
          value *= fBlindingFactor; break;
        case kAdditiveMultiplicative:
          value = (value + fBlindingOffset * yield) * fBlindingFactor; break;
        default: break;
      }
    };
    /// Difference unblinding
    void  UnBlindValue(Double_t& value, const Double_t& yield) const {
      switch (fBlindingStrategy) {
        case kAdditive:
          value -= yield * fBlindingOffset; break;
        case kMultiplicative:
          value /= fBlindingFactor; break;
        case kAdditiveMultiplicative:
          value = value / fBlindingFactor - yield * fBlindingOffset; break;
        default: break;
      }
    };

    /// Blind the asymmetry of an array of subsystems
    void  Blind(QwSubsystemArrayParity& diff) {
      diff.Blind(this);
    };
    /// Unblind the asymmetry of an array of subsystems
    void  UnBlind(QwSubsystemArrayParity& diff) {
      diff.UnBlind(this);
    };

    /// Blind the difference of an array of subsystems
    void  Blind(QwSubsystemArrayParity& diff, const QwSubsystemArrayParity& yield) {
      diff.Blind(this, yield);
    };
    /// Unblind the difference of an array of subsystems
    void  UnBlind(QwSubsystemArrayParity& diff, const QwSubsystemArrayParity& yield) {
      diff.UnBlind(this, yield);
    };

  private:
    /// Private default constructor
    QwBlinder () { };
    /// Private copy constructor
    QwBlinder (const QwBlinder& tb) { };
    /// Private assignment operator
    const QwBlinder& operator= (const QwBlinder& tb) { return *this; };

    //  Variables and functions used in blinding the detector asymmetries
    EQwBlindingStrategy fBlindingStrategy; /// Blinding strategy
    Double_t fBlindingOffset; /// The term to be added to detector asymmetries
    Double_t fBlindingFactor; /// The factor to be mutliplied to detector asymmetries

    static const Double_t kMaximumBlindingAsymmetry; /// Maximum blinding asymmetry (in ppm)
    static const Double_t kMaximumBlindingFactor;    /// Maximum blinding factor (in % from identity)

    QwDatabase* fSQL;    /// Store pointer to SQL object
    UInt_t fSeedID;      /// Database ID of seed used to generate this blinding factor (seeds.seed_id)
    TString fSeed;       /// RNG seed string

    vector<UChar_t>  fDigest;         /// Checksum in raw hex
    string           fChecksum;       /// Checksum in ASCII hex

    UInt_t fMaxTests;                   /// Maximum number of test values
    vector<UInt_t>   fTestNumber;       /// Vector of test numbers (IDs)
    vector<Double_t> fTestValue;        /// Vector of test values, original
    vector<Double_t> fBlindTestValue;   /// Vector of test values, after blinding
    vector<Double_t> fUnBlindTestValue; /// Vector of test values, after unblinding

    void InitBlinders();                              /// Initializes fBlindingFactor from fSeed.
    void SetTestValues(const TString &barestring);    /// Initializes the test values, fTestNumber, fTestValue,
                                                      /// fBlindTestValue, if fBlindingFactor is set.
    Int_t UseMD5(const TString &barestring);          /// Returns an integer from a string using MD5
    Int_t UseStringManip(const TString &barestring);  /// Returns an integer from a string using
                                                      /// a character manipulation algorithm
    Int_t UsePseudorandom(const TString &barestring); /// Returns an integer from a string using
                                                      /// a version of the helicity bit pseudorandom algorithm.

    Int_t ReadSeed();         ///  Reads fSeed from the fSQL object based on fSeedID
    void WriteChecksum();     ///  Writes fSeedID and fBFChecksum to fSQL for this analysis ID
    void WriteTestValues();   ///  Writes fTestNumber and fBlindTestValue to fSQL for this analysis ID
    Bool_t CheckTestValues(); ///  Recomputes fBlindTestValue to check for memory errors

    vector<UChar_t> GenerateDigest(const char* input);
};

#endif
