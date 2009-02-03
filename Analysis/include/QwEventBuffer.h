/**********************************************************\
* File: QwEventBuffer.h                                    *
*                                                          *
* Author: P. M. King                                       *
* Time-stamp: <2008-07-22 15:40>                           *
\**********************************************************/

#ifndef __QWEVENTBUFFER__
#define __QWEVENTBUFFER__


#include <vector>
#include "Rtypes.h"
#include "TString.h"

#include "THaCodaData.h"
#include "MQwCodaControlEvent.h"

#include "QwSubsystemArray.h"
#include "VQwSubsystem.h"

//////////////////////////////////////////////////////////////////////



class QwEventBuffer: public MQwCodaControlEvent{
 public:
  static const Int_t kRunNotSegmented;
  static const Int_t kNoNextDataFile;
  static const Int_t kFileHandleNotConfigured;

  static const UInt_t kNullDataWord;

 public:
  QwEventBuffer();
  virtual ~QwEventBuffer() {
    if (fEvStream!=NULL){
      delete fEvStream;
      fEvStream = NULL;
    }
  };




  void    SetDataDirectory(const TString datadir){fDataDirectory = datadir;}


  const TString&  GetDataFile() const {return fDataFile;};


  Int_t OpenDataFile(UInt_t current_run, Short_t seg);
  Int_t OpenDataFile(UInt_t current_run);
  Int_t OpenDataFile(const TString filename, const TString rw);
  Int_t CloseDataFile();

  Bool_t IsPhysicsEvent(){
    return ((fIDBankNum == 0xCC)&&(fEvtType>=0 && fEvtType<=15));
  };
  Int_t GetEventNumber(){return fEvtNumber;};

  Int_t  GetEvent();
  
  Bool_t IsROCConfigurationEvent(){
    return (fEvtType>=0x90 && fEvtType<=0xaf);
  };

  Bool_t FillSubsystemConfigurationData(std::vector<VQwSubsystem*> &subsystems);
  Bool_t FillSubsystemData(std::vector<VQwSubsystem*> &subsystems);

  Bool_t FillSubsystemConfigurationData(QwSubsystemArray &subsystems);
  Bool_t FillSubsystemData(QwSubsystemArray &subsystems);

  void ResetFlags();


 protected:
  const Bool_t fDEBUG;
  const TString fDataFileStem;
  const TString fDataFileExtension;

  TString fDataDirectory;
  
  TString fDataFile;

  UInt_t fRunNumber;
  

 protected:
  Int_t  GetFileEvent();
  Int_t  GetEtEvent();


  Bool_t DataFileIsSegmented();

  void CloseThisSegment();
  Int_t OpenNextSegment();

  void DecodeEventIDBank(UInt_t *buffer);
  Bool_t DecodeSubbankHeader(UInt_t *buffer);


  const TString&  DataFile(const UInt_t run, const Short_t seg);
  
  void SetEventLength(const ULong_t tmplength) {fEvtLength = tmplength;};
  void SetEventType(const UInt_t tmptype) {fEvtType = tmptype;};
  void SetWordsSoFar(const ULong_t tmpwords) {fWordsSoFar = tmpwords;};
 


 protected:
  

  Bool_t fRunIsSegmented;


  std::vector<Int_t>           fRunSegments;
  std::vector<Int_t>::iterator this_runsegment;

  enum CodaStreamMode{fEvStreamNull, fEvStreamFile, fEvStreamET} fEvStreamMode;
  THaCodaData *fEvStream; //  Pointer to a THaCodaFile or THaEtClient

 protected:
  Bool_t fPhysicsEventFlag;

  UInt_t fEvtLength;
  UInt_t fWordsSoFar;

  UInt_t fEvtType;

  UInt_t fEvtTag;
  UInt_t fBankDataType;
  UInt_t fIDBankNum;

  UInt_t fEvtNumber;
  UInt_t fEvtClass;
  UInt_t fStatSum;

  UInt_t fFragLength;
  UInt_t fSubbankTag;  
  UInt_t fSubbankType;  
  UInt_t fSubbankNum;
  UInt_t fROC;




};

#endif
