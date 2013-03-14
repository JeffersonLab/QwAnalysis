#ifndef __QwTrackingDataserver_h__
#define __QwTrackingDataserver_h__

// Inherits from
#include "VQwDataserver.h"

// ROOT headers
#include "TROOT.h"
#include "TH1F.h"

// Qweak headers
#include "QwEventBuffer.h"
#include "QwTrackingWorker.h"

class QwTrackingDataserver : public VQwDataserver {

  private:
    QwEventBuffer fEventBuffer;
    QwHitContainer* fHitList;

  public:
    QwTrackingDataserver (const char*);
    virtual ~QwTrackingDataserver();

    void NextEvent();
    QwHitContainer* GetHitList();

};

#endif // __QwTrackingDataserver_h__
