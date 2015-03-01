/**********************************************************\
* File: QwTriggerScintillator.h                            *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QWTRIGGERSCINTILLATOR__
#define __QWTRIGGERSCINTILLATOR__

#include <vector>
#include "TTree.h"

#include "VQwSubsystemTracking.h"

#include "QwHit.h"
#include "QwHitContainer.h"

#include "QwTypes.h"
#include "QwDetectorInfo.h"

#include "MQwV775TDC.h"
#include "QwScaler_Channel.h"
#include "QwPMT_Channel.h"
#include "QwF1TDContainer.h"
#include "QwSoftwareMeantime.h"

#include "QwColor.h"
#include "QwOptions.h"

///
/// \ingroup QwTracking
class QwTriggerScintillator: public VQwSubsystemTracking, public MQwSubsystemCloneable<QwTriggerScintillator> {
  /******************************************************************
   *  Class: QwTriggerScintillator
   *
   *
   ******************************************************************/
 private:
  /// Private default constructor (not implemented, will throw linker error on use)
  QwTriggerScintillator();

 public:
  /// Constructor with name
  QwTriggerScintillator(const TString& name);
  /// Virtual destructor
  virtual ~QwTriggerScintillator();

  /*  Member functions derived from VQwSubsystem. */
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString mapfile){return 0;};


  using VQwSubsystem::ConstructHistograms;
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  using VQwSubsystem::ConstructBranchAndVector;
  void  ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  ReportConfiguration(Bool_t verbose);
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ProcessEvent();
 

  void  FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
  void  FillHardwareErrorSummary();

  void  ClearEventData();

  static void DefineOptions(QwOptions& options);
  void ProcessOptions(QwOptions& options);


  void GetHitList(QwHitContainer & grandHitContainer) {
    grandHitContainer.Append(fTDCHits);
  };


  QwTriggerScintillator& operator=  (const QwTriggerScintillator &value);

 protected:

  EQwModuleType fCurrentType;

  Bool_t fDEBUG;

  Bool_t   fSoftwareMeantimeOption;
  Double_t fSoftwareMeantimeTimeWindowNs;


  TString fRegion;     ///  Name of this subsystem (the region).
  Int_t   fCurrentBankIndex;
  Int_t   fCurrentSlot;
  Int_t   fCurrentModuleIndex;

  UInt_t  fBankID[3];  
  //bank ID's of 3 different modules for trigger scintillator
  //fBankID[0] for V792/V775 QDC_Bank
  //fBankID[1] for SIS3801   SCA_Bank
  //fBankID[2] for F1TDC     F1TDC_Bank

  static const UInt_t kMaxNumberOfModulesPerROC;
  static const Int_t  kF1ReferenceChannelNumber;

  static const Int_t  kMaxNumberOfQwHitPlane;

  UInt_t kMaxNumberOfChannelsPerF1TDC;
  Int_t fNumberOfModules;



  MQwV775TDC       fQDCTDC;
  MQwF1TDC         fF1TDCDecoder;
  QwF1TDContainer *fF1TDContainer; 
  F1TDCReferenceContainer *fF1RefContainer;
  MeanTimeContainer       *fSoftwareMeantimeContainer[2];

  void FillRawWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);

  void  ClearAllBankRegistrations();
  Int_t RegisterROCNumber  (const UInt_t roc_id);
  Int_t RegisterSubbank    (const UInt_t bank_id);
  Int_t RegisterSlotNumber (const UInt_t slot_id); // Tells this object that it will decode data from the current bank

  Int_t GetModuleIndex(size_t bank_index, size_t slot_num) const;

  Bool_t IsSlotRegistered(Int_t bank_index, Int_t slot_num) const {
    return (GetModuleIndex(bank_index,slot_num) != -1);
  };


  EQwModuleType RegisterModuleType(TString moduletype);
  Int_t LinkChannelToSignal(const UInt_t chan, const TString &name);
  Int_t FindSignalIndex(const EQwModuleType modtype, const TString &name) const;



  void  SubtractReferenceTimes(); // be executed in ProcessEvent()

  // add   QwDetectorInfo   into QwDetecotrInfor of QwHit
  // add   F1TDC resolution into fTimeRes of QwHit
  // add   calibrated time (fTime*F1TDC_resolution_ns) into fTimens of QwHit

  void  UpdateHits();             // be executed in ProcessEvent()

  void  AddSoftwareMeantimeToHits(Bool_t option);

  std::vector< QwHit >              fTDCHits;

  std::vector< std::vector< QwDetectorID   > > fDetectorIDs; 
  // Indexed by module_index and Channel; and so on....
  std::vector< std::pair<Int_t, Int_t> > fReferenceChannels;  
  // reference chans number <first:tdc_index, second:channel_number>
  // fReferenceChannels[tdc_index,channel_number][ num of [tdc,chan] set]
  std::vector< std::vector<Double_t> > fReferenceData; 
  // fReferenceData[bank_index][channel_number]


  std::vector< std::vector<Int_t> > fModuleIndex;  //  Module index, indexed by bank_index and slot_number

  std::vector< EQwModuleType > fModuleTypes;
  std::vector< std::vector< std::pair< EQwModuleType, Int_t> > > fModulePtrs; // Indexed by Module_index and Channel; gives the plane and wire assignment.

  //    We need a mapping of module,channel into PMT index, ADC/TDC
  std::vector< std::vector<QwPMT_Channel> > fPMTs;
  std::vector<QwSIS3801D24_Channel>         fSCAs;
  std::map<TString,size_t>                  fSCAs_map;
  std::vector<Int_t>                        fSCAs_offset;


  // For reference time subtraction
  Int_t fRefTime_SlotNum;
  Int_t fRefTime_ChanNum;
  Double_t fRefTime;

  Bool_t IsF1ReferenceChannel (Int_t slot, Int_t chan) { 
    return ( slot == fRefTime_SlotNum &&  chan == fRefTime_ChanNum) ;
  };

};

#endif
