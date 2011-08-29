/*
 * QwRegressionSubsystem.cc
 *
 *  Created on: Aug 11, 2011
 *      Author: meeker
 */

#include "QwRegressionSubsystem.h"
#include "VQwSubsystemParity.h"
#include "QwRegression.h"
#include "QwSubsystemArrayParity.h"

//RegisterSubsystemFactory(QwRegressionSubsystem);


QwRegressionSubsystem::~QwRegressionSubsystem()
{
}

void QwRegressionSubsystem::Copy (const QwRegressionSubsystem &source)
{
    VQwSubsystem::Copy(const_cast<QwRegressionSubsystem*>(&source));
    QwRegression::Copy(&source);

    return;
}

 QwRegressionSubsystem* QwRegressionSubsystem:: operator= (const QwRegressionSubsystem *value)
 {
     for (size_t i = 0; i < value->fDependentVar.size(); i++)
     {
         this->fDependentVar.at(i).second = value->fDependentVar.at(i).second;
     }

     return this;
}


QwRegressionSubsystem& QwRegressionSubsystem::operator+= (QwRegressionSubsystem *value)
{  
     for(size_t i = 0; i < value->fDependentVar.size(); i++)
     {
         *(this->fDependentVar.at(i).second) += *(value->fDependentVar.at(i).second);
     }

     return *this;
}


QwRegressionSubsystem& ::QwRegressionSubsystem::operator-= (const QwRegressionSubsystem *value)
{   
    for(size_t i = 0; i < value->fDependentVar.size();i++)
    {
        *(this->fDependentVar.at(i).second) -= *(value->fDependentVar.at(i).second);
    }

    return *this;
}


QwRegressionSubsystem& QwRegressionSubsystem:: operator*= (const QwRegressionSubsystem *value)
{
  
    for(size_t i = 0; i < value-> fDependentVar.size(); i++)
    {
        *(this->fDependentVar.at(i).second) *= *(value->fDependentVar.at(i).second);
    }
  
    return *this; 
}



QwRegressionSubsystem& QwRegressionSubsystem:: operator/= (const QwRegressionSubsystem *value)
{
    for(size_t i = 0; i < value->fDependentVar.size(); i++)
    {
        *(this->fDependentVar.at(i).second) /= *(value->fDependentVar.at(i).second);
    }
    
    return *this;
}


void QwRegressionSubsystem:: Sum(QwRegressionSubsystem *value1, QwRegressionSubsystem *value2){

    *this = value1;
    *this += value2;

}

void QwRegressionSubsystem::Difference(QwRegressionSubsystem *value1,QwRegressionSubsystem *value2)
{
    *this = value1;
    *this -= value2;
  
}


void QwRegressionSubsystem:: AccumulateRunningSum(QwRegressionSubsystem* value)
{
    for (size_t i = 0; i < value-> fDependentVar.size(); i++)
    {
        fDependentVar.at(i).second->AccumulateRunningSum(value->fDependentVar.at(i).second);
    }
}

void QwRegressionSubsystem::CalculateRunningAverage()
{
    for(size_t i = 0; i < fDependentVar.size(); i++)
    {
        fDependentVar.at(i).second->CalculateRunningAverage();
    }
}

void QwRegressionSubsystem:: PrintValue() const{

    for (size_t i = 0; i < fDependentVar.size(); i++)
    {
        fDependentVar.at(i).second->PrintValue();
    }

}






/// DERIVED FUNCTIONS /// 


/*  All of the functions below are using generic
 * returns for testing purposes. 
 */



Int_t QwRegressionSubsystem::LoadChannelMap(TString)
{
  Int_t sample = 0;
  return sample;
}


Int_t QwRegressionSubsystem::LoadInputParameters(TString)
{
  Int_t sample = 0;
  return sample;
}


Int_t QwRegressionSubsystem::LoadEventCuts(TString)
{
  Int_t sample = 0;
  return sample;
  
}

Int_t QwRegressionSubsystem::ProcessConfigurationBuffer(UInt_t, UInt_t, UInt_t*, UInt_t)
{
  Int_t sample = 0;
  return sample;
}


