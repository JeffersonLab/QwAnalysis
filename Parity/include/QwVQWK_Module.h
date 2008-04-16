/**********************************************************\
* File: QwVQWK_Module.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QwVQWK_MODULE__
#define __QwVQWK_MODULE__

#include <vector>
#include <TTree.h>
#include <boost/array.hpp>

#include "QwVQWK_Channel.h"

#include "QwHistogramHelper.h"
extern QwHistogramHelper gQwHists;

class QwVQWK_Module{
  /******************************************************************
   *  Class: QwVQWK_Module
   *         Base class containing decoding functions for a single
   *         VQWK ADC module.
   ******************************************************************/
 public:
  QwVQWK_Module() {
    fIdealNumberOfSamples = 2000;
    for (size_t i=0; i<fChannels.size(); i++){
      fChannels.at(i).InitializeChannel("");
      fNumberOfDataWords = fChannels.at(i).GetNumberOfDataWords();
    }
  };
  ~QwVQWK_Module() {};

  void SetChannel(size_t channel, TString &name);

  Int_t ProcessConfigBuffer(UInt_t* buffer, UInt_t num_words_left);

  void  ClearEventData();
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left);

  QwVQWK_Module& operator=  (const QwVQWK_Module &value);
  QwVQWK_Module& operator+= (const QwVQWK_Module &value);
  QwVQWK_Module& operator-= (const QwVQWK_Module &value);
  void Sum(QwVQWK_Module &value1, QwVQWK_Module &value2);
  void Difference(QwVQWK_Module &value1, QwVQWK_Module &value2);
  void Ratio(QwVQWK_Module &numer, QwVQWK_Module &denom);



  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values);
  void  FillTreeVector(std::vector<Float_t> &values);


  size_t GetSequenceNumber(){return (fSequenceNumber);};

  Bool_t IsGoodEvent(){return fEventIsGood;};

 protected:
  

 private:
  UInt_t  fNumberOfDataWords;
  size_t  fIdealNumberOfSamples;

  size_t  fSequenceNumber;
  Bool_t  fEventIsGood;

  boost::array<QwVQWK_Channel, 8> fChannels;  //  If we change the number of channels in the module this needs to change.

};


void QwVQWK_Module::SetChannel(size_t channel, TString &name)
{
  if (channel >= fChannels.size()){
    std::cerr << "QwVQWK_Module::SetChannel:  Channel number out of range: channel==" << channel << std::endl;
  } else if (fChannels.at(channel).GetElementName()!="") {
    std::cerr << "QwVQWK_Module::SetChannel:  Channel " << channel << " already has a name: "
	      << fChannels.at(channel).GetElementName() 
	      << ".  We can't rename it!."
	      << std::endl;
  } else {
    fChannels.at(channel).InitializeChannel(name);
  }
};

Int_t QwVQWK_Module::ProcessConfigBuffer(UInt_t* buffer, UInt_t num_words_left)
{
  UInt_t words_read = 0;
  UInt_t numblocks  = 0;
  UInt_t numsamples = 0;
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

	std::cerr << "Set fIdealNumberOfSamples to " << fIdealNumberOfSamples << std::endl;
      }
    }
    words_read = 32;
  }
  return words_read;
};

void QwVQWK_Module::ClearEventData()
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).ClearEventData();
  }
  fEventIsGood = kTRUE;
};



Int_t QwVQWK_Module::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left)
{
  UInt_t words_read = 0;
  if (num_words_left >= fNumberOfDataWords){
    for (size_t i=0; i<fChannels.size(); i++){
      words_read += fChannels.at(i).ProcessEvBuffer(&(buffer[words_read]),
						    num_words_left-words_read);
    }
    //  Now check for errors.
    fSequenceNumber  = fChannels.at(0).GetSequenceNumber();
    fEventIsGood &= fChannels.at(0).MatchNumberOfSamples(fIdealNumberOfSamples);
    /*     std::cerr << "QwVQWK_Module::ProcessEvBuffer: fSequenceNumber==" << fSequenceNumber */
    /* 	      << "; fIdealNumberOfSamples==" << fIdealNumberOfSamples */
    /* 	      << std::endl; */
    for (size_t i=1; i<fChannels.size(); i++){
      fEventIsGood &= fChannels.at(i).MatchSequenceNumber(fSequenceNumber);
      fEventIsGood &= fChannels.at(i).MatchNumberOfSamples(fIdealNumberOfSamples);
    }
  } else {
    std::cerr << "QwVQWK_Module::ProcessEvBuffer: Not enough words!"
	      << std::endl;
  }
  return words_read;
};

void  QwVQWK_Module::ConstructHistograms(TDirectory *folder, TString &prefix)
{
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();
  std::cerr << " QwVQWK_Module::ConstructHistograms:  prefix==" << prefix
	    << std::endl;
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).ConstructHistograms(folder, prefix);
  }
};

void  QwVQWK_Module::FillHistograms()
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).FillHistograms();
  }
};

void  QwVQWK_Module::DeleteHistograms()
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).DeleteHistograms();
  }};

void  QwVQWK_Module::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).ConstructBranchAndVector(tree, prefix, values);
  }
};

void  QwVQWK_Module::FillTreeVector(std::vector<Float_t> &values)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).FillTreeVector(values);
  }
};

QwVQWK_Module& QwVQWK_Module::operator= (const QwVQWK_Module &value)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i) = value.fChannels.at(i);
  }
  fEventIsGood = value.fEventIsGood;
  return *this;
};

QwVQWK_Module& QwVQWK_Module::operator+= (const QwVQWK_Module &value)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i) += value.fChannels.at(i);
  }
  fEventIsGood &= value.fEventIsGood;
  return *this;
};

QwVQWK_Module& QwVQWK_Module::operator-= (const QwVQWK_Module &value)
{
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i) -= value.fChannels.at(i);
  }
  fEventIsGood &= value.fEventIsGood;
  return *this;
};

void QwVQWK_Module::Sum(QwVQWK_Module &value1, QwVQWK_Module &value2)
{
  *this =  value1;
  *this += value2;
};

void QwVQWK_Module::Difference(QwVQWK_Module &value1, QwVQWK_Module &value2)
{
  *this =  value1;
  *this -= value2;
};

void QwVQWK_Module::Ratio(QwVQWK_Module &numer, QwVQWK_Module &denom)
{
  fEventIsGood = numer.fEventIsGood && denom.fEventIsGood;
  for (size_t i=0; i<fChannels.size(); i++){
    fChannels.at(i).Ratio(numer.fChannels.at(i), denom.fChannels.at(i));
  }
};




#endif
