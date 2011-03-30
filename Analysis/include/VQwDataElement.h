/*!
 * \file   VQwDataElement.h
 * \brief  Definition of the pure virtual base class of all data elements
 *
 * \author P. M. King
 * \date   2007-05-08 15:40
 */

#ifndef __VQWDATAELEMENT__
#define __VQWDATAELEMENT__

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>

#include "Rtypes.h"
#include "TString.h"
#include "TDirectory.h"
#include "TH1.h"
#include "QwTypes.h"

/**
 *  \class   VQwDataElement
 *  \ingroup QwAnalysis
 *  \brief   The pure virtual base class of all data elements
 *
 * Each stream of data inherits from this virtual base class, which requires
 * some standard operations on it such as ratios, summing, subtraction.  The
 * specific implementation of those operation is left to be implemented by the
 * implemented inherited classes, but this class sets up the structure.
 *
 * As an example, all individual VQWK channels inherit from this class and
 * implement the pure virtual functions of VQwDataElement.
 *
 * \dot
 * digraph example {
 *   node [shape=round, fontname=Helvetica, fontsize=10];
 *   VQwDataElement [ label="VQwDataElement" URL="\ref VQwDataElement"];
 *   QwVQWK_Channel [ label="QwVQWK_Channel" URL="\ref QwVQWK_Channel"];
 *   VQwDataElement -> QwVQWK_Channel;
 * }
 * \enddot
 */
class VQwDataElement {
 public:
  /// Flag to be used to decide which data needs to be histogrammed and
  /// entered in the tree
  enum EDataToSave {kRaw = 0, kDerived};

  
   


 public:

  VQwDataElement();
  virtual ~VQwDataElement();

  /*! \brief Is the name of this element empty? */
  Bool_t IsNameEmpty() const { return fElementName.IsNull(); }
  /*! \brief Set the name of this element */
  void SetElementName(const TString &name) { fElementName = name; }
  /*! \brief Get the name of this element */
  virtual const TString& GetElementName() const { return fElementName; }

