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

// Qweak headers
#include "VQwHardwareChannel.h"
#include "MQwMockable.h"

// Forward declarations
class QwBlinder;
class QwParameterFile;
class QwErrDBInterface;

///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwVQWK_Channel: public VQwHardwareChannel, public MQwMockable {
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
  static void  PrintErrorCounterHead();
  static void  PrintErrorCounterTail();

  using VQwHardwareChannel::GetRawValue;
  using VQwHardwareChannel::GetValue;
  using VQwHardwareChannel::GetValueM2;
  using VQwHardwareChannel::GetValueError;
  using VQwHardwareChannel::GetValueWidth;

 public:
  QwVQWK_Channel(): MQwMockable() {
    InitializeChannel("","");
    SetVQWKSaturationLimt(8.5);//set the default saturation limit
  };

  QwVQWK_Channel(TString name, TString datatosave = "raw"): MQwMockable() {
    InitializeChannel(name, datatosave);
    SetVQWKSaturationLimt(8.5);//set the default saturation limit
  };
  virtual ~QwVQWK_Channel() { };

  /// \brief Initialize the fields in this object
  void  InitializeChannel(TString name, TString datatosave);

  /// \brief Initialize the fields in this object
  void  InitializeChannel(TString subsystem, TString instrumenttype, TString name, TString datatosave);

  void LoadChannelParameters(QwParameterFile &paramfile);

  // Will update the default sample size for the module.
  void SetDefaultSampleSize(size_t num_samples_map) {
    // This will be checked against the no.of samples read by the module
    fNumberOfSamples_map = num_samples_map;
  };
  
  void  ClearEventData();

  void ReportErrorCounters();//This will display the error summary for each device


  void UpdateEventCutErrorCount(){//Update error counter for event cut faliure

  }

  /// Internally generate random event data
  void  RandomizeEventData(int helicity = 0.0, double time = 0.0);

  ///  TODO:  SetHardwareSum should be removed, and SetEventData
  ///         should be made protected.
  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  void  SetEventData(Double_t* block, UInt_t sequencenumber = 0);



  /// Encode the event data into a CODA buffer
  void  EncodeEventData(std::vector<UInt_t> &buffer);
  /// Decode the event data from a CODA buffer
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index = 0);
  /// Process the event data according to pedestal and calibration factor
  void  ProcessEvent();


  QwVQWK_Channel& operator=  (const QwVQWK_Channel &value);
  //  VQwHardwareChannel& operator=  (const VQwHardwareChannel &value);
  void AssignScaledValue(const QwVQWK_Channel &value, Double_t scale);
  void AssignValueFrom(const VQwDataElement* valueptr);

  QwVQWK_Channel& operator+= (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator-= (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator*= (const QwVQWK_Channel &value);
  const QwVQWK_Channel operator+ (const QwVQWK_Channel &value) const;
  const QwVQWK_Channel operator- (const QwVQWK_Channel &value) const;
  const QwVQWK_Channel operator* (const QwVQWK_Channel &value) const;
  void Sum(const QwVQWK_Channel &value1, const QwVQWK_Channel &value2);
  void Difference(const QwVQWK_Channel &value1, const QwVQWK_Channel &value2);
  void Ratio(const QwVQWK_Channel &numer, const QwVQWK_Channel &denom);
  void Product(const QwVQWK_Channel &value1, const QwVQWK_Channel &value2);
  void DivideBy(const QwVQWK_Channel& denom);
  void AddChannelOffset(Double_t Offset);
  void Scale(Double_t Offset);

  void AccumulateRunningSum(const QwVQWK_Channel& value);
  void AccumulateRunningSum(const VQwHardwareChannel *value){
    const QwVQWK_Channel *tmp_ptr = dynamic_cast<const QwVQWK_Channel*>(value);
    if (tmp_ptr != NULL) AccumulateRunningSum(*tmp_ptr);
  };
  ////deaccumulate one value from the running sum
  void DeaccumulateRunningSum(QwVQWK_Channel& value){
    value.fGoodEventCount=-1;
    AccumulateRunningSum(value);
    value.fGoodEventCount=0;
  };
  /*
  void DeaccumulateRunningSum(VQwHardwareChannel *value){
    const QwVQWK_Channel *tmp_ptr = dynamic_cast<const QwVQWK_Channel*>(value);
    if (tmp_ptr != NULL) DeaccumulateRunningSum(*tmp_ptr);
  };
  */

  void CalculateRunningAverage();

  Bool_t MatchSequenceNumber(size_t seqnum);
  Bool_t MatchNumberOfSamples(size_t numsamp);

  /*Event cut related routines*/
  Bool_t ApplySingleEventCuts(Double_t LL,Double_t UL);//check values read from modules are at desired level
  Bool_t ApplySingleEventCuts();//check values read from modules are at desired level by comparing upper and lower limits (fULimit and fLLimit) set on this channel
  Int_t GetEventcutErrorCounters();// report number of events failed due to HW and event cut faliure

  void SetVQWKSaturationLimt(Double_t sat_volts=8.5){//Set the absolute staturation limit in volts.
    fSaturationABSLimit=sat_volts;
  }

  Double_t GetVQWKSaturationLimt(){//Get the absolute staturation limit in volts.
    return fSaturationABSLimit;
  }


  Int_t ApplyHWChecks(); //Check for harware errors in the devices. This will return the device error code.

  void UpdateErrorCounters(UInt_t error_flag);//update counters based on the flag passed to it
    
  
  /*End*/

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  Int_t GetRawValue(size_t element) const {
    RangeCheck(element);
    if (element==0) return fHardwareBlockSum_raw;
    return fBlock_raw[element-1];
  }
  Double_t GetValue(size_t element) const {
    RangeCheck(element);
    if (element==0) return fHardwareBlockSum;
    return fBlock[element-1];
  }
  Double_t GetValueM2(size_t element) const {
    RangeCheck(element);
    if (element==0) return fHardwareBlockSumM2;
    return fBlockM2[element-1];
  }
  Double_t GetValueError(size_t element) const {
    RangeCheck(element);
    if (element==0) return fHardwareBlockSumError;
    return fBlockError[element-1];
  }


  Double_t GetAverageVolts() const;

  size_t GetSequenceNumber() const {return (fSequenceNumber);};
  size_t GetNumberOfSamples() const {return (fNumberOfSamples);};

  void   SetCalibrationToVolts(){SetCalibrationFactor(kVQWK_VoltsPerBit);};

  void Copy(const VQwDataElement *source);

  friend std::ostream& operator<< (std::ostream& stream, const QwVQWK_Channel& channel);
  void PrintValue() const;
  void PrintInfo() const;

  Double_t GetAverage()      const { return fHardwareBlockSum; };
  Double_t GetAverageError() const { return fHardwareBlockSumError; };


  /// \brief Blind this channel as an asymmetry
  void Blind(const QwBlinder *blinder);
  /// \brief Blind this channel as a difference
  void Blind(const QwBlinder *blinder, const QwVQWK_Channel& yield);

  // Error Counters exist in QwVQWK_Channel, not in VQwHardwareChannel
  //
  void AddErrEntriesToList(std::vector<QwErrDBInterface> &row_list);

 protected:
  QwVQWK_Channel& operator/= (const QwVQWK_Channel &value);

private:
  //  The following specific access methods should only be used internally,
  //  if at all.
  Double_t GetBlockValue(size_t blocknum) const { return GetValue(blocknum+1);};
  Double_t GetBlockErrorValue(size_t blocknum) const { return GetValueError(blocknum+1);};

  Double_t GetHardwareSum() const       { return GetValue(0);};
  Double_t GetHardwareSumM2() const     { return GetValueM2(0); };
  Double_t GetHardwareSumWidth() const  { return GetValueWidth(0); };
  Double_t GetHardwareSumError() const  { return GetValueError(0); };
  //  Double_t GetSoftwareSum() const {return fSoftwareBlockSum;};

  Int_t GetRawBlockValue(size_t blocknum) const {return GetRawValue(blocknum+1);};
  Int_t GetRawHardwareSum() const       { return GetRawValue(0);};
  Int_t GetRawSoftwareSum() const {return fSoftwareBlockSum_raw;};


 private:
  static const Bool_t kDEBUG;
  static const Int_t  kWordsPerChannel; //no.of words per channel in the CODA buffer
  static const Int_t  kMaxChannels;     //no.of channels per module


  /*! \name ADC Calibration                    */
  // @{
  static const Double_t kVQWK_VoltsPerBit;
  //@}


  /*! \name Channel information data members   */

  /*! \name Channel configuration data members */
  // @{

  //UInt_t  fBlocksPerEvent;
  Short_t fBlocksPerEvent;
  // @}


  /*! \name Event data members---Raw values */
  // @{
  Int_t fBlock_raw[4];      ///< Array of the sub-block data as read from the module
  Int_t fHardwareBlockSum_raw; ///< Module-based sum of the four sub-blocks as read from the module
  Int_t fSoftwareBlockSum_raw; ///< Sum of the data in the four sub-blocks raw
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

 

  // Set of error counters for each HW test.
  Int_t fErrorCount_HWSat;    ///< check to see ADC channel is saturated 
  Int_t fErrorCount_sample;   ///< for sample size check                 
  Int_t fErrorCount_SW_HW;    ///< HW_sum==SW_sum check
  Int_t fErrorCount_Sequence; ///< sequence number check
  Int_t fErrorCount_SameHW;   ///< check to see ADC returning same HW value
  Int_t fErrorCount_ZeroHW;   ///< check to see ADC returning zero

  Int_t fNumEvtsWithEventCutsRejected; ///< Counts the Event cut rejected events 






  Int_t fADC_Same_NumEvt; ///< Keep track of how many events with same ADC value returned
  Int_t fSequenceNo_Prev; ///< Keep the sequence number of the last event
  Int_t fSequenceNo_Counter; ///< Internal counter to keep track of the sequence number
  Double_t fPrev_HardwareBlockSum; ///< Previous Module-based sum of the four sub-blocks



  Double_t fSaturationABSLimit;///<absolute value of the VQWK saturation volt


  const static Bool_t bDEBUG=kFALSE;///<debugging display purposes

  ///<For VQWK data element trimming uses
  Bool_t bHw_sum;
  Bool_t bHw_sum_raw;
  Bool_t  bBlock;
  Bool_t  bBlock_raw;
  Bool_t bNum_samples;
  Bool_t bDevice_Error_Code;
  Bool_t bSequence_number;

private:
  
  




};



#endif
