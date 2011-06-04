/**********************************************************\
* File: VQwBCM.h                                           *
*                                                          *
* Author: (??) & J.C Cornejo                               *
* Time-stamp: <2011-05-26>                                 *
\**********************************************************/

#ifndef __VQWBCM__
#define __VQWBCM__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

#include "QwParameterFile.h"
#include "VQwDataElement.h"
#include "VQwHardwareChannel.h"

// Forward declarations
class QwDBInterface;

template<typename T> class QwCombinedBCM;
template<typename T> class QwBCM;

/**
 * \ingroup QwAnalysis_BeamLine
 */
class VQwBCM : public VQwDataElement {
  /***************************************************************
   *  Class:  VQwBCM
   *          Pure Virtual base class for the BCMs in the beamline.
   *          Through use of the Create factory function, one can 
   *          get a concrete instance of a templated QwBCM.
   *
   ***************************************************************/
public:
  VQwBCM() { }; // Do not use this function!!
  VQwBCM(TString name){ // Do not use this function!!
      //InitializeChannel(name,"raw");
  };

  virtual ~VQwBCM() {
    //DeleteHistograms();
  };

  // VQwDataElement virtual functions
  virtual Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0) = 0;
  virtual void  ConstructHistograms(TDirectory *folder, TString &prefix) = 0;
  virtual void  FillHistograms() = 0;
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  virtual void SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability) = 0;
  virtual void Ratio( const VQwBCM &numer, const VQwBCM &denom)
    { std::cerr << "Ratio not defined! (VQwBCM)" << std::endl; }
  virtual void ClearEventData() = 0;

  // Virtual functions delegated to sub classes
  virtual void  InitializeChannel(TString name, TString datatosave) = 0;
  // new routine added to update necessary information for tree trimming
  virtual void  InitializeChannel(TString subsystem, TString name, TString datatosave) = 0;
  virtual void SetDefaultSampleSize(Int_t sample_size) = 0;
  virtual void SetEventCutMode(Int_t bcuts) = 0;
  virtual void SetPedestal(Double_t ped) = 0;
  virtual void SetCalibrationFactor(Double_t calib) = 0;
  virtual void RandomizeEventData(int helicity, double time) = 0;
  virtual void EncodeEventData(std::vector<UInt_t> &buffer) = 0;
  virtual Bool_t ApplySingleEventCuts() = 0;//Check for good events by stting limits on the devices readings
  virtual void  ProcessEvent() = 0;
  virtual void Scale(Double_t factor) = 0;
  virtual void CalculateRunningAverage() = 0;
  virtual void AccumulateRunningSum(const VQwBCM& value) = 0;
  virtual void ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values) = 0;
  virtual void ConstructBranch(TTree *tree, TString &prefix) = 0;
  virtual void ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist) = 0;
  virtual void FillTreeVector(std::vector<Double_t> &values) const = 0;
  virtual void DeleteHistograms() = 0;
  virtual void Copy(VQwDataElement *source) = 0;

  virtual std::vector<QwDBInterface> GetDBEntry() = 0;

  virtual const VQwHardwareChannel* GetCharge() const = 0;

  virtual void  SetRandomEventParameters(Double_t mean, Double_t sigma) = 0;
  virtual void  SetRandomEventAsymmetry(Double_t asymmetry) = 0 ;
  virtual void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency) =0;

  // Operators
  virtual VQwBCM& operator=  (const VQwBCM &value) =0;
  virtual VQwBCM& operator+= (const VQwBCM &value) =0;
  virtual VQwBCM& operator-= (const VQwBCM &value) =0;

  // This one is for QwCombinedBCM (could be done differently)
  virtual void SetBCMForCombo(VQwBCM* bcm, Double_t weight, Double_t sumqw ) = 0;

  // Factory function to produce appropriate BCM
  static VQwBCM* Create(TString subsystemname, TString type, TString name);
  static VQwBCM* Create(TString type); // Create a generic BCM (define properties later)

protected:
};


