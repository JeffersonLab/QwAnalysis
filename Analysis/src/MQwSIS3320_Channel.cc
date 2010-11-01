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

// Qweak headers
#include "QwLog.h"
#include "QwHistogramHelper.h"

// Initialize data storage format flags
const unsigned int MQwSIS3320_Channel::FORMAT_ACCUMULATOR = 0x0;
const unsigned int MQwSIS3320_Channel::FORMAT_LONG_WORD_SAMPLING = 0x1;
const unsigned int MQwSIS3320_Channel::FORMAT_SHORT_WORD_SAMPLING = 0x2;
// Initialize sampling mode format flags
const unsigned int MQwSIS3320_Channel::MODE_ACCUM_EVENT = 0x1;
const unsigned int MQwSIS3320_Channel::MODE_MULTI_EVENT = 0x3;
const unsigned int MQwSIS3320_Channel::MODE_SINGLE_EVENT = 0x4;
const unsigned int MQwSIS3320_Channel::MODE_NOTREADY = 0xda00;

// Compile-time debug level
const Bool_t MQwSIS3320_Channel::kDEBUG = kFALSE;


/**
 * Conversion factor to translate the average bit count in an ADC
 * channel of the SIS3320 into average voltage.
 * There are 2^12 possible states over the full 5 V range.
 */
const Double_t MQwSIS3320_Channel::kVoltsPerBit = 5.0 / pow(2.0, 12);

/**
 * Conversion factor to translate the single sampling period to time.
 * The ADC will sample at 250 MHz, corresponding with 4 ns per sample.
 */
