/**********************************************************\
* File: QwIntegrationPMT.cc                               *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwIntegrationPMT.h"
#include "QwHistogramHelper.h"
#include <stdexcept>




/********************************************************/
void QwIntegrationPMT::SetPedestal(Double_t pedestal)
{
	fPedestal=pedestal;
	fTriumf_ADC.SetPedestal(fPedestal);
	return;
};

void QwIntegrationPMT::SetCalibrationFactor(Double_t calib)
{
	fCalibration=calib;
	fTriumf_ADC.SetCalibrationFactor(fCalibration); 
	return;
};
/********************************************************/ 
void  QwIntegrationPMT::InitializeChannel(TString name, TString datatosave)
{
  SetPedestal(0.);
  SetCalibrationFactor(1.);
  fTriumf_ADC.InitializeChannel(name,datatosave);
  counter=0; //used to validate sequence number in the IsGoodEvent()
  Event_Counter=0;//used to calculate the running AVG 
  fIntegrationPMT_Running_AVG=0;
  fIntegrationPMT_Running_AVG_square=0;
  SetElementName(name);
  return;
};
/********************************************************/
void QwIntegrationPMT::ClearEventData()
{
  fTriumf_ADC.ClearEventData();
  return;
};
/********************************************************/
void QwIntegrationPMT::ReportErrorCounters(){
  fTriumf_ADC.ReportErrorCounters();
};

/********************************************************/
void QwIntegrationPMT::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fTriumf_ADC.SetRandomEventParameters(mean, sigma);
  return;
};
/********************************************************/
void QwIntegrationPMT::SetRandomEventAsymmetry(Double_t asymmetry)
{
  fTriumf_ADC.SetRandomEventAsymmetry(asymmetry);
  return;
};
/********************************************************/
void QwIntegrationPMT::RandomizeEventData(int helicity)
{
  fTriumf_ADC.RandomizeEventData(helicity);
  return;
};
/********************************************************/
void QwIntegrationPMT::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fTriumf_ADC.SetHardwareSum(hwsum, sequencenumber);
  return;
};
/********************************************************/
void QwIntegrationPMT::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fTriumf_ADC.SetEventData(block, sequencenumber);
  return;
};
/********************************************************/
void QwIntegrationPMT::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fTriumf_ADC.EncodeEventData(buffer);
};
/********************************************************/
void  QwIntegrationPMT::ProcessEvent()
{
  
  if(ApplyHWChecks())
    {
      //std::cout<<"***********8Process Event()*************"<<std::endl;
      fTriumf_ADC.ProcessEvent();
      fGoodEvent=kTRUE;
    } 
  else
    fGoodEvent=kFALSE; 
 
  return;
};
/********************************************************/
Bool_t QwIntegrationPMT::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;	

  if (fDevice_flag != -1){// if fDevice_flag is -1  then do not check for hardware check on this IntegrationPMT. Since this device is not on the event cut file.
    //fEventIsGood&=fTriumf_ADC.MatchNumberOfSamples(fSampleSize);//check the sample size is correct
    fEventIsGood&=fTriumf_ADC.ApplyHWChecks();//will check for consistancy between HWSUM and SWSUM also check for sample size

  }else
    if (bDEBUG) std::cout<<GetElementName()<<" Ignored IntegrationPMT "<<std::endl;  

  
 
  
  return fEventIsGood;  
};
/********************************************************/

Int_t QwIntegrationPMT::SetSingleEventCuts(std::vector<Double_t> & dEventCuts){//two limts and sample size
  fLLimit=dEventCuts.at(0);
  fULimit=dEventCuts.at(1);
  fDevice_flag=dEventCuts.at(2);
  //std::cout<<GetElementName()<<" IntegrationPMT fDevice_flag "<<fDevice_flag<<std::endl;
  
  return 1;
};


///* will not compile with Buddhini's code 12nov09 
void QwIntegrationPMT::SetDefaultSampleSize(Int_t sample_size){
 fTriumf_ADC.SetDefaultSampleSize((size_t)sample_size);
}
//*/  

