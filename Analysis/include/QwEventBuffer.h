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

class QwOptions;

//////////////////////////////////////////////////////////////////////



///
/// \ingroup QwAnalysis
class QwEventBuffer: public MQwCodaControlEvent{
 public:
  static void DefineOptions(QwOptions &options);

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

  /// \brief Sets internal flags based on the QwOptions
  void ProcessOptions(QwOptions &options);

  /// \brief Returns a string like <run#> or <run#>.<file#>
  TString GetRunLabel() const;

  /// \brief Opens the event stream (file or ET) based on the internal flags
  Int_t OpenNextStream();
  /// \brief Closes a currently open event stream.
  Int_t CloseStream();

  void    SetDataDirectory(const TString datadir){fDataDirectory = datadir;}


  const TString&  GetDataFile() const {return fDataFile;};


  Int_t OpenDataFile(UInt_t current_run, Short_t seg);
  Int_t OpenDataFile(UInt_t current_run, const TString rw = "R");
  Int_t OpenDataFile(const TString filename, const TString rw = "R");
  Int_t CloseDataFile();

  Int_t OpenETStream(TString computer, TString session, int mode);
  Int_t CloseETStream();

  Bool_t IsPhysicsEvent(){
    return ((fIDBankNum == 0xCC)&&(fEvtType>=0 && fEvtType<=15));
  };
  Int_t GetEventNumber(){return fEvtNumber;};

  Int_t GetNextEvent();

  Int_t  GetEvent();
  Int_t  WriteEvent(int* buffer);

  Bool_t IsROCConfigurationEvent(){
    return (fEvtType>=0x90 && fEvtType<=0xaf);
  };

  Bool_t FillSubsystemConfigurationData(QwSubsystemArray &subsystems);
  Bool_t FillSubsystemData(QwSubsystemArray &subsystems);


  Int_t EncodeSubsystemData(QwSubsystemArray &subsystems);
  Int_t EncodePrestartEvent(int runnumber, int runtype = 0);
  Int_t EncodeGoEvent();
  Int_t EncodePauseEvent();
  Int_t EncodeEndEvent();

  void ResetFlags();

 private:
  //  These methods will be removed from a future version
  void ClearEventData(std::vector<VQwSubsystem*> &subsystems);

  Bool_t FillSubsystemConfigurationData(std::vector<VQwSubsystem*> &subsystems);
  Bool_t FillSubsystemData(std::vector<VQwSubsystem*> &subsystems);


 protected:
  ///
  Bool_t fOnline;
  TString fETHostname;
  TString fETSession;
  std::pair<Int_t, Int_t> fRunRange;
  Bool_t fChainDataFiles;
  std::pair<UInt_t, UInt_t> fEventRange;

 protected:
  const TString fDataFileStem;
  const TString fDataFileExtension;

  TString fDataDirectory;

  TString fDataFile;

  UInt_t fRunNumber;


 protected:
  Int_t  GetFileEvent();
  Int_t  GetEtEvent();

  Int_t WriteFileEvent(int* buffer);

  Bool_t DataFileIsSegmented();

  Int_t CloseThisSegment();
  Int_t OpenNextSegment();

  void DecodeEventIDBank(UInt_t *buffer);
  Bool_t DecodeSubbankHeader(UInt_t *buffer);

  const TString&  DataFile(const UInt_t run, const Short_t seg);

  void SetEventLength(const ULong_t tmplength) {fEvtLength = tmplength;};
  void SetEventType(const UInt_t tmptype) {fEvtType = tmptype;};
  void SetWordsSoFar(const ULong_t tmpwords) {fWordsSoFar = tmpwords;};



 protected:
  enum CodaStreamMode{fEvStreamNull, fEvStreamFile, fEvStreamET} fEvStreamMode;
  THaCodaData *fEvStream; //  Pointer to a THaCodaFile or THaEtClient

  Int_t fCurrentRun;

  Bool_t fRunIsSegmented;


  std::vector<Int_t>           fRunSegments;
  std::vector<Int_t>::iterator this_runsegment;


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

 protected:
  UInt_t     fNumPhysicsEvents;

};

#endif
