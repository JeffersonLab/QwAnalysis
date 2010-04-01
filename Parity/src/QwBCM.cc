/**********************************************************\
* File: QwBCM.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBCM.h"
#include "QwHistogramHelper.h"
#include <stdexcept>


#include "QwVQWK_Channel.h"
#include "QwSIS3801_Channel.h"



/********************************************************/
template<typename T>
void QwBCM<T>::SetPedestal(Double_t pedestal)
{
	fPedestal=pedestal;
	fBeamCurrent.SetPedestal(fPedestal);
	return;
};

template<typename T>
void QwBCM<T>::SetCalibrationFactor(Double_t calib)
{
	fCalibration=calib;
	fBeamCurrent.SetCalibrationFactor(fCalibration);
	return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fBeamCurrent.InitializeChannel(name,datatosave);
  this->SetElementName(name);
  //set default limits to event cuts
  fLLimit=0;//init two timits
  fULimit=0;//init two timits
  return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::ClearEventData()
{
  fBeamCurrent.ClearEventData();
  return;
};


/********************************************************/
template<typename T>
void QwBCM<T>::SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fBeamCurrent.SetRandomEventDriftParameters(amplitude, phase, frequency);
  return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fBeamCurrent.AddRandomEventDriftParameters(amplitude, phase, frequency);
  return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fBeamCurrent.SetRandomEventParameters(mean, sigma);
  return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fBeamCurrent.SetRandomEventAsymmetry(asymmetry);
  return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::RandomizeEventData(int helicity)
{
  fBeamCurrent.RandomizeEventData(helicity);
  return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fBeamCurrent.SetHardwareSum(hwsum, sequencenumber);
  return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fBeamCurrent.SetEventData(block, sequencenumber);
  return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::SetEventNumber(int event)
{
  fBeamCurrent.SetEventNumber(event);
  return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fBeamCurrent.EncodeEventData(buffer);
};
/********************************************************/
template<typename T>
void QwBCM<T>::ProcessEvent()
{
  this->ApplyHWChecks();//first apply HW checks and update HW  error flags. Calling this routine either in ApplySingleEventCuts or here do not make any difference for a BCM but do for a BPMs because they have derrived devices.
  fBeamCurrent.ProcessEvent();
  return;
};
/********************************************************/
template<typename T>
Bool_t QwBCM<T>::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

  fDeviceErrorCode=fBeamCurrent.ApplyHWChecks();//will check for HW consistancy and return the error code (=0 is HW good)
  fEventIsGood=(fDeviceErrorCode & 0x0);//if no HW error return true


  return fEventIsGood;
};
/********************************************************/

template<typename T>
Int_t QwBCM<T>::SetSingleEventCuts(Double_t LL=0, Double_t UL=0){//std::vector<Double_t> & dEventCuts){//two limts and sample size
  fLLimit=LL;
  fULimit=UL;
  return 1;
};

template<typename T>
void QwBCM<T>::SetDefaultSampleSize(Int_t sample_size){
  fBeamCurrent.SetDefaultSampleSize((size_t)sample_size);
}


/********************************************************/
template<typename T>
Bool_t QwBCM<T>::ApplySingleEventCuts(){
  //std::cout<<" QwBCM::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;


  if (fBeamCurrent.ApplySingleEventCuts(fLLimit,fULimit)){
    status=kTRUE;
  }
  else{
    fBeamCurrent.UpdateEventCutErrorCount();//update event cut falied counts
    if (bDEBUG) std::cout<<" evnt cut failed:-> set limit "<<fULimit<<" harware sum  "<<fBeamCurrent.GetHardwareSum();
    status&=kFALSE;
  }
  fDeviceErrorCode|=fBeamCurrent.GetEventcutErrorFlag();//retrun the error flag for event cuts


  return status;

};

/********************************************************/

template<typename T>
Int_t QwBCM<T>::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure
  fBeamCurrent.GetEventcutErrorCounters();
  return 1;
}



template<typename T>
Int_t QwBCM<T>::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  fBeamCurrent.ProcessEvBuffer(buffer,word_position_in_buffer);

  return word_position_in_buffer;
};
/********************************************************/
template<typename T>
QwBCM<T>& QwBCM<T>::operator= (const QwBCM<T> &value)
{
//   std::cout<<" Here in QwBCM::operator= \n";
  if (this->GetElementName()!="")
    {
      this->fBeamCurrent=value.fBeamCurrent;
      this->fPedestal=value.fPedestal;
      this->fCalibration=value.fCalibration;
    }
//   std::cout<<" to be copied \n";
//   value.Print();
//   std::cout<<" copied \n";
//   this->Print();

  return *this;
};