Int_t QwRegressionSubsystem::ProcessEvBuffer(UInt_t, UInt_t, UInt_t*, UInt_t)
{
  Int_t sample = 0;
  return sample;
}


VQwSubsystem& QwRegressionSubsystem::operator=(VQwSubsystem* value)
{
    
    QwRegressionSubsystem* input= dynamic_cast<QwRegressionSubsystem*>(value);
    
    for(size_t i = 0; i < input->fDependentVar.size(); i++)
    {
      *(this->fDependentVar.at(i).second) = *(input->fDependentVar.at(i).second);
    }
    return *this;
}



VQwSubsystem& QwRegressionSubsystem::operator+=(VQwSubsystem* value)
{
  QwRegressionSubsystem* input = dynamic_cast<QwRegressionSubsystem*>(value);
  
  for(size_t i = 0; i < input->fDependentVar.size(); i++)
  {
    *(this->fDependentVar.at(i).second) += *(input->fDependentVar.at(i).second);
  }
  
  return *this;
  
}

VQwSubsystem& QwRegressionSubsystem:: operator-=(VQwSubsystem* value)
{
  QwRegressionSubsystem* input = dynamic_cast<QwRegressionSubsystem*>(value);
  
  for(size_t i = 0; i<input->fDependentVar.size(); i++)
  {
    *(this->fDependentVar.at(i).second) += *(input->fDependentVar.at(i).second);
  } 
  
  return *this;
}

void QwRegressionSubsystem::Sum(VQwSubsystem* value1, VQwSubsystem* value2)
{
  QwRegressionSubsystem* input1 = dynamic_cast<QwRegressionSubsystem*>(value1);
  
  QwRegressionSubsystem* input2 = dynamic_cast<QwRegressionSubsystem*>(value2);
  
  *this = input1;
  *this += input2;
  
}


void QwRegressionSubsystem::Difference(VQwSubsystem* value1, VQwSubsystem* value2)
{
  QwRegressionSubsystem* input1 = dynamic_cast<QwRegressionSubsystem*>(value1);
  
  QwRegressionSubsystem* input2 = dynamic_cast<QwRegressionSubsystem*>(value2);
  
  *this = input1;
  *this -= input2;
  
}


void QwRegressionSubsystem::Ratio(VQwSubsystem* value1, VQwSubsystem* value2)
{
  QwRegressionSubsystem* input1 = dynamic_cast<QwRegressionSubsystem*>(value1);
  
  QwRegressionSubsystem* input2 = dynamic_cast<QwRegressionSubsystem*>(value2);
  
  *this = input1;
  *this /= input2;
}


void QwRegressionSubsystem::Scale(Double_t value)
{ 
  for(size_t i = 0; i < this->fDependentVar.size(); i++)
  {
    this->fDependentVar.at(i).second->Scale(value);
  }
  
};

VQwSubsystem* QwRegressionSubsystem::Copy()
{
  return (new QwRegressionSubsystem(*this));
    
}
  

void AccumulateRunningSum(VQwSubsystem* input)
{
  QwRegressionSubsystem* value = dynamic_cast<QwRegressionSubsystem*> (input);
  
  AccumulateRunningSum(value);
}


Bool_t QwRegressionSubsystem::ApplySingleEventCuts()
{
  return false;
}

Int_t QwRegressionSubsystem::GetEventcutErrorCounters()
{
  return 4;
  
}

UInt_t QwRegressionSubsystem::GetEventcutErrorFlag()
{
    return 4;
  
}



void QwRegressionSubsystem::Copy(VQwSubsystem* source)
{
    try
    {
      QwRegressionSubsystem* input = dynamic_cast<QwRegressionSubsystem*>(source);
      if (input != NULL){
          VQwSubsystem::Copy(input);
          QwRegression::Copy(input);
      } else {
        TString loc="Standard exception from QwRegressionSubsystem::Copy = "
          +source->GetSubsystemName()+" "
          +this->GetSubsystemName()+" are not of the same type";
          throw std::invalid_argument(loc.Data());
        }
    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  // this->Print();

  return;
}


