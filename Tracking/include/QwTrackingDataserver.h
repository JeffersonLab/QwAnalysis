#ifndef __QwTrackingDataserver_h__
#define __QwTrackingDataserver_h__

// Inherits from
#include "VQwDataserver.h"

// ROOT headers
#include "TROOT.h"
#include "TH1F.h"

// Qweak support headers
#include "Det.h"
#include "Qset.h"
#include "Qoptions.h"

// Qweak tracking worker
#include "QwTrackingWorker.h"

class QwTrackingDataserver : public VQwDataserver {

  private:
    QwASCIIEventBuffer fASCIIbuffer;
    QwHitContainer* fASCIIgrandHitList;

  public:
    QwTrackingDataserver (const char*);
    ~QwTrackingDataserver();

    void NextEvent();
    QwHitContainer* GetHitList();

  //ClassDef(QwTrackingDataserver,1)
};

#endif // __QwTrackingDataserver_h__
