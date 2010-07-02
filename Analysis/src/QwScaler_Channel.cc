/**********************************************************\
* File: QwScaler_Channel.cc                              *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#include "QwScaler_Channel.h"
#include "QwHistogramHelper.h"
#include <stdexcept>

#include <QwLog.h>


template<unsigned int data_mask, unsigned int data_shift>
const Bool_t QwScaler_Channel<data_mask,data_shift>::kDEBUG = kFALSE;


/********************************************************/
template<unsigned int data_mask, unsigned int data_shift>
Int_t QwScaler_Channel<data_mask,data_shift>::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure

  return 1;
}

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::ClearEventData()
{
  fValue_Raw = 0;
  fValue     = 0.0;
};

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::RandomizeEventData(int helicity)
{

  Double_t mean = 300.0;
  Double_t sigma = 50.0;
  UInt_t Dataword = abs((Int_t)gRandom->Gaus(mean,sigma));

  //At this point, I am not clear about the sigle word structure yet.
  //How many bits will be configured for data, which bits will be configured for user info?
  //std::cout<<"word = "<<std::hex<<word<<std::dec<<std::endl;
  fValue_Raw = Dataword;
};


template<unsigned int data_mask, unsigned int data_shift>
Int_t QwScaler_Channel<data_mask,data_shift>::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index)
{
  UInt_t words_read = 0;
  if (IsNameEmpty()){
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
      words_read = fNumberOfDataWords;
  } else if (num_words_left >= fNumberOfDataWords) {
    this->fValue_Raw = ((buffer[0] & data_mask) >> data_shift);
    this->fValue     = 0.0 + this->fValue_Raw;
    words_read = fNumberOfDataWords;
  } else {
    QwError << "QwScaler_Channel::ProcessEvBuffer: Not enough words!"
	    << QwLog::endl;
  }
  return words_read;
};

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::SetEventData(Double_t value){

  this->fValue     = value;
  this->fValue_Raw = (UInt_t)value;
  //std::cout<<"fValue is set to: value = "<<value<<std::endl;

}

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::EncodeEventData(std::vector<UInt_t> &buffer)
{
  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
  } else {
    buffer.push_back( ((this->fValue_Raw<<data_shift)&data_mask) );
    //std::cout<<"this->fValue="<<this->fValue<<std::endl;
  }
};


template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::ProcessEvent()
{
  //scaler events processed here.
  return;
};


template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::PrintValue() const
{
  QwMessage << std::setprecision(4)
            << std::setw(18) << std::left << GetElementName() << ","
            << std::setw(15) << std::left << GetValue()
            << QwLog::endl;
}

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::PrintInfo() const
{
  QwMessage << "***************************************" << QwLog::endl;
  QwMessage << "QwScaler channel: " << GetElementName()
            << QwLog::endl;
}

template<unsigned int data_mask, unsigned int data_shift>
void  QwScaler_Channel<data_mask,data_shift>::ConstructHistograms(TDirectory *folder, TString &prefix){
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else {
    //  Now create the histograms.
    TString basename, fullname;
    basename = prefix + GetElementName();

    fHistograms.resize(1, NULL);
    size_t index=0;
    fHistograms[index]   = gQwHists.Construct1DHist(basename);
    index += 1;
  }
};

template<unsigned int data_mask, unsigned int data_shift>
void  QwScaler_Channel<data_mask,data_shift>::FillHistograms(){

  size_t index=0;
  if (IsNameEmpty()){
    //  This channel is not used, so skip creating the histograms.
  } else {
    if (fHistograms[index] != NULL)
      //std::cout<<GetElementName()<<" is filled with value "<<this->fValue<<std::endl;
      fHistograms[index]->Fill(this->fValue);
    index += 1;
  }
};


template<unsigned int data_mask, unsigned int data_shift>
void  QwScaler_Channel<data_mask,data_shift>::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = basename + "/D";

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }
};

template<unsigned int data_mask, unsigned int data_shift>
void  QwScaler_Channel<data_mask,data_shift>::ConstructBranch(TTree *tree, TString &prefix)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();

    tree->Branch(basename, &fValue);
  }
};

