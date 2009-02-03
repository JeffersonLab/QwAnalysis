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
  void  ProcessEvent();
  void Print();
  Bool_t IsGoodEvent();	
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

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Float_t> &values);
  void  FillTreeVector(std::vector<Float_t> &values);
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
  
  QwVQWK_Channel fWire[4];
  QwVQWK_Channel fRelPos[2];
  /* These channels contain the beam position within the frame of the BPM*/
  QwVQWK_Channel fAbsPos[3];
  /* contains the beam position in the absolute frame defined as found reference...*/
  
};

/********************************************************/



#endif
