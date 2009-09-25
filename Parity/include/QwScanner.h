/**********************************************************\
* File: QwScanner.h                              *
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
#include "TRandom3.h"

#include "VQwSubsystemTracking.h"
#include "VQwSubsystemParity.h"

#include "MQwV775TDC.h"
#include "QwVQWK_Module.h"
#include "MQwSIS3801_Module.h"

#include "VQwDataElement.h"
#include "QwVQWK_Channel.h"
#include "MQwSIS3801_Channel.h"
#include "QwPMT_Channel.h"

class QwVQWK_Channel;
class MQwSIS3801_Channel;

class QwScanner: public VQwSubsystem,
                 public MQwV775TDC,
                 public MQwSIS3801_Module,
                 public QwVQWK_Module {

 public:
  QwScanner(TString region_tmp);
  ~QwScanner();

  /*  Member functions derived from VQwSubsystem. */
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadQweakGeometry(TString mapfile){return 0;};
  Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detector_info){return 0;};
  Int_t LoadInputParameters(TString mapfile);
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
  void  FillTreeVector(std::vector<Double_t> &values, TString &prefix);
  void GetHitList(QwHitContainer & grandHitContainer){};
  void ReportConfiguration();

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

  void Print();

 protected:

  enum EModuleType{EMPTY = -1, V775_TDC = 0, V792_ADC} fCurrentType;
  Bool_t fDEBUG;

  //    We need a mapping of module,channel into PMT index, ADC/TDC
  std::vector< std::vector<QwPMT_Channel> > fPMTs;
  std::vector<MQwSIS3801_Module*> fSCAs;
  std::vector<QwVQWK_Module*> fADC_Data;

  void FillRawWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
  void  ClearAllBankRegistrations();
  Int_t RegisterROCNumber(const UInt_t roc_id);
  Int_t RegisterSubbank(const UInt_t bank_id);

  // Tells this object that it will decode data from the current bank
  Int_t RegisterSlotNumber(const UInt_t slot_id);
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
  std::vector< std::vector< std::pair<Int_t, Int_t> > > fModulePtrs; // Indexed by Module_index and Channel; gives the plane and wire assignment.


  // scanner specified histograms
  std::vector<TH1*> fHistograms1D;
  std::vector<TH2*> fHistograms2D;

 private:

  Double_t fCurrentPotentialX;
  Double_t fCurrentPotentialY;
  Double_t fDirectionX;
  Double_t fDirectionY;
  Double_t PreDirectionX;
  Double_t PreDirectionY;

  Double_t fPedestal;
  Double_t fCalibration;

  Double_t fPositionX;
  Double_t fPositionY;
  Double_t fRate;

  Double_t MainDetCenterX;
  Double_t MainDetCenterY;

  Double_t HomePositionOffsetX;
  Double_t HomePositionOffsetY;

  Double_t fVoltageOffsetX;
  Double_t fVoltageOffsetY;

  Double_t Cal_FactorX;
  Double_t Cal_FactorY;

  Int_t myTimer;
  Int_t HelicityChanged;

  Int_t FrontScaData;
  Int_t BackScaData;
  Int_t CoincidenceScaData;
};

#endif

