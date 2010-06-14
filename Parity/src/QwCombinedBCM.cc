/**********************************************************\
* File: QwCombinedBCM.cc                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwCombinedBCM.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

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
};

template<typename T>
void QwCombinedBCM<T>::SetCalibrationFactor(Double_t calib)
{
	fCombined_bcm.SetCalibrationFactor(1);
	return;
};

template<typename T>
void QwCombinedBCM<T>::Set(QwBCM<T>* bcm, Double_t weight, Double_t sumqw ){

  fElement.push_back(bcm);
  fWeights.push_back(weight);
  fSumQweights=sumqw;
  //std::cout<<"QwCombinedBCM: Got "<<bcm->GetElementName()<<"  and weight ="<<weight<<"\n";
  }

template<typename T>
void  QwCombinedBCM<T>::InitializeChannel(TString name, TString datatosave)
{
  SetElementName(name);
  fCombined_bcm.InitializeChannel(name,"derived");

  return;
};

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
};

/********************************************************/
template<typename T>
void QwCombinedBCM<T>::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fCombined_bcm.SetRandomEventParameters(mean, sigma);
  return;
};


// void QwCombinedBCM::SetRandomEventAsymmetry(Double_t asymmetry)
// {
//   fCombined_bcm.SetRandomEventAsymmetry(asymmetry);
//   return;
// };

template<typename T>
void QwCombinedBCM<T>::RandomizeEventData(int helicity)
{
  fCombined_bcm.RandomizeEventData(helicity);
  return;
};
/********************************************************/
template<typename T>
void QwCombinedBCM<T>::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fCombined_bcm.SetHardwareSum(hwsum, sequencenumber);
  return;
};


template<typename T>
void QwCombinedBCM<T>::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fCombined_bcm.SetEventData(block, sequencenumber);
  return;
};
/********************************************************/
template<typename T>
void QwCombinedBCM<T>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fCombined_bcm.EncodeEventData(buffer);
};



/********************************************************/
template<typename T>
void  QwCombinedBCM<T>::ProcessEvent()
{

  Bool_t ldebug = kFALSE;
  static QwVQWK_Channel  tmpADC; 
  tmpADC.InitializeChannel("tmpADC","derived");


  for(size_t i=0;i<fElement.size();i++)
  {
    tmpADC=fElement[i]->fBeamCurrent;
    tmpADC.Scale(fWeights[i]);
    fCombined_bcm+=tmpADC;

  }

  fCombined_bcm.Scale(1.0/fSumQweights);


  if(ldebug){
    std::cout<<"***************** \n";
    std::cout<<"QwCombinedBCM: "<<GetElementName()
	     <<"\nweighted average of hardware sums = "<<fCombined_bcm.GetHardwareSum()<<"\n";
    for(size_t i=0;i<4;i++){
      std::cout<<"weighted average of block["<<i<<"] = "<<fCombined_bcm.GetBlockValue(i)<<"\n";
    }
    std::cout<<"***************** \n";
  }

  return;
};


template<typename T>
void QwCombinedBCM<T>::SetDefaultSampleSize(Int_t sample_size){
  fCombined_bcm.SetDefaultSampleSize((size_t)sample_size);
}

/********************************************************/
template<typename T>
Bool_t QwCombinedBCM<T>::ApplySingleEventCuts(){
  Bool_t status=kTRUE;
  if (fCombined_bcm.ApplySingleEventCuts()){
    status=kTRUE;
  }
  else{
    fCombined_bcm.UpdateEventCutErrorCount();//update event cut falied counts
    if (bDEBUG) std::cout<<" evnt cut failed:-> set limit "<<fULimit<<" harware sum  "<<fCombined_bcm.GetHardwareSum();
    status&=kFALSE;
  }
  fDeviceErrorCode|=fCombined_bcm.GetEventcutErrorFlag();//retrun the error flag for event cuts
  //std::cout<<"combined bcm "<<GetElementName()<<" error flag "<<fCombined_bcm.GetEventcutErrorFlag()<<std::endl;
  //Update the error counters
  fCombined_bcm.UpdateHWErrorCounters();

  return status;

};

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
};

/********************************************************/

template<typename T>
void QwCombinedBCM<T>::AccumulateRunningSum(const QwCombinedBCM<T>& value){
  fCombined_bcm.AccumulateRunningSum(value.fCombined_bcm);
};

/********************************************************/


