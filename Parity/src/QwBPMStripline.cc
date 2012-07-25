/**********************************************************\
* File: QwBPMStripline.cc                                 *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#include "QwBPMStripline.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwDBInterface.h"
#include "QwVQWK_Channel.h"
#include "QwScaler_Channel.h"


/* Position calibration factor, transform ADC counts in mm*/
//const Double_t QwBPStripline::kQwStriplineCalibration = 18.77;
//const Double_t QwBPMStripline::kRotationCorrection = 1./1.414;
template<typename T>
const TString QwBPMStripline<T>::subelement[4]={"XP","XM","YP","YM"};

template<typename T>
void  QwBPMStripline<T>::InitializeChannel(TString name)
{
  Short_t i=0;
  Bool_t localdebug = kFALSE;


  VQwBPM::InitializeChannel(name);

  for(i=kXAxis;i<kNumAxes;i++)
    fAbsPos[i].InitializeChannel(name+kAxisLabel[i],"derived");

  fEffectiveCharge.InitializeChannel(name+"_EffectiveCharge","derived");

  for(i=0;i<4;i++) {
    fWire[i].InitializeChannel(name+subelement[i],"raw");
    if(localdebug)
      std::cout<<" Wire ["<<i<<"]="<<fWire[i].GetElementName()<<"\n";
  }

  for(i=kXAxis;i<kNumAxes;i++) fRelPos[i].InitializeChannel(name+"Rel"+kAxisLabel[i],"derived");

  bFullSave=kTRUE;

  return;
}

template<typename T>
void  QwBPMStripline<T>::InitializeChannel(TString subsystem, TString name,
    TString type)
{
  SetModuleType(type);
  InitializeChannel(subsystem, name);
}

template<typename T>
void  QwBPMStripline<T>::InitializeChannel(TString subsystem, TString name)
{
  Short_t i=0;
  Bool_t localdebug = kFALSE;

  VQwBPM::InitializeChannel(name);

  for(i=kXAxis;i<kNumAxes;i++)
    fAbsPos[i].InitializeChannel(subsystem, "QwBPMStripline", name+kAxisLabel[i],"derived");

  fEffectiveCharge.InitializeChannel(subsystem, "QwBPMStripline", name+"_EffectiveCharge","derived");

  for(i=0;i<4;i++) {
    fWire[i].InitializeChannel(subsystem, "QwBPMStripline", name+subelement[i],"raw");
    if(localdebug)
      std::cout<<" Wire ["<<i<<"]="<<fWire[i].GetElementName()<<"\n";
  }

  for(i=kXAxis;i<kNumAxes;i++) fRelPos[i].InitializeChannel(subsystem, "QwBPMStripline", name+"Rel"+kAxisLabel[i],"derived");

  bFullSave=kTRUE;

  return;
}

template<typename T>
void QwBPMStripline<T>::ClearEventData()
{
  Short_t i=0;

  for(i=0;i<4;i++) fWire[i].ClearEventData();

  for(i=kXAxis;i<kNumAxes;i++){
    fAbsPos[i].ClearEventData();
    fRelPos[i].ClearEventData();
  }
  fEffectiveCharge.ClearEventData();

 return;
}


template<typename T>
Bool_t QwBPMStripline<T>::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  UInt_t deviceerror=0;
  for(Short_t i=0;i<4;i++)
    {
      deviceerror|= fWire[i].ApplyHWChecks();  //OR the error code from each wire
      eventokay &= (deviceerror & 0x0);//AND with 0 since zero means HW is good.

      //       if (bDEBUG) std::cout<<" Inconsistent within BPM terminals wire[ "<<i<<" ] "<<std::endl;
      //       if (bDEBUG) std::cout<<" wire[ "<<i<<" ] sequence num "<<fWire[i].GetSequenceNumber()<<" sample size "<<fWire[i].GetNumberOfSamples()<<std::endl;
    }

  return eventokay;
}


template<typename T>
void QwBPMStripline<T>::IncrementErrorCounters()
{
  Short_t i=0;

  for(i=0;i<4;i++) fWire[i].IncrementErrorCounters();
  for(i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].IncrementErrorCounters();
    fAbsPos[i].IncrementErrorCounters();
  }
  fEffectiveCharge.IncrementErrorCounters();
}

