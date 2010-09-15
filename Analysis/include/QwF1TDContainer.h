#ifndef __QwF1TDContainer__
#define __QwF1TDContainer__
/**
 *  \file   QwF1TDContainer.h
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Tuesday, September 14 23:04:45 EDT 2010
 */

#include <iostream>
#include <math.h>


#include "QwTypes.h"
#include "MQwF1TDC.h"

#include "TObject.h"
#include "TClonesArray.h"
#include "TArrayD.h"
#include "TList.h"
#include "TString.h"
#include "TObjString.h"

#include "TROOT.h"
#include "TFile.h"

/**
 *  \class QwF1TDC
 *  \ingroup QwAnalysis
 *
 *  \brief one F1TDC configuration and reference signal(s) holder
 *
 */
class QwF1TDC :  public TObject
{

 public:
  QwF1TDC();
  QwF1TDC(const Int_t roc, const Int_t slot);
  ~QwF1TDC();

  friend std::ostream& operator<<(std::ostream& os, const QwF1TDC &f1tdc);

  const Int_t  GetROCNumber()          const {return fROC;};
  const Int_t  GetSlotNumber()         const {return fSlot;};
  const Int_t  GetChannelNumber()      const {return fChannelNumber;};

  const Bool_t IsReferenceSlot()       const {return fReferenceSlotFlag;};
  const Bool_t IsNormResolution()      const {return fF1TDCNormResFlag;};
  const Bool_t IsSyncMode()            const {return fF1TDCSyncFlag;};

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

  const Double_t GetF1TDC_t_offset()   const {return fF1TDC_t_offset;};

  const Int_t   GetF1TDCIndex()        const {return fF1TDCIndex;};
  const Int_t   GetF1BankIndex()       const {return fF1BankIndex;};
  const TString GetF1SystemName()      const {return fSystemName;};

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


  void SetF1TDCIndex  (const Int_t tdc_index)   {fF1TDCIndex = tdc_index;};
  void SetF1BankIndex (const Int_t bank_index)  {fF1BankIndex = bank_index;};
  void SetF1SystemName(const TString name)      {fSystemName = name;};
  
  void SetF1TDCBuffer(UInt_t *buffer, UInt_t num_words);
  const UInt_t * GetF1TDCBuffer() const {return fBuffer;};

  void PrintF1TDCBuffer();
  void PrintF1TDCConfigure();
  void PrintErrorCounter();
  
  TString GetErrorCounter ();


  void AddSEU() {fF1TDC_SEU_counter++;};
  void AddSYN() {fF1TDC_SYN_counter++;};
  void AddEMM() {fF1TDC_EMM_counter++;};
  void AddTFO() {fF1TDC_TFO_counter++;};
  void AddRLF() {fF1TDC_RLF_counter++;};
  void AddHFO() {fF1TDC_HFO_counter++;};
  void AddOFO() {fF1TDC_OFO_counter++;};

  
  const UInt_t GetSEU() const {return fF1TDC_SEU_counter;};
  const UInt_t GetSYN() const {return fF1TDC_SYN_counter;};
  const UInt_t GetEMM() const {return fF1TDC_EMM_counter;};
  const UInt_t GetTFO() const {return fF1TDC_TFO_counter;};

  const UInt_t GetRLF() const {return fF1TDC_RLF_counter;};   // Resolution Lock Fail couter
  const UInt_t GetHFO() const {return fF1TDC_HFO_counter;};   // Hit    Fifo Overflow counter
  const UInt_t GetOFO() const {return fF1TDC_OFO_counter;};   // Output Fifo Overflow coutner;

  void ResetCounters();

  //void SetReferenceSignals(Int_t chan_row, Double_t val);

  Double_t ReferenceSignalCorrection(Double_t raw_time, Double_t ref_time);


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

 
  
  Int_t   fChannelNumber;
  UInt_t   *fBuffer;


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

  Double_t fF1TDC_t_offset;

  Double_t fF1TDCFactor;


  // We don't look at these errors upto "channels",
  // and we care these numbers per each F1TDC.
  // There is no reason to track them upto channel levels.
  // Do we have?
  // Monday, September 13 14:55:10 EDT 2010, jhlee

