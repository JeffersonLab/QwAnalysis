#ifndef __VQWSUBSYSTEMTRACKING__
#define __VQWSUBSYSTEMTRACKING__


#include "VQwSubsystem.h"

///
/// \ingroup QwTrackingAnl

// Forward declarations
class QwHitContainer;
class QwDetectorInfo;

class VQwSubsystemTracking: public VQwSubsystem {
  /******************************************************************
   *  Class: VQwSubsystemTracking
   *         Virtual base class for the classes containing the
   *         event-based information from each tracking subsystem.
   *         This will define the interfaces used in communicating
   *         with the CODA routines.
   *
   ******************************************************************/
 public:
  VQwSubsystemTracking(TString region_tmp):VQwSubsystem(region_tmp){ };

  virtual ~VQwSubsystemTracking(){};

  virtual void  GetHitList(QwHitContainer & grandHitContainer)=0;
  virtual Int_t LoadQweakGeometry(TString mapfile)=0;//will load the detector geometry file. This method is called individually from each subsystem
  virtual Int_t GetDetectorInfo(std::vector< std::vector< QwDetectorInfo > > & detect_info)=0;

 private:
  VQwSubsystemTracking(){};  //  Private constructor.


};


#endif

