#ifndef __QwTrackingAnalyzer_h__
#define __QwTrackingAnalyzer_h__

// Inherits from
#include "VQwAnalyzer.h"

// ROOT headers
#include "TROOT.h"
#include "TH1F.h"

// Qweak support headers
#include "Det.h"
#include "Qset.h"

// Qweak tracking worker
#include "QwTrackingWorker.h"

class QwTrackingAnalyzer : public VQwAnalyzer {

  private:
    Qset fGeometry;

    QwHitContainer* fHitList;
    QwTrackingWorker* fTrackingWorker;

  public:
    TH1F* fHisto1;
    TH1F* fHisto2;

    QwTrackingAnalyzer (const char*);
    ~QwTrackingAnalyzer();

    void SetHitList(QwHitContainer* hitlist) { fHitList = hitlist; };

    void Process();

  //ClassDef(QwTrackingAnalyzer,1)
};

#endif // __QwTrackingAnalyzer_h__
