#ifndef __MASKEDSTRIPS_F
#define __MASKEDSTRIPS_F


#include "comptonRunConstants.h"

Bool_t maskedStrips(Int_t plane,Int_t strip)
{
  if(plane==0&&(strip==1||strip==5||strip==19)) return kTRUE;//skip masked strip of plane1
  else if(plane==2&&(strip==23||strip==38||strip==52||strip==63)) return kTRUE;//skip masked strip of plane3
  else if(plane==1&&(strip==11||strip==19)) return kTRUE;//skip masked strip of plane2
  else return kFALSE;
}

Int_t identifyCedgeforPlane(Int_t p) ///p:plane # in C++ counting
{
  //  for (Int_t p =startPlane; p <endPlane; p++) {
  // Cedge = Form("Cedge_p%d",p+1);
  // cout<<"Cedge is "<<Cedge<<endl;  
  //  }
  if (p==0) Cedge = Cedge_p1;
  else if (p==1) Cedge = Cedge_p2;
  else if (p==2) Cedge = Cedge_p3;
  return Cedge;
}

#endif

/************* Comments *******
 *this code is entered in C++ counting, which starts from 0.
 *plane 4 is altogether ignored in this function for now
 ******************************/

///run # 24519 onwards, I need to add strip 23 to the plane-3 mask
