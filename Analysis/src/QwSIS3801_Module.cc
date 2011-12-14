/**********************************************************\
* File: QwSIS3801_Module.cc                               *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#include "QwSIS3801_Module.h"


void QwSIS3801_Module::SetChannel(size_t channel, TString &name)
{
  Bool_t local_debug = false;
//   std::cerr << "QwSIS3801_Module::SetChannel:  Trying to set channel "
// 	    << channel << " to name "
// 	    << name
// 	    << "; current name==\"" << fChannels.at(channel).GetElementName()
// 	    << "\"" <<std::endl;
  if (channel >= fChannels.size()){
    std::cerr << "QwSIS3801_Module::SetChannel:  Channel number out of range: channel==" << channel << std::endl;
  } else if (fChannels.at(channel).GetElementName()!="") {
    std::cerr << "QwSIS3801_Module::SetChannel:  Channel " << channel << " already has a name: "
	      << fChannels.at(channel).GetElementName()
	      << ".  We can't rename it!."
	      << std::endl;
  } else {
    if(local_debug) std::cout << "SIS3801: Set channel" << channel << " to " << name << std::endl;
    fChannels.at(channel).InitializeChannel(name,"");
  }
}

Int_t QwSIS3801_Module::ProcessConfigBuffer(UInt_t* buffer, UInt_t num_words_left)
{
  UInt_t words_read = 0;
  UInt_t numblocks  = 0;
  UInt_t numsamples = 0;

  std::cout<<" ============================ \n";
  std::cout<<" In QwSIS3801_Module::ProcessConfigBuffer \n";
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
}

void QwSIS3801_Module::ClearEventData()
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).ClearEventData();
  }
  fEventIsGood = kTRUE;
}


void QwSIS3801_Module::RandomizeEventData(int helicity)
{
  // Randomize all QwSIS3801_Channels
  for (size_t i = 0; i < fChannels.size(); i++) {
    fChannels.at(i).RandomizeEventData(helicity);
  }
  fEventIsGood = kTRUE;

}
//jpan: set mock data
void QwSIS3801_Module::SetEventData(Double_t* buffer)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).SetEventData(buffer[i]);
  }
  fEventIsGood = kTRUE;
}

//jpan: encode data for each channel
void QwSIS3801_Module::EncodeEventData(std::vector<UInt_t> &buffer)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).EncodeEventData(buffer);
  }
}

Int_t QwSIS3801_Module::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left)
{
  UInt_t words_read = 0;
  if (num_words_left >= fNumberOfDataWords){
    for (size_t i=0; i<fChannels.size(); i++){
      words_read += fChannels.at(i).ProcessEvBuffer(&(buffer[words_read]),
						    num_words_left-words_read);
    }

  } else {
    std::cerr << "QwSIS3801_Module::ProcessEvBuffer: Not enough words!"
	      << std::endl;
  }
  return words_read;
}

void  QwSIS3801_Module::ProcessEvent()
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).ProcessEvent();
  }
}

void  QwSIS3801_Module::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();
  //std::cerr << " QwSIS3801_Module::ConstructHistograms:  prefix==" << prefix<< std::endl;

  for (size_t i=0; i<fChannels.size(); i++){
    if (fChannels.at(i).GetElementName()=="") {}
    else  fChannels.at(i).ConstructHistograms(folder, prefix);
  }
}

void  QwSIS3801_Module::FillHistograms()
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).FillHistograms();
  }
}

void  QwSIS3801_Module::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).ConstructBranchAndVector(tree, prefix, values);
  }
}

void  QwSIS3801_Module::FillTreeVector(std::vector<Double_t> &values) const
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).FillTreeVector(values);
  }
}

QwSIS3801D24_Channel* QwSIS3801_Module::GetChannel(const TString name)
{
  QwSIS3801D24_Channel* tmp = NULL;
  if (! fChannels.empty()) {
    for (size_t i = 0; i < fChannels.size(); i++) {
      if (fChannels.at(i).GetElementName() == name) {
        tmp = &(fChannels.at(i));
      }
    }
  }
  return tmp;
}


QwSIS3801_Module& QwSIS3801_Module::operator= (const QwSIS3801_Module &value)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i) = value.fChannels.at(i);
  }
  fEventIsGood = value.fEventIsGood;
  return *this;
}

QwSIS3801_Module& QwSIS3801_Module::operator+= (const QwSIS3801_Module &value)
{

  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i) += value.fChannels.at(i);
  }
  fEventIsGood = value.fEventIsGood;


  return *this;
}

QwSIS3801_Module& QwSIS3801_Module::operator-= (const QwSIS3801_Module &value)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i) -= value.fChannels.at(i);
  }
  fEventIsGood = value.fEventIsGood;
  return *this;
}

void QwSIS3801_Module::Sum(QwSIS3801_Module &value1, QwSIS3801_Module &value2)
{
  *this =  value1;
  *this += value2;
}

void QwSIS3801_Module::Difference(QwSIS3801_Module &value1, QwSIS3801_Module &value2)
{
  *this =  value1;
  *this -= value2;
}

void QwSIS3801_Module::Ratio(QwSIS3801_Module &numer, QwSIS3801_Module &denom)
{
  fEventIsGood = numer.fEventIsGood && denom.fEventIsGood;
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).Ratio(numer.fChannels.at(i), denom.fChannels.at(i));
  }
}



void QwSIS3801_Module::InitializeChannel(TString name)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).InitializeChannel(name,"");
  }
}

void QwSIS3801_Module::Copy(const QwSIS3801_Module *source)
{
  try {
    if (typeid(*source) == typeid(*this)) {
      const QwSIS3801_Module* input = dynamic_cast<const QwSIS3801_Module*>(source);
      for (size_t i = 0; i < this->fChannels.size(); i++)
        this->fChannels[i].Copy(&(input->fChannels[i]));
    } else {
      TString loc("Standard exception from QwSIS3801_Module::Copy = ");
      loc += TString("arguments are not of the same type");
      throw std::invalid_argument(loc.Data());
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
}

void  QwSIS3801_Module::PrintInfo() const
{
  for (size_t i = 0; i < fChannels.size(); i++) {
    fChannels.at(i).PrintInfo();
  }
}



