/**********************************************************\
* File: QwEnergyCalculator.cc                             *
*                                                         *
* Author: B.Waidyawansa                                   *
* Time-stamp: 05-24-2010                                  *
\**********************************************************/

#include "QwEnergyCalculator.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwDBInterface.h"


// static QwVQWK_Channel  targetbeamangle;
// static QwVQWK_Channel  targetbeamx;
// static QwVQWK_Channel  beamx;

void QwEnergyCalculator::InitializeChannel(TString name,TString datatosave )
{
  SetElementName(name);
  fEnergyChange.InitializeChannel(name,datatosave);
  //  beamx.InitializeChannel("beamx","derived");
  return;
}

void QwEnergyCalculator::InitializeChannel(TString subsystem, TString name,TString datatosave )
{
  SetElementName(name);
  fEnergyChange.InitializeChannel(subsystem, "QwEnergyCalculator", name,datatosave);
  //  beamx.InitializeChannel("beamx","derived");
  return;
}

void QwEnergyCalculator::Set(const VQwBPM* device, TString type, TString property,Double_t tmatrix_ratio)
{
  Bool_t ldebug = kFALSE;

  fDevice.push_back(device);
  fProperty.push_back(property);
  fType.push_back(type);
  fTMatrixRatio.push_back(tmatrix_ratio);

  if(ldebug)
    std::cout<<"QwEnergyCalculator:: Using "<<device->GetElementName()<<" with ratio "<< tmatrix_ratio <<" for "<<property<<std::endl;
 
  return;
}

void QwEnergyCalculator::SetRootSaveStatus(TString &prefix)
{
  if(prefix=="diff_"||prefix=="yield_"|| prefix=="asym_")
    bFullSave=kFALSE;
  
  return;
}

void QwEnergyCalculator::ClearEventData(){
  fEnergyChange.ClearEventData();
  return;
}



void  QwEnergyCalculator::ProcessEvent(){

  Bool_t ldebug = kFALSE;
  Double_t targetbeamangle = 0;

  static QwVQWK_Channel tmp;
  tmp.InitializeChannel("tmp","derived");
  tmp.ClearEventData();

  for(UInt_t i = 0; i<fProperty.size(); i++){
    if(fProperty[i].Contains("targetbeamangle")){
      targetbeamangle = atan((((QwCombinedBPM<QwVQWK_Channel>*)fDevice[i])->fSlope[VQwBPM::kXAxis]).GetValue());
      targetbeamangle *= fTMatrixRatio[i];
      if(ldebug) std::cout<<"QwEnegyCalculator::ProcessEvent() :: Beam angle in X at target = "<<targetbeamangle<<std::endl;
      fEnergyChange.AddChannelOffset(targetbeamangle);
      if(ldebug) std::cout<<"QwEnegyCalculator::ProcessEvent() :: dp/p += (M12/M16)*X angle = "<<fEnergyChange.GetValue()<<std::endl;
    }
    else {
      tmp.AssignValueFrom(fDevice[i]->GetPosition(VQwBPM::kXAxis));
      if(ldebug) std::cout<<"QwEnegyCalculator::ProcessEvent() :: X position from "<<fDevice[i]->GetElementName()<<" = "<<tmp.GetValue()<<std::endl;
      tmp.Scale(fTMatrixRatio[i]);
      if(ldebug) std::cout<<"QwEnegyCalculator::ProcessEvent() :: (M11/M16)*X position = "<<tmp.GetValue()<<std::endl;
      fEnergyChange += tmp;
    }
  }
  if(ldebug) std::cout<<"QwEnegyCalculator::ProcessEvent() :: dp/p = "<<fEnergyChange.GetValue()<<std::endl;
  return;
}


Bool_t QwEnergyCalculator::ApplySingleEventCuts(){
  Bool_t status=kTRUE;

  UInt_t error_code = 0;
  for(UInt_t i = 0; i<fProperty.size(); i++){
    if(fProperty[i].Contains("targetbeamangle")){
      error_code |= ((QwCombinedBPM<QwVQWK_Channel>*)fDevice[i])->fSlope[0].GetErrorCode();
    } else {
      error_code |= fDevice[i]->GetPosition(VQwBPM::kXAxis)->GetErrorCode();
    }
  }
  //fEnergyChange.UpdateErrorFlag(error_code);//No need to do this. error codes are ORed when energy is calculated

  if (fEnergyChange.ApplySingleEventCuts()){
    status=kTRUE;
  }
  else{
    status&=kFALSE;
  }
  return status;
}

void QwEnergyCalculator::IncrementErrorCounters()
{
  fEnergyChange.IncrementErrorCounters();
}


void QwEnergyCalculator::PrintErrorCounters() const{
  // report number of events failed due to HW and event cut faliure
  fEnergyChange.PrintErrorCounters();
}

UInt_t QwEnergyCalculator::UpdateErrorFlag()
{
  UInt_t error_code = 0;
  for(UInt_t i = 0; i<fProperty.size(); i++){
    if(fProperty[i].Contains("targetbeamangle")){
      error_code |= ((QwCombinedBPM<QwVQWK_Channel>*)fDevice[i])->fSlope[0].GetErrorCode();
    } else {
      error_code |= fDevice[i]->GetPosition(VQwBPM::kXAxis)->GetErrorCode();
    }
  }
  fEnergyChange.UpdateErrorFlag(error_code);
  return fEnergyChange.GetEventcutErrorFlag();
}


