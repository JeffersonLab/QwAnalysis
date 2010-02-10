#ifndef __VQwAnalyzer_h__
#define __VQwAnalyzer_h__

// ROOT headers
#include <TFile.h>
#include <TTree.h>

// Qweak headers
#include "QwLog.h"
#include "VQwSystem.h"

// Forward declarations
class QwSubsystemArray;

/**
 *  \class VQwAnalyzer
 *  \ingroup QwThreadSafe
 *  \brief A framework for a thread-safe analyzer object
 *
 * The requirements on this analyzer objects are the following:
 * - no explicit output to files or other systems should be generated
 * - at this point the QwLog class is not thread-safe, but it presents the
 *   easiest way forward to getting threads safe output (and at least it would
 *   allow to easily distinguish which thread is writing output).  Subsystems
 *   should therefore avoid direct calls the the thread-unsafe std streams
 * - it needs to implement all its functions in an explicit thread-safe way,
 *   meaning that for all 'global' operations, it requests a thread lock by
 *   calling the static function TThread::Lock() and releases the lock by
 *   calling TThread::UnLock().  The lock should only be kept as short as
 *   possible.  (At this point we don't foresee the need for TMutex objects.)
 */
class VQwAnalyzer : public VQwSystem {

  private:
    /// Private assignment operator, to prevent it from being used.
    VQwAnalyzer& operator= (const VQwAnalyzer &value) {
      return *this;
    };

  protected:
    /// Subsystem array with detectors
    QwSubsystemArray* fDetectors;	//!

    /// ROOT file (static so only one ROOT file is created!)
    static TFile* fRootFile;
    /// ROOT tree (static so only one ROOT tree is created!)
    static TTree* fRootTree;
    // TODO These should probably be boost::shared_ptr...

    /// \name Analysis flags for thread-locking operations
    // @{
    Bool_t kHisto;	///< Write output to histograms
    Bool_t kTree;	///< Write output to trees
    // @}

  public:
    /// Default constructor
    VQwAnalyzer (const char* name = 0): VQwSystem(name) { };
    /// Destructor
    virtual ~VQwAnalyzer() { };

    /// Get a pointer to the array of detectors (ensure exclusive access!)
    virtual QwSubsystemArray* GetSubsystemArray() { return fDetectors; };

    /// \brief Open and initialize the ROOT file
    virtual void OpenRootFile(Int_t run);
    /// \brief Close and clean-up the ROOT file
    virtual void CloseRootFile();

    /// Process the event data stored in the subsystem array fDetectors
    virtual void ProcessEvent() {
      QwError << "VQwAnalyzer::ProcessEvent not implemented!" << QwLog::endl;
      return;
    };

  ClassDef(VQwAnalyzer,1);

}; // class VQwAnalyzer

#endif // __VQwAnalyzer_h__
