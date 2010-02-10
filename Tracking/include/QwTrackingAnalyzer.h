#ifndef __QwTrackingAnalyzer_h__
#define __QwTrackingAnalyzer_h__

// Inherits from
#include "VQwAnalyzer.h"

// Forward declarations
class QwSubsystemArrayTracking;
class QwTrackingWorker;
class QwHitContainer;

/**
 *  \class QwTrackingAnalyzer
 *  \ingroup QwThreadSafe
 *  \brief Self-contained, thread-safe tracking analyzer object
 */
class QwTrackingAnalyzer : public VQwAnalyzer {

  private:
    /// Decode hit list
    QwHitContainer* fHitList;			//!
    /// Tracking worker
    QwTrackingWorker* fTrackingWorker;		//!

    // Flags
    Bool_t kTracking;

  public:
    /// \brief Default constructor
    QwTrackingAnalyzer (const char* name = 0);
    /// \brief Destructor
    virtual ~QwTrackingAnalyzer();

    /// \brief Get the subsystem array cast for tracking
    QwSubsystemArrayTracking* GetSubsystemArrayTracking();

    void ProcessEvent();

  ClassDef(QwTrackingAnalyzer,1);

}; // class QwTrackingAnalyzer

#endif // __QwTrackingAnalyzer_h__
