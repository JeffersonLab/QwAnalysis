/**********************************************************\
* File: QwBeamMod.h                                       *
*                                                         *
* Author: Joshua Hoskins                                  *
* Time-stamp: 052510                                      *
\**********************************************************/

#ifndef __QwBEAMMOD__
#define __QwBEAMMOD__

#include <vector>
#include "TTree.h"

#include "VQwSubsystemParity.h"
#include <QwVQWK_Channel.h>

//enum EBeamInstrumentType{kBPMStripline = 0,
//			 kBCM,
//			 kCombinedBCM,
//			 kCombinedBPM};

// this emun vector needs to be coherent with the DetectorTypes declaration in the QwBeamLine constructor

class QwModChannelID;

/*****************************************************************
*  Class:
******************************************************************/
class QwBeamMod: public VQwSubsystemParity{

 public:

  QwBeamMod(TString region_tmp):VQwSubsystem(region_tmp),VQwSubsystemParity(region_tmp)
    {

      // these declaration need to be coherent with the enum vector EBeamInstrumentType
      fgModTypeNames.push_back(""); // Need to define these wrt to our detector types.
      fgModTypeNames.push_back("");
      fgModTypeNames.push_back("");
      fgModTypeNames.push_back("");
      for(size_t i=0;i<fgModTypeNames.size();i++)
        fgModTypeNames[i].ToLower();
    };

  ~QwBeamMod() {
    DeleteHistograms();
  };

 std::vector<TString> fgModTypeNames;
  /* derived from VQwSubsystem */
  void ProcessOptions(QwOptions &options);//Handle command line options
  void AccumulateRunningSum(VQwSubsystem*);

  Int_t LoadChannelMap(TString mapfile);
  Int_t LoadEventCuts(TString filename);//derived from VQwSubsystemParity
  Int_t LoadGeometry(TString mapfile);
  Int_t LoadInputParameters(TString pedestalfile);


  Bool_t ApplySingleEventCuts();//derived from VQwSubsystemParity
  Int_t GetEventcutErrorCounters();// report number of events falied due to HW and event cut faliures
  Int_t GetEventcutErrorFlag();//return the error flag

  Int_t ProcessConfigurationBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
  Int_t ProcessEvBuffer(const UInt_t roc_id, const UInt_t bank_id, UInt_t* buffer, UInt_t num_words);
//  void  PrintDetectorID();

  void  ClearEventData();
  void  ProcessEvent();


//  const Bool_t PublishInternalValues() const;
//  const Bool_t ReturnInternalValue(TString name, VQwDataElement* value) const;

//  void RandomizeEventData(int helicity = 0);
//  void EncodeEventData(std::vector<UInt_t> &buffer);

  VQwSubsystem&  operator=  (VQwSubsystem *value);
  VQwSubsystem&  operator+= (VQwSubsystem *value);
  VQwSubsystem&  operator-= (VQwSubsystem *value);


  void Sum(VQwSubsystem  *value1, VQwSubsystem  *value2);

  void Difference(VQwSubsystem  *value1, VQwSubsystem  *value2);
  void Ratio(VQwSubsystem *numer, VQwSubsystem *denom);

  void Scale(Double_t factor);
/*
  void Calculate_Running_Average();
  void Do_RunningSum();
*/

  void CalculateRunningAverage();
  void  PrintModChannelID();
  void ConstructHistograms(TDirectory *folder, TString &prefix);
  void FillHistograms();
  void DeleteHistograms();


  void ConstructBranchAndVector(TTree *tree, TString &prefix, std::vector<Double_t> &values);
  void  ConstructBranch(TTree *tree, TString& prefix) { };
  void  ConstructBranch(TTree *tree, TString& prefix, QwParameterFile& trim_file) { };
  void FillTreeVector(std::vector<Double_t> &values) const;
  void FillDB(QwDatabase *db, TString datatype);

  void Copy(VQwSubsystem *source);
  VQwSubsystem*  Copy();

  Bool_t Compare(VQwSubsystem *source);

  void Print();

 protected:
 Int_t GetDetectorTypeID(TString name);
 Int_t GetDetectorIndex(Int_t TypeID, TString name);    // when the type and the name is passed the detector index from appropriate vector will be returned
 						        // for example if TypeID is bcm  then the index of the detector from fBCM vector for given name will be returnd.


 //std::vector <QwBCM> fBCM;
 //std::vector <QwCombinedBCM> fBCMCombo;
 //std::vector <QwCombinedBPM> fBPMCombo;

 std::vector <QwVQWK_Channel> fModChannel;
 std::vector <QwModChannelID> fModChannelID;





 private:

 // Int_t fQwBeamLineErrorCount;
 // Double_t fSumXweights;
 // Double_t fSumYweights;
 // Double_t fSumQweights;


 static const Bool_t bDEBUG=kFALSE;

};



class QwModChannelID
{
 public:

  QwModChannelID(Int_t subbankid, Int_t wordssofar,TString name,
		   TString modtype ,QwBeamMod * obj);


/*     QwModChannelID():fSubbankIndex(-1),fWordInSubbank(-1),fTypeID(-1),fIndex(-1), */
/*     fSubelement(999999),fmoduletype(""),fmodulename("") */
/*     {}; */

  Int_t fSubbankIndex;        //Generated from ROCID(readout CPU) & BankID(corespondes to internal headers to ID differnt types of data..ex. F1TDC)
  Int_t fWordInSubbank;
  //first word reported for this channel in the subbank
  //(eg VQWK channel report 6 words for each event, scalers oly report one word per event)

  // The first word of the subbank gets fWordInSubbank=0

  TString fmoduletype; // eg: VQWK, SCALER
  TString fmodulename;
 // TString fdetectortype;

  Int_t  kUnknownDeviceType;
  Int_t  fTypeID;           // type of detector eg: lumi or stripline, etc..
  Int_t  fIndex;            // index of this detector in the vector containing all the detector of same type

  void Print();

};



#endif
