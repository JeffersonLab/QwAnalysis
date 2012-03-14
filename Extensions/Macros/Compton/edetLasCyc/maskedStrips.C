#ifndef __MASKEDSTRIPS_F
#define __MASKEDSTRIPS_F


#include "comptonRunConstants.h"

Bool_t maskedStrips(Int_t plane,Int_t strip)
{///the following is entered in C++ counting, which starts from 0.
  if(plane==0&&(strip==1||strip==5||strip==19)) return kTRUE;//skip masked strip of plane1
  else if(plane==2&&(strip==38||strip==52||strip==63)) return kTRUE;//skip masked strip of plane3
  else if(plane==1&&(strip==11||strip==19)) return kTRUE;//skip masked strip of plane2
  else return kFALSE;
}
#endif
