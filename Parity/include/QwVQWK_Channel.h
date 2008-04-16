/**********************************************************\
* File: QwVQWK_Channel.h                                           *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2007-05-08 15:40>                           *
\**********************************************************/

#ifndef __QwVQWK_CHANNEL__
#define __QwVQWK_CHANNEL__

#include <vector>
#include <TTree.h>

#include "VQwDataElement.h"
#include "QwHistogramHelper.h"


extern QwHistogramHelper gQwHists;

class QwVQWK_Channel: public VQwDataElement {
  /******************************************************************
   *  Class: QwVQWK_Channel
   *         Base class containing decoding functions for the VQWK_Channel
   *         6 32-bit datawords.
   *         The functions in this class will decode a single channel
   *         worth of VQWK_Channel data and provide the components
   *         through member functions.
   ******************************************************************/
 public:
  QwVQWK_Channel() { };
  QwVQWK_Channel(TString name){
    InitializeChannel(name);
  };
  ~QwVQWK_Channel() {DeleteHistograms();};

  void  InitializeChannel(TString name){
    SetElementName(name);
    SetNumberOfDataWords(6);
    fBlocksPerEvent = 4;
  };

  void  ClearEventData();
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left);

  void  ProcessEvent();

  QwVQWK_Channel& operator=  (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator+= (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator-= (const QwVQWK_Channel &value);
  void Sum(QwVQWK_Channel &value1, QwVQWK_Channel &value2);
  void Difference(QwVQWK_Channel &value1, QwVQWK_Channel &value2);
  void Ratio(QwVQWK_Channel &numer, QwVQWK_Channel &denom);

  Bool_t MatchSequenceNumber(size_t seqnum);
  Bool_t MatchNumberOfSamples(size_t numsamp);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values);
  void  FillTreeVector(std::vector<Float_t> &values);

  Double_t GetBlockValue(size_t blocknum){return fBlock[blocknum];};
  Double_t GetHardwareSum(){return fHardwareBlockSum;};
  Double_t GetSoftwareSum(){return fSoftwareBlockSum;};

  size_t GetSequenceNumber(){return (fSequenceNumber);};
  size_t GetNumberOfSamples(){return (fNumberOfSamples);};

 protected:
  

 private:
  static const Bool_t kDEBUG;

  /*  ADC Calibration                     */
  static const Double_t kVQWK_VoltsPerBit;

  /*  Channel information data members    */
  

  /*  Channel configuration data memebers */
  UInt_t  fSamplesPerBlock;
  UInt_t  fBlocksPerEvent;

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  /*  Event data members */
  Double_t fBlock[4];         /*! Array of the sub-block data             */
  Double_t fHardwareBlockSum; /*! Module-based sum of the four sub-blocks */
  Double_t fSoftwareBlockSum; /*! Sum of the data in the four sub-blocks  */
  size_t fSequenceNumber;     /*! Event sequence number for this channel  */
  size_t fNumberOfSamples;    /*! Number of samples in the event          */
};


const Bool_t QwVQWK_Channel::kDEBUG = kFALSE;


/*!  Conversion factor to translate the average bit count in an ADC
 *   channel into average voltage.
 *   The base factor is 0.07929 mV per count, and zero counts corresponds 
 *   to zero voltage.
 */
const Double_t QwVQWK_Channel::kVQWK_VoltsPerBit = 76.29e-6;


void QwVQWK_Channel::ClearEventData(){
  for (size_t i=0; i<4; i++){
    fBlock[i]         = 0;
  }
  fHardwareBlockSum = 0;
  fSoftwareBlockSum = 0;
  fSequenceNumber   = 0;
  fNumberOfSamples  = 0;
};

Int_t QwVQWK_Channel::ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left){
  UInt_t words_read = 0;
  Long_t localbuf[6];
  if (GetElementName()==""){
    //  This channel is not used, but is present in the data stream.
    //  Skip over this data.
    words_read = fNumberOfDataWords;
  } else if (num_words_left >= fNumberOfDataWords){
    for (size_t i=0; i<6; i++){
      localbuf[i] = buffer[i];
    }
    fSoftwareBlockSum = 0.0;
    for (size_t i=0; i<4; i++){
      fBlock[i] = Double_t(localbuf[i]);
      fSoftwareBlockSum += fBlock[i];
    }
    fHardwareBlockSum = Double_t(localbuf[4]);

    /*  Permanent change in the structure of the 6th word of the ADC readout.
     *  The upper 16 bits are the number of samples, and the upper 8 of the
     *  lower 16 are the sequence number.  This matches the structure of
     *  the ADC readout in block read mode, and now also in register read mode.
     *  P.King, 2007sep04.
     */
    fSequenceNumber   = (localbuf[5]>>8)  & 0xFF;
    fNumberOfSamples  = (localbuf[5]>>16) & 0xFFFF;

    words_read = fNumberOfDataWords;

    if (kDEBUG && GetElementName()=="Bar1Right"){
      //    if (num_words_left == 6){
      std::cout << std::hex
		<< buffer[0] << " " << buffer[1] << " "
		<< buffer[2] << " " << buffer[3] << " "
		<< buffer[4] << " " << buffer[5] << " --- "
		<< std::dec
		<< fBlock[0] << " " << fBlock[1] << " "
		<< fBlock[2] << " " << fBlock[3] << " "
		<< fSoftwareBlockSum << " " << fHardwareBlockSum
		<< " " << fSequenceNumber << " " << fNumberOfSamples
		<< std::endl;
    }
  } else {
    std::cerr << "QwVQWK_Channel::ProcessEvBuffer: Not enough words!"
	      << std::endl;
  }
  return words_read;
};

