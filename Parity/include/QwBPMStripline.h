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

#include "QwVQWK_Channel.h"


/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
class QwBPMStripline : public VQwDataElement{
/////
 public:
  QwBPMStripline() { };
  QwBPMStripline(TString name, Bool_t ROTATED){
    InitializeChannel(name, ROTATED);
  };
  
  ~QwBPMStripline() {DeleteHistograms();};

  Int_t ProcessEvBuffer(UInt_t* buffer, 
			UInt_t word_position_in_buffer,UInt_t indexnumber);

  void  InitializeChannel(TString name, Bool_t ROTATED);
  void  ClearEventData();
  void ReportErrorCounters();

  void  SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY);
  void  RandomizeEventData(int helicity);
  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  ProcessEvent();
  void Print();

  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  Int_t SetSingleEventCuts(std::vector<Double_t> &);
  void SetDefaultSampleSize(Int_t sample_size);
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure

  void ResetRunningAverages();//will reset the average values and event counter
  void CalculateRunningAverages();//calculate running averages for the BCM
  Bool_t CheckRunningAverages(Bool_t bDisplayAVG);//check the avg values are within the event cut limits.

  void Copy(VQwDataElement *source);


  QwBPMStripline& operator=  (const QwBPMStripline &value);
  QwBPMStripline& operator+= (const QwBPMStripline &value);
  QwBPMStripline& operator-= (const QwBPMStripline &value);
  void Sum(QwBPMStripline &value1, QwBPMStripline &value2);
  void Difference(QwBPMStripline &value1, QwBPMStripline &value2);
  void Ratio(QwBPMStripline &numer, QwBPMStripline &denom);
  void Scale(Double_t factor);

  void SetOffset(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset);
  UInt_t GetSubElementIndex(TString subname);
  TString GetSubElementName(Int_t subindex);
  void SetSubElementPedestal(Int_t j, Double_t value);
  void SetSubElementCalibrationFactor(Int_t j, Double_t value);


  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  void  SetRootSaveStatus(TString &prefix);

  /////
 private:
  static const Bool_t kDEBUG;
  /*  Position calibration factor, transform ADC counts in mm */
  static const Double_t kQwStriplineCalibration;
  /* Rotation factor for the BPM which antenna are at 45 deg */
  static const Double_t kRotationCorrection;

 protected:
  static const TString subelement[4];
  static const TString axis[3];

  Double_t fOffset[3];
  Bool_t bRotated;
  Bool_t bFullSave; // used to restrict the amount of data histogramed

  Bool_t fGoodEvent; 



  Double_t fULimitX, fLLimitX, fULimitY, fLLimitY;//this sets the upper and lower limits on the X & Y of the BPM stripline
  Int_t fSampleSize;
  Int_t fSequenceNo_Prev;//used to validate sequence number in the IsGoodEvent()
  Int_t counter;//used to validate sequence number in the IsGoodEvent()


  QwVQWK_Channel fWire[4];
  QwVQWK_Channel fWSum;
  QwVQWK_Channel fRelPos[2];
  Double_t fRelPos_Running_AVG[2];//BPM running AVG
  Double_t fRelPos_Running_AVG_square[2];//BPM running AVG square
  /* These channels contain the beam position within the frame of the BPM*/
  QwVQWK_Channel fAbsPos[3];
  
  Int_t Event_Counter;

  
  Double_t fPrevious_HW_Sum[4];//stores the last event's hardware sum.
  Int_t fHW_Sum_Stuck_Counter[4];//increment this if HW_sum is stuck with one value

  Int_t fDevice_flag;//sets the event cut level for the device fDevice_flag=1 Event cuts & HW check,fDevice_flag=0 HW check, fDevice_flag=-1 no check 


  const static Bool_t bDEBUG=kFALSE;//debugging display purposes

  /* contains the beam position in the absolute frame defined as found reference...*/

};
 
/********************************************************/



#endif
