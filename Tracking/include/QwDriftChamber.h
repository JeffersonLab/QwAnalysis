/**********************************************************\
* File: QwDriftChamber.h                                   *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-08 15:40>                           *
\**********************************************************/


#ifndef __QWDRIFTCHAMBER__
#define __QWDRIFTCHAMBER__

#include "QwParameterFile.h"

#include<TH1F.h>
#include<TH2F.h>
#include<TTree.h>

#include "QwHit.h"
#include "QwHitContainer.h"

#include "QwTypes.h"
#include "QwDetectorInfo.h"

#include <exception>
#include<iostream>
#include<fstream>
#include<string>

#include "VQwSubsystem.h"
#include "MQwF1TDC.h"



class QwDriftChamber: public VQwSubsystem, public MQwF1TDC{
  /******************************************************************
   *  Class: QwDriftChamber
   *
   *
   ******************************************************************/
 public:  
  QwDriftChamber(TString region_tmp);
  QwDriftChamber(TString region_tmp,std::vector< QwHit > &fWireHits_TEMP);

    ~QwDriftChamber()
    {
      DeleteHistograms();
    }

  Int_t OK;
    
  /*  Member functions derived from VQwSubsystem. */

  
  Int_t LoadChannelMap(TString mapfile );
  void  ClearEventData();

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words){return 0;};

  Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);

  void  ProcessEvent(){
    if (! HasDataLoaded()) return;
    SubtractReferenceTimes();
  };
  
  

  
  void  ConstructHistograms(TDirectory *folder);
  void  FillHistograms();
  void  DeleteHistograms();

  

 

  /* Unique member functions */
  virtual void  ReportConfiguration() = 0;

  virtual void  SubtractReferenceTimes() = 0;
  
  void getHitList(QwHitContainer & grandHitContainer){
    //std::cout << " HDC "<<fTDCHits.size()<<std::endl;
    grandHitContainer.Append(fTDCHits);
 };
   

protected:

 Int_t LinkReferenceChannel(const UInt_t chan, const UInt_t plane, const UInt_t wire);
 virtual Int_t BuildWireDataStructure(const UInt_t chan, const UInt_t package, const UInt_t plane, const Int_t wire)=0;
 virtual Int_t AddChannelDefinition(const UInt_t plane, const UInt_t wire)= 0;



 protected:
 virtual void FillRawTDCWord(Int_t bank_index, Int_t slot_num, Int_t chan, UInt_t data) = 0;




 protected:
   void  ClearAllBankRegistrations();
   Int_t RegisterROCNumber(const UInt_t roc_id);

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
  static const UInt_t kMaxNumberOfChannelsPerTDC;

  static const UInt_t kReferenceChannelPlaneNumber;

  Int_t fNumberOfTDCs;


std::vector< std::vector<Int_t> > fTDC_Index;  //  TDC index, indexed by bank_index and slot_number
  std::vector< std::pair<Int_t, Int_t> > fReferenceChannels;

  
  std::vector< QwHit > fTDCHits;

  std::vector< QwHit > &fWireHits;




  std::vector<Int_t>   fWiresPerPlane;

  //  NOTE:  The plane and wire indices count from "1" instead 
  //         of from "0".  
  //         When you're creating loops, just be careful that
  //         you don't try to use the first (zero-th) element
  //         of either index.
  std::vector< std::vector< Double_t> > fReferenceData;


 /*=====
   *  Histograms should be listed below here.
   *  They should be pointers to histograms which will be created
   *  inside the ConstructHistograms() 
   */
  TH1F *TotHits[13];
  TH1F *TOFP[13];
  TH1F *TOFP_raw[13];
  TH1F *WiresHit[13];
  TH2F *TOFW[13];
  TH2F *TOFW_raw[13];
  TH2F *HitsWire[13];  
  




 //below are the data structures that are used in HDC/VDC  
    

  std::vector< std::vector< QwDetectorID > > fTDCPtrs; // Indexed by TDC_index and Channel; gives the package, plane and wire assignment.


  std::vector< std::vector< QwDetectorInfo > > fWireData; // Indexed by plane, and wire number; eventually should include package index 
    
  std::vector< std::vector< UInt_t > > fDirectionData; //Indexed by pckg and plane each element represent the wire direction ( a value from 0 to 6)- Rakitha(10/23/2008)



};

#endif
