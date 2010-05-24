/**********************************************************\
* File: QwBCM.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBCM.h"
#include "QwHistogramHelper.h"
#include <stdexcept>




/********************************************************/
void QwBCM::SetPedestal(Double_t pedestal)
{
	fPedestal=pedestal;
	fTriumf_ADC.SetPedestal(fPedestal);
	return;
};

void QwBCM::SetCalibrationFactor(Double_t calib)
{
	fCalibration=calib;
	fTriumf_ADC.SetCalibrationFactor(fCalibration);
	return;
};
/********************************************************/
void  QwBCM::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(name,datatosave);
  SetElementName(name);
  //set default limits to event cuts
  fLLimit=0;//init two timits
  fULimit=0;//init two timits
  return;
};
/********************************************************/
void QwBCM::ClearEventData()
{
  fTriumf_ADC.ClearEventData();
  return;
};


/********************************************************/
void QwBCM::UseExternalRandomVariable()
{
  fTriumf_ADC.UseExternalRandomVariable();
  return;
};
/********************************************************/
void QwBCM::SetExternalRandomVariable(double random_variable)
{
  fTriumf_ADC.SetExternalRandomVariable(random_variable);
  return;
};
/********************************************************/
void QwBCM::SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.SetRandomEventDriftParameters(amplitude, phase, frequency);
  return;
};
/********************************************************/
void QwBCM::AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency)
{
  fTriumf_ADC.AddRandomEventDriftParameters(amplitude, phase, frequency);
  return;
};
/********************************************************/
void QwBCM::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fTriumf_ADC.SetRandomEventParameters(mean, sigma);
  return;
};
/********************************************************/
void QwBCM::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fTriumf_ADC.SetRandomEventAsymmetry(asymmetry);
  return;
};
/********************************************************/
void QwBCM::RandomizeEventData(int helicity, double time)
{
  fTriumf_ADC.RandomizeEventData(helicity, time);
  return;
};
/********************************************************/
void QwBCM::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fTriumf_ADC.SetHardwareSum(hwsum, sequencenumber);
  return;
};
/********************************************************/
void QwBCM::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fTriumf_ADC.SetEventData(block, sequencenumber);
  return;
};
/********************************************************/
void QwBCM::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fTriumf_ADC.EncodeEventData(buffer);
};
/********************************************************/
void  QwBCM::ProcessEvent()
{
  ApplyHWChecks();//first apply HW checks and update HW  error flags. Calling this routine either in ApplySingleEventCuts or here do not make any difference for a BCM but do for a BPMs because they have derrived devices.
  fTriumf_ADC.ProcessEvent();
  //update the event cut counters
  fTriumf_ADC.UpdateHWErrorCounters();
  return;
};
/********************************************************/
Bool_t QwBCM::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

  fDeviceErrorCode=fTriumf_ADC.ApplyHWChecks();//will check for HW consistancy and return the error code (=0 is HW good)
  fEventIsGood=(fDeviceErrorCode & 0x0);//if no HW error return true


  return fEventIsGood;
};
/********************************************************/

Int_t QwBCM::SetSingleEventCuts(Double_t LL=0, Double_t UL=0){//std::vector<Double_t> & dEventCuts){//two limts and sample size
  fTriumf_ADC.SetSingleEventCuts(LL,UL);
  return 1;
};

void QwBCM::SetDefaultSampleSize(Int_t sample_size){
  fTriumf_ADC.SetDefaultSampleSize((size_t)sample_size);
}


/********************************************************/
Bool_t QwBCM::ApplySingleEventCuts(){
  //std::cout<<" QwBCM::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;


  if (fTriumf_ADC.ApplySingleEventCuts()){
    status=kTRUE;
  }
  else{
    fTriumf_ADC.UpdateEventCutErrorCount();//update event cut falied counts
    if (bDEBUG) std::cout<<" evnt cut failed:-> set limit "<<fULimit<<" harware sum  "<<fTriumf_ADC.GetHardwareSum();
    status&=kFALSE;
  }
  fDeviceErrorCode|=fTriumf_ADC.GetEventcutErrorFlag();//retrun the error flag for event cuts


  return status;

};

/********************************************************/

Int_t QwBCM::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure
  fTriumf_ADC.GetEventcutErrorCounters();
  return 1;
}



