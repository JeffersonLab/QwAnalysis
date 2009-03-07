/**********************************************************\
* File: QwPMT_Channel.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QwPMT_CHANNEL__
#define __QwPMT_CHANNEL__

#include <vector>
#include <TTree.h>

#include "VQwDataElement.h"
#include "QwHistogramHelper.h"



class QwPMT_Channel: public VQwDataElement {
  /******************************************************************
   *  Class: QwPMT_Channel
   *         
   ******************************************************************/
 public:
  QwPMT_Channel() { };
  QwPMT_Channel(TString name){
    InitializeChannel(name);
  };
  ~QwPMT_Channel() {DeleteHistograms();};

  void  InitializeChannel(TString name){
    SetElementName(name);
  };

  void  ClearEventData();
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement=0){return 0;};

  void  SetValue(Double_t data){fValue= data;};

  void  ProcessEvent();

  QwPMT_Channel& operator=  (const QwPMT_Channel &value);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values);
  void  FillTreeVector(std::vector<Float_t> &values);

 protected:
  

 private:
  static const Bool_t kDEBUG;

  /*  ADC Calibration                     */
  static const Double_t kPMT_VoltsPerBit;

  /*  Channel information data members    */
  

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  /*  Event data members */
  Double_t fValue;

};



#endif
