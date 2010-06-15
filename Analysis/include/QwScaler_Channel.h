/**********************************************************\
* File: QwScaler_Channel.h                                 *
*                                                          *
* Author: J. Pan                                           *
* Date:   Thu Sep 16 18:08:33 CDT 2009                     *
\**********************************************************/

#ifndef __QWSCALER_CHANNEL__
#define __QWSCALER_CHANNEL__

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
template <UInt_t data_mask=0xffffffff, UInt_t data_shift=0 >
class QwScaler_Channel: public VQwDataElement {

public:
  QwScaler_Channel() { };
  QwScaler_Channel(TString name){
    InitializeChannel(name);
  };
  ~QwScaler_Channel() {DeleteHistograms();};

  void  InitializeChannel(TString name){
    fValue = 0;
    SetNumberOfDataWords(1);  //Scaler - single word, 32 bits
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

  Double_t GetValue() const { return fValue; };


  QwScaler_Channel& operator=  (const QwScaler_Channel &value);
  QwScaler_Channel& operator+= (const QwScaler_Channel &value);
  QwScaler_Channel& operator-= (const QwScaler_Channel &value);
  void Sum(QwScaler_Channel &value1, QwScaler_Channel &value2);
  void Difference(QwScaler_Channel &value1, QwScaler_Channel &value2);
  void Ratio(QwScaler_Channel &numer, QwScaler_Channel &denom);
  void Offset(Double_t Offset);
  void Scale(Double_t Offset);

  Bool_t ApplySingleEventCuts();//check values read from modules are at desired level
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);



  void Copy(VQwDataElement *source);

  void PrintValue() const;
  void PrintInfo() const;

protected:

private:
  static const Bool_t kDEBUG;

  UInt_t   fValue_Raw;
  Double_t fValue;

  /*  Ntuple array indices */
  size_t fTreeArrayIndex;
  size_t fTreeArrayNumEntries;


  Int_t fNumEvtsWithHWErrors;//counts the HW falied events
  Int_t fNumEvtsWithEventCutsRejected;////counts the Event cut rejected events
};

//  These typedef's should be the last things in the file.
//  Class template instationation must be made in the source
//  file for anything defined here.
typedef class QwScaler_Channel<0x00ffffff,0> QwSIS3801D24_Channel;
typedef class QwScaler_Channel<0xffffffff,0>    QwSIS3801_Channel;
typedef class QwScaler_Channel<0xffffffff,0>    QwSTR7200_Channel;


#endif
