
#ifndef __QwVQWK_COMBINEDPMT__
#define __QwVQWK_COMBINEDPMT__

#include <vector>
#include <TTree.h>

#include "QwVQWK_Channel.h"
#include "QwIntegrationPMT.h"
#include "QwBlinder.h"

#define MYSQLPP_SSQLS_NO_STATICS
#include "QwSSQLS.h"
#include "QwDatabase.h"

class QwCombinedPMT : public VQwDataElement{
/////
 public:
  QwCombinedPMT(){
    QwCombinedPMT("");
  };

  QwCombinedPMT(TString name){
    InitializeChannel(name, "derived");
  };

  ~QwCombinedPMT() {DeleteHistograms();};

  void  InitializeChannel(TString name, TString datatosave);
  void  LinkChannel(TString name);

  void Add(QwIntegrationPMT* pmt, Double_t weight);

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);
  void  ClearEventData();

  void ReportErrorCounters();

  void CalculateSumAndAverage();

  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity);
  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings 
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure
  Int_t SetSingleEventCuts(std::vector<Double_t> &);//two limts and sample size
  void SetDefaultSampleSize(Int_t sample_size);
  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
    //fCombinedPMT.SetEventCutMode(bcuts);
  }
  void Print() const;

  QwCombinedPMT& operator=  (const QwCombinedPMT &value);
  QwCombinedPMT& operator+= (const QwCombinedPMT &value);
  QwCombinedPMT& operator-= (const QwCombinedPMT &value);
  void Sum(QwCombinedPMT &value1, QwCombinedPMT &value2);
  void Difference(QwCombinedPMT &value1, QwCombinedPMT &value2);
  void Ratio(QwCombinedPMT &numer, QwCombinedPMT &denom); 
  void Scale(Double_t factor);
  void Calculate_Running_Average();
  void Do_RunningSum(); 
  void BlindMe(QwBlinder *blinder);

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  DeleteHistograms();

  Double_t GetAverage(TString type="")      {return fSumADC.GetAverage();};
  Double_t GetAverageError(TString type="") {return fSumADC.GetAverageError();};

  void Copy(VQwDataElement *source);

  QwDBInterface  GetDBEntry(TString subname);

 protected:

 private:

  Int_t fDataToSave;
  Double_t fCalibration; 
  Double_t fULimit, fLLimit;
  Double_t fSequenceNo_Prev;

  Bool_t fGoodEvent; /// used to validate sequence number in the IsGoodEvent()

  std::vector <QwIntegrationPMT*> fElement;
  std::vector <Double_t> fWeights;

  QwIntegrationPMT  fSumADC;
  //QwIntegrationPMT  fAvgADC;

  Int_t fDevice_flag; /// sets the event cut level for the device 
                      /// fDevice_flag=1 Event cuts & HW check,
                      /// fDevice_flag=0 HW check, fDevice_flag=-1 no check

  Int_t fDeviceErrorCode; /// keep the device HW status using a unique code
                          /// from the QwVQWK_Channel::fDeviceErrorCode

  Bool_t bEVENTCUTMODE; /// If this set to kFALSE then Event cuts do not depend
                        /// on HW ckecks. This is set externally through the 
                        /// qweak_beamline_eventcuts.map

  const static  Bool_t bDEBUG=kFALSE; /// debugging display purposes

};

#endif
