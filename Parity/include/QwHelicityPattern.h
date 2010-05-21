/**********************************************************\
* File: QwHelicityPattern.h                                *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/
#ifndef __QwHelicityPattern__
#define __QwHelicityPattern__


#include "TTree.h"
#include "QwSubsystemArrayParity.h"
#include "QwBlinder.h"

#include <vector>
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

  void ProcessOptions(QwOptions &options); //Handle command line options
  void  LoadEventData(QwSubsystemArrayParity &event);
  Bool_t IsCompletePattern();
  void  CalculateAsymmetry(QwBlinder *blinder = 0);
  void  CalculateBurstAverage();
  void  CalculateRunningAverage();

  void  ConstructHistograms(){ConstructHistograms((TDirectory*)NULL);};
  void  ConstructHistograms(TDirectory *folder);
  void  FillHistograms();
  void  DeleteHistograms();
  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  FillDB(QwDatabase *db);
  Bool_t IsGoodAsymmetry(){ return IsGood;};

  void  ClearEventData();
  void  ClearBurstSum();
  void  ClearRunningSum();

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

  // Yield and asymmetry of a single helicity pattern
  QwSubsystemArrayParity fYield;
  QwSubsystemArrayParity fAsymmetry;
  // Alternate asymmetry calculations
  Bool_t fEnableAlternateAsym;
  QwSubsystemArrayParity fAsymmetry1;
  QwSubsystemArrayParity fAsymmetry2;

  // Burst sum/difference of the yield and asymmetry
  Bool_t fEnableBurstSum;
  QwSubsystemArrayParity fBurstSumYield;
  QwSubsystemArrayParity fBurstSumDifference;

  // Running sum/average of the yield and asymmetry
  Bool_t fEnableRunningSum;
  QwSubsystemArrayParity fRunningSumYield;
  QwSubsystemArrayParity fRunningSumAsymmetry;

 private:

  QwSubsystemArrayParity fDifference;
  QwSubsystemArrayParity fPositiveHelicitySum;
  QwSubsystemArrayParity fNegativeHelicitySum;


  Bool_t IsGood;



};


#endif
