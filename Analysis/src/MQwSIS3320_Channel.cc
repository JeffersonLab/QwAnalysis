/**
 * \file	MQwSIS3320_Channel.cc
 *
 * \brief	Implementation of the decoding of SIS3320 sampling ADC data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The MQwSIS3320 set of classes is defined to read the integrated and sampled
 * data from the Compton photon detector.  Because the scope of this module is
 * similar the the VQWK ADC module (integration and asymmetries), parts of
 * this class are very similar to QwVQWK_Channel.
 *
 * Additional functionality is required to process the sampling data in the
 * SIS3320 events.
 *
 */

#include "MQwSIS3320_Channel.h"


// Initialize mode flags
const unsigned int MQwSIS3320_Channel::MODE_ACCUMULATOR = 0x0;
const unsigned int MQwSIS3320_Channel::MODE_LONG_WORD_SAMPLING = 0x1;
const unsigned int MQwSIS3320_Channel::MODE_SHORT_WORD_SAMPLING = 0x2;
const unsigned int MQwSIS3320_Channel::MODE_NOTREADY = 0xda00;
// Initialize sampling mode format flags
const unsigned int MQwSIS3320_Channel::FORMAT_MULTI_EVENT = 0x3;
const unsigned int MQwSIS3320_Channel::FORMAT_SINGLE_EVENT = 0x4;

// Compile-time debug level
const Bool_t MQwSIS3320_Channel::kDEBUG = kTRUE;


/**
 * Conversion factor to translate the average bit count in an ADC
 * channel of the SIS3320 into average voltage.
 * There are 2^12 possible states over the full 5 V range.
 */
const Double_t MQwSIS3320_Channel::kVoltsPerBit = 5.0 / pow(2.0, 12);


/**
 * Initialize the MQwSIS3320_Channel by assigning it a name
 * @param channel Number of the channel
 * @param name Name for the channel
 */
void  MQwSIS3320_Channel::InitializeChannel(UInt_t channel, TString name)
{
  // Inherited from VQwDataElement
  SetElementName(name);
  SetNumberOfDataWords(0);

  // Set channel number
  fChannel = channel;

  // Default values when no event read yet
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
Bool_t MQwSIS3320_Channel::IsGoodEvent()
{
  Bool_t fEventIsGood = kTRUE;
  fEventIsGood &= (GetNumberOfSamples() > 0);
  return fEventIsGood;
};

/**
 * Clear the event data in sampling buffer and accumulators
 */
void MQwSIS3320_Channel::ClearEventData()
{
  // Clear the sampled events
  for (size_t i = 0; i < fSamples.size(); i++)
    fSamples.at(i).ClearEventData();
  fSamples.clear(); // and back to zero events
  for (size_t i = 0; i < fSamplesRaw.size(); i++)
    fSamplesRaw.at(i).ClearEventData();
  fSamplesRaw.clear(); // and back to zero events

  // Clear the accumulators
  for (size_t i = 0; i < fAccumulators.size(); i++)
    fAccumulators.at(i).ClearEventData();
  for (size_t i = 0; i < fAccumulatorsRaw.size(); i++)
    fAccumulatorsRaw.at(i).ClearEventData();
  // (the number of accumulators is constant, don't clear them)
};

/**
 * Extract the sampling and accumulator data from the CODA buffer
 * @param buffer Input buffer
 * @param num_words_left Number of words left in the input buffer
 * @param index Starting position in the buffer
 * @return Number of words processed in this method
 */
Int_t MQwSIS3320_Channel::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index)
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
    words_read = 2;
    UInt_t packedtiming; // locals declared outside of switch/case
    switch (local_mode) {

      // This is an accumulator buffer
      case MODE_ACCUMULATOR:
        words_read--;
        // TODO The -- is because we use one word to determine whether this is
        // an accumulator block -> BAD!

        // Read the accumulator blocks
        for (size_t i = 0; i < fAccumulatorsRaw.size(); i++) {
          words_read += fAccumulatorsRaw[i].ProcessEvBuffer(&(buffer[words_read]), num_words_left-words_read);
          if (kDEBUG) std::cout << fAccumulatorsRaw[i] << std::endl;
        }
        // Read the threshold information
        fAccumulatorDAC = buffer[words_read++];
        fAccumulatorThreshold1 = buffer[words_read++];
        fAccumulatorThreshold2 = buffer[words_read++];
        // Read the timing information, packed in one word
        packedtiming = buffer[words_read++];
        fAccumulatorTimingAfter6  = (packedtiming >>= 8) & 0xFF;
        fAccumulatorTimingBefore6 = (packedtiming >>= 8) & 0xFF;
        fAccumulatorTimingAfter5  = (packedtiming >>= 8) & 0xFF;
        fAccumulatorTimingBefore5 = (packedtiming >>= 8) & 0xFF;

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
            words_read = 5;

            // For all events in this buffer
            fSamplesRaw.resize(fNumberOfEvents);
            for (size_t event = 0; event < fNumberOfEvents; event++) {
              // create a new raw sampled event
              fSamplesRaw[event].SetNumberOfSamples(fSamplesPerEvent);
              // pass the buffer to read the samples
              words_read += fSamplesRaw[event].ProcessEvBuffer(&(buffer[words_read]), num_words_left-words_read);
              if (kDEBUG) std::cout << fSamplesRaw[event] << std::endl;
            }

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

            // Create a new raw sampled event
            fSamplesRaw.resize(fNumberOfSamples);
            // Pass the buffer to read the samples (only one event)
            fSamplesRaw.at(0).SetNumberOfSamples(fSamplesPerEvent);
            fSamplesRaw.at(0).ProcessEvBuffer(buffer, num_words_left, fSamplePointer);

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

  } else {
    std::cerr << "MQwSIS3320_Channel::ProcessEvBuffer: Not enough words!" << std::endl;
  }

  return words_read;
};

