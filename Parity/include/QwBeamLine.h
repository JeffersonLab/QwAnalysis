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

// Qweak headers
#include "VQwSubsystemParity.h"
#include "QwTypes.h"
#include "QwBPMStripline.h"
#include "QwBCM.h"
#include "QwBPMCavity.h"
#include "QwCombinedBCM.h"
#include "QwCombinedBPM.h"
#include "QwEnergyCalculator.h"
#include "QwHaloMonitor.h"
#include "QwQPD.h"
#include "QwLinearDiodeArray.h"


class QwBeamDetectorID;

/*****************************************************************
*  Class:
******************************************************************/
class QwBeamLine : public VQwSubsystemParity{

 public:

  QwBeamLine(TString region_tmp):VQwSubsystem(region_tmp),VQwSubsystemParity(region_tmp)
    { };

  ~QwBeamLine() {
    DeleteHistograms();
  };


  /* derived from VQwSubsystem */
  
  void  ProcessOptions(QwOptions &options);//Handle command line options
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t LoadEventCuts(TString filename);//derived from VQwSubsystemParity
  Int_t LoadGeometryDefinition(TString mapfile);

  Bool_t ApplySingleEventCuts();//derived from VQwSubsystemParity
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliures
  Int_t GetEventcutErrorFlag();//return the error flag

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
  void CalculateRunningAverage();

  void ConstructHistograms(TDirectory *folder, TString &prefix);
  void FillHistograms();
  void DeleteHistograms();

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


  QwBPMStripline* GetBPMStripline(const TString name);
  QwBCM* GetBCM(const TString name);
  QwBPMCavity* GetBPMCavity(const TString name);
  QwCombinedBCM* GetCombinedBCM(const TString name);
  QwCombinedBPM* GetCombinedBPM(const TString name);
  QwEnergyCalculator* GetEnergyCalculator(const TString name);
  const QwBPMCavity* GetBPMCavity(const TString name) const;
  const QwBPMStripline* GetBPMStripline(const TString name) const;
  const QwBCM* GetBCM(const TString name) const;
  const QwCombinedBCM* GetCombinedBCM(const TString name) const;
  const QwCombinedBPM* GetCombinedBPM(const TString name) const;
  const QwEnergyCalculator* GetEnergyCalculator(const TString name) const;



/////
 protected:
 Int_t GetDetectorIndex(EQwBeamInstrumentType TypeID, TString name);
 //when the type and the name is passed the detector index from appropriate vector will be returned
 //for example if TypeID is bcm  then the index of the detector from fBCM vector for given name will be returnd.
 std::vector <QwBPMStripline> fStripline;
 std::vector <QwBCM> fBCM;
 std::vector <QwQPD> fQPD;
 std::vector <QwLinearDiodeArray> fLinearArray;
 std::vector <QwBPMCavity> fCavity;
 std::vector <QwHaloMonitor> fHaloMonitor;
 std::vector <QwCombinedBCM> fBCMCombo;
 std::vector <QwCombinedBPM> fBPMCombo;
 std::vector <QwEnergyCalculator> fECalculator;
 std::vector <QwBeamDetectorID> fBeamDetectorID;



/////
 private:
 // std::vector<TString> DetectorTypes;// for example could be BCM, LUMI,BPMSTRIPLINE, etc..
 Int_t fQwBeamLineErrorCount;
 Double_t fSumXweights;
 Double_t fSumYweights;
 Double_t fSumQweights;


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
