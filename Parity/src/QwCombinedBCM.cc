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
void QwCombinedBCM<T>::SetPedestal(Double_t pedestal)
{
	fCombined_bcm.SetPedestal(0);
	return;
}

template<typename T>
void QwCombinedBCM<T>::SetCalibrationFactor(Double_t calib)
{
	fCombined_bcm.SetCalibrationFactor(1);
	return;
}

template<typename T>
void QwCombinedBCM<T>::SetBCMForCombo(VQwBCM* bcm, Double_t weight, Double_t sumqw ){
  // Convert back to QWBCM<T>* from generic VQwBCM*
  fElement.push_back(dynamic_cast<QwBCM<T>* >(bcm));
  fWeights.push_back(weight);
  fSumQweights=sumqw;
  //std::cout<<"QwCombinedBCM: Got "<<bcm->GetElementName()<<"  and weight ="<<weight<<"\n";
  }

template<typename T>
void  QwCombinedBCM<T>::InitializeChannel(TString name, TString datatosave)
{
  this->SetElementName(name);
  fCombined_bcm.InitializeChannel(name,"derived");

  return;
}

template<typename T>
void  QwCombinedBCM<T>::InitializeChannel(TString subsystem, TString name, TString datatosave)
{
  this->SetElementName(name);
  fCombined_bcm.InitializeChannel(subsystem, "QwCombinedBCM", name,"derived");

  return;
}

template<typename T>
void  QwCombinedBCM<T>::InitializeChannel(TString subsystem, TString name,
    TString type, TString datatosave)
{
  this->SetElementName(name);
  this->SetModuleType(type);
  fCombined_bcm.InitializeChannel(subsystem, "QwCombinedBCM", name,"derived");

  return;
}


template<typename T>
void QwCombinedBCM<T>::ClearEventData()
{
  fCombined_bcm.ClearEventData();
  return;
}


template<typename T>
void QwCombinedBCM<T>::ReportErrorCounters()
{
  //fCombined_bcm.ReportErrorCounters();
}

/********************************************************/
template<typename T>
void QwCombinedBCM<T>::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fCombined_bcm.SetRandomEventParameters(mean, sigma);
  return;
}

template<typename T>
 void QwCombinedBCM<T>::SetRandomEventAsymmetry(Double_t asymmetry)
{
   fCombined_bcm.SetRandomEventAsymmetry(asymmetry);
   return;
}

template<typename T>
void QwCombinedBCM<T>::RandomizeEventData(int helicity)
{
  fCombined_bcm.RandomizeEventData(helicity);
  return;
}
/********************************************************/
// template<typename T>
// void QwCombinedBCM<T>::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
// {
//   fCombined_bcm.SetHardwareSum(hwsum, sequencenumber);
//   return;
// }
// template<typename T>
// void QwCombinedBCM<T>::SetEventData(Double_t* block, UInt_t sequencenumber)
// {
//   fCombined_bcm.SetEventData(block, sequencenumber);
//   return;
// }
/********************************************************/
template<typename T>
void QwCombinedBCM<T>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fCombined_bcm.EncodeEventData(buffer);
}



/********************************************************/
template<typename T>
void  QwCombinedBCM<T>::ProcessEvent()
{

  Bool_t ldebug = kFALSE;
  static T tmpADC;
  tmpADC.InitializeChannel("tmp","derived");


  for(size_t i=0;i<fElement.size();i++)
  {
    tmpADC=fElement[i]->fBeamCurrent;
    tmpADC.Scale(fWeights[i]);
    fCombined_bcm +=tmpADC;

  }

  fCombined_bcm.Scale(1.0/fSumQweights);


  if(ldebug){
    std::cout<<"***************** \n";
    std::cout<<"QwCombinedBCM: "<<this->GetElementName()
	     <<"\nweighted average of hardware sums = "<<fCombined_bcm.GetValue()<<"\n";
    if (fCombined_bcm.GetNumberOfSubelements()>1){
      for(size_t i=0;i<4;i++){
	std::cout<<"weighted average of block["<<i<<"] = "<<fCombined_bcm.GetValue(i)<<"\n";
      }
    }
    std::cout<<"***************** \n";
  }
  return;
}


template<typename T>
void QwCombinedBCM<T>::SetDefaultSampleSize(Int_t sample_size){
  fCombined_bcm.SetDefaultSampleSize((size_t)sample_size);
}

