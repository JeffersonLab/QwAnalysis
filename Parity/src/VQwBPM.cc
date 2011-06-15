/**********************************************************\
* File: VQwBPM.cc                                         *
*                                                         *
* Author:B.Waidyawansa                                    *
* Time-stamp:03-06-2010                                   *
\**********************************************************/

#include "VQwBPM.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwLog.h"


/* With X being vertical up and Z being the beam direction toward the beamdump */
const TString  VQwBPM::axis[3]={"X","Y","Z"};


void  VQwBPM::InitializeChannel(TString name)
{

  bEVENTCUTMODE    = false;
  fDeviceErrorCode = 0;
  fErrorFlag=0;
  Short_t i = 0; 

  for(i=0;i<2;i++)
    fAbsPos[i].InitializeChannel(name+axis[i],"derived");
  
  fEffectiveCharge.InitializeChannel(name+"_EffectiveCharge","derived");
  
  for(i=0;i<3;i++) fPositionCenter[i] = 0.0;

  SetElementName(name);
  
  return;
}

void VQwBPM::ClearEventData()
{
  for(Short_t i=0;i<2;i++)
    fAbsPos[i].ClearEventData();

  fEffectiveCharge.ClearEventData();
  
  return;
}

void VQwBPM::GetSurveyOffsets(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset)
{
  // Read in the position offsets from the geometry map file
  for(Short_t i=0;i<3;i++) fPositionCenter[i]=0.0;
  fPositionCenter[0]=Xoffset;
  fPositionCenter[1]=Yoffset;
  fPositionCenter[2]=Zoffset;
  return;
}


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
}

void VQwBPM::SetRotation(Double_t rotation_angle){
  // Read the rotation angle in degrees (to beam right)
  Bool_t ldebug = kFALSE;
  fSinRotation = 0;
  fSinRotation = 0;
  fRotationAngle = rotation_angle;
  fSinRotation = TMath::Sin(fRotationAngle*(TMath::DegToRad()));
  fCosRotation = TMath::Cos(fRotationAngle*(TMath::DegToRad()));

  if(ldebug){
    std::cout<<"\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    std::cout<<this->GetElementName();
    std::cout<<" is rotated by angle = "<<rotation_angle<<std::endl;
    
  }
}

void VQwBPM::SetRotationOff(){
  // Turn off rotation. This object is already in accelerator coordinates.
  fRotationAngle = 0.0;
  SetRotation(fRotationAngle);
  bRotated=kFALSE;
}

void VQwBPM::SetGains(TString pos, Double_t value){
  if(pos.Contains("X")) fGains[0] = value;
  if(pos.Contains("Y")) fGains[1] = value;
}

Int_t VQwBPM::GetEventcutErrorCounters()
{
  for(Short_t i=0;i<2;i++) 
    fAbsPos[i].GetEventcutErrorCounters();
  fEffectiveCharge.GetEventcutErrorCounters();

  return 1;
}


Bool_t VQwBPM::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Short_t i=0;
  fErrorFlag=0;
 
  //Event cuts for Absolute X & Y
  for(i=0;i<2;i++){
    if (fAbsPos[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
    //Get the Event cut error flag for AbsX/Y
    fErrorFlag|=fAbsPos[i].GetEventcutErrorFlag();
  }

 //Event cuts for four wire sum (EffectiveCharge)
  if (fEffectiveCharge.ApplySingleEventCuts()){ 
      status&=kTRUE;
  }
  else{
    status&=kFALSE;
    if (bDEBUG) std::cout<<"EffectiveCharge event cut failed ";
  }
  //Get the Event cut error flag for EffectiveCharge
  fErrorFlag|=fEffectiveCharge.GetEventcutErrorFlag();


  return status;
  
}

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
}


VQwBPM& VQwBPM::operator= (const VQwBPM &value)
{
  if (GetElementName()!=""){
    this->fEffectiveCharge=value.fEffectiveCharge;
    Short_t i = 0;
    for(i=0;i<2;i++) this->fAbsPos[i]=value.fAbsPos[i];
    for(i=0;i<3;i++) this->fPositionCenter[i]=value.fPositionCenter[i];
  }

  return *this;
}

VQwBPM& VQwBPM::operator+= (const VQwBPM &value)
{
  if (GetElementName()!=""){
    this->fEffectiveCharge+=value.fEffectiveCharge;
    for(Short_t i=0;i<2;i++) this->fAbsPos[i]+=value.fAbsPos[i];
  }
  return *this;
}

VQwBPM& VQwBPM::operator-= (const VQwBPM &value)
{
  if (GetElementName()!=""){
    this->fEffectiveCharge-=value.fEffectiveCharge;
    for(Short_t i=0;i<2;i++) this->fAbsPos[i]-=value.fAbsPos[i];
  }
  return *this;
}


void VQwBPM::Sum(VQwBPM &value1, VQwBPM &value2)
{
  *this =  value1;
  *this += value2;
}

void VQwBPM::Difference(VQwBPM &value1, VQwBPM &value2)
{
  *this =  value1;
  *this -= value2;
}


void VQwBPM::Scale(Double_t factor)
{
  fEffectiveCharge.Scale(factor);
  for(Short_t i = 0;i<2;i++)fAbsPos[i].Scale(factor);
  return;
}




void VQwBPM::SetRootSaveStatus(TString &prefix)
{
  if(prefix=="diff_"||prefix=="yield_"|| prefix=="asym_")
    bFullSave=kFALSE;
  
  return;
}

void VQwBPM::PrintValue() const
{
  Short_t i;
  for (i = 0; i < 2; i++) fAbsPos[i].PrintValue();
  fEffectiveCharge.PrintValue();
    
  return;
}

void VQwBPM::PrintInfo() const
{
  Short_t i = 0;
  for (i = 0; i < 4; i++)  fAbsPos[i].PrintInfo();
  fEffectiveCharge.PrintInfo();
}


void VQwBPM::CalculateRunningAverage()
{
  Short_t i = 0;
  for (i = 0; i < 2; i++) fAbsPos[i].CalculateRunningAverage();
  fEffectiveCharge.CalculateRunningAverage();

  return;
}


void VQwBPM::AccumulateRunningSum(const VQwBPM& value)
{
  // TODO This is unsafe, see QwBeamline::AccumulateRunningSum
  Short_t i = 0;
  for (i = 0; i < 2; i++) fAbsPos[i].AccumulateRunningSum(value.fAbsPos[i]);
  fEffectiveCharge.AccumulateRunningSum(value.fEffectiveCharge);
  return;
}


/********************************************************/
void  VQwBPM::Copy(VQwBPM *source)
{
  try
    {
      if(typeid(*source)==typeid(*this))
	{
	  VQwBPM* input=((VQwBPM*)source);
	  this->fElementName = input->fElementName;
	  this->bFullSave = input->bFullSave;
	  this->fEffectiveCharge.Copy(&(input->fEffectiveCharge));
	  for(size_t i =0;i<2;i++)
	    this->fAbsPos[i].Copy(&(input->fAbsPos[i]));

	}
      else
	{
	  TString loc="Standard exception from VQwBPM::Copy = "
	    +source->GetElementName()+" "
	    +this->GetElementName()+" are not of the same type";
	  throw std::invalid_argument(loc.Data());
	}
    }
  catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }

  return;
}
