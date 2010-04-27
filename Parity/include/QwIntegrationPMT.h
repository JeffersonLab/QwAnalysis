/**********************************************************\
* File: QwIntegrationPMT.h                                *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwVQWK_IntegrationPMT__
#define __QwVQWK_IntegrationPMT__

#include <vector>
#include <TTree.h>

#include "QwVQWK_Channel.h"
#include "QwBlinder.h"

#define MYSQLPP_SSQLS_NO_STATICS
#include "QwSSQLS.h"
#include "QwDatabase.h"

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
class QwIntegrationPMT : public VQwDataElement{
/////
 public:
  QwIntegrationPMT() { };
  QwIntegrationPMT(TString name){
    InitializeChannel(name,"raw");
  };
  ~QwIntegrationPMT() {DeleteHistograms();};

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);

  void  InitializeChannel(TString name, TString datatosave);
  void SetElementName(const TString &name) { fElementName = name; fTriumf_ADC.SetElementName(name);};

  void  ClearEventData();
  void ReportErrorCounters();
/********************************************************/

  void  SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity);

  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  Double_t GetHardwareSum();
  Double_t GetBlockValue(Int_t blocknum);

  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure
  Int_t SetSingleEventCuts(std::vector<Double_t> &);//two limts and sample size
  void SetDefaultSampleSize(Int_t sample_size);
  void SetEventNumber(int event);

  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
    fTriumf_ADC.SetEventCutMode(bcuts);
  }


  void Calculate_Running_Average();
  void Do_RunningSum();
  void BlindMe(QwBlinder *blinder);

  void Print() const;

  Double_t GetRawBlockValue(size_t blocknum)
           {return fTriumf_ADC.GetRawBlockValue(blocknum);};



  QwIntegrationPMT& operator=  (const QwIntegrationPMT &value);
  QwIntegrationPMT& operator+= (const QwIntegrationPMT &value);
  QwIntegrationPMT& operator-= (const QwIntegrationPMT &value);
  void Sum(QwIntegrationPMT &value1, QwIntegrationPMT &value2);
  void Difference(QwIntegrationPMT &value1, QwIntegrationPMT &value2);
  void Ratio(QwIntegrationPMT &numer, QwIntegrationPMT &denom);
  void Scale(Double_t factor);

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  DeleteHistograms();

  Double_t GetAverage(TString type)      {return fTriumf_ADC.GetAverage();};
  Double_t GetAverageError(TString type) {return fTriumf_ADC.GetAverageError();};

  void Copy(VQwDataElement *source);

  QwParityDB::md_data   GetMainDetectorDBEntry(QwDatabase *db, TString mtype, TString subname);
  QwParityDB::lumi_data GetLumiDetectorDBEntry(QwDatabase *db, TString mtype, TString subname);

 protected:


 private:

  Double_t fPedestal;
  Double_t fCalibration;
  Double_t fULimit, fLLimit;
  Bool_t fGoodEvent;//used to validate sequence number in the IsGoodEvent()

  QwVQWK_Channel fTriumf_ADC;

  Int_t fDevice_flag;//sets the event cut level for the device fDevice_flag=1 Event cuts & HW check,fDevice_flag=0 HW check, fDevice_flag=-1 no check
  Int_t fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode

  const static  Bool_t bDEBUG=kFALSE;//debugging display purposes
  Bool_t bEVENTCUTMODE; //global switch to turn event cuts ON/OFF
};

#endif
