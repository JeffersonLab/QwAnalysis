/**********************************************************\
* File: QwVQWK_Channel.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QwVQWK_CHANNEL__
#define __QwVQWK_CHANNEL__

// System headers
#include <vector>

// ROOT headers
#include "TTree.h"

// Boost math library for random number generation
#include "boost/random.hpp"

// Qweak headers
#include "VQwDataElement.h"

// Forward declarations
class QwBlinder;

/// Flag to be used to decide which data needs to be histogrammed and
/// entered in the tree
enum EDataToSave {kRaw = 0, kDerived};


///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwVQWK_Channel: public VQwDataElement {
/****************************************************************//**
 *  Class: QwVQWK_Channel
 *         Base class containing decoding functions for the VQWK_Channel
 *         6 32-bit datawords.
 *         The functions in this class will decode a single channel
 *         worth of VQWK_Channel data and provide the components
 *         through member functions.
 ******************************************************************/
 public:
  static Int_t GetBufferOffset(Int_t moduleindex, Int_t channelindex);

 public:
  QwVQWK_Channel() {
    InitializeChannel("","");
  };

  QwVQWK_Channel(TString name, TString datatosave = "raw") {
    InitializeChannel(name, datatosave);
  };
  virtual ~QwVQWK_Channel() {
    //DeleteHistograms();
  };

  /// \brief Initialize the fields in this object
  void  InitializeChannel(TString name, TString datatosave);

  /// \brief Initialize the fields in this object
  void  InitializeChannel(TString subsystem, TString instrumenttype, TString name, TString datatosave);

  // Will update the default sample size for the module.
  void SetDefaultSampleSize(size_t NumberOfSamples_map) {
    // This will be checked against the no.of samples read by the module
    fNumberOfSamples_map = NumberOfSamples_map;
  };
  


  void  ClearEventData();

  void ReportErrorCounters();//This will display the error summary for each device


  void UpdateEventCutErrorCount(){//Update error counter for event cut faliure
    fNumEvtsWithEventCutsRejected++;
  }

  /// \name Parity mock data generation
  // @{
  /// Set a single set of harmonic drift parameters
  void  SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  /// Add drift parameters to the internal set
  void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  /// Set the normal random event parameters
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  /// Set the helicity asymmetry
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  /// Internally generate random event data
  void  RandomizeEventData(int helicity = 0.0, double time = 0.0);
  /// Set the flag to use an externally provided random variable
  void  UseExternalRandomVariable() { fUseExternalRandomVariable = true; };
  /// Set the externally provided random variable
  void  SetExternalRandomVariable(Double_t random_variable) {
    fUseExternalRandomVariable = true;
    fExternalRandomVariable = random_variable;
  };
  // @}

  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  void  SetEventData(Double_t* block, UInt_t sequencenumber = 0);



  /// Encode the event data into a CODA buffer
  void  EncodeEventData(std::vector<UInt_t> &buffer);
  /// Decode the event data from a CODA buffer
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index = 0);
  /// Process the event data according to pedestal and calibration factor
  void  ProcessEvent();


  QwVQWK_Channel& operator=  (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator+= (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator-= (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator*= (const QwVQWK_Channel &value);
  const QwVQWK_Channel operator+ (const QwVQWK_Channel &value) const;
  const QwVQWK_Channel operator- (const QwVQWK_Channel &value) const;
  const QwVQWK_Channel operator* (const QwVQWK_Channel &value) const;
  void Sum(QwVQWK_Channel &value1, QwVQWK_Channel &value2);
  void Difference(QwVQWK_Channel &value1, QwVQWK_Channel &value2);
  void Ratio(QwVQWK_Channel &numer, QwVQWK_Channel &denom);
  void Product(QwVQWK_Channel &value1, QwVQWK_Channel &value2);

  void AddChannelOffset(Double_t Offset);
  void Scale(Double_t Offset);

  void AccumulateRunningSum(const QwVQWK_Channel& value);
  void CalculateRunningAverage();

  Bool_t MatchSequenceNumber(size_t seqnum);
  Bool_t MatchNumberOfSamples(size_t numsamp);

  /*Event cut related routines*/
  Bool_t ApplySingleEventCuts(Double_t LL,Double_t UL);//check values read from modules are at desired level
  Bool_t ApplySingleEventCuts();//check values read from modules are at desired level by comparing upper and lower limits (fULimit and fLLimit) set on this channel
  void SetSingleEventCuts(Double_t min, Double_t max);//set the upper and lower limits (fULimit and fLLimit) set on this channel
  Int_t GetEventcutErrorCounters();// report number of events failed due to HW and event cut faliure
  Int_t GetEventcutErrorFlag(){//return the error flag
    return fDeviceErrorCode;
  };
  Double_t GetEventCutUpperLimit(){
    return fULimit;
  }
  Double_t GetEventCutLowerLimit(){
    return fLLimit;
  }

  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
  }

  Int_t ApplyHWChecks(); //Check for harware errors in the devices. This will return the device error code.

  void UpdateHWErrorCounters(Int_t error_flag);//update counters based on the flag passed to it
  void UpdateHWErrorCounters(){//update the counters based on the this->fDeviceErrorCode
    UpdateHWErrorCounters(fDeviceErrorCode);
  };
  /*End*/

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  Double_t GetBlockValue(size_t blocknum) const { return fBlock[blocknum]; };
  Double_t GetBlockErrorValue(size_t blocknum) const { return fBlockError[blocknum]; };
  Double_t GetHardwareSum() const       { return fHardwareBlockSum; };
  Double_t GetHardwareSumM2() const     { return fHardwareBlockSumM2; };
  Double_t GetHardwareSumError() const  { return fHardwareBlockSumError; };
  Double_t GetAverageVolts() const;
  //  Double_t GetSoftwareSum() const {return fSoftwareBlockSum;};

  Double_t GetRawBlockValue(size_t blocknum) const {return fBlock_raw[blocknum];};
  Double_t GetRawHardwareSum() const {return fHardwareBlockSum_raw;};
  Double_t GetRawSoftwareSum() const {return fSoftwareBlockSum_raw;};

  size_t GetSequenceNumber() const {return (fSequenceNumber);};
  size_t GetNumberOfSamples() const {return (fNumberOfSamples);};

  void     SetPedestal(Double_t ped) { fPedestal = ped; kFoundPedestal = 1; };
  Double_t GetPedestal() const       { return fPedestal; };
  void     SetCalibrationFactor(Double_t factor) { fCalibrationFactor = factor; kFoundGain = 1; };
  void     SetCalibrationToVolts()               { fCalibrationFactor = kVQWK_VoltsPerBit; kFoundGain = 1; };
  Double_t GetCalibrationFactor() const          { return fCalibrationFactor; };

  void Copy(VQwDataElement *source);

  friend std::ostream& operator<< (std::ostream& stream, const QwVQWK_Channel& channel);
  void PrintValue() const;
  void PrintInfo() const;

  Double_t GetAverage()      const { return fHardwareBlockSum; };
  Double_t GetAverageError() const { return fHardwareBlockSumError; };
  UInt_t GetGoodEventCount() const { return fGoodEventCount; };

  /// \brief Blind this channel as an asymmetry
  void Blind(const QwBlinder *blinder);
  /// \brief Blind this channel as a difference
  void Blind(const QwBlinder *blinder, const QwVQWK_Channel& yield);

  

 protected:


 private:
  static const Bool_t kDEBUG;
  static const Int_t  kWordsPerChannel; //no.of words per channel in the CODA buffer
  static const Int_t  kMaxChannels;     //no.of channels per module

  Int_t fDataToSave;

  /*! \name ADC Calibration                    */
  // @{
  static const Double_t kVQWK_VoltsPerBit;
  Double_t fPedestal; /*!< Pedestal of the hardware sum signal,
			   we assume the pedestal level is constant over time
			   and can be divided by four for use with each block,
			   units: [counts / number of samples] */
  Double_t fCalibrationFactor;
  Bool_t kFoundPedestal;
  Bool_t kFoundGain;
  //@}


  /*! \name Channel information data members   */

  /*! \name Channel configuration data members */
  // @{

  //UInt_t  fBlocksPerEvent;
  Short_t fBlocksPerEvent;
  // @}

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  /*! \name Event data members---Raw values */
  // @{
  Double_t fBlock_raw[4];      ///< Array of the sub-block data as read from the module
  Double_t fHardwareBlockSum_raw; ///< Module-based sum of the four sub-blocks as read from the module
  Double_t fSoftwareBlockSum_raw; ///< Sum of the data in the four sub-blocks raw
  /*! \name Event data members---Potentially calibrated values*/
  // @{
  // The following values potentially have pedestal removed  and calibration applied
  Double_t fBlock[4];          ///< Array of the sub-block data
  Double_t fHardwareBlockSum;  ///< Module-based sum of the four sub-blocks
  // @}


  /// \name Calculation of the statistical moments
  // @{
  // Moments of the separate blocks
  Double_t fBlockM2[4];        ///< Second moment of the sub-block
  Double_t fBlockError[4];     ///< Uncertainty on the sub-block
  // Moments of the hardware sum
  Double_t fHardwareBlockSumM2;    ///< Second moment of the hardware sum
  Double_t fHardwareBlockSumError; ///< Uncertainty on the hardware sum
  // @}


  size_t fSequenceNumber;      ///< Event sequence number for this channel
  size_t fPreviousSequenceNumber; ///< Previous event sequence number for this channel
  size_t fNumberOfSamples;     ///< Number of samples  read through the module
  size_t fNumberOfSamples_map; ///< Number of samples in the expected to  read through the module. This value is set in the QwBeamline map file


  /// \name Parity mock data generation
  // @{
  /// Internal randomness generator
  static boost::mt19937 fRandomnessGenerator;
  /// Internal normal probability distribution
  static boost::normal_distribution<double> fNormalDistribution;
  /// Internal normal random variable
  static boost::variate_generator
    < boost::mt19937, boost::normal_distribution<double> > fNormalRandomVariable;
  /// Flag to use an externally provided normal random variable
  bool fUseExternalRandomVariable;
  /// Externally provided normal random variable
  double  fExternalRandomVariable;

  // Parameters of the mock data
  Double_t fMockAsymmetry;     ///< Helicity asymmetry
  Double_t fMockGaussianMean;  ///< Mean of normal distribution
  Double_t fMockGaussianSigma; ///< Sigma of normal distribution
  std::vector<Double_t> fMockDriftAmplitude; ///< Harmonic drift amplitude
  std::vector<Double_t> fMockDriftFrequency; ///< Harmonic drift frequency
  std::vector<Double_t> fMockDriftPhase;     ///< Harmonic drift phase
  // @}


  Int_t fNumEvtsWithEventCutsRejected;/*! Counts the Event cut rejected events */

  // Set of error counters for each HW test.
  Int_t fErrorCount_sample;   // for sample size check
  Int_t fErrorCount_SW_HW;    // HW_sum==SW_sum check
  Int_t fErrorCount_Sequence; // sequence number check
  Int_t fErrorCount_SameHW;   // check to see ADC returning same HW value
  Int_t fErrorCount_ZeroHW;   // check to see ADC returning zero


  static const Int_t kErrorFlag_sample     = 0x2;  // in Decimal 2   for sample size check
  static const Int_t kErrorFlag_SW_HW      = 0x4;  // in Decimal 4   HW_sum==SW_sum check
  static const Int_t kErrorFlag_Sequence   = 0x8;  // in Decimal 8   sequence number check
  static const Int_t kErrorFlag_SameHW     = 0x10; // in Decimal 16  check to see ADC returning same HW value
  static const Int_t kErrorFlag_ZeroHW     = 0x20; // in Decimal 32  check to see ADC returning zero
  static const Int_t kErrorFlag_EventCut_L = 0x40; // in Decimal 64  check to see ADC failed upper limit of the event cut
  static const Int_t kErrorFlag_EventCut_U = 0x80; // in Decimal 128 check to see ADC failed upper limit of the event cut



  Int_t fDeviceErrorCode; ///< Unique error code for HW failed beam line devices


  Int_t fADC_Same_NumEvt; ///< Keep track of how many events with same ADC value returned
  Int_t fSequenceNo_Prev; ///< Keep the sequence number of the last event
  Int_t fSequenceNo_Counter; ///< Internal counter to keep track of the sequence number
  Double_t fPrev_HardwareBlockSum; ///< Previous Module-based sum of the four sub-blocks

  UInt_t fGoodEventCount;//counts the HW and event check passed events

  Int_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts are OFF
  Double_t fULimit, fLLimit;//this sets the upper and lower limits on the VQWK_Channel::fHardwareBlockSum

  /*
  //debug- Ring analysis
  Int_t fEventCounter;
  Int_t fTripCounter;
  Bool_t bTrip;
  */


  const static Bool_t bDEBUG=kFALSE;//debugging display purposes

  //For VQWK data element trimming uses
  Bool_t bHw_sum;
  Bool_t bHw_sum_raw;
  Bool_t  bBlock;
  Bool_t  bBlock_raw;
  Bool_t bNum_samples;
  Bool_t bDevice_Error_Code;
  Bool_t bSequence_number;
  



};



#endif
