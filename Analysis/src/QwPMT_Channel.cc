/**********************************************************\
* File: QwPMT_Channel.cc                                   *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2009-03-07 12:00>                           *
\**********************************************************/

#include "QwPMT_Channel.h"

const Bool_t QwPMT_Channel::kDEBUG = kFALSE;


/*!  Conversion factor to translate the average bit count in an ADC
 *   channel into average voltage.
 *   The base factor is 0.07929 mV per count, and zero counts corresponds
 *   to zero voltage.
 */
const Double_t QwPMT_Channel::kPMT_VoltsPerBit = 76.29e-6;


void QwPMT_Channel::ClearEventData(){
  fValue   = 0;
};

void QwPMT_Channel::RandomizeEventData(int helicity){
  //need to set to random value
  fValue   = 123;
};

void  QwPMT_Channel::EncodeEventData(std::vector<UInt_t> &TrigBuffer)
{
  std::cout<<"QwPMT_Channel::EncodeEventData() not fully implemented yet."<<std::endl;

  Long_t localbuf;

  if (IsNameEmpty()) {
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
  } else {
    localbuf = (Long_t) fValue;
    TrigBuffer.push_back(localbuf);
  }

};

void  QwPMT_Channel::ProcessEvent()
{

};


void  QwPMT_Channel::ConstructHistograms(TDirectory *folder, TString &prefix){
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

void  QwPMT_Channel::FillHistograms(){
  size_t index=0;
  if (GetElementName()==""){
    //  This channel is not used, so skip creating the histograms.
  } else {
    if (fHistograms[index] != NULL)
      fHistograms[index]->Fill(this->fValue);
    index += 1;
  }
};

void  QwPMT_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip setting up the tree.
  } else {
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = "/F";

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }
};

void  QwPMT_Channel::FillTreeVector(std::vector<Double_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the tree vector.
  } else if (fTreeArrayNumEntries<=0){
    std::cerr << "QwPMT_Channel::FillTreeVector:  fTreeArrayNumEntries=="
	      << fTreeArrayNumEntries << std::endl;
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries){
    std::cerr << "QwPMT_Channel::FillTreeVector:  values.size()=="
	      << values.size()
	      << "; fTreeArrayIndex+fTreeArrayNumEntries=="
	      << fTreeArrayIndex+fTreeArrayNumEntries
	      << std::endl;
  } else {
    size_t index=fTreeArrayIndex;
    values[index++] = this->fValue;
  }
};



QwPMT_Channel& QwPMT_Channel::operator= (const QwPMT_Channel &value){
  if (GetElementName()!=""){
    this->fValue  = value.fValue;
  }
  return *this;
};

void QwPMT_Channel::Print() const
{
  std::cout<<"QwPMT_Channel::Print() not implemented yet."<<std::endl;
};