void QwEnergyCalculator::UpdateErrorFlag(const QwEnergyCalculator *ev_error){
  fEnergyChange.UpdateErrorFlag(ev_error->fEnergyChange);
};


void QwEnergyCalculator::CalculateRunningAverage(){
  fEnergyChange.CalculateRunningAverage();
}



void QwEnergyCalculator::AccumulateRunningSum(const QwEnergyCalculator& value){
  fEnergyChange.AccumulateRunningSum(value.fEnergyChange);
}

void QwEnergyCalculator::DeaccumulateRunningSum(QwEnergyCalculator& value){
  fEnergyChange.DeaccumulateRunningSum(value.fEnergyChange);
}



Int_t QwEnergyCalculator::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t subelement){
  return 0;
}


QwEnergyCalculator& QwEnergyCalculator::operator= (const QwEnergyCalculator &value){
  if (GetElementName()!="")
    this->fEnergyChange=value.fEnergyChange;

  return *this;
}

QwEnergyCalculator& QwEnergyCalculator::operator+= (const QwEnergyCalculator &value){

  if (GetElementName()!="")
    this->fEnergyChange+=value.fEnergyChange;

  return *this;
}

QwEnergyCalculator& QwEnergyCalculator::operator-= (const QwEnergyCalculator &value){
  if (GetElementName()!="")
    this->fEnergyChange-=value.fEnergyChange;

  return *this;
}


void QwEnergyCalculator::Ratio(QwEnergyCalculator &numer, QwEnergyCalculator &denom){
  // this function is called when forming asymmetries. In this case waht we actually want for the
  // qwk_energy/(dp/p) is the difference only not the asymmetries

  *this=numer;
  return;
}


void QwEnergyCalculator::Scale(Double_t factor){
  fEnergyChange.Scale(factor);
  return;
}


void QwEnergyCalculator::PrintInfo() const{
  fEnergyChange.PrintInfo();
  return;
}


void QwEnergyCalculator::PrintValue() const{
  fEnergyChange.PrintValue();
  return;
}

Bool_t QwEnergyCalculator::ApplyHWChecks(){
  // For the energy calculator there are no physical channels that we can relate to because it is being
  // derived from combinations of physical channels. Therefore, this is not exactly a "HW Check"
  // but just a check of the HW checks of the used channels.

  Bool_t eventokay=kTRUE;
  return eventokay;
}


Int_t QwEnergyCalculator::SetSingleEventCuts(Double_t minX, Double_t maxX){
  fEnergyChange.SetSingleEventCuts(minX,maxX);
  return 1;
}

void QwEnergyCalculator::SetSingleEventCuts(UInt_t errorflag, Double_t LL=0, Double_t UL=0, Double_t stability=0){
  //set the unique tag to identify device type (bcm,bpm & etc)
  errorflag|=kBCMErrorFlag;//currently I use the same flag for bcm
  QwMessage<<"QwEnergyCalculator Error Code passing to QwVQWK_Ch "<<errorflag<<QwLog::endl;
  fEnergyChange.SetSingleEventCuts(errorflag,LL,UL,stability);
}


void  QwEnergyCalculator::ConstructHistograms(TDirectory *folder, TString &prefix){
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";
    SetRootSaveStatus(thisprefix);
    fEnergyChange.ConstructHistograms(folder, thisprefix);
  }
  return;
}

void  QwEnergyCalculator::FillHistograms(){
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else
    fEnergyChange.FillHistograms();
  
  return;
}

void  QwEnergyCalculator::ConstructBranchAndVector(TTree *tree, TString &prefix,
						   std::vector<Double_t> &values){
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";
    
    SetRootSaveStatus(thisprefix);
    
    fEnergyChange.ConstructBranchAndVector(tree,thisprefix,values);
  }
    return;
}



void  QwEnergyCalculator::ConstructBranch(TTree *tree, TString &prefix){
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    TString thisprefix=prefix;
    if(prefix=="asym_")
      thisprefix="diff_";

    SetRootSaveStatus(thisprefix);
    fEnergyChange.ConstructBranch(tree,thisprefix);
  }
  return;
}

void  QwEnergyCalculator::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist){

  TString devicename;
  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else
    if (modulelist.HasValue(devicename)){
      TString thisprefix=prefix;
      if(prefix=="asym_")
	thisprefix="diff_";
      SetRootSaveStatus(thisprefix);   
      fEnergyChange.ConstructBranch(tree,thisprefix);
      QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
      }
  return;
}

void  QwEnergyCalculator::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else
    fEnergyChange.FillTreeVector(values);
  return;
}


std::vector<QwDBInterface> QwEnergyCalculator::GetDBEntry()
{
  std::vector <QwDBInterface> row_list;
  row_list.clear();
  fEnergyChange.AddEntriesToList(row_list);
  return row_list;

}

std::vector<QwErrDBInterface> QwEnergyCalculator::GetErrDBEntry()
{
  std::vector <QwErrDBInterface> row_list;
  row_list.clear();
  fEnergyChange.AddErrEntriesToList(row_list);
  return row_list;
}

