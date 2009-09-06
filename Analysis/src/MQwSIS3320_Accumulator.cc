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

// Fields in accumulator data buffer
const unsigned int MQwSIS3320_Accumulator::INDEX_NUM = 0;
const unsigned int MQwSIS3320_Accumulator::INDEX_SUM = 1;

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
  }
  return *this;
};
