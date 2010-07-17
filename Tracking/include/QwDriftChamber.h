/**********************************************************\
* File: QwDriftChamber.h                                   *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-08 15:40>                           *
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

#include "VQwSubsystemTracking.h"
#include "MQwF1TDC.h"
#include "MQwV775TDC.h"



///
/// \ingroup QwTracking
class QwDriftChamber: public VQwSubsystemTracking, public MQwF1TDC{
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


  Int_t LoadChannelMap(TString mapfile );
  //LoadGeometryDefinition will load QwDetectorInfo vector from a map file
  //Currently this method is specific to each region
  virtual Int_t LoadGeometryDefinition(TString mapfile )=0;
  Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detector_info) //will update the detector_info from the fDetectorInfo data.
  {
    detector_info.insert(detector_info.end(),fDetectorInfo.begin(),fDetectorInfo.end()) ;
    return 1;
  };
  virtual Int_t LoadInputParameters(TString mapfile){return 0;};
  void  ClearEventData();

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words){return 0;};

  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  virtual void  ProcessEvent()=0;//has separate meanings in VDC and HDC

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();





  /* Unique member functions */
  virtual void  ReportConfiguration() = 0;

  virtual void  SubtractReferenceTimes() = 0;


  void  CalculateDriftDistance();

  virtual Double_t CalculateDriftDistance(Double_t drifttime, QwDetectorID detector)=0;


  virtual  void GetHitList(QwHitContainer & grandHitContainer)
  {
    grandHitContainer.Append(fWireHits);
  };

  void GetTDCHitList(QwHitContainer & grandHitContainer)
  {
    grandHitContainer.Append(fTDCHits);
  };


  Int_t OK;

  // private:

  //  virtual Double_t DoCalculate(Double_t drifttime,QwDetectorID detector,Double_t angle)=0;

 protected:

  Int_t LinkReferenceChannel(const UInt_t chan, const UInt_t plane, const UInt_t wire);
  virtual Int_t BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire)=0;
  //  virtual Int_t AddChannelDefinition(const UInt_t plane, const UInt_t wire)= 0;
  virtual Int_t AddChannelDefinition() = 0;



 protected:
  virtual void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data) = 0;




 protected:
  void  ClearAllBankRegistrations();
  Int_t RegisterROCNumber(const UInt_t roc_id, const UInt_t bank_id);

  Int_t RegisterSlotNumber(const UInt_t slot_id); // Tells this object that it will decode data from the current bank



  Int_t GetTDCIndex(size_t bank_index, size_t slot_num) const;

  Bool_t IsSlotRegistered(Int_t bank_index, Int_t slot_num) const
  {
    return (GetTDCIndex(bank_index,slot_num) != -1);
  };



 protected:
  Bool_t fDEBUG;

  TString fRegion;  ///  Name of this subsystem (the region).

  


 protected:
  size_t fCurrentBankIndex;
  Int_t fCurrentSlot;
  Int_t fCurrentTDCIndex;

 protected:
  static const UInt_t kMaxNumberOfTDCsPerROC;
  static const UInt_t kReferenceChannelPlaneNumber;

  UInt_t kMaxNumberOfChannelsPerTDC;
  Int_t fNumberOfTDCs;

  std::vector< std::vector<Int_t> > fTDC_Index;  //  TDC index, indexed by bank_index and slot_number

  std::vector< std::pair<Int_t, Int_t> > fReferenceChannels;  
  // reference chans number <first:tdc_index, second:channel_number>
  // fReferenceChannels[tdc_index,channel_number][ num of [tdc,chan] set]

  std::vector< QwHit > fTDCHits;
  std::vector< QwHit > &fWireHits;
  std::vector< Int_t > fWiresPerPlane;


  //  NOTE:  The plane and wire indices count from "1" instead
  //         of from "0".
  //         When you're creating loops, just be careful that
  //         you don't try to use the first (zero-th) element
  //         of either index.
  std::vector< std::vector<Double_t> > fReferenceData; 
  // wire number < reference time > 
  // fReferenceData[reference time][wire number]


  /*=====
   *  Histograms should be listed below here.
   *  They should be pointers to histograms which will be created
   *  inside the ConstructHistograms()
   */
  TH1F *TotHits[13];
  TH1F *TOFP[13];
  TH1F *TOFP_raw[13];
  TH1F *WiresHit[13];  TH2F *TOFW[13];
  TH2F *TOFW_raw[13];
  TH2F *HitsWire[13];
  void InitHistogramPointers() {
    // this magic 13 is eventually fWiresPerPlane.size(), but where?
    for(UInt_t i=0; i<13; ++i) {
      TotHits[i] = TOFP[i] = TOFP_raw[i] = WiresHit[i] = NULL;
      TOFW[i] = TOFW_raw[i] = HitsWire[i] = NULL;
    }
  }






  //below are the data structures that are used in HDC/VDC


  std::vector< std::vector< QwDetectorID   > > fTDCPtrs; 
  // Indexed by TDC_index and Channel; gives the package, plane and wire assignment.
  std::vector< std::vector< QwDetectorInfo > > fWireData;
  std::vector< std::vector< QwDetectorInfo > > fDetectorInfo; 
  // Indexed by package, plane this contains detector geometry information for each region;

  std::vector< std::vector< UInt_t >         > fDirectionData; 
  //Indexed by pckg and plane each element represent the wire direction ( a value from 0 to 6)- Rakitha(10/23/2008)

  std::vector< std::vector< std::vector<Double_t> > > fTimeWireOffsets;


 protected:
  //Double_t fTimeWireOffsets[kNumPackages][2][279]; 
  //Indexed by pckg and plane number and wire number(only used on R3 right now)

  Int_t LoadTimeWireOffset(TString t0_map);

  void SubtractWireTimeOffset();

  void ApplyTimeCalibration();

 /* protected: */
 /*  UInt_t fF1DataIntegrityCount; */
 /*  UInt_t GetF1DataIntegrityCount(){return fF1DataIntegrityCount;}; */

};

#endif
