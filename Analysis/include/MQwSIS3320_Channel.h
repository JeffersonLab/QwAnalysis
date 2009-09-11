/**
 * \class	MQwSIS3320_Channel	MQwSIS3320_Channel.h
 *
 * \brief	Class for the decoding of the SIS3320 sampling ADC data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The MQwSIS3320_Channel class is defined to read the integrated and sampled
 * data from the Compton photon detector.  Because the scope of this module is
 * similar the the VQWK ADC module (integration and asymmetries), parts of
 * this class are very similar to QwVQWK_Channel.
 *
 * The main data members of a MQwSIS3320_Channel are the vector fSamples of
 * MQwSIS3320_Samples where each entry stores a sample event, and the vector of
 * fAccumulators of MQwSIS3320_Accumulator where each entry stores an accumulator
 * block.  Both of these data members are derived from their --Raw counterparts
 * by subtraction of pedestals and multiplication with calibration constants.
 *
 */

#ifndef __MQwSIS3320_Channel__
#define __MQwSIS3320_Channel__

#include <iostream>
#include <vector>
#include <numeric>
#include "TTree.h"

// Boost math library for random number generation
#include "boost/random.hpp"

#include "VQwDataElement.h"
#include "MQwSIS3320_Accumulator.h"
#include "MQwSIS3320_Samples.h"

class MQwSIS3320_Channel: public VQwDataElement {

  public:

    MQwSIS3320_Channel() { };
    MQwSIS3320_Channel(UInt_t channel, TString name) {
      InitializeChannel(channel, name);
    };
    ~MQwSIS3320_Channel() {
      DeleteHistograms();
    };

    void  InitializeChannel(UInt_t channel, TString name);

    void  ClearEventData();

    void  SetRandomEventParameters(Double_t mean, Double_t sigma) { };
    void  SetRandomEventAsymmetry(Double_t asymmetry) { };
    void  RandomizeEventData(int helicity) { };
    void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0) { };
    void  SetEventData(Double_t* block, UInt_t sequencenumber = 0) { };
    void  EncodeEventData(std::vector<UInt_t> &buffer);

    Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index = 0);
    void  ProcessEvent();

    const MQwSIS3320_Channel operator+ (const Double_t &value) const;
    const MQwSIS3320_Channel operator- (const Double_t &value) const;
    const MQwSIS3320_Channel operator+ (const MQwSIS3320_Channel &value) const;
    const MQwSIS3320_Channel operator- (const MQwSIS3320_Channel &value) const;
    MQwSIS3320_Channel& operator=  (const MQwSIS3320_Channel &value);
    MQwSIS3320_Channel& operator+= (const Double_t &value);
    MQwSIS3320_Channel& operator-= (const Double_t &value);
    MQwSIS3320_Channel& operator+= (const MQwSIS3320_Channel &value);
    MQwSIS3320_Channel& operator-= (const MQwSIS3320_Channel &value);
    void Sum(MQwSIS3320_Channel &value1, MQwSIS3320_Channel &value2);
    void Difference(MQwSIS3320_Channel &value1, MQwSIS3320_Channel &value2);
    void Ratio(MQwSIS3320_Channel &numer, MQwSIS3320_Channel &denom) { };
    void Offset(Double_t Offset);
    void Scale(Double_t Offset);

    Bool_t MatchSequenceNumber(UInt_t seqnum);
    Bool_t MatchNumberOfSamples(UInt_t numsamp);

    void  ConstructHistograms(TDirectory *folder, TString &prefix) { };
    void  FillHistograms() { };

    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values) { };
    void  FillTreeVector(std::vector<Double_t> &values) { };

    MQwSIS3320_Samples& GetSamples(size_t i) { return fSamples.at(i); };
    MQwSIS3320_Samples& GetSamplesRaw(size_t i) { return fSamplesRaw.at(i); };

    const size_t GetSequenceNumber() const { return (fSequenceNumber); };
    const size_t GetNumberOfSamples() const { return (fNumberOfSamples); };
    const size_t GetNumberOfAccumulators() const { return (fNumberOfAccumulators); };
    void SetNumberOfAccumulators(UInt_t naccumulators) {
      fNumberOfAccumulators = naccumulators;
      fAccumulatorsRaw.resize(naccumulators);
    };

    void SetPedestal(Double_t ped) {fPedestal = ped; return; };
    Double_t GetPedestal() { return fPedestal; };
    void SetCalibrationFactor(Double_t factor) { fCalibrationFactor = factor; return; };
    Double_t GetCalibrationFactor() { return fCalibrationFactor; };

    Bool_t IsGoodEvent();

    void Copy(VQwDataElement *source) { };

    void Print() const;

 protected:

 private:

    static const Bool_t kDEBUG;

    // Identification information
    Int_t fChannel;
    Bool_t fHasSamplingData;
    Bool_t fHasAccumulatorData;

    // Randomness generator
    static boost::mt19937 fRandomnessGenerator;
    static boost::normal_distribution<double> fNormalDistribution;
    static boost::variate_generator
      < boost::mt19937, boost::normal_distribution<double> >fNormalRandomVariable;

    /* ADC Calibration */
    static const Double_t kVoltsPerBit;
    Double_t fPedestal;
    Double_t fCalibrationFactor;

    /* In sampling mode we have multiple events in a single data block */
    Int_t fCurrentEvent; //! Current triggered event (allow for negative sentinel)
    UInt_t fNumberOfEvents; //! Number of triggered events
    UInt_t fSamplesPerEvent; //! Number of sample per triggered event
    UInt_t fNumberOfSamples; //! Total number of samples in the buffer

    /* ADC sample data */
    UInt_t fSampleFormat;
    UInt_t fSamplePointer;
    std::vector<MQwSIS3320_Samples> fSamples;
    std::vector<MQwSIS3320_Samples> fSamplesRaw;

    /* ADC accumulator data */
    Int_t fNumberOfAccumulators;
    Int_t fAccumulatorDAC;
    Int_t fAccumulatorThreshold1, fAccumulatorThreshold2;
    Int_t fAccumulatorTimingBefore5, fAccumulatorTimingAfter5;
    Int_t fAccumulatorTimingBefore6, fAccumulatorTimingAfter6;
    std::vector<MQwSIS3320_Accumulator> fAccumulators;
    std::vector<MQwSIS3320_Accumulator> fAccumulatorsRaw;

    /* Sequence number */
    UInt_t fSequenceNumber;

    /**
     * Parity mock data generation parameters:
     * For the sampling ADC a standard pulse profile is assumed, and the
     * following parameters are used in that distribution.  The asymmetry
     * is applied on the integrated of the entire pulse.
     */
    Double_t fMockAsymmetry;
    Double_t fMockGaussianMean;
    Double_t fMockGaussianSigma;

    // Mode flags
    static const unsigned int MODE_ACCUMULATOR;
    static const unsigned int MODE_LONG_WORD_SAMPLING;
    static const unsigned int MODE_SHORT_WORD_SAMPLING;
    static const unsigned int MODE_NOTREADY;

    // Sampling mode format flags
    static const unsigned int FORMAT_MULTI_EVENT;
    static const unsigned int FORMAT_SINGLE_EVENT;

};

#endif // __MQwSIS3320_Channel__
