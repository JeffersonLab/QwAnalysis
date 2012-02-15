/**********************************************************\
* File: QwHaloMonitor.cc                                  *
*                                                         *
* Author:B. Waidyawansa                                   *
* Time-stamp:24-june-2010                                 *
\**********************************************************/

#include "QwHaloMonitor.h"

// System headers
#include <stdexcept>

// Qweak headers
#include "QwDBInterface.h"

void  QwHaloMonitor::InitializeChannel(TString subsystem, TString name){
  fHalo_Counter.InitializeChannel(name);
  SetElementName(name);
}

void  QwHaloMonitor::InitializeChannel(TString name){
  fHalo_Counter.InitializeChannel(name);
  SetElementName(name);
}

void QwHaloMonitor::ClearEventData()
{
  fHalo_Counter.ClearEventData();
}

void QwHaloMonitor::ProcessEvent()
{
  // no processing required for the halos as they are just counters(?).
  fHalo_Counter.ProcessEvent();
}

Int_t QwHaloMonitor::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left,UInt_t index)
{
  return fHalo_Counter.ProcessEvBuffer(buffer,num_words_left);
}


Bool_t QwHaloMonitor::ApplyHWChecks()
{
  Bool_t eventokay=kTRUE;
  return eventokay;
}


Bool_t QwHaloMonitor::ApplySingleEventCuts()
{
  return fHalo_Counter.ApplySingleEventCuts();
}


Int_t QwHaloMonitor::GetEventcutErrorCounters()
{// report number of events failed due to HW and event cut faliure
  return 1;
}

void QwHaloMonitor::UpdateEventcutErrorFlag(QwHaloMonitor *ev_error){
  
}

QwHaloMonitor& QwHaloMonitor::operator= (const QwHaloMonitor &value)
{
  if (GetElementName()!=""){
    this->fHalo_Counter=value.fHalo_Counter;
  }
  return *this;
}

QwHaloMonitor& QwHaloMonitor::operator+= (const QwHaloMonitor &value)
{
  if (GetElementName()!=""){
    this->fHalo_Counter+=value.fHalo_Counter;
  }
  return *this;
}

QwHaloMonitor& QwHaloMonitor::operator-= (const QwHaloMonitor &value)
{
  if (GetElementName()!=""){
    this->fHalo_Counter-=value.fHalo_Counter;
  }
  return *this;
}


void QwHaloMonitor::Sum(QwHaloMonitor &value1, QwHaloMonitor &value2){
  *this =  value1;
  *this += value2;
}

void QwHaloMonitor::Difference(QwHaloMonitor &value1, QwHaloMonitor &value2){
  *this =  value1;
  *this -= value2;
}

void QwHaloMonitor::Ratio(QwHaloMonitor &numer, QwHaloMonitor &denom)
{
  if (GetElementName()!=""){
      this->fHalo_Counter.Ratio(numer.fHalo_Counter,denom.fHalo_Counter);
  }
  return;
}

void QwHaloMonitor::Scale(Double_t factor)
{
  fHalo_Counter.Scale(factor);
}

void QwHaloMonitor::AccumulateRunningSum(const QwHaloMonitor& value) {
  fHalo_Counter.AccumulateRunningSum(value.fHalo_Counter);
}

void QwHaloMonitor::DeaccumulateRunningSum(QwHaloMonitor& value) {
  //fHalo_Counter.DeccumulateRunningSum(value.fHalo_Counter);
}

void QwHaloMonitor::CalculateRunningAverage(){
  fHalo_Counter.CalculateRunningAverage();
}


void QwHaloMonitor::PrintValue() const
{
  fHalo_Counter.PrintValue();
}

void QwHaloMonitor::PrintInfo() const
{
  std::cout << "QwVQWK_Channel Info " << std::endl;
  fHalo_Counter.PrintInfo();
}

void  QwHaloMonitor::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
      fHalo_Counter.ConstructHistograms(folder, prefix);
  }
}

void  QwHaloMonitor::FillHistograms()
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fHalo_Counter.FillHistograms();
  }
}

void  QwHaloMonitor::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fHalo_Counter.ConstructBranchAndVector(tree, prefix,values);
    // this functions doesn't do anything yet
  }
}

void  QwHaloMonitor::ConstructBranch(TTree *tree, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fHalo_Counter.ConstructBranch(tree, prefix);
    // this functions doesn't do anything yet
  }
}



void  QwHaloMonitor::ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist)
{
  TString devicename;

  devicename=GetElementName();
  devicename.ToLower();
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else {

    //QwMessage <<" QwHaloMonitor "<<devicename<<QwLog::endl;
    if (modulelist.HasValue(devicename)){
      fHalo_Counter.ConstructBranch(tree, prefix);
      QwMessage <<" Tree leaf added to "<<devicename<<QwLog::endl;
    }
    // this functions doesn't do anything yet
  }
}




void  QwHaloMonitor::FillTreeVector(std::vector<Double_t> &values) const
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fHalo_Counter.FillTreeVector(values);
    // this functions doesn't do anything yet
  }
}


std::vector<QwDBInterface> QwHaloMonitor::GetDBEntry()
{
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

  name          = fHalo_Counter.GetElementName();
  beam_n        = fHalo_Counter.GetGoodEventCount();

  // Get HardwareSum average and its error
  avg           = fHalo_Counter.GetValue();
  err           = fHalo_Counter.GetValueError();
  // ADC subblock sum : 0 in MySQL database
  beam_subblock = 0;

  row.SetDetectorName(name);
  row.SetSubblock(beam_subblock);
  row.SetN(beam_n);
  row.SetValue(avg);
  row.SetError(err);

  row_list.push_back(row);

  /*

  // Get four Block averages and thier errors

  for (int i=0; i<4; i++) {
    row.Reset();
    avg           = fHalo_Counter.GetBlockValue(i);
    err           = fHalo_Counter.GetBlockErrorValue(i);
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
  */

  return row_list;

}
