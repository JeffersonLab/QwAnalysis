#ifndef __QwF1TDContainer__
#define __QwF1TDContainer__
/**
 *  \file   QwF1TDContainer.h
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Friday, July 30 23:43:34 EDT 2010
 */

#include <iostream>

#include "QwTypes.h"
#include "MQwF1TDC.h"

#include "TObject.h"
#include "TClonesArray.h"
#include "TArrayD.h"

/**
 *  \class QwF1TDC
 *  \ingroup QwAnalysis
 *
 *  \brief one F1TDC configuration and reference signal(s) holder
 *
 */
class QwF1TDC : public TObject
{

 public:
  QwF1TDC();
  QwF1TDC(const Int_t roc, const Int_t slot);
  ~QwF1TDC();

  const Int_t  GetROCNumber()          const {return fROC;};
  const Int_t  GetSlotNumber()         const {return fSlot;};
  const Bool_t IsReferenceSlot()       const {return fReferenceSlotFlag;};
  const Bool_t IsNormalResolution()    const {return fF1TDCNormalResolutionFlag;};

  const UInt_t GetF1TDC_refcnt()       const {return fF1TDC_refcnt;};
  const UInt_t GetF1TDC_hsdiv()        const {return fF1TDC_hsdiv;};
  const UInt_t GetF1TDC_refclkdiv()    const {return fF1TDC_refclkdiv;};
  const UInt_t GetF1TDC_trigwin()      const {return fF1TDC_trigwin;};
  const UInt_t GetF1TDC_triglat()      const {return fF1TDC_triglat;};

  const Double_t GetF1TDC_tframe()     const {return fF1TDC_tframe_ns;};
  const Double_t GetF1TDC_full_range() const {return fF1TDC_full_range_ns;};
  const Double_t GetF1TDC_window()     const {return fF1TDC_window_ns;};
  const Double_t GetF1TDC_latency()    const {return fF1TDC_latency_ns;};
  const Double_t GetF1TDC_resolution() const {return fF1TDC_resolution_ns;};
  const Double_t GetF1TDC_bin_size()   const {return fF1TDC_resolution_ns;};

  void SetROCNumber       (const Int_t roc)          {fROC = roc;};
  void SetSlotNumber      (const Int_t slot)         {fSlot = slot;};
  void SetReferenceSlot   (const Bool_t reflag)      {fReferenceSlotFlag = reflag;};

  void SetF1TDC_refcnt    (const UInt_t refcnt)      {fF1TDC_refcnt = refcnt;};
  void SetF1TDC_hsdiv     (const UInt_t hsdiv)       {fF1TDC_hsdiv = hsdiv;};
  void SetF1TDC_refclkdiv (const UInt_t refclkdiv)   {fF1TDC_refclkdiv = refclkdiv;};
  void SetF1TDC_trigwin   (const UInt_t trigwin)     {fF1TDC_trigwin = trigwin;};
  void SetF1TDC_triglat   (const UInt_t triglat)     {fF1TDC_triglat = triglat;};

  void SetF1TDC_tframe    (const Double_t tframe_ns) {fF1TDC_tframe_ns = tframe_ns;};
  void SetF1TDC_full_range(const Double_t range_ns)  {fF1TDC_full_range_ns = range_ns;};
  void SetF1TDC_window    (const Double_t win_ns)    {fF1TDC_window_ns = win_ns;};
  void SetF1TDC_latency   (const Double_t lat_ns)    {fF1TDC_latency_ns = lat_ns;};
  void SetF1TDC_resolution(const Double_t resol_ns)  {fF1TDC_resolution_ns = resol_ns;};
  void SetF1TDC_bin_size  (const Double_t binsize_ns){fF1TDC_resolution_ns = binsize_ns;};

  void AddSEU() {fF1TDC_SEU_counter++;};
  void AddSYN() {fF1TDC_SYN_counter++;};
  void AddEMM() {fF1TDC_EMM_counter++;};

