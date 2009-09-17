/**********************************************************\
* File: MQwSIS3801_Module.h                                *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#ifndef __MQwSIS3801_MODULE__
#define __MQwSIS3801_MODULE__

#include <vector>
#include "TTree.h"
#include "boost/array.hpp"

#include "MQwSIS3801_Channel.h"

#include "QwHistogramHelper.h"

///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class MQwSIS3801_Module {

 public:
  MQwSIS3801_Module() {

    for (size_t i=0; i<fChannels.size(); i++){
      fChannels.at(i).InitializeChannel("");
      fNumberOfDataWords = fChannels.at(i).GetNumberOfDataWords();
    }
  };
  ~MQwSIS3801_Module() {};

  void SetChannel(size_t channel, TString &name);

  Int_t ProcessConfigBuffer(UInt_t* buffer, UInt_t num_words_left);

  void  ClearEventData();
  void  RandomizeEventData(int helicity);
  void  SetEventData(Double_t* buffer); //jpan: set mock data
  void  EncodeEventData(std::vector<UInt_t> &buffer);
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left);

  MQwSIS3801_Module& operator=  (const MQwSIS3801_Module &value);
  MQwSIS3801_Module& operator+= (const MQwSIS3801_Module &value);
  MQwSIS3801_Module& operator-= (const MQwSIS3801_Module &value);
  void Sum(MQwSIS3801_Module &value1, MQwSIS3801_Module &value2);
  void Difference(MQwSIS3801_Module &value1, MQwSIS3801_Module &value2);
  void Ratio(MQwSIS3801_Module &numer, MQwSIS3801_Module &denom);

  void ProcessEvent();


  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);


  size_t GetSequenceNumber(){return (fSequenceNumber);};
  
  // formarly known as IsGoodEvent()
  Bool_t ApplyHWChecks(){return fEventIsGood;}; //Check for harware errors in the devices and this is replaced for IsGoodEvent() routine

  void SetPedestal(Double_t pedestal);
  void SetCalibrationFactor(Double_t factor);
  void InitializeChannel(TString name);

  void Copy(MQwSIS3801_Module *source);

  void Print() const;
  MQwSIS3801_Channel* GetChannel(const TString name);



 protected:


 private:
  UInt_t  fNumberOfDataWords;
  size_t  fIdealNumberOfSamples;

  size_t  fSequenceNumber;
  Bool_t  fEventIsGood;

  boost::array<MQwSIS3801_Channel, 32> fChannels;  //  If we change the number of channels in the module this needs to change.

};





#endif
