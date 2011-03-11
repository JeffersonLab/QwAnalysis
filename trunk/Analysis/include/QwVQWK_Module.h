
/**********************************************************\
* File: QwVQWK_Module.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QwVQWK_MODULE__
#define __QwVQWK_MODULE__

#include <vector>
#include "TTree.h"
#include "boost/array.hpp"

#include "QwVQWK_Channel.h"

///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
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
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity);
  void  SetEventData(Double_t* buffer, UInt_t sequencenumber); //jpan: set mock data
  void  EncodeEventData(std::vector<UInt_t> &buffer);
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left);

  QwVQWK_Module& operator=  (const QwVQWK_Module &value);
  QwVQWK_Module& operator+= (const QwVQWK_Module &value);
  QwVQWK_Module& operator-= (const QwVQWK_Module &value);
  void Sum(QwVQWK_Module &value1, QwVQWK_Module &value2);
  void Difference(QwVQWK_Module &value1, QwVQWK_Module &value2);
  void Ratio(QwVQWK_Module &numer, QwVQWK_Module &denom);

  void ProcessEvent();


  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values) const;


  size_t GetSequenceNumber(){return (fSequenceNumber);};

  // formarly known as IsGoodEvent()
  Bool_t ApplyHWChecks(){return fEventIsGood;}; //Check for harware errors in the devices and this is replaced for IsGoodEvent() routine

  void SetPedestal(Double_t pedestal);
  void SetCalibrationFactor(Double_t factor);
  void InitializeChannel(TString name, TString datatosave);

  void Copy(QwVQWK_Module *source);

  void PrintValue() const;
  void PrintInfo() const;

  QwVQWK_Channel* GetChannel(const TString name);

//  If we change the number of channels in the module this needs to change.
  boost::array<QwVQWK_Channel, 8> fChannels;


 protected:


 private:
  UInt_t  fNumberOfDataWords;
  size_t  fIdealNumberOfSamples;

  size_t  fSequenceNumber;
  Bool_t  fEventIsGood;

};





#endif
