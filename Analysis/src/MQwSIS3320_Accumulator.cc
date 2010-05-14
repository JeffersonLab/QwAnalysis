/**
 * \file	MQwSIS3320_Accumulator.cc
 *
 * \brief	Implementation of the SIS3320 sampling ADC accumulator
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

#include "MQwSIS3320_Accumulator.h"

// Qweak headers
#include "QwLog.h"

// Fields in accumulator data buffer
const unsigned int MQwSIS3320_Accumulator::INDEX_NUM = 0;
const unsigned int MQwSIS3320_Accumulator::INDEX_SUM = 1;

Int_t MQwSIS3320_Accumulator::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement)
{
  UInt_t words_read = 0;
  UInt_t index;
  index = 0;
  if (num_words_left >= fNumberOfDataWords) {
    // Read all words
    fNumberOfSamples = buffer[0];
    fAccumulatorSum = buffer[2];  // first assign Int_t to Long_t, so that we
    fAccumulatorSum <<= sizeof(Int_t);      //  can shift it into the higher registers
    fAccumulatorSum += buffer[1];
    words_read = fNumberOfDataWords;

  } else {
    std::cerr << "MQwSIS3320_Samples::ProcessEvBuffer: Not enough words!" << std::endl;
  }

  return words_read;
};


/**
 * Addition of offset to accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Accumulator MQwSIS3320_Accumulator::operator+ (const Double_t &value) const
{
  MQwSIS3320_Accumulator result = *this;
  result += value;
  return result;
};

/**
 * Subtraction of offset from accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Accumulator MQwSIS3320_Accumulator::operator- (const Double_t &value) const
{
  MQwSIS3320_Accumulator result = *this;
  result -= value;
  return result;
};

/**
 * Multiplication of factor to accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Accumulator MQwSIS3320_Accumulator::operator* (const Double_t &value) const
{
  MQwSIS3320_Accumulator result = *this;
  result *= value;
  return result;
};

/**
 * Division of factor from accumulated data (not division-by-zero safe)
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Accumulator MQwSIS3320_Accumulator::operator/ (const Double_t &value) const
{
  MQwSIS3320_Accumulator result = *this;
  result /= value;
  return result;
};

/**
 * Multiplication assignment of factor to accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Accumulator& MQwSIS3320_Accumulator::operator*= (const Double_t &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum *= value;
  }
  return *this;
};

/**
 * Division assignment of factor from accumulated data (not division-by-zero safe)
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Accumulator& MQwSIS3320_Accumulator::operator/= (const Double_t &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum /= value;
  }
  return *this;
};

/**
 * Addition assignment of offset to accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Accumulator& MQwSIS3320_Accumulator::operator+= (const Double_t &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum += value;
  }
  return *this;
};

/**
 * Subtraction assignment of offset from accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Accumulator& MQwSIS3320_Accumulator::operator-= (const Double_t &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum -= value;
  }
  return *this;
};

/**
 * Addition of accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Accumulator MQwSIS3320_Accumulator::operator+ (const MQwSIS3320_Accumulator &value) const
{
  MQwSIS3320_Accumulator result = *this;
  result += value;
  return result;
};

/**
 * Subtraction of accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Accumulator MQwSIS3320_Accumulator::operator- (const MQwSIS3320_Accumulator &value) const
{
  MQwSIS3320_Accumulator result = *this;
  result -= value;
  return result;
};

/**
 * Assignment of accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Accumulator& MQwSIS3320_Accumulator::operator= (const MQwSIS3320_Accumulator &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum = value.fAccumulatorSum;
    this->fNumberOfSamples = value.fNumberOfSamples;
  }
  return *this;
};

/**
 * Addition assignment of accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Accumulator& MQwSIS3320_Accumulator::operator+= (const MQwSIS3320_Accumulator &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum += value.fAccumulatorSum;
    this->fNumberOfSamples += value.fNumberOfSamples;
  }
  return *this;
};

/**
 * Subtraction assignment of accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Accumulator& MQwSIS3320_Accumulator::operator-= (const MQwSIS3320_Accumulator &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum -= value.fAccumulatorSum;
    this->fNumberOfSamples -= value.fNumberOfSamples;
  }
  return *this;
};


void  MQwSIS3320_Accumulator::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (IsNameEmpty()) {
    //  This accumulator is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = "sum/D";
    values.push_back(0.0);
    list += ":nevents/D"; // TODO this should actually be an int

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }
};

void MQwSIS3320_Accumulator::FillTreeVector(std::vector<Double_t> &values)
{
  if (IsNameEmpty()) {
    //  This accumulator is not used, so skip filling the tree vector.
  } else if (fTreeArrayNumEntries <= 0) {
    std::cerr << "MQwSIS3320_Accumulator::FillTreeVector:  fTreeArrayNumEntries == "
              << fTreeArrayNumEntries << std::endl;
  } else if (values.size() < fTreeArrayIndex + fTreeArrayNumEntries) {
    std::cerr << "MQwSIS3320_Accumulator::FillTreeVector:  values.size() == "
              << values.size()
              << "; fTreeArrayIndex + fTreeArrayNumEntries == "
              << fTreeArrayIndex + fTreeArrayNumEntries
              << std::endl;
  } else {
    size_t index = fTreeArrayIndex;
    values[index++] = this->GetAccumulatorSum();
    values[index++] = this->GetNumberOfSamples();
  }
};


