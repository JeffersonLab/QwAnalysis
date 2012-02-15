/**********************************************************\
* File: QwSubsystemArrayTracking.h                         *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-22 15:50>                           *
\**********************************************************/

#ifndef __QWSUBSYSTEMARRAYTRACKING__
#define __QWSUBSYSTEMARRAYTRACKING__

// Qweak headers
#include "QwSubsystemArray.h"
#include "QwHitContainer.h"
#include "VQwSubsystemTracking.h"


///
/// \ingroup QwTracking
class QwSubsystemArrayTracking:  public QwSubsystemArray {

  private:

    /// Private default constructor
    QwSubsystemArrayTracking(); // not implemented, will throw linker error on use

  public:

    /// Constructor with options
    QwSubsystemArrayTracking(QwOptions& options): QwSubsystemArray(options, CanContain) { };
    /// Default destructor
    virtual ~QwSubsystemArrayTracking() { };

    /// \brief Get the subsystem with the specified name
    VQwSubsystemTracking* GetSubsystemByName(const TString& name);

    /// \brief Get the detector info for all detectors
    const QwGeometry GetGeometry();

    // Update the wire-based hit list from each subsystem
    void GetHitList(QwHitContainer& hitlist);
    void GetHitList(QwHitContainer* hitlist) { GetHitList(*hitlist); };

    /// 
    void FillHardwareErrorSummary();

  protected:

    /// Test whether this subsystem array can contain a particular subsystem
    static Bool_t CanContain(VQwSubsystem* subsys) {
      return (dynamic_cast<VQwSubsystemTracking*>(subsys) != 0);
    };

}; // class QwSubsystemArrayTracking

#endif // __QWSUBSYSTEMARRAYTRACKING__
