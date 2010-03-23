#ifndef __QwControl_h__
#define __QwControl_h__

// ROOT headers
#include <TThread.h>

// Qweak headers
#include "VQwSystem.h"

// Qweak analyzer and dataserver
#include "VQwAnalyzer.h"
#include "VQwDataserver.h"

// Global pointers recognised by CINT
R__EXTERN class QwControl* gQwControl;

#define NUM_ANALYZERS 4

// Thread-spinning functions
void* QwControlThread (void*);
void* QwAnalyzerThread (void*);

/**
 *  \class QwControl
 *  \ingroup QwThreadSafe
 *  \brief The framework of a generic analysis
 *
 * The QwControl class implements the framework of a generic Qweak analysis.
 */
class QwControl : public VQwSystem {

  private:

    TThread* fControlThread;	///< Control thread
    TThread* fAnalyzerThreads;	///< Threads with analyzers objects

    VQwAnalyzer* fAnalyzers[9];	///< Analyzers
    VQwDataserver* fDataserver;	///< Dataserver

    bool fIsBatch;	///< Are we running in batch mode, or interactive mode
    bool fIsOnline;	///< Are we running online (CODA stream) or offline (ascii)
    bool fIsFinished;	///< Are we at the end of the data stream yet

  public:
    /// \brief Default constructor
    QwControl(const char* name = 0, int* argc = 0, char** argv = 0);
    /// \brief Destructor
    virtual ~QwControl();

    /// Spin off the control in a different thread
    void StartControl();

    /// Run the generic analysis
    void Run();

    /// \brief Online event loop
    void OnlineLoop();
    /// \brief Offline event loop
    void OfflineLoop();

    /// Set the analyzer
    virtual void SetAnalyzer (VQwAnalyzer* analyzer);
    /// Set the dataserver
    virtual void SetDataserver (VQwDataserver* dataserver);

    /// \brief Start the analyzer
    virtual void StartAnalyzer(VQwAnalyzer* analyzer);
    /// \brief Start the dataserver
    virtual void StartDataserver(VQwDataserver* dataserver);


    void SetIsOnline (bool online) { fIsOnline = online; };
    bool IsOnline () { return fIsOnline; };
    void SetIsFinished (bool finished) { fIsFinished = finished; };
    bool IsFinished () { return fIsFinished; };

  ClassDef(QwControl,1);
};

#endif // __QwControl_h__