  /*! \brief Clear the event data in this element */
  virtual void  ClearEventData() = 0;
  /*! \brief Process the CODA event buffer for this element */
  virtual Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement=0) = 0;

  /*! \brief Get the number of data words in this data element */
  size_t GetNumberOfDataWords() {return fNumberOfDataWords;}

  /*! \brief Get the number of subelements in this data element */
  size_t GetNumberOfSubelements() {return fNumberOfSubElements;};
  
  Int_t GetRawValue() const       {return this->GetRawValue(0);};
  Double_t GetValue() const       {return this->GetValue(0);};
  Double_t GetValueM2() const     {return this->GetValueM2(0);};
  Double_t GetValueError() const  {return this->GetValueError(0);};
  Double_t GetValueWidth() const  {return this->GetValueWidth(0);};
  virtual Int_t GetRawValue(size_t element) const      {return 0;};
  virtual Double_t GetValue(size_t element) const      {return 0.0;};
  virtual Double_t GetValueM2(size_t element) const    {return 0.0;};
  virtual Double_t GetValueError(size_t element) const {return 0.0;};
  Double_t GetValueWidth(size_t element) const {
    Double_t width;
    if (fGoodEventCount>0){
      width = (GetValueError(element)*std::sqrt(Double_t(fGoodEventCount))); 
    } else {
      width = 0.0;
    } 
    return width;
  };
  UInt_t GetGoodEventCount() const { return fGoodEventCount; };

  /*! \brief Assignment operator */
  virtual VQwDataElement& operator= (const VQwDataElement &value);
  /*! \brief Addition-assignment operator */
  virtual VQwDataElement& operator+= (const VQwDataElement &value)
    { std::cerr << "Operation += not defined!" << std::endl; return *this; }
  /*! \brief Subtraction-assignment operator */
  virtual VQwDataElement& operator-= (const VQwDataElement &value)
    { std::cerr << "Operation -= not defined!" << std::endl; return *this; }
  /*! \brief Sum operator */
  virtual void Sum(const VQwDataElement &value1, const VQwDataElement &value2)
    { std::cerr << "Sum not defined!" << std::endl; }
  /*! \brief Difference operator */
  virtual void Difference(const VQwDataElement &value1, const VQwDataElement &value2)
    { std::cerr << "Difference not defined!" << std::endl; }
  /*! \brief Ratio operator */
  virtual void Ratio(const VQwDataElement &numer, const VQwDataElement &denom)
    { std::cerr << "Ratio not defined!" << std::endl; }

  /*! \brief Construct the histograms for this data element */
  virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  /*! \brief Fill the histograms for this data element */
  virtual void  FillHistograms() = 0;
  /*! \brief Delete the histograms for this data element */
  void  DeleteHistograms();

  /*! \brief Print single line of value and error of this data element */
  virtual void PrintValue() const { }
  /*! \brief Print multiple lines of information about this data element */
  virtual void PrintInfo() const { std::cout << GetElementName() << std::endl; }


  /*! \brief set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  virtual void SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability){std::cerr << "SetSingleEventCuts not defined!" << std::endl; };
  /*! \brief report number of events falied due to HW and event cut faliure */
  Int_t GetEventcutErrorCounters();

  /*! \brief return the error flag on this channel/device*/
  UInt_t GetEventcutErrorFlag();

  void ResetErrorFlag(UInt_t flag){
    fErrorFlag=flag;
  }

  
  
  /*! \brief Return the name of the inheriting subsystem name*/
  TString GetSubsystemName() const {
    return fSubsystemName;
  }

   /*! \brief Set the name of the inheriting subsystem name*/
  void SetSubsystemName(TString sysname){
    fSubsystemName=sysname;
  }
  
   /*! \brief Return the type of the beam instrument*/
  TString GetModuleType() const {
    return fModuleType;
  }

   /*! \brief set the type of the beam instrument*/
  void SetModuleType(TString ModuleType){
    fModuleType=ModuleType;
  }

 protected:
  /*! \brief Set the number of data words in this data element */
  void SetNumberOfDataWords(const UInt_t &numwords) {fNumberOfDataWords = numwords;}
  /*! \brief Set the number of data words in this data element */
  void SetNumberOfSubElements(const size_t elements) {fNumberOfSubElements = elements;};

  /*! \brief Set the flag indicating if raw or derived values are
   *         in this data element */
  void SetDataToSave(TString datatosave) {
    if      (datatosave == "raw")
      fDataToSave = kRaw;
    else if (datatosave == "derived")
      fDataToSave = kDerived;
    else
      fDataToSave = kRaw; // wdc, added default fall-through
  }

  /*! \brief Checks that the requested element is in range, to be
   *         used in accesses to subelements similar to
   *         std::vector::at(). */
  void RangeCheck(size_t element) const {
    if (element<0 || element >= fNumberOfSubElements){
      TString loc="VQwDataElement::RangeCheck for "
	+this->GetElementName()+" failed for subelement "+Form("%d",element);
      throw std::out_of_range(loc.Data());
    }
  };


 protected:
  TString fElementName; ///< Name of this data element
  UInt_t  fNumberOfDataWords; ///< Number of raw data words in this data element
  UInt_t  fNumberOfSubElements; ///< Number of subelements in this data element

  EDataToSave fDataToSave;

  UInt_t fGoodEventCount;  ///< Number of good events accumulated in this element


  /// Histograms associated with this data element
  std::vector<TH1*> fHistograms;

  //name of the inheriting subsystem
  TString  fSubsystemName;
  //Data module Type 
  TString  fModuleType;

  /// Unique error code for HW failures
  UInt_t fDeviceErrorCode; 
  //Error flag
  UInt_t fErrorFlag;
  UInt_t fDefErrorFlag;

}; // class VQwDataElement

inline VQwDataElement::VQwDataElement():
  fElementName(""), fNumberOfDataWords(0),
  fNumberOfSubElements(0), fDataToSave(kRaw),
  fGoodEventCount(0),
  fSubsystemName(""), fModuleType(""),
  fErrorFlag(0), fDefErrorFlag(0)
{
  fHistograms.clear();
}

inline VQwDataElement::~VQwDataElement(){
}

/**
 * Assignment operator sets the name and number of data words
 */
inline VQwDataElement& VQwDataElement::operator= (const VQwDataElement &value){
  fElementName       = value.fElementName;
  fNumberOfDataWords = value.fNumberOfDataWords;
  fNumberOfSubElements = value.fNumberOfSubElements;
  return *this;
}


/**
 * Delete the histograms for with this data element
 */
inline void VQwDataElement::DeleteHistograms()
{
  for (size_t i=0; i<fHistograms.size(); i++){
    if (fHistograms.at(i) != NULL){
      fHistograms.at(i)->Delete();
      fHistograms.at(i) =  NULL;
    }
  }
  fHistograms.clear();
}

inline UInt_t VQwDataElement::GetEventcutErrorFlag(){//return the error flag
  if (((fErrorFlag & kGlobalCut) == kGlobalCut) && fDeviceErrorCode>0){
    //we care only about global cuts
    return fErrorFlag;
  }
  return 0;
};



#endif // __VQWDATAELEMENT__