  void SetSEU() {fF1TDC_SEU_counter++;};
  void SetSYN() {fF1TDC_SYN_counter++;};
  void SetEMM() {fF1TDC_EMM_counter++;};
  
  const UInt_t GetSEU() const {return fF1TDC_SEU_counter;};
  const UInt_t GetSYN() const {return fF1TDC_SYN_counter;};
  const UInt_t GetEMM() const {return fF1TDC_EMM_counter;};

  void ResetCounters();

  //void SetReferenceSignals(Int_t chan_row, Double_t val);

 public:
  Int_t fROC;          // F1TDC physical roc number, not index 
                       // 4  : ROC 4   
                       // 9  : ROC 9
                       // 10 : ROC 10

  Int_t fSlot;         // F1TDC slot number 
                       // QwAnalysis' slot range 0-20. But, the physical VME
                       // slot range is 1-21. QwAlaysis's slot range is acceptable
                       // if we carefully match them with each other as follows:
                       //  QwAnalysis slot 0, 1, 2, .... , 2
                       //  VME        slot 1, 2, 3, .... , 21
                       // Inside codes, we did the following way:
                       //  QwAnalysis slot 0, 1, 2, .... , 20
                       //     VME     slot    1, 2, 3, .... , 21
                       // It is acceptable if we don't use the VME slot 21 as
                       // we do.
                       // Friday, July 30 11:26:36 EDT 2010, jhlee

 

  UInt_t fChannelNumber;
  
  UInt_t   fF1TDC_refcnt;
  UInt_t   fF1TDC_hsdiv;
  UInt_t   fF1TDC_refclkdiv;
  UInt_t   fF1TDC_trigwin;
  UInt_t   fF1TDC_triglat;
  
  Double_t fF1TDC_tframe_ns;
  Double_t fF1TDC_full_range_ns;
  //  Double_t fF1TDC_bin_size_ns;
  Double_t fF1TDC_window_ns;
  Double_t fF1TDC_latency_ns;
  Double_t fF1TDC_resolution_ns;

  UInt_t   fF1TDC_SEU_counter;  // Single Event Upset counter
  UInt_t   fF1TDC_EMM_counter;  // Event number MisMatch counter
  UInt_t   fF1TDC_SYN_counter;  // SYNc  issue counter

  Bool_t   fReferenceSlotFlag; //! 
  TArrayD  *fReferenceSignals;

  Bool_t   fF1TDCNormalResolutionFlag;


 private:
  // void Initialize();

  ClassDef(QwF1TDC,1);

};

/**
 *  \class QwF1TDContainer
 *  \ingroup QwAnalysis
 *
 *  \brief  F1TDCs configuration and reference siganls container
 * 
 *  This would be used to be a configuration and reference signal(s)
 *  container for F1TDCs which are used in one subsystem. The subsystem
 *  has one QwF1TDContaier that have DetectorType, Region, and QwF1TDC(s)
 *
 */


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
  
  void Clear (Option_t *option = "");
  void Reset (Option_t *option = "");
  void Delete(Option_t *option = "");

  void AddQwF1TDC(QwF1TDC &in);

  Int_t              GetSize()         const {return fNQwF1TDCs;};
  Int_t              HowManyF1TDCs()   const {return fNQwF1TDCs;};
  EQwDetectorType    GetDetectorType() const {return fDetectorType;}
  EQwRegionID        GetRegion()       const {return fRegion;};
  TClonesArray      *GetF1TDCs()       const {return fQwF1TDCs;};

  QwF1TDC     *GetF1TDC(Int_t f1tdcID) const;

 public:

  EQwDetectorType    fDetectorType;
  EQwRegionID        fRegion;
  
  //  typedef std::vararry <Double_t> 

private:

  static const Int_t gMaxCloneArray; //!  ///< Maximum number which a subsystem has F1TDCs

  ClassDef(QwF1TDContainer,1);

};

#endif
