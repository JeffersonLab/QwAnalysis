/**********************************************************\
* File: VQwBPM.h                                           *
*                                                          *
* Author: B.Waidyawansa                                    *
* Time-stamp: <2010-05-24>                                 *
\**********************************************************/


#ifndef __VQWBPM__
#define __VQWBPM__

// ROOT headers
#include <TTree.h>
#include <TMath.h>

// Qweak headers
#include "QwVQWK_Channel.h"
#include "VQwDataElement.h"
#include "VQwHardwareChannel.h"

///
/// \ingroup QwAnalysis_BeamLine
///

class VQwBPM : public VQwDataElement {
  /******************************************************************
   *  Class: VQwBPM
   *         Virtual base class for the BPMs in the beamline.
   *         This will define a set of functions that are used by the
   *         BPMStripline and CombinedBPM classes for data decoding.
   *         This parent class will define the basic structure of a 
   *         BPM. The basic object doesn't care about the inputs.
   *         It only defines absoloute positions and an effective charge.
   *           BPMs  have 4 input wires for position: XP, XM, YP, YM
   *           Cavityy monitors have 3 wires: X, Y and I
   *           CombinedBPM use absolute X and Y derived from BPM X and Ys.
   ******************************************************************/

  //  friend class QwEnergyCalculator;  

 public:
  ///  Axis enumerator for the BPMs; 
  ///  Z will never be an instrumented axis.
  enum EBPMAxes{kXAxis=0, kYAxis, kNumAxes};

 public:
  // Default constructor
  VQwBPM() { };
  VQwBPM(TString name):VQwDataElement(){
    InitializeChannel(name);
    fQwStriplineCalibration = 18.81; // adc counts/mm default value
    for(Short_t i=kXAxis;i<kNumAxes;i++)  fRelativeGains[i]=1.0;
  };

  virtual ~VQwBPM(){DeleteHistograms(); };


  void   InitializeChannel(TString name);
  void   ClearEventData();
  void   GetSurveyOffsets(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset);
  void   GetElectronicFactors(Double_t BSENfactor, Double_t AlphaX, Double_t AlphaY);
  void   SetRotation(Double_t);
  void   SetRotationOff();
  UInt_t  GetEventcutErrorFlag(){//return the error flag
    return fErrorFlag;
    //return fDeviceErrorCode;
  };
  Int_t   GetEventcutErrorCounters();

  Bool_t  ApplySingleEventCuts();
  void    SetSingleEventCuts(TString, Double_t, Double_t);

  VQwBPM& operator+=(const VQwBPM&);
  VQwBPM& operator-=(const VQwBPM&);

  void Sum(VQwBPM &value1, VQwBPM &value2);
  void Difference(VQwBPM &value1, VQwBPM &value2);
  void Scale(Double_t factor);
  void Copy(VQwBPM *source);

  virtual VQwBPM& operator=  (const VQwBPM &value);

  void          SetRootSaveStatus(TString &prefix);


  VQwHardwareChannel* GetPositionX(){
    return &fAbsPos[0];
  };

  const VQwHardwareChannel* GetPositionX() const{
    return const_cast<VQwBPM*>(this)->GetPositionX();
  };

  VQwHardwareChannel* GetPositionY(){
    return &fAbsPos[1];
  };

  const VQwHardwareChannel* GetPositionY() const{
    return const_cast<VQwBPM*>(this)->GetPositionY();
  };


  
  /*   VQwHardwareChannel* GetEffectiveCharge(){ */
  /*     return &fEffectiveCharge; */
  /*   } */
  const VQwHardwareChannel* GetEffectiveCharge() const{
    return &fEffectiveCharge;
  };
  const VQwHardwareChannel* GetPosition(size_t index) const{
    return &fAbsPos[index];
  };
  
  Double_t GetPositionInZ() const{
    return fPositionCenter[2];
  };

  void PrintValue() const;
  void PrintInfo() const;
  void CalculateRunningAverage();
  void AccumulateRunningSum(const VQwBPM& value);


  private:


  protected:
  ///  Axis labels for the instrumented directions;
  ///  Z will never be an instrumented axis.
  static const TString kAxisLabel[2];

  // Position calculation related paramters
  Double_t fPositionCenter[3];
  Double_t fQwStriplineCalibration;
  Double_t fRelativeGains[2];

  // Rotation related paramters
  Bool_t   bRotated;
  Double_t fRotationAngle;
  Double_t fCosRotation;
  Double_t fSinRotation;

  // Device outputs
  QwVQWK_Channel fAbsPos[2]; // Z will not be considered as a vqwk_channel
  QwVQWK_Channel fEffectiveCharge;

  // Data quality checks related flags
  Bool_t   fGoodEvent;
  Bool_t   bEVENTCUTMODE;//If this set to kFALSE then Event cuts are OFF
  Int_t    fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode
  Bool_t   bFullSave; // used to restrict the amount of data histogramed

  const static Bool_t bDEBUG=kFALSE;//debugging display purposes

};


#endif


