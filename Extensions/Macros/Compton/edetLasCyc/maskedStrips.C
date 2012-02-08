#include "comptonRunConstants.h"

Bool_t maskedStrips(Int_t plane,Int_t strip)
{
  if(plane==1&&(strip==2||strip==6||strip==20)) return kTRUE;//skip masked strip of plane1
  else if(plane==3&&(strip==39||strip==53||strip==64)) return kTRUE;//skip masked strip of plane3
  else if(plane==2&&(strip==12)) return kTRUE;//skip masked strip of plane2
  else return kFALSE;
}
