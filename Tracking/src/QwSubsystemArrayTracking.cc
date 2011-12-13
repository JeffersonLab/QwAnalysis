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
 * Get the detector info for all subsystems
 * @return Detector info container
 */
const QwGeometry QwSubsystemArrayTracking::GetGeometry()
{
  QwGeometry geometry;
  if (!empty()) {
    for (iterator subsys = begin(); subsys != end(); ++subsys) {
      VQwSubsystemTracking* subsys_tracking =
        dynamic_cast<VQwSubsystemTracking*>(subsys->get());
      if (subsys_tracking != 0) {
        geometry.push_back(subsys_tracking->GetDetectorInfo());
      }
    }
  }
  return geometry;
}


/**
 * Get the hit list from the subsystems in this array
 * @param hitlist Hit container to store the hits in
 */
void QwSubsystemArrayTracking::GetHitList(QwHitContainer& hitlist)
{
  if (!empty()) {
    hitlist.clear();
    for (iterator subsys = begin(); subsys != end(); ++subsys) {
      VQwSubsystemTracking* subsys_tracking =
        dynamic_cast<VQwSubsystemTracking*>(subsys->get());
      if (subsys_tracking != 0) {
        subsys_tracking->GetHitList(hitlist);
      } else {
        std::cerr << "QwSubsystemArrayTracking::GetHitList: Subsystem \""
                  << ((subsys)->get())->GetSubsystemName()
                  << "\" isn't a tracking subsystem." << std::endl;
      }
    }
  }
}



void  QwSubsystemArrayTracking::FillHardwareErrorSummary()
{

  // Just try to use a default copy constructor... first
  // 
  //  for (const_iterator subsys = dummy_source->begin(); subsys != dummy_source->end(); ++subsys) {
  for (iterator subsys = begin(); subsys != end(); ++subsys) {
    VQwSubsystemTracking* subsys_tracking = dynamic_cast<VQwSubsystemTracking*>(subsys->get());
    subsys_tracking->FillHardwareErrorSummary();
  }
  return;
}
