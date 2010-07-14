#ifndef __VQWSUBSYSTEMTRACKING__
#define __VQWSUBSYSTEMTRACKING__


#include "VQwSubsystem.h"

///
/// \ingroup QwTracking

// Forward declarations
class QwHitContainer;
class QwDetectorInfo;

class VQwSubsystemTracking: virtual public VQwSubsystem {
  /******************************************************************
   *  Class: VQwSubsystemTracking
   *         Virtual base class for the classes containing the
   *         event-based information from each tracking subsystem.
   *         This will define the interfaces used in communicating
   *         with the CODA routines.
   *
   ******************************************************************/
  public:

    /// Constructor with name
    VQwSubsystemTracking(TString name): VQwSubsystem(name) {
      SetEventTypeMask(0xfffe); // do not accept 0x1
    };
    /// Default destructor
    virtual ~VQwSubsystemTracking() { };


    /// Load geometry definition for tracking subsystems (required)
    virtual Int_t LoadGeometryDefinition(TString mapfile) = 0;

    /// Get the detector geometry information
    virtual Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detect_info)=0;


    /// Get the hit list
    virtual void  GetHitList(QwHitContainer & grandHitContainer) = 0;

private:

    /// Private default constructor
    VQwSubsystemTracking() { };

}; // class VQwSubsystemTracking

#endif // __VQWSUBSYSTEMTRACKING__
