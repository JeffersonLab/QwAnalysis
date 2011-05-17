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
  template <typename TT> friend class QwCombinedBPM;
  friend class QwEnergyCalculator;  

 public:
  ///  Axis enumerator for the BPMs; 
  ///  Z will never be an instrumented axis.
  enum EBeamPositionMonitorAxis{kXAxis=0, kYAxis, kNumAxes};

 public:
  // Default constructor
  VQwBPM() {InitializeChannel_base();};
  VQwBPM(TString& name) {InitializeChannel_base();};

  virtual ~VQwBPM(){DeleteHistograms(); };


  void   InitializeChannel(TString name);
  //  virtual void   ClearEventData() = 0;
  void   GetSurveyOffsets(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset);
  void   GetElectronicFactors(Double_t BSENfactor, Double_t AlphaX, Double_t AlphaY);
  void   SetRotation(Double_t);
  void   SetRotationOff();
  UInt_t  GetEventcutErrorFlag(){//return the error flag
    return fErrorFlag;
    //return fDeviceErrorCode;
  };
  //  virtual Int_t   GetEventcutErrorCounters() = 0;

/*   Bool_t  ApplySingleEventCuts(); */
  void    SetSingleEventCuts(TString, Double_t, Double_t);
  void    SetSingleEventCuts(TString, UInt_t, Double_t, Double_t, Double_t);

/*   VQwBPM& operator+=(const VQwBPM&); */
/*   VQwBPM& operator-=(const VQwBPM&); */

/*   void Sum(VQwBPM &value1, VQwBPM &value2); */
/*   void Difference(VQwBPM &value1, VQwBPM &value2); */
/*   void Scale(Double_t factor); */
  void Copy(VQwBPM *source);

  virtual VQwBPM& operator=  (const VQwBPM &value);

  void          SetRootSaveStatus(TString &prefix);


/*   VQwDataElement* GetPositionX(){ */
/*     return fAbsPos_base[0]; */
/*   }; */
/*   const VQwDataElement* GetPositionX() const{ */
/*     return const_cast<VQwBPM*>(this)->GetPositionX(); */
/*   }; */
/*   VQwDataElement* GetPositionY(){ */
/*     return fAbsPos_base[1]; */
/*   }; */
/*   const VQwDataElement* GetPositionY() const{ */
/*     return const_cast<VQwBPM*>(this)->GetPositionY(); */
/*   }; */

/*   VQwHardwareChannel* GetPosition(size_t axis){ */
/*     if (axis<0 || axis>2){ */
/*       TString loc="VQwBPM::GetPosition for " */
/*         +this->GetElementName()+" failed for axis value "+Form("%d",axis); */
/*       throw std::out_of_range(loc.Data()); */
/*     } */
/*     return fAbsPos_base[axis]; */
/*   } */
/*   const VQwHardwareChannel* GetPosition(size_t axis) const{ */
/*     return const_cast<VQwBPM*>(this)->GetPosition(axis); */
/*   } */

  virtual const VQwHardwareChannel* GetPosition(EBeamPositionMonitorAxis axis) const = 0;
  

/*   VQwHardwareChannel* GetEffectiveCharge(){ */
/*     return fEffectiveCharge_base; */
/*   } */
  virtual const VQwHardwareChannel* GetEffectiveCharge() const = 0;

  Double_t GetPositionInZ() const{
    return fPositionCenter[2];
  };

/*   void PrintValue() const; */
/*   void PrintInfo() const; */
/*   void CalculateRunningAverage(); */
/*   void AccumulateRunningSum(const VQwBPM& value); */


  private:

  void InitializeChannel_base() {
/*     fAbsPos_base[0] = NULL; */
/*     fAbsPos_base[1] = NULL; */
/*     fEffectiveCharge_base = NULL; */
    fQwStriplineCalibration = 18.81; // adc counts/mm default value
    for(Short_t i=0;i<2;i++)  fRelativeGains[i]=1.0;
  };

  protected:
  ///  Axis labels for the instrumented directions;
  ///  Z will never be an instrumented axis.
  static const TString kAxisLabel[2];

  virtual VQwHardwareChannel* GetSubelementByName(TString ch_name) = 0;
  VQwHardwareChannel* GetSubelementByIndex(size_t index){
    return GetSubelementByName(fSubelementNames.at(index));
  };

  protected:
  std::vector<TString> fSubelementNames;

  // Position calculation related paramters
  Double_t fPositionCenter[3];
  Double_t fQwStriplineCalibration;
  Double_t fRelativeGains[2];

  // Rotation related paramters
  Bool_t   bRotated;
  Double_t fRotationAngle;
  Double_t fCosRotation;
  Double_t fSinRotation;


  // Data quality checks related flags
  Bool_t   fGoodEvent;
  //  Bool_t   bEVENTCUTMODE;//If this set to kFALSE then Event cuts are OFF
  //  Int_t    fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode
  Bool_t   bFullSave; // used to restrict the amount of data histogramed

  const static Bool_t bDEBUG=kFALSE;//debugging display purposes

};


#endif