/********************************************************/
template<typename T>
Bool_t QwCombinedBCM<T>::ApplySingleEventCuts(){
  Bool_t status=kTRUE;
  //  First update the error code based on the codes
  //  of the elements.  This requires that the BCMs
  //  have had ApplySingleEventCuts run on them already.
  for (size_t i=0;i<fElement.size();i++){
    fCombined_bcm.UpdateErrorCode(fElement.at(i)->fBeamCurrent.GetErrorCode());
  }
  if (fCombined_bcm.ApplySingleEventCuts()){
    status=kTRUE;
  }
  else{
    if (bDEBUG) std::cout<<" evnt cut failed:-> set limit "<<fULimit<<" harware sum  "<<fCombined_bcm.GetValue();
    status&=kFALSE;
  }
  this->fErrorFlag|=fCombined_bcm.GetEventcutErrorFlag();//retrun the error flag for event cuts
  //std::cout<<"combined bcm "<<GetElementName()<<" error flag "<<fCombined_bcm.GetEventcutErrorFlag()<<std::endl;

  return status;

}

/********************************************************/

template<typename T>
Int_t QwCombinedBCM<T>::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure
  fCombined_bcm.GetEventcutErrorCounters();
  return 1;
}

/********************************************************/

template<typename T>
void QwCombinedBCM<T>::CalculateRunningAverage(){
  fCombined_bcm.CalculateRunningAverage();
}

/********************************************************/

template<typename T>
void QwCombinedBCM<T>::AccumulateRunningSum(const QwCombinedBCM<T>& value){
  fCombined_bcm.AccumulateRunningSum(value.fCombined_bcm);
}

/********************************************************/


template<typename T>
Int_t QwCombinedBCM<T>::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
 //  fCombined_bcm.ProcessEvBuffer(buffer,word_position_in_buffer);

//   return word_position_in_buffer;
  return 0;
}
/********************************************************/
template<typename T>
QwCombinedBCM<T>& QwCombinedBCM<T>::operator= (const QwCombinedBCM<T> &value)
{
  if (this->GetElementName()!="")
    this->fCombined_bcm=value.fCombined_bcm;

  return *this;
}

template<typename T>
VQwBCM& QwCombinedBCM<T>::operator= (const VQwBCM &value)
{
  if (this->GetElementName()!="")
    dynamic_cast<QwCombinedBCM<T>* >(this)->fCombined_bcm=
      dynamic_cast<const QwCombinedBCM<T>* >(&value)->fCombined_bcm;

  return *this;
}

template<typename T>
QwCombinedBCM<T>& QwCombinedBCM<T>::operator+= (const QwCombinedBCM<T> &value)
{
  if (this->GetElementName()!="")
    this->fCombined_bcm+=value.fCombined_bcm;
  return *this;
}

template<typename T>
VQwBCM& QwCombinedBCM<T>::operator+= (const VQwBCM &value)
{
  if (this->GetElementName()!="")
    dynamic_cast<QwCombinedBCM<T>* >(this)->fCombined_bcm+=
      dynamic_cast<const QwCombinedBCM<T>* >(&value)->fCombined_bcm;

  return *this;
}

template<typename T>
QwCombinedBCM<T>& QwCombinedBCM<T>::operator-= (const QwCombinedBCM<T> &value)
{
  if (this->GetElementName()!="")
    this->fCombined_bcm-=value.fCombined_bcm;

  return *this;
}

template<typename T>
VQwBCM& QwCombinedBCM<T>::operator-= (const VQwBCM &value)
{
  if (this->GetElementName()!="")
    dynamic_cast<QwCombinedBCM<T>* >(this)->fCombined_bcm-=
      dynamic_cast<const QwCombinedBCM<T>* >(&value)->fCombined_bcm;

  return *this;
}

template<typename T>
void QwCombinedBCM<T>::Sum(QwCombinedBCM<T> &value1, QwCombinedBCM<T> &value2){
  *this =  value1;
  *this += value2;
}

template<typename T>
void QwCombinedBCM<T>::Difference(QwCombinedBCM<T> &value1, QwCombinedBCM<T> &value2){
  *this =  value1;
  *this -= value2;
}

template<typename T>
void QwCombinedBCM<T>::Ratio(const VQwBCM &numer, const VQwBCM &denom)
{
  Ratio(*dynamic_cast<const QwCombinedBCM<T>* >(&numer),
      *dynamic_cast<const QwCombinedBCM<T>* >(&denom));
}

