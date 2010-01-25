/**********************************************************\
* File: QwCombinedBCM.cc                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwCombinedBCM.h"
#include "QwHistogramHelper.h"
#include <stdexcept>




/********************************************************/

//this is a combined BCM made out of BCMs that are already callibrated and have pedstals removed.
//This will be used for linear regression and for projection of charge at the target

// void QwBCM::SetPedestal(Double_t pedestal)
// {
// 	fPedestal=pedestal;
// 	fTriumf_ADC.SetPedestal(0);
// 	return;
// };


void QwCombinedBCM::SetPedestal(Double_t pedestal)
{
	fCombined_bcm.SetPedestal(0);
	return;
};

void QwCombinedBCM::SetCalibrationFactor(Double_t calib)
{
	fCombined_bcm.SetCalibrationFactor(1); 
	return;
};

void QwCombinedBCM::Add(QwBCM* bcm, Double_t weight  ){

  fElement.push_back(bcm);
  fWeights.push_back(weight);

  //std::cout<<"QwCombinedBCM: Got "<<bcm->GetElementName()<<"  and weight ="<<weight<<"\n";
  }
  
void  QwCombinedBCM::InitializeChannel(TString name, TString datatosave)
{
  SetElementName(name); 
  fCombined_bcm.InitializeChannel(name,"derived"); 

  return;
};

void QwCombinedBCM::ClearEventData()
{
  fCombined_bcm.ClearEventData();  
  return; 
}


void QwCombinedBCM::ReportErrorCounters()
{
  //fCombined_bcm.ReportErrorCounters();
};

/********************************************************/
void QwCombinedBCM::SetRandomEventParameters(Double_t mean, Double_t sigma)
{
  fCombined_bcm.SetRandomEventParameters(mean, sigma);
  return;
};


// void QwCombinedBCM::SetRandomEventAsymmetry(Double_t asymmetry)
// {
//   fCombined_bcm.SetRandomEventAsymmetry(asymmetry);
//   return;
// };

void QwCombinedBCM::RandomizeEventData(int helicity)
{
  fCombined_bcm.RandomizeEventData(helicity);
  return;
};
/********************************************************/
void QwCombinedBCM::SetHardwareSum(Double_t hwsum, UInt_t sequencenumber)
{
  fCombined_bcm.SetHardwareSum(hwsum, sequencenumber);
  return;
};


void QwCombinedBCM::SetEventData(Double_t* block, UInt_t sequencenumber)
{
  fCombined_bcm.SetEventData(block, sequencenumber);
  return;
};
/********************************************************/
void QwCombinedBCM::EncodeEventData(std::vector<UInt_t> &buffer)
{
  fCombined_bcm.EncodeEventData(buffer);
};



/********************************************************/
void  QwCombinedBCM::ProcessEvent()
{
 
  Bool_t ldebug = kFALSE;
  static QwVQWK_Channel  tmpADC; 
  Double_t  total_weights=0;

  fCombined_bcm.ClearEventData();
 
  for(size_t i=0;i<fElement.size();i++)
  {  
   
    //std::cout<<"*******************************\n";
    //std::cout<<"Reading bcm : "<<fElement[i]->GetElementName()<<" and its weight = "<<fWeights[i]<<"\n";
    
    tmpADC.Copy(&(fElement[i]->fTriumf_ADC));
    tmpADC=fElement[i]->fTriumf_ADC;
    //std::cout<<"get  hw_sum = "<<tmpADC.GetHardwareSum()<<" vs     actual "<<(fElement[i]-> fTriumf_ADC).GetHardwareSum()<<std::endl;

    tmpADC.Scale(fWeights[i]);
    //std::cout<<"scaled  hw_sum = "<<tmpADC.GetHardwareSum()<<std::endl;
    fCombined_bcm+=tmpADC;
    //std::cout<<"fcombined bcm  hw_sum = "<<fCombined_bcm.GetHardwareSum()<<std::endl;
    total_weights +=fWeights[i];
  }
  
  //std::cout<<"total weights = "<<total_weights<<"\n";
  fCombined_bcm.Scale(1.0/total_weights);

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


void QwCombinedBCM::SetDefaultSampleSize(Int_t sample_size){
  fCombined_bcm.SetDefaultSampleSize((size_t)sample_size);
}
  

/********************************************************/

Int_t QwCombinedBCM::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure

  return 1;
}

/********************************************************/

void QwCombinedBCM::Calculate_Running_Average(){
  fCombined_bcm.Calculate_Running_Average();
};

/********************************************************/

void QwCombinedBCM::Do_RunningSum(){
  fCombined_bcm.Do_RunningSum();
};

/********************************************************/


Int_t QwCombinedBCM::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement)
{
 //  fCombined_bcm.ProcessEvBuffer(buffer,word_position_in_buffer);

//   return word_position_in_buffer;
  return 0;
};
/********************************************************/
QwCombinedBCM& QwCombinedBCM::operator= (const QwCombinedBCM &value)
{
  if (GetElementName()!="") 
    this->fCombined_bcm=value.fCombined_bcm;

  return *this;
};

QwCombinedBCM& QwCombinedBCM::operator+= (const QwCombinedBCM &value)
{
  if (GetElementName()!="")
    this->fCombined_bcm+=value.fCombined_bcm;
  return *this;
};

QwCombinedBCM& QwCombinedBCM::operator-= (const QwCombinedBCM &value)
{
  if (GetElementName()!="")
    this->fCombined_bcm-=value.fCombined_bcm;

  return *this;
};


void QwCombinedBCM::Sum(QwCombinedBCM &value1, QwCombinedBCM &value2){
  *this =  value1;
  *this += value2;
};

void QwCombinedBCM::Difference(QwCombinedBCM &value1, QwCombinedBCM &value2){
  *this =  value1;
  *this -= value2;
};

void QwCombinedBCM::Ratio(QwCombinedBCM &numer, QwCombinedBCM &denom)
{  
  if (GetElementName()!="")
    this->fCombined_bcm.Ratio(numer.fCombined_bcm,denom.fCombined_bcm);

  return;
};

void QwCombinedBCM::Scale(Double_t factor)
{
  fCombined_bcm.Scale(factor);
  return;
}


void QwCombinedBCM::Print() const
{
  //std::cout<<"QwVQWK_Channel Info " <<std::endl;
  //std::cout<<" Running AVG "<<GetElementName()<<" current running AVG "<<BCM_Running_AVG<<std::endl;
  std::cout<<"QwVQWK_Channel Info " <<std::endl;
  fCombined_bcm.Print();
  return;
}

/********************************************************/
void  QwCombinedBCM::ConstructHistograms(TDirectory *folder, TString &prefix)
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

void  QwCombinedBCM::FillHistograms()
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

void  QwCombinedBCM::DeleteHistograms()
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fCombined_bcm.DeleteHistograms();
    }
  return;
};

void  QwCombinedBCM::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      fCombined_bcm.ConstructBranchAndVector(tree,prefix,values);
    }
  return;
};

void  QwCombinedBCM::FillTreeVector(std::vector<Double_t> &values)
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
void  QwCombinedBCM::Copy(VQwDataElement *source)
{
  try
    {
      if(typeid(*source)==typeid(*this))
	{
	  QwCombinedBCM* input=((QwCombinedBCM*)source);
	  this->fElementName=input->fElementName;
	  //std::cout<<this->fElementName<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n";
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


