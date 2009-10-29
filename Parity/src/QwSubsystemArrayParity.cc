/**********************************************************\
* File: QwSubsystemArrayParity.cc                         *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwSubsystemArrayParity.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

#include "VQwSubsystemParity.h"


//*****************************************************************

VQwSubsystemParity* QwSubsystemArrayParity::GetSubsystem(const TString name)
{
  VQwSubsystemParity* tmp = NULL;
  tmp = dynamic_cast<VQwSubsystemParity*>(QwSubsystemArray::GetSubsystem(name));
  
  return tmp;
}; 

void  QwSubsystemArrayParity::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemParity* subsys_parity = dynamic_cast<VQwSubsystemParity*>(subsys->get());
    subsys_parity->ConstructBranchAndVector(tree, prefix, values);
  }
};

void  QwSubsystemArrayParity::FillTreeVector(std::vector<Double_t> &values)
{
  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemParity* subsys_parity = dynamic_cast<VQwSubsystemParity*>(subsys->get());
    subsys_parity->FillTreeVector(values);
  }
};


//*****************************************************************

void  QwSubsystemArrayParity::Copy(QwSubsystemArrayParity *source)
{
  if (!source->empty()){
    for (iterator subsys = source->begin(); subsys != source->end(); ++subsys )
      {
	VQwSubsystemParity *srcptr =
	    dynamic_cast<VQwSubsystemParity*>(subsys->get());
	if (srcptr != NULL){
	  this->push_back(srcptr->Copy());
	}
      }
  }

};


QwSubsystemArrayParity& QwSubsystemArrayParity::operator= (const QwSubsystemArrayParity &source)
{
  Bool_t localdebug=kFALSE;
  if(localdebug)  std::cout<<"QwSubsystemArrayParity::operator= \n";
  if (!source.empty()){
    if (this->size() == source.size()){
      for(size_t i=0;i<source.size();i++){
	if (source.at(i)==NULL || this->at(i)==NULL){
	  //  Either the source or the destination subsystem
	  //  are null
	} else {
	  VQwSubsystemParity *ptr1 =
	    dynamic_cast<VQwSubsystemParity*>(this->at(i).get());
	  if (typeid(*ptr1)==typeid(*(source.at(i).get()))){
	    if(localdebug) std::cout<<" here in QwSubsystemArrayParity::operator= types mach \n";
	    *(ptr1) = source.at(i).get();
	  } else {
	    //  Subsystems don't match
	      std::cerr<<" QwSubsystemArrayParity::operator= types do not mach \n";
	      std::cerr<<" typeid(ptr1)="<< typeid(*ptr1).name() <<" but typeid(*(source.at(i).get()))="<<typeid(*(source.at(i).get())).name()<<"\n";
	  }
	}
      }
    } else {
      //  Array sizes don't match
    }
  } else {
    //  The source is empty
  }
  return *this;
};


QwSubsystemArrayParity& QwSubsystemArrayParity::operator+= (const QwSubsystemArrayParity &value)
{
  if (!value.empty()){
    if (this->size() == value.size()){
      for(size_t i=0;i<value.size();i++){
	if (value.at(i)==NULL || this->at(i)==NULL){
	  //  Either the value or the destination subsystem
	  //  are null
	} else {
	  VQwSubsystemParity *ptr1 =
	    dynamic_cast<VQwSubsystemParity*>(this->at(i).get());
	  if (typeid(*ptr1)==typeid(*(value.at(i).get()))){
	    *(ptr1) += value.at(i).get();
	    //std::cout<<"QwSubsystemArrayParity::operator+ here where types match \n";
	  } else {
	    std::cerr<<"QwSubsystemArrayParity::operator+ here where types don't match \n";
	    std::cerr<<" typeid(ptr1)="<< typeid(ptr1).name() <<" but typeid(value.at(i)))="<<typeid(value.at(i)).name()<<"\n";
	    //  Subsystems don't match
	  }
	}
      }
    } else {
      //  Array sizes don't match
    }
  } else {
    //  The vsource is empty
  }
  return *this;
};

QwSubsystemArrayParity& QwSubsystemArrayParity::operator-= (const QwSubsystemArrayParity &value)
{
  if (!value.empty()){
    if (this->size() == value.size()){
      for(size_t i=0;i<value.size();i++){
	if (value.at(i)==NULL || this->at(i)==NULL){
	  //  Either the value or the destination subsystem
	  //  are null
	} else {
	  VQwSubsystemParity *ptr1 =
	    dynamic_cast<VQwSubsystemParity*>(this->at(i).get());
	  if (typeid(*ptr1)==typeid(*(value.at(i).get()))){
	    *(ptr1) -= value.at(i).get();
	  } else {
	    //  Subsystems don't match
	  }
	}
      }
    } else {
      //  Array sizes don't match
    }
  } else {
    //  The vsource is empty
  }
  return *this;
};

void QwSubsystemArrayParity::Sum(QwSubsystemArrayParity &value1, QwSubsystemArrayParity &value2 )
{

  if (!value1.empty()&& !value2.empty()){
    *(this)  = value1;
    *(this) += value2;
  } else {
    //  The source is empty
  }
};

void QwSubsystemArrayParity::Difference(QwSubsystemArrayParity &value1, QwSubsystemArrayParity &value2 )
{

  if (!value1.empty()&& !value2.empty()){
    *(this)  =  value1;
    *(this) -= value2;
  } else {
    //  The source is empty
  }
};



void QwSubsystemArrayParity::Ratio(QwSubsystemArrayParity &numer, QwSubsystemArrayParity &denom )
{
  Bool_t localdebug=kFALSE;

  if(localdebug) std::cout<<"QwSubsystemArrayParity::Ratio \n";
  *this=numer;
  if ( !denom.empty()){
    if (this->size() == denom.size() ){
      for(size_t i=0;i<denom.size();i++){
        if (denom.at(i)==NULL || this->at(i)==NULL){
          //  Either the value or the destination subsystem  are null
	  if(localdebug) std::cout<<"Either the value or the destination subsystem  are null\n";
        } else {
	  VQwSubsystemParity *ptr1 =
	    dynamic_cast<VQwSubsystemParity*>(this->at(i).get());
	  if (typeid(*ptr1)==typeid(*(denom.at(i).get())))
            {
              ptr1->Ratio(numer.at(i).get(),denom.at(i).get());
            } else {
              //  Subsystems don't match
	    std::cerr<<"QwSubsystemArrayParity::Ratio subsystem #"<<i
		     <<" type do not match : ratio computation aborted \n";
            }
        }
      }
    } else {
      std::cerr<<"QwSubsystemArrayParity::Ratio == array size do not match : ratio computation aborted \n";
      //  Array sizes don't match
    }
  } else {
    std::cerr<<"QwSubsystemArrayParity::Ratio == source empty : ratio computation aborted \n";
    //  The source is empty
  }
  if(localdebug) std::cout<<"I am out of it \n";

};

Bool_t QwSubsystemArrayParity::ApplySingleEventCuts(){
  Int_t CountFalse;
  Bool_t status;
  VQwSubsystemParity *subsys_parity;
  CountFalse=0;
  //std::cout<<" here in QwSubsystemArrayParity::SingleEventCut()"<<std::endl;
  if (!empty()){
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      subsys_parity=dynamic_cast<VQwSubsystemParity*>((subsys)->get());
      if (!subsys_parity->ApplySingleEventCuts()) 
      {
	CountFalse++;
	//update the sFailedSubsystem vector
	std::cout<<" ** Failed ** "<<" Subsystem name "<<((subsys)->get())->GetSubsystemName()<<std::endl;
	sFailedSubsystems.push_back(((subsys)->get())->GetSubsystemName());
      }
    }
  }
  if (CountFalse > 0)
    status = false;
  else
    status = true;

 return status;
}


Bool_t QwSubsystemArrayParity::CheckRunningAverages(Bool_t bDiplayAVG){ //check the running averages of sub systems and passing argument decide print AVG or not
  Int_t CountFalse;
  Bool_t status;
  VQwSubsystemParity *subsys_parity;
  CountFalse=0;
  //std::cout<<" here in QwSubsystemArrayParity::SingleEventCut()"<<std::endl;
  if (!empty()){
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      subsys_parity=dynamic_cast<VQwSubsystemParity*>((subsys)->get());
      if (!subsys_parity->CheckRunningAverages(bDiplayAVG)) 
      {
	CountFalse++;
	//update the sFailedSubsystem vector
	std::cout<<" ** Failed Running AVG ** "<<" Subsystem name "<<((subsys)->get())->GetSubsystemName()<<std::endl;
	//sFailedSubsystems.push_back(((subsys)->get())->GetSubsystemName());
      }
    }
  }
  if (CountFalse > 0)
    status = false;
  else
    status = true;

 return status;
}
 

Int_t QwSubsystemArrayParity::GetEventcutErrorCounters(){

  
  VQwSubsystemParity *subsys_parity;
  
  
  if (!empty()){
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      subsys_parity=dynamic_cast<VQwSubsystemParity*>((subsys)->get());
      subsys_parity->GetEventcutErrorCounters();
      
	//std::cout<<" ** Failed ** "<<" Subsystem name "<<((subsys)->get())->GetSubsystemName()<<std::endl;
	//sFailedSubsystems.push_back(((subsys)->get())->GetSubsystemName());
     
    }
  }
  

  return 1;
}
