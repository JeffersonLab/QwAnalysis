/**********************************************************\
* File: QwQPD.cc                                          *
*                                                         *
* Author: B. Waidyawansa                                  *
* Time-stamp:  09-14-2010                                 *
\**********************************************************/

#include "QwQPD.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwDBInterface.h"

/* Position calibration factor, transform ADC counts in mm*/
const TString  QwQPD::subelement[4]={"BR","TR","BL","TL"};


void  QwQPD::InitializeChannel(TString name)
{
  Short_t i=0;
  Bool_t localdebug = kFALSE;


  //  fEffectiveCharge.InitializeChannel(name+"_EffectiveCharge","derived");

  for(i=0;i<4;i++) {
    fPhotodiode[i].InitializeChannel(name+subelement[i],"raw");
    
    if(localdebug)
      std::cout<<" photodiode ["<<i<<"]="<<fPhotodiode[i].GetElementName()<<"\n";
  }
  
  for(i=kXAxis;i<kNumAxes;i++) 
    fRelPos[i].InitializeChannel(name+"Rel"+kAxisLabel[i],"derived");

  VQwBPM::InitializeChannel(name);
  
  bFullSave=kTRUE;

  return;
}

void  QwQPD::InitializeChannel(TString subsystem, TString name)
{
  Short_t i=0;
  Bool_t localdebug = kFALSE;

  VQwBPM::InitializeChannel(name);

  fEffectiveCharge.InitializeChannel(subsystem, "QwQPD", name+"_EffectiveCharge","derived");

  for(i=0;i<4;i++) {
    fPhotodiode[i].InitializeChannel(subsystem, "QwQPD", name+subelement[i],"raw");
    if(localdebug)
      std::cout<<" photodiode ["<<i<<"]="<<fPhotodiode[i].GetElementName()<<"\n";
  }
  for(i=kXAxis;i<kNumAxes;i++) fRelPos[i].InitializeChannel(subsystem, "QwQPD", name+"Rel"+kAxisLabel[i],"derived");
  for(i=kXAxis;i<kNumAxes;i++) fAbsPos[i].InitializeChannel(subsystem, "QwQPD", name+kAxisLabel[i],"derived");

  bFullSave=kTRUE;

  return;
}

void QwQPD::GetCalibrationFactors(Double_t AlphaX, Double_t AlphaY)
{
  // Read in the calibration factors from the injector_beamline_geometry.map
  // for the QPD, AlphaX and AlphaY gives the conversion from adc counts to mm.
 
  Bool_t ldebug = kFALSE;

  fQwQPDCalibration[0]=1.0/AlphaX;
  fQwQPDCalibration[1]=1.0/AlphaY;

  if(ldebug){
    std::cout<<"\n%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n";
    std::cout<<this->GetElementName();
    std::cout<<"\nfQwQPDCalibration[0]  = "<<fQwQPDCalibration[0]<<std::endl;
    std::cout<<"\nfQwQPDCalibration[1]  = "<<fQwQPDCalibration[1]<<std::endl;
    std::cout<<"AlphaX = "<<fRelativeGains[0]<<std::endl;
    std::cout<<"AlphaY = "<<fRelativeGains[1]<<std::endl;    
  }
  return;
}

void QwQPD::ClearEventData()
{
  Short_t i=0;

  VQwBPM::ClearEventData();

  for(i=0;i<4;i++) fPhotodiode[i].ClearEventData();

  for(i=kXAxis;i<kNumAxes;i++) fRelPos[i].ClearEventData();

 return;
}


Bool_t QwQPD::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;

  UInt_t deviceerror=0;
  for(Short_t i=0;i<4;i++)
    {
      deviceerror|= fPhotodiode[i].ApplyHWChecks();  //OR the error code from each wire
      eventokay &= (deviceerror & 0x0);//AND with 0 since zero means HW is good.

      if (bDEBUG) std::cout<<" Inconsistent within QPD terminals photodiode[ "<<i<<" ] "<<std::endl;
      if (bDEBUG) std::cout<<" photodiode[ "<<i<<" ] sequence num "<<fPhotodiode[i].GetSequenceNumber()<<" sample size "<<fPhotodiode[i].GetNumberOfSamples()<<std::endl;
    }

  return eventokay;
}


Int_t QwQPD::GetEventcutErrorCounters()
{
  Short_t i=0;

  for(i=0;i<4;i++) 
    fPhotodiode[i].GetEventcutErrorCounters();

  VQwBPM::GetEventcutErrorCounters();

  return 1;
}


