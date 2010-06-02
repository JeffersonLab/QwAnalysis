/**********************************************************\
* File: QwFakeHelicity.h                                      *
*                                                         *
* Author:B.Waidyawansa                                    *
* Time-stamp:01/06/2010                                   *
\**********************************************************/

#ifndef __QwFAKEHELICITY__
#define __QwFAKEHELICITY__

#include <vector>
#include "TTree.h"

#include "VQwSubsystemParity.h"
#include "QwOptions.h"

enum FakeHelicityRootSavingType{kHelSaveMPS = 0,
			    kHelSavePattern,
                            kHelNoSave};
// this emun vector needs to be coherent with the DetectorTypes declaration in the QwBeamLine constructor


///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwWord
{
 public:
 QwWord():fSubbankIndex(-1),fWordInSubbank(-1),fModuleType(""),
	  fWordName(""),fWordType(""),fValue(-1){};

  Int_t fSubbankIndex;
  UInt_t fWordInSubbank;
  TString fModuleType;
  TString fWordName;
  TString fWordType;
  Int_t fValue;

  void PrintID()
    {
      std::cout<<fWordName<<" :: "<<fSubbankIndex<<" ; "
	       <<fModuleType<<" ; "<<fWordType<<"\n";
    }

  void Print()
    {
      std::cout<<fWordName<<" :: "<<fValue<<"\n";
    }

  void ClearEventData()
    {
      fValue=-1;
    }

};

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwFakeHelicity : public VQwSubsystemParity{
  /////
 public:

  QwFakeHelicity(TString region_tmp):VQwSubsystem(region_tmp),VQwSubsystemParity(region_tmp),fMinPatternPhase(1)
    {

      fEventNumberOld=-1; fEventNumber=-1;
      fPatternPhaseNumberOld=-1; fPatternPhaseNumber=0;
      fPatternNumberOld=-1;  fPatternNumber=-1;
      kuserbit=-1;
      fFakePatternPolarity=kUndefinedFakeHelicity;
      fFakeHelicity=kUndefinedFakeHelicity;
      fFakeHelicityBitPlus=kFALSE;
      fFakeHelicityBitMinus=kFALSE;
      fGoodFakeHelicity=kFALSE;
      fGoodPattern=kFALSE;
      fFakeHelicityDecodingMode=-1;
    };

  ~QwFakeHelicity()
    {
//      DeleteHistograms();
    };
   /* derived from VQwSubsystem */
  Int_t  LoadChannelMap(TString mapfile);
  Int_t  LoadInputParameters(TString pedestalfile);
  Int_t  LoadEventCuts(TString  filename);//Loads event cuts applicabale to QwFakeHelicity class, derived from VQwSubsystemParity
  Bool_t ApplySingleEventCuts();//Apply event cuts in the QwFakeHelicity class, derived from VQwSubsystemParity
  Int_t  GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure, derived from VQwSubsystemParity
  Int_t  GetEventcutErrorFlag();//return the error flag 

  Int_t  ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id,
				   UInt_t* buffer, UInt_t num_words);
  Int_t  ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void   ProcessEventInputRegisterMode();
  void   ProcessEvent(); //fake the helicity inside the code


  void   PrintDetectorID();

  void   ClearEventData();
  void   EncodeEventData(std::vector<UInt_t> &buffer);

  void  SetFirst24Bits(UInt_t first24bits);
  void  SetEventPatternPhase(Int_t event, Int_t pattern, Int_t phase);


  void  PredictFakeHelicity();
  void  RunPredictor();

  void  SetGoodFakeHelicityDelay(Int_t delay);

  Int_t GetHelicityActual();
  Long_t GetEventNumber();
  Long_t GetPatternNumber();
  Int_t GetPhaseNumber();

  void Copy(VQwSubsystem *source);
  VQwSubsystemParity*  Copy();
  VQwSubsystem&  operator=  (VQwSubsystem *value);
  VQwSubsystem&  operator+=  (VQwSubsystem *value);
  void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);

  //the following functions do nothing really : adding and sutracting helicity doesn't mean anything
  VQwSubsystem&  operator-=  (VQwSubsystem *value){return *this;};
  void Scale(Double_t factor){return;};
  void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
  void Calculate_Running_Average();
  void Do_RunningSum();
  // end of "empty" functions

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();
  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  SetFakeHelicityDelay(Int_t delay);
  void Print();

  Bool_t IsGoodFakeHelicity();
  Int_t GetMaxPatternPhase(){
    return fMaxPatternPhase;
  };
  
