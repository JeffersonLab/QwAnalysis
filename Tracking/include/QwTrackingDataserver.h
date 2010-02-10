#ifndef __QwTrackingDataserver_h__
#define __QwTrackingDataserver_h__

// Inherits from
#include "VQwDataserver.h"

// Forward declarations
class QwEventBuffer;

/**
 *  \class QwTrackingDataserver
 *  \ingroup QwThreadSafe
 *  \brief Self-contained, thread-safe tracking dataserver object
 */
class QwTrackingDataserver : public VQwDataserver {

  private:

  public:
    /// \brief Default constructor
    QwTrackingDataserver (const char* name = 0);
    /// \brief Destructor
    virtual ~QwTrackingDataserver();

    /// \brief Get a new run from the IO stream
    Int_t GetRun(Int_t run);
    /// \brief Get a new event from the IO stream
    Int_t GetEvent();

    /// Pass the CODA data to the specified subsystem array (not thread-safe!)
    void FillSubsystemData(QwSubsystemArray* detectors);

  ClassDef(QwTrackingDataserver,1);

}; // class QwTrackingDataserver

#endif // __QwTrackingDataserver_h__
