/**********************************************************\
* File: VQwBPM.cc                                         *
*                                                         *
* Author:B.Waidyawansa                                    *
* Time-stamp:03-06-2010                                   *
\**********************************************************/

#include "VQwBPM.h"
#include "QwHistogramHelper.h"
#include <stdexcept>


/* With X being vertical up and Z being the beam direction toward the beamdump */
const TString  VQwBPM::axis[3]={"X","Y","Z"};

void  VQwBPM::InitializeChannel(TString name, Bool_t ROTATED)
{

  bEVENTCUTMODE    = false;
  fDeviceErrorCode = 0;
  bRotated=ROTATED;
  bFullSave=kTRUE;
    
  for(Short_t i=0;i<3;i++) fPositionCenter[0] = 0.0;

  SetElementName(name);

  return;
};



void VQwBPM::GetOffset(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset)
{
  // Read in the position offsets from the geometry map file
  for(Short_t i=0;i<3;i++) fPositionCenter[i]=0.0;
  fPositionCenter[0]=Xoffset;
  fPositionCenter[1]=Yoffset;
  fPositionCenter[2]=Zoffset;
  return;
};



VQwBPM& VQwBPM::operator= (const VQwBPM &value)
{
  if (GetElementName()!=""){
    for(Short_t i=0;i<3;i++) this->fPositionCenter[i]=value.fPositionCenter[i];
  }

  return *this;
};

void VQwBPM::Sum(VQwBPM &value1, VQwBPM &value2)
{
  *this =  value1;
  *this += value2;
};

void VQwBPM::Difference(VQwBPM &value1, VQwBPM &value2)
{
  *this =  value1;
  *this -= value2;
};


void VQwBPM::SetRootSaveStatus(TString &prefix)
{
  if(prefix=="diff_"||prefix=="yield_"|| prefix=="asym_")
    bFullSave=kFALSE;
  
  return;
}


