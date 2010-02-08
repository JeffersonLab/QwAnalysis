#ifndef __VQwDataserver_h__
#define __VQwDataserver_h__

// System headers
#include <iostream>

// Qweak headers
#include "QwLog.h"
#include "VQwSystem.h"

class QwSubsystemArray;
class QwEventBuffer;

class VQwDataserver : public VQwSystem {

  private:
    // Declare assignment operator private
    VQwDataserver& operator= (const VQwDataserver &value) {
      return *this;
    };

  protected:
    QwEventBuffer* fEventBuffer;	//!

  public:
    VQwDataserver (const char* name = 0): VQwSystem (name) { };
    virtual ~VQwDataserver() { };

    virtual Int_t GetRun(Int_t run) {
      QwError << "VQwDataserver::GetRun not implemented!" << QwLog::endl;
      return -1;
    };
    virtual Int_t GetEvent() {
      QwError << "VQwDataserver::GetEvent not implemented!" << QwLog::endl;
      return -1;
    };
    virtual Int_t GetEventNumber() {
      QwError << "VQwDataserver::GetEventNumber not implemented!" << QwLog::endl;
      return -1;
    };

    virtual Int_t OpenDataStream() {
      QwError << "VQwDataserver::OpenDataStream not implemented!" << QwLog::endl;
      return -1;
    };
    virtual Int_t OpenDataFile() {
      QwError << "VQwDataserver::OpenDataFile not implemented!" << QwLog::endl;
      return -1;
    };
    virtual Int_t CloseDataStream() {
      QwError << "VQwDataserver::OpenDataStream not implemented!" << QwLog::endl;
      return -1;
    };
    virtual void CloseDataFile() {
//       // Close CODA file
//       fEventBuffer->CloseDataFile();
//       fEventBuffer->ReportRunSummary();
      return;
    };


    virtual void FillSubsystemData(QwSubsystemArray* detectors) {
      QwError << "VQwDataserver::FillSubsystemData not implemented!" << QwLog::endl;
      return;
    };


  ClassDef(VQwDataserver,1); // corresponding ClassImp in QwRoot.h
};

#endif // __VQwDataserver_h__
