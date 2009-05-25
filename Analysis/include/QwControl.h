#ifndef __QwControl_h__
#define __QwControl_h__

// Inherits from
#include "TRint.h"

// Global pointers recognised by CINT
R__EXTERN class QwControl* gQwControl;
R__EXTERN class QwRoot*    gQwRoot;


class QwControl : public TRint {

  protected:

    static QwControl* fExists;	// Check whether interface already existing

    bool fIsBatch;		// Are we running in batch mode, or interactive mode
    bool fIsOnline;		// Are we running online (CODA stream) or offline (ascii)
				// (duplication of fIsOnline with QwRoot.h)

  public:
    QwControl (const char* appClassName, int* argc, char** argv,
		void* options = NULL, int numOptions = -1, bool noLogo = 0);
    virtual ~QwControl();
    virtual void StartAnalyzer();
    virtual void StartDataserver();


  ClassDef(QwControl,1)

};

#endif
