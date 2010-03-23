#ifndef __QwRoot_h__
#define __QwRoot_h__

// Qweak headers
#include <TRint.h>
#include <TThread.h>

// Global pointers recognised by CINT
R__EXTERN class QwRoot* gQwRoot;


/**
 *  \class QwRoot
 *  \ingroup QwThreadSafe
 *  \brief A ROOT command line interface
 *
 * The QwRoot class implements a ROOT command line interface.  It inherits
 * from the ROOT interactive C++ interpreter TRint.  All functionality available
 * in ROOT is available in this interface.  In particular, all histograms that
 * are defined in memory or in a ROOT file are accessible under the path
 * "/root/ROOT memory", while they are being filled.
 *
 */
class QwRoot : public TRint {

  protected:

    static QwRoot* fExists;	///< Check whether interface already existing

  public:
    /// \brief Constructor
    QwRoot (const char* appClassName, int* argc, char** argv,
		void* options = NULL, int numOptions = -1, bool noLogo = false);

    /// \brief Destructor
    virtual ~QwRoot();

}; // class QwRoot

#endif // __QwRoot_h__