Bool_t QwQPD::ApplySingleEventCuts()
{
  Bool_t status=kTRUE;
  Int_t i=0;
  fErrorFlag=0;
  //Event cuts for four pads
  for(i=0;i<4;i++){
    if (fPhotodiode[i].ApplySingleEventCuts()){ //for RelX
      status&=kTRUE;
    }
    else{
      status&=kFALSE;
      if (bDEBUG) std::cout<<" single pad "<<fPhotodiode[i].GetElementName()<<" event cut failed ";
    }
    //Get the Event cut error flag for the pads
    fErrorFlag|=fPhotodiode[i].GetEventcutErrorFlag();
  }

  for(i=0;i<2;i++){
    status &= fRelPos[i].ApplySingleEventCuts();
    fErrorFlag|=fRelPos[i].GetEventcutErrorFlag();

    status &= fAbsPos[i].ApplySingleEventCuts();
    fErrorFlag|=fAbsPos[i].GetEventcutErrorFlag();
  }
  status &= fEffectiveCharge.ApplySingleEventCuts();
  fErrorFlag|=fEffectiveCharge.GetEventcutErrorFlag();
  
  return status;

}

VQwHardwareChannel* QwQPD::GetSubelementByName(TString ch_name)
{
  VQwHardwareChannel* tmpptr = NULL;
  ch_name.ToLower();
  if (ch_name=="tl"){
    tmpptr = &fPhotodiode[0];
  }else if (ch_name=="tr"){
    tmpptr = &fPhotodiode[1];
  }else if (ch_name=="br"){
    tmpptr = &fPhotodiode[2];
  }else if (ch_name=="bl"){
    tmpptr = &fPhotodiode[3];
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
    TString loc="QwQPD::GetSubelementByName for"
      + this->GetElementName() + " was passed "
      + ch_name + ", which is an unrecognized subelement name.";
    throw std::invalid_argument(loc.Data());
  }
  return tmpptr;
}


void QwQPD::SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX)
{

  if (ch_name=="tl"){
    QwMessage<<"TL LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[0].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="tr"){
    QwMessage<<"TR LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[1].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="br"){
    QwMessage<<"BR LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[2].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="bl"){
    QwMessage<<"BL LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[3].SetSingleEventCuts(minX,maxX);

  }else if (ch_name=="relx"){
    QwMessage<<"RelX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[0].SetSingleEventCuts(minX,maxX);
    
  }else if (ch_name=="rely"){
    QwMessage<<"RelY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[1].SetSingleEventCuts(minX,maxX);
    
  }else if (ch_name=="x" || ch_name=="y" || ch_name=="effectivecharge" ){

    VQwBPM::SetSingleEventCuts(ch_name, minX, maxX);
  }

}

