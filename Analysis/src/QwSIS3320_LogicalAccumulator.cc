/**
 * \file	QwSIS3320_LogicalAccumulator.cc
 *
 * \brief	Implementation of the SIS3320 sampling ADC accumulator
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

#include "QwSIS3320_LogicalAccumulator.h"
#include "QwSIS3320_Accumulator.h"

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"

/**
 * Addition of offset to accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
const QwSIS3320_LogicalAccumulator QwSIS3320_LogicalAccumulator::operator+ (const Double_t &value) const
{
  QwSIS3320_LogicalAccumulator result = *this;
  result += value;
  return result;
}

/**
 * Subtraction of offset from accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
const QwSIS3320_LogicalAccumulator QwSIS3320_LogicalAccumulator::operator- (const Double_t &value) const
{
  QwSIS3320_LogicalAccumulator result = *this;
  result -= value;
  return result;
}

/**
 * Multiplication of factor to accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
const QwSIS3320_LogicalAccumulator QwSIS3320_LogicalAccumulator::operator* (const Double_t &value) const
{
  QwSIS3320_LogicalAccumulator result = *this;
  result *= value;
  return result;
}

/**
 * Division of factor from accumulated data (not division-by-zero safe)
 * @param value Right-hand side
 * @return Left-hand side
 */
const QwSIS3320_LogicalAccumulator QwSIS3320_LogicalAccumulator::operator/ (const Double_t &value) const
{
  QwSIS3320_LogicalAccumulator result = *this;
  if (value != 0)
    result /= value;
  return result;
}

/**
 * Multiplication assignment of factor to accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
QwSIS3320_LogicalAccumulator& QwSIS3320_LogicalAccumulator::operator*= (const Double_t &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum *= value;
  }
  return *this;
}

/**
 * Division assignment of factor from accumulated data (not division-by-zero safe)
 * @param value Right-hand side
 * @return Left-hand side
 */
QwSIS3320_LogicalAccumulator& QwSIS3320_LogicalAccumulator::operator/= (const Double_t &value)
{
  if (!IsNameEmpty()) {
    if (value != 0.0)
      this->fAccumulatorSum /= value;
  }
  return *this;
}

/**
 * Addition assignment of offset to accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
QwSIS3320_LogicalAccumulator& QwSIS3320_LogicalAccumulator::operator+= (const Double_t &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum += value;
  }
  return *this;
}

/**
 * Subtraction assignment of offset from accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
QwSIS3320_LogicalAccumulator& QwSIS3320_LogicalAccumulator::operator-= (const Double_t &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum -= value;
  }
  return *this;
}

/**
 * Addition of accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
const QwSIS3320_LogicalAccumulator QwSIS3320_LogicalAccumulator::operator+ (const QwSIS3320_LogicalAccumulator &value) const
{
  QwSIS3320_LogicalAccumulator result = *this;
  result += value;
  return result;
}

/**
 * Subtraction of accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
const QwSIS3320_LogicalAccumulator QwSIS3320_LogicalAccumulator::operator- (const QwSIS3320_LogicalAccumulator &value) const
{
  QwSIS3320_LogicalAccumulator result = *this;
  result -= value;
  return result;
}

/**
 * Assignment of accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
QwSIS3320_LogicalAccumulator& QwSIS3320_LogicalAccumulator::operator= (const QwSIS3320_LogicalAccumulator &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum = value.fAccumulatorSum;
    this->fNumberOfSamples = value.fNumberOfSamples;
  }
  return *this;
}

/**
 * Addition assignment of accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
QwSIS3320_LogicalAccumulator& QwSIS3320_LogicalAccumulator::operator+= (const QwSIS3320_LogicalAccumulator &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum += value.fAccumulatorSum;
    this->fNumberOfSamples += value.fNumberOfSamples;
  }
  return *this;
}

/**
 * Subtraction assignment of accumulated data
 * @param value Right-hand side
 * @return Left-hand side
 */
QwSIS3320_LogicalAccumulator& QwSIS3320_LogicalAccumulator::operator-= (const QwSIS3320_LogicalAccumulator &value)
{
  if (!IsNameEmpty()) {
    this->fAccumulatorSum -= value.fAccumulatorSum;
    this->fNumberOfSamples -= value.fNumberOfSamples;
  }
  return *this;
}


