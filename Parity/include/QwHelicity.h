/**********************************************************\
* File: QwHelicity.h                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwHELICITY__
#define __QwHELICITY__

// System headers
#include <vector>

// ROOT headers
#include "TTree.h"

// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwWord.h"

// Forward declarations
class QwDatabase;

enum HelicityRootSavingType{kHelSaveMPS = 0,
			    kHelSavePattern,
                            kHelNoSave};
// this emun vector needs to be coherent with the DetectorTypes declaration in the QwBeamLine constructor



/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwHelicity: public VQwSubsystemParity{
  /////
 public:


 QwHelicity(TString region_tmp): VQwSubsystem(region_tmp),
    VQwSubsystemParity(region_tmp),
    fMinPatternPhase(1), fUsePredictor(kTRUE), fIgnoreHelicity(kFALSE)
  {
    // Default helicity delay to two patterns.
    fHelicityDelay = 2;
    // Default helicity bit pattern
    fHelicityPattern = 0x69; // this is +--+-++-
    // Default the EventType flags to HelPlus=1 and HelMinus=4
    // These are only used in Moller decoding mode.
    kEventTypeHelPlus  = 4;
    kEventTypeHelMinus = 1;
    //
    fEventNumberOld=-1; fEventNumber=-1;
    fPatternPhaseNumberOld=-1; fPatternPhaseNumber=-1;
    fPatternNumberOld=-1;  fPatternNumber=-1;
    kUserbit=-1;
    fActualPatternPolarity=kUndefinedHelicity;
    fDelayedPatternPolarity=kUndefinedHelicity;
    fHelicityReported=kUndefinedHelicity;
    fHelicityActual=kUndefinedHelicity;
    fHelicityDelayed=kUndefinedHelicity;
    fHelicityBitPlus=kFALSE;
    fHelicityBitMinus=kFALSE;
    fGoodHelicity=kFALSE;
    fGoodPattern=kFALSE;
    fHelicityDecodingMode=-1;
  };

  virtual ~QwHelicity() {
    DeleteHistograms();
  };



  /* derived from VQwSubsystem */
  /// \brief Define options function

  static void DefineOptions(QwOptions &options);
  void ProcessOptions(QwOptions &options);
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t LoadEventCuts(TString  filename);//Loads event cuts applicabale to QwHelicity class, derived from VQwSubsystemParity
  Bool_t ApplySingleEventCuts();//Apply event cuts in the QwHelicity class, derived from VQwSubsystemParity
  Int_t  GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure, derived from VQwSubsystemParity
  UInt_t  GetEventcutErrorFlag();//return the error flag

  Int_t  ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id,
				   UInt_t* buffer, UInt_t num_words);
  Int_t  ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words){return ProcessEvBuffer(0,roc_id,bank_id,buffer,num_words);};
  Int_t  ProcessEvBuffer(UInt_t ev_type, const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void   ProcessEventUserbitMode();//ProcessEvent has two modes Userbit and Inputregister modes
  void   ProcessEventInputRegisterMode();
  void   ProcessEventInputMollerMode();

  void   EncodeEventData(std::vector<UInt_t> &buffer);

  void  PrintDetectorID() const;


  virtual void  ClearEventData();
  virtual void  ProcessEvent();

  UInt_t GetRandomSeedActual() { return iseed_Actual; };
  UInt_t GetRandomSeedDelayed() { return iseed_Delayed; };

  void   PredictHelicity();
  void   RunPredictor();
  void   SetHelicityDelay(Int_t delay);
  void   SetHelicityPattern(UInt_t bits);

  Int_t  GetHelicityReported();
  Int_t  GetHelicityActual();
  Int_t  GetHelicityDelayed();
  Long_t GetEventNumber();
  Long_t GetPatternNumber();
  Int_t  GetPhaseNumber();
  Int_t GetMaxPatternPhase(){
    return fMaxPatternPhase;
  };
  Int_t GetMinPatternPhase(){
    return fMinPatternPhase;
  }
  void SetFirstBits(UInt_t nbits, UInt_t firstbits);
  void SetEventPatternPhase(Int_t event, Int_t pattern, Int_t phase);

  void  Copy(VQwSubsystem *source);
  virtual VQwSubsystem*  Copy();
  VQwSubsystem&  operator=  (VQwSubsystem *value);
  VQwSubsystem&  operator+=  (VQwSubsystem *value);
  void  Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);

  //the following functions do nothing really : adding and sutracting helicity doesn't mean anything
  VQwSubsystem& operator-= (VQwSubsystem *value) {return *this;};
  void  Scale(Double_t factor) {return;};
  void  Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void  Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
  // end of "empty" functions

  void  AccumulateRunningSum(VQwSubsystem* value) { };
  void  CalculateRunningAverage() { };

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();
  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& trim_file);
  void  FillTreeVector(std::vector<Double_t> &values) const;
  void  FillDB(QwDatabase *db, TString type);
  void  Print() const;

  Bool_t IsHelicityIgnored(){return fIgnoreHelicity;};

  virtual Bool_t IsGoodHelicity();

