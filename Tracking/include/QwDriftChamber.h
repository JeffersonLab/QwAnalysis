/**********************************************************\
* File: QwDriftChamber.h                                   *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
*         J. H. Lee                                        *
* Time-stamp: Wednesday, July 28 11:30:21 EDT 2010         *
\**********************************************************/


#ifndef __QWDRIFTCHAMBER__
#define __QWDRIFTCHAMBER__

#include "TH1F.h"
#include "TH2F.h"
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
#include "MQwF1TDC.h"
#include "MQwV775TDC.h"

#include "QwF1TDContainer.h"

///
/// \ingroup QwTracking
class QwDriftChamber: public VQwSubsystemTracking{
  /******************************************************************
   *  Class: QwDriftChamber
   *
   *
   ******************************************************************/
 public:
  QwDriftChamber(TString region_tmp);
  QwDriftChamber(TString region_tmp,std::vector< QwHit > &fWireHits_TEMP);

  virtual ~QwDriftChamber(){};

  /*  Member functions derived from VQwSubsystem. */


 
 
  Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detector_info) //will update the detector_info from the fDetectorInfo data.
  {
    detector_info.insert(detector_info.end(),fDetectorInfo.begin(),fDetectorInfo.end()) ;
    return 1;
  };
  virtual Int_t LoadInputParameters(TString mapfile){return 0;};
 

 

  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
 
  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  PrintConfigurationBuffer(UInt_t *buffer,UInt_t num_words);
  void  ReportConfiguration();

  //separate meanings in VDC and HDC
  virtual void  SubtractReferenceTimes() = 0;
  virtual void  ProcessEvent() = 0;
  virtual Int_t LoadGeometryDefinition(TString mapfile ) = 0;
 
  virtual Int_t LoadChannelMap(TString mapfile ) = 0;
  virtual void  ClearEventData() = 0;

  


  /* Unique member functions */


  void  FillDriftDistanceToHits();
 
 


  virtual  void GetHitList(QwHitContainer & grandHitContainer)
  {
    grandHitContainer.Append(fWireHits);
  };

  void GetTDCHitList(QwHitContainer & grandHitContainer)
  {
    grandHitContainer.Append(fTDCHits);
  };


  Int_t OK;

 protected:

  virtual void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data) = 0;
  virtual Int_t AddChannelDefinition() = 0;
  virtual Int_t BuildWireDataStructure(const UInt_t chan, const EQwDetectorPackage package, const Int_t plane, const Int_t wire)=0;
  virtual Double_t CalculateDriftDistance(Double_t drifttime, QwDetectorID detector)=0;
  virtual void ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  virtual void  FillHistograms() = 0;
  virtual void  DeleteHistograms() = 0;
  virtual Int_t LoadTimeWireOffset(TString t0_map) = 0;
  virtual void SubtractWireTimeOffset() = 0;
  virtual void ApplyTimeCalibration() = 0;


  Int_t LinkReferenceChannel(const UInt_t chan, const Int_t plane, const Int_t wire);
  void  ClearAllBankRegistrations();
  Int_t RegisterROCNumber(const UInt_t roc_id, const UInt_t bank_id);
  Int_t RegisterSubbank(const UInt_t bank_id);
  Int_t RegisterSlotNumber(const UInt_t slot_id); // Tells this object that it will decode data from the current bank
  Int_t GetTDCIndex(size_t bank_index, size_t slot_num) const;
  Bool_t IsSlotRegistered(Int_t bank_index, Int_t slot_num) const
  {
    return (GetTDCIndex(bank_index,slot_num) != -1);
  };


  Bool_t fDEBUG;

  TString fRegion;  ///  Name of this subsystem (the region).
  size_t fCurrentBankIndex;
  Int_t fCurrentSlot;
  Int_t fCurrentTDCIndex;

  // static const UInt_t kMaxNumberOfTDCsPerROC;
  static const UInt_t kMaxNumberOfSlotsPerROC;
  static const Int_t kReferenceChannelPlaneNumber; // plane is Int_t

  UInt_t kMaxNumberOfChannelsPerTDC;
  

  Int_t  fNumberOfTDCs;

  std::vector< std::vector<Int_t> > fTDC_Index;  //  TDC index, indexed by bank_index and slot_number

  std::vector< std::pair<Int_t, Int_t> > fReferenceChannels;  
  // reference chans number <first:tdc_index, second:channel_number>
  // fReferenceChannels[tdc_index,channel_number][ num of [tdc,chan] set]
  std::vector< std::vector<Double_t> > fReferenceData; 
  // wire number  < reference time > 
  // we use a wire number of QwHit to save a bank id of a reference time.
  // thus, for fReferenceData, the wire (fElement) is the same as
  // its bankid. Therefore we can use  fReferenceData[bankid][reference time]


  std::vector< QwHit > fTDCHits;
  std::vector< QwHit > &fWireHits;
  std::vector< Int_t > fWiresPerPlane;

  MQwF1TDC fF1TDC;

  QwF1TDContainer *fF1TDContainer;

  UInt_t fF1TDCBadDataCount;


  //  NOTE:  The plane and wire indices count from "1" instead
  //         of from "0".
  //         When you're creating loops, just be careful that
  //         you don't try to use the first (zero-th) element
  //         of either index.


  /*=====
   *  Histograms should be listed below here.
   *  They should be pointers to histograms which will be created
   *  inside the ConstructHistograms()
   */
  std::vector <TH1F*> TotHits;
  std::vector <TH1F*> TOFP;
  std::vector <TH1F*> TOFP_raw;
  std::vector <TH1F*> WiresHit;
  std::vector <TH2F*> TOFW;
  std::vector <TH2F*> TOFW_raw;
  std::vector <TH2F*> HitsWire;
  
  void InitHistogramPointers() {
    TotHits.clear();
    TOFP.clear();
    TOFP_raw.clear();
    WiresHit.clear();
    TOFW.clear();
    TOFW_raw.clear();
    HitsWire.clear();
  }

  /* TH1F *TotHits[13]; */
  /* TH1F *TOFP[13]; */
  /* TH1F *TOFP_raw[13]; */
  /* TH1F *WiresHit[13];  TH2F *TOFW[13]; */
  /* TH2F *TOFW_raw[13]; */
  /* TH2F *HitsWire[13]; */
  /* void InitHistogramPointers() { */
  /*   // this magic 13 is eventually fWiresPerPlane.size(), but where? */
  /*   for(UInt_t i=0; i<13; ++i) { */
  /*     TotHits[i] = TOFP[i] = TOFP_raw[i] = WiresHit[i] = NULL; */
  /*     TOFW[i] = TOFW_raw[i] = HitsWire[i] = NULL; */
  /*   } */
  /* } */






  //below are the data structures that are used in HDC/VDC


  std::vector< std::vector< QwDetectorID   > > fTDCPtrs; 
  // Indexed by TDC_index and Channel; gives the package, plane and wire assignment.
  std::vector< std::vector< QwDetectorInfo > > fWireData;
  std::vector< std::vector< QwDetectorInfo > > fDetectorInfo; 
  // Indexed by package, plane this contains detector geometry information for each region;

  std::vector< std::vector< UInt_t >         > fDirectionData; 
  //Indexed by pckg and plane each element represent the wire direction ( a value from 0 to 6)- Rakitha(10/23/2008)



};

#endif
