/**********************************************************\
* File: MQwSIS3801_Channel.h                               *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#ifndef __MQwSIS3801_CHANNEL__
#define __MQwSIS3801_CHANNEL__

#include <vector>
#include "TTree.h"

// Boost math library for random number generation
#include "boost/random.hpp"

//jpan: Mersenne Twistor: A 623-diminsionally equidistributed
//uniform pseudorandom number generator
#include "TRandom3.h"


#include "VQwDataElement.h"



// this data is used to decided which data need to be histogrammed or ttree-ed


///
/// \ingroup QwAnalysis_ADC
///
/// \ingroup QwAnalysis_BL
class MQwSIS3801_Channel: public VQwDataElement {

 public:
  MQwSIS3801_Channel() { };
  MQwSIS3801_Channel(TString name){
    InitializeChannel(name);
  };
  ~MQwSIS3801_Channel() {DeleteHistograms();};

  void  InitializeChannel(TString name){
    fValue = 0;
    SetNumberOfDataWords(1);  //SIS3801 - single word, 32 bits
    fNumEvtsWithHWErrors=0;//init error counters
    fNumEvtsWithEventCutsRejected=0;//init error counters
    SetElementName(name);
    return;
  };

  void  ClearEventData();

  void ReportErrorCounters();//This will display the error summary for each device
  void UpdateHWErrorCount(){//Update error counter for HW faliure
    fNumEvtsWithHWErrors++;
  }

  void UpdateEventCutErrorCount(){//Update error counter for event cut faliure
    fNumEvtsWithEventCutsRejected++;
  }

  void  RandomizeEventData(int helicity);
  void  SetEventData(Double_t value);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left,UInt_t index=0);
  void  ProcessEvent();

  Double_t  GetValue(){return fValue;};


  MQwSIS3801_Channel& operator=  (const MQwSIS3801_Channel &value);
  MQwSIS3801_Channel& operator+= (const MQwSIS3801_Channel &value);
  MQwSIS3801_Channel& operator-= (const MQwSIS3801_Channel &value);
  void Sum(MQwSIS3801_Channel &value1, MQwSIS3801_Channel &value2);
  void Difference(MQwSIS3801_Channel &value1, MQwSIS3801_Channel &value2);
  void Ratio(MQwSIS3801_Channel &numer, MQwSIS3801_Channel &denom);
  void Offset(Double_t Offset);
  void Scale(Double_t Offset);

  Bool_t ApplySingleEventCuts();//check values read from modules are at desired level
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);



  void Copy(VQwDataElement *source);

  void Print() const;

 protected:


 private:
  static const Bool_t kDEBUG;

  UInt_t fValue;

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;


  Int_t fNumEvtsWithHWErrors;//counts the HW falied events
  Int_t fNumEvtsWithEventCutsRejected;////counts the Event cut rejected events


const static Bool_t bDEBUG=kFALSE;//debugging display purposes
};


#endif
