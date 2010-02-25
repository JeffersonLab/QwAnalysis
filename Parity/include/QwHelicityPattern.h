/**********************************************************\
* File: QwHelicityPattern.h                                *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/
#ifndef __QwHelicityPattern__
#define __QwHelicityPattern__

#include <vector>
#include <TTree.h>
#include "QwSubsystemArrayParity.h"

///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwHelicityPattern{
  /******************************************************************
   *  Class: QwHelicityPattern
   *
   *
   ******************************************************************/
 public:
  QwHelicityPattern(QwSubsystemArrayParity &event);
  ~QwHelicityPattern(){};

  void  LoadEventData(QwSubsystemArrayParity &event);
  Bool_t IsCompletePattern();
  void  CalculateAsymmetry();
  void CalculateRunningAverage();

  void  ConstructHistograms(){ConstructHistograms((TDirectory*)NULL);};
  void  ConstructHistograms(TDirectory *folder);
  void  FillHistograms();
  void  DeleteHistograms();
  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);

  Bool_t IsGoodAsymmetry(){ return IsGood;};

  void  ClearEventData();
  void  Print();

 protected:
  Bool_t fDEBUG;
  std::vector<QwSubsystemArrayParity> fEvents;
  std::vector<Bool_t> fEventLoaded;
  std::vector<Int_t> fHelicity;// this is here up to when we code the Helicity decoding routine
  std::vector<Int_t> fEventNumber;
  Int_t fCurrentPatternNumber;
  Int_t fPatternSize;
  Int_t fQuartetNumber;

  QwSubsystemArrayParity  fYield;
  QwSubsystemArrayParity  fAsymmetry;
  QwSubsystemArrayParity fAverage;
  QwSubsystemArrayParity fRunningSum;
 private:
  QwSubsystemArrayParity pos_sum;
  QwSubsystemArrayParity neg_sum;
  QwSubsystemArrayParity difference;
  
  
  Bool_t IsGood; 
  
};


#endif
