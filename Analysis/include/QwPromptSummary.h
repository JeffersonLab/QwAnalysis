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
 *  \brief one F1TDC configuration and reference signal(s) holder
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


  void SetMean                (const Double_t in) { fMean=in; };
  void SetMeanError           (const Double_t in) { fMeanError=in; };
  void SetMeanUnit            (const TString  in) { fMeanUnit=in; };

  void SetAsymmetryDifference           (const Double_t in) { fAsymmetryDifference=in; };
  void SetAsymmetryDifferenceError      (const Double_t in) { fAsymmetryDifferenceError=in; };
  void SetAsymmetryDifferenceUnit       (const TString  in) { fAsymmetryDifferenceUnit=in; };
  void SetAsymmetryDifferenceWidth      (const Double_t in) { fAsymmetryDifferenceWidth=in; };
  void SetAsymmetryDifferenceWidthError (const Double_t in) { fAsymmetryDifferenceWidthError=in; };
  void SetAsymmetryDifferenceWidthUnit  (const TString  in) { fAsymmetryDifferenceWidthUnit=in; };


  TString GetCSVSummary();
  TString GetTextSummary();


 private:

  TString fElementName;
  
  Double_t fMean;
  Double_t fMeanError;
  TString  fMeanUnit;

  Double_t fAsymmetryDifference;
  Double_t fAsymmetryDifferenceError;
  TString  fAsymmetryDifferenceUnit;

  Double_t fAsymmetryDifferenceWidth;
  Double_t fAsymmetryDifferenceWidthError;
  TString  fAsymmetryDifferenceWidthUnit;

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
  void FillDataInElement(TString name, Double_t mean, Double_t mean_err, TString mean_unit, Double_t asym_diff, Double_t asym_diff_err, TString asym_diff_unit, Double_t asym_diff_width, Double_t asym_diff_width_err, TString asym_diff_width_unit);

  //  void Print(const Option_t* options = 0) const;

  Int_t  GetSize()         const {return fNElements;};
  Int_t  Size()            const {return fNElements;};
  Int_t  HowManyElements() const {return fNElements;};

  void PrintCSV();
  void PrintTextSummary();

private:


  PromptSummaryElement* GetElementByName(TString name);

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