template<typename T>
void QwBPMStripline<T>::PrintErrorCounters() const
{
  Short_t i=0;

  for(i=0;i<4;i++) fWire[i].PrintErrorCounters();
  for(i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].PrintErrorCounters();
    fAbsPos[i].PrintErrorCounters();
  }
  fEffectiveCharge.PrintErrorCounters();
}

template<typename T>
UInt_t QwBPMStripline<T>::GetEventcutErrorFlag(){
  Short_t i=0;
  UInt_t error=0;
  for(i=0;i<4;i++) error|=fWire[i].GetEventcutErrorFlag();
  for(i=kXAxis;i<kNumAxes;i++) {
    error|=fRelPos[i].GetEventcutErrorFlag();
    error|=fAbsPos[i].GetEventcutErrorFlag();
  }
  error|=fEffectiveCharge.GetEventcutErrorFlag();  
  return error;
}

template<typename T>
UInt_t QwBPMStripline<T>::UpdateErrorFlag()
{
  Short_t i=0;
  UInt_t error1=0;
  UInt_t error2=0;
  for(i=0;i<4;i++){
    error1 |= fWire[i].GetErrorCode();
    error2 |= fWire[i].GetEventcutErrorFlag();
  }
  for(i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].UpdateErrorFlag(error1);
    fAbsPos[i].UpdateErrorFlag(error1);
    error2|=fRelPos[i].GetEventcutErrorFlag();
    error2|=fAbsPos[i].GetEventcutErrorFlag();
  }
  fEffectiveCharge.UpdateErrorFlag(error1);
  error2|=fEffectiveCharge.GetEventcutErrorFlag();
  return error2;
};


template<typename T>
void QwBPMStripline<T>::UpdateErrorFlag(const VQwBPM *ev_error){
  Short_t i=0;
  try {
    if(typeid(*ev_error)==typeid(*this)) {
      // std::cout<<" Here in QwBPMStripline::UpdateErrorFlag \n";
      if (this->GetElementName()!="") {
        const QwBPMStripline<T>* value_bpm = dynamic_cast<const QwBPMStripline<T>* >(ev_error);
	for(i=0;i<4;i++){
	  fWire[i].UpdateErrorFlag(value_bpm->fWire[i]);
	}
	for(i=kXAxis;i<kNumAxes;i++) {
	  fRelPos[i].UpdateErrorFlag(value_bpm->fRelPos[i]);
	  fAbsPos[i].UpdateErrorFlag(value_bpm->fAbsPos[i]); 
	}
	fEffectiveCharge.UpdateErrorFlag(value_bpm->fEffectiveCharge); 
      }
    } else {
      TString loc="Standard exception from QwBPMStripline::UpdateErrorFlag :"+
        ev_error->GetElementName()+" "+this->GetElementName()+" are not of the "
        +"same type";
      throw std::invalid_argument(loc.Data());
    }
  } catch (std::exception& e) {
    std::cerr<< e.what()<<std::endl;
  }  
};


