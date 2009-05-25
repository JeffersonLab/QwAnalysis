#ifndef __QwRoot_h__
#define __QwRoot_h__

#include "TThread.h"

#include "VQwSystem.h"
#include "VQwAnalyzer.h"
#include "VQwDataserver.h"

void* QwRunThread (void*);

class QwRoot : public VQwSystem {

  private:
    TThread* fRunThread;	// thread with analyzer

    VQwAnalyzer* fAnalyzer;	// analyzer
    VQwDataserver* fDataserver;	// dataserver

    bool fIsOnline;	// Are we running online (CODA stream) or offline (ASCII)
    bool fIsFinished;	// Are we at the end of the data stream yet

  public:
    QwRoot (const char* name, bool = false): VQwSystem(name) { };
    ~QwRoot() { };

    void Run();
    void Start();
    void OnlineLoop();
    void OfflineLoop();

    // analyzer creation and starting
    VQwAnalyzer* CreateAnalyzer (const char* name);
    void SetAnalyzer (VQwAnalyzer* analyzer);

    // dataserver creation and starting
    VQwDataserver* CreateDataserver (const char* name);
    void SetDataserver (VQwDataserver* dataserver);


    void SetIsOnline (bool online) { fIsOnline = online; };
    bool IsOnline () { return fIsOnline; };
    void SetIsFinished (bool finished) { fIsFinished = finished; };
    bool IsFinished () { return fIsFinished; };

  ClassDef(QwRoot,1)
};

#endif // __QwRoot_h__