  UInt_t   fF1TDC_SEU_counter;  // Single Event Upset counter
  UInt_t   fF1TDC_EMM_counter;  // Event number MisMatch counter
  UInt_t   fF1TDC_SYN_counter;  // SYNc  issue counter
  UInt_t   fF1TDC_TFO_counter;  // Trigger Fifo Overflow counter


  UInt_t   fF1TDC_RLF_counter;   // Resolution Lock Fail couter
  UInt_t   fF1TDC_HFO_counter;   // Hit    Fifo Overflow counter
  UInt_t   fF1TDC_OFO_counter;   // Output Fifo Overflow coutner;


  Bool_t   fReferenceSlotFlag; //! 
  TArrayD  *fReferenceSignals;

  Bool_t   fF1TDCNormResFlag;
  Bool_t   fF1TDCSyncFlag;


  Int_t    fF1TDCIndex;  // keep the tdcindex info from GetTDCIndex() function
  Int_t    fF1BankIndex; // This is the Bank index, which
                         // is the redefined index to access "bank=0x0901"
                         // defined in map file (e.g. qweak_R2.map)
 
 private:

  //TString  fF1ErrorSummaryString;

  TString  fSystemName;
  static const Int_t fWordsPerBuffer;
  static const Int_t fMaxF1TDCChannelNumber;
  


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


class QwF1TDContainer :  public TObject
{
  
 public:
  Int_t                    fNQwF1TDCs;
  TObjArray               *fQwF1TDCList; 
 
 public:
  QwF1TDContainer();
  virtual ~QwF1TDContainer();

  // friend std::ostream& operator<<(std::ostream& os, const QwF1TDContainer &container);

  void SetSystemName(const TString name);// {fSystemName = name;};
  const TString GetSystemName() const {return fSystemName;};

  void AddQwF1TDC(QwF1TDC *in);
  void Print();

  Int_t              GetSize()         const {return fNQwF1TDCs;};
  Int_t              Size()            const {return fNQwF1TDCs;};
  Int_t              HowManyF1TDCs()   const {return fNQwF1TDCs;};
  EQwDetectorType    GetDetectorType() const {return fDetectorType;}
  EQwRegionID        GetRegion()       const {return fRegion;};

  QwF1TDC* GetF1TDC(Int_t roc, Int_t slot);
  QwF1TDC* GetF1TDCwithIndex(Int_t tdc_index);
  QwF1TDC* GetF1TDCwithBankIndexSLOT(Int_t bank_index, Int_t slot);

  const Double_t GetF1TDCResolution();
  const Int_t GetF1TDCChannelNumber();


  
  void AddSYN(Int_t roc, Int_t slot);
  void AddEMM(Int_t roc, Int_t slot);
  void AddSEU(Int_t roc, Int_t slot);
  void AddTFO(Int_t roc, Int_t slot);

  void AddRLF(Int_t roc, Int_t slot);
  void AddHFO(Int_t roc, Int_t slot);
  void AddOFO(Int_t roc, Int_t slot);

 
  Bool_t  CheckDataIntegrity(const UInt_t roc_id, UInt_t *buffer, UInt_t num_words);

  const MQwF1TDC GetF1TDCDecoder() const {return fF1TDCDecoder;};
  
  Double_t ReferenceSignalCorrection(Double_t raw_time, Double_t ref_time, Int_t bank_index, Int_t slot);

  void PrintErrorSummary();

  void WriteErrorSummary();


 public:

  EQwDetectorType    fDetectorType;
  EQwRegionID        fRegion;
  TString            fSystemName;
  
  //  typedef std::vararry <Double_t> 

private:


  MQwF1TDC fF1TDCDecoder;
  Bool_t fLocalDebug;
  
  TList* GetErrorSummary();

  TString PrintNoF1TDC(Int_t roc, Int_t slot);
  TString PrintNoF1TDC(Int_t tdc_index);
  Bool_t  CheckRegisteredF1(Int_t roc, Int_t slot);

  ClassDef(QwF1TDContainer,1);

};


#endif
