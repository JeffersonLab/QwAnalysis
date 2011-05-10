/**********************************************************\
* File: QwBCM.h                                  *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwVQWK_BCM__
#define __QwVQWK_BCM__

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

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL
template<typename T>
class QwBCM : public VQwDataElement{
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
  ~QwBCM() {
    DeleteHistograms();
  };

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t word_position_in_buffer, UInt_t subelement=0);

  void  InitializeChannel(TString name, TString datatosave);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name, TString datatosave);
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


  QwBCM& operator=  (const QwBCM &value);
  QwBCM& operator+= (const QwBCM &value);
  QwBCM& operator-= (const QwBCM &value);
  void Sum(QwBCM &value1, QwBCM &value2);
  void Difference(QwBCM &value1, QwBCM &value2);
  void Ratio(QwBCM &numer, QwBCM &denom);
  void Scale(Double_t factor);

  void AccumulateRunningSum(const QwBCM& value);
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

#endif