template<typename T>
Bool_t QwBPMStripline<T>::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t i=0;

  UInt_t element_error_code[2];
  //Event cuts for four wires
  for(i=0;i<4;i++){
    if (fWire[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
  }

  //Get the rex/abs X event cut error flag from xm and xp
  element_error_code[kXAxis] = GetSubelementByName("xm")->GetErrorCode() | GetSubelementByName("xp")->GetErrorCode();
  //Get the rex/abs Y event cut  error flag from ym and yp
  element_error_code[kYAxis] = GetSubelementByName("ym")->GetErrorCode() | GetSubelementByName("yp")->GetErrorCode();
  //Update the error flags for rel and abs positions
  fRelPos[kXAxis].UpdateErrorFlag(element_error_code[kXAxis]);
  fRelPos[kYAxis].UpdateErrorFlag(element_error_code[kYAxis]);
  fAbsPos[kXAxis].UpdateErrorFlag(element_error_code[kXAxis]);
  fAbsPos[kYAxis].UpdateErrorFlag(element_error_code[kYAxis]);
  //update the sum of error flags of all wires to the charge element
  fEffectiveCharge.UpdateErrorFlag(element_error_code[kXAxis]|element_error_code[kYAxis]);

  

   //Event cuts for Relative X & Y
  for(i=kXAxis;i<kNumAxes;i++){
    if (fRelPos[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Rel X event cut failed ";
    }
  }

  for(i=kXAxis;i<kNumAxes;i++){
    if (fAbsPos[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
  }

  //Event cuts for four wire sum (EffectiveCharge) are already ORed when EffectiveCharge is calculated
  if (fEffectiveCharge.ApplySingleEventCuts()){
      status&=kTRUE;
  }
  else{
    status&=kFALSE;
    if (bDEBUG) std::cout<<"EffectiveCharge event cut failed ";
  }
  return status;
}

template<typename T>
VQwHardwareChannel* QwBPMStripline<T>::GetSubelementByName(TString ch_name)
{
  VQwHardwareChannel* tmpptr = NULL;
  ch_name.ToLower();
  if (ch_name=="xp"){
    tmpptr = &fWire[0];
  }else if (ch_name=="xm"){
    tmpptr = &fWire[1];
  }else if (ch_name=="yp"){
    tmpptr = &fWire[2];
  }else if (ch_name=="ym"){
    tmpptr = &fWire[3];
  }else if (ch_name=="relx"){
    tmpptr = &fRelPos[0];
  }else if (ch_name=="rely"){
    tmpptr = &fRelPos[1];
  }else  if (ch_name=="absx" || ch_name=="x" ){
    tmpptr = &fAbsPos[0];
  }else if (ch_name=="absy" || ch_name=="y"){
    tmpptr = &fAbsPos[1];
  }else if (ch_name=="effectivecharge" || ch_name=="charge"){
    tmpptr = &fEffectiveCharge;
  } else {
    TString loc="QwBPMStripline::GetSubelementByName for"
      + this->GetElementName() + " was passed "
      + ch_name + ", which is an unrecognized subelement name.";
    throw std::invalid_argument(loc.Data());
  }
  return tmpptr;
}

/*
template<typename T>
void QwBPMStripline<T>::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{
  VQwHardwareChannel* tmpptr = GetSubelementByName(ch_name);
  QwMessage << ch_name 
	    << " LL " <<  minX <<" UL " << maxX <<QwLog::endl;
  tmpptr->SetSingleEventCuts(minX,maxX);
}


template<typename T>
void QwBPMStripline<T>::SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t minX, Double_t maxX, Double_t stability){
  errorflag|=kBPMErrorFlag;//update the device flag
  if (ch_name=="xp"){
    QwMessage<<"XP LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[0].SetSingleEventCuts(errorflag,minX,maxX,stability);

  }else if (ch_name=="xm"){
    QwMessage<<"XM LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[1].SetSingleEventCuts(errorflag,minX,maxX,stability);

  }else if (ch_name=="yp"){
    QwMessage<<"YP LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[2].SetSingleEventCuts(errorflag,minX,maxX,stability);

  }else if (ch_name=="ym"){
    QwMessage<<"YM LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[3].SetSingleEventCuts(errorflag,minX,maxX,stability);

  }else if (ch_name=="relx"){
    QwMessage<<"RelX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[0].SetSingleEventCuts(errorflag,minX,maxX,stability);

  }else if (ch_name=="rely"){
    QwMessage<<"RelY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[1].SetSingleEventCuts(errorflag,minX,maxX,stability);

  } else  if (ch_name=="absx"){
  //cuts for the absolute x and y
    QwMessage<<"AbsX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[0].SetSingleEventCuts(errorflag,minX,maxX,stability);

  }else if (ch_name=="absy"){
    QwMessage<<"AbsY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fAbsPos[1].SetSingleEventCuts(errorflag,minX,maxX,stability);

  }else if (ch_name=="effectivecharge"){
    QwMessage<<"EffectveQ LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fEffectiveCharge.SetSingleEventCuts(errorflag,minX,maxX,stability);

  }
}

*/

template<typename T>
void  QwBPMStripline<T>::ProcessEvent()
{
  Bool_t localdebug = kFALSE;
  static T numer("numerator","derived"), denom("denominator","derived");
  static T tmp1("tmp1","derived"), tmp2("tmp2","derived");
  static T rawpos[2] = {T("rawpos_0","derived"),T("rawpos_1","derived")};

  Short_t i = 0;

  ApplyHWChecks();
  /**First apply HW checks and update HW  error flags. 
     Calling this routine here and not in ApplySingleEventCuts  
     makes a difference for a BPMs because they have derrived devices.
  */

  fEffectiveCharge.ClearEventData();

  for(i=0;i<4;i++)
    {
      fWire[i].ProcessEvent();
      fEffectiveCharge+=fWire[i];
    }


  /**
     To obtain the beam position in X and Y in the CEBAF coordinates, we use the following equations
     
                                                                 (XP - AlphaX XM)
     RelX (bpm coordinates) = fQwStriplineCalibration x GainX x ----------------
                                                                 (XP + AlphaX XM) 

                                                                  (YP - AplhaY YM)
     RelY (bpm coordinates) = fQwStriplineCalibration x GainY x ----------------
                                                                  (YP + AlphaY YM)

     To get back to accelerator coordinates, rotate anti-clockwise around +Z by phi degrees (angle w.r.t X axis).							
     
     RelX (accelarator coordinates) =  cos(phi) RelX - sin(phi)RelY
    
     RelY (accelarator coordinates) =  sin(phi) RelX + cos(Phi)RelY 
 
  */

  for(i=kXAxis;i<kNumAxes;i++)
    {
      fWire[i*2+1].Scale(fRelativeGains[i]);
      numer.Difference(fWire[i*2],fWire[i*2+1]);
      denom.Sum(fWire[i*2],fWire[i*2+1]);
      rawpos[i].Ratio(numer,denom);
      rawpos[i].Scale(fQwStriplineCalibration);

      if(localdebug)
	{
	  std::cout<<" stripline name="<<fElementName<<std::endl;
	  //	  std::cout<<" event number= "<<fWire[i*2].GetSequenceNumber()<<std::endl;
	  std::cout<<" hw  Wire["<<i*2<<"]="<<fWire[i*2].GetValue()<<"  ";
	  std::cout<<" hw relative gain *  Wire["<<i*2+1<<"]="<<fWire[i*2+1].GetValue()<<"\n";
	  std::cout<<" Relative gain["<<i<<"]="<<fRelativeGains[i]<<"\n";
	  std::cout<<" hw numerator= "<<numer.GetValue()<<"  ";
	  std::cout<<" hw denominator= "<<denom.GetValue()<<"\n";
	  std::cout<<" Rotation = "<<fRotationAngle<<std::endl;
	}
    }

  for(i=kXAxis;i<kNumAxes;i++){ 
    tmp1.AssignScaledValue(rawpos[i],   fCosRotation);
    tmp2.AssignScaledValue(rawpos[1-i], fSinRotation);
    if (i == kXAxis) {
      fRelPos[i].Difference(tmp1,tmp2);
    } else {
      fRelPos[i].Sum(tmp1,tmp2);
    }
  }


  for(i=kXAxis;i<kNumAxes;i++){
    fAbsPos[i] = fRelPos[i];
    fAbsPos[i].AddChannelOffset(fPositionCenter[i]);
    fAbsPos[i].Scale(1.0/fGains[i]);

    if(localdebug)
    {
	std::cout<<" hw  fRelPos["<<kAxisLabel[i]<<"]="<<fRelPos[i].GetValue()<<"\n";
	std::cout<<" hw  fOffset["<<kAxisLabel[i]<<"]="<<fPositionCenter[i]<<"\n";
	std::cout<<" hw  fAbsPos["<<kAxisLabel[i]<<"]="<<fAbsPos[i].GetValue()<<"\n \n";
    }
    
  }
  
  return;
}


template<typename T>
Int_t QwBPMStripline<T>::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
  if(index<4)
    {
      fWire[index].ProcessEvBuffer(buffer,word_position_in_buffer);
    }
  else
    {
    std::cerr <<
      "QwBPMStripline::ProcessEvBuffer(): attemp to fill in raw date for a wire that doesn't exist \n";
    }
  return word_position_in_buffer;
}



template<typename T>
void QwBPMStripline<T>::PrintValue() const
{
  for (Short_t i = 0; i < 2; i++) {
    fAbsPos[i].PrintValue();
    fRelPos[i].PrintValue();
  }
  return;
}


template<typename T>
void QwBPMStripline<T>::WritePromptSummary(QwPromptSummary *ps, TString type)
{

  QwMessage << "void QwBPMStripline<T>::WritePromptSummary() const test " << QwLog::endl;
  //  for (Short_t i = 0; i < 2; i++) {
  //    fAbsPos[i].PrintValue();
  //    fRelPos[i].PrintValue();
  //  }
  return;
}


template<typename T>
void QwBPMStripline<T>::PrintInfo() const
{
  Short_t i = 0;
  for (i = 0; i < 4; i++) fWire[i].PrintInfo();
  for (i = 0; i < 2; i++) {
    fRelPos[i].PrintInfo();
    fAbsPos[i].PrintInfo();
  }
  fEffectiveCharge.PrintInfo();
}


template<typename T>
TString QwBPMStripline<T>::GetSubElementName(Int_t subindex)
{
  TString thisname;
  if(subindex<4&&subindex>-1)
    thisname=fWire[subindex].GetElementName();
  else
    std::cerr<<"QwBPMStripline::GetSubElementName for "<<
      GetElementName()<<" this subindex doesn't exists \n";

  return thisname;
}

template<typename T>
UInt_t QwBPMStripline<T>::GetSubElementIndex(TString subname)
{
  subname.ToUpper();
  UInt_t localindex = kInvalidSubelementIndex;
  for(Short_t i=0;i<4;i++) if(subname==subelement[i])localindex=i;

  if(localindex==kInvalidSubelementIndex)
    std::cerr << "QwBPMStripline::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index"<<std::endl;
  return localindex;
}

template<typename T>
void  QwBPMStripline<T>::GetAbsolutePosition()
{
  for(Short_t i=kXAxis;i<kNumAxes;i++){
    fAbsPos[i]= fRelPos[i];
    fAbsPos[i].AddChannelOffset(fPositionCenter[i]);
  }
  // For Z, the absolute position will be the offset we are reading from the
  // geometry map file. Since we are not putting that to the tree it is not
  // treated as a vqwk channel.
}


template<typename T>
VQwBPM& QwBPMStripline<T>::operator= (const VQwBPM &value)
{
  *(dynamic_cast<QwBPMStripline<T>*>(this)) =
    *(dynamic_cast<const QwBPMStripline<T>*>(&value));
  return *this;
}

template<typename T>
QwBPMStripline<T>& QwBPMStripline<T>::operator= (const QwBPMStripline<T> &value)
{
  VQwBPM::operator= (value);

  this->bRotated=value.bRotated;
  if (GetElementName()!=""){
    Short_t i = 0;
    this->fEffectiveCharge=value.fEffectiveCharge;
    for(i=0;i<4;i++) this->fWire[i]=value.fWire[i];
    for(i=kXAxis;i<kNumAxes;i++) {
      this->fRelPos[i]=value.fRelPos[i];
      this->fAbsPos[i]=value.fAbsPos[i];
    }
  }
  return *this;
}

template<typename T>
VQwBPM& QwBPMStripline<T>::operator+= (const VQwBPM &value)
{
  *(dynamic_cast<QwBPMStripline<T>*>(this)) +=
    *(dynamic_cast<const QwBPMStripline<T>*>(&value));
  return *this;
}

template<typename T>
QwBPMStripline<T>& QwBPMStripline<T>::operator+= (const QwBPMStripline<T> &value)
{

  if (GetElementName()!=""){
    Short_t i = 0;
    this->fEffectiveCharge+=value.fEffectiveCharge;
    for(i=0;i<4;i++) this->fWire[i]+=value.fWire[i];
    for(i=kXAxis;i<kNumAxes;i++) {
      this->fRelPos[i]+=value.fRelPos[i];
      this->fAbsPos[i]+=value.fAbsPos[i];
    }
  }
  return *this;
}

template<typename T>
VQwBPM& QwBPMStripline<T>::operator-= (const VQwBPM &value)
{
  *(dynamic_cast<QwBPMStripline<T>*>(this)) -=
    *(dynamic_cast<const QwBPMStripline<T>*>(&value));
  return *this;
}
template<typename T>
QwBPMStripline<T>& QwBPMStripline<T>::operator-= (const QwBPMStripline<T> &value)
{

  if (GetElementName()!=""){
    Short_t i = 0;
    this->fEffectiveCharge-=value.fEffectiveCharge;
    for(i=0;i<4;i++) this->fWire[i]-=value.fWire[i];
    for(i=kXAxis;i<kNumAxes;i++) {
      this->fRelPos[i]-=value.fRelPos[i];
      this->fAbsPos[i]-=value.fAbsPos[i];
    }
  }
  return *this;
}

template<typename T>
void QwBPMStripline<T>::Ratio( VQwBPM &numer, VQwBPM &denom)
{
  Ratio(*dynamic_cast<QwBPMStripline<T>*>(&numer),
      *dynamic_cast<QwBPMStripline<T>*>(&denom));
}

template<typename T>
void QwBPMStripline<T>::Ratio( QwBPMStripline<T> &numer, QwBPMStripline<T> &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // stripline is the difference only not the asymmetries

  *this=numer;
  this->fEffectiveCharge.Ratio(numer.fEffectiveCharge,denom.fEffectiveCharge);
  return;
}



template<typename T>
void QwBPMStripline<T>::Scale(Double_t factor)
{
  Short_t i = 0;
  fEffectiveCharge.Scale(factor);

  for(i=0;i<4;i++) fWire[i].Scale(factor);
  for(Short_t i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].Scale(factor);
    fAbsPos[i].Scale(factor);
  }
  return;
}


template<typename T>
void QwBPMStripline<T>::CalculateRunningAverage()
{
  Short_t i = 0;
  for (i = 0; i < 4; i++){
    fWire[i].CalculateRunningAverage();
  }

  for (i = 0; i < 2; i++){
    fRelPos[i].CalculateRunningAverage();
    fAbsPos[i].CalculateRunningAverage();
  }
  fEffectiveCharge.CalculateRunningAverage();
  return;
}


template<typename T>
void QwBPMStripline<T>::AccumulateRunningSum(const VQwBPM& value)
{
  AccumulateRunningSum(*dynamic_cast<const QwBPMStripline<T>* >(&value));
}

template<typename T>
void QwBPMStripline<T>::AccumulateRunningSum(const QwBPMStripline<T>& value)
{
  Short_t i = 0;
  for (i = 0; i < 4; i++){
    fWire[i].AccumulateRunningSum(value.fWire[i]);
  }
  for (i = 0; i < 2; i++){
    fRelPos[i].AccumulateRunningSum(value.fRelPos[i]);
    fAbsPos[i].AccumulateRunningSum(value.fAbsPos[i]);
  }
  fEffectiveCharge.AccumulateRunningSum(value.fEffectiveCharge);
  return;
}
template<typename T>
void    QwBPMStripline<T>::DeaccumulateRunningSum(VQwBPM& value){
  DeaccumulateRunningSum(*dynamic_cast<QwBPMStripline<T>* >(&value));
};

template<typename T>
void    QwBPMStripline<T>::DeaccumulateRunningSum(QwBPMStripline<T>& value){
  Short_t i = 0;
  for (i = 0; i < 4; i++){
    fWire[i].DeaccumulateRunningSum(value.fWire[i]);
  }
  for (i = 0; i < 2; i++){
    fRelPos[i].DeaccumulateRunningSum(value.fRelPos[i]);
    fAbsPos[i].DeaccumulateRunningSum(value.fAbsPos[i]);
  }
  fEffectiveCharge.DeaccumulateRunningSum(value.fEffectiveCharge);  return;
  
};

template<typename T>
void  QwBPMStripline<T>::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }  else {
    fEffectiveCharge.ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;

    if(prefix=="asym_")
      thisprefix="diff_";
    this->SetRootSaveStatus(prefix);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].ConstructHistograms(folder, thisprefix);
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fRelPos[i].ConstructHistograms(folder, thisprefix);
      fAbsPos[i].ConstructHistograms(folder, thisprefix);
    }
  }
  return;
}

