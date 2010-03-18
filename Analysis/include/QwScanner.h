/**********************************************************\
* File: QwScanner.h                                        *
*                                                          *
* Author: J. Pan                                           *
* jpan@jlab.org                                            *
*                                                          *
* Thu May 21 21:20:41 CDT 2009                             *
\**********************************************************/

#ifndef __QWSCANNER__
#define __QWSCANNER__

#include <vector>

#include "TTree.h"
#include "TFile.h"
#include "TRandom3.h"

#include "VQwSubsystemTracking.h"
#include "VQwSubsystemParity.h"

#include "MQwV775TDC.h"
#include "QwVQWK_Module.h"
#include "QwSIS3801_Module.h"

#include "VQwDataElement.h"
#include "QwVQWK_Channel.h"
#include "QwSIS3801_Channel.h"
#include "QwPMT_Channel.h"

class QwScanner: public VQwSubsystemTracking,
                 public VQwSubsystemParity
{

 public:

   QwScanner(TString region_tmp);
   virtual ~QwScanner();

  // VQwSubsystem methods
    VQwSubsystem& operator=  (VQwSubsystem *value) { return *this; };
    VQwSubsystem& operator+= (VQwSubsystem *value) { return *this; };
    VQwSubsystem& operator-= (VQwSubsystem *value) { return *this; };
    void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2) { return; };
    void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2) { return; };
    void Ratio(VQwSubsystem *numer, VQwSubsystem *denom) { return; };
    void Scale(Double_t factor) { return; };
    void Calculate_Running_Average(){return;};
    void Do_RunningSum(){return;};//update the running sums for devices

    Int_t LoadEventCuts(TString filename) { return 0; };
    Bool_t ApplySingleEventCuts() { return kTRUE; };
    Int_t GetEventcutErrorCounters() { return 0; };
    Bool_t CheckRunningAverages(Bool_t ) { return kTRUE; };

    void Copy(VQwSubsystem *source) { VQwSubsystem::Copy(source); return; };
    VQwSubsystem*  Copy() { return this; };
    Bool_t Compare(VQwSubsystem *source) { return kTRUE; };

  /*  Member functions derived from VQwSubsystem. */
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadQweakGeometry(TString mapfile){return 0;};
  Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detector_info){return 0;};
  Int_t LoadInputParameters(TString parameterfile);
  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  InitializeChannel(TString name, TString datatosave);
  void  ClearEventData();
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetTriggerEventData(Double_t TrigEventValue);
  void  SetPositionEventData(Double_t* PositionEvBuf, UInt_t sequencenumber);
  void  SetPedestal(Double_t ped);
  void  SetCalibrationFactor(Double_t calib);
  void  RandomizeEventData(int helicity);
  void  EncodeEventData(std::vector<UInt_t> &buffer);
  Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  ProcessEvent();
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);

  void  FillTreeVector(std::vector<Double_t> &values);

  void  FillDB(QwDatabase *db, TString type) {return;};

  void  GetHitList(QwHitContainer & grandHitContainer){};
  void  ReportConfiguration();

  Bool_t Compare(QwScanner &value);
  QwScanner& operator=  (QwScanner &value);
  QwScanner& operator+=  (QwScanner &value);
  QwScanner& operator-=  (QwScanner &value);

  Bool_t ApplyHWChecks(){//Check for harware errors in the devices
    Bool_t status = kTRUE;
    for (size_t i=0; i<fADC_Data.size(); i++){
      if (fADC_Data.at(i) != NULL){
	status &= fADC_Data.at(i)->ApplyHWChecks();
      }
    }
    return status;
  };

  void StoreRawData(Bool_t rawdata) { bRawData = rawdata; };

  void Print();

 protected:

  enum EModuleType{EMPTY = -1, V775_TDC = 0, V792_ADC} fCurrentType;
  Bool_t fDEBUG;

  //    We need a mapping of module,channel into PMT index, ADC/TDC
  std::vector< std::vector<QwPMT_Channel> > fPMTs;
  std::vector<QwSIS3801_Module*> fSCAs;
  std::vector<QwVQWK_Module*> fADC_Data;

  void FillRawWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
  void  ClearAllBankRegistrations();
  Int_t RegisterROCNumber(const UInt_t roc_id);
  Int_t RegisterSubbank(const UInt_t bank_id);

  // Tells this object that it will decode data from the current bank
  Int_t RegisterSlotNumber(const UInt_t slot_id);
  void DecodeTDCWord(UInt_t &word);
  UInt_t GetTDCSlotNumber(){return fV775SlotNumber;};
  Bool_t IsValidDataword(){return fV775ValidFlag;};
  UInt_t GetTDCChannelNumber(){return fV775ChannelNumber;};
  UInt_t GetTDCData(){return fV775Dataword;};


  const QwScanner::EModuleType RegisterModuleType(TString moduletype);
  Int_t GetModuleIndex(size_t bank_index, size_t slot_num) const;
  Bool_t IsSlotRegistered(Int_t bank_index, Int_t slot_num) const {
    return (GetModuleIndex(bank_index,slot_num) != -1);
  };


  Int_t LinkChannelToSignal(const UInt_t chan, const TString &name);
  Int_t FindSignalIndex(const QwScanner::EModuleType modtype, const TString &name) const;
  TString fRegion;  ///  Name of this subsystem (the region).
  size_t fCurrentBankIndex;
  Int_t fCurrentSlot;
  Int_t fCurrentIndex;
  static const UInt_t kMaxNumberOfModulesPerROC;
  static const UInt_t kMaxNumberOfChannelsPerModule;
  Int_t fNumberOfModules;
  std::vector< std::vector<Int_t> > fModuleIndex;  //  Module index, indexed by bank_index and slot_number
  std::vector< enum EModuleType > fModuleTypes;
  std::vector< std::vector< std::pair<Int_t, Int_t> > > fModulePtrs; // Indexed by Module_index and Channel

  Int_t GetEventcutErrorFlag() { return 0; };//return the error flag to the main routine

  // scanner specified histograms
  std::vector<TH1*> fHistograms1D;
  std::vector<TH2*> fHistograms2D;

 private:
  Double_t get_value( TH2* h, Double_t x, Double_t y, Int_t& checkvalidity);

 private:

  static const UInt_t kV775Mask_SlotNumber;
  static const UInt_t kV775Mask_WordType;
  static const UInt_t kV775Mask_ChannelNumber;
  static const UInt_t kV775Mask_Dataword;
  static const UInt_t kV775WordType_Datum;

  Bool_t fV775ValidFlag;
  UInt_t fV775SlotNumber;
  UInt_t fV775ChannelNumber;
  UInt_t fV775Dataword;

  Double_t fMainDetCenterX;
  Double_t fMainDetCenterY;
  Double_t fHomePositionOffsetX;
  Double_t fHomePositionOffsetY;
  Double_t fVoltageOffsetX;
  Double_t fVoltageOffsetY;
  Double_t fCal_FactorX;
  Double_t fCal_FactorY;
  Double_t fPedestal;
  Double_t fCalibration;

  Int_t myTimer;
  Bool_t bRawData;
  Int_t fTreeArrayNumEntries;
  Int_t fTreeArrayIndex;

  std::vector <Double_t> fScannerVector;

  Double_t fCurrentPotentialX;
  Double_t fCurrentPotentialY;
  Double_t fDirectionX;
  Double_t fDirectionY;
  Double_t fPreDirectionX;
  Double_t fPreDirectionY;

  Double_t fPowSupply;
  Double_t fPositionX;
  Double_t fPositionY;
  Double_t fRate;

  Int_t FrontScaData;
  Int_t BackScaData;
  Int_t CoincidenceScaData;

  Int_t fEvtCounter;
  Int_t fTrigEvtCounter;
  Int_t fSumEvtCounter;
  Int_t fSumFlag;
  Int_t fTrigFlag;
  Double_t fFrontSCA;
  Double_t fBackSCA;
  Double_t fCoincidenceSCA;
  Double_t fFrontADC;
  Double_t fFrontTDC;
  Double_t fBackADC;
  Double_t fBackTDC;
};


#endif

