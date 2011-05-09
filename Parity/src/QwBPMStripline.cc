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

/* Position calibration factor, transform ADC counts in mm*/
//const Double_t QwBPStripline::kQwStriplineCalibration = 18.77;
//const Double_t QwBPMStripline::kRotationCorrection = 1./1.414;
const TString QwBPMStripline::subelement[4]={"XP","XM","YP","YM"};

void  QwBPMStripline::InitializeChannel(TString name)
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

void  QwBPMStripline::InitializeChannel(TString subsystem, TString name)
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

void QwBPMStripline::ClearEventData()
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


Bool_t QwBPMStripline::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

  fDeviceErrorCode=0;
  for(Short_t i=0;i<4;i++)
    {
      fDeviceErrorCode|= fWire[i].ApplyHWChecks();  //OR the error code from each wire
      fEventIsGood &= (fDeviceErrorCode & 0x0);//AND with 0 since zero means HW is good.

      if (bDEBUG) std::cout<<" Inconsistent within BPM terminals wire[ "<<i<<" ] "<<std::endl;
      if (bDEBUG) std::cout<<" wire[ "<<i<<" ] sequence num "<<fWire[i].GetSequenceNumber()<<" sample size "<<fWire[i].GetNumberOfSamples()<<std::endl;
    }

  return fEventIsGood;
}


Int_t QwBPMStripline::GetEventcutErrorCounters()
{
  Short_t i=0;

  for(i=0;i<4;i++) fWire[i].GetEventcutErrorCounters();
  for(i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].GetEventcutErrorCounters();
    fAbsPos[i].GetEventcutErrorCounters();
  }
  fEffectiveCharge.GetEventcutErrorCounters();

  return 1;
}


