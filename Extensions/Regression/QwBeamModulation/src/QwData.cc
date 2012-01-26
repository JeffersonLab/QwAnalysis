#define QwData_cxx
#include "QwData.hh"
#include "headers.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#ifdef QwData_cxx

QwData::QwData(): fSlope(-1),fError(-1),slope(-1),error(-1)
{
}

QwData::~QwData()
{
}

Double_t QwData::GetSlope(){return (fSlope);}
Double_t QwData::GetError(){return (fError);}

void  QwData::CalcAverage(){

  fSlope = weight_slope/inv_variance;
  fError = TMath::Sqrt(1/inv_variance);

}

Double_t  QwData::GetRMS(){
  return( error*TMath::Sqrt(entries) ); 
}

QwData QwData::operator+(QwData &parameter){
  //
  //This is to be used with weighted sums or averages.
  //

  QwData temp = *this;

  temp.weight_slope =  parameter.slope/TMath::Power(parameter.error,2) + weight_slope/TMath::Power(error,2);
  temp.inv_variance = 1/TMath::Power(parameter.error,2) + 1/TMath::Power(error,2);

  CalcAverage();

  return(temp);
}

void QwData::operator+=(QwData &parameter){

  this->weight_slope=parameter.weight_slope/TMath::Power(parameter.inv_variance,2)+weight_slope/TMath::Power(inv_variance,2);
  this->inv_variance = 1/TMath::Power(parameter.inv_variance,2) + 1/TMath::Power(inv_variance,2);

  CalcAverage();

  return;
}
#endif
