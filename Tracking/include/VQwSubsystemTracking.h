#ifndef __VQWSUBSYSTEMTRACKING__
#define __VQWSUBSYSTEMTRACKING__


#include "VQwSubsystem.h"
#include "QwHitContainer.h"

class VQwSubsystemTracking: public VQwSubsystem{
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
 
 private:
  VQwSubsystemTracking(){};  //  Private constructor.


};


#endif

