/**********************************************************\
* File: QwQPD.h                                           *
*                                                         *
* Author: B.Waidyawansa                                   *
* Time-stamp: 09-14-2010                                  *
\**********************************************************/

#ifndef __QwQPD__
#define __QwQPD__

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwVQWK_Channel.h"
#include "VQwBPM.h"
#include "QwParameterFile.h"

// Forward declarations
class QwDBInterface;
class QwErrDBInterface;

/*****************************************************************
*  Class:
******************************************************************/
///
/// \ingroup QwAnalysis_BL

class QwQPD : public VQwBPM {

 public:
  static UInt_t  GetSubElementIndex(TString subname);

  QwQPD() {
  };
  QwQPD(TString name):VQwBPM(name){
    InitializeChannel(name);
  };
  QwQPD(TString subsystemname, TString name):VQwBPM(name){
    SetSubsystemName(subsystemname);
    InitializeChannel(subsystemname, name);
    fQwQPDCalibration[0] = 1.0;
    fQwQPDCalibration[1] = 1.0;
  };    
  QwQPD(const QwQPD& source)
  : VQwBPM(source),
    fPhotodiode(source.fPhotodiode),
    fRelPos(source.fRelPos),
    fAbsPos(source.fAbsPos),
    fEffectiveCharge(source.fEffectiveCharge)
  { }
  virtual ~QwQPD() { };
  
  void    InitializeChannel(TString name);
  // new routine added to update necessary information for tree trimming
  void    InitializeChannel(TString subsystem, TString name);

  void LoadChannelParameters(QwParameterFile &paramfile){
    for(Short_t i=0;i<4;i++)
      fPhotodiode[i].LoadChannelParameters(paramfile);
  }

  void    GetCalibrationFactors(Double_t AlphaX, Double_t AlphaY);
  
  void    ClearEventData();
  Int_t   ProcessEvBuffer(UInt_t* buffer,
			UInt_t word_position_in_buffer,UInt_t indexnumber);
  void    ProcessEvent();

  const VQwHardwareChannel* GetPosition(EBeamPositionMonitorAxis axis) const {
    if (axis<0 || axis>2){
      TString loc="QwQPD::GetPosition for "
        +this->GetElementName()+" failed for axis value "+Form("%d",axis);
      throw std::out_of_range(loc.Data());
    }
    return &fAbsPos[axis];
  }
  const VQwHardwareChannel* GetEffectiveCharge() const {return &fEffectiveCharge;}

  TString GetSubElementName(Int_t subindex);
  void    GetAbsolutePosition(){};

  Bool_t  ApplyHWChecks();//Check for harware errors in the devices
  Bool_t  ApplySingleEventCuts();//Check for good events by stting limits on the devices readings
  //void    SetSingleEventCuts(TString ch_name, Double_t minX, Double_t maxX);
  /*! \brief Inherited from VQwDataElement to set the upper and lower limits (fULimit and fLLimit), stability % and the error flag on this channel */
  //void    SetSingleEventCuts(TString ch_name, UInt_t errorflag,Double_t min, Double_t max, Double_t stability);
  void    SetEventCutMode(Int_t bcuts);
  void IncrementErrorCounters();
  void PrintErrorCounters() const;// report number of events failed due to HW and event cut faliure
  UInt_t  GetEventcutErrorFlag();
  UInt_t  UpdateErrorFlag();
  void UpdateErrorFlag(const VQwBPM *ev_error);

  void    SetDefaultSampleSize(Int_t sample_size);
  void    SetRandomEventParameters(Double_t meanX, Double_t sigmaX, Double_t meanY, Double_t sigmaY);
  void    RandomizeEventData(int helicity = 0, double time = 0.0);
  void    SetEventData(Double_t* block, UInt_t sequencenumber);
  void    EncodeEventData(std::vector<UInt_t> &buffer);
  void    SetSubElementPedestal(Int_t j, Double_t value);
  void    SetSubElementCalibrationFactor(Int_t j, Double_t value);

  void    Ratio(QwQPD &numer, QwQPD &denom);
  void    Scale(Double_t factor);

  VQwBPM& operator=  (const VQwBPM &value);
  VQwBPM& operator+= (const VQwBPM &value);
  VQwBPM& operator-= (const VQwBPM &value);

  virtual QwQPD& operator=  (const QwQPD &value);
  virtual QwQPD& operator+= (const QwQPD &value);
  virtual QwQPD& operator-= (const QwQPD &value);

  void    AccumulateRunningSum(const QwQPD& value);
  void    AccumulateRunningSum(const VQwBPM& value);
  void    DeaccumulateRunningSum(VQwBPM &value);
  void    DeaccumulateRunningSum(QwQPD& value);
  void    CalculateRunningAverage();

  void    ConstructHistograms(TDirectory *folder, TString &prefix);
  void    FillHistograms();

  void    ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void    ConstructBranch(TTree *tree, TString &prefix);
  void    ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void    FillTreeVector(std::vector<Double_t> &values) const;



  std::vector<QwDBInterface> GetDBEntry();
  std::vector<QwErrDBInterface> GetErrDBEntry();
  void    MakeQPDList();


  protected:
  VQwHardwareChannel* GetSubelementByName(TString ch_name);


  /////
 private:


  static const TString subelement[4]; 
  /*  Position calibration factor, transform ADC counts in mm */
  Double_t fQwQPDCalibration[2];


 protected:
  QwVQWK_Channel fPhotodiode[4];
  QwVQWK_Channel fRelPos[2];

  //  These are the "real" data elements, to which the base class
  //  fAbsPos_base and fEffectiveCharge_base are pointers.
  QwVQWK_Channel fAbsPos[2];
  QwVQWK_Channel fEffectiveCharge;

  std::vector<QwVQWK_Channel> fQPDElementList;

};


#endif
