#ifndef __QwTrackingAnalyzer_h__
#define __QwTrackingAnalyzer_h__

// Inherits from
#include "VQwAnalyzer.h"

// ROOT headers
#include <TROOT.h>

// Forward declarations
class QwSubsystemArrayTracking;
class QwTrackingWorker;
class QwHitContainer;

class QwTrackingAnalyzer : public VQwAnalyzer {

  private:
    QwHitContainer* fHitList;			//!
    QwTrackingWorker* fTrackingWorker;		//!

    // Flags
    Bool_t kTracking;

  public:
    QwTrackingAnalyzer (const char* name = 0);
    virtual ~QwTrackingAnalyzer();

    QwSubsystemArrayTracking* GetSubsystemArrayTracking();

    void ProcessEvent();

  ClassDef(QwTrackingAnalyzer,1);
};

#endif // __QwTrackingAnalyzer_h__
