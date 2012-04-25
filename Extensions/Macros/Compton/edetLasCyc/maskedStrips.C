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


Int_t identifyCedgeforPlane(Int_t p, Float_t activeStrip[nPlanes][nStrips], Float_t stripAsymEr[nPlanes][nStrips])
{
  Bool_t debug=0;
  for (Int_t s =startStrip; s <endStrip; s++) {
    //if (maskedStrips(p,s)) continue; //!careful, this is not a full-proof method
    //if (maskedStrips(p,s+1)) continue;
    if (stripAsymEr[p][s+1] >= 3.0*stripAsymEr[p][s]) { //!the factor of 3.0 is arbitrarily put now
      cout<<"Compton edge for plane "<<p+1<<" (auto)identified as strip #"<<s+1<<endl;
      return (Int_t)activeStrip[p][s];//!notice that I return the Cedge strip number in human counting
    }
    else if (debug) printf("Cedge not found:compared %f and twice of %f for strip:%d\n",stripAsymEr[p][s+1],stripAsymEr[p][s],s);
  }
  cout<<"***Alert Compton edge for plane "<<p+1<<" could not be (auto) found***"<<endl;
  if (p==0) Cedge[p] = Cedge_p1;
  else if (p==1) Cedge[p] = Cedge_p2;
  else if (p==2) Cedge[p] = Cedge_p3;
  return Cedge[p];
}
#endif

/************* Comments *******
 *this code is entered in C++ counting, which starts from 0.
 *plane 4 is altogether ignored in this function for now
 ******************************/

///run # 24519 onwards, I need to add strip 23 to the plane-3 mask
