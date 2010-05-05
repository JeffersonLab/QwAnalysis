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



// The class QwDBIntegratedPMT is used in QwIntegratePMT class
// QwDBIntegratedPMT   GetDBEntry(QwDatabase *db, TString mtype, TString subname);
// so that we can use the same name GetDBEntry as the other subsystem  and 
// we don't use any specific-detector-related function inside the QwIntegratePMT 
// class. Thus, we use the same function name in all subsystem and 
// the function outside the QwIntegratePMT class.
// Tue May  4 19:42:01 EDT 2010 (jhlee)

class QwDBIntegratedPMT
{
  
 private:
  
  UInt_t fAnalysisId;
  UInt_t fDetectorId;
  UInt_t fSubblock;
  UInt_t fN;
  Double_t fValue;
  Double_t fError;
  Char_t fMeasurementTypeId[4];

 public:

  QwDBIntegratedPMT():fAnalysisId(0),fDetectorId(0),fSubblock(0),fN(0),fValue(0.0),fError(0.0)
    {std::strcpy(fMeasurementTypeId, "");} ;
    ~QwDBIntegratedPMT(){};

    void SetAnalysisID(UInt_t id) {fAnalysisId = id;};
    void SetDetectorID(UInt_t id) {fDetectorId = id;};
    void SetMeasurementTypeID(const char* in) {std::strncpy(fMeasurementTypeId, in, 3);};
    void SetSubblock(UInt_t in) {fSubblock = in;};
    void SetN(UInt_t in)        {fN = in;};
    void SetValue(Double_t in)  {fValue = in;};
    void SetError(Double_t in)  {fError = in;};
    
    /* 	UInt_t GetAnalysisID() {return fAnalysisId;}; */
    /* 	UInt_t GetDetectorID() {return fDetectorId;}; */
    /* 	Char_t* GetMeasurementTypeID() {return fMeasurementTypeId;}; */
    /* 	UInt_t GetSubblock() {return fSubblock;}; */
    /* 	UInt_t GetN() {return fN;}; */
    /* 	Double_t GetValue() {return fValue;}; */
    /* 	Double_t GetError() {return fError;}; */
    
    QwParityDB::md_data MDDataDBClone() {
      QwParityDB::md_data row(0);
      row.analysis_id         = fAnalysisId;
      row.main_detector_id    = fDetectorId;
      row.measurement_type_id = fMeasurementTypeId;
      row.subblock            = fSubblock;
      row.n                   = fN;
      row.value               = fValue;
      row.error               = fError;
      return row;
    };
    
    QwParityDB::lumi_data LDDataDBClone() {
      QwParityDB::lumi_data row(0);
      row.analysis_id         = fAnalysisId;
      row.lumi_detector_id    = fDetectorId;
      row.measurement_type_id = fMeasurementTypeId;
      row.subblock            = fSubblock;
      row.n                   = fN;
      row.value               = fValue;
      row.error               = fError;
      return row;
    };

};



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
  void  RandomizeEventData(int helicity = 0, double time = 0.0);

  void  UseExternalRandomVariable();
  void  SetExternalRandomVariable(Double_t random_variable);

  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  Double_t GetHardwareSum();
  Double_t GetBlockValue(Int_t blocknum);

  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

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

  QwDBIntegratedPMT   GetDBEntry(QwDatabase *db, TString mtype, TString subname); 


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
