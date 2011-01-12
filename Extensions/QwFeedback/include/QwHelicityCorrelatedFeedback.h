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
#include "QwVQWK_Channel.h"
#include "QwParameterFile.h"
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
    for (Int_t i=0;i<kHelModes;i++){
      fFBRunningAsymmetry[i].Copy(&event);
      fHelModeGoodPatternCounter[i]=0;
    }

    fEnableBurstSum=kFALSE;
    fGoodPatternCounter=0;


/*     fFeedbackStatus=kTRUE; */
/*     if (fEPICSCtrl.Get_FeedbackStatus()>0) */
/*       fEPICSCtrl.Set_FeedbackStatus(0); */
/*     if (fFeedbackStatus){ */
/*       fFeedbackStatus=kFALSE; */
/*       fEPICSCtrl.Set_FeedbackStatus(1.0); */
/*     } */

    CheckFeedbackStatus();

    fPreviousHelPat=0;//at the beginning of the run this is non existing
    fCurrentHelPatMode=-1;//at the beginning of the run this is non existing


    fTargetCharge.InitializeChannel("q_targ","derived");
    fChargeAsymmetry0.InitializeChannel("q_targ","derived");//this is the charge asym at the beginning of the feedback loop
    fPreviousChargeAsymmetry.InitializeChannel("q_targ","derived");//charge asymmetry at the previous feedback loop
    fCurrentChargeAsymmetry.InitializeChannel("q_targ","derived");//current charge asymmetry 

    fIAAsymmetry0.InitializeChannel("q_targ","derived");//this is the charge asymmetry of the IA at the beginning of the feedback loop
    fPreviousIAAsymmetry.InitializeChannel("q_targ","derived");//this is the charge asymmetry of the IA at the previous feedback loop
    fCurrentIAAsymmetry.InitializeChannel("q_targ","derived");//current charge asymmetry of the IA

    //    out_file_IA = fopen("Feedback_IA_log.txt", "wt");
    out_file_IA = fopen("/local/scratch/qweak/Feedback_IA_log.txt", "wt");
    //out_file_IA = fopen("/dev/shm/Feedback_IA_log.txt", "a");    
    fprintf(out_file_IA,"Pat num. \t\t  A_q[mode]\t\t\t\t\t\t\t\t\t  IA Setpoint \t\t  IA Previous Setpoint \t\t\n");
    fclose(out_file_IA);
    //    out_file_PITA = fopen("Feedback_PITA_log.txt", "wt");
    out_file_PITA = fopen("/local/scratch/qweak/Feedback_PITA_log.txt", "wt");
    //out_file_PITA = fopen("/dev/shm/Feedback_PITA_log.txt", "a");     
    fprintf(out_file_PITA,"Pat num. \t\t  A_q\t\t\t\t\t\t\t\t\t  PITA Setpoint[+] \t\t  PITA Previous Setpoint \t\tPITA Setpoint[-] \t\t  PITA Previous Setpoint \n");
    fclose(out_file_PITA);
  
  };
    
    
    ~QwHelicityCorrelatedFeedback() { 
        if (!fFeedbackStatus){
	  fFeedbackStatus=kFALSE;
	  fEPICSCtrl.Set_FeedbackStatus(0);
	}

    };  
    ///inherited from QwHelicityPattern
    void CalculateAsymmetry();
    void ClearRunningSum();
    void AccumulateRunningSum();
    void CalculateRunningAverage();
    void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
    void  FillTreeVector(std::vector<Double_t> &values) const;

    /// \brief Define the configuration options
    static void DefineOptions(QwOptions &options);
    /// \brief Process the configuration options
    void ProcessOptions(QwOptions &options);

    /// \brief Load deltaA_q, no.of good patterns to accumulate and other neccessary feedback parameters
    void LoadParameterFile(TString filename);

    /// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width
    void GetTargetChargeStat();
    /// \brief retrieves the target charge asymmetry,asymmetry error ,asymmetry width for given mode
    void GetTargetChargeStat(Int_t mode);


 

    /// \brief Feed the Hall C IA set point based on the charge asymmetry 
    void FeedIASetPoint(Int_t mode);

    /// \brief Feed the Hall C PITA set point based on the charge asymmetry 
    void FeedPITASetPoints();

    /// \brief Feed the IA set point based on the charge asymmetry 
    void FeedPCPos();
    /// \brief Feed the IA set point based on the charge asymmetry 
    void FeedPCNeg();

    /// \brief Log the last IA  feedback information
    void LogParameters(Int_t mode);

    /// \brief Log the last PITA feedback information
    void LogParameters();

    /// \brief Set Clean=0 or 1 in the GreenMonster
    void UpdateGMClean(Int_t state);

    /// \brief Update last feedback setting into scan variables in the GreenMonster
    void UpdateGMScanParameters();

    /// \brief Compare current A_q precision with a set precision.
    Bool_t IsAqPrecisionGood();
    /// \brief Compare current A_q precision with a set precision for given mode.
    Bool_t IsAqPrecisionGood(Int_t mode);
    /// \brief Check to see no.of good patterns accumulated after the last feedback is greater than a set value 
    Bool_t IsPatternsAccumulated(){
      if (fGoodPatternCounter>=fAccumulatePatternMax)
	return kTRUE;

      return kFALSE;
    };
    /// \brief Check neccessary conditions and apply IA setponts based on the charge asym for all four modes
    void ApplyFeedbackCorrections();
    /// \brief Check to see no.of good patterns accumulated after the last feedback is greater than a set value for given mode
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

    Bool_t SetInitialCondition(){
      if (fPITASetpointPOS_t0>0 && fPITASetpointNEG_t0>0)//apply the t_0 correction if it is available
	fInitialCorrection=kTRUE;
      else
	fInitialCorrection=kFALSE;
      return fInitialCorrection;
    }

    //Define separate running sums for differents helicity pattern modes
    /// \brief The types of helicity patterns based on following pattern history
    ///1. +--+ +--+
    ///2. +--+ -++-
    ///3. -++- +--+
    ///4. -++- -++-
 
    
    TString GetHalfWavePlateState();
    void    CheckFeedbackStatus();

    static const Int_t kHelPat1=1001;//to compare with current or previous helpat
    static const Int_t kHelPat2=110;
    static const Int_t kHelModes=4;//kHelModes

    QwSubsystemArrayParity fFBRunningAsymmetry[kHelModes];
    Int_t fCurrentHelPat;
    Int_t fPreviousHelPat;

    Int_t fCurrentHelPatMode;
    
    Double_t fChargeAsymmetry;//current charge asym
    Double_t fChargeAsymmetryError;//current charge asym precision
    Double_t fChargeAsymmetryWidth;//current charge asym width

    Double_t fChargeAsym[kHelModes];//current charge asym
    Double_t fChargeAsymError[kHelModes];//current charge asym precision
    Double_t fChargeAsymWidth[kHelModes];//current charge asym width

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
    Double_t fIASlopeA[kHelModes];
    Double_t fDelta_IASlopeA[kHelModes];

    //IA setpoints for 4 modes 
    Double_t fIASetpoint[kHelModes];//New setpont calculated based on the charge asymmetry
    Double_t fPrevIASetpoint[kHelModes];//previous setpoint
    Double_t fIASetpointlow;//lower and upper limits for IA dac hardware counts
    Double_t fIASetpointup;

    //PITA Slopes for halfwave plate IN & OUT
    Double_t fPITASlopeIN;
    Double_t fPITASlopeOUT;
    Double_t fPITASlope;

    //PITA setpoints for pos hel and neg hel
    Double_t fPITASetpointPOS;
    Double_t fPITASetpointNEG;
    Double_t fPITASetpointPOS_t0;//Initial PC positive HW setpoint
    Double_t fPITASetpointNEG_t0;//Initial PC negative HW setpoint
    Bool_t fInitialCorrection;//Is true at the beginning so that t_0 correction is appiled before doing any correction
    
    
    Double_t fPrevPITASetpointPOS;//previous setpoint
    Double_t fPrevPITASetpointNEG;//previous setpoint
    Double_t fPITASetpointlow;//lower and upper limits for PITA dac hardware counts
    Double_t fPITASetpointup;

    
    
    ///  Create an EPICS control event
    QwEPICSControl fEPICSCtrl;
    GreenMonster   fScanCtrl;

    //Pattern counter
    Int_t fGoodPatternCounter;//increment the quartet number - reset after each feedback operation
    Int_t fHelModeGoodPatternCounter[kHelModes];//count patterns for each mode seperately - reset after each feedback operation

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
    FILE *out_file_PITA;
    FILE *out_file_IA;

    Bool_t fHalfWaveIN;
    Bool_t fHalfWaveOUT;

    Bool_t fHalfWaveRevert;

    TString fHalfWavePlateStatus;

    Bool_t fPITAFB;
    Bool_t fIAFB;
    Bool_t fFeedbackStatus;
  
};

#endif