Bool_t QwBPMStripline::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t i=0;
  fErrorFlag=0;

  UInt_t error_code = 0;
  //Event cuts for four wires
  for(i=0;i<4;i++){
    if (fWire[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Abs X event cut failed ";
    }
    //Get the Event cut error flag for wires
    fErrorFlag|=fWire[i].GetEventcutErrorFlag();

    error_code |= fWire[i].GetErrorCode();
  }
  

   //Event cuts for Relative X & Y
  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].UpdateErrorCode(error_code);
    if (fRelPos[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" Rel X event cut failed ";
    }

    //Get the Event cut error flag for RelX/Y
    fErrorFlag|=fRelPos[i].GetEventcutErrorFlag();
  }

  for(i=kXAxis;i<kNumAxes;i++){
    fAbsPos[i].UpdateErrorCode(error_code);
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
  fEffectiveCharge.UpdateErrorCode(error_code);
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


void QwBPMStripline::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{

  if (ch_name=="xp"){
    QwMessage<<"XP LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="xm"){
    QwMessage<<"XM LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[1].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="yp"){
    QwMessage<<"YP LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[2].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="ym"){
    QwMessage<<"YM LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fWire[3].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="relx"){
    QwMessage<<"RelX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="rely"){
    QwMessage<<"RelY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[1].SetSingleEventCuts(minX,maxX);

  } else  if (ch_name=="absx"){
  //cuts for the absolute x and y
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


void QwBPMStripline::SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t minX, Double_t maxX, Double_t stability){
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

void  QwBPMStripline::ProcessEvent()
{
  Bool_t localdebug = kFALSE;
  static QwVQWK_Channel numer("numerator"), denom("denominator"), tmp1("tmp1"),tmp2("tmp2");
  static QwVQWK_Channel tmppos[2];
 
  tmppos[0].InitializeChannel("tmppos_0","derived");
  tmppos[1].InitializeChannel("tmppos_1","derived");

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
     
                                                       (XP - AlphaX.XM)
     RelX (bpm coordinates) = fQwStriplineCalibration x ----------------
                                                       (XP + AlphaX.XM) 

                                                        (YP - AlphaY.YM)
     RelY (bpm coordinates) = fQwStriplineCalibration x ----------------
                                                        (YP + AlphaY.YM)

     To get back to accelerator coordinates, rotate anti-clockwise around +Z by phi degrees (angle w.r.t X axis).							
     
     RelX (accelarator coordinates) =  cos(phi) RelX - sin(phi)RelY
    
     RelY (accelarator coordinates) =  sin(phi) RelX + cos(Phi)RelY 
 
  */

  for(i=kXAxis;i<kNumAxes;i++)
    {
      fWire[i*2+1].Scale(fRelativeGains[i]);
      numer.Difference(fWire[i*2],fWire[i*2+1]);
      denom.Sum(fWire[i*2],fWire[i*2+1]);
      fRelPos[i].Ratio(numer,denom);
      fRelPos[i].Scale(fQwStriplineCalibration);


    if(localdebug)
      {
	std::cout<<" stripline name="<<fElementName<<std::endl;
	  std::cout<<" event number= "<<fWire[i*2].GetSequenceNumber()<<std::endl;
	  std::cout<<" hw  Wire["<<i*2<<"]="<<fWire[i*2].GetHardwareSum()<<"  ";
	  std::cout<<" hw relative gain *  Wire["<<i*2+1<<"]="<<fWire[i*2+1].GetHardwareSum()<<"\n";
	  std::cout<<" Relative gain["<<i<<"]="<<fRelativeGains[i]<<"\n";
	  std::cout<<" hw numerator= "<<numer.GetHardwareSum()<<"  ";
	  std::cout<<" hw denominator= "<<denom.GetHardwareSum()<<"\n";
	  std::cout<<" Rotation = "<<fRotationAngle<<std::endl;
	}
    }
//   std::cout<<" \nstripline name="<<fElementName<<std::endl;
//   std::cout<<" Rotation = "<<fRotationAngle<<std::endl;

  for(i=kXAxis;i<kNumAxes;i++){ 
    //std::cout<<" hw  before RelPos["<<i<<"]="<<fRelPos[i].GetHardwareSum()<<"  ";
    tmp1.ClearEventData();
    tmp2.ClearEventData();
    tmppos[i].ClearEventData();
    tmp1 =  fRelPos[i];
    tmp2 =  fRelPos[1-i];
    tmp1.Scale(fCosRotation);
    tmp2.Scale(fSinRotation);
    if (i==0) 
      tmppos[i].Difference(tmp1,tmp2);
      else tmppos[i].Sum(tmp1,tmp2);
  }


  fRelPos[0]=tmppos[0];
  fRelPos[1]=tmppos[1];


  for(i=kXAxis;i<kNumAxes;i++){
    fAbsPos[i]= fRelPos[i];
    fAbsPos[i].AddChannelOffset(fPositionCenter[i]);

    if(localdebug)
      {
	std::cout<<" hw  fRelPos["<<kAxisLabel[i]<<"]="<<fRelPos[i].GetHardwareSum()<<"\n";
	std::cout<<" hw  fOffset["<<kAxisLabel[i]<<"]="<<fPositionCenter[i]<<"\n";
	std::cout<<" hw  fAbsPos["<<kAxisLabel[i]<<"]="<<fAbsPos[i].GetHardwareSum()<<"\n \n";
      }
    
  }
  
  return;
}


Int_t QwBPMStripline::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
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



void QwBPMStripline::PrintValue() const
{
  for (Short_t i = 0; i < 2; i++) {
    fAbsPos[i].PrintValue();
    fRelPos[i].PrintValue();
  }
  return;
}

void QwBPMStripline::PrintInfo() const
{
  Short_t i = 0;
  for (i = 0; i < 4; i++) fWire[i].PrintInfo();
  for (i = 0; i < 2; i++) {
    fRelPos[i].PrintInfo();
    fAbsPos[i].PrintInfo();
  }
  fEffectiveCharge.PrintInfo();
}


TString QwBPMStripline::GetSubElementName(Int_t subindex)
{
  TString thisname;
  if(subindex<4&&subindex>-1)
    thisname=fWire[subindex].GetElementName();
  else
    std::cerr<<"QwBPMStripline::GetSubElementName for "<<
      GetElementName()<<" this subindex doesn't exists \n";

  return thisname;
}

UInt_t QwBPMStripline::GetSubElementIndex(TString subname)
{
  subname.ToUpper();
  UInt_t localindex=999999;
  for(Short_t i=0;i<4;i++) if(subname==subelement[i])localindex=i;

  if(localindex>3)
    std::cerr << "QwBPMStripline::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index"<<std::endl;

  return localindex;
}

void  QwBPMStripline::GetAbsolutePosition()
{
  for(Short_t i=kXAxis;i<kNumAxes;i++){
    fAbsPos[i]= fRelPos[i];
    fAbsPos[i].AddChannelOffset(fPositionCenter[i]);
  }
  // For Z, the absolute position will be the offset we are reading from the
  // geometry map file. Since we are not putting that to the tree it is not
  // treated as a vqwk channel.
}

QwBPMStripline& QwBPMStripline::operator= (const QwBPMStripline &value)
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


QwBPMStripline& QwBPMStripline::operator+= (const QwBPMStripline &value)
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

QwBPMStripline& QwBPMStripline::operator-= (const QwBPMStripline &value)
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


void QwBPMStripline::Ratio(QwBPMStripline &numer, QwBPMStripline &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // stripline is the difference only not the asymmetries

  *this=numer;
  this->fEffectiveCharge.Ratio(numer.fEffectiveCharge,denom.fEffectiveCharge);
  return;
}



void QwBPMStripline::Scale(Double_t factor)
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


void QwBPMStripline::CalculateRunningAverage()
{
  Short_t i = 0;
  for (i = 0; i < 2; i++) fRelPos[i].CalculateRunningAverage();
  for (i = 0; i < 3; i++) fAbsPos[i].CalculateRunningAverage();
  // No data for z position
  return;
}

void QwBPMStripline::AccumulateRunningSum(const QwBPMStripline& value)
{
  // TODO This is unsafe, see QwBeamline::AccumulateRunningSum
  Short_t i = 0;
  for (i = 0; i < 2; i++) fRelPos[i].AccumulateRunningSum(value.fRelPos[i]);
  for (i = 0; i < 3; i++) fAbsPos[i].AccumulateRunningSum(value.fAbsPos[i]);
  // No data for z position
  return;
}


void  QwBPMStripline::ConstructHistograms(TDirectory *folder, TString &prefix)
{

  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }  else {
    fEffectiveCharge.ConstructHistograms(folder, prefix);
    TString thisprefix=prefix;

    if(prefix=="asym_")
      thisprefix="diff_";
    SetRootSaveStatus(prefix);
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

void  QwBPMStripline::FillHistograms()
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

void  QwBPMStripline::DeleteHistograms()
{
  if (GetElementName()=="") {
  }
  else {
    fEffectiveCharge.DeleteHistograms();
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fWire[i].DeleteHistograms();
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fRelPos[i].DeleteHistograms();
      fAbsPos[i].DeleteHistograms();
    }
  }
  return;
}


void  QwBPMStripline::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  }
  else {
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";

    SetRootSaveStatus(prefix);

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

void  QwBPMStripline::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip constructing trees.
  }
  else {
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";

    SetRootSaveStatus(prefix);

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

void  QwBPMStripline::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
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

	SetRootSaveStatus(prefix);

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

void  QwBPMStripline::FillTreeVector(std::vector<Double_t> &values) const
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

void QwBPMStripline::Copy(VQwDataElement *source)
{
  try
    {
      if( typeid(*source)==typeid(*this) ) {
       QwBPMStripline* input = ((QwBPMStripline*)source);
       this->fElementName = input->fElementName;
       this->fEffectiveCharge.Copy(&(input->fEffectiveCharge));
       this->bRotated = input->bRotated;
       this->bFullSave = input->bFullSave;
       Short_t i = 0;
       for(i = 0; i<3; i++) this->fPositionCenter[i] = input->fPositionCenter[i];
       for(i = 0; i<4; i++) this->fWire[i].Copy(&(input->fWire[i]));
       for(i = 0; i<2; i++){
	 this->fRelPos[i].Copy(&(input->fRelPos[i]));
	 this->fAbsPos[i].Copy(&(input->fAbsPos[i]));
       }
     }
      else {
       TString loc="Standard exception from QwBPMStripline::Copy = "
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

void QwBPMStripline::SetEventCutMode(Int_t bcuts)
{
  Short_t i = 0;
  bEVENTCUTMODE=bcuts;
  for (i=0;i<4;i++) fWire[i].SetEventCutMode(bcuts);
  for (i=kXAxis;i<kNumAxes;i++) {
    fRelPos[i].SetEventCutMode(bcuts);
    fAbsPos[i].SetEventCutMode(bcuts);
  }
  fEffectiveCharge.SetEventCutMode(bcuts);
}


void QwBPMStripline::MakeBPMList()
{
  UShort_t i = 0;

  QwVQWK_Channel bpm_sub_element;

  for(i=kXAxis;i<kNumAxes;i++) {
    bpm_sub_element.ClearEventData();
    bpm_sub_element.Copy(&fRelPos[i]);
    bpm_sub_element = fRelPos[i];
    fBPMElementList.push_back( bpm_sub_element );
    bpm_sub_element.ClearEventData();
    bpm_sub_element.Copy(&fAbsPos[i]);
    bpm_sub_element = fAbsPos[i];
    fBPMElementList.push_back( bpm_sub_element );
  }
  bpm_sub_element.ClearEventData();
  bpm_sub_element.Copy(&fEffectiveCharge);
  bpm_sub_element = fEffectiveCharge;
  fBPMElementList.push_back( bpm_sub_element );

  return;
}


std::vector<QwDBInterface> QwBPMStripline::GetDBEntry()
{

  UShort_t i = 0;
  UShort_t n_bpm_element = 0;

  std::vector <QwDBInterface> row_list;
  row_list.clear();

  QwDBInterface row;

  TString name;
  Double_t avg         = 0.0;
  Double_t err         = 0.0;
  UInt_t beam_subblock = 0;
  UInt_t beam_n        = 0;

  for(n_bpm_element=0; n_bpm_element<fBPMElementList.size(); n_bpm_element++) {

    row.Reset();
    // the element name and the n (number of measurements in average)
    // is the same in each block and hardwaresum.

    name          = fBPMElementList.at(n_bpm_element).GetElementName();
    beam_n        = fBPMElementList.at(n_bpm_element).GetGoodEventCount();

    // Get HardwareSum average and its error
    avg           = fBPMElementList.at(n_bpm_element).GetHardwareSum();
    err           = fBPMElementList.at(n_bpm_element).GetHardwareSumError();
    // ADC subblock sum : 0 in MySQL database
    beam_subblock = 0;

    row.SetDetectorName(name);
    row.SetSubblock(beam_subblock);
    row.SetN(beam_n);
    row.SetValue(avg);
    row.SetError(err);

    row_list.push_back(row);

    // Get four Block averages and thier errors

    for(i=0; i<4; i++) {
      row.Reset();
      avg           = fBPMElementList.at(n_bpm_element).GetBlockValue(i);
      err           = fBPMElementList.at(n_bpm_element).GetBlockErrorValue(i);
      beam_subblock = (UInt_t) (i+1);
      // QwVQWK_Channel  | MySQLif(loc
      // fBlock[0]       | subblock 1
      // fBlock[1]       | subblock 2
      // fBlock[2]       | subblock 3
      // fBlock[3]       | subblock 4
      row.SetDetectorName(name);
      row.SetSubblock(beam_subblock);
      row.SetN(beam_n);
      row.SetValue(avg);
      row.SetError(err);

      row_list.push_back(row);
    }
  }

  return row_list;

}

/**********************************
 * Mock data generation routines
 **********************************/

void  QwBPMStripline::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY)
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


void QwBPMStripline::RandomizeEventData(int helicity, double time)
{
  for (Short_t i=0; i<4; i++) fWire[i].RandomizeEventData(helicity, time);

  return;
}


void QwBPMStripline::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  for (Short_t i=0; i<2; i++)
    {
      fRelPos[i].SetHardwareSum(relpos[i], sequencenumber);
    }

  return;
}


void QwBPMStripline::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (Short_t i=0; i<4; i++) fWire[i].EncodeEventData(buffer);
}


void QwBPMStripline::SetDefaultSampleSize(Int_t sample_size)
{
  for(Short_t i=0;i<4;i++) fWire[i].SetDefaultSampleSize((size_t)sample_size);
  return;
}


void QwBPMStripline::SetSubElementPedestal(Int_t j, Double_t value)
{
  fWire[j].SetPedestal(value);
  return;
}

void QwBPMStripline::SetSubElementCalibrationFactor(Int_t j, Double_t value)
{
  fWire[j].SetCalibrationFactor(value);
  return;
}