/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
template<typename T>
class QwBCM : public VQwBCM {
/////
  friend class QwCombinedBCM<T>;
 public:
  QwBCM() { };
  QwBCM(TString name){
    InitializeChannel(name,"raw");
  };
  QwBCM(TString subsystemname, TString name){
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name,"raw");
  };
  QwBCM(TString subsystemname, TString name, TString type){
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name,type,"raw");
  };
  ~QwBCM() {
    DeleteHistograms();
  };

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);

  void  InitializeChannel(TString name, TString datatosave);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name, TString datatosave);
  void  InitializeChannel(TString subsystem, TString name, TString type,
      TString datatosave);
  void  ClearEventData();


  void  SetRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  AddRandomEventDriftParameters(Double_t amplitude, Double_t phase, Double_t frequency);
  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void  RandomizeEventData(int helicity = 0, double time = 0);
  void  EncodeEventData(std::vector<UInt_t> &buffer);

  void  UseExternalRandomVariable();
  void  SetExternalRandomVariable(Double_t random_variable);

  void  ProcessEvent();
  Bool_t ApplyHWChecks();//Check for harware errors in the devices
  Bool_t ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure
  UInt_t GetEventcutErrorFlag(){//return the error flag
    return fBeamCurrent.GetEventcutErrorFlag();
  }

  Int_t SetSingleEventCuts(Double_t mean, Double_t sigma);//two limts and sample size
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  void SetSingleEventCuts(UInt_t errorflag,Double_t min, Double_t max, Double_t stability);
  
  void SetDefaultSampleSize(Int_t sample_size);
  void SetEventCutMode(Int_t bcuts){
    bEVENTCUTMODE=bcuts;
    fBeamCurrent.SetEventCutMode(bcuts);
  }

  void PrintValue() const;
  void PrintInfo() const;

  const VQwHardwareChannel* GetCharge() const {
    return &fBeamCurrent;
  };


  // Implementation of Parent class's virtual operators
  VQwBCM& operator=  (const VQwBCM &value);
  VQwBCM& operator+= (const VQwBCM &value);
  VQwBCM& operator-= (const VQwBCM &value);

  // This is used only by a QwComboBCM. It is placed here since in QwBeamLine we do
  // not readily have the appropriate template every time we want to use this
  // function.
  virtual void SetBCMForCombo(VQwBCM* bcm, Double_t weight, Double_t sumqw ) {
    std::cerr<<"SetBCMForCombo for QwCombinedBCM<T> not defined!!\n";
  };

  // This class specific operators
  QwBCM& operator=  (const QwBCM &value);
  QwBCM& operator+= (const QwBCM &value);
  QwBCM& operator-= (const QwBCM &value);
  void Sum(QwBCM &value1, QwBCM &value2);
  void Difference(QwBCM &value1, QwBCM &value2);
  void Ratio(const VQwBCM &numer, const VQwBCM &denom);
  void Ratio(const QwBCM &numer, const QwBCM &denom);
  void Scale(Double_t factor);

  void AccumulateRunningSum(const VQwBCM& value);
  void CalculateRunningAverage();

  void SetPedestal(Double_t ped);
  void SetCalibrationFactor(Double_t calib);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void  FillTreeVector(std::vector<Double_t> &values) const;
  void  DeleteHistograms();

  UInt_t   GetGoodEventCount() {return fBeamCurrent.GetGoodEventCount();};

  void Copy(VQwDataElement *source);

  std::vector<QwDBInterface> GetDBEntry();


/////
 protected:

/////
 private:
  Double_t fPedestal;
  Double_t fCalibration;
  Double_t fULimit, fLLimit;
  Bool_t fGoodEvent;//used to validate sequence number in the IsGoodEvent()




  T fBeamCurrent;

  Int_t fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode

  const static  Bool_t bDEBUG=kFALSE;//debugging display purposes
  Bool_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts do not depend on HW ckecks. This is set externally through the qweak_beamline_eventcuts.map

 private:
  //  Functions to be removed
  
  //  void  SetEventData(Double_t* block, UInt_t sequencenumber);
  /*   void  SetEventNumber(int event); */

  void  SetHardwareSum(Double_t hwsum, UInt_t sequencenumber = 0);
  Double_t GetAverage()        {return fBeamCurrent.GetValue();};
  Double_t GetAverageError()   {return fBeamCurrent.GetValueError();};


};


#endif // __VQWBCM__
