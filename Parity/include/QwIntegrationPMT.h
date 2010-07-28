/**********************************************************\
* File: QwIntegrationPMT.h                                *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwVQWK_IntegrationPMT__
#define __QwVQWK_IntegrationPMT__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwVQWK_Channel.h"
#include "QwParameterFile.h"

// Qweak database headers
#define MYSQLPP_SSQLS_NO_STATICS
#include "QwSSQLS.h"
#include "QwDatabase.h"

// Forward declarations
class QwBlinder;


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
  ~QwIntegrationPMT() {
    DeleteHistograms();
  };

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);

  void  InitializeChannel(TString name, TString datatosave);
  void SetElementName(const TString &name) { fElementName = name; fTriumf_ADC.SetElementName(name);};

  const QwVQWK_Channel* GetChannel(const TString name) const {
    if (fTriumf_ADC.GetElementName() == name) return &fTriumf_ADC;
    else return 0;
  };

  void  ClearEventData();
  void ReportErrorCounters();
/********************************************************/

  void  SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity = 0, double time = 0.0);
  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  UseExternalRandomVariable();
  void  SetExternalRandomVariable(Double_t random_variable);

  Double_t GetHardwareSum();
  Double_t GetBlockValue(Int_t blocknum);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure
  Int_t SetSingleEventCuts(Double_t, Double_t);//set two limts
  void SetDefaultSampleSize(Int_t sample_size);

  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
    fTriumf_ADC.SetEventCutMode(bcuts);
  }

  /// \brief Blind the asymmetry
  void Blind(const QwBlinder *blinder);
  /// \brief Blind the difference using the yield
  void Blind(const QwBlinder *blinder, const QwIntegrationPMT& yield);

  void PrintValue() const;
  void PrintInfo() const;

  Double_t GetRawBlockValue(size_t blocknum)
           {return fTriumf_ADC.GetRawBlockValue(blocknum);};



  QwIntegrationPMT& operator=  (const QwIntegrationPMT &value);
  QwIntegrationPMT& operator+= (const QwIntegrationPMT &value);
  QwIntegrationPMT& operator-= (const QwIntegrationPMT &value);
  void Sum(QwIntegrationPMT &value1, QwIntegrationPMT &value2);
  void Difference(QwIntegrationPMT &value1, QwIntegrationPMT &value2);
  void Ratio(QwIntegrationPMT &numer, QwIntegrationPMT &denom);
  void Scale(Double_t factor);

  void AccumulateRunningSum(const QwIntegrationPMT& value);
  void CalculateRunningAverage();

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& trim_file);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  DeleteHistograms();

  Double_t GetAverage()        {return fTriumf_ADC.GetAverage();};
  Double_t GetAverageError()   {return fTriumf_ADC.GetAverageError();};
  UInt_t   GetGoodEventCount() {return fTriumf_ADC.GetGoodEventCount();};

  void Copy(VQwDataElement *source);

  std::vector<QwDBInterface> GetDBEntry();

 protected:


 private:

  Double_t fPedestal;
  Double_t fCalibration;
  Bool_t fGoodEvent;//used to validate sequence number in the IsGoodEvent()

  QwVQWK_Channel fTriumf_ADC;

  Int_t fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode

  const static  Bool_t bDEBUG=kFALSE;//debugging display purposes
  Bool_t bEVENTCUTMODE; //global switch to turn event cuts ON/OFF
};



#endif
