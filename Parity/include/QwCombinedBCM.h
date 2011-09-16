/**********************************************************\
* File: QwBCM.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __Qw_COMBINEDBCM__
#define __Qw_COMBINEDBCM__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "VQwDataElement.h"
#include "VQwHardwareChannel.h"
#include "QwBCM.h"

// Forward declarations
class QwDBInterface;

/*****************************************************************
*  Class:
******************************************************************/

template<typename T>
class QwCombinedBCM : public QwBCM<T> {
/////
 public:
  QwCombinedBCM() { };
  QwCombinedBCM(TString name){
    InitializeChannel(name, "derived");
  };
  QwCombinedBCM(TString subsystem, TString name){
    InitializeChannel(subsystem, name, "derived");
  };
  QwCombinedBCM(TString subsystemname, TString name, TString type){
    this->SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name,type,"raw");
  };
  virtual ~QwCombinedBCM() { };

  // This is to setup one of the used BCM's in this combo
  void SetBCMForCombo(VQwBCM* bcm, Double_t weight, Double_t sumqw );

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);
  void  ClearEventData();

  void  InitializeChannel(TString name, TString datatosave);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name, TString datatosave);
  void  InitializeChannel(TString subsystem, TString name, TString type,
      TString datatosave);

  void ReportErrorCounters();


  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  void GetEventcutErrorCounters() const;// report number of events falied due to HW and event cut faliure
  UInt_t GetEventcutErrorFlag(){//return the error flag
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

  // Implementation of Parent class's virtual operators
  VQwBCM& operator=  (const VQwBCM &value);
  VQwBCM& operator+= (const VQwBCM &value);
  VQwBCM& operator-= (const VQwBCM &value);

  QwCombinedBCM& operator=  (const QwCombinedBCM &value);
  QwCombinedBCM& operator+= (const QwCombinedBCM &value);
  QwCombinedBCM& operator-= (const QwCombinedBCM &value);
  void Sum(QwCombinedBCM &value1, QwCombinedBCM &value2);
  void Difference(QwCombinedBCM &value1, QwCombinedBCM &value2);
  void Ratio(const QwCombinedBCM &numer, const QwCombinedBCM &denom);
  void Ratio(const VQwBCM &numer, const VQwBCM &denom);
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

  void Copy(VQwDataElement *source);

  VQwHardwareChannel* GetCharge(){
    return &fCombined_bcm;
  };

  const VQwHardwareChannel* GetCharge() const {
    return const_cast<QwCombinedBCM*>(this)->GetCharge();
  };

  std::vector<QwDBInterface> GetDBEntry();


/////
 protected:

  T fCombined_bcm;
/////
 private:

  Double_t fCalibration;
  Double_t fULimit, fLLimit;
  Double_t fSequenceNo_Prev;

  Bool_t fGoodEvent;//used to validate sequence number in the IsGoodEvent() */

  std::vector <QwBCM<T>*> fElement;
  std::vector <Double_t> fWeights;
  Double_t fSumQweights;


  Int_t fDevice_flag;//sets the event cut level for the device fDevice_flag=1 Event cuts & HW check,fDevice_flag=0 HW check, fDevice_flag=-1 no check  */
  Int_t fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode

  const static  Bool_t bDEBUG=kFALSE;//debugging display purposes */
  Bool_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts do not depend on HW ckecks. This is set externally through the qweak_beamline_eventcuts.map

  std::vector<QwVQWK_Channel> fBCMComboElementList;

 private:
  //  Functions to be removed
  
/*   void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0); */
/*   void  SetEventData(Double_t* block, UInt_t sequencenumber); */


};

#endif
