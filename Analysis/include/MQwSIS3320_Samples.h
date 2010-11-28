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

// ROOT headers
#include <TObject.h>
#include <TGraph.h>
#include <TTree.h>

// Qweak headers
#include "VQwDataElement.h"

/// At this point the samples are hard-coded to be of Float_t data type.
/// Ideally, this should be templated out, so that the channel can have
/// UInt_t raw samples, Float_t processed samples, and Double_t average
/// samples.  Seems to cause too many problems right now and needs some
/// thinking.
typedef Double_t MQwSIS3320_Type;

class MQwSIS3320_Samples: public TObject, public VQwDataElement {

  public:

    MQwSIS3320_Samples(UInt_t nsamples = 256) {
      fGraph = 0;
      SetSamplesPerWord(2);
      SetNumberOfSamples(nsamples);
    };
    virtual ~MQwSIS3320_Samples() {
      if (fGraph) delete fGraph;
    };

    size_t GetMinIndex() const { return GetMin().first; };
    size_t GetMaxIndex() const { return GetMax().first; };
    MQwSIS3320_Type GetMinSample() const { return GetMin().second; };
    MQwSIS3320_Type GetMaxSample() const { return GetMax().second; };

    const MQwSIS3320_Type GetSum() const;
    const MQwSIS3320_Type GetSample(size_t i) const { return fSamples.at(i); };
    const MQwSIS3320_Type GetPedestal() const { return GetSample(0); };
    const MQwSIS3320_Type GetSumInTimeWindow(const UInt_t start, const UInt_t stop) const;

    const UInt_t GetNumberOfSamples() const { return fSamples.size(); };
    void SetNumberOfSamples(const UInt_t nsamples) {
      // Initialize index vector
      fIndex.resize(nsamples);
      for (size_t i = 0; i < fIndex.size(); i++) fIndex[i] = i;
      // Initialize sample vector
      fSamples.resize(nsamples);
      SetNumberOfDataWords(GetNumberOfSamples() / GetSamplesPerWord());
    };

    const UInt_t GetSamplesPerWord() const { return fSamplesPerWord; };
    void SetSamplesPerWord(const UInt_t nsamples) {
      fSamplesPerWord = nsamples;
      SetNumberOfDataWords(GetNumberOfSamples() / GetSamplesPerWord());
    };

    const TGraph* GetGraph() const { return fGraph; };


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

    // Update the graph from the index and value vectors
    void UpdateGraph();

    // Output stream operator<< for an accumulator
    friend std::ostream& operator<< (std::ostream& stream, const MQwSIS3320_Samples& s);

  private:

    // Private helper methods for getting minimum and maximum index and samples
    std::pair<size_t,MQwSIS3320_Type> GetMin() const; //!
    std::pair<size_t,MQwSIS3320_Type> GetMax() const; //!

  private:

    //! Number of 12-bit sample values per data word
    UInt_t fSamplesPerWord;
    //! Samples index
    static std::vector<MQwSIS3320_Type> fIndex; //!
    //! Samples values
    std::vector<MQwSIS3320_Type> fSamples;
    //! Graph of samples
    TGraph* fGraph;

    // Ntuple array indices
    size_t fTreeArrayIndex; //!< Index of this data element in tree
    size_t fTreeArrayNumEntries; //!< Number of entries from this data element

  ClassDef(MQwSIS3320_Samples,1);
};

// Output stream operator<< for the samples
inline std::ostream& operator<< (std::ostream& stream, const MQwSIS3320_Samples& s)
{
  for (size_t i = 0; i < s.GetNumberOfSamples(); i++)
    stream << s.GetSample(i) << " ";
  return stream;
};

#endif // __MQwSIS3320_Samples__
