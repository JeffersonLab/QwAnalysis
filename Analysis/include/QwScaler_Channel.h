/**********************************************************\
* File: QwScaler_Channel.h                                 *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#ifndef __QWSCALER_CHANNEL__
#define __QWSCALER_CHANNEL__

// System headers
#include <vector>

// ROOT headers
#include "TTree.h"

// Qweak headers
#include "VQwHardwareChannel.h"
#include "MQwMockable.h"

///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class VQwScaler_Channel: public VQwHardwareChannel, public MQwMockable {

public:
  static Int_t GetBufferOffset(Int_t scalerindex, Int_t wordindex);
  static void  PrintErrorCounterHead();
  static void  PrintErrorCounterTail();

  using VQwHardwareChannel::GetRawValue;
  using VQwHardwareChannel::GetValue;
  using VQwHardwareChannel::GetValueM2;
  using VQwHardwareChannel::GetValueError;
  using VQwHardwareChannel::GetValueWidth;

public:
  VQwScaler_Channel(): MQwMockable() {
    InitializeChannel("","");
  }
    
  VQwScaler_Channel(TString name): MQwMockable() {
    InitializeChannel(name,"");
  };
  virtual ~VQwScaler_Channel() { };

  /// \brief Initialize the fields in this object
  void  InitializeChannel(TString name){InitializeChannel(name, "raw");};
  void  InitializeChannel(TString name, TString datatosave);

  /// \brief Initialize the fields in this object
  void  InitializeChannel(TString subsystem, TString instrumenttype, TString name, TString datatosave);

  void SetDefaultSampleSize(size_t NumberOfSamples_map) {
    //std::cerr << "QwScaler_Channel SetDefaultSampleSize does nothing!"
	  //    << std::endl;
  }

  void  ClearEventData();

  void ReportErrorCounters();//This will display the error summary for each device
  void UpdateHWErrorCount(){//Update error counter for HW faliure
    fNumEvtsWithHWErrors++;
  };

  void UpdateEventCutErrorCount(){//Update error counter for event cut faliure
    fNumEvtsWithEventCutsRejected++;
  };

  void  RandomizeEventData(int helicity = 0, double time = 0.0);
  void  SetEventData(Double_t value);

  virtual void  EncodeEventData(std::vector<UInt_t> &buffer) = 0;
  virtual Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index = 0) = 0;

  void  ProcessEvent();

  Int_t GetRawValue(size_t element) const      { return fValue_Raw; };
  Double_t GetValue(size_t element) const      { return fValue; };
  Double_t GetValueM2(size_t element) const    { return fValueM2; };
  Double_t GetValueError(size_t element) const { return fValueError; };
  UInt_t GetGoodEventCount() const { return fGoodEventCount; };

  VQwScaler_Channel& operator=  (const VQwScaler_Channel &value);
  void AssignValueFrom(const VQwDataElement* valueptr);
  //  VQwHardwareChannel& operator=  (const VQwHardwareChannel &data_value);
  VQwScaler_Channel& operator+= (const VQwScaler_Channel &value);
  VQwScaler_Channel& operator-= (const VQwScaler_Channel &value);
  void Sum(VQwScaler_Channel &value1, VQwScaler_Channel &value2);
  void Difference(VQwScaler_Channel &value1, VQwScaler_Channel &value2);
  void Ratio(const VQwScaler_Channel &numer, const VQwScaler_Channel &denom);
  void Product(VQwScaler_Channel &numer, VQwScaler_Channel &denom);
  void AddChannelOffset(Double_t Offset);
  void Scale(Double_t Offset);
  void ScaleRawRate(Double_t Offset);
  void DivideBy(const VQwScaler_Channel &denom);

  /// TODO The Normalize function should be replaced by DivideBy,
  ///      for consistency with the QwVQWK_Channel.
  void Normalize(const VQwScaler_Channel &norm){DivideBy(norm);};
  

  Int_t ApplyHWChecks(); //Check for harware errors in the devices. This will return the device error code.

  Bool_t ApplySingleEventCuts();//check values read from modules are at desired level

  /// report number of events falied due to HW and event cut failure
  Int_t GetEventcutErrorCounters();

//   UInt_t GetDeviceErrorCode(){//return the device error code
//     return fDeviceErrorCode;
//   };

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);

  void  ConstructBranch(TTree *tree, TString &prefix);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  void AccumulateRunningSum(const VQwScaler_Channel &value);
  void AccumulateRunningSum(const VQwHardwareChannel *value){
    const VQwScaler_Channel *tmp_ptr = dynamic_cast<const VQwScaler_Channel*>(value);
    if (tmp_ptr != NULL) AccumulateRunningSum(*tmp_ptr);
  };

  void Copy(VQwDataElement *source);

  void PrintValue() const;
  void PrintInfo() const;
  void CalculateRunningAverage();

  // These are related to those hardware channels that need to normalize
  // to an external clock
  virtual Bool_t NeedsExternalClock() { return fNeedsExternalClock; };
  virtual void SetNeedsExternalClock(Bool_t needed) { fNeedsExternalClock = needed; };
  virtual std::string GetExternalClockName() {  return fNormChannelName; };
  virtual void SetExternalClockPtr( const VQwDataElement* clock) { fNormChannelPtr = clock; };
  virtual void SetExternalClockName( const std::string name) { fNormChannelName = name; };

protected:
  VQwScaler_Channel& operator/=(const VQwScaler_Channel&);
  
protected:
  static const Bool_t kDEBUG;

  UInt_t   fValue_Raw;
  Double_t fValue;
  Double_t fValueM2;
  Double_t fValueError;
  const VQwDataElement *fNormChannelPtr;
  Double_t fClockNormalization;
  std::string fNormChannelName;
  Bool_t fNeedsExternalClock;

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  Int_t fNumEvtsWithHWErrors;//counts the HW falied events
  Int_t fNumEvtsWithEventCutsRejected;////counts the Event cut rejected events
};


//  Derived templated class
template <UInt_t data_mask=0xffffffff, UInt_t data_shift=0 >
class QwScaler_Channel: public VQwScaler_Channel
{
  public:

    // Define the constructors (cascade)
    QwScaler_Channel(): VQwScaler_Channel() { };
    QwScaler_Channel(TString name): VQwScaler_Channel(name) { };

  public:

  // Implement the templated methods
  void  EncodeEventData(std::vector<UInt_t> &buffer);
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index = 0);


};


//  These typedef's should be the last things in the file.
//  Class template instationation must be made in the source
//  file for anything defined here.
typedef class QwScaler_Channel<0x00ffffff,0> QwSIS3801D24_Channel;
typedef class QwScaler_Channel<0xffffffff,0> QwSIS3801D32_Channel;
typedef class QwScaler_Channel<0xffffffff,0>    QwSIS3801_Channel;
typedef class QwScaler_Channel<0xffffffff,0>    QwSTR7200_Channel;


#endif
