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
  counter=0; //used to validate sequence number in the IsGoodEvent()
  Event_Counter=0;//used to calculate the running AVG 
  fBCM_Running_AVG=0;
  fBCM_Running_AVG_square=0;
  SetElementName(name);
  return;
};
/********************************************************/
void QwBCM::ClearEventData()
{
  fTriumf_ADC.ClearEventData();
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
void QwBCM::RandomizeEventData(int helicity)
{
  fTriumf_ADC.RandomizeEventData(helicity);
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
  
  if(ApplyHWChecks())
    {
      fTriumf_ADC.ProcessEvent();
      fGoodEvent=kTRUE;
    } 
  else
    fGoodEvent=kFALSE; 
 
  return;
};
/********************************************************/
Bool_t QwBCM::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;	

  if (fSampleSize > 0){// if samplesize is 0 then this do not check for hardware check on this BCM.
    fEventIsGood&=fTriumf_ADC.ApplyHWChecks();//will check for consistancy between HWSUM and SWSUM also check sample size is non-zero
    fEventIsGood&=fTriumf_ADC.MatchNumberOfSamples(fSampleSize);//check the sample size is correct
    //Also need to check sequence number 
    if (fEventIsGood){
      fSequenceNo_Prev++;
      if (counter==0 || fTriumf_ADC.GetSequenceNumber()==0){//starting the data run 
	fSequenceNo_Prev=fTriumf_ADC.GetSequenceNumber();     
      }

      if (!fTriumf_ADC.MatchSequenceNumber(fSequenceNo_Prev)){//we have a sequence number error
	fEventIsGood&=kFALSE;       
	std::cout<<" QwBCM "<<GetElementName()<<" Sequence number is not incrementing properly; previous value = "<<fSequenceNo_Prev<<" Current value= "<< fTriumf_ADC.GetSequenceNumber()<<std::endl;     
      }
   
      counter++;

      //Checking for HW_sum is returning same value.

      if (fPrevious_HW_Sum != fTriumf_ADC.GetRawHardwareSum()){
	//std::cout<<" BCM hardware sum is different  "<<std::endl;
	fPrevious_HW_Sum = fTriumf_ADC.GetRawHardwareSum();
	fHW_Sum_Stuck_Counter=0;
      }else
	fHW_Sum_Stuck_Counter++;//hw_sum is same increment the counter

      //check for the hw_sum is giving the same value
      if (fHW_Sum_Stuck_Counter>0){
	std::cout<<" BCM hardware sum is same for more than  "<<fHW_Sum_Stuck_Counter<<" time consecutively  "<<std::endl;
	fEventIsGood&=kFALSE;
      }



      
    }else{
      //std::cout<<" QwBCM "<<GetElementName()<<" QwVQWK_Channel error "<<std::endl;
      counter=0;//resetting the counter after IsGoodEvent() a faliure 
    }
  }else
    std::cout<<GetElementName()<<" Ignored BCM "<<std::endl;  

  
 
  
  return fEventIsGood;  
};
/********************************************************/

Int_t QwBCM::SetSingleEventCuts(std::vector<Double_t> & dEventCuts){//two limts and sample size
  fLLimit=dEventCuts.at(0);
  fULimit=dEventCuts.at(1);
  fSampleSize=(Int_t)dEventCuts.at(2);
  
  return 1;
};
  

/********************************************************/
Bool_t QwBCM::ApplySingleEventCuts(){
  //std::cout<<" QwBCM::SingleEventCuts() "<<std::endl;
  Bool_t status=kTRUE;
  //if (status)
  //std::cout<<" Seq_num (BCM) "<<fTriumf_ADC.GetSequenceNumber()<<std::endl;

   

  if (fGoodEvent){// if the BCM harware is good
    if (!(fULimit==0 && fLLimit==0)){//  if fULimit & fLLimit  are non  zero then perform event cuts on this BCM.

	  
    
	if (fTriumf_ADC.GetHardwareSum()<=fULimit && fTriumf_ADC.GetHardwareSum()>=fLLimit){ //I need to think about checking the limit of negative values (I think current limits have to be set backwards for -ve values)
	  Event_Counter++;//Increment the counter, it is a good event.

	  //calculate the running AVG
	  fBCM_Running_AVG=(((Event_Counter-1)*fBCM_Running_AVG)/Event_Counter +  fTriumf_ADC.GetHardwareSum()/Event_Counter); //this is the running avg of the BCM
	  fBCM_Running_AVG_square=(((Event_Counter-1)*fBCM_Running_AVG_square)/Event_Counter +  fTriumf_ADC.GetHardwareSum()*fTriumf_ADC.GetHardwareSum()/Event_Counter); //this is the running avg squre of the BCM
	  status&=kTRUE;
	  //std::cout<<" BCM Sample size "<<fTriumf_ADC.GetNumberOfSamples()<<std::endl;
	}
	else{
	  std::cout<<" evnt cut failed:-> set limit "<<fULimit<<" harware sum  "<<fTriumf_ADC.GetHardwareSum();
	  status&=kFALSE;//kTRUE;//kFALSE;
	}
      }else
	status &=kTRUE;

      //if (!status)
      // std::cout<<"QwBCM::"<<GetElementName()<<" event cuts falied "<<std::endl;
  }
  else{
    std::cout<<" Hardware failed ";
    status&=kFALSE;
  }
    





  return status;

};

/********************************************************/

Int_t QwBCM::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure

  return 1;
}

/********************************************************/

Bool_t QwBCM::CheckRunningAverages(Bool_t bDisplayAVG){
  Bool_t status;

  Double_t fRunning_sigma;

  if (!(fULimit==0 && fLLimit==0)){// if samplesize is -1 then this BCM is not in the eventcut list ignore it.
    if (fBCM_Running_AVG<=fULimit && fBCM_Running_AVG>=fLLimit){
      status = kTRUE;
      fRunning_sigma=(fBCM_Running_AVG_square-(fBCM_Running_AVG*fBCM_Running_AVG))/Event_Counter;
      if (bDisplayAVG)
	std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<fBCM_Running_AVG<<" current running AVG^2: "<<fBCM_Running_AVG_square<<"Uncertainty in mean "<<fRunning_sigma<<std::endl;
    }
    else{
      if (bDisplayAVG)
	std::cout<<"QwBCM::"<<GetElementName()<<" current running AVG "<<fBCM_Running_AVG<<" is out of range ! current running AVG^2: "<<fBCM_Running_AVG_square<<std::endl;
      status = kFALSE;
    }
  }
  else
    status = kTRUE;
      
  return status;
};

void QwBCM::CalculateRunningAverages(){
  
  //calculate the running AVG
  fBCM_Running_AVG=(((Event_Counter-1)*fBCM_Running_AVG)/Event_Counter +  fTriumf_ADC.GetHardwareSum()/Event_Counter); //this is the running avg of the BCM
  fBCM_Running_AVG_square=(((Event_Counter-1)*fBCM_Running_AVG_square)/Event_Counter +  fTriumf_ADC.GetHardwareSum()*fTriumf_ADC.GetHardwareSum()/Event_Counter); //this is the running avg squre of the BCM
 
};

/********************************************************/

void QwBCM::ResetRunningAverages(){
  std::cout<<"QwBCM::"<<GetElementName()<<" current running AVG "<<fBCM_Running_AVG<<" is out of range ! current running AVG^2 "<<fBCM_Running_AVG_square<<std::endl;
  Event_Counter=0;
  fBCM_Running_AVG=0;
  fBCM_Running_AVG_square=0;
};


/********************************************************/


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
}