void QwQPD::SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t minX, Double_t maxX, Double_t stability){
  errorflag|=kBPMErrorFlag;//update the device flag
  if (ch_name=="tl"){
    QwMessage<<"TL LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[0].SetSingleEventCuts(errorflag,minX,maxX,stability); 

  }else if (ch_name=="tr"){
    QwMessage<<"TR LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[1].SetSingleEventCuts(errorflag,minX,maxX,stability); 

  }else if (ch_name=="br"){
    QwMessage<<"BR LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[2].SetSingleEventCuts(errorflag,minX,maxX,stability); 

  }else if (ch_name=="bl"){
    QwMessage<<"BL LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fPhotodiode[3].SetSingleEventCuts(errorflag,minX,maxX,stability); 

  }else if (ch_name=="relx"){
    QwMessage<<"RelX LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[0].SetSingleEventCuts(errorflag,minX,maxX,stability); 

  }else if (ch_name=="rely"){
    QwMessage<<"RelY LL " <<  minX <<" UL " << maxX <<QwLog::endl;
    fRelPos[1].SetSingleEventCuts(errorflag,minX,maxX,stability); 

  }else if (ch_name=="absx"){
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


void  QwQPD::ProcessEvent()
{
  Bool_t localdebug = kFALSE;
  static QwVQWK_Channel numer[2];
  static QwVQWK_Channel tmp("tmp");
  static QwVQWK_Channel tmp1("tmp1");
  static QwVQWK_Channel tmp2("tmp2");

  numer[0].InitializeChannel("Xnumerator","raw");
  numer[1].InitializeChannel("Ynumerator","raw");

  Short_t i = 0;

  ApplyHWChecks();
  /* Frst apply HW checks and update HW  error flags. 
     Calling this routine here and not in ApplySingleEventCuts  
     makes a difference for a QPDs because they are derrived devices.
  */

  fEffectiveCharge.ClearEventData();

  for(i=0;i<4;i++){
    fPhotodiode[i].ProcessEvent();
    fEffectiveCharge+=fPhotodiode[i];
  }
  
  if (localdebug) fEffectiveCharge.PrintInfo();
   

  /** The positions X and Y from a QPD are calculated using following equations,

 TL   ---------  TR
     | 4  |  2 |
      ---------
 BL  | 3  |  1 | BR
      ---------


          (4-2) + (3-1)                (4-3) + (2-1)
      X =  ------------           Y =  -------------
             1+2+3+4                      1+2+3+4
  **/

  if(localdebug){
    std::cout<<"#############################\n";
    std::cout<<" QPD name = "<<fElementName<<std::endl;
    std::cout<<" event number = "<<fPhotodiode[0].GetSequenceNumber()<<"\n";
    std::cout<<" hw  BR ="<<fPhotodiode[0].GetValue()<<"\n";
    std::cout<<" hw  TR ="<<fPhotodiode[1].GetValue()<<"\n";
    std::cout<<" hw  BL ="<<fPhotodiode[2].GetValue()<<"\n";
    std::cout<<" hw  TL ="<<fPhotodiode[3].GetValue()<<"\n\n";
  }

  // X numerator
  tmp1.ClearEventData();
  tmp1.Difference(fPhotodiode[3],fPhotodiode[1]);  // 4-2
  tmp2.ClearEventData();
  tmp2.Difference(fPhotodiode[2],fPhotodiode[0]);  // 3-1
  tmp.ClearEventData();
  numer[0].Sum(tmp1,tmp2);

  // Y numerator
  tmp1.ClearEventData();
  tmp1.Difference(fPhotodiode[3],fPhotodiode[2]);  // 4-3
  tmp2.ClearEventData();
  tmp2.Difference(fPhotodiode[1],fPhotodiode[0]);  // 2-1
  tmp.ClearEventData();
  numer[1].Sum(tmp1,tmp2);

  for(i=kXAxis;i<kNumAxes;i++){
    tmp.ClearEventData();
    tmp.Sum(fPhotodiode[0],fPhotodiode[1]);
    tmp1.ClearEventData();
    tmp1.Sum(fPhotodiode[2],fPhotodiode[3]);
    fEffectiveCharge.Sum(tmp,tmp1);

    // X/Y reading in ADC counts
    fRelPos[i].Ratio(numer[i],fEffectiveCharge);

    // X/Y reading in mm.
    fAbsPos[i]=fRelPos[i];
    fAbsPos[i].Scale(fQwQPDCalibration[i]);

    if(localdebug){
      std::cout<<" hw  numerator= "<<numer[i].GetValue()<<"  ";
      std::cout<<" hw  denominator (== Effective_Charge)= "<<fEffectiveCharge.GetValue()<<"\n";
      std::cout<<" hw  clibration factors= "<<fQwQPDCalibration[i]<<"\n";
      std::cout<<" hw  fRelPos["<<kAxisLabel[i]<<"]="<<fRelPos[i].GetValue()<<"\n \n";
      std::cout<<" hw  fAbsPos["<<kAxisLabel[i]<<"]="<<fAbsPos[i].GetValue()<<"\n \n";
    }
  }
  
  return;
}


Int_t QwQPD::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t index)
{
  if(index<4)
    {
      fPhotodiode[index].ProcessEvBuffer(buffer,word_position_in_buffer);
    }
  else
    {
    std::cerr <<
      "QwQPD::ProcessEvBuffer(): attemp to fill in raw date for a wire that doesn't exist \n";
    }
  return word_position_in_buffer;
}


TString QwQPD::GetSubElementName(Int_t subindex)
{
  TString thisname;
  if(subindex<4&&subindex>-1)
    thisname=fPhotodiode[subindex].GetElementName();
  else
    std::cerr<<"QwQPD::GetSubElementName for "<<
      GetElementName()<<" this subindex doesn't exists \n";

  return thisname;
}

UInt_t QwQPD::GetSubElementIndex(TString subname)
{
  subname.ToUpper();
  UInt_t localindex=999999;
  for(Short_t i=0;i<4;i++) if(subname==subelement[i])localindex=i;

  if(localindex>3)
    std::cerr << "QwQPD::GetSubElementIndex is unable to associate the string -"
	      <<subname<<"- to any index"<<std::endl;

  return localindex;
}

VQwBPM& QwQPD::operator= (const VQwBPM &value)
{
  *(dynamic_cast<QwQPD*>(this)) = *(dynamic_cast<const QwQPD*>(&value));
  return *this;
}

QwQPD& QwQPD::operator= (const QwQPD &value)
{
  VQwBPM::operator=(value);
  if (GetElementName()!=""){
    Short_t i = 0;
    // this->fEffectiveCharge=value.fEffectiveCharge;
    for(i=0;i<4;i++) this->fPhotodiode[i]=value.fPhotodiode[i];
    for(i=kXAxis;i<kNumAxes;i++) this->fRelPos[i]=value.fRelPos[i];
    
  }
  return *this;
}

VQwBPM& QwQPD::operator+= (const VQwBPM &value)
{
  *(dynamic_cast<QwQPD*>(this)) += *(dynamic_cast<const QwQPD*>(&value));
  return *this;
}

QwQPD& QwQPD::operator+= (const QwQPD &value)
{
//  VQwBPM::operator+= (value);
  if (GetElementName()!=""){
    Short_t i = 0;
    for(i=0;i<4;i++) this->fPhotodiode[i]+=value.fPhotodiode[i];
    for(i=kXAxis;i<kNumAxes;i++) this->fRelPos[i]+=value.fRelPos[i];

  }
  return *this;
}

VQwBPM& QwQPD::operator-= (const VQwBPM &value)
{
  *(dynamic_cast<QwQPD*>(this)) -= *(dynamic_cast<const QwQPD*>(&value));
  return *this;
}

QwQPD& QwQPD::operator-= (const QwQPD &value)
{
//  VQwBPM::operator-= (value);
  if (GetElementName()!=""){
    Short_t i = 0;
    for(i=0;i<4;i++) this->fPhotodiode[i]-=value.fPhotodiode[i];
    for(i=kXAxis;i<kNumAxes;i++) this->fRelPos[i]-=value.fRelPos[i];

  }
  return *this;
}


void QwQPD::Ratio(QwQPD &numer, QwQPD &denom)
{
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // QPD is the difference only not the asymmetries

  *this=numer;
  this->fEffectiveCharge.Ratio(numer.fEffectiveCharge,denom.fEffectiveCharge);
  return;
}



void QwQPD::Scale(Double_t factor)
{
  Short_t i = 0;

  for(i=0;i<4;i++) fPhotodiode[i].Scale(factor);
  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].Scale(factor);
    fAbsPos[i].Scale(factor);
  }
  fEffectiveCharge.Scale(factor);
  return;
}

