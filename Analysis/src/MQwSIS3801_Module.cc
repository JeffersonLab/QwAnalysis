/**********************************************************\
* File: MQwSIS3801_Module.cc                               *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#include <vector>
#include <TTree.h>
#include <boost/array.hpp>

#include "MQwSIS3801_Module.h"

#include "QwHistogramHelper.h"




void MQwSIS3801_Module::SetChannel(size_t channel, TString &name)
{
//   std::cerr << "MQwSIS3801_Module::SetChannel:  Trying to set channel "
// 	    << channel << " to name "
// 	    << name
// 	    << "; current name==\"" << fChannels.at(channel).GetElementName()
// 	    << "\"" <<std::endl;
  if (channel >= fChannels.size()){
    std::cerr << "MQwSIS3801_Module::SetChannel:  Channel number out of range: channel==" << channel << std::endl;
  } else if (fChannels.at(channel).GetElementName()!="") {
    std::cerr << "MQwSIS3801_Module::SetChannel:  Channel " << channel << " already has a name: "
	      << fChannels.at(channel).GetElementName()
	      << ".  We can't rename it!."
	      << std::endl;
  } else {
    std::cout << "SIS3801: Set channel" << channel << " to " << name << std::endl;
    fChannels.at(channel).InitializeChannel(name);
  }
};

Int_t MQwSIS3801_Module::ProcessConfigBuffer(UInt_t* buffer, UInt_t num_words_left)
{
  UInt_t words_read = 0;
  UInt_t numblocks  = 0;
  UInt_t numsamples = 0;
  std::cout<<" ============================ \n";
  std::cout<<" In MQwSIS3801_Module::ProcessConfigBuffer \n";
  std::cout<<" ============================ \n";
  if (num_words_left >= 32){
    if (buffer[0]==buffer[16]){
      //  The index values agree with each other, so let's continue.
      if (buffer[3]!=0xFF && buffer[3]==buffer[3+16]){
	numblocks = buffer[3];
      } else if (buffer[3+16]!=0xFF){
	numblocks = buffer[3+16];
      }
      if (buffer[4]!=0xFFFF && buffer[4]==buffer[3+16]){
	numsamples = buffer[4];
      } else if (buffer[4+16]!=0xFFFF){
	numsamples = buffer[4+16];
      }
      if (numblocks!=0 && numsamples!=0){
	fIdealNumberOfSamples = (numblocks+1)*numsamples;

	std::cout<<"Set fIdealNumberOfSamples to " << fIdealNumberOfSamples << std::endl;
	std::cerr << "Set fIdealNumberOfSamples to " << fIdealNumberOfSamples << std::endl;
      }
    }
    words_read = 32;
  }
  return words_read;
};

void MQwSIS3801_Module::ClearEventData()
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).ClearEventData();
  }
  fEventIsGood = kTRUE;
};


void MQwSIS3801_Module::RandomizeEventData(int helicity)
{
  // Randomize all MQwSIS3801_Channels
  for (size_t i = 0; i < fChannels.size(); i++) {
    fChannels.at(i).RandomizeEventData(helicity);
  }
  fEventIsGood = kTRUE;

}
//jpan: set mock data
void MQwSIS3801_Module::SetEventData(Double_t* buffer)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).SetEventData(buffer[i]);
  }
  fEventIsGood = kTRUE;
};

//jpan: encode data for each channel
void MQwSIS3801_Module::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).EncodeEventData(buffer);
  }
};

Int_t MQwSIS3801_Module::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left)
{
  UInt_t words_read = 0;
  if (num_words_left >= fNumberOfDataWords){
    for (size_t i=0; i<fChannels.size(); i++){
      words_read += fChannels.at(i).ProcessEvBuffer(&(buffer[words_read]),
						    num_words_left-words_read);
    }

  } else {
    std::cerr << "MQwSIS3801_Module::ProcessEvBuffer: Not enough words!"
	      << std::endl;
  }
  return words_read;
};

void  MQwSIS3801_Module::ProcessEvent()
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).ProcessEvent();
  }
};

void  MQwSIS3801_Module::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();
  std::cerr << " MQwSIS3801_Module::ConstructHistograms:  prefix==" << prefix
	    << std::endl;
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).ConstructHistograms(folder, prefix);
  }
};

void  MQwSIS3801_Module::FillHistograms()
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).FillHistograms();
  }
};

void  MQwSIS3801_Module::DeleteHistograms()
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).DeleteHistograms();
  }};

void  MQwSIS3801_Module::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).ConstructBranchAndVector(tree, prefix, values);
  }
};

void  MQwSIS3801_Module::FillTreeVector(std::vector<Double_t> &values)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).FillTreeVector(values);
  }
};

MQwSIS3801_Channel* MQwSIS3801_Module::GetChannel(const TString name)
{
  MQwSIS3801_Channel* tmp = NULL;
  if (! fChannels.empty()) {
    for (size_t i = 0; i < fChannels.size(); i++) {
      if (fChannels.at(i).GetElementName() == name) {
        tmp = &(fChannels.at(i));
      }
    }
  }
  return tmp;
};


MQwSIS3801_Module& MQwSIS3801_Module::operator= (const MQwSIS3801_Module &value)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i) = value.fChannels.at(i);
  }
  fEventIsGood = value.fEventIsGood;
  return *this;
};

MQwSIS3801_Module& MQwSIS3801_Module::operator+= (const MQwSIS3801_Module &value)
{

  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i) += value.fChannels.at(i);
  }
  fEventIsGood = value.fEventIsGood;


  return *this;
};

MQwSIS3801_Module& MQwSIS3801_Module::operator-= (const MQwSIS3801_Module &value)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i) -= value.fChannels.at(i);
  }
  fEventIsGood = value.fEventIsGood;
  return *this;
};

void MQwSIS3801_Module::Sum(MQwSIS3801_Module &value1, MQwSIS3801_Module &value2)
{
  *this =  value1;
  *this += value2;
};

void MQwSIS3801_Module::Difference(MQwSIS3801_Module &value1, MQwSIS3801_Module &value2)
{
  *this =  value1;
  *this -= value2;
};

void MQwSIS3801_Module::Ratio(MQwSIS3801_Module &numer, MQwSIS3801_Module &denom)
{
  fEventIsGood = numer.fEventIsGood && denom.fEventIsGood;
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).Ratio(numer.fChannels.at(i), denom.fChannels.at(i));
  }
};



void MQwSIS3801_Module::InitializeChannel(TString name)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).InitializeChannel(name);
  }
};

void MQwSIS3801_Module::Copy(MQwSIS3801_Module *source)
{
  try {
    if (typeid(*source) == typeid(*this)) {
      MQwSIS3801_Module* input = ((MQwSIS3801_Module*) source);
      for (size_t i = 0; i < this->fChannels.size(); i++)
        this->fChannels[i].Copy(&(input->fChannels[i]));
    } else {
      TString loc("Standard exception from MQwSIS3801_Module::Copy = ");
      loc += TString("arguments are not of the same type");
      throw std::invalid_argument(loc.Data());
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
};

void  MQwSIS3801_Module::Print() const
{
  for (size_t i = 0; i < fChannels.size(); i++) {
    fChannels.at(i).Print();
  }
};



