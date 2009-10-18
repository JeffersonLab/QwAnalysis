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


enum EDataToSave{kRaw=0, kDerived};
// this data is used to decided which data need to be histogrammed or ttree-ed


///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class QwVQWK_Channel: public VQwDataElement {
/****************************************************************//**
 *  Class: QwVQWK_Channel
 *         Base class containing decoding functions for the VQWK_Channel
 *         6 32-bit datawords.
 *         The functions in this class will decode a single channel
 *         worth of VQWK_Channel data and provide the components
 *         through member functions.
 ******************************************************************/
 public:
  QwVQWK_Channel():fNumEvtsWithErrors(0) { };
  QwVQWK_Channel(TString name, TString datatosave="raw"):fNumEvtsWithErrors(0){
    InitializeChannel(name, datatosave);
  };
  ~QwVQWK_Channel() {
    DeleteHistograms();
  };

  void  InitializeChannel(TString name, TString datatosave){
    SetElementName(name);
    SetNumberOfDataWords(6);
    fBlocksPerEvent = 4;
    fPedestal=0.0;
    fCalibrationFactor=1.;
    if(datatosave=="raw") fDataToSave=kRaw;
    else
      if(datatosave=="derived") fDataToSave=kDerived;
    return;
  };

  void  ReportErrorCounters(){
    if (fNumEvtsWithErrors>0) 
      std::cout << "VQWK channel " << GetElementName() 
		<< " had " << fNumEvtsWithErrors 
		<< " events with a sample count mismatch."
		<< std::endl;
  };

  void  ClearEventData();
  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left,UInt_t index=0);
  void  ProcessEvent();

  QwVQWK_Channel& operator=  (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator+= (const QwVQWK_Channel &value);
  QwVQWK_Channel& operator-= (const QwVQWK_Channel &value);
  void Sum(QwVQWK_Channel &value1, QwVQWK_Channel &value2);
  void Difference(QwVQWK_Channel &value1, QwVQWK_Channel &value2);
  void Ratio(QwVQWK_Channel &numer, QwVQWK_Channel &denom);
  void Offset(Double_t Offset);
  void Scale(Double_t Offset);

  Bool_t MatchSequenceNumber(size_t seqnum);
  Bool_t MatchNumberOfSamples(size_t numsamp);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);

  Double_t GetBlockValue(size_t blocknum){return fBlock[blocknum];};
  Double_t GetHardwareSum(){return fHardwareBlockSum;};
  //  Double_t GetSoftwareSum(){return fSoftwareBlockSum;};

  Double_t GetRawBlockValue(size_t blocknum){return fBlock_raw[blocknum];};
  Double_t GetRawHardwareSum(){return fHardwareBlockSum_raw;};
  Double_t GetRawSoftwareSum(){return fSoftwareBlockSum_raw;};

  size_t GetSequenceNumber(){return (fSequenceNumber);};
  size_t GetNumberOfSamples(){return (fNumberOfSamples);};
  
  void SetPedestal(Double_t ped){fPedestal=ped; return;};
  Double_t GetPedestal(){return fPedestal;};
  void SetCalibrationFactor(Double_t factor){fCalibrationFactor=factor; return;};
  Double_t GetCalibrationFactor(){return fCalibrationFactor;};

  Bool_t IsGoodEvent();	

  void Copy(VQwDataElement *source);

  void Print() const;

 protected:
  

 private:
  static const Bool_t kDEBUG;

  Int_t fDataToSave; 

  /*  ADC Calibration                     */
  static const Double_t kVQWK_VoltsPerBit;
  Double_t fPedestal;         /*! pedestal of the hardware sum signal,
			     we assume the pedestal level is constant over time
			     and can be divided by four for use with each block,
			     units: [counts/number of Sample] */
  Double_t fCalibrationFactor; 

  /*  Channel information data members    */
  Int_t fNumEvtsWithErrors;

  /*  Channel configuration data members */
  UInt_t  fSamplesPerBlock;
  UInt_t  fBlocksPerEvent;

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;

  /*  Event data members */
  Double_t fBlock_raw[4];     /*! Array of the sub-block data as read from the module */
  Double_t fHardwareBlockSum_raw; /*! Module-based sum of the four sub-blocks as read from the module  */
  Double_t fSoftwareBlockSum_raw; /*! Sum of the data in the four sub-blocks raw */
  /* the following values potentially have pedestal removed  and calibration applied */
  Double_t fBlock[4];         /*! Array of the sub-block data             */
  Double_t fHardwareBlockSum; /*! Module-based sum of the four sub-blocks */
  size_t fSequenceNumber;     /*! Event sequence number for this channel  */
  size_t fNumberOfSamples;    /*! Number of samples in the event          */

};


#endif
