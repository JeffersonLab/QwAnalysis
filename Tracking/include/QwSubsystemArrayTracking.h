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

  public:

    /// Default constructor
    QwSubsystemArrayTracking(): QwSubsystemArray(CanContain) { };
    /// Constructor with options
    QwSubsystemArrayTracking(QwOptions& options): QwSubsystemArray(options, CanContain) { };
    /// Constructor with map file
    QwSubsystemArrayTracking(const char* filename): QwSubsystemArray(filename, CanContain) { };
    /// Default destructor
    virtual ~QwSubsystemArrayTracking() { };

    /// \brief Get the subsystem with the specified name
    VQwSubsystemTracking* GetSubsystemByName(const TString& name);

    // Update the wire-based hit list from each subsystem
    void GetHitList(QwHitContainer& hitlist);
    void GetHitList(QwHitContainer* hitlist) { GetHitList(*hitlist); };

    // Update the TDC-based hit list from each subsystem
    void GetTDCHitList(QwHitContainer& tdchitlist);
    void GetTDCHitList(QwHitContainer* tdchitlist) { GetTDCHitList(*tdchitlist); };

  protected:

    /// Test whether this subsystem array can contain a particular subsystem
    static Bool_t CanContain(VQwSubsystem* subsys) {
      return (dynamic_cast<VQwSubsystemTracking*>(subsys) != 0);
    };

}; // class QwSubsystemArrayTracking

#endif // __QWSUBSYSTEMARRAYTRACKING__
