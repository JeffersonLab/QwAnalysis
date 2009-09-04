/**
 * \file	MQwSIS330.cc
 *
 * \brief	Implementation of the decoding of SIS3320 sampling ADC data buffers
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The MQwSIS3320 class is defined to read the integrated and sampled data
 * from the Compton photon detector.  Because the scope of this module is
 * similar the the VQWK ADC module (integration and asymmetries), parts of
 * this class are very similar to QwVQWK_Channel.
 *
 * Additional functionality is required to process the sampling data in the
 * SIS3320 events.
 *
 */


#include "MQwSIS3320.h"

/**
 * Conversion factor to translate the average bit count in an ADC
 * channel of the SIS3320 into average voltage.
 * There are 2^16 possible states over the full 10 V range.
 */
const Double_t MQwSIS3320::kVoltsPerBit = 10.0 / pow(2.0, 16);



/**
 * Initialize the MQwSIS3320 channel by assigning it a name
 * @param name Name for the channel
 */
void  MQwSIS3320::InitializeChannel(TString name)
{
  SetElementName(name);
  SetNumberOfDataWords(0);

  // Default unassigned identification
  fChannel = -1;
  fCurrentEvent = -1;
  fHasSamplingData = kFALSE;
  fHasAccumulatorData = kFALSE;

  // Pedestal calibration
  fPedestal = 0.0;
  fCalibrationFactor = 1.0;

  // Mock data parameters
  fMockAsymmetry = 0.0;
  fMockGaussianMean = 0.0;
  fMockGaussianSigma = 0.0;

  return;
};

/**
 * Check whether the event is a good event from the number of read samples
 * @return kTRUE if samples have been read
 */
Bool_t MQwSIS3320::IsGoodEvent()
{
  Bool_t fEventIsGood = kTRUE;
  fEventIsGood &= (GetNumberOfSamples() > 0);
  return fEventIsGood;
};

/**
 * Clear the event data
 */
void MQwSIS3320::ClearEventData()
{
  fNumberOfSamples = 0;
  fSamples.clear();
  fSamplesRaw.clear();
};

/**
 * Extract the sampling and accumulator data from the CODA buffer
 * @param buffer Input buffer
 * @param num_words_left Number of words left in the input buffer
 * @param index ?
 * @return Number of words processed in this method
 */
Int_t MQwSIS3320::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index)
{
  UInt_t words_read = 0;

  if (IsNameEmpty()) {
    // This channel is not used, but is present in the data stream.
    // Skip over this data.
    words_read = fNumberOfDataWords;
  } else if (num_words_left >= fNumberOfDataWords) {

    // Check whether the first word corresponds to the correct channel number
    if (! fChannel == buffer[0]) return words_read;

    // Determine whether we are receiving a sampling or accumulator buffer:
    // Most significant short word:
    // - zero in accumulator mode
    //   (assuming less than 0x10000 samples per event)
    // - 0x2 when a sampling buffer follows
    // Least significant short word:
    // - encodes the type of sampling mode (autostart, stop on trigger, etc)
    // - number of samples per event in accumulator mode
    UInt_t local_mode = (buffer[1] >> 16) & 0xFFFF;
    UInt_t local_format = (buffer[1]) & 0xFFFF;
    switch (local_mode) {

      // This is an accumulator buffer
      case MODE_ACCUMULATOR:
        fNumberOfDataWords = 2 + 6 * 3;


        fHasAccumulatorData = kTRUE;
        break; // end of MODE_ACCUMULATOR

      // This is a sampling buffer using long words
      case MODE_LONG_WORD_SAMPLING:
        break; // end of MODE_LONG_WORD_SAMPLING

      // This is a sampling buffer using short words
      case MODE_SHORT_WORD_SAMPLING:
        switch (local_format) {

          // This is a sampling buffer in multi event mode:
          // - many events are saved in one buffer for a complete helicity event
          case FORMAT_MULTI_EVENT:
            fSamplesPerEvent = buffer[3];
            fNumberOfEvents = buffer[4];
            fSamplePointer = 0;
            break;

          // This is a sampling buffer in single event mode:
          // - one event only is saved in the buffer, and each event triggers
          //   a read-out so multiple event can occur in one helicity period
          // - because a circular buffer is used the pointer to the last sample
          //   is stored
          case FORMAT_SINGLE_EVENT:
            fSamplesPerEvent = buffer[3];
            fNumberOfEvents = 1;
            fSamplePointer = buffer[2];
            break;

          // Default
          default:
            std::cerr << "Error: Received unknown sampling format!" << std::endl;
            words_read = 0;
            return words_read;

        } // end of switch (local_format)

        fHasSamplingData = kTRUE;
        break; // end of MODE_SHORT_WORD_SAMPLING

      // This is an incomplete buffer
      case MODE_NOTREADY:
        std::cerr << "Error: SIS3320 was not ready yet!" << std::endl;
        break; // end of MODE_NOTREADY

      // Default
      default:
        std::cerr << "Error: Received unknown mode!" << std::endl;
        words_read = 0;
        return words_read;

    } // end of switch (local_mode)

    words_read = fNumberOfDataWords;

  } else {
    std::cerr << "MQwSIS3320::ProcessEvBuffer: Not enough words!" << std::endl;
  }
  return words_read;
};