Int_t QwBCM::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  fTriumf_ADC.ProcessEvBuffer(buffer,word_position_in_buffer);

  return word_position_in_buffer;
};
/********************************************************/
QwBCM& QwBCM::operator= (const QwBCM &value)
{
//   std::cout<<" Here in QwBCM::operator= \n";
  if (GetElementName()!="")
    {
      this->fTriumf_ADC=value.fTriumf_ADC;
      this->fPedestal=value.fPedestal;
      this->fCalibration=value.fCalibration;
    }
//   std::cout<<" to be copied \n";
//   value.Print();
//   std::cout<<" copied \n";
//   this->Print();

  return *this;
};

QwBCM& QwBCM::operator+= (const QwBCM &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC+=value.fTriumf_ADC;
      this->fPedestal+=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
};

QwBCM& QwBCM::operator-= (const QwBCM &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC-=value.fTriumf_ADC;
      this->fPedestal-=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
};


void QwBCM::Sum(QwBCM &value1, QwBCM &value2){
  *this =  value1;
  *this += value2;
};

void QwBCM::Difference(QwBCM &value1, QwBCM &value2){
  *this =  value1;
  *this -= value2;
};

void QwBCM::Ratio(QwBCM &numer, QwBCM &denom)
{
  //  std::cout<<"QwBCM::Ratio element name ="<<GetElementName()<<" \n";
  if (GetElementName()!="")
    {
      //  std::cout<<"here in \n";
      this->fTriumf_ADC.Ratio(numer.fTriumf_ADC,denom.fTriumf_ADC);
      this->fPedestal=0;
      this->fCalibration=0;
    }
  return;
};

void QwBCM::Scale(Double_t factor)
{
  fTriumf_ADC.Scale(factor);
  return;
}

void QwBCM::CalculateRunningAverage() {
  fTriumf_ADC.CalculateRunningAverage();
};

void QwBCM::AccumulateRunningSum(const QwBCM& value) {
  fTriumf_ADC.AccumulateRunningSum(value.fTriumf_ADC);
};

void QwBCM::Print() const
{
  //std::cout<<"QwVQWK_Channel Info " <<std::endl;
  //std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<BCM_Running_AVG<<std::endl;
  std::cout<<"QwVQWK_Channel Info " <<std::endl;
  fTriumf_ADC.Print();
  return;
}

/********************************************************/
void  QwBCM::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fTriumf_ADC.ConstructHistograms(folder, prefix);
    }
  return;
};

void  QwBCM::FillHistograms()
{
  if (GetElementName()=="")
    {
      //  This channel is not used, so skip filling the histograms.
    }
  else
    {
      fTriumf_ADC.FillHistograms();
    }


  return;
};

void  QwBCM::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.ConstructBranchAndVector(tree, prefix,values);
      // this functions doesn't do anything yet
    }
  return;
};

void  QwBCM::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.FillTreeVector(values);
      // this functions doesn't do anything yet
    }
  return;
};

void  QwBCM::DeleteHistograms()
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fTriumf_ADC.DeleteHistograms();
    }
  return;
};
/********************************************************/
void  QwBCM::Copy(VQwDataElement *source)
{
  try
    {
      if(typeid(*source)==typeid(*this))
	{
	  QwBCM* input=((QwBCM*)source);
	  this->fElementName=input->fElementName;
	  this->fPedestal=input->fPedestal;
	  this->fCalibration=input->fCalibration;
	  this->fTriumf_ADC.Copy(&(input->fTriumf_ADC));
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
};




QwDBInterface QwBCM::GetDBEntry(TString subname)
{
  QwDBInterface row;

  TString name;
  Double_t avg         = 0.0;
  Double_t err         = 0.0;
  UInt_t beam_subblock = 0;
  UInt_t beam_n        = 0;

  name          = this->GetElementName();
  avg           = this->GetAverage();
  err           = this->GetAverageError();
  beam_subblock = 9;// no meaning, later will be replaced with a real one
  beam_n        = this->GetGoodEventCount();


  row.SetDetectorName(name);
  row.SetSubblock(beam_subblock);
  row.SetN(beam_n);
  row.SetValue(avg);
  row.SetError(err);


  return row;

};
