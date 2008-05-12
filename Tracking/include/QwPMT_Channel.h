/**********************************************************\
* File: QwPMT_Channel.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QwPMT_CHANNEL__
#define __QwPMT_CHANNEL__

#include <vector>
#include <TTree.h>

#include "VQwDataElement.h"
#include "QwHistogramHelper.h"


extern QwHistogramHelper gQwHists;

class QwPMT_Channel: public VQwDataElement {
  /******************************************************************
   *  Class: QwPMT_Channel
   *         
   ******************************************************************/
 public:
  QwPMT_Channel() { };
  QwPMT_Channel(TString name){
    InitializeChannel(name);
  };
  ~QwPMT_Channel() {DeleteHistograms();};

  void  InitializeChannel(TString name){
    SetElementName(name);
  };

  void  ClearEventData();
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left){};

  void  SetValue(Double_t data){fValue= data;};

  void  ProcessEvent();

  QwPMT_Channel& operator=  (const QwPMT_Channel &value);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values);
  void  FillTreeVector(std::vector<Float_t> &values);

 protected:
  

 private:
  static const Bool_t kDEBUG;

  /*  ADC Calibration                     */
  static const Double_t kPMT_VoltsPerBit;

  /*  Channel information data members    */
  

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  /*  Event data members */
  Double_t fValue;

};


const Bool_t QwPMT_Channel::kDEBUG = kFALSE;


/*!  Conversion factor to translate the average bit count in an ADC
 *   channel into average voltage.
 *   The base factor is 0.07929 mV per count, and zero counts corresponds 
 *   to zero voltage.
 */
const Double_t QwPMT_Channel::kPMT_VoltsPerBit = 76.29e-6;


void QwPMT_Channel::ClearEventData(){
  fValue   = 0;
};


void  QwPMT_Channel::ProcessEvent()
{
  
};


void  QwPMT_Channel::ConstructHistograms(TDirectory *folder, TString &prefix){
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else {
    //  Now create the histograms.
    TString basename, fullname;
    basename = prefix + GetElementName();

    fHistograms.resize(1, NULL);
    size_t index=0;
    fHistograms[index]   = gQwHists.Construct1DHist(basename);
    index += 1;
  }
};

void  QwPMT_Channel::FillHistograms(){
  size_t index=0;
  if (GetElementName()==""){
    //  This channel is not used, so skip creating the histograms.
  } else {
    if (fHistograms[index] != NULL)
      fHistograms[index]->Fill(this->fValue);
    index += 1;
  }
};

void  QwPMT_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip setting up the tree.
  } else {  
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = "/F";

    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    tree->Branch(basename, &(values[fTreeArrayIndex]), list);
  }
};

void  QwPMT_Channel::FillTreeVector(std::vector<Float_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the tree vector.
  } else if (fTreeArrayNumEntries<=0){
    std::cerr << "QwPMT_Channel::FillTreeVector:  fTreeArrayNumEntries=="
	      << fTreeArrayNumEntries << std::endl;
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries){
    std::cerr << "QwPMT_Channel::FillTreeVector:  values.size()=="
	      << values.size()
	      << "; fTreeArrayIndex+fTreeArrayNumEntries=="
	      << fTreeArrayIndex+fTreeArrayNumEntries
	      << std::endl;
  } else {
    size_t index=fTreeArrayIndex;
    values[index++] = this->fValue;
  }
};



QwPMT_Channel& QwPMT_Channel::operator= (const QwPMT_Channel &value){
  if (GetElementName()!=""){
    this->fValue  = value.fValue;
  }
  return *this;
};


#endif
