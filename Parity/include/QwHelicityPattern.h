/**********************************************************\
* File: QwHelicityPattern.h                                *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/
#ifndef __QwHelicityPattern__
#define __QwHelicityPattern__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwSubsystemArrayParity.h"
#include "QwBlinder.h"

// Forward declarations
class QwHelicity;

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
  ~QwHelicityPattern() { };

  /// \brief Define the configuration options
  static void DefineOptions(QwOptions &options);
  /// \brief Process the configuration options
  void ProcessOptions(QwOptions &options);

  void  LoadEventData(QwSubsystemArrayParity &event);
  Bool_t IsCompletePattern() const;
  void  CalculateAsymmetry();

  /// Enable/disable alternate asymmetry calculation
  void  EnableAlternateAsymmetry(const Bool_t flag = kTRUE) { fEnableAlternateAsym = flag; };
  /// Disable alternate asymmetry calculation
  void  DisableAlternateAsymmetry() { fEnableAlternateAsym = kFALSE; };
  /// Status of alternate asymmetry calculation flag
  Bool_t IsAlternateAsymEnabled() { return fEnableAlternateAsym; };

  /// Enable/disable burst sum calculation
  void  EnableBurstSum(const Bool_t flag = kTRUE) { fEnableBurstSum = flag; };
  /// Disable burst sum calculation
  void  DisableBurstSum() { fEnableBurstSum = kFALSE; };
  /// Status of burst sum calculation flag
  Bool_t IsBurstSumEnabled() { return fEnableBurstSum; };

  /// Enable/disable running sum calculation
  void  EnableRunningSum(const Bool_t flag = kTRUE) { fEnableRunningSum = flag; };
  /// Disable running sum calculation
  void  DisableRunningSum() { fEnableRunningSum = kFALSE; };
  /// Status of running sum calculation flag
  Bool_t IsRunningSumEnabled() { return fEnableRunningSum; };

  void  AccumulateBurstSum();
  void  AccumulateRunningBurstSum();
  void  AccumulateRunningSum();

  void  CalculateBurstAverage();
  void  CalculateRunningBurstAverage();
  void  CalculateRunningAverage();

  void  PrintRunningBurstAverage() const;
  void  PrintRunningAverage() const;

  void  ConstructHistograms(){ConstructHistograms((TDirectory*)NULL);};
  void  ConstructHistograms(TDirectory *folder);
  void  FillHistograms();
  void  DeleteHistograms();
  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile &trim_tree);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  FillDB(QwDatabase *db);
  Bool_t IsGoodAsymmetry(){ return IsGood;};

  void  ClearEventData();
  void  ClearBurstSum();
  void  ClearRunningSum();

  void  Print() const;

 protected:
  Bool_t fDEBUG;

  //  QwHelicity* fHelicitySubsystem;

  std::vector<QwSubsystemArrayParity> fEvents;
  std::vector<Bool_t> fEventLoaded;
  std::vector<Int_t> fHelicity;// this is here up to when we code the Helicity decoding routine
  std::vector<Int_t> fEventNumber;
  Int_t fCurrentPatternNumber;
  Int_t fPatternSize;
  Int_t fQuartetNumber;

  // Blinding strategy
  QwBlinder fBlinder;

  // Yield and asymmetry of a single helicity pattern
  QwSubsystemArrayParity fYield;
  QwSubsystemArrayParity fAsymmetry;
  // Alternate asymmetry calculations
  Bool_t fEnableAlternateAsym;
  QwSubsystemArrayParity fAsymmetry1;
  QwSubsystemArrayParity fAsymmetry2;

  // Burst sum/difference of the yield and asymmetry
  Bool_t fEnableBurstSum;
  QwSubsystemArrayParity fBurstYield;
  QwSubsystemArrayParity fBurstDifference;
  QwSubsystemArrayParity fBurstAsymmetry;
  QwSubsystemArrayParity fRunningBurstYield;
  QwSubsystemArrayParity fRunningBurstDifference;
  QwSubsystemArrayParity fRunningBurstAsymmetry;

  // Running sum/average of the yield and asymmetry
  Bool_t fEnableRunningSum;
  QwSubsystemArrayParity fRunningYield;
  QwSubsystemArrayParity fRunningDifference;
  QwSubsystemArrayParity fRunningAsymmetry;
  QwSubsystemArrayParity fRunningAsymmetry1;
  QwSubsystemArrayParity fRunningAsymmetry2;

 private:

  QwSubsystemArrayParity fDifference;
  QwSubsystemArrayParity fPositiveHelicitySum;
  QwSubsystemArrayParity fNegativeHelicitySum;


  Bool_t IsGood;



};


#endif