template<typename T>
void  QwBPMStripline<T>::FillHistograms()
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }
  else {
    fEffectiveCharge.FillHistograms();
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].FillHistograms();
    }
    for(i=kXAxis;i<kNumAxes;i++){
      fRelPos[i].FillHistograms();
      fAbsPos[i].FillHistograms();
    }
    //No data for z position
  }
  return;
}

template<typename T>
void  QwBPMStripline<T>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  }
  else {
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";

    this->SetRootSaveStatus(prefix);

    fEffectiveCharge.ConstructBranchAndVector(tree,prefix,values);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].ConstructBranchAndVector(tree,thisprefix,values);
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
      fAbsPos[i].ConstructBranchAndVector(tree,thisprefix,values);
    }

  }
  return;
}

template<typename T>
void  QwBPMStripline<T>::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  }
  else {
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";

    this->SetRootSaveStatus(prefix);

    fEffectiveCharge.ConstructBranch(tree,prefix);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].ConstructBranch(tree,thisprefix);
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fRelPos[i].ConstructBranch(tree,thisprefix);
      fAbsPos[i].ConstructBranch(tree,thisprefix);
    }

  }
  return;
}

template<typename T>
void  QwBPMStripline<T>::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;
  /*
  QwMessage <<" QwBCM::ConstructBranch "<<QwLog::endl;
  modulelist.RewindToFileStart();
  while (modulelist.ReadNextLine()){
      modulelist.TrimComment('!');   // Remove everything after a '!' character.
      modulelist.TrimWhitespace();   // Get rid of leading and trailing spaces.
      QwMessage <<" "<<modulelist.GetLine()<<" ";
  }
  QwMessage <<QwLog::endl;
  */
  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else
    {
      if (modulelist.HasValue(devicename)){
	TString thisprefix=prefix;
	if(prefix=="asym_")
	  thisprefix="diff_";

	this->SetRootSaveStatus(prefix);

	fEffectiveCharge.ConstructBranch(tree,prefix);
	Short_t i = 0;
	if(bFullSave) {
	  for(i=0;i<4;i++) fWire[i].ConstructBranch(tree,thisprefix);
	}
	for(i=kXAxis;i<kNumAxes;i++) {
	  fRelPos[i].ConstructBranch(tree,thisprefix);
	  fAbsPos[i].ConstructBranch(tree,thisprefix);
	}

	QwMessage <<" Tree leaves added to "<<devicename<<" Corresponding channels"<<QwLog::endl;
      }
      // this functions doesn't do anything yet
    }





  return;
}

