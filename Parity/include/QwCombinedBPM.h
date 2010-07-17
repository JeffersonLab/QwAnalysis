/**********************************************************\
* File: QwCombinedBPM.h                                   *
*                                                         *
* Author:B. Waidyawansa                                   *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwVQWK_COMBINEDBPM__
#define __QwVQWK_COMBINEDBPM__

#include <vector>
#include <TTree.h>

#include "QwVQWK_Channel.h"
#include "QwBPMStripline.h" 
#include "VQwBPM.h"

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
class QwCombinedBPM : public VQwBPM{
  friend class QwEnergyCalculator;  

  /////
 public:
  QwCombinedBPM(){};
  QwCombinedBPM(TString name):VQwBPM(name){
    InitializeChannel(name);
  };

  ~QwCombinedBPM() {
    DeleteHistograms();
  };

  void    InitializeChannel(TString name);
  void    ClearEventData();
  Int_t   ProcessEvBuffer(UInt_t* buffer,
			UInt_t word_position_in_buffer,UInt_t indexnumber);
  void    ProcessEvent();
  void    PrintValue() const;
  void    PrintInfo() const;

  Bool_t  ApplyHWChecks();//Check for harware errors in the devices
  Bool_t  ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  void    SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX);
  void    SetEventCutMode(Int_t bcuts);
  Int_t   GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure

  void    Set(QwBPMStripline* bpm, Double_t charge_weight,  Double_t x_weight, Double_t y_weight,Double_t sumqw);
 
  void    Copy(VQwDataElement *source);
  void    Ratio(QwCombinedBPM &numer, QwCombinedBPM &denom);
  void    Scale(Double_t factor);

  virtual QwCombinedBPM& operator=  (const QwCombinedBPM &value);
  virtual QwCombinedBPM& operator+= (const QwCombinedBPM &value);
  virtual QwCombinedBPM& operator-= (const QwCombinedBPM &value);

  void    AccumulateRunningSum(const QwCombinedBPM& value);
  void    CalculateRunningAverage();

  void    ConstructHistograms(TDirectory *folder, TString &prefix);
  
  void    FillHistograms();
  void    DeleteHistograms();

  void    ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void    ConstructBranch(TTree *tree, TString &prefix);
  void    ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void    FillTreeVector(std::vector<Double_t> &values);

  /* Functions for least square fit */
  void     CalculateFixedParameter(std::vector<Double_t> fWeights, Int_t pos);
  Double_t SumOver( std::vector <Double_t> weight , std::vector <QwVQWK_Channel> val);
  void     LeastSquareFit( Int_t pos, std::vector<Double_t> fWeights) ; //bbbbb

 

  /////
 private:
  Bool_t fixedParamCalculated;

  //used for least squares fit
  Double_t erra[2],errb[2],covab[2];
  Double_t A[2], B[2], D[2], m[2];
  Double_t chi_square[2];
  Double_t fSumQweights; 

  std::vector <QwBPMStripline*> fElement;
  std::vector <Double_t> fQWeights;
  std::vector <Double_t> fXWeights;
  std::vector <Double_t> fYWeights;


 protected:
  /* This channel contains the beam slope w.r.t the X & Y axis at the target */
  QwVQWK_Channel fSlope[2];
  
  /* This channel contains the beam intercept w.r.t the X & Y axis at the target */
  QwVQWK_Channel fIntercept[2];

};



#endif
