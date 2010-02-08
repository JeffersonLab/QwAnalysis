#ifndef __QwTrackingDataserver_h__
#define __QwTrackingDataserver_h__

// Inherits from
#include "VQwDataserver.h"

// ROOT headers
#include <TROOT.h>

// Forward declarations
class QwEventBuffer;

class QwTrackingDataserver : public VQwDataserver {

  private:

  public:
    QwTrackingDataserver (const char* name = 0);
    virtual ~QwTrackingDataserver();

    Int_t GetRun(Int_t run);

    Int_t GetEvent();
    Int_t GetEventNumber();

    void FillSubsystemData(QwSubsystemArray* detectors);

  ClassDef(QwTrackingDataserver,1);
};

#endif // __QwTrackingDataserver_h__