void  ProcessEvent()
{
  
};


void  QwVQWK_Channel::ConstructHistograms(TDirectory *folder, TString &prefix){
  //  If we have defined a subdirectory in the ROOT file, then change into it.
  if (folder != NULL) folder->cd();

  if (GetElementName()==""){
    //  This channel is not used, so skip filling the histograms.
  } else {
    //  Now create the histograms.
    TString basename, fullname;
    basename = prefix + GetElementName();

    fHistograms.resize(8+4+2, NULL);
    size_t index=0;
    for (size_t i=0; i<4; i++){
      fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_block%d_raw",i));
      fHistograms[index+1] = gQwHists.Construct1DHist(basename+Form("_block%d_volt",i));
      index += 2;
    }
    fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_hw_raw"));
    fHistograms[index+1] = gQwHists.Construct1DHist(basename+Form("_hw_volt"));
    index += 2;
    fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_sw_raw"));
    fHistograms[index+1] = gQwHists.Construct1DHist(basename+Form("_sw_volt"));
    index += 2;
    fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_seqnum"));
    index += 1;
    fHistograms[index]   = gQwHists.Construct1DHist(basename+Form("_numsamples"));
    index += 1;
  }
};

void  QwVQWK_Channel::FillHistograms(){
  size_t index=0;
  if (GetElementName()==""){
    //  This channel is not used, so skip creating the histograms.
  } else {
    for (size_t i=0; i<4; i++){
      if (fHistograms[index] != NULL) 
	fHistograms[index]->Fill(this->GetBlockValue(i));
      //    if (fHistograms[index+1] != NULL)
      //      fHistograms[index+1]->Fill(this->GetBlockVoltage(i));
      index += 2;
    }
    if (fHistograms[index] != NULL)
      fHistograms[index]->Fill(this->GetHardwareSum());
    //  if (fHistograms[index] != NULL)
    //    fHistograms[index+1]->Fill(this->GetHardwareSumVoltage());
    index += 2;
    if (fHistograms[index] != NULL)
      fHistograms[index]->Fill(this->GetSoftwareSum());
    //  if (fHistograms[index+1] != NULL)
    //    fHistograms[index+1]->Fill(this->GetSoftwareSumVoltage());
    index += 2;
    if (fHistograms[index] != NULL)
      fHistograms[index]->Fill(this->GetSequenceNumber());
    index += 1;
    if (fHistograms[index] != NULL)
      fHistograms[index]->Fill(this->GetNumberOfSamples());
    index += 1;
  }
};

void  QwVQWK_Channel::ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip setting up the tree.
  } else {  
    TString basename = prefix + GetElementName();
    fTreeArrayIndex  = values.size();

    values.push_back(0.0);
    TString list = "block0/F";
    values.push_back(0.0);
    list += ":block1/F";
    values.push_back(0.0);
    list += ":block2/F";
    values.push_back(0.0);
    list += ":block3/F";
    values.push_back(0.0);
    list += ":hw_sum/F";
    values.push_back(0.0);
    list += ":sw_sum/F";
    if (prefix != "asym"){
      values.push_back(0.0);
      list += ":seqnum/F";
      values.push_back(0.0);
      list += ":num_samples/F";
    }
    fTreeArrayNumEntries = values.size() - fTreeArrayIndex;
    tree->Branch(basename, &(values[fTreeArrayIndex]), list);
    if (kDEBUG && GetElementName()=="Bar1Right"){
      std::cerr << "QwVQWK_Channel::ConstructBranchAndVector: fTreeArrayIndex==" << fTreeArrayIndex
		<< "; fTreeArrayNumEntries==" << fTreeArrayNumEntries
		<< "; values.size()==" << values.size()
		<< "; list==" << list
		<< std::endl;
    }
  }
};

