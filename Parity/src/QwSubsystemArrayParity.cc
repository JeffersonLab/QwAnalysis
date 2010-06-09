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

void QwSubsystemArrayParity::ProcessOptions(QwOptions &options){
  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemParity* subsys_parity = dynamic_cast<VQwSubsystemParity*>(subsys->get());
    subsys_parity->ProcessOptions(options);
  }
}; //Handle command line options


VQwSubsystemParity* QwSubsystemArrayParity::GetSubsystemByName(const TString& name)
{
  return dynamic_cast<VQwSubsystemParity*>  (QwSubsystemArray::GetSubsystemByName(name));
};

void  QwSubsystemArrayParity::ConstructBranchAndVector(TTree *tree, TString & prefix, std::vector <Double_t> &values)
{
  fTreeArrayIndex  = values.size();
  values.push_back(0.0);
  tree->Branch("CodaEventNumber",&(values[fTreeArrayIndex]),"/D" );
  values.push_back(0.0);
  tree->Branch("CodaEventType",&(values[fTreeArrayIndex+1]),"/D" );

  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemParity* subsys_parity = dynamic_cast<VQwSubsystemParity*>(subsys->get());
    subsys_parity->ConstructBranchAndVector(tree, prefix, values);
  }
};

void  QwSubsystemArrayParity::FillTreeVector(std::vector<Double_t> &values)
{
  size_t index=fTreeArrayIndex;
  values[index++] = this->GetCodaEventNumber();
  values[index++] = this->GetCodaEventType();

  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemParity* subsys_parity = dynamic_cast<VQwSubsystemParity*>(subsys->get());
    subsys_parity->FillTreeVector(values);
  }
};



void  QwSubsystemArrayParity::FillDB(QwDatabase *db, TString type)
{
  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemParity* subsys_parity = dynamic_cast<VQwSubsystemParity*>(subsys->get());
    subsys_parity->FillDB(db, type);
  }
};


//*****************************************************************

/**
 * Copy an array of subsystems into this array
 * @param source Subsystem array
 */
void  QwSubsystemArrayParity::Copy(const QwSubsystemArrayParity *source)
{
  if (!source->empty()) {
    for (const_iterator subsys = source->begin(); subsys != source->end(); ++subsys) {
      VQwSubsystemParity *srcptr =
          dynamic_cast<VQwSubsystemParity*>(subsys->get());
      if (srcptr != NULL) {
        this->push_back(srcptr->Copy());
      }
    }
  }
};


/**
 * Assignment operator
 * @param source Subsystem array to assign to this array
 * @return This subsystem array after assignment
 */
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


/**
 * Addition-assignment operator
 * @param value Subsystem array to add to this array
 * @return This subsystem array after addition
 */
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

/**
 * Subtraction-assignment operator
 * @param value Subsystem array to subtract from this array
 * @return This array after subtraction
 */
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

/**
 * Sum of two subsystem arrays
 * @param value1 First subsystem array
 * @param value2 Second subsystem array
 */
void QwSubsystemArrayParity::Sum(
  const QwSubsystemArrayParity &value1,
  const QwSubsystemArrayParity &value2)
{

  if (!value1.empty()&& !value2.empty()){
    *(this)  = value1;
    *(this) += value2;
  } else {
    //  The source is empty
  }
};

/**
 * Difference of two subsystem arrays
 * @param value1 First subsystem array
 * @param value2 Second subsystem array
 */
void QwSubsystemArrayParity::Difference(
  const QwSubsystemArrayParity &value1,
  const QwSubsystemArrayParity &value2)
{

  if (!value1.empty()&& !value2.empty()){
    *(this)  =  value1;
    *(this) -= value2;
  } else {
    //  The source is empty
  }
};

/**
 * Scale this subsystem array
 * @param factor Scale factor
 */
void QwSubsystemArrayParity::Scale(Double_t factor){
   for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemParity* subsys_parity = dynamic_cast<VQwSubsystemParity*>(subsys->get());
    subsys_parity->Scale(factor);
  }
};


