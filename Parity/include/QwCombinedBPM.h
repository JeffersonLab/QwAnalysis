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
#include "QwBPMStripline.h" //bbbbb

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
class QwCombinedBPM : public VQwDataElement{
/////
 public:
  QwCombinedBPM() { };
  QwCombinedBPM(TString name, Bool_t ROTATED){
    InitializeChannel(name, ROTATED);
  };
  
  ~QwCombinedBPM() {DeleteHistograms();};

  Int_t ProcessEvBuffer(UInt_t* buffer, 
			UInt_t word_position_in_buffer,UInt_t indexnumber);

  void  InitializeChannel(TString name, Bool_t ROTATED);
  void  Add(QwBPMStripline* bpm, Double_t charge_weight,  Double_t x_weight, Double_t y_weight); //bbbbb

  void  ClearEventData();
  void  SetRandomEventAsymmetry(Double_t asymmetry);
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
  Int_t GetEventcutErrorFlag(){//return the error flag 
    return fDeviceErrorCode;
  };

  void SetEventCutMode(Int_t bcuts);
  

  void Copy( VQwDataElement *source);
  Double_t SumOver( std::vector <Double_t> weight , std::vector <Double_t> val); //bbbbbbbb
  void LeastSquareFit( Int_t pos, Double_t A, Double_t B, Double_t C, Double_t D, Double_t E, Double_t F ); //bbbbb


  QwCombinedBPM& operator=  (const QwCombinedBPM &value);
  QwCombinedBPM& operator+= (const QwCombinedBPM &value);
  QwCombinedBPM& operator-= (const QwCombinedBPM &value);
  void Sum(QwCombinedBPM &value1, QwCombinedBPM &value2);
  void Difference(QwCombinedBPM &value1, QwCombinedBPM &value2);
  void Ratio(QwCombinedBPM &numer, QwCombinedBPM &denom);
  void Scale(Double_t factor);
 
  void Calculate_Running_Average();
  void Do_RunningSum(); 

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void SetOffset(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset);
 /*  UInt_t GetSubElementIndex(TString subname); */
/*   TString GetSubElementName(Int_t subindex); */
/*   void SetSubElementPedestal(Int_t j, Double_t value); */
/*   void SetSubElementCalibrationFactor(Int_t j, Double_t value); */


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

  std::vector <QwBPMStripline*> fElement; //bbbbb
  std::vector <Double_t> fQWeights; //bbbbb
  std::vector <Double_t> fXWeights; //bbbbb
  std::vector <Double_t> fYWeights; //bbbbb

 

 protected:
  //static const TString subelement[4];
  static const TString axis[3];

  Double_t fComboOffset[3];
  Bool_t bRotated;
  Bool_t bFullSave; // used to restrict the amount of data histogramed

  Bool_t fGoodEvent; 

  Double_t a[2],b[2],erra[2],errb[2],covab[2];

  Double_t fULimitX, fLLimitX, fULimitY, fLLimitY;//this sets the upper and lower limits on the X & Y of the BPM stripline
 


  // QwVQWK_Channel fCombinedWire[4]; //bbbb individual wires at the target
  QwVQWK_Channel fCombinedWSum; //bbbb  charge at the target


  
  /* These channels contain the beam position within the frame of the BPM*/
  QwVQWK_Channel fCombinedRelPos[2]; //bbbbb absolute position at the target
  
    
  

  Int_t fDevice_flag;//sets the event cut level for the device fDevice_flag=1 Event cuts & HW check,fDevice_flag=0 HW check, fDevice_flag=-1 no check 
  Int_t fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode

  const static Bool_t bDEBUG=kFALSE;//debugging display purposes

  Bool_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts are OFF

  /* contains the beam position in the absolute frame defined as found reference...*/

};
 
/********************************************************/



#endif
