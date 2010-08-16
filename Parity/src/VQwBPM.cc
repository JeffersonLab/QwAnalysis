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

void  VQwBPM::InitializeChannel(TString name)
{

  bEVENTCUTMODE    = false;
  fDeviceErrorCode = 0;

  fAbsPos[0].InitializeChannel(name+axis[0],"derived");
  fAbsPos[1].InitializeChannel(name+axis[1],"derived");

  fEffectiveCharge.InitializeChannel(name+"_EffectiveCharge","derived");
  
  fPositionCenter[0] = 0.0;
  fPositionCenter[1] = 0.0;
  fPositionCenter[2] = 0.0;


  SetElementName(name);

  return;
};

void VQwBPM::ClearEventData()
{
  fAbsPos[0].ClearEventData();
  fAbsPos[1].ClearEventData();
  fEffectiveCharge.ClearEventData();

  return;
};

void VQwBPM::GetOffset(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset)
{
  // Read in the position offsets from the geometry map file
  fPositionCenter[0]=0.0;
  fPositionCenter[1]=0.0;
  fPositionCenter[2]=0.0;

  fPositionCenter[0]=Xoffset;
  fPositionCenter[1]=Yoffset;
  fPositionCenter[2]=Zoffset;
  return;
};



Int_t VQwBPM::GetEventcutErrorCounters()
{
  fAbsPos[0].GetEventcutErrorCounters();
  fAbsPos[1].GetEventcutErrorCounters();
  fEffectiveCharge.GetEventcutErrorCounters();

  return 1;
};


Bool_t VQwBPM::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t i=0;

 
  //Event cuts for Absolute X & Y
  if (fAbsPos[0].ApplySingleEventCuts()){ //for RelX
    status&=kTRUE;
  }
  else{
    fAbsPos[0].UpdateEventCutErrorCount();
    status&=kFALSE;
    if (bDEBUG) std::cout<<" Abs X event cut failed ";
  }
  //update the event cut counters
  fAbsPos[0].UpdateHWErrorCounters();
  //Get the Event cut error flag for AbsX/Y
  fDeviceErrorCode|=fAbsPos[0].GetEventcutErrorFlag();
  
  if (fAbsPos[1].ApplySingleEventCuts()){ //for RelX
    status&=kTRUE;
  }
  else{
    fAbsPos[1].UpdateEventCutErrorCount();
    status&=kFALSE;
    if (bDEBUG) std::cout<<" Abs X event cut failed ";
  }
  //update the event cut counters
  fAbsPos[1].UpdateHWErrorCounters();
  //Get the Event cut error flag for AbsX/Y
  fDeviceErrorCode|=fAbsPos[1].GetEventcutErrorFlag();
  
  
 //Event cuts for four wire sum (EffectiveCharge)
  if (fEffectiveCharge.ApplySingleEventCuts()){ 
      status&=kTRUE;
  }
  else{
    fEffectiveCharge.UpdateEventCutErrorCount();
    status&=kFALSE;
    if (bDEBUG) std::cout<<"EffectiveCharge event cut failed ";
  }
  //update the event cut counters
  fEffectiveCharge.UpdateHWErrorCounters();
  //Get the Event cut error flag for EffectiveCharge
  fDeviceErrorCode|=fEffectiveCharge.GetEventcutErrorFlag();


  return status;
  
};

void VQwBPM::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{
  //cuts for the absolute x and y
  if (ch_name=="absx"){
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[1].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="effectivecharge"){
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fEffectiveCharge.SetSingleEventCuts(minX,maxX);

  }
};


VQwBPM& VQwBPM::operator= (const VQwBPM &value)
{
  if (GetElementName()!=""){
    this->fEffectiveCharge=value.fEffectiveCharge;
    this->fAbsPos[0]=value.fAbsPos[0];
    this->fAbsPos[1]=value.fAbsPos[1];
    this->fPositionCenter[0]=value.fPositionCenter[0];
    this->fPositionCenter[1]=value.fPositionCenter[1];
    this->fPositionCenter[1]=value.fPositionCenter[1];
  }

  return *this;
};

VQwBPM& VQwBPM::operator+= (const VQwBPM &value)
{
  if (GetElementName()!=""){
    this->fEffectiveCharge+=value.fEffectiveCharge;
    this->fAbsPos[0]+=value.fAbsPos[0];
    this->fAbsPos[1]+=value.fAbsPos[1];
    this->fAbsPos[2]+=value.fAbsPos[2];
  }
  return *this;
};

VQwBPM& VQwBPM::operator-= (const VQwBPM &value)
{
  if (GetElementName()!=""){
    this->fEffectiveCharge-=value.fEffectiveCharge;
    this->fAbsPos[0]-=value.fAbsPos[0];
    this->fAbsPos[1]-=value.fAbsPos[1];
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


void VQwBPM::Scale(Double_t factor)
{
  fEffectiveCharge.Scale(factor);
  fAbsPos[0].Scale(factor);
  fAbsPos[1].Scale(factor);
  return;
};



void VQwBPM::SetRootSaveStatus(TString &prefix)
{
  if(prefix=="diff_"||prefix=="yield_"|| prefix=="asym_")
    bFullSave=kFALSE;
  
  return;
}


