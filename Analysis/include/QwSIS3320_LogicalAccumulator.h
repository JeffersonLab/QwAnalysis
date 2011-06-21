/**
 * \class	QwSIS3320_LogicalAccumulator	QwSIS3320_LogicalAccumulator.h
 *
 * \brief	SIS3320 sampling ADC accumulator
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The QwSIS3320_LogicalAccumulator should allow convenient access to the accumulator
 * data collected with the SIS3320 for the Compton photon detector.  This class
 * implements its own sum, difference, and ratio methods inherited from the
 * general VQwDataElement.
 *
 */

#ifndef __QwSIS3320_LogicalAccumulator__
#define __QwSIS3320_LogicalAccumulator__

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
//#include "QwSIS3320_Accumulator.h"

class QwSIS3320_Accumulator;

class QwSIS3320_LogicalAccumulator: public VQwDataElement {
public:

  QwSIS3320_LogicalAccumulator(TString name = "") {
    SetElementName(name);
  };
  ~QwSIS3320_LogicalAccumulator() { };

  Double_t GetNumberOfSamples() const { return fNumberOfSamples; };
  Double_t GetAccumulatorSum() const { return fAccumulatorSum; };
  Double_t GetAccumulatorAvg() const {
    if (fAccumulatorSum > 0)
      return fAccumulatorSum / fNumberOfSamples;
    else return 0.0;
  };

  void  ProcessEvent();
  void  ClearEventData() { fAccumulatorSum = 0; fNumberOfSamples = 0; };
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement = 0) { return 0; };


  const QwSIS3320_LogicalAccumulator operator/ (const Double_t &value) const;
  const QwSIS3320_LogicalAccumulator operator* (const Double_t &value) const;
  const QwSIS3320_LogicalAccumulator operator+ (const Double_t &value) const;
  const QwSIS3320_LogicalAccumulator operator- (const Double_t &value) const;
  QwSIS3320_LogicalAccumulator& operator/= (const Double_t &value);
  QwSIS3320_LogicalAccumulator& operator*= (const Double_t &value);
  QwSIS3320_LogicalAccumulator& operator+= (const Double_t &value);
  QwSIS3320_LogicalAccumulator& operator-= (const Double_t &value);
  const QwSIS3320_LogicalAccumulator operator+ (const QwSIS3320_LogicalAccumulator &value) const;
  const QwSIS3320_LogicalAccumulator operator- (const QwSIS3320_LogicalAccumulator &value) const;
  QwSIS3320_LogicalAccumulator& operator=  (const QwSIS3320_LogicalAccumulator &value);
  QwSIS3320_LogicalAccumulator& operator+= (const QwSIS3320_LogicalAccumulator &value);
  QwSIS3320_LogicalAccumulator& operator-= (const QwSIS3320_LogicalAccumulator &value);
  void Sum(const QwSIS3320_LogicalAccumulator &value1, const QwSIS3320_LogicalAccumulator &value2);
  void Difference(const QwSIS3320_LogicalAccumulator &value1, const QwSIS3320_LogicalAccumulator &value2);
  void Ratio(const QwSIS3320_LogicalAccumulator &numer, const QwSIS3320_LogicalAccumulator &denom);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  // Output stream operator<< for an accumulator
  friend std::ostream& operator<< (std::ostream& stream, const QwSIS3320_LogicalAccumulator& a);

  // Define operator+=, operator-=, etc, for accumulator asymmetries

  void AddAccumulatorReference( QwSIS3320_Accumulator *accum, Double_t weight);
protected:

  Double_t fNumberOfSamples;	///< Number of accumulated samples
  Double_t fAccumulatorSum;	///< Accumulator sum
  Double_t fAccumulatorAvg;	///< Accumulator average

private:

  // Ntuple array indices
  size_t fTreeArrayIndex; //! Index of this data element in tree
  size_t fTreeArrayNumEntries; //! Number of entries from this data element

  std::vector<QwSIS3320_Accumulator*> fAccumulators;
  std::vector<Double_t> fAccumulatorWeights;
};

// Output stream operator<< for the accumulators
inline std::ostream& operator<< (std::ostream& stream, const QwSIS3320_LogicalAccumulator& a) {
  stream << a.GetAccumulatorSum() << " (" << a.GetNumberOfSamples() << ")";
  return stream;
}

#endif // __QwSIS3320_LogicalAccumulator__