template<typename T>
Int_t QwCombinedBCM<T>::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
 //  fCombined_bcm.ProcessEvBuffer(buffer,word_position_in_buffer);

//   return word_position_in_buffer;
  return 0;
};
/********************************************************/
template<typename T>
QwCombinedBCM<T>& QwCombinedBCM<T>::operator= (const QwCombinedBCM<T> &value)
{
  if (GetElementName()!="")
    this->fCombined_bcm=value.fCombined_bcm;

  return *this;
};

template<typename T>
QwCombinedBCM<T>& QwCombinedBCM<T>::operator+= (const QwCombinedBCM<T> &value)
{
  if (GetElementName()!="")
    this->fCombined_bcm+=value.fCombined_bcm;
  return *this;
};

template<typename T>
QwCombinedBCM<T>& QwCombinedBCM<T>::operator-= (const QwCombinedBCM<T> &value)
{
  if (GetElementName()!="")
    this->fCombined_bcm-=value.fCombined_bcm;

  return *this;
};


template<typename T>
void QwCombinedBCM<T>::Sum(QwCombinedBCM<T> &value1, QwCombinedBCM<T> &value2){
  *this =  value1;
  *this += value2;
};

template<typename T>
void QwCombinedBCM<T>::Difference(QwCombinedBCM<T> &value1, QwCombinedBCM<T> &value2){
  *this =  value1;
  *this -= value2;
};

template<typename T>
void QwCombinedBCM<T>::Ratio(QwCombinedBCM<T> &numer, QwCombinedBCM<T> &denom)
{
  if (GetElementName()!="")
    this->fCombined_bcm.Ratio(numer.fCombined_bcm,denom.fCombined_bcm);

  return;
};

template<typename T>
void QwCombinedBCM<T>::Scale(Double_t factor)
{
  fCombined_bcm.Scale(factor);
  return;
}


template<typename T>
void QwCombinedBCM<T>::Print() const
{
  //std::cout<<"QwVQWK_Channel Info " <<std::endl;
  //std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<BCM_Running_AVG<<std::endl;
  std::cout<<"QwVQWK_Channel Info " <<std::endl;
  fCombined_bcm.Print();
  return;
}

/********************************************************/
template<typename T>
Bool_t QwCombinedBCM<T>::ApplyHWChecks()
{
  // For the combined devices there are no physical channels that we can relate to because they  are being
  // derived from combinations of physical channels. Therefore, this is not exactly a "HW Check"
  // but just a check of the HW checks of the combined channels.

  Bool_t fEventIsGood=kTRUE;

//   fDeviceErrorCode=0;
//   for(int i=0;i<4;i++)
//     {
//       fDeviceErrorCode|= fCombinedWire[i].ApplyHWChecks();  //OR the error code from each wire
//       fEventIsGood &= (fDeviceErrorCode & 0x0);//AND with 0 since zero means HW is good.

//       if (bDEBUG) std::cout<<" Inconsistent within BPM terminals wire[ "<<i<<" ] "<<std::endl;
//       if (bDEBUG) std::cout<<" wire[ "<<i<<" ] sequence num "<<fCombinedWire[i].GetSequenceNumber()<<" sample size "<<fWire[i].GetNumberOfSamples()<<std::endl;
//     }

  return fEventIsGood;
};

template<typename T>
Int_t QwCombinedBCM<T>::SetSingleEventCuts(Double_t LL=0, Double_t UL=0){
  fCombined_bcm.SetSingleEventCuts(LL,UL);
  return 1;
};

/********************************************************/
template<typename T>
void  QwCombinedBCM<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fCombined_bcm.ConstructHistograms(folder, prefix);
    }
  return;

};

template<typename T>
void  QwCombinedBCM<T>::FillHistograms()
{
 if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fCombined_bcm.FillHistograms();
    }


  return;
};

template<typename T>
void  QwCombinedBCM<T>::DeleteHistograms()
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fCombined_bcm.DeleteHistograms();
    }
  return;
};

template<typename T>
void  QwCombinedBCM<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fCombined_bcm.ConstructBranchAndVector(tree,prefix,values);
    }
  return;
};

template<typename T>
void  QwCombinedBCM<T>::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fCombined_bcm.FillTreeVector(values);
    }
  return;
};


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


template class QwCombinedBCM<QwVQWK_Channel>; 
//template class QwCombinedBCM<QwSIS3801_Channel>; 
