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
const Bool_t MQwSIS3320_Channel::kDEBUG = kFALSE;


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

  // Set accumulator names
  for (size_t i = 0; i < fAccumulators.size(); i++) {
    TString name = GetElementName() + TString("_accum") + Form("%ld",i);
    fAccumulators[i].SetElementName(name);
    fAccumulatorsRaw[i].SetElementName(name + "_raw");
  }
  fSamples.resize(1); fSamplesRaw.resize(1); // TODO This is done right now to
  // make sure that we can create the tree branches.  If there is ever a
  // triggered tree in the ROOT file, then we will need to figure out what
  // to do with this.
  for (size_t i = 0; i < fSamples.size(); i++) {
    TString name = GetElementName() + TString("_samples") + Form("%ld",i);
    fSamples[i].SetElementName(name);
    fSamplesRaw[i].SetElementName(name + "_raw");
  }

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
  for (size_t i = 0; i < fSamples.size(); i++)
    fEventIsGood &= (fSamples[i].GetNumberOfSamples() > 0);
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

        // Definition of the accumulators: (numbered from 1)
        // 1: digital sum of all samples in the event
        //
        // 2: threshold 1 < sample value
        // 3: threshold 2 < sample value <= threshold 1
        // 4: sample value <= threshold 2
        // Test: 1 == 2 + 3 + 4
        //
        // 6: sample value <= threshold 2
        //    and add samples before and after
        // 5: threshold 2 < sample value <= threshold 1
        //    and add samples before and after
        //    but samples added in accumulator 6 are not added to accumulator 5

        // Read the accumulator blocks
        for (size_t i = 0; i < fAccumulatorsRaw.size(); i++) {
          words_read += fAccumulatorsRaw[i].ProcessEvBuffer(&(buffer[words_read]), num_words_left-words_read);
          if (kDEBUG) std::cout << "Accum " << i+1 << ": " << fAccumulatorsRaw[i] << std::endl;
        }
        // Read the threshold information
        fAccumulatorDAC = buffer[words_read++];
        // Thresholds are maximum 12 bits, so they seem to have wasted a word here :-)
        fAccumulatorThreshold1 = buffer[words_read++];
        fAccumulatorThreshold2 = buffer[words_read++];
        // Read the timing information, packed in one word
        packedtiming = buffer[words_read++];
        fAccumulatorTimingAfter6  = (packedtiming >>= 8) & 0xFF;
        fAccumulatorTimingBefore6 = (packedtiming >>= 8) & 0xFF;
        fAccumulatorTimingAfter5  = (packedtiming >>= 8) & 0xFF;
        fAccumulatorTimingBefore5 = (packedtiming >>= 8) & 0xFF;
        if (kDEBUG) {
          std::cout << "DAC: " << fAccumulatorDAC << std::endl;
          std::cout << "Thresholds: " << fAccumulatorThreshold1 << ", "
                                      << fAccumulatorThreshold2 << std::endl;
          std::cout << "Timings on accum 5: " << fAccumulatorTimingAfter5 << ", "
                                              << fAccumulatorTimingBefore5 << std::endl;
          std::cout << "Timings on accum 6: " << fAccumulatorTimingAfter6 << ", "
                                              << fAccumulatorTimingBefore6 << std::endl;
        }
        if (kDEBUG) {
          MQwSIS3320_Accumulator sum("sum");
          sum = fAccumulatorsRaw[1] + fAccumulatorsRaw[2] + fAccumulatorsRaw[3];
          std::cout << "Accum 1: " << fAccumulatorsRaw[0] << std::endl;
          std::cout << "Accum 2 + 3 + 4: " << sum << std::endl;
        }

        fHasAccumulatorData = kTRUE;
        break; // end of MODE_ACCUMULATOR

      // This is a sampling buffer using long words
      case MODE_LONG_WORD_SAMPLING:
        break; // end of MODE_LONG_WORD_SAMPLING

      // This is a sampling buffer using short words
      case MODE_SHORT_WORD_SAMPLING:
        UInt_t numberofsamples, numberofevents;
        switch (local_format) {

          // This is a sampling buffer in multi event mode:
          // - many events are saved in one buffer for a complete helicity event
          case FORMAT_MULTI_EVENT:
            numberofsamples = buffer[3];
            numberofevents = buffer[4];
            fSamplePointer = 0;
            words_read = 5;

            // For all events in this buffer
            SetNumberOfEvents(numberofevents);
            for (size_t event = 0; event < GetNumberOfEvents(); event++) {
              // create a new raw sampled event
              fSamplesRaw[event].SetNumberOfSamples(numberofsamples);
              // pass the buffer to read the samples
              words_read += fSamplesRaw[event].ProcessEvBuffer(&(buffer[words_read]), num_words_left-words_read);
              if (kDEBUG) std::cout << "Samples " << event << ": " << fSamplesRaw[event] << std::endl;
            }

            break;

          // This is a sampling buffer in single event mode:
          // - one event only is saved in the buffer, and each event triggers
          //   a read-out so multiple event can occur in one helicity period
          // - because a circular buffer is used the pointer to the last sample
          //   is stored
          case FORMAT_SINGLE_EVENT:
            numberofsamples = buffer[3];
            numberofevents = 1;
            fSamplePointer = buffer[2];

            // Create a new raw sampled event
            SetNumberOfEvents(numberofevents);
            // Pass the buffer to read the samples (only one event)
            fSamplesRaw.at(0).SetNumberOfSamples(numberofsamples);
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
// TODO
//    header.push_back(fNumberOfSamples * fNumberOfEvents); // Total number of samples
//    header.push_back(fNumberOfSamples); // Number of samples per event
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
  // Because the sampling
  for (size_t i = 0; i < fSamplesRaw.size(); i++) {
    fSamples[i] = (fSamplesRaw[i] - fPedestal) * fCalibrationFactor;
  }
  for (size_t i = 0; i < fAccumulatorsRaw.size(); i++) {
    Double_t pedestal = fPedestal * fAccumulatorsRaw[i].GetNumberOfSamples();
    fAccumulators[i] = (fAccumulatorsRaw[i] - pedestal) * fCalibrationFactor;
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
// TOOO
//  if (!IsNameEmpty()) {
//    status = (fNumberOfSamples == numsamples);
//  }
  return status;
};


void  MQwSIS3320_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  // Accumulators
  for (size_t i = 0; i < fAccumulators.size(); i++) {
    fAccumulators[i].ConstructBranchAndVector(tree, prefix, values);
    fAccumulatorsRaw[i].ConstructBranchAndVector(tree, prefix, values);
  }
  // Samples (only collected when running over data, so structure does not
  // actually exist yet at time of branch construction)
  //fSamples[0].ConstructBranchAndVector(tree, prefix, values);
  //fSamplesRaw[0].ConstructBranchAndVector(tree, prefix, values);
  // TODO See below for issues with including samples in the mps tree
};

