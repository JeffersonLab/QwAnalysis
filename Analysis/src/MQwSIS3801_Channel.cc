/**********************************************************\
* File: MQwSIS3801_Channel.cc                              *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#include "MQwSIS3801_Channel.h"
#include "QwHistogramHelper.h"
#include <stdexcept>


const Bool_t MQwSIS3801_Channel::kDEBUG = kFALSE;


/********************************************************/
Int_t MQwSIS3801_Channel::GetEventcutErrorCounters(){// report number of events falied due to HW and event cut faliure

  return 1;
}

void MQwSIS3801_Channel::ClearEventData()
{
  fValue = 0;
};

void MQwSIS3801_Channel::RandomizeEventData(int helicity)
{
   
  Double_t mean = 300.0;
  Double_t sigma = 50.0;
  UInt_t Dataword = gRandom->Gaus(mean,sigma);

  //At this point, I am not clear about the sigle word structure yet.
  //How many bits will be configured for data, which bits will be configured for user info?
  //std::cout<<"word = "<<std::hex<<word<<std::dec<<std::endl;
  fValue = Dataword;
};


Int_t MQwSIS3801_Channel::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t index)
{
  UInt_t words_read = 0;
  if (IsNameEmpty()){
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
    //  words_read = fNumberOfDataWords;
  } 

  else if (num_words_left >= fNumberOfDataWords) {
      this->fValue = buffer[0];
      words_read = fNumberOfDataWords;
  }

  else
      {
	std::cerr << "MQwSIS3801_Channel::ProcessEvBuffer: Not enough words!"
		  << std::endl;
      }
  return words_read;
};

void MQwSIS3801_Channel::SetEventData(Double_t &value){

  this->fValue = (UInt_t)value;

}

void MQwSIS3801_Channel::EncodeEventData(std::vector<UInt_t> &buffer)
{
  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
  } else {
    buffer.push_back(UInt_t(this->fValue));
  }
};


void MQwSIS3801_Channel::ProcessEvent()
{
  //scaler events processed here.
  return;
};


void MQwSIS3801_Channel::Print() const
{
  std::cout<<"***************************************"<<"\n";
  std::cout<<"MQwSIS3801 channel: "<<GetElementName()<<"\n"<<"\n";
  return;
}

void  MQwSIS3801_Channel::ConstructHistograms(TDirectory *folder, TString &prefix){
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

void  MQwSIS3801_Channel::FillHistograms(){

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


void  MQwSIS3801_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = "/D";

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }
};

void  MQwSIS3801_Channel::FillTreeVector(std::vector<Double_t> &values)
{
  if (IsNameEmpty()){
    //  This channel is not used, so skip setting up the tree.
  } else if (fTreeArrayNumEntries<=0){
    std::cerr << "MQwSIS3801_Channel::FillTreeVector:  fTreeArrayNumEntries=="
	      << fTreeArrayNumEntries << std::endl;
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries){
    std::cerr << "MQwSIS3801_Channel::FillTreeVector:  values.size()=="
	      << values.size()
	      << "; fTreeArrayIndex+fTreeArrayNumEntries=="
	      << fTreeArrayIndex+fTreeArrayNumEntries
	      << std::endl;
  } else {
    size_t index=fTreeArrayIndex;
    values[index++] = this->fValue;
  }
};


MQwSIS3801_Channel& MQwSIS3801_Channel::operator= (const MQwSIS3801_Channel &value){
  if (!IsNameEmpty()) {
    this->fValue  = value.fValue;
  }
  return *this;
};


MQwSIS3801_Channel& MQwSIS3801_Channel::operator+= (const MQwSIS3801_Channel &value){
  if (!IsNameEmpty()){
    this->fValue  += value.fValue;
  }
  return *this;
};

MQwSIS3801_Channel& MQwSIS3801_Channel::operator-= (const MQwSIS3801_Channel &value){
  if (!IsNameEmpty()){
    this->fValue  -= value.fValue;
  }
  return *this;
};

void MQwSIS3801_Channel::Sum(MQwSIS3801_Channel &value1, MQwSIS3801_Channel &value2){
  *this =  value1;
  *this += value2;
};

void MQwSIS3801_Channel::Difference(MQwSIS3801_Channel &value1, MQwSIS3801_Channel &value2){
  *this =  value1;
  *this -= value2;
};

void MQwSIS3801_Channel::Ratio(MQwSIS3801_Channel &numer, MQwSIS3801_Channel &denom){
  if (!IsNameEmpty()){

  }
};

void MQwSIS3801_Channel::Offset(Double_t offset)
{
  if (!IsNameEmpty())
    {

    }
};


void MQwSIS3801_Channel::Scale(Double_t scale)
{
  if (!IsNameEmpty())
    {
	this->fValue  = this->fValue*scale;
    }
};



Bool_t MQwSIS3801_Channel::ApplySingleEventCuts(){
  

  return kTRUE;
};



void MQwSIS3801_Channel::Copy(VQwDataElement *source)
{
    try
    {
     if(typeid(*source)==typeid(*this))
       {
	 MQwSIS3801_Channel* input=((MQwSIS3801_Channel*)source);
	 this->fElementName=input->fElementName;

       }
     else
       {
	 TString loc="Standard exception from MQwSIS3801_Channel::Copy = "
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

void  MQwSIS3801_Channel::ReportErrorCounters(){
    if (fNumEvtsWithHWErrors>0) 
      std::cout << "SIS3801 channel " << GetElementName() 
		<< " had " << fNumEvtsWithHWErrors 
		<< " events with a hardware faliure."
		<< std::endl;

    if (fNumEvtsWithEventCutsRejected>0) 
      std::cout << "SIS3801 channel " << GetElementName() 
		<< " had " << fNumEvtsWithEventCutsRejected 
		<< " events rejected by Event Cuts."
		<< std::endl;
  };
