// Author : Jeong Han Lee
//          jhlee@jlab.org
//
// Date   : Tuesday, February 28 14:59:01 EST 2012
// 
//         This script is used to calculate or find good software mentime of MDs and TSs,
//         because of F1TDC multihit structure
// 
//         One needs qwroot instead of root
//         $ qwroot 
//         Qw-Root [0] .L F1TDCGoodMT.C
//         Qw-Root [1] GoodMTMultiHits(1,2,-1, 10000, 50, 20, 1, 1, filename, 102)
//
//         MD1, TS2, event 0:10000, delta MD1 < 50, delta TS2 <20, MD debug on, TS debug on, rootfile, binnumber
//         in order to save debug outputs in a file
//         Qw-Root [3] GoodMtMultiHits(1,2,-1,-1,50,20,1,1); > debug.log
//
//         GoodMTMultiHits(2,2,-1,-1, 2000, 2000, 1,1, "MDTrigger_15027.root"); > MD2_2000_TS2_2000_Run15027.log
//
//
//         0.0.2 Thursday, March  1 16:50:53 EST 2012 , jhlee
//
//               added a difference between hardware meantime and software meantime of TS
//                     in order to check this script quality.
//                     if we see always constant difference between software and hardware
//                     we might say, this scripts is good enough to use to build software meantime
//                     of MD, which doesn't have hardware meantime. 
//
//               added some boolean conditions to suppress Draw or Canvas when a given event range is small
//     

#include <iostream>
#include <cmath>


#include "TObject.h"
#include "TObjArray.h"
#include "TString.h"
#include "TMath.h"

#include "TROOT.h"




/* /\** */
/*  *  \class MeanTime */
/*  *  \ingroup QwAnalysis */
/*  * */
/*  *  \brief one software meantim holder */
/*  * */
/*  *\/ */

/* // A good meantime holder  */
/* class MeanTime  :  public TObject */
/* { */

/* public: */

/*   MeanTime(); */
/*   MeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id); */
/*   virtual ~MeanTime(); */

/*   Bool_t   HasMeanTime()            {return fHasValue;}; */

/*   Double_t GetMeanTime()      const { return fMeanTime; }; */
/*   Double_t GetSubtractTime()  const { return fSubtractTime; }; */

/*   Double_t GetDiffHardSoftMeanTime() const { return fDiffHardSoftMeanTime; }; */

/*   Double_t GetPositiveValue() const { return fPositiveValue; }; */
/*   Double_t GetNegativeValue() const { return fNegativeValue; }; */

/*   Int_t    GetPositiveHitId() const { return fPositiveHitId; }; */
/*   Int_t    GetNegativeHitId() const { return fNegativeHitId; }; */

/*   Int_t    GetMeanTimeId()    const { return fMeanTimeId; }; */


/*   void     AddPNValues(Double_t p_value, Double_t n_value, Int_t p_id, Int_t n_id); */
/*   void     SetDetectorType(TString d_type) {fDetectorType = d_type;}; */
/*   void     SetMeanTimeId(Int_t id)         {fMeanTimeId = id;}; */

/*   void     SetHardwareMeanTime(Double_t hardware_meantime); */

/*   void     Print(Bool_t on); */

/*   Bool_t   IsInTimeWindow(Double_t time_window); */
/*   Bool_t   IsHitIDsMatch(); */

/* private: */

/*   Bool_t   fHasValue; */
/*   Double_t fMeanTime; */
/*   Double_t fSubtractTime;  // positive - negative */

/*   Double_t fDiffHardSoftMeanTime; // Hardware MT - Software MT */
/*   Double_t fHardWareMeanTime; */

/*   Double_t fPositiveValue; */
/*   Double_t fNegativeValue; */

/*   Int_t    fPositiveHitId; */
/*   Int_t    fNegativeHitId; */
/*   Int_t    fMeanTimeId; */
  
/*   Long64_t fEventId; */
/*   TString  fDetectorType; */

/*   ClassDef(MeanTime,0); */
/* }; */




/* /\** */
/*  *  \class MeanTimeContainer */
/*  *  \ingroup QwAnalysis */
/*  * */
/*  *  \brief  Software Meantime container */
/*  *  */
/*  *\/ */


/* // all possible combination of meantimes per an event */
/* // a method to find good meantimes. */
/* // .... */
/* class MeanTimeContainer  */
/* { */


/*  public : */
/*   Int_t                    fNMeanTimes; */
/*   TObjArray               *fMeanTimeList;  */
/*   Int_t                    fNHarewareMeanTimes; */
/*   Int_t                    fNPositive; */
/*   Int_t                    fNNegative; */

/* public: */

/*   MeanTimeContainer(); */
/*   MeanTimeContainer(TString name); */
/*   virtual ~MeanTimeContainer(); */

  

/*   void Add(Double_t p_value, Double_t n_value, Int_t hit_id); */
/*   void Add(Double_t p_value[7], Double_t n_value[7]); */
/*   void Add(Double_t p_value[7], Double_t n_value[7], Double_t hardware_meantime[7]); */
/*   Int_t AddMeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id); */
/*   void Clear(); */

/*   void ProcessMeanTime(); */
 
/*   Int_t  Size()           const {return fNMeanTimes;}; */
/*   Int_t  HardwareMTSize() const {return fNHarewareMeanTimes;}; */

/*   void   SetSystemName  (const TString name  ) { fDetectorName = name;}; */
/*   void   SetTimeWindow  (const Double_t in_ns) {fTimeWindowNs = in_ns;}; */
/*   void   SetDetectorType(const TString name  ) {fDetectorName = name;}; */
/*   void   SetEventId     (const Long64_t in   ) {fEventId = in;}; */

/*   const  TString GetDetectorType() const {return fDetectorName;}; */

/*   MeanTime *GetMeanTimeObject(Int_t index); */
/*   Double_t  GetMeanTime(Int_t index); */

/*   void Print(Bool_t on); */
 
/* private: */

/*   Double_t  fTimeWindowNs; */
/*   TString   fDetectorName; */
/*   //  Int_t     fNMeanTimes; */
  
/*   Long64_t  fEventId; */

/*   Double_t  fPositiveValue[7]; */
/*   Double_t  fNegativeValue[7]; */
/*   Double_t  fHardwareMeantimeValue[7]; */

/*   void      MatchHardwareMeanTime(); */
/*   Bool_t    IsHarewareMatchSoftware(); */

/*   ClassDef(MeanTimeContainer,0); */

/* }; */
