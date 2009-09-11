/**
 * \class	MQwSIS3320_Samples	MQwSIS3320_Samples.h
 *
 * \brief	SIS3320 sampling ADC samples
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The MQwSIS3320_Samples should allow convenient access to the sampling data
 * collected with the SIS3320 for the Compton photon detector.  This class
 * implements its own sum, difference, and ratio methods inherited from the
 * general VQwDataElement.
 *
 */

#ifndef __MQwSIS3320_Samples__
#define __MQwSIS3320_Samples__

#include <iostream>
#include <vector>
#include <numeric>
#include "TTree.h"

// Boost math library for random number generation
#include "boost/random.hpp"

#include "VQwDataElement.h"

class MQwSIS3320_Samples: public VQwDataElement {

  public:

    MQwSIS3320_Samples(UInt_t nsamples = 256) {
      SetSamplesPerWord(2);
      SetNumberOfSamples(nsamples);
    };
    ~MQwSIS3320_Samples() { };

    const Int_t GetSum() const { return std::accumulate(fSamples.begin(), fSamples.end(), 0); };
    const Int_t GetSample(size_t i) const { return fSamples.at(i); };

    const UInt_t GetNumberOfSamples() const { return fSamples.size(); };
    void SetNumberOfSamples(const UInt_t nsamples) {
      fSamples.resize(nsamples);
      SetNumberOfDataWords(GetNumberOfSamples() / GetSamplesPerWord());
    };

    const UInt_t GetSamplesPerWord() const { return fSamplesPerWord; };
    void SetSamplesPerWord(const UInt_t nsamples) {
      fSamplesPerWord = nsamples;
      SetNumberOfDataWords(GetNumberOfSamples() / GetSamplesPerWord());
    };

    void  ClearEventData() { fSamples.clear(); };
    Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement = 0);

    const MQwSIS3320_Samples operator/ (const Double_t &value) const;
    const MQwSIS3320_Samples operator* (const Double_t &value) const;
    const MQwSIS3320_Samples operator+ (const Double_t &value) const;
    const MQwSIS3320_Samples operator- (const Double_t &value) const;
    MQwSIS3320_Samples& operator/= (const Double_t &value);
    MQwSIS3320_Samples& operator*= (const Double_t &value);
    MQwSIS3320_Samples& operator+= (const Double_t &value);
    MQwSIS3320_Samples& operator-= (const Double_t &value);
    const MQwSIS3320_Samples operator+ (const MQwSIS3320_Samples &value) const;
    const MQwSIS3320_Samples operator- (const MQwSIS3320_Samples &value) const;
    MQwSIS3320_Samples& operator=  (const MQwSIS3320_Samples &value);
    MQwSIS3320_Samples& operator+= (const MQwSIS3320_Samples &value);
    MQwSIS3320_Samples& operator-= (const MQwSIS3320_Samples &value);
    void Sum(const MQwSIS3320_Samples &value1, const MQwSIS3320_Samples &value2);
    void Difference(const MQwSIS3320_Samples &value1, const MQwSIS3320_Samples &value2);
    void Ratio(const MQwSIS3320_Samples &numer, const MQwSIS3320_Samples &denom);

    void  ConstructHistograms(TDirectory *folder, TString &prefix) { };
    void  FillHistograms() { };

    // Output stream operator<< for an accumulator
    friend std::ostream& operator<< (std::ostream& stream, const MQwSIS3320_Samples& s);

    // Define operator+=, operator-=, etc, for accumulator asymmetries

  protected:

    UInt_t fSamplesPerWord; //! Number of 12-bit sample values per data word
    std::vector<Int_t> fSamples; //! Samples data buffer (allow for negative values)

  private:

};

// Output stream operator<< for the samples
inline std::ostream& operator<< (std::ostream& stream, const MQwSIS3320_Samples& s) {
  for (size_t i = 0; i < s.GetNumberOfSamples(); i++)
    stream << s.GetSample(i) << " ";
  return stream;
};

#endif // __MQwSIS3320_Samples__
