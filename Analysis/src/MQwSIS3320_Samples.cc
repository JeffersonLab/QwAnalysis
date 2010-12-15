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
ClassImp(MQwSIS3320_Samples);

// System headers
#include <numeric>
#include <algorithm>

// Qweak headers
#include "QwLog.h"

std::vector<MQwSIS3320_Type> MQwSIS3320_Samples::fIndex;

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
          QwError << "MQwSIS3320_Samples: Illegal number of samples per word!" << QwLog::endl;
          words_read = 0;
          return words_read;
      }
    }
    words_read = fNumberOfDataWords;

  } else {
    QwError << "MQwSIS3320_Samples: Not enough words while processing buffer!" << QwLog::endl;
  }

  return words_read;
};


std::pair<size_t,MQwSIS3320_Type> MQwSIS3320_Samples::GetMax() const
{
  std::vector<MQwSIS3320_Type>::const_iterator max = std::max_element(fSamples.begin(), fSamples.end());
  return std::pair<size_t,MQwSIS3320_Type>(max - fSamples.begin(),*max);
};

std::pair<size_t,MQwSIS3320_Type> MQwSIS3320_Samples::GetMin() const
{
  std::vector<MQwSIS3320_Type>::const_iterator min = std::min_element(fSamples.begin(), fSamples.end());
  return std::pair<size_t,MQwSIS3320_Type>(min - fSamples.begin(),*min);
};

const MQwSIS3320_Type MQwSIS3320_Samples::GetSum() const
{
  return std::accumulate(fSamples.begin(), fSamples.end(), 0.0);
};


const MQwSIS3320_Type MQwSIS3320_Samples::GetSumInTimeWindow(const UInt_t start, const UInt_t stop) const
{
  if (start >= fSamples.size() || stop >= fSamples.size()) return 0;
  return std::accumulate(&fSamples.at(start), &fSamples.at(stop), 0.0);
};


void MQwSIS3320_Samples::UpdateGraph()
{
  const size_t n = fSamples.size();
  const MQwSIS3320_Type* vx = &(*fIndex.begin());
  const MQwSIS3320_Type* vy = &(*fSamples.begin());
  fGraph = new TGraph(n, vx, vy);
}


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
    if (value != 0.0)
      this->fSamples.at(i) /= value;
    else
      this->fSamples.at(i) = 0.0;
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
  this->fSamples.resize(value.fSamples.size());
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
