/**********************************************************\
* File: QwBPMStripline.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwVQWK_STRIPLINE__
#define __QwVQWK_STRIPLINE__

#include <vector>
#include <TTree.h>

#define MYSQLPP_SSQLS_NO_STATICS
#include "QwSSQLS.h"

#include "QwVQWK_Channel.h"
#include "QwDatabase.h"
#include "VQwBPM.h"
#include "QwParameterFile.h"
/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL

class QwBPMStripline : public VQwBPM {
  friend class QwCombinedBPM;
  friend class QwEnergyCalculator;

 public:
  QwBPMStripline() { };
  QwBPMStripline(TString name, Bool_t ROTATED):VQwBPM(name){
    InitializeChannel(name);
    bRotated=ROTATED;

  };

  ~QwBPMStripline() {
    DeleteHistograms();
  };

  void    InitializeChannel(TString name);
  void    ClearEventData();
  Int_t   ProcessEvBuffer(UInt_t* buffer,
			UInt_t word_position_in_buffer,UInt_t indexnumber);
  void    ProcessEvent();
  void    PrintValue() const;
  void    PrintInfo() const;

  UInt_t  GetSubElementIndex(TString subname);
  TString GetSubElementName(Int_t subindex);
  void    GetAbsolutePosition();

  Bool_t  ApplyHWChecks();//Check for harware errors in the devices
  Bool_t  ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  void    SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX);
  void    SetEventCutMode(Int_t bcuts);
  Int_t   GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure

  void    SetDefaultSampleSize(Int_t sample_size);
  void    SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY);
  void    RandomizeEventData(int helicity = 0, double time = 0.0);
  void    SetEventData(Double_t* block, UInt_t sequencenumber);
  void    EncodeEventData(std::vector<UInt_t> &buffer);
  void    SetSubElementPedestal(Int_t j, Double_t value);
  void    SetSubElementCalibrationFactor(Int_t j, Double_t value);

  void    Copy(VQwDataElement *source);
  void    Ratio(QwBPMStripline &numer, QwBPMStripline &denom);
  void    Scale(Double_t factor);

  virtual QwBPMStripline& operator=  (const QwBPMStripline &value);
  virtual QwBPMStripline& operator+= (const QwBPMStripline &value);
  virtual QwBPMStripline& operator-= (const QwBPMStripline &value);

  void    AccumulateRunningSum(const QwBPMStripline& value);
  void    CalculateRunningAverage();

  void    ConstructHistograms(TDirectory *folder, TString &prefix);
  void    FillHistograms();
  void    DeleteHistograms();

  void    ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void    ConstructBranch(TTree *tree, TString &prefix);
  void    ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void    FillTreeVector(std::vector<Double_t> &values) const;



  std::vector<QwDBInterface> GetDBEntry();
  void    MakeBPMList();


  /////
 private:
  /*  Position calibration factor, transform ADC counts in mm */
  static const Double_t kQwStriplineCalibration;
  /* Rotation factor for the BPM which antenna are at 45 deg */
  static const Double_t kRotationCorrection;
  static const TString subelement[4];



 protected:
  Bool_t   bRotated;
  QwVQWK_Channel fWire[4];
  QwVQWK_Channel fRelPos[2];
  QwVQWK_Channel fAbsPos[2]; // Z will not be considered as a vqwk_channel
  QwVQWK_Channel fEffectiveCharge;

  std::vector<QwVQWK_Channel> fBPMElementList;

};


#endif
