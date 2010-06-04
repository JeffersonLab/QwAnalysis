/**********************************************************\
* File: QwSubsystemArrayTracking.h                         *
*                                                          *
* Author: P. M. King, Rakitha Beminiwattha                 *
* Time-stamp: <2008-07-22 15:50>                           *
\**********************************************************/

#ifndef __QWSUBSYSTEMARRAYTRACKING__
#define __QWSUBSYSTEMARRAYTRACKING__


#include "QwSubsystemArray.h"
#include "QwHitContainer.h"
#include "QwDriftChamber.h"
#include "VQwSubsystemTracking.h"


///
/// \ingroup QwTracking
class QwSubsystemArrayTracking:  public QwSubsystemArray {

  public:

    QwSubsystemArrayTracking(): QwSubsystemArray() { };
    QwSubsystemArrayTracking(const char* filename): QwSubsystemArray(filename) { };
    virtual ~QwSubsystemArrayTracking() { };

    // Update the wire-based hit list from each subsystem
    void GetHitList(QwHitContainer& hitlist);
    void GetHitList(QwHitContainer* hitlist);

    // Update the TDC-based hit list from each subsystem
    void GetTDCHitList(QwHitContainer& tdchitlist);
    void GetTDCHitList(QwHitContainer* tdchitlist);

    VQwSubsystemTracking* GetSubsystem(const TString name);
};


VQwSubsystemTracking* QwSubsystemArrayTracking::GetSubsystem(const TString name)
{
  VQwSubsystemTracking* tmp = NULL;
  tmp = dynamic_cast<VQwSubsystemTracking*>(QwSubsystemArray::GetSubsystem(name));

  return tmp;
};

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
};

void QwSubsystemArrayTracking::GetTDCHitList(QwHitContainer* tdchitlist)
{
  GetTDCHitList(*tdchitlist);
};

void QwSubsystemArrayTracking::GetTDCHitList(QwHitContainer& tdchitlist)
{
  if (!empty()) {
    tdchitlist.clear();

    for (iterator subsys = begin(); subsys != end(); ++subsys) {
      VQwSubsystemTracking* subsys_tracking =
        dynamic_cast<VQwSubsystemTracking*>((subsys)->get());
      QwDriftChamber* subsys_driftchamber =
        dynamic_cast<QwDriftChamber*>((subsys)->get());
      if (subsys_tracking != NULL) {
        if (subsys_driftchamber != NULL)
          subsys_driftchamber->GetTDCHitList(tdchitlist);
        else
          subsys_tracking->GetHitList(tdchitlist);
      } else {
        std::cerr << "QwSubsystemArrayTracking::GetTDCHitList: Subsystem \""
                  << ((subsys)->get())->GetSubsystemName()
                  << "\" isn't a tracking subsystem." << std::endl;
      }
    }
  }
};

void QwSubsystemArrayTracking::GetHitList(QwHitContainer* hitlist)
{
  GetHitList(* hitlist);
};

#endif
