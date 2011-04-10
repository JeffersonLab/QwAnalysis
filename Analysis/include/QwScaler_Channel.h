/**********************************************************\
* File: QwScaler_Channel.h                                 *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#ifndef __QWSCALER_CHANNEL__
#define __QWSCALER_CHANNEL__

#include <vector>
#include "TTree.h"

// Boost math library for random number generation
#include "boost/random.hpp"

//jpan: Mersenne Twistor: A 623-diminsionally equidistributed
//uniform pseudorandom number generator
#include "TRandom3.h"


#include "VQwDataElement.h"



// this data is used to decided which data need to be histogrammed or ttree-ed


///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class VQwScaler_Channel: public VQwDataElement {

  public:
  static Int_t GetBufferOffset(Int_t scalerindex, Int_t wordindex);


  public:
  VQwScaler_Channel() {
    InitializeChannel("");
  };

  VQwScaler_Channel(TString name) {
    InitializeChannel(name);
  };
  virtual ~VQwScaler_Channel() { DeleteHistograms(); };

  void  InitializeChannel(TString name) {
    fValue_Raw  = 0;
    fValue      = 0.0;
    fValueM2    = 0.0;
    fValueError = 0.0;
    fPedestal   = 0.0;
    fCalibrationFactor = 1.0;

    fTreeArrayIndex = 0;
    fTreeArrayNumEntries =0;

    SetNumberOfDataWords(1);  //Scaler - single word, 32 bits

    fNumEvtsWithHWErrors=0;//init error counters
    fNumEvtsWithEventCutsRejected=0;//init error counters

    fDeviceErrorCode = 0;
    fGoodEventCount = 0;
    SetElementName(name);
    return;
  };

  void     SetPedestal(Double_t ped) { fPedestal = ped; };
  Double_t GetPedestal() const       { return fPedestal; };
  void     SetCalibrationFactor(Double_t factor) { fCalibrationFactor = factor; };
  Double_t GetCalibrationFactor() const          { return fCalibrationFactor; };

  void  ClearEventData();

  void ReportErrorCounters();//This will display the error summary for each device
  void UpdateHWErrorCount(){//Update error counter for HW faliure
    fNumEvtsWithHWErrors++;
  };

  void UpdateEventCutErrorCount(){//Update error counter for event cut faliure
    fNumEvtsWithEventCutsRejected++;
  };

  void  RandomizeEventData(int helicity);
  void  SetEventData(Double_t value);

  virtual void  EncodeEventData(std::vector<UInt_t> &buffer) = 0;
  virtual Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index = 0) = 0;

  void  ProcessEvent();

  Double_t GetValue() const { return fValue; };
  Double_t GetValueM2() const { return fValueM2; };
  Double_t GetValueWidth() const  { 
    if (fGoodEventCount>0){
      return (fValueError*sqrt(fGoodEventCount)); 
    }
    return 0.0;
  };
  Double_t GetValueError() const  { return fValueError; };


  VQwScaler_Channel& operator=  (const VQwScaler_Channel &value);
  VQwDataElement& operator=  (const VQwDataElement &data_value);
  VQwScaler_Channel& operator+= (const VQwScaler_Channel &value);
  VQwScaler_Channel& operator-= (const VQwScaler_Channel &value);
  void Sum(VQwScaler_Channel &value1, VQwScaler_Channel &value2);
  void Difference(VQwScaler_Channel &value1, VQwScaler_Channel &value2);
  void Ratio(VQwScaler_Channel &numer, VQwScaler_Channel &denom);
  void Offset(Double_t Offset);
  void Scale(Double_t Offset);
  void Normalize(const VQwScaler_Channel &norm);

  Bool_t ApplySingleEventCuts();//check values read from modules are at desired level
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);

  void  ConstructBranch(TTree *tree, TString &prefix);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  void AccumulateRunningSum(const VQwScaler_Channel &value);

  void Copy(VQwDataElement *source);

  void PrintValue() const;
  void PrintInfo() const;
  void CalculateRunningAverage();

protected:

  static const Bool_t kDEBUG;

  UInt_t   fValue_Raw;
  Double_t fValue;
  Double_t fValueM2;
  Double_t fValueError;

  Double_t fPedestal;
  Double_t fCalibrationFactor;

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  Int_t fNumEvtsWithHWErrors;//counts the HW falied events
  Int_t fNumEvtsWithEventCutsRejected;////counts the Event cut rejected events

  UInt_t fGoodEventCount;
  Int_t fDeviceErrorCode;

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
