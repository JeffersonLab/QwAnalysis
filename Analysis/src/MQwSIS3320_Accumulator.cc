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
#include "QwHistogramHelper.h"

// Fields in accumulator data buffer
const unsigned int MQwSIS3320_Accumulator::INDEX_NUM = 0;
const unsigned int MQwSIS3320_Accumulator::INDEX_SUM = 1;

Int_t MQwSIS3320_Accumulator::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement)
{
  UInt_t words_read = 0;
  UInt_t index;
  Long_t sum;
  Int_t nsamples;
  index = 0;
  if (num_words_left >= fNumberOfDataWords) {

    // Read all words
    nsamples = buffer[0];
    sum = buffer[2];		// first assign Int_t to Long_t, so that we
    sum <<= sizeof(Int_t);	//  can shift it into the higher registers
    sum += buffer[1];
    words_read = fNumberOfDataWords;

    fNumberOfSamples = nsamples;
    fAccumulatorSum = sum;

  } else {
    QwWarning << "MQwSIS3320_Accumulator::ProcessEvBuffer: Not enough words!" << QwLog::endl;
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

void MQwSIS3320_Accumulator::FillTreeVector(std::vector<Double_t> &values) const
{
  if (IsNameEmpty()) {
    //  This accumulator is not used, so skip filling the tree vector.
  } else if (fTreeArrayNumEntries <= 0) {
    QwError << "MQwSIS3320_Accumulator::FillTreeVector:  fTreeArrayNumEntries == "
            << fTreeArrayNumEntries << QwLog::endl;
  } else if (values.size() < fTreeArrayIndex + fTreeArrayNumEntries) {
    QwError << "MQwSIS3320_Accumulator::FillTreeVector:  values.size() == "
            << values.size()
            << "; fTreeArrayIndex + fTreeArrayNumEntries == "
            << fTreeArrayIndex + fTreeArrayNumEntries
            << QwLog::endl;
  } else {
    size_t index = fTreeArrayIndex;
    values[index++] = this->GetAccumulatorSum();
    values[index++] = this->GetNumberOfSamples();
  }
};


void MQwSIS3320_Accumulator::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (IsNameEmpty()) {

    //  This channel is not used, so skip filling the histograms.

  } else {

    TString basename = prefix + GetElementName();

    fHistograms.resize(3, NULL);
    size_t index = 0;
    fHistograms[index++]   = gQwHists.Construct1DHist(basename + Form("_nevents"));
    fHistograms[index++]   = gQwHists.Construct1DHist(basename + Form("_sum"));
    fHistograms[index++]   = gQwHists.Construct1DHist(basename + Form("_avg"));

  }
};

void MQwSIS3320_Accumulator::FillHistograms()
{
  Int_t index = 0;
  if (IsNameEmpty()) {

    //  This channel is not used, so skip creating the histograms.

  } else {

    if (fHistograms[index] != NULL)
      fHistograms[index++]->Fill(GetNumberOfSamples());
    if (fHistograms[index] != NULL)
      fHistograms[index+1]->Fill(GetAccumulatorSum());
    if (fHistograms[index] != NULL)
      fHistograms[index+1]->Fill(GetAccumulatorAvg());

  }
};

