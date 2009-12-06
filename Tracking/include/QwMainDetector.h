/**********************************************************\
* File: QwMainDetector.h                                      *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QWMAINDETECTOR__
#define __QWMAINDETECTOR__

#include <vector>
#include "TTree.h"

#include "VQwSubsystemTracking.h"

#include "MQwV775TDC.h"

#include "QwPMT_Channel.h"


///
/// \ingroup QwTracking
class QwMainDetector: public VQwSubsystemTracking, public MQwV775TDC {
  /******************************************************************
   *  Class: QwMainDetector
   *
   *
   ******************************************************************/
 public:
  QwMainDetector(TString region_tmp);
  ~QwMainDetector();

  /*  Member functions derived from VQwSubsystem. */
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadQweakGeometry(TString mapfile){return 0;};
  Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detector_info){ return 0;};
  Int_t LoadInputParameters(TString mapfile){return 0;};

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ClearEventData();
  Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ProcessEvent();

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  

  QwMainDetector& operator=  (const QwMainDetector &value);

 protected:
  enum EModuleType{EMPTY = -1, V775_TDC = 0, V792_ADC} fCurrentType;

  Bool_t fDEBUG;

  void FillRawWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);

 protected:
  void  ClearAllBankRegistrations();
  Int_t RegisterROCNumber(const UInt_t roc_id);

  Int_t RegisterSlotNumber(const UInt_t slot_id); // Tells this object that it will decode data from the current bank

  const QwMainDetector::EModuleType RegisterModuleType(TString moduletype);

  Int_t GetModuleIndex(size_t bank_index, size_t slot_num) const;

  Bool_t IsSlotRegistered(Int_t bank_index, Int_t slot_num) const {
    return (GetModuleIndex(bank_index,slot_num) != -1);
  };


  Int_t LinkChannelToSignal(const UInt_t chan, const TString &name);

  Int_t FindSignalIndex(const QwMainDetector::EModuleType modtype, const TString &name) const;

  void GetHitList(QwHitContainer & grandHitContainer){
    
  }; //empty function


 protected:

  
  TString fRegion;  ///  Name of this subsystem (the region).


 protected:
  size_t fCurrentBankIndex;
  Int_t fCurrentSlot;
  Int_t fCurrentIndex;

 protected:
  static const UInt_t kMaxNumberOfModulesPerROC;
  static const UInt_t kMaxNumberOfChannelsPerModule;

  Int_t fNumberOfModules;
  
  std::vector< std::vector<Int_t> > fModuleIndex;  //  Module index, indexed by bank_index and slot_number

  std::vector< enum EModuleType > fModuleTypes;
  std::vector< std::vector< std::pair<Int_t, Int_t> > > fModulePtrs; // Indexed by Module_index and Channel; gives the plane and wire assignment.



  //    We need a mapping of module,channel into PMT index, ADC/TDC
  std::vector< std::vector<QwPMT_Channel> > fPMTs;



};

#endif
