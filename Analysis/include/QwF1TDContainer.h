#ifndef __QwF1TDContainer__
#define __QwF1TDContainer__

#include <iostream>

#include "QwTypes.h"
#include "MQwF1TDC.h"

#include "TObject.h"
#include "TClonesArray.h"

class QwF1TDC : public TObject
{

 public:
  QwF1TDC();
  QwF1TDC(const Int_t roc, const Int_t slot);
  ~QwF1TDC();


  const Int_t  GetROCNumber()    const {return fROC;};
  const Int_t  GetSlotNumber()   const {return fSlot;};
  const Bool_t IsReferenceSlot() const {return fReferenceSlotFlag;};

  void  SetROCNumber    (const Int_t roc)     {fROC = roc;};
  void  SetSlotNumber   (const Int_t slot)    {fSlot = slot;};
  void  SetReferenceSlot(const Bool_t reflag) {fReferenceSlotFlag = reflag;};


 public:
  Int_t fROC;          // F1TDC physical roc number, not index 
                       // 4  : ROC 4   
                       // 9  : ROC 9
                       // 10 : ROC 10

  Int_t fSlot;         // F1TDC slot number 
                       // QwAnalysis' slot range 0-20. But, the physical VME
                       // slot range is 1-21. QwAlaysis's slot range is acceptable
                       // if we carefully match them with each other as follows:
                       //  QwAnalysis slot 0, 1, 2, .... , 20
                       //  VME        slot 1, 2, 3, .... , 21
                       // Inside codes, we did the following way:
                       //  QwAnalysis slot 0, 1, 2, .... , 20
                       //     VME     slot    1, 2, 3, .... , 21
                       // It is acceptable if we don't use the VME slot 21 as
                       // we do.
                       // Friday, July 30 11:26:36 EDT 2010, jhlee

  Bool_t   fReferenceSlotFlag; //! 

  UInt_t   fF1TDC_refcnt;
  UInt_t   fF1TDC_hsdiv;
  UInt_t   fF1TDC_refclkdiv;
  UInt_t   fF1TDC_trigwin;
  UInt_t   fF1TDC_triglat;
  
  Double_t fF1TDC_tframe_ns;
  Double_t fF1TDC_full_range_ns;
  Double_t fF1TDC_bin_size_ns;
  Double_t fF1TDC_window_ns;
  Double_t fF1TDC_latency_ns;
  Double_t fF1TDC_resolution_ns;

 private:
  void Initialize();

  ClassDef(QwF1TDC,1);

};



class QwF1TDContainer : public TObject
{

 
  
 public:
  Int_t                    fNQwF1TDCs;
  TClonesArray             *fQwF1TDCs; // array range 0 -  gMaxCloneArray -1
  static TClonesArray      *gQwF1TDCs;

 public:
  QwF1TDContainer();
  QwF1TDContainer(EQwDetectorType detector_type);
  QwF1TDContainer(EQwDetectorType detector_type, EQwRegionID region);
  ~QwF1TDContainer();
  
  // Housekeeping methods
  void Clear(Option_t *option = "");
  void Reset(Option_t *option = "");
  void Delete(Option_t *option = "");

  void AddQwF1TDC(QwF1TDC &in);
  // Get the number of F1TDCs
  Int_t GetSize() const { return fNQwF1TDCs; };

  EQwDetectorType    GetDetectorType() {return fDetectorType;}
  EQwRegionID        GetRegion()       {return fRegion;};

  TClonesArray *GetF1TDCs() const { return fQwF1TDCs; };
  QwF1TDC *GetF1TDC (Int_t f1tdcID) const ;



  
 public:

  EQwDetectorType    fDetectorType;
  EQwRegionID        fRegion;
  
  //  typedef std::vararry <Double_t> 
private:
  static const Int_t gMaxCloneArray; //!  ///< Maximum number which a subsystem has F1TDCs

  ClassDef(QwF1TDContainer,1);

};

#endif