const Double_t MQwSIS3320_Channel::kNanoSecondsPerSample = 4.0;


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

  // Start with zero samples
  fSamples.resize(0); fSamplesRaw.resize(0);
  // Clear the average samples
  fAverageSamples.ClearEventData();
  fAverageSamplesRaw.ClearEventData();

  //for (size_t i = 0; i < fSamples.size(); i++) {
  //  TString name = GetElementName() + TString("_samples") + Form("%ld",i);
  //  fSamples[i].SetElementName(name);
  //  fSamplesRaw[i].SetElementName(name + "_raw");
  //}

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
  // Clear the average samples
  fAverageSamples.ClearEventData();
  fAverageSamplesRaw.ClearEventData();

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

    // The first word of the buffer is in the format: 0xFADC/id/ch/, where id
    // is the module identification, and ch the channel number in that module.

    // Check whether the first word has 0xFADC as the upper half
    UInt_t local_tag = (buffer[0] >> 16) & 0xFFFF;
    if (local_tag != 0xFADC) return words_read;

    // Check whether the first word contains the correct channel number
    UInt_t local_module = 0;
    local_module = (buffer[0] >> 8) & 0xFF;
    UInt_t local_channel = buffer[0] & 0xFF;
 
    if (local_channel != fChannel) return words_read;

    // Determine whether we are receiving a sampling or accumulator buffer:
    // Most significant short word (upper half):
    // - 0x0 in accumulator mode
    // - 0x1 when an expanded sampling buffer follows (12-bit sample / 32-bit word)
    // - 0x2 when a packed sampling buffer follows (2 * 12-bit samples / 32-bit word)
    // Least significant short word (lower half): 0x/stop mode/setup mode/ where
    // - stop mode is 1 for hardware trigger
    // - setup mode is 1,2,3,4...
    UInt_t local_format = (buffer[1] >> 16) & 0xFFFF;
    UInt_t local_stopmode = 0;
    local_stopmode = (buffer[1] >> 8) & 0xFF;

    UInt_t local_setupmode = (buffer[1]) & 0xFF;
    words_read = 2;
    UInt_t packedtiming; // locals declared outside of switch/case
    switch (local_format) {

      // This is a sampling buffer using long words
      case FORMAT_LONG_WORD_SAMPLING:
        QwWarning << "MQwSIS3320_Channel: Expanded word format not implemented"
                  << QwLog::endl;
        break; // end of FORMAT_LONG_WORD_SAMPLING

      // This is a sampling buffer using short words
      case FORMAT_SHORT_WORD_SAMPLING:
        UInt_t numberofsamples, numberofevents_expected, numberofevents_actual;
        switch (local_setupmode) {

          // This is a sampling buffer in multi event mode:
          // - many events are saved in one buffer for a complete helicity event
          case MODE_ACCUM_EVENT:
          case MODE_MULTI_EVENT:
            numberofsamples = buffer[3];
            numberofevents_expected = buffer[4];
            fSamplePointer = 0;
            words_read = 6;

            // For all events in this buffer
            SetNumberOfEvents(numberofevents_expected);
            numberofevents_actual = 0; // double check while reading the events
            for (size_t event = 0; event < GetNumberOfEvents(); event++) {
              // create a new raw sampled event
              fSamplesRaw[event].SetNumberOfSamples(numberofsamples);
              // pass the buffer to read the samples
              UInt_t samples_read = fSamplesRaw[event].ProcessEvBuffer(&(buffer[words_read]), num_words_left-words_read);
              // check whether we actually read any data
              if (samples_read == 0) break;
              // an actual sampled event was read
              words_read += samples_read;
              numberofevents_actual++;
              if (kDEBUG) QwOut << "Samples " << event << ": " << fSamplesRaw[event] << QwLog::endl;
            }
            if (numberofevents_expected != numberofevents_actual) {
              QwWarning << "MQwSIS3320_Channel: Expected " << numberofevents_expected << " events, "
                        << "but only read " << numberofevents_actual << "." << QwLog::endl;
              SetNumberOfEvents(numberofevents_actual);
            }

            break;

          // This is a sampling buffer in single event mode:
          // - one event only is saved in the buffer, and each event triggers
          //   a read-out so multiple event can occur in one helicity period
          // - because a circular buffer is used the pointer to the last sample
          //   is stored
          case MODE_SINGLE_EVENT:
            numberofsamples = buffer[3];
            numberofevents_expected = 1;
            fSamplePointer = buffer[2];

            // Create a new raw sampled event
            SetNumberOfEvents(numberofevents_expected);
            // Pass the buffer to read the samples (only one event)
            fSamplesRaw.at(0).SetNumberOfSamples(numberofsamples);
            fSamplesRaw.at(0).ProcessEvBuffer(buffer, num_words_left, fSamplePointer);

            break;

          // Default
          default:
            QwError << "MQwSIS3320_Channel: Received unknown sampling format: "
                    << std::hex << local_setupmode << std::dec << "." << QwLog::endl;
            words_read = 0;
            return words_read;

        } // end of switch (local_setupmode)

        fHasSamplingData = kTRUE;
        break; // end of FORMAT_SHORT_WORD_SAMPLING

      // This is an accumulator buffer
      case FORMAT_ACCUMULATOR:

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
          if (kDEBUG) QwOut << "Accum " << i+1 << ": " << fAccumulatorsRaw[i] << QwLog::endl;
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
          QwOut << "DAC: " << fAccumulatorDAC << QwLog::endl;
          QwOut << "Thresholds: " << fAccumulatorThreshold1 << ", "
                                  << fAccumulatorThreshold2 << QwLog::endl;
          QwOut << "Timings on accum 5: " << fAccumulatorTimingAfter5 << ", "
                                          << fAccumulatorTimingBefore5 << QwLog::endl;
          QwOut << "Timings on accum 6: " << fAccumulatorTimingAfter6 << ", "
                                          << fAccumulatorTimingBefore6 << QwLog::endl;
        }
        if (kDEBUG) {
          MQwSIS3320_Accumulator sum("sum");
          sum = fAccumulatorsRaw[1] + fAccumulatorsRaw[2] + fAccumulatorsRaw[3];
          QwOut << "Accum 1: " << fAccumulatorsRaw[0] << QwLog::endl;
          QwOut << "Accum 2 + 3 + 4: " << sum << QwLog::endl;
        }

        fHasAccumulatorData = kTRUE;
        break; // end of FORMAT_ACCUMULATOR

      // This is an incomplete buffer
      case MODE_NOTREADY:
        QwError << "MQwSIS3320_Channel: SIS3320 was not ready yet!" << QwLog::endl;
        break; // end of FORMAT_NOTREADY

      // Default
      default:
        QwError << "MQwSIS3320_Channel: Received unknown mode: "
                << std::hex << local_format << std::dec << "." << QwLog::endl;
        words_read = 0;
        return words_read;

    } // end of switch (local_format)

  } else {
    QwError << "MQwSIS3320_Channel: Not enough words while processing buffer!" << QwLog::endl;
  }

  return words_read;
};

