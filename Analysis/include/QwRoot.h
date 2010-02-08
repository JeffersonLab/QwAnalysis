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

class QwRoot : public VQwSystem {

  private:
    TThread* fRootThread;	// thread with main analysis loop
    TThread* fAnalyzerThreads;	// thread with analyzers objects

    VQwAnalyzer* fAnalyzers;	// analyzers
    VQwDataserver* fDataserver;	// dataserver

    bool fIsOnline;	// Are we running online (CODA stream) or offline (ASCII)
    bool fIsFinished;	// Are we at the end of the data stream yet

  public:
    QwRoot (const char* name = 0, bool = false): VQwSystem(name) { };
    virtual ~QwRoot();

    void Run();
    void Start();
    void OnlineLoop();
    void OfflineLoop();

    // Set the analyzer
    void SetAnalyzer (VQwAnalyzer* analyzer);

    // Set the dataserver
    void SetDataserver (VQwDataserver* dataserver);


    void SetIsOnline (bool online) { fIsOnline = online; };
    bool IsOnline () { return fIsOnline; };
    void SetIsFinished (bool finished) { fIsFinished = finished; };
    bool IsFinished () { return fIsFinished; };

  ClassDef(QwRoot,1);
};

#endif // __QwRoot_h__
