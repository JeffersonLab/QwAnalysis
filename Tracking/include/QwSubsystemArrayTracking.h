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
#include "VQwSubsystemTracking.h"


///
/// \ingroup QwTrackingAnl
class QwSubsystemArrayTracking:  public QwSubsystemArray {

 public:
  QwSubsystemArrayTracking() {};
  ~QwSubsystemArrayTracking(){};

  // This will update the hit list from each subsystem
  void GetHitList(QwHitContainer & grandHitList);
  void GetHitList(QwHitContainer * grandHitList);

  VQwSubsystemTracking* GetSubsystem(const TString name);

};


VQwSubsystemTracking* QwSubsystemArrayTracking::GetSubsystem(const TString name)
{
  VQwSubsystemTracking* tmp = NULL;
  tmp = dynamic_cast<VQwSubsystemTracking*>  (QwSubsystemArray::GetSubsystem(name));

  return tmp;
};

void QwSubsystemArrayTracking::GetHitList(QwHitContainer & grandHitList)
{
  if (!empty()){
    grandHitList.clear();

    for (iterator subsys = begin(); subsys != end(); ++subsys){
      VQwSubsystemTracking* tsubsys =
	dynamic_cast<VQwSubsystemTracking*>((subsys)->get());
      if (tsubsys != NULL){
	tsubsys->GetHitList(grandHitList);
      } else {
	std::cerr << "QwSubsystemArrayTracking::GetHitList: Subsystem \""
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
