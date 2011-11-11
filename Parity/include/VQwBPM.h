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

class QwParameterFile;
template<typename T> class QwCombinedBPM;
template<typename T> class QwBPMStripline;

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
  template <typename TT> friend class QwBPMStripline;
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

  virtual ~VQwBPM() { };


  void   InitializeChannel(TString name);
  //  virtual void   ClearEventData() = 0;
  void   GetSurveyOffsets(Double_t Xoffset, Double_t Yoffset, Double_t Zoffset);
  void   GetElectronicFactors(Double_t BSENfactor, Double_t AlphaX, Double_t AlphaY);
  void   SetRotation(Double_t);
  void   SetRotationOff();
  /*
  UInt_t  GetEventcutErrorFlag(){//return the error flag
    return fErrorFlag;
    };
  */
  //  virtual Int_t   GetEventcutErrorCounters() = 0;

/*   Bool_t  ApplySingleEventCuts(); */
  void    SetSingleEventCuts(TString, Double_t, Double_t);
  void    SetSingleEventCuts(TString, UInt_t, Double_t, Double_t, Double_t);
  virtual void UpdateEventcutErrorFlag(UInt_t errorflag) = 0;
  virtual void UpdateEventcutErrorFlag(VQwBPM *ev_error) = 0;


/*   void Sum(VQwBPM &value1, VQwBPM &value2); */
/*   void Difference(VQwBPM &value1, VQwBPM &value2); */
  virtual void Scale(Double_t factor) {
    std::cerr << "Scale for VQwBPM not implemented!\n";
  }
  virtual void Copy(const VQwBPM *source);
  void SetGains(TString pos, Double_t value);

  // Operators subclasses MUST support!
  virtual VQwBPM& operator=  (const VQwBPM &value) =0;
  virtual VQwBPM& operator+= (const VQwBPM &value) =0;
  virtual VQwBPM& operator-= (const VQwBPM &value) =0;

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
  virtual void CalculateRunningAverage() = 0;
  virtual void AccumulateRunningSum(const VQwBPM& value) {
    std::cerr << "AccumulateRunningSum not implemented for BPM named="
      <<GetElementName()<<"\n";
  };
  virtual void DeaccumulateRunningSum(VQwBPM& value) = 0;

  virtual void ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  virtual void FillHistograms() = 0;

  virtual void ConstructBranchAndVector(TTree *tree, TString &prefix,
      std::vector<Double_t> &values) = 0;
  virtual void ConstructBranch(TTree *tree, TString &prefix) = 0;
  virtual void ConstructBranch(TTree *tree, TString &prefix,
      QwParameterFile& modulelist) = 0;
  virtual void FillTreeVector(std::vector<Double_t> &values) const = 0;
  virtual std::vector<QwDBInterface> GetDBEntry() = 0;

  virtual void Ratio(VQwBPM &numer, VQwBPM &denom) {
    std::cerr << "Ratio() is not defined for BPM named="<<GetElementName()<<"\n";
  }

  // Stuff required for QwBPMStripLine
  virtual UInt_t  GetSubElementIndex(TString subname) {
    std::cerr << "GetSubElementIndex() is not implemented for BPM named="
      <<GetElementName()<< "!!\n";
    return 0;
  }
  virtual TString GetSubElementName(Int_t subindex) {
    std::cerr << "GetSubElementName()  is not implemented!!\n";
    return TString("OBJECT_UNDEFINED"); // Return an erroneous TString
  }
  virtual void GetAbsolutePosition() {
    std::cerr << "GetAbsolutePosition() is not implemented!!\n";
  }
  virtual void SetEventCutMode(Int_t bcuts) = 0;
  virtual Int_t GetEventcutErrorCounters() {// report number of events falied due to HW and event cut faliure
    std::cerr << "GetEventcutErrorCounters() is not implemented!!\n";
    return 0;
  }
  virtual Bool_t ApplySingleEventCuts() = 0;//Check for good events by stting limits on the devices readings
  virtual void ProcessEvent() = 0;

  // These only applies to a combined BPM
  virtual const VQwHardwareChannel* GetAngleX() const {
    std::cerr << "GetAngleX() is not implemented for VQwBPM, must be"
      "used in a CombinedBPM!\n";
    return 0;
  }
  virtual const VQwHardwareChannel* GetAngleY() const {
    std::cerr << "GetAngleY() is not implemented for VQwBPM, must be"
      "used in a CombinedBPM!\n";
    return 0;
  }
  virtual void SetBPMForCombo(const VQwBPM* bpm, Double_t charge_weight,
      Double_t x_weight, Double_t y_weight,Double_t sumqw) {
    std::cerr << "VQwBPM::SetBPMForCombo only defined for CombinedBPM's!!!\n";
  }


  virtual void SetDefaultSampleSize(Int_t sample_size) {
    std::cerr << "SetDefaultSampleSize() is undefined!!!\n";
  }
  virtual void SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY) {
    std::cerr<< "SetRandomEventParameters undefined!!\n";
  }
  virtual void RandomizeEventData(int helicity = 0, double time = 0.0) {
    std::cerr << "RandomizeEventData is undefined!!!\n";
  }
  virtual void EncodeEventData(std::vector<UInt_t> &buffer) {
    std::cerr << "EncodeEventData is undefined!!!\n";
  }
  virtual void SetSubElementPedestal(Int_t j, Double_t value) {
    std::cerr << "SetSubElementPedestal is undefined!!!\n";
  }
  virtual void SetSubElementCalibrationFactor(Int_t j, Double_t value) {
    std::cerr << "SetSubElementCalibrationFactor is undefined!!!\n";
  }
  virtual void PrintInfo() const { 
    std::cout<<"PrintInfo() for VQwBPM not impletemented\n";
  };

  // Factory function to produce appropriate BCM
  static VQwBPM* CreateStripline(TString subsystemname, TString type, TString name);
  static VQwBPM* CreateStripline(TString type); // Create a generic BPM (define properties later)
  static VQwBPM* CreateCombo(TString subsystemname, TString type, TString name);
  static VQwBPM* CreateCombo(TString type); // Create a generic BPM (define properties later)

  private:

  void InitializeChannel_base() {
/*     fAbsPos_base[0] = NULL; */
/*     fAbsPos_base[1] = NULL; */
/*     fEffectiveCharge_base = NULL; */
    fQwStriplineCalibration = 18.81; // adc counts/mm default value
    for(Short_t i=0;i<2;i++) {
      fRelativeGains[i]=1.0;
      fGains[i]=1.0;
    }
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
  Double_t fGains[2];
  static const TString axis[3];

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