template<typename T>
void  QwBPMStripline<T>::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the tree.
  }
  else {
    fEffectiveCharge.FillTreeVector(values);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].FillTreeVector(values);
    }
    for(i=kXAxis;i<kNumAxes;i++){
      fRelPos[i].FillTreeVector(values);
      fAbsPos[i].FillTreeVector(values);
    }
  }
  return;
}

template<typename T>
void QwBPMStripline<T>::SetEventCutMode(Int_t bcuts)
{
  Short_t i = 0;
  //  bEVENTCUTMODE=bcuts;
  for (i=0;i<4;i++) fWire[i].SetEventCutMode(bcuts);
  for (i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].SetEventCutMode(bcuts);
    fAbsPos[i].SetEventCutMode(bcuts);
  }
  fEffectiveCharge.SetEventCutMode(bcuts);
}


template<typename T>
void QwBPMStripline<T>::MakeBPMList()
{
  for(size_t i=kXAxis;i<kNumAxes;i++) {
    T relpos(fRelPos[i]);
    relpos = fRelPos[i];                     // data
    fBPMElementList.push_back(relpos);
    T abspos(fAbsPos[i]);
    abspos = fAbsPos[i];                     // data
    fBPMElementList.push_back(abspos);
  }
  T bpm_sub_element(fEffectiveCharge);
  bpm_sub_element = fEffectiveCharge;
  fBPMElementList.push_back(bpm_sub_element);
}


