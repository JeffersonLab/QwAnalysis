/**********************************************************\
* File: QwHelicityCorrelatedFeedback.h                     *
*                                                          *
* Author: Rakitha Beminiwattha                             *
* Time-stamp: <2010-09-28>                                 *
\**********************************************************/
#ifndef __QwHelicityCorrelatedFeedback__
#define __QwHelicityCorrelatedFeedback__

#include "QwHelicityPattern.h"

#include "QwEPICSControl.h"
#include "GreenMonster.h"


///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwHelicityCorrelatedFeedback : public QwHelicityPattern {
  /******************************************************************
   *  Class: QwHelicityCorrelatedFeedback
   *
   *
   ******************************************************************/

 public:
  QwHelicityCorrelatedFeedback(QwSubsystemArrayParity &event):QwHelicityPattern(event){
    fEnableBurstSum=kFALSE;
    fGoodPatternCounter=0;
  };
    
    
    ~QwHelicityCorrelatedFeedback() { };  
    ///inherited from QwHelicityPattern
    void CalculateAsymmetry();
    void ClearRunningSum();

    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);

    /// \brief Load deltaA_q, no.of good patterns to accumulate and other neccessary feedback parameters
    void LoadParameterFile(TString filename);



 

    /// \brief Feed the Hall C IA set point based on the charge asymmetry 
    void FeedIASetPoint();

    /// \brief Feed the IA set point based on the charge asymmetry 
    void FeedPCPos();
    /// \brief Feed the IA set point based on the charge asymmetry 
    void FeedPCNeg();

    /// \brief Log the last feedback information
    void LogParameters();

    /// \brief Set Clean=0 or 1 in the GreenMonster
    void UpdateGMClean(Int_t state);

    /// \brief Update last feedback setting into scan variables in the GreenMonster
    void UpdateGMScanParameters();

    /// \brief Compare current A_q precision with a set precision.
    Bool_t IsAqPrecisionGood();

    /// \brief Check to see no.of good patterns accumulated after the last feedback is greater than a set value
    Bool_t IsPatternsAccumulated();

    /// \brief Returns the type of the last helicity pattern based on following pattern history
    ///1. +--+ +--+
    ///2. +--+ -++-
    ///3. -++- +--+
    ///4. -++- -++-

    Int_t GetLastGoodHelicityPatternType();

 private:
    /// \brief Returns the charge asymmetry stats when required by feedback caluculations.
    Double_t GetChargeAsym(){
      return fChargeAsymmetry;
    };
    Double_t GetChargeAsymError(){
      return fChargeAsymmetryError;
    };
    Double_t GetChargeAsymWidth(){
      return fChargeAsymmetryWidth;
    };

    // Must call HelPat::GetTargetChargeStat(Double_t & asym, Double_t & error, Double_t & width) to update these values.
    Double_t fChargeAsymmetry;//current charge asym
    Double_t fChargeAsymmetryError;//current charge asym precision
    Double_t fChargeAsymmetryWidth;//current charge asym width

    Int_t fAccumulatePatternMax; //upper limit to the patterns before the feedback triiger;
    Int_t fChargeAsymPrecision; //Charge asymmetry precision in ppm

    //Keep a record of optimal values for IA, PC POS and NEG
    Double_t fOptimalIA;
    Double_t fOptimalPCP;
    Double_t fOptimalPCN;

    ///  Create an EPICS control event
    QwEPICSControl fEPICSCtrl;
    GreenMonster   fScanCtrl;

    //Pattern counter
    Int_t fGoodPatternCounter;

    // Keep four VQWK channels, one each for pattern history 1, 2, 3, and 4
    // Use AddToRunningSum to add the asymmetry for the current pattern
    // into the proper pattern history runnign sum.

  
};

#endif
