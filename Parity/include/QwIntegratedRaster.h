/**********************************************************\
* File: QwIntegratedRaster.h                              *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwIntegratedRaster__
#define __QwIntegratedRaster__

// System headers
#include <vector>

// ROOT headers
#include "TTree.h"

// Qweak headers
#include "VQwSubsystemParity.h"

#include "QwIntegratedRasterChannel.h"
#include "QwTypes.h"


class QwIntegratedRasterDetectorID{

 public:

  QwIntegratedRasterDetectorID():fSubbankIndex(-1),fWordInSubbank(-1),
    fIndex(-1),
    fSubelement(999999),fmoduletype(""),fdetectorname("")
    {};

  int fSubbankIndex;
  int fWordInSubbank; //first word reported for this channel in the subbank
                      //(eg VQWK channel report 6 words for each event, scalers oly report one word per event)
  // The first word of the subbank gets fWordInSubbank=0

  //  EQwInstrumentType fTypeID;     // type of detector
  int fIndex;      // index of this detector in the vector containing all the detector of same type
  UInt_t fSubelement; // some detectors have many subelements (eg stripline have 4 antenas) some have only one sub element(eg lumis have one channel)

  TString fmoduletype; // eg: VQWK, SCALER
  TString fdetectorname;
  TString fdetectortype; // IntegratedRasterChannel,fLumiCounter .. this string is encoded by fTypeID
  //  std::vector<TString> fCombinedChannelNames;
  //  std::vector<Double_t> fWeight;
  void Print() const;

};


/*****************************************************************
*  Class:
******************************************************************/
class QwIntegratedRaster : public VQwSubsystemParity, public MQwCloneable<QwIntegratedRaster> {
  /////  
public:
  QwIntegratedRaster(TString region_tmp):VQwSubsystem(region_tmp),VQwSubsystemParity(region_tmp)
   {
   };

  ~QwIntegratedRaster() {
    DeleteHistograms();
  };


  /* derived from VQwSubsystem */

  /// \brief Define options function
  static void DefineOptions(QwOptions &options);


  void ProcessOptions(QwOptions &options);//Handle command line options
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t LoadEventCuts(TString filename);//derived from VQwSubsystemParity
  Bool_t ApplySingleEventCuts();//derived from VQwSubsystemParity
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliures
  UInt_t GetEventcutErrorFlag();//return the error flag

  void AccumulateRunningSum(VQwSubsystem* value);
  void CalculateRunningAverage();

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  PrintDetectorID() const;

  void  ClearEventData();
  Bool_t IsGoodEvent();

  void  ProcessEvent();
  void  ExchangeProcessedData(){};
  void  ProcessEvent_2(){};

  void  SetRandomEventParameters(Double_t mean, Double_t sigma);
  void  SetRandomEventAsymmetry(Double_t asymmetry);
  void RandomizeEventData(int helicity = 0, double time = 0.0);
  void EncodeEventData(std::vector<UInt_t> &buffer);

  VQwSubsystem&  operator=  (VQwSubsystem *value);
  VQwSubsystem&  operator+= (VQwSubsystem *value);
  VQwSubsystem&  operator-= (VQwSubsystem *value);
  void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);
  void Scale(Double_t factor);

  using VQwSubsystem::ConstructHistograms;
  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString &prefix);
  void  ConstructBranch(TTree *tree, TString &prefix, QwParameterFile& trim_file);
  void  FillTreeVector(std::vector<Double_t> &values) const;
  void  FillDB(QwDatabase *db, TString datatype);

  const VQwDataElement* GetChannel(const TString name) const;

  void Copy(VQwSubsystem *source);
  VQwSubsystem*  Copy();
  Bool_t Compare(VQwSubsystem *source);

  void PrintValue() const;
  void PrintInfo() const;

/////
protected: 
 
  // EQwPMTInstrumentType GetDetectorTypeID(TString name);
 
  Int_t GetDetectorIndex(TString name);

  std::vector <QwIntegratedRasterChannel<QwVQWK_Channel> >      fIntegratedRasterChannel;  
  std::vector <QwIntegratedRasterDetectorID>   fDetectorIDs;

protected:

/////
private:
  Int_t fQwIntegratedRasterErrorCount;

  static const Bool_t bDEBUG=kFALSE;

};




#endif