template<typename T>
std::vector<QwDBInterface> QwBPMStripline<T>::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();

  for(size_t i=0;i<2;i++) {
    fRelPos[i].AddEntriesToList(row_list);
    fAbsPos[i].AddEntriesToList(row_list);
  }
  fEffectiveCharge.AddEntriesToList(row_list);
  return row_list;
}


template<typename T>
std::vector<QwErrDBInterface> QwBPMStripline<T>::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  row_list.clear();

  for(size_t i=0;i<2;i++) {
    fRelPos[i].AddErrEntriesToList(row_list);
    fAbsPos[i].AddErrEntriesToList(row_list);
  }
  fEffectiveCharge.AddErrEntriesToList(row_list);
  return row_list;
}



/**********************************
 * Mock data generation routines
 **********************************/

template<typename T>
void  QwBPMStripline<T>::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY)
{
  // Average values of the signals in the stripline ADCs
  Double_t sumX = 1.1e8; // These are just guesses, but I made X and Y different
  Double_t sumY = 0.9e8; // to make it more interesting for the analyzer...

  // Rotate the requested position if necessary (this is not tested yet)
  if (bRotated) {
    Double_t rotated_meanX = (meanX*fCosRotation - meanY*fSinRotation);// / fRotationCorrection;
    Double_t rotated_meanY = (meanX*fSinRotation + meanY*fCosRotation);// / fRotationCorrection;
    meanX = rotated_meanX;
    meanY = rotated_meanY;
  }

  // Determine the asymmetry from the position
  Double_t meanXP = (1.0 + meanX / fQwStriplineCalibration) * sumX / 2.0;
  Double_t meanXM = (1.0 - meanX / fQwStriplineCalibration) * sumX / 2.0; // = sumX - meanXP;
  Double_t meanYP = (1.0 + meanY / fQwStriplineCalibration) * sumY / 2.0;
  Double_t meanYM = (1.0 - meanY / fQwStriplineCalibration) * sumY / 2.0; // = sumY - meanYP;

  // Determine the spread of the asymmetry (this is not tested yet)
  // (negative sigma should work in the QwVQWK_Channel, but still using fabs)
  Double_t sigmaXP = fabs(sumX * sigmaX / meanX);
  Double_t sigmaXM = sigmaXP;
  Double_t sigmaYP = fabs(sumY * sigmaY / meanY);
  Double_t sigmaYM = sigmaYP;

  // Propagate these parameters to the ADCs
  fWire[0].SetRandomEventParameters(meanXP, sigmaXP);
  fWire[1].SetRandomEventParameters(meanXM, sigmaXM);
  fWire[2].SetRandomEventParameters(meanYP, sigmaYP);
  fWire[3].SetRandomEventParameters(meanYM, sigmaYM);
}


