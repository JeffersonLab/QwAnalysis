/**********************************************************\
* File: QwBCM.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwVQWK_COMBINEDBCM__
#define __QwVQWK_COMBINEDBCM__

#include <vector>
#include <TTree.h>

#include "QwVQWK_Channel.h"
#include "QwBCM.h"

/*****************************************************************
*  Class:
******************************************************************/

class QwCombinedBCM : public VQwDataElement{
/////
 public:
  QwCombinedBCM() { };
  QwCombinedBCM(TString name){
    InitializeChannel(name, "derived");
  };
  QwCombinedBCM(TString subsystem, TString name){
    InitializeChannel(subsystem, name, "derived");
  };
  ~QwCombinedBCM() {
    DeleteHistograms();
  };




  void Set(QwBCM* bcm, Double_t weight, Double_t sumqw ); ///added by me

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);
  void  ClearEventData();

  void  InitializeChannel(TString name, TString datatosave);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name, TString datatosave);

  void ReportErrorCounters();


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
  Int_t GetEventcutErrorFlag(){//return the error flag
    return fCombined_bcm.GetEventcutErrorFlag();
  }
  Int_t SetSingleEventCuts(Double_t mean, Double_t sigma);//two limts and sample size
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  void SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability);
  void SetDefaultSampleSize(Int_t sample_size);
  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
    fCombined_bcm.SetEventCutMode(bcuts);
  }

  void PrintValue() const;
  void PrintInfo() const;




  QwCombinedBCM& operator=  (const QwCombinedBCM &value);
  QwCombinedBCM& operator+= (const QwCombinedBCM &value);
  QwCombinedBCM& operator-= (const QwCombinedBCM &value);
  void Sum(QwCombinedBCM &value1, QwCombinedBCM &value2);
  void Difference(QwCombinedBCM &value1, QwCombinedBCM &value2);
  void Ratio(QwCombinedBCM &numer, QwCombinedBCM &denom);
  void Scale(Double_t factor);

  void AccumulateRunningSum(const QwCombinedBCM& value);
  void CalculateRunningAverage();

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void  FillTreeVector(std::vector<Double_t> &values) const;
  void  DeleteHistograms();

  void Copy(VQwDataElement *source);

  VQwDataElement* GetCharge(){
    return &fCombined_bcm;
  };

  const VQwDataElement* GetCharge() const {
    return const_cast<QwCombinedBCM*>(this)->GetCharge();
  };

  std::vector<QwDBInterface> GetDBEntry();


/////
 protected:

  QwVQWK_Channel fCombined_bcm;
/////
 private:

  Double_t fCalibration;
  Double_t fULimit, fLLimit;
  Double_t fSequenceNo_Prev;

  Bool_t fGoodEvent;//used to validate sequence number in the IsGoodEvent() */

  std::vector <QwBCM*> fElement;
  std::vector <Double_t> fWeights;
  Double_t fSumQweights;


  Int_t fDevice_flag;//sets the event cut level for the device fDevice_flag=1 Event cuts & HW check,fDevice_flag=0 HW check, fDevice_flag=-1 no check  */
  Int_t fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode

  const static  Bool_t bDEBUG=kFALSE;//debugging display purposes */
  Bool_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts do not depend on HW ckecks. This is set externally through the qweak_beamline_eventcuts.map

  std::vector<QwVQWK_Channel> fBCMComboElementList;

};

#endif
