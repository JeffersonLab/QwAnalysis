/**********************************************************\
* File: QwHelicity.C                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwHelicity.h"
#include "QwHistogramHelper.h"

//*****************************************************************

void QwHelicity::SetHelicityReported(Int_t reported)
{
  fHelicityReported=reported;
  return;
};

void QwHelicity::SetHelicityActual(Int_t actual)
{
  fHelicityActual=actual;
  return;
};


void QwHelicity::SetQuartetNumber(Long_t quartet)
{
  fQuartetNumber=quartet;
  return;
}; 

void QwHelicity::SetEventnumber(Long_t event)
{
  fEventNumber=event;
  return;
};

void QwHelicity::SetAll(Int_t reported,Int_t actual,Long_t quartet,Long_t event)
{
  SetHelicityReported(reported);
  SetHelicityActual(actual);
  SetQuartetNumber(quartet);
  SetEventnumber(event);
  return;
}

//**************************************************//
Int_t QwHelicity::GetHelicityReported()
{
  return fHelicityReported;
};

Int_t QwHelicity::GetHelicityActual()
{
  return fHelicityActual;
};
 
Long_t QwHelicity::GetQuartetNumber()
{
  return  fQuartetNumber;
};

Long_t QwHelicity::GetEventnumber()
{
  return fEventNumber;
};
//**************************************************//
Bool_t QwHelicity::MatchQuartetNumber(Long_t quartet)
{
  if(quartet== fQuartetNumber)
    return kTRUE;
  else
    return kFALSE;
};

Bool_t QwHelicity::MatchEventNumber(Long_t event)
{
  if(event== fEventNumber)
    return kTRUE;
  else
    return kFALSE;
}; 

//**************************************************//
QwHelicity& QwHelicity::operator=  (const QwHelicity &value)
{
  this->fHelicityReported=fHelicityReported;
  this->fHelicityActual=fHelicityActual;
  this->fQuartetNumber=fQuartetNumber;
  this->fEventNumber=fEventNumber;
 
  return *this;
}
