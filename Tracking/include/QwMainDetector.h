/*------------------------------------------------------------------------*//*!

 \file QwMainDetector.h

 \author P. King
 \author P. Wang

 \date	2007-05-08 15:40

 \brief This is the main executable for the tracking analysis.

*//*-------------------------------------------------------------------------*/

#ifndef __QWMAINDETECTOR__
#define __QWMAINDETECTOR__

#include <vector>
#include "TTree.h"

#include "VQwSubsystemTracking.h"
#include "QwColor.h"

#include "MQwV775TDC.h"
//#include "MQwF1TDC.h"
#include "QwPMT_Channel.h"
#include "QwSIS3801_Module.h"
#include "QwScaler_Channel.h"
#include "QwF1TDContainer.h"

///
/// \ingroup QwTracking
class QwMainDetector: public VQwSubsystemTracking, public MQwSubsystemCloneable<QwMainDetector> {
  /******************************************************************
   *  Class: QwMainDetector
   *
   *
   ******************************************************************/
 private:
  /// Private default constructor (not implemented, will throw linker error on use)
  QwMainDetector();

 public:
  /// Constructor with name
  QwMainDetector(const TString& name);
  /// Virtual destructor
  virtual ~QwMainDetector();

  /// Copying is not supported for tracking subsystems
  void Copy(const VQwSubsystem *source) {
    QwWarning << "Copy() is not supported for tracking subsystems." << QwLog::endl;
  }

  /*  Member functions derived from VQwSubsystem. */
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadGeometryDefinition(TString mapfile);
  Int_t LoadInputParameters(TString mapfile){return 0;};

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ClearEventData();
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ProcessEvent();

  using VQwSubsystem::ConstructHistograms;
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  using VQwSubsystem::ConstructBranchAndVector;
  void  ConstructBranchAndVector(TTree *tree, TString& prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  void  FillHardwareErrorSummary();

  QwMainDetector& operator=(const QwMainDetector &value);

  void ReportConfiguration();

 protected:
  EQwModuleType fCurrentType;

  Bool_t fDEBUG;

  MQwV775TDC fQDCTDC;
  MQwF1TDC fF1TDCDecoder;
  QwF1TDContainer *fF1TDContainer;

  void FillRawWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data);

 protected:
  void  ClearAllBankRegistrations();
  Int_t RegisterROCNumber(const UInt_t roc_id);
  Int_t RegisterSubbank(const UInt_t bank_id);
  Int_t RegisterSlotNumber(const UInt_t slot_id); // Tells this object that it will decode data from the current bank

  EQwModuleType RegisterModuleType(TString moduletype);

  Int_t GetModuleIndex(size_t bank_index, size_t slot_num) const;

  Bool_t IsSlotRegistered(Int_t bank_index, Int_t slot_num) const {
    return (GetModuleIndex(bank_index,slot_num) != -1);
  };

  Int_t LinkChannelToSignal(const UInt_t chan, const TString &name);
  Int_t FindSignalIndex(const EQwModuleType modtype, const TString &name) const;

  void GetHitList(QwHitContainer & grandHitContainer){

  }; //empty function


 protected:


  TString fRegion;  ///  Name of this subsystem (the region).


 protected:
  size_t fCurrentBankIndex;
  Int_t fCurrentSlot;
  Int_t fCurrentIndex;

  UInt_t fBankID[3];  //bank ID for 3 different modules
                      //fBankID[0] for V792/V775 QDC_Bank
                      //fBankID[1] for SIS3801   SCA_Bank
                      //fBankID[2] for F1TDC     F1TDC_Bank

 protected:
  static const UInt_t kMaxNumberOfModulesPerROC;
  UInt_t kMaxNumberOfChannelsPerF1TDC;
  Int_t fNumberOfModules;

  std::vector< std::vector<Int_t> > fModuleIndex;  //  Module index, indexed by bank_index and slot_number
  std::vector< EQwModuleType > fModuleTypes;
  std::vector< std::vector< std::pair< EQwModuleType, Int_t> > > fModulePtrs; // Indexed by Module_index and Channel; gives the plane and wire assignment.

  //    We need a mapping of module,channel into PMT index, ADC/TDC
  std::vector< std::vector<QwPMT_Channel> > fPMTs;
  std::vector<QwSIS3801_Module*> fSCAs;

  // For reference time substraction
  Int_t reftime_slotnum;
  Int_t reftime_channum;
  Double_t reftime;

  Bool_t IsF1ReferenceChannel (Int_t slot, Int_t chan) { 
    return ( slot == reftime_slotnum &&  chan == reftime_channum) ;
  };

  Int_t tdc_slot_number;
  Int_t tdc_chan_number;
  Int_t tmp_last_chan;

  

};

#endif