void MQwSIS3320_Channel::EncodeEventData(std::vector<UInt_t> &buffer)
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
//     for (size_t i = 0; i < fSamplesRaw.size() / 2; i++) {
//       sample  = fSamplesRaw.at(2*i) << 16;
//       sample |= fSamplesRaw.at(2*i+1);
//       samples.push_back(sample);
//     }

    for (size_t i = 0; i < header.size(); i++)
      buffer.push_back(header.at(i));
    for (size_t i = 0; i < samples.size(); i++)
      buffer.push_back(samples.at(i));
  }
};

/**
 * Process the event by removing pedestals and applying calibration
 */
void MQwSIS3320_Channel::ProcessEvent()
{
  for (size_t i = 0; i < fSamplesRaw.size(); i++)
    fSamples.at(i) = (fSamplesRaw.at(i) - fPedestal) * fCalibrationFactor;
  for (size_t i = 0; i < fAccumulatorsRaw.size(); i++) {
    Double_t pedestal = fPedestal * fAccumulatorsRaw.at(i).GetNumberOfSamples();
    fAccumulators.at(i) = (fAccumulatorsRaw.at(i) - pedestal) * fCalibrationFactor;
  }

  return;
};

/**
 * Addition of offset
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Channel MQwSIS3320_Channel::operator+ (const Double_t &value) const
{
  MQwSIS3320_Channel result = *this;
  result += value;
  return result;
};

/**
 * Subtraction of offset
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Channel MQwSIS3320_Channel::operator- (const Double_t &value) const
{
  MQwSIS3320_Channel result = *this;
  result -= value;
  return result;
};

/**
 * Addition
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Channel MQwSIS3320_Channel::operator+ (const MQwSIS3320_Channel &value) const
{
  MQwSIS3320_Channel result = *this;
  result += value;
  return result;
};

/**
 * Subtraction
 * @param value Right-hand side
 * @return Left-hand side
 */
const MQwSIS3320_Channel MQwSIS3320_Channel::operator- (const MQwSIS3320_Channel &value) const
{
  MQwSIS3320_Channel result = *this;
  result -= value;
  return result;
};

/**
 * Assignment
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Channel& MQwSIS3320_Channel::operator= (const MQwSIS3320_Channel &value)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      this->fSamples.at(i) = value.fSamples.at(i);
  }
  return *this;
};

/**
 * Addition assignment of offset
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Channel& MQwSIS3320_Channel::operator+= (const Double_t &value)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      this->fSamples.at(i) += value;
  }
  return *this;
};

/**
 * Subtraction assignment of offset
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Channel& MQwSIS3320_Channel::operator-= (const Double_t &value)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      this->fSamples.at(i) -= value;
  }
  return *this;
};

/**
 * Addition assignment
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Channel& MQwSIS3320_Channel::operator+= (const MQwSIS3320_Channel &value)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      this->fSamples.at(i) += value.fSamples.at(i);
  }
  return *this;
};

/**
 * Subtraction assignment
 * @param value Right-hand side
 * @return Left-hand side
 */
MQwSIS3320_Channel& MQwSIS3320_Channel::operator-= (const MQwSIS3320_Channel &value)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      this->fSamples.at(i) -= value.fSamples.at(i);
  }
  return *this;
};

/**
 * Sum of two channels
 * @param value1
 * @param value2
 */
void MQwSIS3320_Channel::Sum(MQwSIS3320_Channel &value1, MQwSIS3320_Channel &value2)
{
  *this =  value1;
  *this += value2;
};

/**
 * Difference of two channels
 * @param value1
 * @param value2
 */
void MQwSIS3320_Channel::Difference(MQwSIS3320_Channel &value1, MQwSIS3320_Channel &value2)
{
  *this =  value1;
  *this -= value2;
};

/**
 * Addition of a offset
 * @param offset
 */
void MQwSIS3320_Channel::Offset(Double_t offset)
{
  if (!IsNameEmpty()) {
    for (size_t i = 0; i < fSamples.size(); i++)
      fSamples.at(i) += offset;
  }
};

/**
 * Scaling by a scale factor
 * @param scale
 */
void MQwSIS3320_Channel::Scale(Double_t scale)
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
Bool_t MQwSIS3320_Channel::MatchSequenceNumber(UInt_t seqnumber)
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
Bool_t MQwSIS3320_Channel::MatchNumberOfSamples(UInt_t numsamples)
{
  Bool_t status = kTRUE;
  if (!IsNameEmpty()) {
    status = (fNumberOfSamples == numsamples);
  }
  return status;
};


/**
 * Print some debugging information about the MQwSIS3320_Channel to std::cout
 */
void MQwSIS3320_Channel::Print() const
{
  std::cout << "MQwSIS3320_Channel: " << GetElementName() << std::endl;
  std::cout << "fSamples: ";
  for (size_t i = 0; i < fSamples.size(); i++)
    std::cout << " " << fSamples.at(i);
  std::cout << "fAccumulators: ";
  for (size_t i = 0; i < fAccumulators.size(); i++)
    std::cout << fAccumulators.at(i) << std::endl;

  return;
}

