/**********************************************************\
* File: VQwDataElement.h                                     *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/


#ifndef __VQWDATAELEMENT__
#define __VQWDATAELEMENT__

#include <vector>
#include "Rtypes.h"
#include "TString.h"
#include "TDirectory.h"
#include "TH1.h"

class VQwDataElement{
 public:
  VQwDataElement(){};
  //  VQwDataElement(UInt_t numwords):fNumberOfDataWords(numwords) {};
  virtual ~VQwDataElement();

  void SetElementName(const TString &name) {fElementName = name;};
  TString GetElementName() {return fElementName;};

  virtual void  ClearEventData() = 0;
  virtual Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left) = 0;

  virtual VQwDataElement& operator= (const VQwDataElement &value);
  virtual VQwDataElement& operator+= (const VQwDataElement &value)                    {std::cerr << "Crap!!!"<<std::endl;return *this;};
  virtual VQwDataElement& operator-= (const VQwDataElement &value)                    {std::cerr << "Crap!!!"<<std::endl;return *this;};
  virtual void Sum(const VQwDataElement &value1, const VQwDataElement &value2)        {std::cerr << "Crap!!!"<<std::endl;};
  virtual void Difference(const VQwDataElement &value1, const VQwDataElement &value2) {std::cerr << "Crap!!!"<<std::endl;};
  virtual void Ratio(const VQwDataElement &numer, const VQwDataElement &denom)        {std::cerr << "Crap!!!"<<std::endl;};

  virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  virtual void  FillHistograms() = 0;
  void  DeleteHistograms();


  size_t GetNumberOfDataWords() {return fNumberOfDataWords;};

 protected:
  void SetNumberOfDataWords(const UInt_t &numwords) {fNumberOfDataWords = numwords;};

 protected:
  TString fElementName;
  UInt_t  fNumberOfDataWords;

  std::vector<TH1*> fHistograms;
};


inline VQwDataElement::~VQwDataElement(){
};

inline VQwDataElement& VQwDataElement::operator= (const VQwDataElement &value){
  fElementName       = value.fElementName;
  fNumberOfDataWords = value.fNumberOfDataWords;
  return *this;
};

inline void  VQwDataElement::DeleteHistograms(){
  for (size_t i=0; i<fHistograms.size(); i++){
    if (fHistograms.at(i) != NULL){
      fHistograms.at(i)->Delete();
      fHistograms.at(i) =  NULL;
    }
  }
  fHistograms.clear();
};



#endif
