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

  VQwDataElement(){};
  //  VQwDataElement(UInt_t numwords):fNumberOfDataWords(numwords) {};
  virtual ~VQwDataElement();

  /*! \brief Is the name of this element empty? */
  Bool_t IsNameEmpty() const { return fElementName.IsNull(); };
  /*! \brief Set the name of this element */
  void SetElementName(const TString &name) { fElementName = name; };
  /*! \brief Get the name of this element */
  virtual const TString& GetElementName() const { return fElementName; };

  /*! \brief Clear the event data in this element */
  virtual void  ClearEventData() = 0;
  /*! \brief Process the CODA event buffer for this element */
  virtual Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left, UInt_t subelement=0) = 0;

  /*! \brief Assignment operator */
  virtual VQwDataElement& operator= (const VQwDataElement &value);
  /*! \brief Addition-assignment operator */
  virtual VQwDataElement& operator+= (const VQwDataElement &value)
    { std::cerr << "Operation += not defined!" << std::endl; return *this; };
  /*! \brief Subtraction-assignment operator */
  virtual VQwDataElement& operator-= (const VQwDataElement &value)
    { std::cerr << "Operation -= not defined!" << std::endl; return *this; };
  /*! \brief Sum operator */
  virtual void Sum(const VQwDataElement &value1, const VQwDataElement &value2)
    { std::cerr << "Sum not defined!" << std::endl; };
  /*! \brief Difference operator */
  virtual void Difference(const VQwDataElement &value1, const VQwDataElement &value2)
    { std::cerr << "Difference not defined!" << std::endl; };
  /*! \brief Ratio operator */
  virtual void Ratio(const VQwDataElement &numer, const VQwDataElement &denom)
    { std::cerr << "Ratio not defined!" << std::endl; };

  /*! \brief Construct the histograms for this data element */
  virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  /*! \brief Fill the histograms for this data element */
  virtual void  FillHistograms() = 0;
  /*! \brief Delete the histograms for this data element */
  void  DeleteHistograms();

  /*! \brief Print single line of value and error of this data element */
  virtual void PrintValue() const { };
  /*! \brief Print multiple lines of information about this data element */
  virtual void PrintInfo() const { std::cout << GetElementName() << std::endl; };

  /*! \brief Get the number of data words in this data element */
  size_t GetNumberOfDataWords() {return fNumberOfDataWords;};
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure
  
  /*! \brief Return the name of the inheriting subsystem name*/
  TString GetSubsystemName() const {
    return fSubsystemName;
  };

   /*! \brief Set the name of the inheriting subsystem name*/
  void SetSubsystemName(TString sysname){
    fSubsystemName=sysname;
  };
  
   /*! \brief Return the type of the beam instrument*/
  TString GetModuleType() const {
    return fModuleType;
  };

   /*! \brief set the type of the beam instrument*/
  void SetModuleType(TString ModuleType){
    fModuleType=ModuleType;
  };

 protected:
  /*! \brief Set the number of data words in this data element */
  void SetNumberOfDataWords(const UInt_t &numwords) {fNumberOfDataWords = numwords;};

 protected:
  TString fElementName; ///< Name of this data element
  UInt_t  fNumberOfDataWords; ///< Number of data words in this data element

  /// Histograms associated with this data element
  std::vector<TH1*> fHistograms;

  //name of the inheriting subsystem
  TString  fSubsystemName;
  //Data module Type 
  TString  fModuleType;

}; // class VQwDataElement


inline VQwDataElement::~VQwDataElement(){
};

/**
 * Assignment operator sets the name and number of data words
 */
inline VQwDataElement& VQwDataElement::operator= (const VQwDataElement &value){
  fElementName       = value.fElementName;
  fNumberOfDataWords = value.fNumberOfDataWords;
  return *this;
};


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
};



#endif // __VQWDATAELEMENT__