void QwQPD::Copy(QwQPD *source)
{
  Short_t i = 0;

  VQwBPM::Copy(source);
  for(i=0;i<4;i++) fPhotodiode[i].Copy(&(source->fPhotodiode[i]));
  for(i=kXAxis;i<kNumAxes;i++){
    fRelPos[i].Copy(&(source->fRelPos[i]));
    fAbsPos[i].Copy(&(source->fAbsPos[i]));
  }
  fEffectiveCharge.Copy(&(source->fEffectiveCharge));

  return;
}

void QwQPD::CalculateRunningAverage()
{
  Short_t i = 0;
  for (i = 0; i < 2; i++){
    fRelPos[i].CalculateRunningAverage();
    fAbsPos[i].CalculateRunningAverage();
  }
  fEffectiveCharge.CalculateRunningAverage();
  return;
}

void QwQPD::AccumulateRunningSum(const QwQPD& value)
{
  // TODO This is unsafe, see QwBeamline::AccumulateRunningSum
  Short_t i = 0;
  for (i = 0; i < 2; i++){
    fRelPos[i].AccumulateRunningSum(value.fRelPos[i]);
    fAbsPos[i].AccumulateRunningSum(value.fAbsPos[i]);
  }
  fEffectiveCharge.AccumulateRunningSum(value.fEffectiveCharge);
  return;
}


void  QwQPD::ConstructHistograms(TDirectory *folder, TString &prefix)
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
      for(i=0;i<4;i++) fPhotodiode[i].ConstructHistograms(folder, thisprefix);
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fAbsPos[i].ConstructHistograms(folder, thisprefix);
      fRelPos[i].ConstructHistograms(folder, thisprefix);
    }
  }
  return;
}

