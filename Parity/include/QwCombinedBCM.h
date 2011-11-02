/**********************************************************\
* File: QwBCM.h                                           *
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

  // No processing of event buffer
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement = 0) { return 0; };

  void  InitializeChannel(TString name, TString datatosave);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name, TString datatosave);
  void  InitializeChannel(TString subsystem, TString name, TString type,
      TString datatosave);

  // No reporting of error counters
  void ReportErrorCounters() { };

  void  ProcessEvent();

  Bool_t ApplyHWChecks();
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings

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

  void SetPedestal(Double_t ped) {
    QwBCM<T>::SetPedestal(0.0);
  }
  void SetCalibrationFactor(Double_t calib) {
    QwBCM<T>::SetCalibrationFactor(1.0);
  }

  void Copy(VQwDataElement *source);

  VQwHardwareChannel* GetCharge(){
    return &(this->fBeamCurrent);
  };

  const VQwHardwareChannel* GetCharge() const {
    return const_cast<QwCombinedBCM*>(this)->GetCharge();
  };

  std::vector<QwDBInterface> GetDBEntry();

/////
 private:

  std::vector <QwBCM<T>*> fElement;
  std::vector <Double_t> fWeights;
  Double_t fSumQweights;

};

#endif