void MQwSIS3320_Channel::EncodeEventData(std::vector<UInt_t> &buffer)
{
  //  Long_t sample;
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
  // Correct for pedestal and calibration factor
  if (fSamplesRaw.size() > 0) fSamples.resize(fSamplesRaw.size(), fSamplesRaw[0]);
  for (size_t i = 0; i < fSamplesRaw.size(); i++) {
    fSamples[i] = (fSamplesRaw[i] - fPedestal) * fCalibrationFactor;
  }
  for (size_t i = 0; i < fAccumulatorsRaw.size(); i++) {
    Double_t pedestal = fPedestal * fAccumulatorsRaw[i].GetNumberOfSamples();
    fAccumulators[i] = (fAccumulatorsRaw[i] - pedestal) * fCalibrationFactor;
  }

  // Calculate the average sample snapshot
  if (fSamples.size() > 0) {
    fAverageSamples.SetNumberOfSamples(fSamples[0].GetNumberOfSamples());
    fAverageSamplesRaw.SetNumberOfSamples(fSamples[0].GetNumberOfSamples());
  }
  for (size_t i = 0; i < fSamples.size(); i++) {
    fAverageSamples += fSamples[i];
    fAverageSamplesRaw += fSamplesRaw[i];
  }
  if (fSamples.size() > 0) {
    fAverageSamples /= fSamples.size();
    fAverageSamplesRaw /= fSamplesRaw.size();
  }

  // Calculate the windowed sample sums
  for (size_t i = 0; i < fSamples.size(); i++) {
    for (size_t timewindow = 0; timewindow < fTimeWindows.size(); timewindow++) {
      UInt_t start = fTimeWindows[timewindow].first;
      UInt_t stop = fTimeWindows[timewindow].second;
      fTimeWindowAverages[timewindow] += fSamples[i].GetSumInTimeWindow(start, stop);
    }
  }
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
      fSamples.at(i) = value.fSamples.at(i);
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
      fSamples.at(i) += value;
    for (size_t i = 0; i < fAccumulators.size(); i++)
      fAccumulators.at(i) += value;
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
      fSamples.at(i) -= value;
    for (size_t i = 0; i < fAccumulators.size(); i++)
      fAccumulators.at(i) -= value;
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
      fSamples.at(i) += value.fSamples.at(i);
    for (size_t i = 0; i < fAccumulators.size(); i++)
      fAccumulators.at(i) += value.fAccumulators.at(i);
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
      fSamples.at(i) -= value.fSamples.at(i);
    for (size_t i = 0; i < fAccumulators.size(); i++)
      fAccumulators.at(i) -= value.fAccumulators.at(i);
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
    for (size_t i = 0; i < fAccumulators.size(); i++)
      fAccumulators.at(i) += offset;
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
    for (size_t i = 0; i < fAccumulators.size(); i++)
      fAccumulators.at(i) *= scale;
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

void MQwSIS3320_Channel::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (IsNameEmpty()) {
    //  This channel is not used, so skip filling the histograms.
  } else {

    // Accumulators
    for (size_t i = 0; i < fAccumulators.size(); i++) {
      fAccumulators[i].ConstructHistograms(folder,prefix);
      fAccumulatorsRaw[i].ConstructHistograms(folder,prefix);
    }

    TString basename = prefix + GetElementName();

    fHistograms.resize(3, NULL);
    size_t index = 0;
    fHistograms[index++] = gQwHists.Construct1DHist(basename+Form("_sum"));
    fHistograms[index++] = gQwHists.Construct1DHist(basename+Form("_ped"));
    fHistograms[index++] = gQwHists.Construct1DHist(basename+Form("_ped_raw"));
  }
};