/////
 protected:
  Bool_t CheckIORegisterMask(const UInt_t& ioregister, const UInt_t& mask) const {
    return ((ioregister & mask) == mask);
  };

 protected:
  enum HelicityRootSavingType{kHelSaveMPS = 0,
			      kHelSavePattern,
			      kHelNoSave};

  enum HelicityEncodingType{kHelUserbitMode=0,
			    kHelInputRegisterMode,
			    kHelLocalyMadeUp,
			    kHelInputMollerMode};
  // this values allow to switch the code between different helicity encoding mode.

  enum InputRegisterBits{kInputReg_HelPlus     = 0x1,
			 kInputReg_HelMinus    = 0x2,
			 kInputReg_PatternSync = 0x4,
			 kInputReg_FakeMPS     = 0x8000};

  std::vector <QwWord> fWord;
  std::vector < std::pair<Int_t, Int_t> > fWordsPerSubbank;  // The indices of the first & last word in each subbank

  Int_t fHelicityDecodingMode;
  // this variable is set at initialization in function QwHelicity::LoadChannelMap
  // it allows one to customize the helicity decoding mode
  // the helicity decoding mode will take one of the value of enum  HelicityEncodingType



  Int_t kUserbit;
  // this is used to tagged the userbit info among all the fWords
  // if we run the local helicity mode, the userbit contains the info
  // about helicity, event number, pattern number etc.
  Int_t kScalerCounter;
  // again this is used in the case we are running the local helicity mode
  // the scalercounter counts how many events happened since the last reading
  // should be one all the time if not the event is suspicious and not used for analysis
  Int_t kInputRegister, kPatternCounter, kMpsCounter, kPatternPhase;

  UInt_t kEventTypeHelPlus, kEventTypeHelMinus;

  Int_t fEventNumberOld, fEventNumber;
  Int_t fPatternPhaseNumberOld, fPatternPhaseNumber;
  Int_t fPatternNumberOld, fPatternNumber;
  Int_t fPatternSeed;
  Int_t fActualPatternPolarity;   ///<  True polarity of the current pattern
  Int_t fDelayedPatternPolarity;  ///<  Reported polarity of the current pattern
  Int_t fPreviousPatternPolarity; ///<  True polarity of the previous pattern.
  Int_t fHelicityReported, fHelicityActual, fHelicityDelayed;
  // reported is what is registered in the coda file (it is the actual beam helicity fHelicityDelay pattern before this event)
  // actual is the helicity of the beam for this event
  // delayed is the expected reported helicity predicted by the random generator
  // std::vector <Int_t> fCheckHelicityDelay;//  this is obsolete
  //this array keeps in memory the Actual helicity up to when it can be compared to the reported helicity
  Bool_t fHelicityBitPlus;
  Bool_t fHelicityBitMinus;
  Bool_t fGoodHelicity;
  Bool_t fGoodPattern;

  UInt_t fHelicityPattern;

  std::vector<TH1*> fHistograms;
  Int_t fHistoType;
  //allow one to select which types of histograms are created and filled
  void SetHistoTreeSave(const TString &prefix);



  static const Bool_t kDEBUG=kFALSE;
  // local helicity is a special mode for encoding helicity info
  // it is not the fullblown helicity encoding we want to use for the main
  // data taking. For example this was used during the injector data taking
  // in winter 2008-09 injector tests
  static const Int_t kUndefinedHelicity= -9999;


 /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;
  UInt_t n_ranbits; //counts how many ranbits we have collected
  UInt_t iseed_Actual; //stores the random seed for the helicity predictor
  UInt_t iseed_Delayed;
  //stores the random seed to predict the reported helcity
  Int_t fHelicityDelay;
  //number of events the helicity is delayed by before being reported
  //static const Int_t MaxPatternPhase =4;
  Int_t fMaxPatternPhase;
  Int_t fMinPatternPhase;
  Bool_t IsGoodPatternNumber();
  Bool_t IsGoodEventNumber();
  Bool_t MatchActualHelicity(Int_t actual);
  Bool_t IsGoodPhaseNumber();
  Bool_t IsContinuous();

  virtual UInt_t GetRandbit(UInt_t& ranseed);
  UInt_t GetRandbit24(UInt_t& ranseed);//for 24bit pattern
  UInt_t GetRandbit30(UInt_t& ranseed);//for 30bit pattern
  UInt_t GetRandomSeed(UShort_t* first24randbits);
  virtual Bool_t CollectRandBits();
  Bool_t CollectRandBits24();//for 24bit pattern
  Bool_t CollectRandBits30();//for 30bit pattern


  void   ResetPredictor();

  Bool_t Compare(VQwSubsystem *source);

  Bool_t BIT24;//sets the random seed size 24bit/30bits
  Bool_t BIT30;
  Bool_t fUsePredictor;
  Bool_t fHelicityInfoOK;
  Int_t  fPATTERNPHASEOFFSET;

  Bool_t fIgnoreHelicity;

  UInt_t fEventType;


 private:

  unsigned int parity(unsigned int v) {
    // http://graphics.stanford.edu/~seander/bithacks.html#ParityParallel
    v ^= v >> 16;
    v ^= v >> 8;
    v ^= v >> 4;
    v &= 0xf;
    return (0x6996 >> v) & 1;
  }

};


#endif