/////
 protected:
   enum HelicityRootSavingType{kHelSaveMPS = 0,
			      kHelSavePattern,
			      kHelNoSave};
   
  enum HelicityEncodingType{kHelUserbitMode=0,
			    kHelInputRegisterMode,
			    kHelLocalyMadeUp};
  // this values allow to switch the code between different helicity encoding mode. 
  
  std::vector <QwWord> fWord;
  std::vector < std::pair<Int_t, Int_t> > fWordsPerSubbank;  // The indices of the first & last word in each subbank

  Int_t fFakeHelicityDecodingMode; 
  // this variable is set at initialization in function QwFakeHelicity::LoadChannelMap
  // it allows one to customize the helicity decoding mode
  // the helicity decoding mode will take one of the value of enum  FakeHelicityEncodingType

  

  Int_t kuserbit;
  // this is used to tagged the userbit info among all the fWords
  // if we run the local helicity mode, the userbit contains the info
  // about helicity, event number, pattern number etc.
  Int_t kscalercounter;
  // again this is used in the case we are running the local helicity mode
  // the scalercounter counts how many events happened since the last reading
  // should be one all the time if not the event is suspicious and not used for analysis
  Int_t kinputregister, kpatterncounter, kmpscounter, kpatternphase;
  Int_t fEventNumberOld, fEventNumber;
  Int_t fPatternPhaseNumberOld, fPatternPhaseNumber;
  Int_t fPatternNumberOld,  fPatternNumber;
  Int_t fFakePatternPolarity;
  Int_t fFakeHelicity;
  // reported is what is registered in the coda file (it is the actual beam helicity fFakeHelicityDelay pattern before this event)
  // actual is the helicity of the beam for this event
  // delayed is the expected reported helicity predicted by the random generator
  // std::vector <Int_t> fCheckFakeHelicityDelay;//  this is obsolete
  //this array keeps in memory the Actual helicity up to when it can be compared to the reported helicity
  Bool_t fFakeHelicityBitPlus;
  Bool_t fFakeHelicityBitMinus;
  Bool_t fGoodFakeHelicity;
  Bool_t fGoodPattern;

  std::vector<TH1*> fHistograms;
  Int_t fHistoType;
  //allow one to select which types of histograms are created and filled
  void SetHistoTreeSave(TString &prefix);



  static const Bool_t kDEBUG=kFALSE;
  static const Bool_t dolocalhelicity=kTRUE;//kFALSE;//temp word way to encode helicity
  // local helicity is a special mode for encoding helicity info
  // it is not the fullblown helicity encoding we want to use for the main
  // data taking. For example this was used during the injector data taking
  // in winter 2008-09 injector tests
  static const Bool_t dolocalhelicity2=kFALSE;//kTRUE;//full blown mode
  static const Int_t kUndefinedFakeHelicity= -9999;


 /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;
  UInt_t n_ranbits; //counts how many ranbits we have collected
  UInt_t iseed_Fake; //stores the random seed for the helicity predictor
  UInt_t iseed_Delayed;
  //stores the random seed to predict the reported helcity
  Int_t fFakeHelicityDelay;
  //number of events the helicity is delayed by before being reported
  static const Int_t fMaxPatternPhase =4;
  Int_t fMinPatternPhase;
  Bool_t IsGoodPatternNumber();
  Bool_t IsGoodEventNumber();
  Bool_t MatchActualFakeHelicity(Int_t actual);
  Bool_t IsGoodPhaseNumber();
  Bool_t IsContinuous();

  UInt_t GetRandbit(UInt_t& ranseed);
  UInt_t GetRandbit24(UInt_t& ranseed);//for 24bit pattern
  UInt_t GetRandbit30(UInt_t& ranseed);//for 30bit pattern
  UInt_t GetRandomSeed();
/*   Bool_t CollectRandBits24();//for 24bit pattern */
  Bool_t CollectRandBits();//for 30bit pattern
  Bool_t FakeSeed(); //fake the seed
  
  void   ResetPredictor();

  Bool_t Compare(VQwSubsystem *source);

  Bool_t BIT24;//sets the random seed size 24bit/30bits
  Bool_t BIT30;

  Int_t fPATTERNPHASEOFFSET;
  
};


//const Bool_t QwFakeHelicity::kDEBUG = kFALSE;
//const Bool_t QwFakeHelicity::dolocalhelicity= kTRUE;
//const Int_t QwFakeHelicity::MaxPatternPhase = 4;
//const Int_t QwFakeHelicity::kUndefinedFakeHelicity=-9999;

#endif


