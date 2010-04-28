/**********************************************************\
* File: QwVQWK_Channel.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QwVQWK_CHANNEL__
#define __QwVQWK_CHANNEL__

#include <vector>
#include "TTree.h"

// Boost math library for random number generation
#include "boost/random.hpp"

// Qweak headers
#include "VQwDataElement.h"
#include "QwBlinder.h"

enum EDataToSave{kRaw=0, kDerived};
// this data is used to decided which data need to be histogrammed or ttree-ed


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
  QwVQWK_Channel() { };

  QwVQWK_Channel(TString name, TString datatosave="raw"){
    InitializeChannel(name, datatosave);
  };
  ~QwVQWK_Channel() {
  //DeleteHistograms();
  };

  void  InitializeChannel(TString name, TString datatosave);

  void SetDefaultSampleSize(size_t NumberOfSamples_map){ //Will update the default sample size for the module.
    fNumberOfSamples_map=NumberOfSamples_map;//this will be checked against the no.of samples read by the module
  };


  void  ClearEventData();

  void ReportErrorCounters();//This will display the error summary for each device


  void UpdateEventCutErrorCount(){//Update error counter for event cut faliure
    fNumEvtsWithEventCutsRejected++;
  }

  void  SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity);
  void  SetEventNumber(int event) {fEventNumber = event;};
  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  void  SetEventData(Double_t* block, UInt_t sequencenumber = 0);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left,UInt_t index=0);
  void  ProcessEvent();

  QwVQWK_Channel& operator=  (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator+= (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator-= (const QwVQWK_Channel &value);
  void Sum(QwVQWK_Channel &value1, QwVQWK_Channel &value2);
  void Difference(QwVQWK_Channel &value1, QwVQWK_Channel &value2);
  void Ratio(QwVQWK_Channel &numer, QwVQWK_Channel &denom);
  void Product(QwVQWK_Channel &value1, QwVQWK_Channel &value2);

  void Offset(Double_t Offset);
  void Scale(Double_t Offset);
  void Calculate_Running_Average();//pass the current event count in the run to calculate running average
  void Print_Running_Average();
  void Do_RunningSum();

  Bool_t MatchSequenceNumber(size_t seqnum);
  Bool_t MatchNumberOfSamples(size_t numsamp);

  /*Event cut related routines*/
  Bool_t ApplySingleEventCuts(Double_t LL,Double_t UL);//check values read from modules are at desired level
  Bool_t ApplySingleEventCuts();//check values read from modules are at desired level by comparing upper and lower limits (fULimit and fLLimit) set on this channel 
  void SetSingleEventCuts(Double_t min, Double_t max);//set the upper and lower limits (fULimit and fLLimit) set on this channel 
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure
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

  /*End*/

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);

  Double_t GetBlockValue(size_t blocknum){return fBlock[blocknum];};
  Double_t GetHardwareSum(){return fHardwareBlockSum;};
  Double_t GetAverageVolts();
  //  Double_t GetSoftwareSum(){return fSoftwareBlockSum;};

  Double_t GetRawBlockValue(size_t blocknum){return fBlock_raw[blocknum];};
  Double_t GetRawHardwareSum(){return fHardwareBlockSum_raw;};
  Double_t GetRawSoftwareSum(){return fSoftwareBlockSum_raw;};

  size_t GetSequenceNumber(){return (fSequenceNumber);};
  size_t GetNumberOfSamples(){return (fNumberOfSamples);};

  void SetPedestal(Double_t ped){fPedestal=ped; return;};
  Double_t GetPedestal(){return fPedestal;};
  void SetCalibrationFactor(Double_t factor){fCalibrationFactor=factor; return;};
  Double_t GetCalibrationFactor(){return fCalibrationFactor;};

  
  void Copy(VQwDataElement *source);

  void Print() const;
  Double_t GetAverage() {return fAverage_n;};
  Double_t GetAverageError() {return fAverage_error;};
  void BlindMe(QwBlinder *blinder);

 protected:


 private:
  static const Bool_t kDEBUG;

  // Randomness generator
  static boost::mt19937 fRandomnessGenerator;
  static boost::normal_distribution<double> fNormalDistribution;
  static boost::variate_generator
    < boost::mt19937, boost::normal_distribution<double> > fNormalRandomVariable;

  Int_t fDataToSave;

  /*  ADC Calibration                     */
  static const Double_t kVQWK_VoltsPerBit;
  Double_t fPedestal;         /*! pedestal of the hardware sum signal,
			     we assume the pedestal level is constant over time
			     and can be divided by four for use with each block,
			     units: [counts/number of Sample] */
  Double_t fCalibrationFactor;

  /*  Channel information data members    */


  /*  Channel configuration data members */
  UInt_t  fSamplesPerBlock;
  UInt_t  fBlocksPerEvent;

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  /*  Event data members */
  Double_t fBlock_raw[4];     /*! Array of the sub-block data as read from the module */
  Double_t fHardwareBlockSum_raw; /*! Module-based sum of the four sub-blocks as read from the module  */
  Double_t fSoftwareBlockSum_raw; /*! Sum of the data in the four sub-blocks raw */
  /* the following values potentially have pedestal removed  and calibration applied */
  Double_t fBlock[4];         /*! Array of the sub-block data             */
  Double_t fHardwareBlockSum; /*! Module-based sum of the four sub-blocks */

  size_t fSequenceNumber;     /*! Event sequence number for this channel  */
  size_t fPreviousSequenceNumber; /*! Previous event sequence number for this channel  */
  size_t fNumberOfSamples;    /*! Number of samples  read through the module        */
  size_t fNumberOfSamples_map;    /*! Number of samples in the expected to  read through the module. This value is set in the QwBeamline map file     */

  size_t fEventNumber;

  /// \name Parity mock data distributions
  // @{
  Double_t fMockAsymmetry;	///< Helicity asymmetry
  Double_t fMockGaussianMean;	///< Mean of gaussian distribution
  Double_t fMockGaussianSigma;	///< Sigma of gaussian distribution
  std::vector<Double_t> fMockDriftAmplitude;	///< Drift amplitude
  std::vector<Double_t> fMockDriftFrequency;	///< Drift frequency
  std::vector<Double_t> fMockDriftPhase;	///< Drift phase
  // @}


  Int_t fNumEvtsWithEventCutsRejected;/*! Counts the Event cut rejected events */

  //set of error counters for each HW test.

  Int_t fErrorCount_sample;//for sample size check
  Int_t fErrorCount_SW_HW;//HW_sum==SW_sum check
  Int_t fErrorCount_Sequence;//sequence number check
  Int_t fErrorCount_SameHW;//check to see ADC returning same HW value
  Int_t fErrorCount_ZeroHW;//check to see ADC returning zero


  static const Int_t kErrorFlag_sample=0x2;   // in Decimal 2.  for sample size check
  static const Int_t kErrorFlag_SW_HW=0x4;    // in Decimal 4.  HW_sum==SW_sum check
  static const Int_t kErrorFlag_Sequence=0x8; // in Decimal 8.  sequence number check
  static const Int_t kErrorFlag_SameHW=0x10;   //in Decimal 16.  check to see ADC returning same HW value
  static const Int_t kErrorFlag_ZeroHW=0x20;   //in Decimal 32.  check to see ADC returning zero
  static const Int_t kErrorFlag_EventCut_L=0x40;   //in Decimal 64  check to see ADC falied upper limit of the event cut
  static const Int_t kErrorFlag_EventCut_U=0x80;   //in Decimal 128  check to see ADC falied upper limit of the event cut



  Int_t fDeviceErrorCode;/*! Unique error code for HW failed beam line devices */


  Int_t fADC_Same_NumEvt;/*! Keep track of how many events with same ADC value returned.*/
  Int_t fSequenceNo_Prev;/* ! Keep the sequence number of the last event */
  Int_t fSequenceNo_Counter;/* ! Internal counter to keep track of the sequence number */
  Double_t fPrev_HardwareBlockSum;/*! Previos Module-based sum of the four sub-blocks */

  Double_t fRunning_sum;//Running sum for the device
  Double_t fRunning_sum_square;//Running sum square for the device
  Double_t fAverage_n;/* Running average for the device !*/
  Double_t fAverage_n_square;/* Running average square for the device !*/
  Double_t fAverage_error;
  Int_t fGoodEventCount;//counts the HW and event check passed events

  Int_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts are OFF
  Double_t fULimit, fLLimit;//this sets the upper and lower limits on the VQWK_Channel::fHardwareBlockSum

  /*
  //debug- Ring analysis
  Int_t fEventCounter;
  Int_t fTripCounter;
  Bool_t bTrip;
  */


  const static Bool_t bDEBUG=kFALSE;//debugging display purposes



};


#endif
