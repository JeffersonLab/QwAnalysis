/**********************************************************\
* File: QwEnergyCalculator.cc                             *
*                                                         *
* Author: B.Waidyawansa                                   *
* Time-stamp: 05-24-2010                                  *
\**********************************************************/

#include "QwEnergyCalculator.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

static QwVQWK_Channel  targetbeamangle;
static QwVQWK_Channel  targetbeamx;
static QwVQWK_Channel  beamx;

void QwEnergyCalculator::InitializeChannel(TString name,TString datatosave )
{
  SetElementName(name);
  fEnergyChange.InitializeChannel(name,datatosave);
  beamx.InitializeChannel("beamx","derived");
  return;
};

void QwEnergyCalculator::InitializeChannel(TString subsystem, TString name,TString datatosave )
{
  SetElementName(name);
  fEnergyChange.InitializeChannel(subsystem, "QwEnergyCalculator", name,datatosave);
  beamx.InitializeChannel("beamx","derived");
  return;
};

void QwEnergyCalculator::Set(VQwBPM* device, TString type, TString property,Double_t tmatrix_ratio)
{
  Bool_t ldebug = kFALSE;

  fDevice.push_back(device);
  fProperty.push_back(property);
  fType.push_back(type);
  fTMatrixRatio.push_back(tmatrix_ratio);

  if(ldebug)
    std::cout<<"QwEnergyCalculator:: Using "<<device->GetElementName()<<" with ratio "<< tmatrix_ratio <<" for "<<property<<std::endl;
 
  return;
};


void QwEnergyCalculator::ClearEventData(){
  fEnergyChange.ClearEventData();
  return;
}



void  QwEnergyCalculator::ProcessEvent(){

  //  Bool_t ldebug = kFALSE;
  Double_t targetbeamangle = 0;

  static QwVQWK_Channel tmp;
  tmp.InitializeChannel("tmp","derived");
  tmp.ClearEventData();

  for(UInt_t i = 0; i<fProperty.size(); i++){
    if(fProperty[i].Contains("targetbeamangle")){
      targetbeamangle = atan((((QwCombinedBPM*)fDevice[i])->fSlope[0]).GetHardwareSum());
      targetbeamangle *= fTMatrixRatio[i];
      fEnergyChange.AddChannelOffset(targetbeamangle);
    }
    else {
      if(fType[i].Contains("bpmstripline"))
	tmp = ((QwBPMStripline*)fDevice[i])->fAbsPos[0];
      if(fType[i].Contains("combinedbpm"))
	tmp = ((QwCombinedBPM*)fDevice[i])->fAbsPos[0];
      tmp.Scale(fTMatrixRatio[i]);
      fEnergyChange += tmp;
    }
  }
  return;
};


Bool_t QwEnergyCalculator::ApplySingleEventCuts(){
  Bool_t status=kTRUE;

  if (fEnergyChange.ApplySingleEventCuts()){
    status=kTRUE;
  }
  else{
    fEnergyChange.UpdateEventCutErrorCount();//update event cut falied counts
    status&=kFALSE;
  }
  fDeviceErrorCode|=fEnergyChange.GetEventcutErrorFlag();//retrun the error flag for event cuts
  //Update the error counters
  fEnergyChange.UpdateHWErrorCounters();

  return status;

};


Int_t QwEnergyCalculator::GetEventcutErrorCounters(){
  // report number of events falied due to HW and event cut faliure
  fEnergyChange.GetEventcutErrorCounters();

  return 1;
}


void QwEnergyCalculator::CalculateRunningAverage(){
  fEnergyChange.CalculateRunningAverage();
};



void QwEnergyCalculator::AccumulateRunningSum(const QwEnergyCalculator& value){
  fEnergyChange.AccumulateRunningSum(value.fEnergyChange);
};



Int_t QwEnergyCalculator::ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer,UInt_t subelement){
  return 0;
};


QwEnergyCalculator& QwEnergyCalculator::operator= (const QwEnergyCalculator &value){
  if (GetElementName()!="")
    this->fEnergyChange=value.fEnergyChange;

  return *this;
};

QwEnergyCalculator& QwEnergyCalculator::operator+= (const QwEnergyCalculator &value){
  if (GetElementName()!="")
    this->fEnergyChange+=value.fEnergyChange;
  return *this;
};

