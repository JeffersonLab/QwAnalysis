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
#include "QwSubsystemArray.h"

class QwHelicityPattern{
  /******************************************************************
   *  Class: QwHelicityPattern
   *
   *
   ******************************************************************/
 public:
  QwHelicityPattern(QwSubsystemArray &event, Int_t pattern_size);
  ~QwHelicityPattern(){};

  void  LoadEventData(QwSubsystemArray &event);

  Bool_t IsCompletePattern();

  void  CalculateAsymmetry();

  void  ConstructHistograms(){ConstructHistograms((TDirectory*)NULL);};
  void  ConstructHistograms(TDirectory *folder);
  void  FillHistograms();
  void  DeleteHistograms();
  void  ClearEventData();
  

 protected:
  Bool_t fDEBUG;
  Int_t fAsymSoFar;
  std::vector<QwSubsystemArray> fEvents;
  std::vector<Bool_t> fEventLoaded;
  std::vector<Int_t> fFakeHelicity;// this is here up to when we code the Helicty decoding routine

  QwSubsystemArray  fYield;
  QwSubsystemArray  fAsymmetry;


 private:
  QwSubsystemArray pos_sum;
  QwSubsystemArray neg_sum;
  QwSubsystemArray difference;
  
};


#endif
