#ifndef __QwRoot_h__
#define __QwRoot_h__

// ROOT headers
#include <TThread.h>

// Qweak headers
#include "VQwSystem.h"

// Qweak analyzer and dataserver
#include "VQwAnalyzer.h"
#include "VQwDataserver.h"

#define NUM_ANALYZERS 4

void* QwRootThread (void*);
void* QwAnalyzerThread (void*);

/**
 *  \class QwRoot
 *  \ingroup QwThreadSafe
 *  \brief The framework of a generic analysis
 *
 * The QwRoot class implements the framework of a generic Qweak analysis.
 */
class QwRoot : public VQwSystem {

  private:
    TThread* fRootThread;	///< thread with main analysis loop
    TThread* fAnalyzerThreads;	///< thread with analyzers objects

    VQwAnalyzer* fAnalyzers;	///< analyzers
    VQwDataserver* fDataserver;	///< dataserver

    bool fIsOnline;	// Are we running online (CODA stream) or offline (ASCII)
    bool fIsFinished;	// Are we at the end of the data stream yet

  public:
    /// \brief Default constructor
    QwRoot (const char* name = 0);
    /// \brief Destructor
    virtual ~QwRoot();

    /// Run the generic analysis
    void Run();
    /// Start a thread with the analysis
    void Start();

    /// \brief Online event loop
    void OnlineLoop();
    /// \brief Offline event loop
    void OfflineLoop();

    /// Set the analyzer
    void SetAnalyzer (VQwAnalyzer* analyzer);

    /// Set the dataserver
    void SetDataserver (VQwDataserver* dataserver);


    void SetIsOnline (bool online) { fIsOnline = online; };
    bool IsOnline () { return fIsOnline; };
    void SetIsFinished (bool finished) { fIsFinished = finished; };
    bool IsFinished () { return fIsFinished; };

  ClassDef(QwRoot,1);
};

#endif // __QwRoot_h__