template<typename T>
QwBCM<T>& QwBCM<T>::operator+= (const QwBCM<T> &value)
{
  if (this->GetElementName()!="")
    {
      this->fBeamCurrent+=value.fBeamCurrent;
      this->fPedestal+=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
};

template<typename T>
QwBCM<T>& QwBCM<T>::operator-= (const QwBCM<T> &value)
{
  if (this->GetElementName()!="")
    {
      this->fBeamCurrent-=value.fBeamCurrent;
      this->fPedestal-=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
};


template<typename T>
void QwBCM<T>::Sum(QwBCM<T> &value1, QwBCM<T> &value2){
  *this =  value1;
  *this += value2;
};

template<typename T>
void QwBCM<T>::Difference(QwBCM<T> &value1, QwBCM<T> &value2){
  *this =  value1;
  *this -= value2;
};

template<typename T>
void QwBCM<T>::Ratio(QwBCM<T> &numer, QwBCM<T> &denom)
{
  //  std::cout<<"QwBCM::Ratio element name ="<<GetElementName()<<" \n";
  if (this->GetElementName()!="")
    {
      //  std::cout<<"here in \n";
      this->fBeamCurrent.Ratio(numer.fBeamCurrent,denom.fBeamCurrent);
      this->fPedestal=0;
      this->fCalibration=0;
    }
  return;
};

template<typename T>
void QwBCM<T>::Scale(Double_t factor)
{
  fBeamCurrent.Scale(factor);
  return;
}

template<typename T>
void QwBCM<T>::Calculate_Running_Average(){
  fBeamCurrent.Calculate_Running_Average();
};

template<typename T>
void QwBCM<T>::Do_RunningSum(){
  fBeamCurrent.Do_RunningSum();
};

template<typename T>
void QwBCM<T>::Print() const
{
  //std::cout<<"Channel Info " <<std::endl;
  //std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<BCM_Running_AVG<<std::endl;
  std::cout<<"Channel Info " <<std::endl;
  fBeamCurrent.Print();
  return;
}

/********************************************************/
template<typename T>
void QwBCM<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
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
};

template<typename T>
void QwBCM<T>::FillHistograms()
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
};

template<typename T>
void QwBCM<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fBeamCurrent.ConstructBranchAndVector(tree, prefix,values);
      // this functions doesn't do anything yet
    }
  return;
};

template<typename T>
void QwBCM<T>::FillTreeVector(std::vector<Double_t> &values)
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fBeamCurrent.FillTreeVector(values);
      // this functions doesn't do anything yet
    }
  return;
};

template<typename T>
void QwBCM<T>::DeleteHistograms()
{
  if (this->GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fBeamCurrent.DeleteHistograms();
    }
  return;
};
/********************************************************/
template<typename T>
void QwBCM<T>::Copy(VQwDataElement *source)
{
  try
    {
      if(typeid(*source)==typeid(*this))
	{
	  QwBCM* input=((QwBCM*)source);
	  this->fElementName=input->fElementName;
	  this->fPedestal=input->fPedestal;
	  this->fCalibration=input->fCalibration;
	  this->fBeamCurrent.Copy(&(input->fBeamCurrent));
	}
      else
	{
	  TString loc="Standard exception from QwBCM::Copy = "
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



template<typename T>
QwParityDB::beam QwBCM<T>::GetDBEntry(QwDatabase *db, TString mtype, TString subname)
{
  QwParityDB::beam row(0);
  
  UInt_t beam_run_id      = 0;
  UInt_t beam_analysis_id = 0;
  UInt_t beam_monitor_id  = 0;
  Char_t beam_measurement_type[4];

  TString name;
  Double_t avg = 0.0;
  Double_t err = 0.0;

  if(mtype.Contains("yield"))
    {
      sprintf(beam_measurement_type, "yq");
    }
  else if(mtype.Contains("asymmetry"))
    {
      sprintf(beam_measurement_type, "aq");
    }
  else if(mtype.Contains("average") )
    {
      sprintf(beam_measurement_type, "yq");
    }
  else if(mtype.Contains("runningsum"))
    {
      sprintf(beam_measurement_type, "yq");
    }
  else
    {
      sprintf(beam_measurement_type, "null");
    }
  
  name = this->GetElementName();
  avg  = this->GetAverage("");
  err  = this->GetAverageError("");

  beam_run_id      = db->GetRunID();
  beam_analysis_id = db->GetAnalysisID();
  beam_monitor_id  = db->GetMonitorID(name.Data());

  row.analysis_id         = beam_analysis_id;
  row.measurement_type_id = beam_measurement_type;
  row.monitor_id          = beam_monitor_id;
  row.value               = avg;
  row.error               = err;

  printf("%12s::RunID %d AnalysisID %d %4s MonitorID %4d %18s , [%18.2e, %12.2e] \n", 
	 mtype.Data(), beam_run_id, beam_analysis_id, beam_measurement_type, beam_monitor_id, name.Data(),  avg, err);
  
  return row;
  
};


template class QwBCM<QwVQWK_Channel>; 
//template class QwBCM<QwSIS3801_Channel>; 
