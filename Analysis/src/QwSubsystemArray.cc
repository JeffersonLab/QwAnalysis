/**********************************************************\
* File: QwSubsystemArray.cc                               *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwSubsystemArray.h"
#include "QwHistogramHelper.h"
#include <stdexcept>



//*****************************************************************

void QwSubsystemArray::push_back(VQwSubsystem* subsys){
    if (subsys==NULL){
      std::cerr << "QwSubsystemArray::push_back():  NULL subsys"
		<< std::endl;
      //  This is an empty subsystem...
      //  Do nothing for now.
    } else if (!this->empty() && GetSubsystem(subsys->GetSubsystemName())){
      //  There is already a subsystem with this name!
    } else {
      boost::shared_ptr<VQwSubsystem> tmpptr(subsys);
      SubsysPtrs::push_back(tmpptr);
    }
  };


VQwSubsystem* QwSubsystemArray::GetSubsystem(const TString name)
{
  VQwSubsystem* tmp = NULL;
  if (!empty()){
    for (const_iterator subsys = begin(); subsys != end(); ++subsys){
      if ((*subsys)->GetSubsystemName() == name){
	tmp = (*subsys).get();
      } else {
      }
    }
  }
  return tmp;
};


void  QwSubsystemArray::ClearEventData()
{
  if (!empty())
    std::for_each(begin(), end(), 
		  boost::mem_fn(&VQwSubsystem::ClearEventData));
};
   
Int_t QwSubsystemArray::ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
				 buffer, UInt_t num_words)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ProcessConfigurationBuffer(roc_id, bank_id, buffer, num_words);
    }
};
  
Int_t QwSubsystemArray::ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t*
		      buffer, UInt_t num_words)
{
  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      (*subsys)->ProcessEvBuffer(roc_id, bank_id, buffer, num_words);
    }
};


void  QwSubsystemArray::ProcessEvent()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::ProcessEvent));
};

void  QwSubsystemArray::Copy(QwSubsystemArray *source)
{
  if (!source->empty()){
    for (iterator subsys = source->begin(); subsys != source->end(); ++subsys )     
      {
	this->push_back((*subsys)->Copy());
      }
  }

};

//*****************************************************************

void  QwSubsystemArray::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (!empty())
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      //      std::cout<<" here in QwSubsystemArray::ConstructHistograms \n";
      (*subsys)->ConstructHistograms(folder,prefix);
    }
};

void  QwSubsystemArray::FillHistograms()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::FillHistograms));
};

void  QwSubsystemArray::DeleteHistograms()
{
  if (!empty())
    std::for_each(begin(), end(), boost::mem_fn(&VQwSubsystem::DeleteHistograms));
};

//*****************************************************************

QwSubsystemArray& QwSubsystemArray::operator= (const QwSubsystemArray &source)
{

  if (!source.empty()){
    if (this->size() == source.size()){
      for(size_t i=0;i<source.size();i++){
	if (source.at(i)==NULL || this->at(i)==NULL){
	  //  Either the source or the destination subsystem
	  //  are null

	} else {
	  if (typeid(this->at(i))==typeid(source.at(i))){
	    //  std::cout<<" here in QwSubsystemArray::operator= \n";
	    *(this->at(i)) = source.at(i).get();	
	  } else {
	    //  Subsystems don't match
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


QwSubsystemArray& QwSubsystemArray::operator+= (const QwSubsystemArray &value)
{  
  if (!value.empty()){
    if (this->size() == value.size()){
      for(size_t i=0;i<value.size();i++){
	if (value.at(i)==NULL || this->at(i)==NULL){
	  //  Either the value or the destination subsystem
	  //  are null
	} else {
	  if (typeid(this->at(i))==typeid(value.at(i))){
	    *(this->at(i)) += value.at(i).get();
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

QwSubsystemArray& QwSubsystemArray::operator-= (const QwSubsystemArray &value)
{  
  if (!value.empty()){
    if (this->size() == value.size()){
      for(size_t i=0;i<value.size();i++){
	if (value.at(i)==NULL || this->at(i)==NULL){
	  //  Either the value or the destination subsystem
	  //  are null
	} else {
	  if (typeid(this->at(i))==typeid(value.at(i))){
	    *(this->at(i)) -= value.at(i).get();
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

void QwSubsystemArray::Sum(QwSubsystemArray &value1, QwSubsystemArray &value2 )
{

  if (!value1.empty()&& !value2.empty()){
    *(this)  = value1;
    *(this) += value2;
  } else {
    //  The source is empty
  }
};

void QwSubsystemArray::Difference(QwSubsystemArray &value1, QwSubsystemArray &value2 )
{

  if (!value1.empty()&& !value2.empty()){
    *(this)  =  value1;
    *(this) -= value2;
  } else {
    //  The source is empty
  }
};

void QwSubsystemArray::Ratio(QwSubsystemArray &numer, QwSubsystemArray &denom )
{
  *this=numer;
  if ( !denom.empty()){
    if (this->size() == denom.size() ){
      for(size_t i=0;i<denom.size();i++){
        if (denom.at(i)==NULL || this->at(i)==NULL){
          //  Either the value or the destination subsystem
          //  are null
        } else {
          if (typeid(this->at(i))==typeid(denom.at(i)))
            {
              this->at(i)->Ratio(numer.at(i).get(),denom.at(i).get());
            } else {
              //  Subsystems don't match
            }
        }
      }
    } else {
      //  Array sizes don't match
    }
  } else {
    //  The source is empty
  }
};
