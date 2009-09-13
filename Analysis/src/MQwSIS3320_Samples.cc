/**
 * \file	MQwSIS3320_Samples.cc
 *
 * \brief	Implementation of the SIS3320 sampling ADC samples
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

#include "MQwSIS3320_Samples.h"



Int_t MQwSIS3320_Samples::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement)
{
  UInt_t words_read = 0;
  UInt_t index = 0;
  if (num_words_left >= fNumberOfDataWords) {
    // Read all words
    for (size_t i = 0; i < fNumberOfDataWords; i++) {
      // Shift bits as necessary
      switch (fSamplesPerWord) {
        case 1:
          fSamples[index++] = buffer[i];
          break;
        case 2:
          fSamples[index++] =  buffer[i]        & 0xFFFF; // lowest 16 bits
          fSamples[index++] = (buffer[i] >> 16) & 0xFFFF; // highest 16 bits
          break;
        default:
          std::cerr << "MQwSIS3320_Samples: Illegal number of samples per word!" << std::endl;
          words_read = 0;
          return words_read;
      }
    }
    words_read = fNumberOfDataWords;

  } else {
    std::cerr << "MQwSIS3320_Samples::ProcessEvBuffer: Not enough words!" << std::endl;
  }

  return words_read;
};


const Int_t MQwSIS3320_Samples::GetSum() const
{
  return std::accumulate(fSamples.begin(), fSamples.end(), 0);
};

const Int_t MQwSIS3320_Samples::GetSum(UInt_t start, UInt_t stop) const
{
  if (start >= fSamples.size() || stop >= fSamples.size()) return 0;
  return std::accumulate(&fSamples.at(start), &fSamples.at(stop), 0);
};


/**
 * Addition of offset to sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Samples MQwSIS3320_Samples::operator+ (const Double_t &value) const
{
  MQwSIS3320_Samples result = *this;
  result += value;
  return result;
};

/**
 * Subtraction of offset from sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Samples MQwSIS3320_Samples::operator- (const Double_t &value) const
{
  MQwSIS3320_Samples result = *this;
  result -= value;
  return result;
};

/**
 * Multiplication of factor to sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Samples MQwSIS3320_Samples::operator* (const Double_t &value) const
{
  MQwSIS3320_Samples result = *this;
  result *= value;
  return result;
};

/**
 * Division of factor from sampled data (not division-by-zero safe)
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Samples MQwSIS3320_Samples::operator/ (const Double_t &value) const
{
  MQwSIS3320_Samples result = *this;
  result /= value;
  return result;
};

/**
 * Multiplication assignment of factor to sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Samples& MQwSIS3320_Samples::operator*= (const Double_t &value)
{
  for (size_t i = 0; i < fSamples.size(); i++)
    this->fSamples.at(i) *= value;
  return *this;
};

/**
 * Division assignment of factor from sampled data (not division-by-zero safe)
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Samples& MQwSIS3320_Samples::operator/= (const Double_t &value)
{
  for (size_t i = 0; i < fSamples.size(); i++)
    this->fSamples.at(i) /= value;
  return *this;
};

/**
 * Addition assignment of offset to sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Samples& MQwSIS3320_Samples::operator+= (const Double_t &value)
{
  for (size_t i = 0; i < fSamples.size(); i++)
    this->fSamples.at(i) += value;
  return *this;
};

/**
 * Subtraction assignment of offset from sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Samples& MQwSIS3320_Samples::operator-= (const Double_t &value)
{
  for (size_t i = 0; i < fSamples.size(); i++)
    this->fSamples.at(i) -= value;
  return *this;
};

/**
 * Addition of sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Samples MQwSIS3320_Samples::operator+ (const MQwSIS3320_Samples &value) const
{
  MQwSIS3320_Samples result = *this;
  result += value;
  return result;
};

/**
 * Subtraction of sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Samples MQwSIS3320_Samples::operator- (const MQwSIS3320_Samples &value) const
{
  MQwSIS3320_Samples result = *this;
  result -= value;
  return result;
};

/**
 * Assignment of sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Samples& MQwSIS3320_Samples::operator= (const MQwSIS3320_Samples &value)
{
  for (size_t i = 0; i < fSamples.size(); i++)
    this->fSamples.at(i) = value.fSamples.at(i);
  return *this;
};

/**
 * Addition assignment of sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Samples& MQwSIS3320_Samples::operator+= (const MQwSIS3320_Samples &value)
{
  for (size_t i = 0; i < fSamples.size(); i++)
    this->fSamples.at(i) += value.fSamples.at(i);
  return *this;
};

/**
 * Subtraction assignment of sampled data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Samples& MQwSIS3320_Samples::operator-= (const MQwSIS3320_Samples &value)
{
  for (size_t i = 0; i < fSamples.size(); i++)
    this->fSamples.at(i) -= value.fSamples.at(i);
  return *this;
};


void  MQwSIS3320_Samples::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (IsNameEmpty()) {
    //  This list of samples is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = "sample0/D";
    values.push_back(0.0);
    list += ":sw_ped/D";
    values.push_back(0.0);
    list += ":sw_sum/D";

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }
};

void MQwSIS3320_Samples::FillTreeVector(std::vector<Double_t> &values)
{
  if (fTreeArrayNumEntries <= 0) {
    std::cerr << "MQwSIS3320_Samples::FillTreeVector:  fTreeArrayNumEntries == "
              << fTreeArrayNumEntries << std::endl;
  } else if (values.size() < fTreeArrayIndex + fTreeArrayNumEntries) {
    std::cerr << "MQwSIS3320_Samples::FillTreeVector:  values.size() == "
              << values.size()
              << "; fTreeArrayIndex + fTreeArrayNumEntries == "
              << fTreeArrayIndex + fTreeArrayNumEntries
              << std::endl;
  } else {
    size_t index = fTreeArrayIndex;
    values[index++] = GetSample(0);
    values[index++] = GetSum(0, 15) / 15;
    values[index++] = GetSum();
  }
};
