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

class QwVQWK_Module {
  /******************************************************************
   *  Class: QwVQWK_Module
   *         Base class containing decoding functions for a single
   *         VQWK ADC module.
   ******************************************************************/
 public:
  QwVQWK_Module() {
    fIdealNumberOfSamples = 16680;
    for (size_t i=0; i<fChannels.size(); i++){
      fChannels.at(i).InitializeChannel("","raw");
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





#endif