QwEnergyCalculator& QwEnergyCalculator::operator-= (const QwEnergyCalculator &value){
  if (GetElementName()!="")
    this->fEnergyChange-=value.fEnergyChange;

  return *this;
};


void QwEnergyCalculator::Ratio(QwEnergyCalculator &numer, QwEnergyCalculator &denom){
  if (GetElementName()!="")
    this->fEnergyChange.Ratio(numer.fEnergyChange,denom.fEnergyChange);

  return;
};


void QwEnergyCalculator::Scale(Double_t factor){
  fEnergyChange.Scale(factor);
  return;
};


void QwEnergyCalculator::PrintInfo() const{
  fEnergyChange.PrintInfo();
  return;
};


void QwEnergyCalculator::PrintValue() const{
  fEnergyChange.PrintValue();
  return;
};

Bool_t QwEnergyCalculator::ApplyHWChecks(){
  // For the energy calculator there are no physical channels that we can relate to because it is being
  // derived from combinations of physical channels. Therefore, this is not exactly a "HW Check"
  // but just a check of the HW checks of the used channels.

  Bool_t fEventIsGood=kTRUE;
  return fEventIsGood;
};


Int_t QwEnergyCalculator::SetSingleEventCuts(Double_t minX, Double_t maxX){
  fEnergyChange.SetSingleEventCuts(minX,maxX);
  return 1;
};


void  QwEnergyCalculator::ConstructHistograms(TDirectory *folder, TString &prefix){
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else
    fEnergyChange.ConstructHistograms(folder, prefix);

  return;
};

void  QwEnergyCalculator::FillHistograms(){
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else
    fEnergyChange.FillHistograms();
  return;
};

void  QwEnergyCalculator::DeleteHistograms(){
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else
    fEnergyChange.DeleteHistograms();
  return;
};


void  QwEnergyCalculator::ConstructBranchAndVector(TTree *tree, TString &prefix,
						   std::vector<Double_t> &values){
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else
    fEnergyChange.ConstructBranchAndVector(tree,prefix,values);
  return;
};



void  QwEnergyCalculator::ConstructBranch(TTree *tree, TString &prefix){
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else
    fEnergyChange.ConstructBranch(tree,prefix);
  return;
};

void  QwEnergyCalculator::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist){

  TString devicename;
  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else
    if (modulelist.HasValue(devicename)){
      fEnergyChange.ConstructBranch(tree,prefix);
      QwMessage <<" Tree leave added to "<<devicename<<QwLog::endl;
      }
  return;
};

void  QwEnergyCalculator::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else
    fEnergyChange.FillTreeVector(values);
  return;
};


void  QwEnergyCalculator::Copy(VQwDataElement *source){
  try{
    if(typeid(*source)==typeid(*this)){
      QwEnergyCalculator* input=((QwEnergyCalculator*)source);
      this->fElementName=input->fElementName;
      this->fEnergyChange.Copy(&(input->fEnergyChange));
    }
    else {
      TString loc="Standard exception from QwEnergyCalculator::Copy = "
	+source->GetElementName()+" "
	+this->GetElementName()+" are not of the same type";
      throw std::invalid_argument(loc.Data());
    }
  }
  catch (std::exception& e){
    std::cerr << e.what() << std::endl;
  }

  return;
};



std::vector<QwDBInterface> QwEnergyCalculator::GetDBEntry()
{
  UShort_t i = 0;

  std::vector <QwDBInterface> row_list;
  QwDBInterface row;

  TString name;
  Double_t avg         = 0.0;
  Double_t err         = 0.0;
  UInt_t beam_subblock = 0;
  UInt_t beam_n        = 0;

  row.Reset();

  // the element name and the n (number of measurements in average)
  // is the same in each block and hardwaresum.

  name          = fEnergyChange.GetElementName();
  beam_n        = fEnergyChange.GetGoodEventCount();

  // Get HardwareSum average and its error
  avg           = fEnergyChange.GetHardwareSum();
  err           = fEnergyChange.GetHardwareSumError();
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
    avg           = fEnergyChange.GetBlockValue(i);
    err           = fEnergyChange.GetBlockErrorValue(i);
    beam_subblock = (UInt_t) (i+1);
    // QwVQWK_Channel  | MySQL
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

  return row_list;

};


