/**********************************************************\
* File: QwBCM.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwVQWK_BCM__
#define __QwVQWK_BCM__

#include <vector>
#include <TTree.h>

#include "QwBlinder.h"
#include "QwVQWK_Channel.h"
#define MYSQLPP_SSQLS_NO_STATICS
#include "QwSSQLS.h"


#include "QwDatabase.h"

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
class QwBCM : public VQwDataElement{
/////
  friend class QwCombinedBCM;
 public:
  QwBCM() { };
  QwBCM(TString name){
    InitializeChannel(name,"raw");
  };
  ~QwBCM() {DeleteHistograms();};

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);

  void  InitializeChannel(TString name, TString datatosave);
  void  ClearEventData();


  void  SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity = 0, double time = 0);
  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  UseExternalRandomVariable();
  void  SetExternalRandomVariable(Double_t random_variable);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure
  Int_t GetEventcutErrorFlag(){//return the error flag
    return fDeviceErrorCode;
  }

  Int_t SetSingleEventCuts(Double_t mean, Double_t sigma);//two limts and sample size
  void SetDefaultSampleSize(Int_t sample_size);
  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
    fTriumf_ADC.SetEventCutMode(bcuts);
  }

  void Print() const;

  const QwVQWK_Channel& GetCharge() const {return fTriumf_ADC;};


  QwBCM& operator=  (const QwBCM &value);
  QwBCM& operator+= (const QwBCM &value);
  QwBCM& operator-= (const QwBCM &value);
  void Sum(QwBCM &value1, QwBCM &value2);
  void Difference(QwBCM &value1, QwBCM &value2);
  void Ratio(QwBCM &numer, QwBCM &denom);
  void Scale(Double_t factor);
  void Calculate_Running_Average();
  void Do_RunningSum();

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  DeleteHistograms();

  Double_t GetAverage()        {return fTriumf_ADC.GetAverage();};
  Double_t GetAverageError()   {return fTriumf_ADC.GetAverageError();};
  UInt_t   GetGoodEventCount() {return fTriumf_ADC.GetGoodEventCount();};

  void Copy(VQwDataElement *source);

  QwDBInterface GetDBEntry(TString subname="");


/////
 protected:

/////
 private:

  Double_t fPedestal;
  Double_t fCalibration;
  Double_t fULimit, fLLimit;
  Bool_t fGoodEvent;//used to validate sequence number in the IsGoodEvent()




  QwVQWK_Channel fTriumf_ADC;

  Int_t fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode

  const static  Bool_t bDEBUG=kFALSE;//debugging display purposes
  Bool_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts do not depend on HW ckecks. This is set externally through the qweak_beamline_eventcuts.map

};

#endif