void QwSIS3320_LogicalAccumulator::Sum(const QwSIS3320_LogicalAccumulator &value1, const QwSIS3320_LogicalAccumulator &value2)
{
  *this  = value1;
  *this += value2;
}


void QwSIS3320_LogicalAccumulator::Difference(const QwSIS3320_LogicalAccumulator &value1, const QwSIS3320_LogicalAccumulator &value2)
{
  *this  = value1;
  *this -= value2;
}


void QwSIS3320_LogicalAccumulator::Ratio(const QwSIS3320_LogicalAccumulator &numer, const QwSIS3320_LogicalAccumulator &denom)
{
  if (!IsNameEmpty()) {
    if (denom.fAccumulatorSum != 0.0)
      fAccumulatorSum = numer.fAccumulatorSum / denom.fAccumulatorSum;
    fNumberOfSamples = numer.fNumberOfSamples + denom.fNumberOfSamples;
  }
}


void  QwSIS3320_LogicalAccumulator::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (IsNameEmpty()) {
    //  This accumulator is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = "hw_sum/D";

    // TODO: (jc2) I don't know how to deal with Logical Samples right now, so
    // I'll disable this for now.
    //values.push_back(0.0);
    //list += ":num_samples/D";

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }
}

void QwSIS3320_LogicalAccumulator::FillTreeVector(std::vector<Double_t> &values) const
{
  if (IsNameEmpty()) {
    //  This accumulator is not used, so skip filling the tree vector.
  } else if (fTreeArrayNumEntries <= 0) {
    QwError << "QwSIS3320_LogicalAccumulator::FillTreeVector:  fTreeArrayNumEntries == "
            << fTreeArrayNumEntries << QwLog::endl;
  } else if (values.size() < fTreeArrayIndex + fTreeArrayNumEntries) {
    QwError << "QwSIS3320_LogicalAccumulator::FillTreeVector:  values.size() == "
            << values.size()
            << "; fTreeArrayIndex + fTreeArrayNumEntries == "
            << fTreeArrayIndex + fTreeArrayNumEntries
            << QwLog::endl;
  } else {
    size_t index = fTreeArrayIndex;
    values[index++] = GetAccumulatorSum();
    //values[index++] = GetNumberOfSamples();
  }
}


void QwSIS3320_LogicalAccumulator::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (IsNameEmpty()) {

    //  This channel is not used, so skip filling the histograms.

  } else {

    TString basename = prefix + GetElementName();

    // TODO: (jc2) Only one Histogram for now, until I can figure out what to
    // do with Logical Samples
    fHistograms.resize(1, NULL);
    size_t index = 0;
    //fHistograms[index++]   = gQwHists.Construct1DHist(basename + Form("_nevents"));
    fHistograms[index++]   = gQwHists.Construct1DHist(basename + Form("_sum"));
    //fHistograms[index++]   = gQwHists.Construct1DHist(basename + Form("_avg"));

  }
}

void QwSIS3320_LogicalAccumulator::FillHistograms()
{
  Int_t index = 0;
  if (IsNameEmpty()) {

    //  This channel is not used, so skip creating the histograms.

  } else {

    // TODO: (jc2) Only one histogram for now until I figure out what to do with
    // Logical Samples
    //if (fHistograms[index] != NULL)
    //  fHistograms[index++]->Fill(GetNumberOfSamples());
    if (fHistograms[index] != NULL)
      fHistograms[index]->Fill(GetAccumulatorSum());
    //if (fHistograms[index] != NULL)
    //  fHistograms[index+1]->Fill(GetAccumulatorAvg());

  }
}

void  QwSIS3320_LogicalAccumulator::DeleteHistograms()
{
  for (UInt_t i = 0; i < fHistograms.size(); i++) {
    if (fHistograms[i] != NULL)
      fHistograms[i]->Delete();
    fHistograms[i] = NULL;
  }
  fHistograms.clear();
}


void QwSIS3320_LogicalAccumulator::ProcessEvent()
{
  for (size_t i = 0; i < fAccumulators.size(); i++) {
    fAccumulatorSum += fAccumulatorWeights[i]*(fAccumulators[i]->GetAccumulatorSum());
  }
}

void QwSIS3320_LogicalAccumulator::AddAccumulatorReference(
    QwSIS3320_Accumulator *accum, Double_t weight)
{
  if (accum) {
    fAccumulators.push_back(accum);
    fAccumulatorWeights.push_back(weight);
  }
}

