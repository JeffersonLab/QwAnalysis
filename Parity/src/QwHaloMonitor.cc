/**********************************************************\
* File: QwHaloMonitor.cc                                  *
*                                                         *
* Author:B. Waidyawansa                                   *
* Time-stamp:24-june-2010                                 *
\**********************************************************/

#include "QwHaloMonitor.h"
#include "QwHistogramHelper.h"
#include <stdexcept>


void QwHaloMonitor::ClearEventData()
{

  return;
};


void QwHaloMonitor::ProcessEvent()
{

  return;
};

Int_t QwHaloMonitor::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left,UInt_t index)
{
  return fHalo_Counter.ProcessEvBuffer(buffer,num_words_left,index);
};


Bool_t QwHaloMonitor::ApplyHWChecks()
{
  Bool_t fEventIsGood=kTRUE;

  return fEventIsGood ;
};


Bool_t QwHaloMonitor::ApplySingleEventCuts()
{
  return kTRUE ;
};


Int_t QwHaloMonitor::GetEventcutErrorCounters()
{// report number of events falied due to HW and event cut faliure

  return 1;
};

QwHaloMonitor& QwHaloMonitor::operator= (const QwHaloMonitor &value)
{
//   std::cout<<" Here in QwHaloMonitor::operator= \n";
  if (GetElementName()!=""){
    this->fHalo_Counter=value.fHalo_Counter;
  }
  return *this;
};

QwHaloMonitor& QwHaloMonitor::operator+= (const QwHaloMonitor &value)
{
  if (GetElementName()!=""){
    this->fHalo_Counter+=value.fHalo_Counter;
  }
  return *this;
};

QwHaloMonitor& QwHaloMonitor::operator-= (const QwHaloMonitor &value)
{
  if (GetElementName()!=""){
    this->fHalo_Counter-=value.fHalo_Counter;
  }
  return *this;
};


void QwHaloMonitor::Sum(QwHaloMonitor &value1, QwHaloMonitor &value2){
  *this =  value1;
  *this += value2;
};

void QwHaloMonitor::Difference(QwHaloMonitor &value1, QwHaloMonitor &value2){
  *this =  value1;
  *this -= value2;
};

void QwHaloMonitor::Ratio(QwHaloMonitor &numer, QwHaloMonitor &denom)
{
  //  std::cout<<"QwHaloMonitor::Ratio element name ="<<GetElementName()<<" \n";
  if (GetElementName()!=""){
      this->fHalo_Counter.Ratio(numer.fHalo_Counter,denom.fHalo_Counter);
  }
  return;
};

void QwHaloMonitor::Scale(Double_t factor)
{
  fHalo_Counter.Scale(factor);
  return;
};



// void QwHaloMonitor::AccumulateRunningSum(const QwHaloMonitor& value) {
//   fHalo_Counter.AccumulateRunningSum(value.fHalo_Counter);
// };


void QwHaloMonitor::PrintValue() const
{
  fHalo_Counter.PrintValue();
};

void QwHaloMonitor::PrintInfo() const
{
  std::cout << "QwVQWK_Channel Info " << std::endl;
  fHalo_Counter.PrintInfo();
};

void  QwHaloMonitor::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
      fHalo_Counter.ConstructHistograms(folder, prefix);
  }
  return;
};

void  QwHaloMonitor::FillHistograms()
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  }
  else{
    fHalo_Counter.FillHistograms();
  }
  
  
  return;
};

void  QwHaloMonitor::DeleteHistograms()
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } 
  else{
    fHalo_Counter.DeleteHistograms();
  }
  return;
};


void  QwHaloMonitor::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } 
  else{
    fHalo_Counter.ConstructBranchAndVector(tree, prefix,values);
    // this functions doesn't do anything yet
  }
  return;
};

void  QwHaloMonitor::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } 
  else{
    fHalo_Counter.FillTreeVector(values);
    // this functions doesn't do anything yet
  }
  return;
};


void  QwHaloMonitor::Copy(VQwDataElement *source)
{
  try
    {
      if(typeid(*source)==typeid(*this)){
	QwHaloMonitor* input=((QwHaloMonitor*)source);
	this->fElementName=input->fElementName;
	this->fHalo_Counter.Copy(&(input->fHalo_Counter));
      }
      else{
	TString loc="Standard exception from QwHaloMonitor::Copy = "
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