void  QwVQWK_Channel::FillTreeVector(std::vector<Float_t> &values)
{
  if (GetElementName()==""){
    //  This channel is not used, so skip filling the tree vector.
  } else if (fTreeArrayNumEntries<=0){
    std::cerr << "QwVQWK_Channel::FillTreeVector:  fTreeArrayNumEntries=="
	      << fTreeArrayNumEntries << std::endl;
  } else if (values.size() < fTreeArrayIndex+fTreeArrayNumEntries){
    std::cerr << "QwVQWK_Channel::FillTreeVector:  values.size()=="
	      << values.size()
	      << "; fTreeArrayIndex+fTreeArrayNumEntries=="
	      << fTreeArrayIndex+fTreeArrayNumEntries
	      << std::endl;
  } else {
    size_t index=fTreeArrayIndex;
    for (size_t i=0; i<4; i++){
      values[index++] = this->GetBlockValue(i);
    }
    values[index++] = this->GetHardwareSum();
    values[index++] = this->GetSoftwareSum();
    if (index < fTreeArrayIndex+fTreeArrayNumEntries){
      values[index++] = Float_t(this->GetSequenceNumber());
      values[index++] = Float_t(this->GetNumberOfSamples());
    }
  }
  if (kDEBUG && GetElementName()=="Bar1Right"){
    std::cerr << "&(values["<<fTreeArrayIndex<<"])=="<< &(values[fTreeArrayIndex])
	      << std::endl;
    size_t index=fTreeArrayIndex;
    for (size_t i=0; i<fTreeArrayNumEntries; i++){
      std::cerr << "values[" << index << "]==" << values[index++] << " ";
    }
    std::cerr << std::endl;
  }
};



QwVQWK_Channel& QwVQWK_Channel::operator= (const QwVQWK_Channel &value){
  if (GetElementName()!=""){
    for (size_t i=0; i<4; i++){
      this->fBlock[i] = value.fBlock[i];
    }
    this->fHardwareBlockSum = value.fHardwareBlockSum;
    this->fSoftwareBlockSum = value.fSoftwareBlockSum;
    this->fNumberOfSamples  = value.fNumberOfSamples;
    this->fSequenceNumber   = value.fSequenceNumber;
  }
  return *this;
};

QwVQWK_Channel& QwVQWK_Channel::operator+= (const QwVQWK_Channel &value){
  if (GetElementName()!=""){
    for (size_t i=0; i<4; i++){
      this->fBlock[i] += value.fBlock[i];
    }
    this->fHardwareBlockSum += value.fHardwareBlockSum;
    this->fSoftwareBlockSum += value.fSoftwareBlockSum;
    this->fNumberOfSamples  += value.fNumberOfSamples;
    this->fSequenceNumber   = 0;
  }
  return *this;
};

QwVQWK_Channel& QwVQWK_Channel::operator-= (const QwVQWK_Channel &value){
  if (GetElementName()!=""){
    for (size_t i=0; i<4; i++){
      this->fBlock[i] -= value.fBlock[i];
    }
    this->fHardwareBlockSum -= value.fHardwareBlockSum;
    this->fSoftwareBlockSum -= value.fSoftwareBlockSum;
    this->fNumberOfSamples  += value.fNumberOfSamples;
    this->fSequenceNumber   = 0;
  }
  return *this;
};

void QwVQWK_Channel::Sum(QwVQWK_Channel &value1, QwVQWK_Channel &value2){
  *this =  value1;
  *this += value2;
};

void QwVQWK_Channel::Difference(QwVQWK_Channel &value1, QwVQWK_Channel &value2){
  *this =  value1;
  *this -= value2;
};

void QwVQWK_Channel::Ratio(QwVQWK_Channel &numer, QwVQWK_Channel &denom){
  if (GetElementName()!=""){
    for (size_t i=0; i<4; i++){
      this->fBlock[i] = (numer.fBlock[i]) / (denom.fBlock[i]);
    }
    this->fHardwareBlockSum = (numer.fHardwareBlockSum) / (denom.fHardwareBlockSum);
    this->fSoftwareBlockSum = (numer.fSoftwareBlockSum) / (denom.fSoftwareBlockSum);
    this->fNumberOfSamples = denom.fNumberOfSamples;
    this->fSequenceNumber  = 0;
  }
};

Bool_t QwVQWK_Channel::MatchSequenceNumber(size_t seqnum)
{
  Bool_t status = kTRUE;
  if (GetElementName()!=""){
    status = (fSequenceNumber==seqnum);
  }
  return status;
};

Bool_t QwVQWK_Channel::MatchNumberOfSamples(size_t numsamp)
{
  Bool_t status = kTRUE;
  if (GetElementName()!=""){
    status = (fNumberOfSamples==numsamp);
    if (! status)  std::cerr << "QwVQWK_Channel::MatchNumberOfSamples:  Channel "
			     << GetElementName()
			     << " had fNumberOfSamples==" << fNumberOfSamples
			     << " and was supposed to have " << numsamp
			     << std::endl;
  }
  return status;
};



#endif
