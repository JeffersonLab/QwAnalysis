#ifndef __VQwDataserver_h__
#define __VQwDataserver_h__

// Qweak headers
#include "QwLog.h"
#include "VQwSystem.h"

class QwSubsystemArray;
class QwEventBuffer;

/**
 *  \class VQwDataserver
 *  \ingroup QwThreadSafe
 *  \brief A framework for a thread-safe dataserver object
 *
 * The dataserver object
 *
 */
class VQwDataserver : public VQwSystem {

  private:
    /// Private assignment operator, to prevent it from being used.
    VQwDataserver& operator= (const VQwDataserver &value) {
      return *this;
    };

  protected:
    /// Event buffer which serves CODA events
    QwEventBuffer* fEventBuffer;	//!

    /// Current run number
    Int_t fCurrentRun;
    /// Current event number
    Int_t fCurrentEvent;

  public:
    /// Default constructor
    VQwDataserver (const char* name = 0): VQwSystem (name) { };
    /// Destructor
    virtual ~VQwDataserver() { };

    /// Get a new run from the IO stream
    virtual Int_t GetRun(Int_t run) {
      QwError << "VQwDataserver::GetRun not implemented!" << QwLog::endl;
      return -1;
    };
    /// Get a new event from the IO stream
    virtual Int_t GetEvent() {
      QwError << "VQwDataserver::GetEvent not implemented!" << QwLog::endl;
      return -1;
    };

    /// Get the current run number
    virtual Int_t GetRunNumber() { return fCurrentRun; };
    /// Get the current event number
    virtual Int_t GetEventNumber() { return fCurrentEvent; };

    /// Open a CODA data stream
    virtual Int_t OpenDataStream() {
      QwError << "VQwDataserver::OpenDataStream not implemented!" << QwLog::endl;
      return -1;
    };
    /// Open a CODA data file
    virtual Int_t OpenDataFile() {
      QwError << "VQwDataserver::OpenDataFile not implemented!" << QwLog::endl;
      return -1;
    };
    /// Close a CODA data stream
    virtual Int_t CloseDataStream() {
      QwError << "VQwDataserver::OpenDataStream not implemented!" << QwLog::endl;
      return -1;
    };
    /// Close a CODA data file
    virtual void CloseDataFile();


    /// Pass the CODA data to the specified subsystem array (not thread-safe!)
    virtual void FillSubsystemData(QwSubsystemArray* detectors) {
      QwError << "VQwDataserver::FillSubsystemData not implemented!" << QwLog::endl;
      return;
    };

  ClassDef(VQwDataserver,1);

}; // class VQwDataserver

#endif // __VQwDataserver_h__
