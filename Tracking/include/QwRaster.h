/**********************************************************\
* File: QwRaster.h                                        *
*                                                          *
* Author: J. Pan                                           *
* jpan@jlab.org                                            *
*                                                          *
* Tue Jun 29 00:16:14 CDT 2010                             *                                                   *
\**********************************************************/

#ifndef __QWRASTER__
#define __QWRASTER__

#include <vector>
#include "TTree.h"
#include "TFile.h"
#include "TProfile2D.h"
#include "VQwSubsystemTracking.h"
#include "MQwV775TDC.h"
#include "QwVQWK_Module.h"
#include "QwVQWK_Channel.h"
#include "QwPMT_Channel.h"
#include "QwDatabase.h"
#include "QwHistogramHelper.h"

class QwRaster: public VQwSubsystemTracking
{

 public:

   QwRaster(TString region_tmp);
   virtual ~QwRaster();

    // VQwSubsystem methods
    void ProcessOptions(QwOptions &options); //Handle command line options

    Int_t LoadEventCuts(TString filename) { return 0; };
    Bool_t ApplySingleEventCuts() { return kTRUE; };
    Int_t GetEventcutErrorCounters() { return 0; };
    Bool_t CheckRunningAverages(Bool_t ) { return kTRUE; };

  /*  Member functions derived from VQwSubsystem. */
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadGeometryDefinition(TString mapfile){return 0;};
  Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detector_info){return 0;};
  Int_t LoadInputParameters(TString parameterfile);
  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  InitializeChannel(TString name, TString datatosave);
  void  ClearEventData();

  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  ProcessEvent();
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);

  void  FillDB(QwDatabase *db, TString type) {return;};

  void  GetHitList(QwHitContainer & grandHitContainer){};
  void  ReportConfiguration();

  void PrintValue() { };
  void PrintInfo();

 protected:

  enum EModuleType{EMPTY = -1, V775_TDC = 0, V792_ADC} fCurrentType;
  Bool_t fDEBUG;

  MQwV775TDC fQDCTDC;

  //    We need a mapping of module,channel into PMT index, ADC/TDC
  std::vector< std::vector<QwPMT_Channel> > fPMTs;  // for QDC/TDC and F1TDC
  std::vector<QwVQWK_Module*> fADC_Data;

  void FillRawWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
  void  ClearAllBankRegistrations();
  Int_t RegisterROCNumber(const UInt_t roc_id);
  Int_t RegisterSubbank(const UInt_t bank_id);

  // Tells this object that it will decode data from the current bank
  Int_t RegisterSlotNumber(const UInt_t slot_id);
  const QwRaster::EModuleType RegisterModuleType(TString moduletype);
  Int_t GetModuleIndex(size_t bank_index, size_t slot_num) const;
  Bool_t IsSlotRegistered(Int_t bank_index, Int_t slot_num) const {
    return (GetModuleIndex(bank_index,slot_num) != -1);
  };

  Int_t LinkChannelToSignal(const UInt_t chan, const TString &name);
  Int_t FindSignalIndex(const QwRaster::EModuleType modtype, const TString &name) const;
  TString fRegion;  ///  Name of this subsystem (the region).
  size_t fCurrentBankIndex;
  Int_t fCurrentSlot;
  Int_t fCurrentIndex;
  static const UInt_t kMaxNumberOfModulesPerROC;
  static const UInt_t kMaxNumberOfChannelsPerModule;
  Int_t fNumberOfModules;
  std::vector< std::vector<Int_t> > fModuleIndex;  /// Module index, indexed by bank_index and slot_number
  std::vector< enum EModuleType > fModuleTypes;
  std::vector< std::vector< std::pair<Int_t, Int_t> > > fModulePtrs; // Indexed by Module_index and Channel

  Int_t GetEventcutErrorFlag() { return 0; };//return the error flag to the main routine

  // raster specified histograms
  std::vector<TH1*> fHistograms1D;
  std::vector<TH2*> fHistograms2D;
  TH2D* fRateMap;

 private:

  static const Bool_t bStoreRawData;

  Double_t fPositionOffsetX;
  Double_t fPositionOffsetY;
  Double_t fChannel_Offset_X;
  Double_t fChannel_Offset_Y;

  Double_t fCal_Factor_QDC_X;
  Double_t fCal_Factor_QDC_Y;

  Double_t fCal_Factor_VQWK_X;
  Double_t fCal_Factor_VQWK_Y;

  Double_t fVoltage_Offset_X;
  Double_t fVoltage_Offset_Y;

  Double_t fPositionX_ADC;
  Double_t fPositionY_ADC;

  Double_t fPositionX_VQWK;
  Double_t fPositionY_VQWK;

  UInt_t fBankID[2];  //fBankID[0] for V792/V775 QDC_Bank
                      //fBankID[1] for VQWK_Bank

};


#endif

