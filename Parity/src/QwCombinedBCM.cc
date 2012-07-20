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
  
  //This is required to update single event cut faliures in individual channels
  //  First update the error code based on the codes
  //  of the elements.  This requires that the BCMs
  //  have had ApplySingleEventCuts run on them already.
  
  for (size_t i=0;i<fElement.size();i++){
    this->fBeamCurrent.UpdateErrorFlag(fElement.at(i)->fBeamCurrent.GetErrorCode());
  }
  

  //  Everything is identical as for a regular BCM
  return QwBCM<T>::ApplySingleEventCuts();
}

template<typename T>
UInt_t QwCombinedBCM<T>::UpdateErrorFlag(){
  for (size_t i=0;i<fElement.size();i++){
    this->fBeamCurrent.UpdateErrorFlag(fElement.at(i)->fBeamCurrent.GetErrorCode());
  }
  return this->fBeamCurrent.GetEventcutErrorFlag();
}


/********************************************************/
/*
template<typename T>
void QwCombinedBCM<T>::UpdateErrorFlag(const VQwBCM *ev_error){
    
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwCombinedBCM::UpdateErrorFlag \n";
      if (this->GetElementName()!="") {
        QwCombinedBCM<T>* value_bcm = dynamic_cast<QwCombinedBCM<T>* >(ev_error);
	VQwDataElement *value_data = dynamic_cast<VQwDataElement *>(&(value_bcm->fBeamCurrent));
	fBeamCurrent.UpdateErrorFlag(value_data->GetErrorCode());//the routine GetErrorCode() return the error flag + configuration flag unconditionally
      }
    } else {
      TString loc="Standard exception from QwCombinedBCM::UpdateErrorFlag :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }  
    
    QwBCM<T>::UpdateErrorFlag(const ev_error);
};
*/

/********************************************************/
/*
template<typename T>
void QwCombinedBCM<T>::CalculateRunningAverage(){
  fBeamCurrent.CalculateRunningAverage();
=======
  return eventokay;
>>>>>>> .merge-right.r3406
}
*/
/********************************************************/
/*
template<typename T>
<<<<<<< .working
void QwCombinedBCM<T>::AccumulateRunningSum(const VQwBCM &value){
  fBeamCurrent.AccumulateRunningSum(dynamic_cast<const QwCombinedBCM<T>* >(&value)->fBeamCurrent);
}
*/
/********************************************************/
/*
template<typename T>
void QwCombinedBCM<T>::DeaccumulateRunningSum(VQwBCM &value){
  fBeamCurrent.DeaccumulateRunningSum(dynamic_cast<QwCombinedBCM<T>* >(&value)->fBeamCurrent);
}
*/


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
/*
template<typename T>
Bool_t QwCombinedBCM<T>::ApplyHWChecks()
{
  // For the combined devices there are no physical channels that we can relate to because they  are being
  // derived from combinations of physical channels. Therefore, this is not exactly a "HW Check"
  // but just a check of the HW checks of the combined channels.

  Bool_t eventokay=kTRUE;

  return eventokay;
}
*/

/*
template<typename T>
Int_t QwCombinedBCM<T>::SetSingleEventCuts(Double_t LL=0, Double_t UL=0){
  fBeamCurrent.SetSingleEventCuts(LL,UL);
  return 1;
}


template<typename T>
void QwCombinedBCM<T>::SetSingleEventCuts(UInt_t errorflag, Double_t LL=0, Double_t UL=0, Double_t stability=0){
  //set the unique tag to identify device type (bcm,bpm & etc)
  errorflag|=kBCMErrorFlag;//currently I use the same flag for bcm & combinedbcm
  QwMessage<<"QwCombinedBCM Error Code passing to QwVQWK_Ch "<<errorflag<<" "<<stability<<QwLog::endl;
  fBeamCurrent.SetSingleEventCuts(errorflag,LL,UL,stability);

}


template<typename T>
void  QwCombinedBCM<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (this->GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fBeamCurrent.ConstructHistograms(folder, prefix);
    }
  return;

}

template<typename T>
void  QwCombinedBCM<T>::FillHistograms()
{
 if (this->GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fBeamCurrent.FillHistograms();
    }


  return;
}

template<typename T>
void  QwCombinedBCM<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fBeamCurrent.ConstructBranchAndVector(tree,prefix,values);
    }
  return;
}

template<typename T>
void  QwCombinedBCM<T>::ConstructBranch(TTree *tree, TString &prefix)
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fBeamCurrent.ConstructBranch(tree,prefix);
    }
  return;
}

template<typename T>
void  QwCombinedBCM<T>::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;
  devicename=this->GetElementName();
  devicename.ToLower();

  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      if (modulelist.HasValue(devicename)){
	fBeamCurrent.ConstructBranch(tree,prefix);
	QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
      }
    }
  return;
}


template<typename T>
void  QwCombinedBCM<T>::FillTreeVector(std::vector<Double_t> &values) const
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fBeamCurrent.FillTreeVector(values);
    }
  return;
}

*/

/********************************************************/
template class QwCombinedBCM<QwVQWK_Channel>; 
template class QwCombinedBCM<QwSIS3801_Channel>; 
template class QwCombinedBCM<QwSIS3801D24_Channel>; 