void QwSubsystemArrayParity::CalculateRunningAverage() {
   for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemParity* subsys_parity = dynamic_cast<VQwSubsystemParity*>(subsys->get());
    subsys_parity->CalculateRunningAverage();
  }

}
void QwSubsystemArrayParity::AccumulateRunningSum(const QwSubsystemArrayParity& value)
{
  if (!value.empty()) {
    if (this->size() == value.size()) {
      for (size_t i = 0; i < value.size(); i++) {
        if (value.at(i)==NULL || this->at(i)==NULL) {
          //  Either the value or the destination subsystem
          //  are null
        } else {
          VQwSubsystemParity *ptr1 =
            dynamic_cast<VQwSubsystemParity*>(this->at(i).get());
          if (typeid(*ptr1) == typeid(*(value.at(i).get()))) {
            ptr1->AccumulateRunningSum(value.at(i).get());
          } else {
            std::cerr<<"QwSubsystemArrayParity::AccumulateRunningSum here where types don't match \n";
            std::cerr<<" typeid(ptr1)="<< typeid(ptr1).name() <<" but typeid(value.at(i)))="<<typeid(value.at(i)).name()<<"\n";
            //  Subsystems don't match
          }
        }
      }
    } else {
      //  Array sizes don't match
    }
  } else {
    //  The value is empty
  }
};

void QwSubsystemArrayParity::Blind(const QwBlinder *blinder)
{
  // Loop over subsystem array
  for (size_t i = 0; i < this->size(); i++) {
    // Cast into parity subsystems
    VQwSubsystemParity* subsys = dynamic_cast<VQwSubsystemParity*>(this->at(i).get());

    // Check for null pointers
    if (this->at(i) == 0) {
      QwError << "QwSubsystemArrayParity::Blind: "
              << "parity subsystem null pointer!" << QwLog::endl;
      return;
    }

    // Apply blinding
    subsys->Blind(blinder);
  }
};

void QwSubsystemArrayParity::Blind(const QwBlinder *blinder, const QwSubsystemArrayParity& yield)
{
  // Check for array size
  if (this->size() != yield.size()) {
    QwError << "QwSubsystemArrayParity::Blind: "
            << "diff and yield array dimension mismatch!" << QwLog::endl;
    return;
  }

  // Loop over subsystem array
  for (size_t i = 0; i < this->size(); i++) {
    // Cast into parity subsystems
    VQwSubsystemParity* subsys_diff  = dynamic_cast<VQwSubsystemParity*>(this->at(i).get());
    VQwSubsystemParity* subsys_yield = dynamic_cast<VQwSubsystemParity*>(yield.at(i).get());

    // Check for null pointers
    if (subsys_diff == 0 || subsys_yield == 0) {
      QwError << "QwSubsystemArrayParity::Blind: "
              << "diff or yield parity subsystem null pointer!" << QwLog::endl;
      return;
    }

    // Apply blinding
    subsys_diff->Blind(blinder, subsys_yield);
  }
};

void QwSubsystemArrayParity::Ratio(
  const QwSubsystemArrayParity &numer,
  const QwSubsystemArrayParity &denom)
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
  //fSubsystem_Error_Flag=0;//set the error flag
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
	//std::cout<<" ** Failed ** "<<" Subsystem name "<<((subsys)->get())->GetSubsystemName()<<std::endl;
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

Int_t QwSubsystemArrayParity::GetEventcutErrorFlag(){// report number of events falied due to HW and event cut faliure
  VQwSubsystemParity *subsys_parity;
  Int_t ErrorFlag;

  ErrorFlag=0;
  if (!empty()){
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      subsys_parity=dynamic_cast<VQwSubsystemParity*>((subsys)->get());
      ErrorFlag |= subsys_parity->GetEventcutErrorFlag();
    }
  }

  return ErrorFlag;
};
