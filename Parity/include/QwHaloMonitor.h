/**********************************************************\
* File: QwHaloMonitor.h                                   *
*                                                         *
* Author:B. Waidyawansa                                   *
* Time-stamp:24-june-2010                                 *
\**********************************************************/

#ifndef __QwHALO_MONITOR__
#define __QwHALO_MONITOR_

// System headers
#include <vector>

// ROOT headers
#include <TTree.h>

// Qweak headers
#include "QwParameterFile.h"
#include "QwScaler_Channel.h"

// Forward declarations
class QwDBInterface;

/*****************************************************************
*  Class: QwHaloMonitor handles the halo counters. This use
          QwSIS3801_Channel scaler channels.
******************************************************************/
///
/// \ingroup QwAnalysis_BL
class  QwHaloMonitor : public VQwDataElement{
/////

 public:
  QwHaloMonitor() { };
  QwHaloMonitor(TString name){
    InitializeChannel(name);
  };

  QwHaloMonitor(TString subsystemname, TString name){
    InitializeChannel(subsystemname, name);
  };
  ~QwHaloMonitor() {DeleteHistograms();};

  void  InitializeChannel(TString name);
  // new routine added to update necessary information for tree trimming
  void  InitializeChannel(TString subsystem, TString name);
  void  ClearEventData();
  void  ReportErrorCounters();//This will display the error summary for each device
  void  UpdateHWErrorCount();//Update error counter for HW faliure

  Int_t ProcessEvBuffer(UInt_t* buffer, UInt_t num_words_left,UInt_t index=0);
  void  ProcessEvent();

  QwHaloMonitor& operator=  (const QwHaloMonitor &value);
  QwHaloMonitor& operator+= (const QwHaloMonitor &value);
  QwHaloMonitor& operator-= (const QwHaloMonitor &value);
  void Sum(QwHaloMonitor &value1, QwHaloMonitor &value2);
  void Difference(QwHaloMonitor &value1, QwHaloMonitor &value2);
  void Ratio(QwHaloMonitor &numer, QwHaloMonitor &denom);
  void Offset(Double_t Offset);
  void Scale(Double_t Offset);

  void AccumulateRunningSum(const QwHaloMonitor& value);
  void CalculateRunningAverage();

  Bool_t ApplySingleEventCuts();//check values read from modules are at desired level
  Int_t  GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliure
  Bool_t ApplyHWChecks();

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& modulelist);
  void  FillTreeVector(std::vector<Double_t> &values) const;

  void  Copy(VQwDataElement *source);

  void  PrintValue() const;
  void  PrintInfo() const;

  std::vector<QwDBInterface> GetDBEntry();

 protected:

 private:
  static const Bool_t kDEBUG;

  QwSIS3801D24_Channel fHalo_Counter;

  Int_t  fDeviceErrorCode;//keep the device HW status using a unique code from the QwVQWK_Channel::fDeviceErrorCode
  Bool_t bEVENTCUTMODE;//If this set to kFALSE then Event cuts do not depend on HW ckecks. This is set externally through the qweak_beamline_eventcuts.map
  Bool_t fEventIsGood;
};


#endif