/********************************************************/
Bool_t QwIntegrationPMT::ApplySingleEventCuts(){
  //std::cout<<" QwIntegrationPMT::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;
  //if (status)
  //std::cout<<" Seq_num (IntegrationPMT) "<<fTriumf_ADC.GetSequenceNumber()<<std::endl;

   

  if (fGoodEvent){// if the IntegrationPMT harware is good
    if (fDevice_flag==1){// if fDevice_flag==1 then perform the event cut limit test	  
    
	if (fTriumf_ADC.GetHardwareSum()<=fULimit && fTriumf_ADC.GetHardwareSum()>=fLLimit){ //I need to think about checking the limit of negative values (I think current limits have to be set backwards for -ve values)
	  Event_Counter++;//Increment the counter, it is a good event.

	  //calculate the running AVG
	  fIntegrationPMT_Running_AVG=(((Event_Counter-1)*fIntegrationPMT_Running_AVG)/Event_Counter +  fTriumf_ADC.GetHardwareSum()/Event_Counter); //this is the running avg of the IntegrationPMT
	  fIntegrationPMT_Running_AVG_square=(((Event_Counter-1)*fIntegrationPMT_Running_AVG_square)/Event_Counter +  fTriumf_ADC.GetHardwareSum()*fTriumf_ADC.GetHardwareSum()/Event_Counter); //this is the running avg squre of the IntegrationPMT
	  status&=kTRUE;
	  //std::cout<<" IntegrationPMT Sample size "<<fTriumf_ADC.GetNumberOfSamples()<<std::endl;
	}
	else{
	  fTriumf_ADC.UpdateEventCutErrorCount();//update event cut falied counts
	  if (bDEBUG) std::cout<<" evnt cut failed:-> set limit "<<fULimit<<" harware sum  "<<fTriumf_ADC.GetHardwareSum();
	  status&=kFALSE;//kTRUE;//kFALSE;
	}
      }else
	status &=kTRUE;

      //if (!status)
      // std::cout<<"QwIntegrationPMT::"<<GetElementName()<<" event cuts falied "<<std::endl;
  }
  else{
    fTriumf_ADC.UpdateHWErrorCount();//update HW falied counter
    if (bDEBUG) std::cout<<" Hardware failed ";
    status&=kFALSE;
  }
    





  return status;

};

/********************************************************/

Int_t QwIntegrationPMT::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure

  return 1;
}

/********************************************************/

Bool_t QwIntegrationPMT::CheckRunningAverages(Bool_t bDisplayAVG){
  Bool_t status;

  Double_t fRunning_sigma;

  if (!(fULimit==0 && fLLimit==0)){// if samplesize is -1 then this IntegrationPMT is not in the eventcut list ignore it.
    if (fIntegrationPMT_Running_AVG<=fULimit && fIntegrationPMT_Running_AVG>=fLLimit){
      status = kTRUE;
      fRunning_sigma=(fIntegrationPMT_Running_AVG_square-(fIntegrationPMT_Running_AVG*fIntegrationPMT_Running_AVG))/Event_Counter;
      if (bDisplayAVG)
	std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<fIntegrationPMT_Running_AVG<<" current running AVG^2: "<<fIntegrationPMT_Running_AVG_square<<"\n Uncertainty in mean "<<fRunning_sigma<<std::endl;
    }
    else{
      if (bDisplayAVG)
	std::cout<<" QwIntegrationPMT::"<<GetElementName()<<" current running AVG "<<fIntegrationPMT_Running_AVG<<" is out of range ! current running AVG^2: "<<fIntegrationPMT_Running_AVG_square<<std::endl;
      status = kFALSE;
    }
  }
  else
    status = kTRUE;
      
  return status;
};

