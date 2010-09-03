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

// System headers
#include <iostream>
#include <vector>
#include <numeric>

// ROOT headers
#include <TTree.h>

// Boost math library for random number generation
#include <boost/random.hpp>

// Qweak headers
#include "VQwDataElement.h"

/// At this point the samples are hard-coded to be of Float_t data type.
/// Ideally, this should be templated out, so that the channel can have
/// UInt_t raw samples, Float_t processed samples, and Double_t average
/// samples.  Seems to cause too many problems right now and needs some
/// thinking.
typedef Double_t MQwSIS3320_Type;

class MQwSIS3320_Samples: public VQwDataElement {

  public:

    MQwSIS3320_Samples(UInt_t nsamples = 256) {
      SetSamplesPerWord(2);
      SetNumberOfSamples(nsamples);
    };
    ~MQwSIS3320_Samples() { };

    const MQwSIS3320_Type GetSum() const;
    const MQwSIS3320_Type GetSample(size_t i) const { return fSamples.at(i); };
    const MQwSIS3320_Type GetSumInTimeWindow(const UInt_t start, const UInt_t stop) const;

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

    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void  FillTreeVector(std::vector<Double_t> &values) const;

    // Output stream operator<< for an accumulator
    friend std::ostream& operator<< (std::ostream& stream, const MQwSIS3320_Samples& s);

    // Define operator+=, operator-=, etc, for accumulator asymmetries

  protected:

    UInt_t fSamplesPerWord; //! Number of 12-bit sample values per data word
    std::vector<MQwSIS3320_Type> fSamples; //! Samples data buffer

  private:

    // Ntuple array indices
    size_t fTreeArrayIndex; //! Index of this data element in tree
    size_t fTreeArrayNumEntries; //! Number of entries from this data element

};

// Output stream operator<< for the samples
inline std::ostream& operator<< (std::ostream& stream, const MQwSIS3320_Samples& s)
{
  for (size_t i = 0; i < s.GetNumberOfSamples(); i++)
    stream << s.GetSample(i) << " ";
  return stream;
};

#endif // __MQwSIS3320_Samples__
