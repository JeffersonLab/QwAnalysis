/**********************************************************\
<<<<<<< .working
* File: QwCombinedBCM.h                                  *
=======
* File: QwBCM.h                                           *
>>>>>>> .merge-right.r3406
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
  QwCombinedBCM(const QwCombinedBCM& source)
  : QwBCM<T>(source)
  { }
  virtual ~QwCombinedBCM() { };

  // This is to setup one of the used BCM's in this combo
  void SetBCMForCombo(VQwBCM* bcm, Double_t weight, Double_t sumqw );

  // No processing of event buffer
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement = 0) { return 0; };

  void  InitializeChannel(TString name, TString datatosave);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name, TString datatosave);
  void  InitializeChannel(TString subsystem, TString name, TString type,
      TString datatosave);

  void  ProcessEvent();

  Bool_t ApplyHWChecks(){
    return kTRUE;
  };

  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings

  UInt_t UpdateErrorFlag();


  // Implementation of Parent class's virtual operators
  VQwBCM& operator=  (const VQwBCM &value);
  QwCombinedBCM& operator=  (const QwCombinedBCM &value);

  /*
  void AccumulateRunningSum(const VQwBCM &value);
  void DeaccumulateRunningSum(VQwBCM &value);
  void CalculateRunningAverage();
  */
  void SetPedestal(Double_t ped) {
    QwBCM<T>::SetPedestal(0.0);
  }
  void SetCalibrationFactor(Double_t calib) {
    QwBCM<T>::SetCalibrationFactor(1.0);
  }

  VQwHardwareChannel* GetCharge(){
    return &(this->fBeamCurrent);
  };

  const VQwHardwareChannel* GetCharge() const {
    return const_cast<QwCombinedBCM*>(this)->GetCharge();
  };

/////
 private:

  std::vector <QwBCM<T>*> fElement;
  std::vector <Double_t>  fWeights;
  Double_t fSumQweights;

};

#endif
