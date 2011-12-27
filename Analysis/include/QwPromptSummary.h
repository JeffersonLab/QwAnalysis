#ifndef __QwPromptSummary__
#define __QwPromptSummary__
/**
 *  \file   QwPromptSummary.h
 *  \brief  
 *  \author jhlee@jlab.org
 *  \date   Friday, December 16 10:29:10 EST 2011
 */

#include <iostream>


#include "TObject.h"
#include "TClonesArray.h"
#include "TList.h"
#include "TString.h"

#include "TROOT.h"

/**
 *  \class QwPromptSummary
 *  \ingroup QwAnalysis
 *
 *  \brief 
 *
 */

class PromptSummaryElement :  public TObject
{
 public:
  PromptSummaryElement();
  PromptSummaryElement(TString name);
  virtual ~PromptSummaryElement();
  //  friend std::ostream& operator<<(std::ostream& os, const PromptSummaryElement &ps_element);

  void FillData(Double_t mean, Double_t mean_err, TString mean_unit, Double_t asym_diff, Double_t asym_diff_err, TString asym_diff_unit, Double_t asym_diff_width, Double_t asym_diff_width_err, TString asym_diff_width_unit);

  void SetName                (const TString in)  { fElementName=in;};
  TString GetName () {return fElementName;};


  // Mean      : fHardwareBlockSumM2 
  // MeanError : fHardwareBlockSumError = sqrt(fHardwareBlockSumM2) / fGoodEventCount;

  void SetMean                (const Double_t in) { fMean=in; };
  void SetMeanError           (const Double_t in) { fMeanError=in; };
  void SetMeanUnit            (const TString  in) { fMeanUnit=in; };

  // Asymmetry : 

  void SetAsymmetry           (const Double_t in) { fAsymmetry=in; };
  void SetAsymmetryError      (const Double_t in) { fAsymmetryError=in; };
  void SetAsymmetryUnit       (const TString  in) { fAsymmetryUnit=in; };
  void SetAsymmetryWidth      (const Double_t in) { fAsymmetryWidth=in; };
  void SetAsymmetryWidthError (const Double_t in) { fAsymmetryWidthError=in; };
  void SetAsymmetryWidthUnit  (const TString  in) { fAsymmetryWidthUnit=in; };


  TString GetCSVSummary();
  TString GetTextSummary();
  //

  // This is not sigma, but error defined in QwVQWK_Channel::CalculateRunningAverage() in QwVQWK_Channel.cc as follows
  // fHardwareBlockSumError = sqrt(fHardwareBlockSumM2) / fGoodEventCount;
  //
 private:

  TString fElementName;
  
  Double_t fMean;
  Double_t fMeanError;
  TString  fMeanUnit;

  Double_t fAsymmetry;
  Double_t fAsymmetryError;
  TString  fAsymmetryUnit;

  Double_t fAsymmetryWidth;
  Double_t fAsymmetryWidthError;
  TString  fAsymmetryWidthUnit;

  ClassDef(PromptSummaryElement,0);

};


class QwPromptSummary  :  public TObject
{

 public:
  QwPromptSummary();
  QwPromptSummary(Int_t run_number, Int_t runlet_number);
  virtual ~QwPromptSummary();
  //  friend std::ostream& operator<<(std::ostream& os, const QwF1TDC &f1tdc);


  Int_t                    fNElements;
  TObjArray               *fElementList; 

  void SetRunNumber(const Int_t in) {fRunNumber = in;};
  const Int_t GetRunNumber() {return fRunNumber;};
  
  void SetRunletNumber(const Int_t in) {fRunletNumber = in;};
  const Int_t GetRunletNumber() {return fRunletNumber;};


  void AddElement(PromptSummaryElement *in);
  PromptSummaryElement* GetElementByName(TString name);
  
  void FillDataInElement(TString name, Double_t mean, Double_t mean_err, TString mean_unit, Double_t asym_diff, Double_t asym_diff_err, TString asym_diff_unit, Double_t asym_diff_width, Double_t asym_diff_width_err, TString asym_diff_width_unit);
  
  void FillYieldToElement(Double_t mean, Double_t mean_error, TString mean_unit);
  void FillAsymmetryToElement(Double_t asym_diff, Double_t asym_diff_err, TString asym_diff_unit);
  //  void FillDifferenceToElement(Double_t asym_diff, Double_t asym_diff_err, TString asym_diff_unit);

  //  void Print(const Option_t* options = 0) const;

  Int_t  GetSize()         const {return fNElements;};
  Int_t  Size()            const {return fNElements;};
  Int_t  HowManyElements() const {return fNElements;};

  void PrintCSV();
  void PrintTextSummary();

private:


 

  TString PrintTextSummaryHeader();
  TString PrintTextSummaryTailer();
  TString PrintCSVHeader();

  void    SetupElementList();
  
  Int_t   fRunNumber;
  Int_t   fRunletNumber;

  Bool_t  fLocalDebug;

  ClassDef(QwPromptSummary,0);

};


#endif
