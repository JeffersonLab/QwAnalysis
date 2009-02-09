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

  if (!source.empty()){
    if (this->size() == source.size()){
      for(size_t i=0;i<source.size();i++){
	if (source.at(i)==NULL || this->at(i)==NULL){
	  //  Either the source or the destination subsystem
	  //  are null

	} else {
	  VQwSubsystemParity *ptr1 = 
	    dynamic_cast<VQwSubsystemParity*>(this->at(i).get());
	  if (typeid(ptr1)==typeid(source.at(i))){
	    //  std::cout<<" here in QwSubsystemArrayParity::operator= \n";
	    *(ptr1) = source.at(i).get();	
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
	  if (typeid(ptr1)==typeid(value.at(i))){
	    *(ptr1) += value.at(i).get();
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
	  if (typeid(ptr1)==typeid(value.at(i))){
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
  *this=numer;
  if ( !denom.empty()){
    if (this->size() == denom.size() ){
      for(size_t i=0;i<denom.size();i++){
        if (denom.at(i)==NULL || this->at(i)==NULL){
          //  Either the value or the destination subsystem
          //  are null
        } else {
	  VQwSubsystemParity *ptr1 = 
	    dynamic_cast<VQwSubsystemParity*>(this->at(i).get());
          if (typeid(ptr1)==typeid(denom.at(i)))
            {
              ptr1->Ratio(numer.at(i).get(),denom.at(i).get());
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
