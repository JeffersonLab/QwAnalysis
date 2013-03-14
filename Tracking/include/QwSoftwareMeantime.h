#ifndef __QwSoftwareMeantime__
#define __QwSoftwareMeantime__
/**
 *  \file   QwSoftwareMeantime.h
 *  \brief  
 *  \author Jeong Han Lee, jhlee@jlab.org
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
  
  const Int_t GetSoftwareMeantimeHitElement() const { return fSoftwareMeantimeHitElementID;};
  const Int_t GetSoftwarePositiveHitElement() const { return fSoftwarePositiveHitElementID;};
  const Int_t GetSoftwareNegativeHitElement() const { return fSoftwareNegativeHitElementID;};
  const Int_t GetSoftwareSubtractHitElement() const { return fSoftwareSubtractHitElementID;};

  Int_t    GetPositiveHitId() const { return fPositiveHitId; };
  Int_t    GetNegativeHitId() const { return fNegativeHitId; };

  Int_t    GetMeanTimeId()    const { return fMeanTimeId; };
  Int_t    GetPlane()         const { return fPlane;};

  void     AddPNValues(Double_t p_value, Double_t n_value, Int_t p_id, Int_t n_id);
  void     SetDetectorType(TString d_type) {fDetectorType = d_type;};
  void     SetMeanTimeId(Int_t id)         {fMeanTimeId = id;};
  void     SetPlane(Int_t in)              {fPlane = in;};

  void     SetHardwareMeanTime(Double_t hardware_meantime);

  void     Print(Bool_t on);

  Bool_t   IsInTimeWindow(Double_t time_window);
  Bool_t   IsHitIDsMatch();

  void     ClearEventData();

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
  Int_t    fPlane;

  static const Int_t fSoftwareMeantimeHitElementID;
  static const Int_t fSoftwarePositiveHitElementID;
  static const Int_t fSoftwareNegativeHitElementID;
  static const Int_t fSoftwareSubtractHitElementID;

  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(MeanTime,0);
  #endif
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
class MeanTimeContainer   :  public TObject
{


 public :
  Int_t                    fNMeanTimes;
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
  void Add(Int_t element, Int_t hit_number, Double_t time_ns);

  Int_t AddMeanTime(TString name, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id);
  Int_t AddMeanTime(TString name, Int_t plane_id, Long64_t ev_id, Double_t p_in, Double_t n_in, Int_t p_id, Int_t n_id);

  void ClearEventData();

  void ProcessMeanTime();
 
  Int_t  SoftwareMTSize() const {return fNMeanTimes;};
  Int_t  HardwareMTSize() const {return fNHarewareMeanTimes;};

  void   SetSystemName  (const TString name  ) { fDetectorName = name;};
  void   SetPlane       (const Int_t   in    ) { fPlane = in;};
  void   SetTimeWindow  (const Double_t in_ns) { fTimeWindowNs = in_ns;};
  void   SetDetectorType(const TString name  ) { fDetectorName = name;};
  void   SetEventId     (const Long64_t in   ) { fEventId = in;};
  void   SetDisableMatchHardwareMeantime(const Bool_t disable)      { fDisableMatchHardwareMeanTime=disable;};
  const  TString GetDetectorType() const {return fDetectorName;};
  Int_t    GetPlane()         const { return fPlane;};
  
  MeanTime *GetMeanTimeObject(Int_t index);
  Double_t  GetMeanTime(Int_t index);

  void Print(Bool_t on);
 
private:

  Double_t  fTimeWindowNs;
  TString   fDetectorName;
  Int_t     fPlane;
  
  Long64_t  fEventId;

  Double_t  fPositiveValue[7];
  Double_t  fNegativeValue[7];
  Double_t  fHardwareMeantimeValue[7];

  void      MatchHardwareMeanTime();
  Bool_t    IsHarewareMatchSoftware();

  Bool_t    fDisableMatchHardwareMeanTime;


  #if ROOT_VERSION_CODE < ROOT_VERSION(5,90,0)
    ClassDef(MeanTimeContainer,0);
  #endif

};


#endif