template<unsigned int data_mask, unsigned int data_shift>
void  QwScaler_Channel<data_mask,data_shift>::FillTreeVector(std::vector<Double_t> &values)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else if (fTreeArrayNumEntries<=0){
    QwError << "QwScaler_Channel::FillTreeVector:  fTreeArrayNumEntries=="
	    << fTreeArrayNumEntries << QwLog::endl;
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries){
    QwError << "QwScaler_Channel::FillTreeVector:  values.size()=="
	    << values.size()
	    << "; fTreeArrayIndex+fTreeArrayNumEntries=="
	    << fTreeArrayIndex+fTreeArrayNumEntries
	    << QwLog::endl;
  } else {
    size_t index=fTreeArrayIndex;
    values[index++] = this->fValue;
  }
};


template<unsigned int data_mask, unsigned int data_shift>
QwScaler_Channel<data_mask,data_shift>& QwScaler_Channel<data_mask,data_shift>::operator= (const QwScaler_Channel<data_mask,data_shift> &value){
  if (!IsNameEmpty()) {
    this->fValue  = value.fValue;
  }
  return *this;
};


template<unsigned int data_mask, unsigned int data_shift>
QwScaler_Channel<data_mask,data_shift>& QwScaler_Channel<data_mask,data_shift>::operator+= (const QwScaler_Channel<data_mask,data_shift> &value){
  if (!IsNameEmpty()){
    this->fValue  += value.fValue;
  }
  return *this;
};

template<unsigned int data_mask, unsigned int data_shift>
QwScaler_Channel<data_mask,data_shift>& QwScaler_Channel<data_mask,data_shift>::operator-= (const QwScaler_Channel<data_mask,data_shift> &value){
  if (!IsNameEmpty()){
    this->fValue  -= value.fValue;
  }
  return *this;
};

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::Sum(QwScaler_Channel<data_mask,data_shift> &value1, QwScaler_Channel<data_mask,data_shift> &value2){
  *this =  value1;
  *this += value2;
};

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::Difference(QwScaler_Channel<data_mask,data_shift> &value1, QwScaler_Channel<data_mask,data_shift> &value2){
  *this =  value1;
  *this -= value2;
};

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::Ratio(QwScaler_Channel<data_mask,data_shift> &numer, QwScaler_Channel<data_mask,data_shift> &denom){
  if (!IsNameEmpty()){

  }
};

template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::Offset(Double_t offset)
{
  if (!IsNameEmpty())
    {

    }
};


template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::Scale(Double_t scale)
{
  if (!IsNameEmpty())
    {
      this->fValue  = (UInt_t)( this->fValue*scale);
    }
};



template<unsigned int data_mask, unsigned int data_shift>
Bool_t QwScaler_Channel<data_mask,data_shift>::ApplySingleEventCuts(){


  return kTRUE;
};



template<unsigned int data_mask, unsigned int data_shift>
void QwScaler_Channel<data_mask,data_shift>::Copy(VQwDataElement *source)
{
    try
    {
     if(typeid(*source)==typeid(*this))
       {
	 QwScaler_Channel<data_mask,data_shift>* input=((QwScaler_Channel<data_mask,data_shift>*)source);
	 this->fElementName=input->fElementName;

       }
     else
       {
	 TString loc="Standard exception from QwScaler_Channel::Copy = "
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

template<unsigned int data_mask, unsigned int data_shift>
void  QwScaler_Channel<data_mask,data_shift>::ReportErrorCounters(){
    if (fNumEvtsWithHWErrors>0)
      QwMessage << "QwScaler_Channel " << GetElementName()
		<< " had " << fNumEvtsWithHWErrors
		<< " events with a hardware faliure."
		<< QwLog::endl;

    if (fNumEvtsWithEventCutsRejected>0)
      QwMessage << "QwScaler_Channel " << GetElementName()
		<< " had " << fNumEvtsWithEventCutsRejected
		<< " events rejected by Event Cuts."
		<< QwLog::endl;
  };


//  These explicit class template instantiations should be the
//  last thing in this file.  This list should cover all of the
//  types that are typedef'ed in the header file.
template class QwScaler_Channel<0x00ffffff,0>;  // QwSIS3801D24_Channel
template class QwScaler_Channel<0xffffffff,0>;  // QwSIS3801_Channel, etc.
