/*!
 *
 * \file QwSubsystemArrayTracking.cc
 *
 * \brief Implementation of the tracking subsystem array
 */

#include "QwSubsystemArrayTracking.h"

// Qweak headers
#include "QwDriftChamber.h"

/**
 * Get the subsystem with the specified name
 * @param name Name of the subsystem
 * @return Subsystem with that name
 */
VQwSubsystemTracking* QwSubsystemArrayTracking::GetSubsystemByName(const TString& name)
{
  return dynamic_cast<VQwSubsystemTracking*>(QwSubsystemArray::GetSubsystemByName(name));
}

/**
 * Get the hit list from the subsystems in this array
 * @param hitlist Hit container to store the hits in
 */
void QwSubsystemArrayTracking::GetHitList(QwHitContainer& hitlist)
{
  if (!empty()){
    hitlist.clear();

    for (iterator subsys = begin(); subsys != end(); ++subsys){
      VQwSubsystemTracking* tsubsys =
        dynamic_cast<VQwSubsystemTracking*>((subsys)->get());
      if (tsubsys != NULL){
        tsubsys->GetHitList(hitlist);
      } else {
        std::cerr << "QwSubsystemArrayTracking::GetHitList: Subsystem \""
                  << ((subsys)->get())->GetSubsystemName()
                  << "\" isn't a tracking subsystem." << std::endl;
      }
    }
  }
}
