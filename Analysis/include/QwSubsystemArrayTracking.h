
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


class QwSubsystemArrayTracking:  public QwSubsystemArray {
 
 public:
  QwSubsystemArrayTracking() {};
  ~QwSubsystemArrayTracking(){
  };
  
 

  void GetHitList(QwHitContainer & grandHitList);
 

};
  

void QwSubsystemArrayTracking::GetHitList(QwHitContainer & grandHitList){
  if (!empty()){
    grandHitList.clear();
    
    for (iterator subsys = begin(); subsys != end(); ++subsys){
      
      ((VQwSubsystemTracking*)(subsys)->get())->GetHitList(grandHitList);
    }
  }
};


#endif