void QwIntegrationPMT::CalculateRunningAverages(){
  
  //calculate the running AVG
  fIntegrationPMT_Running_AVG=(((Event_Counter-1)*fIntegrationPMT_Running_AVG)/Event_Counter +  fTriumf_ADC.GetHardwareSum()/Event_Counter); //this is the running avg of the IntegrationPMT
  fIntegrationPMT_Running_AVG_square=(((Event_Counter-1)*fIntegrationPMT_Running_AVG_square)/Event_Counter +  fTriumf_ADC.GetHardwareSum()*fTriumf_ADC.GetHardwareSum()/Event_Counter); //this is the running avg squre of the IntegrationPMT
 
};

/********************************************************/

void QwIntegrationPMT::ResetRunningAverages(){
  std::cout<<"QwIntegrationPMT::"<<GetElementName()<<" current running AVG "<<fIntegrationPMT_Running_AVG<<" is out of range ! current running AVG^2 "<<fIntegrationPMT_Running_AVG_square<<std::endl;
  Event_Counter=0;
  fIntegrationPMT_Running_AVG=0;
  fIntegrationPMT_Running_AVG_square=0;
};


/********************************************************/


Int_t QwIntegrationPMT::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
  fTriumf_ADC.ProcessEvBuffer(buffer,word_position_in_buffer);

  return word_position_in_buffer;
};
/********************************************************/
QwIntegrationPMT& QwIntegrationPMT::operator= (const QwIntegrationPMT &value)
{
//   std::cout<<" Here in QwIntegrationPMT::operator= \n";
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

QwIntegrationPMT& QwIntegrationPMT::operator+= (const QwIntegrationPMT &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC+=value.fTriumf_ADC;
      this->fPedestal+=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
};

QwIntegrationPMT& QwIntegrationPMT::operator-= (const QwIntegrationPMT &value)
{
  if (GetElementName()!="")
    {
      this->fTriumf_ADC-=value.fTriumf_ADC;
      this->fPedestal-=value.fPedestal;
      this->fCalibration=0;
    }
  return *this;
};


void QwIntegrationPMT::Sum(QwIntegrationPMT &value1, QwIntegrationPMT &value2){
  *this =  value1;
  *this += value2;
};

void QwIntegrationPMT::Difference(QwIntegrationPMT &value1, QwIntegrationPMT &value2){
  *this =  value1;
  *this -= value2;
};

void QwIntegrationPMT::Ratio(QwIntegrationPMT &numer, QwIntegrationPMT &denom)
{
  //  std::cout<<"QwIntegrationPMT::Ratio element name ="<<GetElementName()<<" \n";
  if (GetElementName()!="")
    {
      //  std::cout<<"here in \n";
      this->fTriumf_ADC.Ratio(numer.fTriumf_ADC,denom.fTriumf_ADC);
      this->fPedestal=0;
      this->fCalibration=0;
    }
  return;
};

void QwIntegrationPMT::Scale(Double_t factor)
{
  fTriumf_ADC.Scale(factor);
  return;
}


void QwIntegrationPMT::Print() const
{
  //std::cout<<"QwVQWK_Channel Info " <<std::endl;
  //std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<IntegrationPMT_Running_AVG<<std::endl;
  std::cout<<"QwVQWK_Channel Info " <<std::endl;
  fTriumf_ADC.Print();
  return;
}

/********************************************************/
void  QwIntegrationPMT::ConstructHistograms(TDirectory *folder, TString &prefix)
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

void  QwIntegrationPMT::FillHistograms()
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

void  QwIntegrationPMT::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
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

void  QwIntegrationPMT::FillTreeVector(std::vector<Double_t> &values)
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

void  QwIntegrationPMT::DeleteHistograms()
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
void  QwIntegrationPMT::Copy(VQwDataElement *source)
{
  try
    {
      if(typeid(*source)==typeid(*this))
	{
	  QwIntegrationPMT* input=((QwIntegrationPMT*)source);
	  this->fElementName=input->fElementName;
	  this->fPedestal=input->fPedestal;
	  this->fCalibration=input->fCalibration;
	  this->fTriumf_ADC.Copy(&(input->fTriumf_ADC));
	}
      else
	{
	  TString loc="Standard exception from QwIntegrationPMT::Copy = "
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


