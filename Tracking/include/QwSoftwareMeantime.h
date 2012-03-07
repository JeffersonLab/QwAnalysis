#ifndef __QwSoftwareMeantime__
#define __QwSoftwareMeantime__
/**
 *  \file   QwSoftwareMeantime.h
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Wednesday, March  7 15:17:15 EST 2012
 */

#include <iostream>
#include <cmath>


#include "TObject.h"
#include "TObjArray.h"
#include "TString.h"
#include "TMath.h"

#include "TROOT.h"




/**
 *  \class MeanTime
 *  \ingroup QwAnalysis
 *
 *  \brief one software meantim holder
 *
 */

// A good meantime holder 
class MeanTime  :  public TObject
{

public:

  MeanTime();
  MeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id);
  virtual ~MeanTime();

  Bool_t   HasMeanTime()            {return fHasValue;};

  Double_t GetMeanTime()      const { return fMeanTime; };
  Double_t GetSubtractTime()  const { return fSubtractTime; };

  Double_t GetDiffHardSoftMeanTime() const { return fDiffHardSoftMeanTime; };

  Double_t GetPositiveValue() const { return fPositiveValue; };
  Double_t GetNegativeValue() const { return fNegativeValue; };

  Int_t    GetPositiveHitId() const { return fPositiveHitId; };
  Int_t    GetNegativeHitId() const { return fNegativeHitId; };

  Int_t    GetMeanTimeId()    const { return fMeanTimeId; };


  void     AddPNValues(Double_t p_value, Double_t n_value, Int_t p_id, Int_t n_id);
  void     SetDetectorType(TString d_type) {fDetectorType = d_type;};
  void     SetMeanTimeId(Int_t id)         {fMeanTimeId = id;};

  void     SetHardwareMeanTime(Double_t hardware_meantime);

  void     Print(Bool_t on);

  Bool_t   IsInTimeWindow(Double_t time_window);
  Bool_t   IsHitIDsMatch();

private:

  Bool_t   fHasValue;
  Double_t fMeanTime;
  Double_t fSubtractTime;  // positive - negative

  Double_t fDiffHardSoftMeanTime; // Hardware MT - Software MT
  Double_t fHardWareMeanTime;

  Double_t fPositiveValue;
  Double_t fNegativeValue;

  Int_t    fPositiveHitId;
  Int_t    fNegativeHitId;
  Int_t    fMeanTimeId;
  
  Long64_t fEventId;
  TString  fDetectorType;

  ClassDef(MeanTime,0);
};




/**
 *  \class MeanTimeContainer
 *  \ingroup QwAnalysis
 *
 *  \brief  Software Meantime container
 * 
 */


// all possible combination of meantimes per an event
// a method to find good meantimes.
// ....
class MeanTimeContainer 
{


 public :
  //  Int_t                    fNMeanTimes;
  TObjArray               *fMeanTimeList; 
  Int_t                    fNHarewareMeanTimes;
  Int_t                    fNPositive;
  Int_t                    fNNegative;

public:

  MeanTimeContainer();
  MeanTimeContainer(TString name);
  virtual ~MeanTimeContainer();

  

  void Add(Double_t p_value, Double_t n_value, Int_t hit_id);
  void Add(Double_t p_value[7], Double_t n_value[7]);
  void Add(Double_t p_value[7], Double_t n_value[7], Double_t hardware_meantime[7]);
  Int_t AddMeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id);
  void Clear();

  void ProcessMeanTime();
 
  Int_t  Size()           const {return fNMeanTimes;};
  Int_t  HardwareMTSize() const {return fNHarewareMeanTimes;};

  void   SetSystemName  (const TString name  ) { fDetectorName = name;};
  void   SetTimeWindow  (const Double_t in_ns) {fTimeWindowNs = in_ns;};
  void   SetDetectorType(const TString name  ) {fDetectorName = name;};
  void   SetEventId     (const Long64_t in   ) {fEventId = in;};

  const  TString GetDetectorType() const {return fDetectorName;};

  MeanTime *GetMeanTimeObject(Int_t index);
  Double_t  GetMeanTime(Int_t index);

  void Print(Bool_t on);
 
private:

  Double_t  fTimeWindowNs;
  TString   fDetectorName;
  Int_t     fNMeanTimes;
  
  Long64_t  fEventId;

  Double_t  fPositiveValue[7];
  Double_t  fNegativeValue[7];
  Double_t  fHardwareMeantimeValue[7];

  void      MatchHardwareMeanTime();
  Bool_t    IsHarewareMatchSoftware();

  ClassDef(MeanTimeContainer,0);

};


#endif
