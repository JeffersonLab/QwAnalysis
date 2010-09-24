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

  Short_t i = 0; 

  for(i=0;i<2;i++)
    fAbsPos[i].InitializeChannel(name+axis[i],"derived");

  fEffectiveCharge.InitializeChannel(name+"_EffectiveCharge","derived");
  
  for(i=0;i<3;i++) fPositionCenter[0] = 0.0;

  SetElementName(name);

  return;
};

void VQwBPM::ClearEventData()
{
  for(Short_t i=0;i<2;i++)
    fAbsPos[i].ClearEventData();

  fEffectiveCharge.ClearEventData();
  
  return;
};

void VQwBPM::GetSurveyOffsets(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset)
{
  // Read in the position offsets from the geometry map file
  for(Short_t i=0;i<3;i++) fPositionCenter[i]=0.0;
  fPositionCenter[0]=Xoffset;
  fPositionCenter[1]=Yoffset;
  fPositionCenter[2]=Zoffset;
  return;
};

void VQwBPM::GetElectronicFactors(Double_t BSENfactor, Double_t AlphaX, Double_t AlphaY)
{
  // Read in the electronic factors from the file
  Bool_t ldebug = kFALSE;

  fQwStriplineCalibration = BSENfactor*18.81;
   fRelativeGains[0]=AlphaX;
   fRelativeGains[1]=AlphaY;

   if(ldebug){
     std::cout<<"\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
     std::cout<<this->GetElementName();
     std::cout<<"\nfQwStriplineCalibration = "<<fQwStriplineCalibration<<std::endl;
     std::cout<<"AlphaX = "<<fRelativeGains[0]<<std::endl;
     std::cout<<"AlphaY = "<<fRelativeGains[1]<<std::endl;

   }
  return;
};

Int_t VQwBPM::GetEventcutErrorCounters()
{
  for(Short_t i=0;i<2;i++) 
    fAbsPos[i].GetEventcutErrorCounters();
  fEffectiveCharge.GetEventcutErrorCounters();

  return 1;
};


Bool_t VQwBPM::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Short_t i=0;

 
  //Event cuts for Absolute X & Y
  for(i=0;i<2;i++){
    if (fAbsPos[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      fAbsPos[i].UpdateEventCutErrorCount();
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
    //update the event cut counters
    fAbsPos[i].UpdateHWErrorCounters();
    //Get the Event cut error flag for AbsX/Y
    fDeviceErrorCode|=fAbsPos[i].GetEventcutErrorFlag();
  }

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
    Short_t i = 0;
    for(i=0;i<2;i++) this->fAbsPos[i]=value.fAbsPos[i];
    for(i=0;i<3;i++) this->fPositionCenter[i]=value.fPositionCenter[i];
  }

  return *this;
};

VQwBPM& VQwBPM::operator+= (const VQwBPM &value)
{
  if (GetElementName()!=""){
    this->fEffectiveCharge+=value.fEffectiveCharge;
    for(Short_t i=0;i<3;i++) this->fAbsPos[i]+=value.fAbsPos[i];
  }
  return *this;
};

VQwBPM& VQwBPM::operator-= (const VQwBPM &value)
{
  if (GetElementName()!=""){
    this->fEffectiveCharge-=value.fEffectiveCharge;
    for(Short_t i=0;i<2;i++) this->fAbsPos[i]-=value.fAbsPos[i];
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
  for(Short_t i = 0;i<2;i++)fAbsPos[i].Scale(factor);
  return;
};



void VQwBPM::SetRootSaveStatus(TString &prefix)
{
  if(prefix=="diff_"||prefix=="yield_"|| prefix=="asym_")
    bFullSave=kFALSE;
  
  return;
}


