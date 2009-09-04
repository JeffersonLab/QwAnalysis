/**
 * \class	MQwSIS330	MQwSIS3320.h
 *
 * \brief	Class for the decoding of SIS3320 sampling ADC data buffers
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

#ifndef __MQwSIS3320__
#define __MQwSIS3320__

#include <iostream>
#include <vector>
#include "TTree.h"

// Boost math library for random number generation
#include "boost/random.hpp"

#include "VQwDataElement.h"


/**
 * \class	MQwSIS330_Accumulator
 *
 * \brief	SIS3320 sampling ADC accumulator
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The MQwSIS330_Accumulator should allow convenient access to the accumulator
 * data collected with the SIS3320 for the Compton photon detector.  At some
 * point it will be expanded to have its own sum, difference, and ratio methods
 * but until then it is quite featureless and serves mostly to make reading of
 * data from the CODA buffer easier.
 *
 */
class MQwSIS3320_Accumulator {

  public:

    MQwSIS3320_Accumulator() {
      fMinValue = 0; fMaxValue = 0;
      fMinTime = 0; fMaxTime = 0;
      fData.clear();
    };
    ~MQwSIS3320_Accumulator() { };

    const Int_t GetMinValue() const { return fMinValue; };
    const Int_t GetMaxValue() const { return fMaxValue; };
    const Int_t GetMinTime() const { return fMinTime; };
    const Int_t GetMaxTime() const { return fMaxTime; };

    const Int_t GetSum() const { return fData.at(INDEX_SUM); };
    const Int_t GetNumberOfSamples() const { return fData.at(INDEX_N); };

    // Output stream operator<< for an accumulator
    friend std::ostream& operator<< (std::ostream& stream, const MQwSIS3320_Accumulator& a);

    // Define operator+=, operator-=, etc, for accumulator asymmetries

  protected:

    std::vector<Int_t> fData; //! Accumulator data buffer

  private:

    int fMinValue, fMaxValue; //! Value-based accumulator limits
    int fMinTime, fMaxTime; //! Time-based accumulator limits

    static const unsigned int INDEX_N;
    static const unsigned int INDEX_SUM;

};

// Fields in accumulator data buffer
const unsigned int MQwSIS3320_Accumulator::INDEX_N = 0;
const unsigned int MQwSIS3320_Accumulator::INDEX_SUM = 1;

// Output stream operator<< for the accumulators
inline std::ostream& operator<< (std::ostream& stream, const MQwSIS3320_Accumulator& a) {
  stream << a.GetSum() << " (" << a.GetNumberOfSamples() << ")";
  return stream;
};


class MQwSIS3320: public VQwDataElement {

  public:

    MQwSIS3320() { };
    MQwSIS3320(TString name) {
      InitializeChannel(name);
    };
    ~MQwSIS3320() {
      DeleteHistograms();
    };

    void  InitializeChannel(TString name);

    void  ClearEventData();

    void  SetRandomEventParameters(Double_t mean, Double_t sigma) { };
    void  SetRandomEventAsymmetry(Double_t asymmetry) { };
    void  RandomizeEventData(int helicity) { };
    void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0) { };
    void  SetEventData(Double_t* block, UInt_t sequencenumber = 0) { };
    void  EncodeEventData(std::vector<UInt_t> &buffer);

    Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index = 0);
    void  ProcessEvent();

    MQwSIS3320& operator=  (const MQwSIS3320 &value);
    MQwSIS3320& operator+= (const MQwSIS3320 &value);
    MQwSIS3320& operator-= (const MQwSIS3320 &value);
    void Sum(MQwSIS3320 &value1, MQwSIS3320 &value2);
    void Difference(MQwSIS3320 &value1, MQwSIS3320 &value2);
    void Ratio(MQwSIS3320 &numer, MQwSIS3320 &denom) { };
    void Offset(Double_t Offset);
    void Scale(Double_t Offset);

    Bool_t MatchSequenceNumber(size_t seqnum);
    Bool_t MatchNumberOfSamples(size_t numsamp);

    void  ConstructHistograms(TDirectory *folder, TString &prefix) { };
    void  FillHistograms() { };

    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values) { };
    void  FillTreeVector(std::vector<Double_t> &values) { };

    Double_t GetSampleValue(size_t sample) { return fSamples.at(sample); };
    Double_t GetSampleValueRaw(size_t sample) { return fSamplesRaw.at(sample); };
    Double_t GetSampleSum() { return fSampleSum; };
    Double_t GetSampleSumRaw() { return fSampleSumRaw; };

    size_t GetSequenceNumber() { return (fSequenceNumber); };
    size_t GetNumberOfSamples() { return (fNumberOfSamples); };

    void SetPedestal(Double_t ped) {fPedestal = ped; return; };
    Double_t GetPedestal() { return fPedestal; };
    void SetCalibrationFactor(Double_t factor) { fCalibrationFactor=factor; return; };
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
    std::vector<Int_t> fSamples;
    std::vector<Int_t> fSamplesRaw;
    Int_t fSampleSum;
    Int_t fSampleSumRaw;

    /* ADC accumulator data */
    Int_t fNumberOfAccumulators;
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

// Initialize mode flags
const unsigned int MQwSIS3320::MODE_ACCUMULATOR = 0x0;
const unsigned int MQwSIS3320::MODE_LONG_WORD_SAMPLING = 0x1;
const unsigned int MQwSIS3320::MODE_SHORT_WORD_SAMPLING = 0x2;
const unsigned int MQwSIS3320::MODE_NOTREADY = 0xda00;
// Initialize sampling mode format flags
const unsigned int MQwSIS3320::FORMAT_MULTI_EVENT = 0x3;
const unsigned int MQwSIS3320::FORMAT_SINGLE_EVENT = 0x4;



/**
 * \class	MQwSIS330_Module
 *
 * \brief	Combination of 8 channels of SIS3320 sampling ADC data
 *
 * \author	W. Deconinck
 * \date	2009-09-04 18:06:23
 * \ingroup	QwCompton
 *
 * The MQwSIS3320_Module class is defined as 8 MQwSIS330 channels.  This
 * class helps keep track of which channels are combined together in one
 * module and therefore read out in the same subbank.
 *
 */

/** \def SIS3320_CHANNELS Number of channels in a SIS3320 module */
#define SIS3320_CHANNELS 8

class MQwSIS3320_Module: std::vector<MQwSIS3320> {

  public:

    MQwSIS3320_Module() { resize(SIS3320_CHANNELS); }; // create the channels

    /**
     * Print some debugging information about the MQwSIS3320 module
     */
    void Print() {
      for (std::vector<MQwSIS3320>::iterator channel = begin(); channel != end(); channel++)
        channel->Print();
    };

  protected:

  private:

};

#endif
