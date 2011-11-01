/**********************************************************\
* File: QwBeamLine.h                                      *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwBEAMLINE__
#define __QwBEAMLINE__

// System headers
#include <vector>

// ROOT headers
#include "TTree.h"
#include "TString.h"

// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwTypes.h"
#include "QwBPMStripline.h"
#include "VQwBCM.h"
#include "QwBCM.h"
#include "QwBPMCavity.h"
#include "QwCombinedBCM.h"
#include "QwCombinedBPM.h"
#include "QwEnergyCalculator.h"
#include "QwHaloMonitor.h"
#include "QwQPD.h"
#include "QwLinearDiodeArray.h"
#include "VQwClock.h"

// Forward declarations
class QwBeamDetectorID;


/*****************************************************************
*  Class:
******************************************************************/
class QwBeamLine : public VQwSubsystemParity, public MQwCloneable<QwBeamLine> {
 public:

  QwBeamLine(TString region_tmp):VQwSubsystem(region_tmp),VQwSubsystemParity(region_tmp),index_4mhz(-1)
    { };

  virtual ~QwBeamLine() { };


  /* derived from VQwSubsystem */
  
  void  ProcessOptions(QwOptions &options);//Handle command line options
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t LoadEventCuts(TString filename);//derived from VQwSubsystemParity
  Int_t LoadGeometryDefinition(TString mapfile);
  void  AssignGeometry(QwParameterFile* mapstr, VQwBPM * bpm);

  Bool_t ApplySingleEventCuts();//derived from VQwSubsystemParity
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliures
  UInt_t GetEventcutErrorFlag();//return the error flag
  //update the smae error flag in the classes belong to the subsystem.
  void UpdateEventcutErrorFlag(UInt_t errorflag);
  //update the error flag in the subsystem level from the top level routines related to stability checks. This will uniquely update the errorflag at each channel based on the error flag in the corresponding channel in the ev_error subsystem
  void UpdateEventcutErrorFlag(VQwSubsystem *ev_error);

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  PrintDetectorID() const;

  void  ClearEventData();
  void  ProcessEvent();

  Bool_t PublishInternalValues() const;

 public:
  void RandomizeEventData(int helicity = 0, double time = 0.0);
  void EncodeEventData(std::vector<UInt_t> &buffer);

  VQwSubsystem&  operator=  (VQwSubsystem *value);
  VQwSubsystem&  operator+= (VQwSubsystem *value);
  VQwSubsystem&  operator-= (VQwSubsystem *value);
  void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);

  void Scale(Double_t factor);

  void AccumulateRunningSum(VQwSubsystem* value);
  //remove one entry from the running sums for devices
  void DeaccumulateRunningSum(VQwSubsystem* value);


  void CalculateRunningAverage();

  using VQwSubsystem::ConstructHistograms;
  void ConstructHistograms(TDirectory *folder, TString &prefix);
  void FillHistograms();

  using VQwSubsystem::ConstructBranchAndVector;
  void ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void ConstructBranch(TTree *tree, TString &prefix);
  void ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& trim_file );
  void FillTreeVector(std::vector<Double_t> &values) const;
  void FillDB(QwDatabase *db, TString datatype);

  void Copy(VQwSubsystem *source);
  VQwSubsystem*  Copy();
  Bool_t Compare(VQwSubsystem *source);

  void PrintValue() const;
  void PrintInfo() const;


  VQwBPM* GetBPMStripline(const TString name);
  VQwBCM* GetBCM(const TString name);
  VQwClock* GetClock(const TString name);
  QwBPMCavity* GetBPMCavity(const TString name);
  VQwBCM* GetCombinedBCM(const TString name);
  VQwBPM* GetCombinedBPM(const TString name);
  QwEnergyCalculator* GetEnergyCalculator(const TString name);
  QwHaloMonitor* GetScalerChannel(const TString name);
  const QwBPMCavity* GetBPMCavity(const TString name) const;
  const VQwBPM* GetBPMStripline(const TString name) const;
  const VQwBCM* GetBCM(const TString name) const;
  const VQwClock* GetClock(const TString name) const;
  const VQwBCM* GetCombinedBCM(const TString name) const;
  const VQwBPM* GetCombinedBPM(const TString name) const;
  const QwEnergyCalculator* GetEnergyCalculator(const TString name) const;
  const QwHaloMonitor* GetScalerChannel(const TString name) const;


/////
 protected:
 Int_t GetDetectorIndex(EQwBeamInstrumentType TypeID, TString name);
 //when the type and the name is passed the detector index from appropriate vector will be returned
 //for example if TypeID is bcm  then the index of the detector from fBCM vector for given name will be returnd.
 typedef boost::shared_ptr<VQwBPM> VQwBPM_ptr;
 std::vector <VQwBPM_ptr> fStripline;
 std::vector <VQwBPM_ptr> fBPMCombo;

 typedef boost::shared_ptr<VQwBCM> VQwBCM_ptr;
 std::vector <VQwBCM_ptr> fBCM;
 std::vector <VQwBCM_ptr> fBCMCombo;

 typedef boost::shared_ptr<VQwClock> VQwClock_ptr;
 std::vector <VQwClock_ptr> fClock;

 std::vector <QwQPD> fQPD;
 std::vector <QwLinearDiodeArray> fLinearArray;
 std::vector <QwBPMCavity> fCavity;
 std::vector <QwHaloMonitor> fHaloMonitor;


 std::vector <QwEnergyCalculator> fECalculator;
 std::vector <QwBeamDetectorID> fBeamDetectorID;



/////
 private:
 // std::vector<TString> DetectorTypes;// for example could be BCM, LUMI,BPMSTRIPLINE, etc..
 Int_t fQwBeamLineErrorCount;
 Double_t fSumXweights;
 Double_t fSumYweights;
 Double_t fSumQweights;


 Int_t index_4mhz;//index of the 4mhz scaler in the QwHaloMonitor vector
 static const Bool_t bDEBUG=kFALSE;

};



class QwBeamDetectorID
{
 public:

  QwBeamDetectorID(Int_t subbankid, Int_t offset,TString name, TString dettype,
		   TString modtype);

  Int_t   fSubbankIndex;
  Int_t   fWordInSubbank;
  //first word reported for this channel in the subbank
  //(eg VQWK channel report 6 words for each event, scalers oly report one word per event)

  // The first word of the subbank gets fWordInSubbank=0

  TString fmoduletype; // eg: VQWK, SCALER
  TString fdetectorname;
  TString fdetectortype; // stripline, bcm, ... this string is encoded by fTypeID

  Int_t   kUnknownDeviceType;
  EQwBeamInstrumentType   fTypeID; // type of detector eg: bcm or stripline, etc..
  Int_t   fIndex;            // index of this detector in the vector containing all the detector of same type
  UInt_t  fSubelement;       // some detectors have many subelements (eg stripline have 4 antenas) some have only one sub element(eg lumis have one channel)


  void    Print() const;

};



#endif
