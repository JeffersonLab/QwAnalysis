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
      fChannels.at(i).InitializeChannel("","");
      fNumberOfDataWords = fChannels.at(i).GetNumberOfDataWords();
    }
  };
  virtual ~QwSIS3801_Module() {};

  void SetChannel(size_t channel, TString &name);

  Int_t ProcessConfigBuffer(UInt_t* buffer, UInt_t num_words_left);

  void  ClearEventData();

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left);

  QwSIS3801_Module& operator=  (const QwSIS3801_Module &value);
  QwSIS3801_Module& operator+= (const QwSIS3801_Module &value);
  QwSIS3801_Module& operator-= (const QwSIS3801_Module &value);

  void ProcessEvent();

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  Bool_t ApplyHWChecks(){return fEventIsGood;};

  void InitializeChannel(TString name);

  void Copy(const QwSIS3801_Module *source);

  QwSIS3801D24_Channel* GetChannel(const TString name);

  // If we change the number of channels in the module this needs to change.
  boost::array<QwSIS3801D24_Channel, 32> fChannels;

 private:
  UInt_t  fNumberOfDataWords;
  size_t  fIdealNumberOfSamples;

  size_t  fSequenceNumber;
  Bool_t  fEventIsGood;

};

#endif