void MQwSIS3320::EncodeEventData(std::vector<UInt_t> &buffer)
{
  Long_t sample;
  std::vector<UInt_t> header;
  std::vector<UInt_t> samples;

  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
  } else {
    header.push_back(fChannel); // Channel number
    header.push_back(fSampleFormat); // Sample format (e.g. 0x20003)
    header.push_back(fNumberOfSamples * fNumberOfEvents); // Total number of samples
    header.push_back(fNumberOfSamples); // Number of samples per event
    header.push_back(fNumberOfEvents); // Number of events
    // Data is stored with two short words per long word
    for (size_t i = 0; i < fSamplesRaw.size() / 2; i++) {
      sample  = fSamplesRaw.at(2*i) << 16;
      sample |= fSamplesRaw.at(2*i+1);
      samples.push_back(sample);
    }

    for (size_t i = 0; i < header.size(); i++)
      buffer.push_back(header.at(i));
    for (size_t i = 0; i < samples.size(); i++)
      buffer.push_back(samples.at(i));
  }
};

void MQwSIS3320::ProcessEvent()
{
  fSamples.clear(); fSamples.resize(fSamplesRaw.size());
  fSampleSum = 0;
  for (size_t i = 0; i < fSamples.size(); i++) {
    fSamples.at(i) = fCalibrationFactor * (fSamplesRaw.at(i) - fPedestal);
    fSampleSum += fSamples.at(i);
  }

  return;
};

/**
 * Assignment, operates on the processed data only
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320& MQwSIS3320::operator= (const MQwSIS3320 &value)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      this->fSamples.at(i) = value.fSamples.at(i);
  }
  return *this;
};

/**
 * Addition assignment, operates on the processed data only
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320& MQwSIS3320::operator+= (const MQwSIS3320 &value)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      this->fSamples.at(i) += value.fSamples.at(i);
  }
  return *this;
};

/**
 * Subtraction assignment, operates on the processed data only
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320& MQwSIS3320::operator-= (const MQwSIS3320 &value)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      this->fSamples.at(i) -= value.fSamples.at(i);
  }
  return *this;
};

/**
 * Sum of two channels, operates on the processed data only
 * @param value1
 * @param value2
 */
void MQwSIS3320::Sum(MQwSIS3320 &value1, MQwSIS3320 &value2)
{
  *this =  value1;
  *this += value2;
};

/**
 * Difference of two channels, operates on the processed data only
 * @param value1
 * @param value2
 */
void MQwSIS3320::Difference(MQwSIS3320 &value1, MQwSIS3320 &value2)
{
  *this =  value1;
  *this -= value2;
};

/**
 * Addition of a offset term, operates on the processed data only
 * @param offset Offset term
 */
void MQwSIS3320::Offset(Double_t offset)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      fSamples.at(i) += offset;
  }
};

/**
 * Scaling by a scale factor, operates on the processed data only
 * @param scale Scale factor
 */
void MQwSIS3320::Scale(Double_t scale)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      fSamples.at(i) *= scale;
  }
};

/**
 * Check whether the sequence number matches
 * @param seqnumber Sequence number
 * @return kTRUE if the sequence number matches
 */
Bool_t MQwSIS3320::MatchSequenceNumber(UInt_t seqnumber)
{
  Bool_t status = kTRUE;
  if (!IsNameEmpty()) {
    status = (fSequenceNumber == seqnumber);
  }
  return status;
};

/**
 * Check whether the number of samples matches
 * @param numsamples Number of samples
 * @return kTRUE if the number of samples matches
 */
Bool_t MQwSIS3320::MatchNumberOfSamples(UInt_t numsamples)
{
  Bool_t status = kTRUE;
  if (!IsNameEmpty()) {
    status = (fNumberOfSamples == numsamples);
  }
  return status;
};


/**
 * Print some debugging information about the MQwSIS3320 channel to std::cout
 */
void MQwSIS3320::Print() const
{
  std::cout << "MQwSIS3320: " << GetElementName() << std::endl;
  std::cout << "fSamples: ";
  for (size_t i = 0; i < fSamples.size(); i++)
    std::cout << " " << fSamples.at(i);
  std::cout << "fAccumulators: ";
  for (size_t i = 0; i < fAccumulators.size(); i++)
    std::cout << fAccumulators.at(i) << std::endl;

  return;
}

