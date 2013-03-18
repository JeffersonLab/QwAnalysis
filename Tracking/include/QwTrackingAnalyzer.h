#ifndef __QwTrackingAnalyzer_h__
#define __QwTrackingAnalyzer_h__

// Inherits from
#include "VQwAnalyzer.h"

// ROOT headers
#include "TROOT.h"
#include "TH1F.h"

// Qweak tracking worker
#include "QwTrackingWorker.h"

class QwTrackingAnalyzer : public VQwAnalyzer {

  private:
    QwHitContainer* fHitList;
    QwTrackingWorker* fTrackingWorker;

  public:
    TH1F* fHisto1;
    TH1F* fHisto2;

    QwTrackingAnalyzer (const char*);
    virtual ~QwTrackingAnalyzer();

    void SetHitList(QwHitContainer* hitlist) { fHitList = hitlist; };

    void Process();

};

#endif // __QwTrackingAnalyzer_h__
