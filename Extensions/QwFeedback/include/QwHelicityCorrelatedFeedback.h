/**********************************************************\
* File: QwHelicityCorrelatedFeedback.h                     *
*                                                          *
* Author: Rakitha Beminiwattha                             *
* Time-stamp: <2010-09-28>                                 *
\**********************************************************/
#ifndef __QwHelicityCorrelatedFeedback__
#define __QwHelicityCorrelatedFeedback__

#include "QwHelicityPattern.h"


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
    
  };
    
    
    ~QwHelicityCorrelatedFeedback() { };  

    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);

    /// \brief Load deltaA_q, no.of good patterns to accumulate and other neccessary feedback parameters
    void LoadParameterFile(TString filename);



 protected:

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


  
};

#endif
