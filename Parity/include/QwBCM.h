/**********************************************************\
* File: QwBPMStripline.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwVQWK_BCM__
#define __QwVQWK_BCM__

#include <vector>
#include <TTree.h>

#include "QwVQWK_Channel.h"


/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
class QwBCM : public VQwDataElement{
/////
 public:
  QwBCM() { };
  QwBCM(TString name){
    InitializeChannel(name,"raw");
  };
  ~QwBCM() {DeleteHistograms();};

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);

  void  InitializeChannel(TString name, TString datatosave);
  void  ClearEventData();

  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ProcessEvent();
  Bool_t IsGoodEvent();
  void Print() const;

  QwBCM& operator=  (const QwBCM &value);
  QwBCM& operator+= (const QwBCM &value);
  QwBCM& operator-= (const QwBCM &value);
  void Sum(QwBCM &value1, QwBCM &value2);
  void Difference(QwBCM &value1, QwBCM &value2);
  void Ratio(QwBCM &numer, QwBCM &denom);
  void Scale(Double_t factor);

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  DeleteHistograms();

  void Copy(VQwDataElement *source);

/////
 protected:

/////
 private:

  Double_t fPedestal;
  Double_t fCalibration;

  QwVQWK_Channel fTriumf_ADC;
};

#endif