void  MQwSIS3320_Channel::FillTreeVector(std::vector<Double_t> &values)
{
  // Accumulators
  for (size_t i = 0; i < fAccumulators.size(); i++) {
    fAccumulators[i].FillTreeVector(values);
    fAccumulatorsRaw[i].FillTreeVector(values);
  }
  // Samples
// TODO The following is disabled because it doesn't work.  The number of samples
// varies from mps to mps, so we keep a dynamic list.  This causes problems because
// the array indices that are fixed on an artificial dummy during the tree and branch
// construction are gone when we get to a (new) actual event.  Garbage gets written
// to the ROOT file and it plain doesn't work(TM).  Need a different approach to
// trigger events: a dedicated tree (useful for qweak tracking too) or a subtree
// of the existing MPS tree (correlation between mps and triggered events is clearer).
// Let's see what we can come up with...
//
//  for (size_t i = 0; i < fSamples.size(); i++) {
//    fSamples[i].FillTreeVector(values);
//    fSamplesRaw[i].FillTreeVector(values);
//  }
};

/**
 * Print some debugging information about the MQwSIS3320_Channel to std::cout
 */
void MQwSIS3320_Channel::Print() const
{
  std::cout << "MQwSIS3320_Channel: " << GetElementName() << std::endl;
  std::cout << "fSamplesRaw:" << std::endl;
  for (size_t i = 0; i < fSamplesRaw.size(); i++) {
    std::cout << i << ": " << fSamplesRaw.at(i) << std::endl;
  }
  std::cout << "fSamples:" << std::endl;
  for (size_t i = 0; i < fSamples.size(); i++) {
    std::cout << i << ": " << fSamples.at(i) << std::endl;
  }
  std::cout << "fAccumulators -> fAccumulatorsRaw:" << std::endl;
  for (size_t i = 0; i < fAccumulators.size(); i++) {
    std::cout << i << ": " << fAccumulatorsRaw.at(i) << " -> " << fAccumulators.at(i) << std::endl;
  }
  UInt_t nped = 15;
  std::cout << "Pedestal is: " << fSamplesRaw[0].GetSum(0,nped) / (Double_t) nped << std::endl;

  return;
}

