/**********************************************************\
* File: QwSIS3801_Module.h                                *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#ifndef __QwSIS3801_MODULE__
#define __QwSIS3801_MODULE__

// System headers
#include <vector>

// Boost headers
#include "boost/array.hpp"

// ROOT headers
#include "TTree.h"

// Qweak headers
#include "QwScaler_Channel.h"

///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwSIS3801_Module {

 public:
  QwSIS3801_Module() {
    for (size_t i=0; i<fChannels.size(); i++){
      fChannels.at(i).InitializeChannel("");
      fNumberOfDataWords = fChannels.at(i).GetNumberOfDataWords();
    }
  };
  ~QwSIS3801_Module() {};

  void SetChannel(size_t channel, TString &name);

  Int_t ProcessConfigBuffer(UInt_t* buffer, UInt_t num_words_left);

  void  ClearEventData();
  void  RandomizeEventData(int helicity);
  void  SetEventData(Double_t* buffer); //jpan: set mock data
  void  EncodeEventData(std::vector<UInt_t> &buffer);
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left);

  QwSIS3801_Module& operator=  (const QwSIS3801_Module &value);
  QwSIS3801_Module& operator+= (const QwSIS3801_Module &value);
  QwSIS3801_Module& operator-= (const QwSIS3801_Module &value);
  void Sum(QwSIS3801_Module &value1, QwSIS3801_Module &value2);
  void Difference(QwSIS3801_Module &value1, QwSIS3801_Module &value2);
  void Ratio(QwSIS3801_Module &numer, QwSIS3801_Module &denom);

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
  void InitializeChannel(TString name);

  void Copy(QwSIS3801_Module *source);

  void PrintValue() const { };
  void PrintInfo() const;

  QwSIS3801D24_Channel* GetChannel(const TString name);

  // If we change the number of channels in the module this needs to change.
  boost::array<QwSIS3801D24_Channel, 32> fChannels;

 protected:


 private:
  UInt_t  fNumberOfDataWords;
  size_t  fIdealNumberOfSamples;

  size_t  fSequenceNumber;
  Bool_t  fEventIsGood;


};





#endif