template<typename T>
void QwBPMStripline<T>::RandomizeEventData(int helicity, double time)
{
  for (Short_t i=0; i<4; i++) fWire[i].RandomizeEventData(helicity, time);

  return;
}


template<typename T>
void QwBPMStripline<T>::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  for (Short_t i=0; i<2; i++)
    {
      //fRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
    }

  return;
}


template<typename T>
void QwBPMStripline<T>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (Short_t i=0; i<4; i++) fWire[i].EncodeEventData(buffer);
}


template<typename T>
void QwBPMStripline<T>::SetDefaultSampleSize(Int_t sample_size)
{
  for(Short_t i=0;i<4;i++) fWire[i].SetDefaultSampleSize((size_t)sample_size);
  return;
}


template<typename T>
void QwBPMStripline<T>::SetSubElementPedestal(Int_t j, Double_t value)
{
  fWire[j].SetPedestal(value);
  return;
}

template<typename T>
void QwBPMStripline<T>::SetSubElementCalibrationFactor(Int_t j, Double_t value)
{
  fWire[j].SetCalibrationFactor(value);
  return;
}


template class QwBPMStripline<QwVQWK_Channel>; 
template class QwBPMStripline<QwSIS3801_Channel>; 
template class QwBPMStripline<QwSIS3801D24_Channel>;
