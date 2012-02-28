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






#include <iostream>
#include <math.h>


#include "TObject.h"
#include "TString.h"

#include "TROOT.h"



// A good meantime holder 
class MeanTime  :  public TObject
{
  
public:

  MeanTime();
  MeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id);
  ~MeanTime(){};

  Bool_t   HasMeanTime() {return fHasValue;};

  Double_t GetMeanTime()     const { return fMeanTime; };
  Double_t GetSubtractTime()  const { return fSubtractTime; };

  Double_t GetPositiveValue() const { return fPositiveValue; };
  Double_t GetNegativeValue() const { return fNegativeValue; };

  Int_t    GetPositiveHitId() const { return fPositiveHitId; };
  Int_t    GetNegativeHitId() const { return fNegativeHitId; };

  Int_t    GetMeanTimeId()    const { return fMeanTimeId; };


  void     AddPNValues(Double_t p_value, Double_t n_value, , Int_t p_id, Int_t n_id);
  void     SetDetectorType(TString d_type) {fDetectorType = d_type;};
  void     SetMeanTimeId(Int_t id) {fMeanTimeId = id;};
  void     Print();

  Bool_t   IsInTimeWindow(Double_t time_window);

private:

  Bool_t   fHasValue;
  Double_t fMeanTime;
  Double_t fSubtractTime;  // positive - negative

  Double_t fPositiveValue;
  Double_t fNegativeValue;

  Int_t    fPositiveHitId;
  Int_t    fNegativeHitId;
  Int_t    fMeanTimeId;
  
  Long64_t fEventId;
  TString  fDetectorType;

  ClassDef(MeanTime,0);
};




// all possible combination of meantimes per an event
// a method to find good meantimes.
// ....
class MeanTimeContainer 
{


 public :
  Int_t                    fNMeanTimes;
  TObjArray               *fMeanTimeList; 

public:

  MeanTimeContainer();
  MeanTimeContainer(TString name);
  ~MeanTimeContainer();


  void Add(Double_t p_value, Double_t n_value, Int_t hit_id);
  void Add(Double_t p_value[7], Double_t n_value[7]);
  void AddMeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id);
  void Clear();

  void ProcessMeanTime();
 
  Int_t  Size() const {return fNMeanTimes;};

  void   SetTimeWindow  (const Double_t in_ns) {fTimeWindowNs = in_ns;};
  void   SetDetectorType(const TString name  ) {fDetectorName = name;};
  void   SetEventId     (const Long64_t in   ) {fEventId = in;};
  const  TString GetDetectorType() const {return fDetectorName;};

  MeanTime *GetMeanTimeObject(Int_t index);
  Double_T  GetMeanTime(Int_t index);

  void Print();
 
private:

  Double_t  fTimeWindowNs;
  TString   fDetectorName;
  Int_t     fNMeanTimes;
  
  Long64_t  fEventId;

  Double_t  fPositiveValue[7];
  Double_t  fNegativeValue[7];

  ClassDef(MeanTimeContainer,0);

};
