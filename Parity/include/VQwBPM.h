/**********************************************************\
* File: VQwBPM.h                                           *
*                                                          *
* Author: B.Waidyawansa                                    *
* Time-stamp: <2010-05-24>                                 *
\**********************************************************/


#ifndef __VQWBPM__
#define __VQWBPM__

#include "TTree.h"

#include "QwVQWK_Channel.h"
#include "VQwDataElement.h"
//#include "QwDatabase.h"
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
  // Default constructor
  VQwBPM() { };
  VQwBPM(TString name):VQwDataElement(){
    InitializeChannel(name);
  };

  virtual ~VQwBPM(){DeleteHistograms(); };


  void   InitializeChannel(TString name);
  void   ClearEventData();
  void   GetOffset(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset);

  Int_t  GetEventcutErrorFlag(){//return the error flag
    return fDeviceErrorCode;
  };
  Int_t  GetEventcutErrorCounters();

  Bool_t  ApplySingleEventCuts();
  void    SetSingleEventCuts(TString, Double_t, Double_t);


  VQwBPM& operator+=(const VQwBPM&);
  VQwBPM& operator-=(const VQwBPM&);

  void Sum(VQwBPM &value1, VQwBPM &value2);
  void Difference(VQwBPM &value1, VQwBPM &value2);
  void Scale(Double_t factor);

  virtual VQwBPM& operator=  (const VQwBPM &value);

  void          SetRootSaveStatus(TString &prefix);



  private:


  protected:
  Double_t fPositionCenter[3];
  static const TString axis[3];


  QwVQWK_Channel fAbsPos[2]; // Z will not be considered as a vqwk_channel
  QwVQWK_Channel fEffectiveCharge;

  Bool_t   fGoodEvent;
  Bool_t   bEVENTCUTMODE;//If this set to kFALSE then Event cuts are OFF
  Int_t    fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode
  Bool_t   bFullSave; // used to restrict the amount of data histogramed

  const static Bool_t bDEBUG=kFALSE;//debugging display purposes

};


#endif