template<typename T>
void QwCombinedBCM<T>::Ratio(const QwCombinedBCM<T> &numer,
    const QwCombinedBCM<T> &denom)
{
  if (this->GetElementName()!="")
    this->fCombined_bcm.Ratio(numer.fCombined_bcm,denom.fCombined_bcm);

  return;
}

template<typename T>
void QwCombinedBCM<T>::Scale(Double_t factor)
{
  fCombined_bcm.Scale(factor);
  return;
}


template<typename T>
void QwCombinedBCM<T>::PrintValue() const
{
  fCombined_bcm.PrintValue();
}

template<typename T>
void QwCombinedBCM<T>::PrintInfo() const
{
  std::cout << "QwVQWK_Channel Info " << std::endl;
  fCombined_bcm.PrintInfo();
}

/********************************************************/
template<typename T>
Bool_t QwCombinedBCM<T>::ApplyHWChecks()
{
  // For the combined devices there are no physical channels that we can relate to because they  are being
  // derived from combinations of physical channels. Therefore, this is not exactly a "HW Check"
  // but just a check of the HW checks of the combined channels.

  Bool_t eventokay=kTRUE;

  return eventokay;
}

template<typename T>
Int_t QwCombinedBCM<T>::SetSingleEventCuts(Double_t LL=0, Double_t UL=0){
  fCombined_bcm.SetSingleEventCuts(LL,UL);
  return 1;
}

/********************************************************/
template<typename T>
void QwCombinedBCM<T>::SetSingleEventCuts(UInt_t errorflag, Double_t LL=0, Double_t UL=0, Double_t stability=0){
  //set the unique tag to identify device type (bcm,bpm & etc)
  errorflag|=kBCMErrorFlag;//currently I use the same flag for bcm & combinedbcm
  QwMessage<<"QwCombinedBCM Error Code passing to QwVQWK_Ch "<<errorflag<<QwLog::endl;
  fCombined_bcm.SetSingleEventCuts(errorflag,LL,UL,stability);

}

/********************************************************/
template<typename T>
void  QwCombinedBCM<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (this->GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fCombined_bcm.ConstructHistograms(folder, prefix);
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
      fCombined_bcm.FillHistograms();
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
      fCombined_bcm.ConstructBranchAndVector(tree,prefix,values);
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
      fCombined_bcm.ConstructBranch(tree,prefix);
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
	fCombined_bcm.ConstructBranch(tree,prefix);
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
      fCombined_bcm.FillTreeVector(values);
    }
  return;
}


/********************************************************/
template<typename T>
void  QwCombinedBCM<T>::Copy(VQwDataElement *source)
{
  try
    {
      if(typeid(*source)==typeid(*this))
	{
	  QwCombinedBCM<T>* input=((QwCombinedBCM<T>*)source);
	  this->fElementName=input->fElementName;
	  this->fCombined_bcm.Copy(&(input->fCombined_bcm));
	}
      else
	{
	  TString loc="Standard exception from QwCombinedBCM::Copy = "
	    +source->GetElementName()+" "
	    +this->GetElementName()+" are not of the same type";
	  throw std::invalid_argument(loc.Data());
	}
    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

  return;
}



/********************************************************/
template<typename T>
std::vector<QwDBInterface> QwCombinedBCM<T>::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  QwDBInterface row;

  TString name;
  UInt_t beam_n        = 0;
  Double_t avg         = 0.0;
  Double_t err         = 0.0;

  // The element name and the n (number of measurements in average)
  // is the same for each Subelement

  name          =  fCombined_bcm.GetElementName();
  beam_n        =  fCombined_bcm.GetGoodEventCount();

  //  Loop over subelements and build the list.
  for(UInt_t beam_subblock=0; 
      beam_subblock<fCombined_bcm.GetNumberOfSubelements();
      beam_subblock++) {
    row.Reset();
    avg           =  fCombined_bcm.GetValue(beam_subblock);
    err           =  fCombined_bcm.GetValueError(beam_subblock);
    row.SetDetectorName(name);
    row.SetSubblock(beam_subblock);
    row.SetN(beam_n);
    row.SetValue(avg);
    row.SetError(err);
    row_list.push_back(row);
  }

  return row_list;

}

template class QwCombinedBCM<QwVQWK_Channel>; 
template class QwCombinedBCM<QwSIS3801_Channel>; 
template class QwCombinedBCM<QwSIS3801D24_Channel>; 
