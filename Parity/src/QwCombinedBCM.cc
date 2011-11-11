/**********************************************************\
* File: QwCombinedBCM.cc                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwCombinedBCM.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwDBInterface.h"

//  Qweak types that we want to use in this template
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"

/********************************************************/

//this is a combined BCM made out of BCMs that are already callibrated and have pedstals removed.
//This will be used for projection of charge at the target

template<typename T>
void QwCombinedBCM<T>::SetBCMForCombo(VQwBCM* bcm, Double_t weight, Double_t sumqw )
{
  // Convert back to QWBCM<T>* from generic VQwBCM*
  fElement.push_back(dynamic_cast<QwBCM<T>* >(bcm));
  fWeights.push_back(weight);
  fSumQweights = sumqw;
}

/********************************************************/

template<typename T>
void  QwCombinedBCM<T>::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  this->SetElementName(name);
  this->fBeamCurrent.InitializeChannel(name,"derived");
}

template<typename T>
void  QwCombinedBCM<T>::InitializeChannel(TString subsystem, TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  this->SetElementName(name);
  this->fBeamCurrent.InitializeChannel(subsystem, "QwCombinedBCM", name,"derived");
}

template<typename T>
void  QwCombinedBCM<T>::InitializeChannel(TString subsystem, TString name,
    TString type, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  this->SetElementName(name);
  this->SetModuleType(type);
  this->fBeamCurrent.InitializeChannel(subsystem, "QwCombinedBCM", name,"derived");
}


/********************************************************/
template<typename T>
void  QwCombinedBCM<T>::ProcessEvent()
{
  static T tmpADC;
  tmpADC.InitializeChannel("tmp","derived");

  for (size_t i = 0; i < fElement.size(); i++) {
    tmpADC = fElement[i]->fBeamCurrent;
    tmpADC.Scale(fWeights[i]);
    this->fBeamCurrent += tmpADC;
  }
  this->fBeamCurrent.Scale(1.0/fSumQweights);

  Bool_t ldebug = kFALSE;
  if (ldebug) {
    QwMessage << "*****************" << QwLog::endl;
    QwMessage << "QwCombinedBCM: " << this->GetElementName() << QwLog::endl
	            << "weighted average of hardware sums = " << this->fBeamCurrent.GetValue() << QwLog::endl;
    if (this->fBeamCurrent.GetNumberOfSubelements() > 1) {
      for (size_t i = 0; i < 4; i++) {
        QwMessage << "weighted average of block[" << i << "] = " << this->fBeamCurrent.GetValue(i) << QwLog::endl;
      }
    }
    QwMessage << "*****************" << QwLog::endl;
  }
}


/********************************************************/
template<typename T>
Bool_t QwCombinedBCM<T>::ApplySingleEventCuts()
{
  //  First update the error code based on the codes
  //  of the elements.  This requires that the BCMs
  //  have had ApplySingleEventCuts run on them already.
  for (size_t i=0;i<fElement.size();i++){
    this->fBeamCurrent.UpdateErrorCode(fElement.at(i)->fBeamCurrent.GetErrorCode());
  }

  //  Everything is identical as for a regular BCM
  return QwBCM<T>::ApplySingleEventCuts();
}

/********************************************************/
template<typename T>
Bool_t QwCombinedBCM<T>::ApplyHWChecks()
{
  // For the combined devices there are no physical channels that we can relate to because they  are being
  // derived from combinations of physical channels. Therefore, this is not exactly a "HW Check"
  // but just a check of the HW checks of the combined channels.
  Bool_t eventokay = kTRUE;

  return eventokay;
}

/********************************************************/
template<typename T>
QwCombinedBCM<T>& QwCombinedBCM<T>::operator= (const QwCombinedBCM<T> &value)
{
  if (this->GetElementName()!="")
    this->fBeamCurrent=value.fBeamCurrent;

  return *this;
}

template<typename T>
VQwBCM& QwCombinedBCM<T>::operator= (const VQwBCM &value)
{
  if (this->GetElementName()!="")
    dynamic_cast<QwCombinedBCM<T>* >(this)->fBeamCurrent=
      dynamic_cast<const QwCombinedBCM<T>* >(&value)->fBeamCurrent;

  return *this;
}

/********************************************************/
template<typename T>
void  QwCombinedBCM<T>::Copy(VQwDataElement *source)
{
  try {
    if (typeid(*source) == typeid(*this)) {
      QwBCM<T>::Copy(source);
      //const QwCombinedBCM<T>* input = dynamic_cast<const QwCombinedBCM<T>*>(source);
      // do we need to copy the elements and weights here?
    } else {
      TString loc="Standard exception from QwCombinedBCM::Copy = "
          +source->GetElementName()+" "
          +this->GetElementName()+" are not of the same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

/********************************************************/
template class QwCombinedBCM<QwVQWK_Channel>; 
template class QwCombinedBCM<QwSIS3801_Channel>; 
template class QwCombinedBCM<QwSIS3801D24_Channel>; 