void  QwQPD::FillHistograms()
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the histograms.
  }
  else {
    fEffectiveCharge.FillHistograms();
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fPhotodiode[i].FillHistograms();
    }
    for(i=kXAxis;i<kNumAxes;i++){
      fAbsPos[i].FillHistograms();
      fRelPos[i].FillHistograms();
    }
  }
  return;
}

void  QwQPD::DeleteHistograms()
{
  if (GetElementName()=="") {
  }
  else {
    fEffectiveCharge.DeleteHistograms();
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fPhotodiode[i].DeleteHistograms();
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fAbsPos[i].DeleteHistograms();
      fRelPos[i].DeleteHistograms();
    }
  }
  return;
}


void  QwQPD::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
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
      for(i=0;i<4;i++) fPhotodiode[i].ConstructBranchAndVector(tree,thisprefix,values);
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fAbsPos[i].ConstructBranchAndVector(tree,thisprefix,values);
      fRelPos[i].ConstructBranchAndVector(tree,thisprefix,values);
    }

  }
  return;
}

void  QwQPD::ConstructBranch(TTree *tree, TString &prefix)
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
      for(i=0;i<4;i++) fPhotodiode[i].ConstructBranch(tree,thisprefix);
    }
    for(i=kXAxis;i<kNumAxes;i++) {
      fAbsPos[i].ConstructBranch(tree,thisprefix);
      fRelPos[i].ConstructBranch(tree,thisprefix);
    }

  }
  return;
}

void  QwQPD::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;

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
	  for(i=0;i<4;i++) fPhotodiode[i].ConstructBranch(tree,thisprefix);
	}
	for(i=kXAxis;i<kNumAxes;i++) {
	  fAbsPos[i].ConstructBranch(tree,thisprefix);
	  fRelPos[i].ConstructBranch(tree,thisprefix);
	}

	QwMessage <<" Tree leaves added to "<<devicename<<" Corresponding channels"<<QwLog::endl;
      }
      // this functions doesn't do anything yet
    }





  return;
}

void  QwQPD::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()=="") {
    //  This channel is not used, so skip filling the tree.
  }
  else {
    fEffectiveCharge.FillTreeVector(values);
    Short_t i = 0;
    if(bFullSave) {
      for(i=0;i<4;i++) fPhotodiode[i].FillTreeVector(values);
    }

    for(i=kXAxis;i<kNumAxes;i++){
      fAbsPos[i].FillTreeVector(values);
      fRelPos[i].FillTreeVector(values);
    }
  }
  return;
}



void QwQPD::SetEventCutMode(Int_t bcuts)
{
  Short_t i = 0;
  // bEVENTCUTMODE=bcuts;
  for (i=0;i<4;i++) fPhotodiode[i].SetEventCutMode(bcuts);
  for (i=kXAxis;i<kNumAxes;i++) {
    fAbsPos[i].SetEventCutMode(bcuts);
    fRelPos[i].SetEventCutMode(bcuts);
  }
  fEffectiveCharge.SetEventCutMode(bcuts);
}


void QwQPD::MakeQPDList()
{
  UShort_t i = 0;

  QwVQWK_Channel qpd_sub_element;

  for(i=kXAxis;i<kNumAxes;i++) {
    qpd_sub_element.ClearEventData();
    qpd_sub_element.Copy(&fAbsPos[i]);
    qpd_sub_element = fAbsPos[i];
    fQPDElementList.push_back( qpd_sub_element );
  }
  qpd_sub_element.Copy(&fEffectiveCharge);
  qpd_sub_element = fEffectiveCharge;
  fQPDElementList.push_back(qpd_sub_element );
  return;
}


