/**********************************************************\
* File: QwMainCerenkovDetector.h                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

///
/// \ingroup QwAnalysis_ADC

#ifndef __QWMAINCERENKOVDETECTOR__
#define __QWMAINCERENKOVDETECTOR__

#include <vector>
#include <TTree.h>

#include "VQwSubsystemParity.h"
#include "QwVQWK_Module.h"

#include "QwIntegrationPMT.h"
#include "QwCombinedPMT.h"

#include "QwTypes.h"

#define MYSQLPP_SSQLS_NO_STATICS
#include "QwSSQLS.h"

#include "QwBlinder.h"

class QwMainCerenkovDetectorID;

enum EMainDetInstrumentType{IntegrationPMT, CombinedPMT};

class QwMainCerenkovDetector: public VQwSubsystemParity {
  /******************************************************************
   *  Class: QwMainCerenkovDetector
   *
   *
   ******************************************************************/
 public:
  QwMainCerenkovDetector(TString region_tmp):VQwSubsystem(region_tmp),VQwSubsystemParity(region_tmp) {
      // these declaration need to be coherent with the enum vector ELumiInstrumentType
      DetectorTypes.push_back("IntegrationPMT");
      DetectorTypes.push_back("CombinationPMT");
      for(size_t i=0;i<DetectorTypes.size();i++)
        DetectorTypes[i].ToLower();
      fTargetCharge.InitializeChannel("q_targ","derived");
      fTargetX.InitializeChannel("x_targ","derived");
      fTargetY.InitializeChannel("y_targ","derived");
      fTargetXprime.InitializeChannel("xp_targ","derived");
      fTargetYprime.InitializeChannel("yp_targ","derived");
      fTargetEnergy.InitializeChannel("e_targ","derived");
 };

  ~QwMainCerenkovDetector() {
    DeleteHistograms();
  };

  /*  Member functions derived from VQwSubsystemParity. */
  void ProcessOptions(QwOptions &options);//Handle command line options
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t LoadEventCuts(TString  filename);
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings 
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure
  Int_t GetEventcutErrorFlag();//return the error flag 
  
  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ClearEventData();
  Bool_t IsGoodEvent();

  void  ProcessEvent();
  void  ExchangeProcessedData();
  void  ProcessEvent_2();


  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity = 0);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ConstructHistograms(TDirectory *folder){
    TString tmpstr("");
    ConstructHistograms(folder,tmpstr);
  };

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  FillDB(QwDatabase *db, TString datatype);

  QwIntegrationPMT* GetChannel(const TString name);

  void Copy(VQwSubsystem *source);
  VQwSubsystem*  Copy();
  Bool_t Compare(VQwSubsystem *source);
  void Scale(Double_t factor);
  void BlindMe(QwBlinder *blinder);

  VQwSubsystem&  operator=  ( VQwSubsystem *value);
  VQwSubsystem&  operator+= ( VQwSubsystem *value);
  VQwSubsystem&  operator-= ( VQwSubsystem *value);


  void Sum(VQwSubsystem *value1, VQwSubsystem *value2);
  void Difference(VQwSubsystem *value1,VQwSubsystem *value2);
  void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
  void Calculate_Running_Average();
  void Do_RunningSum();

  QwIntegrationPMT* GetIntegrationPMT(const TString name);

  void DoNormalization(Double_t factor=1.0);

  Bool_t ApplyHWChecks(){//Check for harware errors in the devices
    Bool_t status = kTRUE;
    for (size_t i=0; i<fIntegrationPMT.size(); i++){
      status &= fIntegrationPMT.at(i).ApplyHWChecks();
    }
    return status;
  };

  void Print();
  void  PrintDetectorID();


 protected:
  Bool_t fDEBUG;

 Int_t GetDetectorTypeID(TString name);

 // when the type and the name is passed the detector index from appropriate vector
 // will be returned. For example if TypeID is IntegrationPMT  then the index of
 // the detector from fIntegrationPMT vector for given name will be returnd.
 Int_t GetDetectorIndex(Int_t TypeID, TString name);

  //std::vector<QwVQWK_Module> fADC_Data;

  std::vector <QwIntegrationPMT> fIntegrationPMT;
  std::vector <QwCombinedPMT> fCombinedPMT;
  std::vector <QwMainCerenkovDetectorID> fMainDetID;

 protected:
  QwBeamCharge   fTargetCharge;
  QwBeamPosition fTargetX;
  QwBeamPosition fTargetY;
  QwBeamAngle    fTargetXprime;
  QwBeamAngle    fTargetYprime;
  QwBeamEnergy   fTargetEnergy;

  Bool_t bIsExchangedDataValid;

 private:

  static const Bool_t bDEBUG=kFALSE; 
  static const Bool_t bNormalization=kTRUE;
  std::vector<TString> DetectorTypes;// for example could be IntegrationPMT, LUMI,BPMSTRIPLINE, etc..
  Int_t fMainDetErrorCount;

};


class QwMainCerenkovDetectorID
{
 public:
  QwMainCerenkovDetectorID():fSubbankIndex(-1),fWordInSubbank(-1),fTypeID(-1),fIndex(-1),
    fSubelement(999999),fmoduletype(""),fdetectorname("")
    {};

  int fSubbankIndex;
  int fWordInSubbank; //first word reported for this channel in the subbank
                      //(eg VQWK channel report 6 words for each event, scalers oly report one word per event)
  // The first word of the subbank gets fWordInSubbank=0

  int fTypeID;     // type of detector
  int fIndex;      // index of this detector in the vector containing all the detector of same type
  UInt_t fSubelement; // some detectors have many subelements (eg stripline have 4 antenas)
                      // some have only one sub element(eg lumis have one channel)

  TString fmoduletype; // eg: VQWK, SCALER
  TString fdetectorname;
  TString fdetectortype; // stripline, IntegrationPMT, ... this string is encoded by fTypeID

  std::vector<TString> fCombinedChannelNames;
  std::vector<Double_t> fWeight;

  void Print();

};


#endif













