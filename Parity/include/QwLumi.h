/**********************************************************\
* File: QwLumi.h                                          *
*                                                         *
* Author:                                                 *
* Time-stamp:                                             *
\**********************************************************/

#ifndef __QwLUMI__
#define __QwLUMI__

#include <vector>
#include "TTree.h"

#include "VQwSubsystemParity.h"


#include "QwIntegrationPMT.h"


enum ELumiInstrumentType{kIntegrationPMT};
// this emun vector needs to be coherent with the DetectorTypes declaration in the QwLumi constructor


///
/// \ingroup QwAnalysis_BL
class QwLumiDetectorID
{
 public:
  QwLumiDetectorID():fSubbankIndex(-1),fWordInSubbank(-1),fTypeID(-1),fIndex(-1),
    fSubelement(999999),fmoduletype(""),fdetectorname("")
    {};

  int fSubbankIndex;
  int fWordInSubbank; //first word reported for this channel in the subbank
                      //(eg VQWK channel report 6 words for each event, scalers oly report one word per event)
  // The first word of the subbank gets fWordInSubbank=0

  int fTypeID;     // type of detector eg: lumi or stripline, etc..
  int fIndex;      // index of this detector in the vector containing all the detector of same type
  UInt_t fSubelement; // some detectors have many subelements (eg stripline have 4 antenas) some have only one sub element(eg lumis have one channel)

  TString fmoduletype; // eg: VQWK, SCALER
  TString fdetectorname;
  TString fdetectortype; // stripline, IntegrationPMT, ... this string is encoded by fTypeID

  

  void Print();

};


/*****************************************************************
*  Class:
******************************************************************/
class QwLumi : public VQwSubsystemParity{
  /////
 public:
  QwLumi(TString region_tmp):VQwSubsystem(region_tmp),VQwSubsystemParity(region_tmp)
    {
      // these declaration need to be coherent with the enum vector ELumiInstrumentType
      DetectorTypes.push_back("IntegrationPMT");
      for(size_t i=0;i<DetectorTypes.size();i++)
	DetectorTypes[i].ToLower();

    };

  ~QwLumi() {
    DeleteHistograms();
  };


  /* derived from VQwSubsystem */
  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);
  Int_t LoadEventCuts(TString filename);//derived from VQwSubsystemParity
  Bool_t ApplySingleEventCuts();//derived from VQwSubsystemParity
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliures
  Int_t GetEventcutErrorFlag();//return the error flag 
  
  void Calculate_Running_Average();
  void Do_RunningSum(); 
  
  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  Int_t ProcessEvBuffer(UInt_t roc_id, UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  void  PrintDetectorID();

  void  ClearEventData();
  void  ProcessEvent();
  Bool_t IsGoodEvent();

  void RandomizeEventData(int helicity = 0);
  void EncodeEventData(std::vector<UInt_t> &buffer);

  VQwSubsystem&  operator=  (VQwSubsystem *value);
  VQwSubsystem&  operator+= (VQwSubsystem *value);
  VQwSubsystem&  operator-= (VQwSubsystem *value);
  void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);

  void Scale(Double_t factor);

  void  ConstructHistograms(TDirectory *folder, TString &prefix);
  void  FillHistograms();
  void  DeleteHistograms();

  void  ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  FillTreeVector(std::vector<Double_t> &values);
  void FillDB(QwDatabase *db){};

  void Copy(VQwSubsystem *source);
  VQwSubsystem*  Copy();
  Bool_t Compare(VQwSubsystem *source);
  void Print();

  QwIntegrationPMT* GetIntegrationPMT(const TString name);

/////
 protected:
 Int_t GetDetectorTypeID(TString name);
 Int_t GetDetectorIndex(Int_t TypeID, TString name);//when the type and the name is passed the detector index from appropriate vector will be returned
 //for example if TypeID is IntegrationPMT  then the index of the detector from fIntegrationPMT vector for given name will be returnd.
 std::vector <QwIntegrationPMT> fIntegrationPMT;
 std::vector <QwLumiDetectorID> fLumiDetectorID;


 


/////
 private:
 std::vector<TString> DetectorTypes;// for example could be IntegrationPMT, LUMI,BPMSTRIPLINE, etc..
 Int_t fQwLumiErrorCount;

  
 static const Bool_t bDEBUG=kFALSE; 

};




#endif