std::vector<QwDBInterface> QwQPD::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();
  for(size_t i=kXAxis;i<kNumAxes;i++) {
    fAbsPos[i].AddEntriesToList(row_list);
  }
  fEffectiveCharge.AddEntriesToList(row_list);
  return row_list;

  //   UShort_t i = 0;
  //   UShort_t n_qpd_element = 0;
  
  //   std::vector <QwDBInterface> row_list;
  //   row_list.clear();
  
  //   QwDBInterface row;
  
  //   TString name;
  //   Double_t avg         = 0.0;
  //   Double_t err         = 0.0;
  //   UInt_t beam_subblock = 0;
  //   UInt_t beam_n        = 0;
  
  //   for(n_qpd_element=0; n_qpd_element<fQPDElementList.size(); n_qpd_element++) {
  
  //     row.Reset();
  //     // the element name and the n (number of measurements in average)
  //     // is the same in each block and hardwaresum.
  
  //     name          = fQPDElementList.at(n_qpd_element).GetElementName();
  //     beam_n        = fQPDElementList.at(n_qpd_element).GetGoodEventCount();
  
  //     // Get HardwareSum average and its error
  //     avg           = fQPDElementList.at(n_qpd_element).GetHardwareSum();
  //     err           = fQPDElementList.at(n_qpd_element).GetHardwareSumError();
  //     // ADC subblock sum : 0 in MySQL database
  //     beam_subblock = 0;
  
  //     row.SetDetectorName(name);
  //     row.SetSubblock(beam_subblock);
  //     row.SetN(beam_n);
  //     row.SetValue(avg);
  //     row.SetError(err);
  
  //     row_list.push_back(row);
  
  //     // Get four Block averages and thier errors
  
  //     for(i=0; i<4; i++) {
  //       row.Reset();
  //       avg           = fQPDElementList.at(n_qpd_element).GetBlockValue(i);
  //       err           = fQPDElementList.at(n_qpd_element).GetBlockErrorValue(i);
  //       beam_subblock = (UInt_t) (i+1);
  //       // QwVQWK_Channel  | MySQL
  //       // fBlock[0]       | subblock 1
  //       // fBlock[1]       | subblock 2
  //       // fBlock[2]       | subblock 3
  //       // fBlock[3]       | subblock 4
  //       row.SetDetectorName(name);
  //       row.SetSubblock(beam_subblock);
  //       row.SetN(beam_n);
  //       row.SetValue(avg);
  //       row.SetError(err);
  
  //       row_list.push_back(row);
  //     }
  //   }

  return row_list;

}

/**********************************
 * Mock data generation routines
 **********************************/

void  QwQPD::SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY)
{
  // Average values of the signals in the stripline ADCs
  Double_t sumX = 1.1e8; // These are just guesses, but I made X and Y different
  Double_t sumY = 0.9e8; // to make it more interesting for the analyzer...

  
  // Determine the asymmetry from the position
  Double_t meanXP = (1.0 + meanX / fQwQPDCalibration[0]) * sumX / 2.0;
  Double_t meanXM = (1.0 - meanX / fQwQPDCalibration[0]) * sumX / 2.0; // = sumX - meanXP;
  Double_t meanYP = (1.0 + meanY / fQwQPDCalibration[1]) * sumY / 2.0;
  Double_t meanYM = (1.0 - meanY / fQwQPDCalibration[1]) * sumY / 2.0; // = sumY - meanYP;

  // Determine the spread of the asymmetry (this is not tested yet)
  // (negative sigma should work in the QwVQWK_Channel, but still using fabs)
  Double_t sigmaXP = fabs(sumX * sigmaX / meanX);
  Double_t sigmaXM = sigmaXP;
  Double_t sigmaYP = fabs(sumY * sigmaY / meanY);
  Double_t sigmaYM = sigmaYP;

  // Propagate these parameters to the ADCs
  fPhotodiode[0].SetRandomEventParameters(meanXP, sigmaXP);
  fPhotodiode[1].SetRandomEventParameters(meanXM, sigmaXM);
  fPhotodiode[2].SetRandomEventParameters(meanYP, sigmaYP);
  fPhotodiode[3].SetRandomEventParameters(meanYM, sigmaYM);
}


void QwQPD::RandomizeEventData(int helicity, double time)
{
  for (Short_t i=0; i<4; i++) fPhotodiode[i].RandomizeEventData(helicity, time);

  return;
}


void QwQPD::SetEventData(Double_t* relpos, UInt_t sequencenumber)
{
  for (Short_t i=0; i<2; i++)
    {
      fAbsPos[i].SetHardwareSum(relpos[i], sequencenumber);
    }

  return;
}


void QwQPD::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (Short_t i=0; i<4; i++) fPhotodiode[i].EncodeEventData(buffer);
}


void QwQPD::SetDefaultSampleSize(Int_t sample_size)
{
  for(Short_t i=0;i<4;i++) fPhotodiode[i].SetDefaultSampleSize((size_t)sample_size);
  return;
}


void QwQPD::SetSubElementPedestal(Int_t j, Double_t value)
{
  fPhotodiode[j].SetPedestal(value);
  return;
}

void QwQPD::SetSubElementCalibrationFactor(Int_t j, Double_t value)
{
  fPhotodiode[j].SetCalibrationFactor(value);
  return;
}

