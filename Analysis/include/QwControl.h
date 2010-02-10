#ifndef __QwControl_h__
#define __QwControl_h__

// Inherits from
#include <TRint.h>

// Forward declarations
class VQwDataserver;
class VQwAnalyzer;

// Global pointers recognised by CINT
R__EXTERN class QwControl* gQwControl;
R__EXTERN class QwRoot*    gQwRoot;


/**
 *  \class QwControl
 *  \ingroup QwThreadSafe
 *  \brief A ROOT command line interface
 *
 * The QwControl class implements a ROOT command line interface.  It inherits
 * from the ROOT interactive C++ interpreter TRint.  All functionality available
 * in ROOT is available in this interface.  In particular, all histograms that
 * are defined in memory or in a ROOT file are accessible under the path
 * "/root/ROOT memory", while they are being filled.
 *
 */
class QwControl : public TRint {

  protected:

    static QwControl* fExists;	///< Check whether interface already existing

    bool fIsBatch;		///< Are we running in batch mode, or interactive mode
    bool fIsOnline;		///< Are we running online (CODA stream) or offline (ascii)

  public:
    /// \brief Constructor
    QwControl (const char* appClassName, int* argc, char** argv,
		void* options = NULL, int numOptions = -1, bool noLogo = 0);
    /// \brief Destructor
    virtual ~QwControl();

    /// \brief Start the analyzer
    virtual void StartAnalyzer(VQwAnalyzer* analyzer);
    /// \brief Start the dataserver
    virtual void StartDataserver(VQwDataserver* dataserver);

}; // class QwControl

#endif // __QwControl_h__
