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

#include "QwParameterFile.h"


//RegisterSubsystemFactory(QwRegressionSubsystem);


QwRegressionSubsystem::~QwRegressionSubsystem()
{
}

struct null_deleter { 
  void operator()(void const *) const { }
};

boost::shared_ptr<VQwSubsystem> QwRegressionSubsystem::GetSharedPointerToStaticObject(){
  boost::shared_ptr<VQwSubsystem> px(this, null_deleter());
  return px;
}

void QwRegressionSubsystem::Copy (const QwRegressionSubsystem &source)
{
    VQwSubsystem::Copy(const_cast<QwRegressionSubsystem*>(&source));
    QwRegression::Copy(&source);

    return;
}

QwRegressionSubsystem* QwRegressionSubsystem:: operator= (const QwRegressionSubsystem *value)
{
  for (size_t i = 0; i < value->fDependentVar.size(); i++) {
    this->fDependentVar.at(i).second->AssignValueFrom(value->fDependentVar.at(i).second);
  }
  return this;
}


QwRegressionSubsystem& QwRegressionSubsystem::operator+= (QwRegressionSubsystem *value)
{  
     for(size_t i = 0; i < value->fDependentVar.size(); i++)
     {
       this->fDependentVar.at(i).second->AddValueFrom(value->fDependentVar.at(i).second);
     }

     return *this;
}


QwRegressionSubsystem& ::QwRegressionSubsystem::operator-= (const QwRegressionSubsystem *value)
{   
    for(size_t i = 0; i < value->fDependentVar.size();i++)
    {
      this->fDependentVar.at(i).second->SubtractValueFrom(value->fDependentVar.at(i).second);
    }

    return *this;
}


QwRegressionSubsystem& QwRegressionSubsystem:: operator*= (const QwRegressionSubsystem *value)
{
  
    for(size_t i = 0; i < value-> fDependentVar.size(); i++)
    {
      this->fDependentVar.at(i).second->MultiplyBy(value->fDependentVar.at(i).second);
    }
  
    return *this; 
}



QwRegressionSubsystem& QwRegressionSubsystem:: operator/= (const QwRegressionSubsystem *value)
{
    for(size_t i = 0; i < value->fDependentVar.size(); i++)
    {
      this->fDependentVar.at(i).second->DivideBy(value->fDependentVar.at(i).second);
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
  if (value!=NULL){
    QwRegression::AccumulateRunningSum(*value);
  }
}

void QwRegressionSubsystem::CalculateRunningAverage()
{
  QwRegression::CalculateRunningAverage();
}

void QwRegressionSubsystem:: PrintValue() const{
  QwRegression::PrintValue();
}


void QwRegressionSubsystem::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    fDependentVar.at(i).second->ConstructHistograms(folder,prefix);
  }
};

void QwRegressionSubsystem::FillHistograms()
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    fDependentVar.at(i).second->FillHistograms();
  }
};

void QwRegressionSubsystem::DeleteHistograms()
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    fDependentVar.at(i).second->DeleteHistograms();
  }
};

void QwRegressionSubsystem::ConstructBranch(TTree *tree, TString & prefix)
{
  for (size_t i = 0; i < fDependentVar.size(); i++){
    fDependentVar.at(i).second->ConstructBranch(tree, prefix);
  }
};

void QwRegressionSubsystem::ConstructBranch(TTree *tree, TString & prefix, QwParameterFile& trim_file)
{
  TString tmp;
  QwParameterFile* nextmodule;
  trim_file.RewindToFileStart();
  tmp="Regression";
  trim_file.RewindToFileStart();
  if (trim_file.FileHasModuleHeader(tmp)){
    nextmodule=trim_file.ReadUntilNextModule();//This section contains sub modules and or channels to be included in the tree
    for (size_t i = 0; i < fDependentVar.size(); i++){
      fDependentVar.at(i).second->ConstructBranch(tree, prefix, *nextmodule);
    }
  }
};






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
  *this =  input;
  return *this;
}



VQwSubsystem& QwRegressionSubsystem::operator+=(VQwSubsystem* value)
{
  QwRegressionSubsystem* input = dynamic_cast<QwRegressionSubsystem*>(value);
  *this += input;
  return *this;
  
}

VQwSubsystem& QwRegressionSubsystem:: operator-=(VQwSubsystem* value)
{
  QwRegressionSubsystem* input = dynamic_cast<QwRegressionSubsystem*>(value);
  *this -= input;
  return *this;
}

void QwRegressionSubsystem::Sum(VQwSubsystem* value1, VQwSubsystem* value2)
{
  QwRegressionSubsystem* input1 = dynamic_cast<QwRegressionSubsystem*>(value1);
  QwRegressionSubsystem* input2 = dynamic_cast<QwRegressionSubsystem*>(value2);
  Sum(input1,input2);
}


void QwRegressionSubsystem::Difference(VQwSubsystem* value1, VQwSubsystem* value2)
{
  QwRegressionSubsystem* input1 = dynamic_cast<QwRegressionSubsystem*>(value1);
  QwRegressionSubsystem* input2 = dynamic_cast<QwRegressionSubsystem*>(value2);
  Difference(input1,input2);
}


void QwRegressionSubsystem::Ratio(VQwSubsystem* value1, VQwSubsystem* value2)
{
  QwRegressionSubsystem* input1 = dynamic_cast<QwRegressionSubsystem*>(value1);
  QwRegressionSubsystem* input2 = dynamic_cast<QwRegressionSubsystem*>(value2);
  *this =  input1;
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
  

void QwRegressionSubsystem::AccumulateRunningSum(VQwSubsystem* input)
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


