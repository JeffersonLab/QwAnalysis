/**
 * \class	MQwSIS3320_Accumulator	MQwSIS3320_Accumulator.h
 *
 * \brief	SIS3320 sampling ADC accumulator
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The MQwSIS3320_Accumulator should allow convenient access to the accumulator
 * data collected with the SIS3320 for the Compton photon detector.  This class
 * implements its own sum, difference, and ratio methods inherited from the
 * general VQwDataElement.
 *
 */

#ifndef __MQwSIS3320_Accumulator__
#define __MQwSIS3320_Accumulator__

#include <iostream>
#include <vector>
#include <numeric>
#include "TTree.h"

// Boost math library for random number generation
#include "boost/random.hpp"

#include "VQwDataElement.h"

class MQwSIS3320_Accumulator: public VQwDataElement {

  public:

    MQwSIS3320_Accumulator() {
      SetNumberOfDataWords(3);
      fMinValue = 0; fMaxValue = 0;
      fMinTime = 0; fMaxTime = 0;
    };
    ~MQwSIS3320_Accumulator() { };

    const Int_t GetMinValue() const { return fMinValue; };
    const Int_t GetMaxValue() const { return fMaxValue; };
    const Int_t GetMinTime() const { return fMinTime; };
    const Int_t GetMaxTime() const { return fMaxTime; };

    const Int_t GetSum() const { return fAccumulatorSum; };
    const Int_t GetNumberOfSamples() const { return fNumberOfSamples; };

    void  ClearEventData() { fAccumulatorSum = 0; fNumberOfSamples = 0; };
    Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement = 0);

    const MQwSIS3320_Accumulator operator/ (const Double_t &value) const;
    const MQwSIS3320_Accumulator operator* (const Double_t &value) const;
    const MQwSIS3320_Accumulator operator+ (const Double_t &value) const;
    const MQwSIS3320_Accumulator operator- (const Double_t &value) const;
    MQwSIS3320_Accumulator& operator/= (const Double_t &value);
    MQwSIS3320_Accumulator& operator*= (const Double_t &value);
    MQwSIS3320_Accumulator& operator+= (const Double_t &value);
    MQwSIS3320_Accumulator& operator-= (const Double_t &value);
    const MQwSIS3320_Accumulator operator+ (const MQwSIS3320_Accumulator &value) const;
    const MQwSIS3320_Accumulator operator- (const MQwSIS3320_Accumulator &value) const;
    MQwSIS3320_Accumulator& operator=  (const MQwSIS3320_Accumulator &value);
    MQwSIS3320_Accumulator& operator+= (const MQwSIS3320_Accumulator &value);
    MQwSIS3320_Accumulator& operator-= (const MQwSIS3320_Accumulator &value);
    void Sum(const MQwSIS3320_Accumulator &value1, const MQwSIS3320_Accumulator &value2);
    void Difference(const MQwSIS3320_Accumulator &value1, const MQwSIS3320_Accumulator &value2);
    void Ratio(const MQwSIS3320_Accumulator &numer, const MQwSIS3320_Accumulator &denom);

    void  ConstructHistograms(TDirectory *folder, TString &prefix) { };
    void  FillHistograms() { };

    // Output stream operator<< for an accumulator
    friend std::ostream& operator<< (std::ostream& stream, const MQwSIS3320_Accumulator& a);

    // Define operator+=, operator-=, etc, for accumulator asymmetries

  protected:

    Long_t fAccumulatorSum; //! Accumulator sum
    Int_t fNumberOfSamples; //! Number of accumulated samples

  private:

    int fMinValue, fMaxValue; //! Value-based accumulator limits
    int fMinTime, fMaxTime; //! Time-based accumulator limits

    static const unsigned int INDEX_NUM;
    static const unsigned int INDEX_SUM;

};

// Output stream operator<< for the accumulators
inline std::ostream& operator<< (std::ostream& stream, const MQwSIS3320_Accumulator& a) {
  stream << a.GetSum() << " (" << a.GetNumberOfSamples() << ")";
  return stream;
};

#endif // __MQwSIS3320_Accumulator__