void MQwSIS3320_Channel::FillHistograms()
{
  size_t index = -1;
  if (fSamples.size() == 0) return;

  if (IsNameEmpty()) {
      //  This channel is not used, so skip creating the histograms.
  } else {

    // Accumulators
    for (size_t i = 0; i < fAccumulators.size(); i++) {
      fAccumulators[i].FillHistograms();
      fAccumulatorsRaw[i].FillHistograms();
    }

    if (fHistograms[++index] != NULL)
      fHistograms[index]->Fill(fAverageSamples.GetSum());
    if (fHistograms[++index] != NULL)
      fHistograms[index]->Fill(fAverageSamples.GetSample(0));
    if (fHistograms[++index] != NULL)
      fHistograms[index]->Fill(fAverageSamplesRaw.GetSample(0));
  }
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

void  MQwSIS3320_Channel::FillTreeVector(std::vector<Double_t> &values) const
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
 * Print value of the MQwSIS3320_Channel
 */
void MQwSIS3320_Channel::PrintValue() const
{
  QwMessage << std::setprecision(4)
            << std::setw(18) << std::left << GetElementName() << ", "
// n/a            << std::setw(15) << std::left << GetHardwareSum() << ", "
// n/a            << std::setw(15) << std::left << GetSampleAverage() << ", "
            << std::setw(15) << std::left << GetNumberOfEvents() << ", "
            << QwLog::endl;
}


/**
 * Print some debugging information about the MQwSIS3320_Channel
 */
void MQwSIS3320_Channel::PrintInfo() const
{
  QwOut << "MQwSIS3320_Channel \"" << GetElementName() << "\":" << QwLog::endl;
  QwOut << "Number of events: " << GetNumberOfEvents() << QwLog::endl;
  QwOut << "fSamplesRaw:" << QwLog::endl;
  for (size_t i = 0; i < fSamplesRaw.size(); i++) {
    QwOut << "event "  << i << ": " << fSamplesRaw.at(i) << QwLog::endl;
  }
  QwOut << "avg: " << fAverageSamplesRaw << QwLog::endl;
  QwOut << "avg sum: " << fAverageSamplesRaw.GetSum() << QwLog::endl;
  QwOut << "fSamples:" << QwLog::endl;
  for (size_t i = 0; i < fSamples.size(); i++) {
    QwOut << "event " << i << ": " << fSamples.at(i) << QwLog::endl;
  }
  QwOut << "avg: " << fAverageSamples << QwLog::endl;
  QwOut << "avg sum: " << fAverageSamples.GetSum() << QwLog::endl;
  QwOut << "fAccumulators -> fAccumulatorsRaw:" << QwLog::endl;
  for (size_t i = 0; i < fAccumulators.size(); i++) {
    QwOut << i << ": " << fAccumulatorsRaw.at(i) << " -> " << fAccumulators.at(i) << QwLog::endl;
  }
  UInt_t nped = 10;
  if (fSamplesRaw.size() > 0) {
    QwOut << "Average pedestal is: " << fAverageSamplesRaw.GetSumInTimeWindow(0, nped) / (Double_t) nped << QwLog::endl;
  }
}

