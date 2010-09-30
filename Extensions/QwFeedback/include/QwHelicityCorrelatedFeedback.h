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

    //Currently pattern type based runningasymmetry accumulation works only with pattern size of 4
    for (Int_t i=0;i<4;i++){
      fFBRunningAsymmetry[i].Copy(&event);
      fHelModeGoodPatternCounter[i]=0;
    }

    fEnableBurstSum=kFALSE;
    fGoodPatternCounter=0;
    

    fPreviousHelPat=0;//at the beginning of the run this is non existing
    fCurrentHelPatMode=0;//at the beginning of the run this is non existing


    fTargetCharge.InitializeChannel("q_targ","derived");
    fChargeAsymmetry0.InitializeChannel("q_targ","derived");//this is the charge asym at the beginning of the feedback loop
    fPreviousChargeAsymmetry.InitializeChannel("q_targ","derived");//charge asymmetry at the previous feedback loop
    fCurrentChargeAsymmetry.InitializeChannel("q_targ","derived");//current charge asymmetry 

    fIAAsymmetry0.InitializeChannel("q_targ","derived");//this is the charge asymmetry of the IA at the beginning of the feedback loop
    fPreviousIAAsymmetry.InitializeChannel("q_targ","derived");//this is the charge asymmetry of the IA at the previous feedback loop
    fCurrentIAAsymmetry.InitializeChannel("q_targ","derived");//current charge asymmetry of the IA

    out_file = fopen("Feedback_log.txt", "wt");
    fclose(out_file);
  
  };
    
    
    ~QwHelicityCorrelatedFeedback() { };  
    ///inherited from QwHelicityPattern
    void CalculateAsymmetry();
    void ClearRunningSum();
    void AccumulateRunningSum();
    void CalculateRunningAverage();

    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);

    /// \brief Load deltaA_q, no.of good patterns to accumulate and other neccessary feedback parameters
    void LoadParameterFile(TString filename);

    /// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width
    void GetTargetChargeStat();


 

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
    Bool_t IsPatternsAccumulated(Int_t mode);

    /// \brief Returns the type of the last helicity pattern based on following pattern history
    ///1. +--+ +--+
    ///2. +--+ -++-
    ///3. -++- +--+
    ///4. -++- -++-

    Int_t GetLastGoodHelicityPatternType();


    void CalculateRunningAverage(Int_t mode);
    void ClearRunningSum(Int_t mode);

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

    //Define separate running sums for differents helicity pattern modes
    /// \brief The types of helicity patterns based on following pattern history
    ///1. +--+ +--+
    ///2. +--+ -++-
    ///3. -++- +--+
    ///4. -++- -++-
 
    QwSubsystemArrayParity fFBRunningAsymmetry[4];
    Int_t fCurrentHelPat;
    Int_t fPreviousHelPat;

    Int_t fCurrentHelPatMode;

    static const Int_t kHelPat1=1001;//to compare with current or previous helpat
    static const Int_t kHelPat2=110;

    // Must call HelPat::GetTargetChargeStat(Double_t & asym, Double_t & error, Double_t & width) to update these values.
    Double_t fChargeAsymmetry;//current charge asym
    Double_t fChargeAsymmetryError;//current charge asym precision
    Double_t fChargeAsymmetryWidth;//current charge asym width

    Int_t fAccumulatePatternMax; //upper limit to the patterns before the feedback triiger;
    Double_t  fChargeAsymPrecision; //Charge asymmetry precision in ppm

    //Keep a record of optimal values for IA, PC POS and NEG
    Double_t fOptimalIA;
    Double_t fOptimalPCP;
    Double_t fOptimalPCN;
    //current IA and PC values
    Double_t fCurrentIA;
    Double_t fCurrentPCP;
    Double_t fCurrentPCN;

    //IA Slopes for 4 modes and their errors
    Double_t fIASlopeA[4];
    Double_t fDelta_IASlopeA[4];
    
    
    

    ///  Create an EPICS control event
    QwEPICSControl fEPICSCtrl;
    GreenMonster   fScanCtrl;

    //Pattern counter
    Int_t fGoodPatternCounter;//increment the quartet number - reset after each feedback operation
    Int_t fHelModeGoodPatternCounter[4];//count patterns for each mode seperately - reset after each feedback operation

    // Keep four VQWK channels, one each for pattern history 1, 2, 3, and 4
    // Use AddToRunningSum to add the asymmetry for the current pattern
    // into the proper pattern history runnign sum.

    QwBeamCharge   fTargetCharge;
    QwBeamCharge   fChargeAsymmetry0;//this is the charge asym at the beginning of the feedback loop
    QwBeamCharge   fPreviousChargeAsymmetry;//charge asymmetry at the previous feedback loop
    QwBeamCharge   fCurrentChargeAsymmetry;//current charge asymmetry 

    QwBeamCharge   fIAAsymmetry0;//this is the charge asymmetry of the IA at the beginning of the feedback loop
    QwBeamCharge   fPreviousIAAsymmetry;//this is the charge asymmetry of the IA at the previous feedback loop
    QwBeamCharge   fCurrentIAAsymmetry;//current charge asymmetry of the IA

    //log file
    FILE *out_file;
  
};

#endif
