//
// File: QwSciFiDetector.h                          
// Author: Jeong Han Lee
// Time-stamp: Sunday, January 15 17:26:13 EST 2012
//

#ifndef __QWSCIFIDETECTOR__
#define __QWSCIFIDETECTOR__

//#include "TH1F.h"
//#include "TH2F.h"
#include "TTree.h"

#include "QwHit.h"
#include "QwHitContainer.h"

#include "QwTypes.h"
#include "QwDetectorInfo.h"

#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "VQwSubsystemTracking.h"
#include "QwF1TDContainer.h"
#include "QwScaler_Channel.h"

class QwHitContainer;


class QwSciFiDetector: public VQwSubsystemTracking, public MQwSubsystemCloneable<QwSciFiDetector> 
{

 private:
  /// Private default constructor (not implemented, will throw linker error on use)
  QwSciFiDetector();

 public:
  QwSciFiDetector(const TString& name);
  virtual ~QwSciFiDetector();
  
  /*  Member functions derived from VQwSubsystemTracking. */
  Int_t LoadChannelMap(TString mapfile );
  Int_t LoadInputParameters(TString mapfile);


  void  FillListOfHits(QwHitContainer& hitlist);

  using VQwSubsystem::ConstructHistograms;
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  using VQwSubsystem::ConstructBranchAndVector;
  void  ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values) const;




  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
 
  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  PrintConfigurationBuffer(UInt_t *buffer,UInt_t num_words);
  void  ReportConfiguration(Bool_t verbose);
  void  ProcessEvent();

  void  FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);
  void  FillHardwareErrorSummary();


  void  ClearEventData();



  void GetHitList(QwHitContainer & grandHitContainer){
    grandHitContainer.Append(fTDCHits);
  };



 protected:


  static const UInt_t kMaxNumberOfSlotsPerROC;
  static const Int_t  kF1ReferenceChannelNumber;

  TString kRegion;  ///  Name of this subsystem (the region).
  Int_t   fCurrentBankIndex;
  Int_t   fCurrentSlot;
  Int_t   fCurrentModuleIndex;
  UInt_t  kMaxNumberOfChannelsPerF1TDC;
  Int_t   kNumberOfVMEModules;
  //  Int_t   kNumberOfF1TDCs;

  


  MQwF1TDC         fF1TDCDecoder;
  QwF1TDContainer *fF1TDContainer;
  F1TDCReferenceContainer * fF1RefContainer;

  void  ClearAllBankRegistrations();
  Int_t RegisterROCNumber  (const UInt_t roc_id);
  Int_t RegisterSubbank    (const UInt_t bank_id);
  Int_t RegisterSlotNumber (const UInt_t slot_id); // Tells this object that it will decode data from the current bank

  Int_t GetModuleIndex(size_t bank_index, size_t slot_num) const;

  Bool_t IsSlotRegistered(Int_t bank_index, Int_t slot_num) const {
    return (GetModuleIndex(bank_index,slot_num) != -1);
  };



  void  SubtractReferenceTimes(); // be executed in ProcessEvent()

  // add   QwDetectorInfo   into QwDetecotrInfor of QwHit
  // add   F1TDC resolution into fTimeRes of QwHit
  // add   calibrated time (fTime*F1TDC_resolution_ns) into fTimens of QwHit

  void  UpdateHits();             // be executed in ProcessEvent()


  std::vector< QwHit >              fTDCHits;
  std::vector< std::vector<Int_t> > fModuleIndex;  
  //  Module index, indexed by bank_index and slot_number
  // std::vector< EQwModuleType > fModuleTypes;

  std::vector< std::vector< QwDetectorID   > > fDetectorIDs; 
  // Indexed by module_index and Channel; and so on....

  std::vector< std::pair<Int_t, Int_t> > fReferenceChannels;  
  // reference chans number <first:tdc_index, second:channel_number>
  // fReferenceChannels[tdc_index,channel_number][ num of [tdc,chan] set]
  std::vector< std::vector<Double_t> > fReferenceData; 
  // fReferenceData[bank_index][channel_number]

  std::vector<QwSIS3801D24_Channel> fSCAs;
  std::map<TString,size_t>          fSCAs_map;
  std::vector<Int_t>                fSCAs_offset;


  // Currently, we have only one scaler, if one wants to add more
  // change this to vector or others.
  // Wednesday, January 18 16:07:23 EST 2012,jhlee
  Int_t fScalerBankIndex;

};

#endif
